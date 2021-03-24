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
        UART_BRDIV_CLK_12M_BAUDRATE_38400,       // BRDIV
        UART_UCXBRF_CLK_12M_BAUDRATE_38400,      // UCxBRF
        UART_UCXBRS_CLK_12M_BAUDRATE_38400,      // UCxBRS
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode, 8 bit mode
        UART_OVERSAMPLING_CLK_12M_BAUDRATE_38400  // Oversampling
};

static const eUSCI_UART_Config uartConfig9600 =
{
       EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
       UART_BRDIV_CLK_12M_BAUDRATE_9600,       // BRDIV
       UART_UCXBRF_CLK_12M_BAUDRATE_9600,      // UCxBRF
       UART_UCXBRS_CLK_12M_BAUDRATE_9600,      // UCxBRS
       EUSCI_A_UART_NO_PARITY,                  // No Parity
       EUSCI_A_UART_LSB_FIRST,                  // LSB First
       EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
       EUSCI_A_UART_MODE,                       // UART mode, 8 bit mode
       UART_OVERSAMPLING_CLK_12M_BAUDRATE_9600  // Oversampling
};

volatile uartRxData_t uartRxData;
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

bool Uart::send(char* txData)
{
#if TARGET_HW_MSP432
    return Uart::MSP432::send(txData);
#else
    return false;
#endif
}

bool Uart::recv(char** rxData)
{
#if TARGET_HW_MSP432
    return Uart::MSP432::recv(rxData);
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
    /* Selecting P3.2 and P3.3 in UART mode
     * P3.2 -> MSP Rx
     * P3.3 -> MSP Tx
     */
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

bool Uart::MSP432::send(char* txData)
{
    char* txPtr = txData;

    // Reset received data structure for next read
    uartRxData.flagRxReady = false;
    uartRxData.idxBuffer = 0;
    memset((void*)uartRxData.rxBuffer, 0, UART_BUFFER_MAX_LENGTH);

    // Transfer data from buffer until hitting end of string.
    while (*txPtr != '\0')
    {
        MAP_UART_transmitData(EUSCI_A2_BASE, *txPtr);
        txPtr++;
    }
    return true;
}

bool Uart::MSP432::recv(char** rxData)
{
    // Wait for rx buffer to be filled before reading data.
    while (!uartRxData.flagRxReady);

    // Point input buffer at the received buffer to be read by caller.
    *rxData = (char*)&uartRxData.rxBuffer[0];

    return true;
}

extern "C" void EUSCIA2_IRQHandler(void)
{
    char rx;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    // Read the received data and store it in the next slot of the uart RX buffer.
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        rx = MAP_UART_receiveData(EUSCI_A2_BASE);
        uartRxData.rxBuffer[uartRxData.idxBuffer] = rx;
        uartRxData.idxBuffer++;

        // Signal that final byte of data was received when final character is \n
        if (rx == '\n')
        {
            uartRxData.flagRxReady = true;
        }
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
