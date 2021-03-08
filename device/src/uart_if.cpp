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

static volatile char DEBUGRXBUF;
static volatile bool flagRxReady;
#endif // TARGET_HW_MSP432

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTION IMPLEMENTATIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool Uart::init()
{
#if TARGET_HW_MSP432
    return Uart::MSP432::init();
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

bool Uart::recv(char* rxData)
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
bool Uart::MSP432::init()
{
    /* Selecting P3.2 and P3.3 in UART mode
     * P3.2 -> MSP Rx
     * P3.3 -> MSP Tx
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig38400);

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
    flagRxReady = false;

    // Transfer data from buffer until hitting end of string.
    while (*txPtr != '\0')
    {
        MAP_UART_transmitData(EUSCI_A2_BASE, *txPtr);
        txPtr++;
    }
    return true;
}

bool Uart::MSP432::recv(char* rxData)
{
    while (!flagRxReady);
    return true; // @TODO - Should wait for entire message to be received from interrupt!!!
}

/* EUSCI A0 UART ISR - Echos data back to PC host */
extern "C" void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        DEBUGRXBUF = MAP_UART_receiveData(EUSCI_A2_BASE);
    }

    // Final byte of data received when final character is \n
    if (DEBUGRXBUF == '\n')
        flagRxReady = true;
}
#endif // TARGET_HW_MSP432
