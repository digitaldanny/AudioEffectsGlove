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
volatile uint16_t rxLenSinceTx; // how many bytes have been received since last TX?

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * HC05 API IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void initHc05()
{
    resetBuffersHc05();
    initSCIC();
    initSCICFIFO();
}

bool resetBuffersHc05()
{
    // Flush RX buffer for next read
    SCI_resetTxFIFO(SCIC_BASE);
    SCI_resetRxFIFO(SCIC_BASE);

    // Reset RX buffer and length for each transfer
    memset((void*)rDataA, 0, rxLenSinceTx);
    rxLenSinceTx = 0;

    return true;
}

bool writeHc05(uint16_t* msg, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        // Wait until there is space on the TX FIFO before transferring next byte
        while(SCI_getTxFIFOStatus(SCIC_BASE) == SCI_FIFO_TX16);
        SCI_writeCharNonBlocking(SCIC_BASE, msg[i]);
    }

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

bool readHc05NonBlocking(uint16_t** rx, uint16_t len)
{
    // Return data if expected number of bytes are received.
    // Otherwise, do nothing.
    if (rxLenSinceTx >= len)
    {
        *rx = (uint16_t*)rDataA;
        return true;
    }
    else
    {
        return false;
    }
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

    Interrupt_enable(INT_SCIC_RX);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8); // is this the correct interrupt group?
}

void initSCICFIFO(void)
{
    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 38400.
    //
    SCI_setConfig(SCIC_BASE, DEVICE_LSPCLK_FREQ, BAUDRATE_DEFAULT, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));

    // Fixing the baud rate formula issue in SCI_setConfig. The issue is documented in the TI thread below.
    // https://e2e.ti.com/support/microcontrollers/c2000/f/c2000-microcontrollers-forum/651054/c2000ware-sci_setconfig-baud-rate-calculated-wrong
    //EALLOW;
    //uint16_t divider = (uint16_t)(((float)DEVICE_LSPCLK_FREQ / (((float)BAUDRATE_DEFAULT) * 8.0f)) + 0.5f) - 1U;
    //HWREGH(SCIC_BASE + SCI_O_HBAUD) = (divider & 0xFF00) >> 8;
    //HWREGH(SCIC_BASE + SCI_O_LBAUD) = (divider & 0x00FF) >> 0;
    //HWREGH(SCIC_BASE + SCI_O_HBAUD) = 0;
    //HWREGH(SCIC_BASE + SCI_O_LBAUD) = 372;
    //EDIS;

    SCI_enableModule(SCIC_BASE);
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
    // Read all data available in the RX buffer
    while (SCI_getRxFIFOStatus(SCIC_BASE) != SCI_FIFO_RX0)
    {
        rDataA[rxLenSinceTx] = SCI_readCharNonBlocking(SCIC_BASE);
        rxLenSinceTx++;

    }

    SCI_clearOverflowStatus(SCIC_BASE);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * UNIT TESTS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_HC05_DATA_PACKET_TEST
/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: hc05NameTest
 * This test connects the DSP-side slave HC-05 to the glove-side master HC-05
 * and reads/writes bytes of data.
 *
 * Pin 139 (RX) must be connected to the HC-05 TX pin, and pin 56 (TX) must be
 * connected to the HC-05 RX pin. The HC-05 EN pin must be connected to GND
 * and the baud rate must be 9600 to communicate with the device in DATA mode.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool messageProtocolTest()
{
    unsigned char ack = DPP_OPCODE_ACK;
    dataPacket_t* rxPtr;

    // Baud rate must be configured to 9600 for this test
    if (BAUDRATE_DEFAULT != BAUDRATE_9600)
    {
        while(1); // Failure - Configure baud rate to 9600
    }

    initHc05();

    while(1)
    {
        // Read back the message from RX pin
        if (!readHc05((uint16_t**)&rxPtr, sizeof(dataPacket_t)))
        {
            while(1); // Read failed, trap CPU
        }

        resetBuffersHc05();

        // Write out ACK message
        if (!writeHc05((uint16_t*)&ack, 1))
        {
            while(1); // Write failed, trap CPU
        }

        for(uint32_t ii = 0; ii < 100000; ii++);
    }
    return true;
}
#endif // ENABLE_HC05_DATA_PACKET_TEST

