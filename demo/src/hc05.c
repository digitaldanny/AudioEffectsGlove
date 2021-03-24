/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#include "hc05.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 *
*/

volatile uint16_t rDataA[SCI_MAX_BUFFER_LENGTH]; // Received data buffer for SCI-C

// Used for checking the received data
volatile uint16_t rDataPointA;
volatile uint16_t rxLenSinceTx; // how many bytes have been received since last TX?

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * HC05 API IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void initHc05()
{
    initSCIC();
}

bool writeHc05(uint16_t* msg, uint16_t len)
{
    // Flush RX buffer for next read
    SCI_resetRxFIFO(SCIC_BASE);

    // Reset RX buffer and length for each transfer
    memset((void*)rDataA, 0, rxLenSinceTx);
    rxLenSinceTx = 0;

    // Write all data in msg buffer
    SCI_writeCharArray(SCIC_BASE, (const uint16_t *)msg, len);

    // Issue PIE ACK
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
    return true;
}

bool readHc05(uint16_t** rx, uint16_t len)
{
    // Wait for expected number of bytes to be received
    while (rxLenSinceTx < len);

    *rx = (uint16_t*)rDataA;
    return true;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * SCI IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void initSCIC(void)
{
    //
    // GPIO28 is the SCI Rx pin.
    //
    GPIO_setMasterCore(139, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_139_SCIRXDC);
    GPIO_setDirectionMode(139, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(139, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(139, GPIO_QUAL_ASYNC);

    //
    // GPIO29 is the SCI Tx pin.
    //
    GPIO_setMasterCore(56, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_56_SCITXDC);
    GPIO_setDirectionMode(56, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(56, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(56, GPIO_QUAL_ASYNC);

    //
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    Interrupt_register(INT_SCIC_RX, sciCRXFIFOISR);

    //
    // Initialize the Device Peripherals:
    //
    initSCICFIFO();

    Interrupt_enable(INT_SCIC_RX);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8); // is this the correct interrupt group?
}

void initSCICFIFO(void)
{
    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 38400.
    //
    SCI_setConfig(SCIC_BASE, DEVICE_LSPCLK_FREQ, 38400, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIC_BASE);
    SCI_enableLoopback(SCIC_BASE);
    SCI_resetChannels(SCIC_BASE);
    SCI_enableFIFO(SCIC_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIC_BASE, SCI_INT_RXFF); // (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_disableInterrupt(SCIC_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIC_BASE, SCI_FIFO_TX2, SCI_FIFO_RX2);
    SCI_performSoftwareReset(SCIC_BASE);

    SCI_resetTxFIFO(SCIC_BASE);
    SCI_resetRxFIFO(SCIC_BASE);
}

//
// sciaRXFIFOISR - SCIA Receive FIFO ISR
//
__interrupt void sciCRXFIFOISR(void)
{
    //SCI_readCharArray(SCIC_BASE, (uint16_t * const)rDataA, 1);

    // Read all data available in the RX buffer
    while (SCI_getRxFIFOStatus(SCIC_BASE)) //(SCI_isDataAvailableNonFIFO(SCIC_BASE))
    {
        rDataA[rxLenSinceTx] = SCI_readCharNonBlocking(SCIC_BASE);
        rxLenSinceTx++;
    }

    SCI_clearOverflowStatus(SCIC_BASE);

    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_RXFF);

    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);

    Example_PassCount++;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * UNIT TESTS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: loopbackTest
 * This test checks that the SCIC module can transmit and receive a full message
 * back in a loopback test.
 *
 * Pin 139 (RX) must be connected to pin 56 (TX).
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
#if ENABLE_SCIC_LOOPBACK_TEST
bool loopbackTest()
{
    //char msg[] = "AT+NAME?\r\n";
    char tx[] = "0123456789";
    uint16_t msgLen = 10;
    char* rx;

    // INITIALIZE UART - BAUDRATE 38400
    initHc05();

    // Write out the message to TX pin
    if (!writeHc05((uint16_t*)tx, msgLen))
    {
        while(1); // Write failed, trap CPU
    }

    // Read back the message from RX pin
    if (!readHc05((uint16_t**)&rx, msgLen))
    {
        while(1); // Read failed, trap CPU
    }

    // Compare TX message to RX message.
    for (int i = 0; i < msgLen; i++)
    {
        if (rx[i] != tx[i])
        {
            return false; // TX and RX messages do not match
        }
    }

    return true;
}
#endif // ENABLE_SCIC_LOOPBACK_TEST
