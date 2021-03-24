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

// Send data for SCI-C
volatile uint16_t sDataA[SCI_MAX_BUFFER_LENGTH];

// Received data for SCI-C
volatile uint16_t rDataA[SCI_MAX_BUFFER_LENGTH];

// Used for checking the received data
volatile uint16_t rDataPointA;
volatile bool flagRxReady;

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
    flagRxReady = false;

    SCI_writeCharArray(SCIC_BASE, (const uint16_t * const)msg, len);

    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF);

    //
    // Issue PIE ACK
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);

    return true;
}

bool readHc05(uint16_t** rx, uint16_t len)
{
    while (!flagRxReady);
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
    Interrupt_register(INT_SCIC_TX, sciCTXFIFOISR);

    //
    // Initialize the Device Peripherals:
    //
    initSCICFIFO();

    Interrupt_enable(INT_SCIC_RX);
    Interrupt_enable(INT_SCIC_TX);

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
// sciaTXFIFOISR - SCIA Transmit FIFO ISR
//
__interrupt void sciCTXFIFOISR(void)
{
    uint16_t i;

    SCI_writeCharArray(SCIC_BASE, sDataA, 1);

    //
    // Increment send data for next cycle
    //
    for(i = 0; i < 2; i++)
    {
        sDataA[i] = (sDataA[i] + 1) & 0x00FF;
    }

    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF);

    //
    // Issue PIE ACK
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}

//
// sciaRXFIFOISR - SCIA Receive FIFO ISR
//
__interrupt void sciCRXFIFOISR(void)
{
    SCI_readCharArray(SCIC_BASE, (uint16_t * const)rDataA, 1);

    flagRxReady = true;

    SCI_clearOverflowStatus(SCIC_BASE);

    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_RXFF);

    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);

    Example_PassCount++;
}