#if ENABLE_SCIC_LOOPBACK_TEST
/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: loopbackTest
 * This test checks that the SCIC module can transmit and receive a full message
 * back in a loopback test.
 *
 * This test internally ties pin 139 (RX) to pin 56 (TX).
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool loopbackTest()
{
    //char msg[] = "AT+NAME?\r\n";
    char tx[] = "0123456789";
    uint16_t msgLen = 10;
    char* rx;

    // Internally tie SCIC TX/RX pins together
    SCI_enableLoopback(SCIC_BASE);

    // INITIALIZE UART - BAUDRATE 38400
    initHc05();

    resetBuffersHc05();

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

#if ENABLE_HC05_NAME_TEST
/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: hc05NameTest
 * This test checks that a read from the HC-05 "AT+NAME?" register returns the
 * string "+NAME:HC-05"
 *
 * This test is similar to running a WHO_AM_I test on other ICs.
 *
 * Pin 139 (RX) must be connected to the HC-05 TX pin, and pin 56 (TX) must be
 * connected to the HC-05 RX pin. The HC-05 EN pin must be connected to 3.3V
 * and the baud rate must be 38400 to communicate with the device in CMD mode.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool hc05NameTest()
{
    char tx[] = "AT+NAME?\r\n";
    uint16_t txLen = 10;
    char rxExpected[] = "+NAME:HC-05\r\n";
    uint16_t rxLenExpected = 13;
    char* rx;

    // Baud rate must be configured to 38400 for this test
    if (BAUDRATE_DEFAULT != BAUDRATE_38400)
    {
        while(1); // Failure - Configure baud rate to 38400
    }

    // INITIALIZE UART - BAUDRATE 38400
    initHc05();

    // Write out the message to TX pin
    if (!writeHc05((uint16_t*)tx, txLen))
    {
        while(1); // Write failed, trap CPU
    }

    // Read back the message from RX pin
    if (!readHc05((uint16_t**)&rx, rxLenExpected))
    {
        while(1); // Read failed, trap CPU
    }

    // Compare TX message to RX message.
    for (int i = 0; i < rxLenExpected; i++)
    {
        if (rx[i] != rxExpected[i])
        {
            return false; // TX and RX messages do not match
        }
    }

    return true;
}
#endif // ENABLE_HC05_NAME_TEST

#if ENABLE_HC05_RW_TO_MASTER_TEST
/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: hc05NameTest
 * This test connects the DSP-side slave HC-05 to the glove-side master HC-05
 * and reads/writes bytes of data.
 *
 * Pin 139 (RX) must be connected to the HC-05 TX pin, and pin 56 (TX) must be
 * connected to the HC-05 RX pin. The HC-05 EN pin must be connected to GND
 * and the baud rate must be 9600 to communicate with the device in DATA mode.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool hc05RwToMasterTest()
{
    char tx[] = "OK\n";
    uint16_t txLen = 3;
    char rxExpected[] = "Hello, world!! 0123456789";
    uint16_t rxLenExpected = 25;
    char* rx;

    uint32_t numTestIterations = 500;
    uint32_t numAccurateTransfers = 0;
    float percentOfAccurateTransfers;

    // Baud rate must be configured to 9600 for this test
    if (BAUDRATE_DEFAULT != BAUDRATE_9600)
    {
        while(1); // Failure - Configure baud rate to 9600
    }

    initHc05();

    for (uint32_t testIteration = 0; testIteration < numTestIterations; testIteration++)
    {
        bool iterationPassed = true;

        // Read back the message from RX pin
        if (!readHc05((uint16_t**)&rx, rxLenExpected))
        {
            while(1); // Read failed, trap CPU
        }

        // Compare recieved message with expected message before RX buffers are cleared.
        for (int i = 0; i < rxLenExpected; i++)
        {
            if (rx[i] != rxExpected[i])
            {
                iterationPassed = false; // TX and RX messages do not match
            }
        }

        resetBuffersHc05();

        // Write out ACK message
        if (!writeHc05((uint16_t*)tx, txLen))
        {
            while(1); // Write failed, trap CPU
        }

        // Update number of iterations passed
        if (iterationPassed)
        {
            numAccurateTransfers++;
        }

        for(uint32_t ii = 0; ii < 100000; ii++);
    }

    percentOfAccurateTransfers = (float)numAccurateTransfers / (float)numTestIterations * 100.0f;
    return true;
}
#endif // ENABLE_HC05_RW_TO_MASTER_TEST
