/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: uart_if.cpp
 * This module configures a uart module to send/receive data to and from the
 * RS232 Bluetooth chip.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "uart_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

volatile uartRxData_t uartRxData; // Stores data receives through UART
volatile int numInterrupts;
volatile char rx;

#if TARGET_HW_MSP432
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
static const eUSCI_UART_Config uartConfig38400 =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        UART_BRDIV_CLK_16M_BAUDRATE_38400,       // BRDIV
        UART_UCXBRF_CLK_16M_BAUDRATE_38400,      // UCxBRF
        UART_UCXBRS_CLK_16M_BAUDRATE_38400,      // UCxBRS
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode, 8 bit mode
        UART_OVERSAMPLING_CLK_16M_BAUDRATE_38400  // Oversampling
};

static const eUSCI_UART_Config uartConfig9600 =
{
       EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
       UART_BRDIV_CLK_16M_BAUDRATE_9600,       // BRDIV
       UART_UCXBRF_CLK_16M_BAUDRATE_9600,      // UCxBRF
       UART_UCXBRS_CLK_16M_BAUDRATE_9600,      // UCxBRS
       EUSCI_A_UART_NO_PARITY,                  // No Parity
       EUSCI_A_UART_LSB_FIRST,                  // LSB First
       EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
       EUSCI_A_UART_MODE,                       // UART mode, 8 bit mode
       UART_OVERSAMPLING_CLK_16M_BAUDRATE_9600  // Oversampling
};
#endif // TARGET_HW_MSP432

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTION IMPLEMENTATIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool Uart::init(baudRate_e baudRate)
{
#if TARGET_HW_MSP432
    if (baudRate == BAUDRATE_38400)
    {
        return Uart::MSP432::init(&uartConfig38400);
    }
    else if (baudRate == BAUDRATE_9600)
    {
        return Uart::MSP432::init(&uartConfig9600);
    }
    else
    {
        /* Configuration not implemented */
        return false;
    }
#else
    return false;
#endif
}

bool Uart::resetBuffer()
{
#if TARGET_HW_MSP432
    return Uart::MSP432::resetBuffer();
#else
    return false;
#endif
}

bool Uart::send(char* txData, uint8_t numBytes)
{
#if TARGET_HW_MSP432
    return Uart::MSP432::send(txData, numBytes);
#else
    return false;
#endif
}

bool Uart::recv(char** rxData, uint8_t numBytes)
{
#if TARGET_HW_MSP432
    return Uart::MSP432::recv(rxData, numBytes);
#else
    return false;
#endif
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* MSP432 IMPLEMENTATIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_MSP432
bool Uart::MSP432::init(const eUSCI_UART_Config* config)
{
    /* Disable UART module to make changes */
    MAP_UART_disableModule(EUSCI_A2_BASE);

    /* Setting functionality mux for UART pins */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(systemIO.uartTx.port,
         systemIO.uartTx.pin | systemIO.uartRx.pin,
         GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, config);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    return true;
}

bool Uart::MSP432::resetBuffer()
{
    // Reset received data structures for next read
    rx = '\0';
    memset((void*)&uartRxData, 0, sizeof(uartRxData_t));
    return true;
}

bool Uart::MSP432::send(char* txData, uint8_t numBytes)
{
    char* txPtr = txData;
    uint8_t numBytesTransferred = 0;
    numInterrupts = 0;

    Uart::MSP432::resetBuffer();

    // Transfer data from buffer until hitting end of string.
    while (numBytesTransferred < numBytes)
    {
        MAP_UART_transmitData(EUSCI_A2_BASE, *txPtr);
        txPtr++;
        numBytesTransferred++;
    }
    return true;
}

bool Uart::MSP432::recv(char** rxData, uint8_t numBytes)
{
    if (numBytes == UART_NUMBYTES_WAITFORNL)
    {
        // Wait for rx buffer to receive new line character.
        while (rx != '\n');
    }
    else
    {
        // Wait for rx buffer to be filled with requested number of bytes.
        while (uartRxData.idxBuffer < numBytes);
    }

    // Point input buffer at the received buffer to be read by caller.
    *rxData = (char*)&uartRxData.rxBuffer[0];

    return true;
}

extern "C" void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    numInterrupts++;
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    // Read the received data and store it in the next slot of the uart RX buffer.
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        rx = MAP_UART_receiveData(EUSCI_A2_BASE);
        uartRxData.rxBuffer[uartRxData.idxBuffer] = rx;
        uartRxData.idxBuffer++;
    }

    // Trap PC in the interrupt if buffer is about to overflow
    if (uartRxData.idxBuffer >= UART_BUFFER_MAX_LENGTH-1)
    {
        /* ERROR: Should never be receiving message longer than the rx buffer length */
        MAP_Interrupt_disableInterrupt(INT_EUSCIA2);
        while(1);
    }
}
#endif // TARGET_HW_MSP432
