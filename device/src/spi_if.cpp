/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: spi_if.cpp
 * This file contains functions to configure/use SPI.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "spi_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void Spi::init()
{
#ifdef TARGET_HW_MSP432
    Spi::MSP432::init();
#endif // TARGET_HW_MSP432
}

uint8_t Spi::transferByte(uint8_t data)
{
#ifdef TARGET_HW_MSP432
    return Spi::MSP432::transferByte(data);
#else
    return 0;
#endif // TARGET_HW_MSP432
}

#ifdef TARGET_HW_MSP432
/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* MSP432 IMPLEMENTATIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Spi::MSP432::init
 * Configure EUSCI_B0 pins (P1.5-P1.7) for 3-wire master Spi.
 * Configuration details included in the description for the 'spiMasterConfig' struct.
 *
 * INPUTS: N/A
 * RETURN: N/A
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void Spi::MSP432::init()
{
    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);

    /* Enabling interrupt on receive */
    SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIB0);
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Spi::MSP432::transferByte
 * Transfer a byte of data to the SPI slave and read back the response.
 *
 * INPUTS:
 * @data - byte of data to transfer.
 *
 * RETURN:
 * @uint8_t - byte of data received from slave.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
uint8_t Spi::MSP432::transferByte(uint8_t data)
{
    /* Polling to see if the TX buffer is ready */
    while (!SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT));

    spiRxData.flagSpiReady = false;

    /* Transmitting data to slave */
    SPI_transmitData(EUSCI_B0_BASE, data);

    /* Wait for IRQ handler to receive and store returned data */
    while (!spiRxData.flagSpiReady);

    return spiRxData.data;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: EUSCIB0_IRQHandler
 * SPI receive handler as defined in startup_msp432p401r_ccs.c -> interruptVectors.
 *
 * This handler completes the following tasks:
 * - Clears SPI RX interrupt flag.
 * - Reads data received from SPI slave.
 * - Sets global flag to "true" to notify main program that response was received.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void EUSCIB0_IRQHandler(void)
{
    uint32_t status = SPI_getEnabledInterruptStatus(EUSCI_B0_BASE);

    SPI_clearInterruptFlag(EUSCI_B0_BASE, status);

    if(status & EUSCI_B_SPI_RECEIVE_INTERRUPT)
    {
        // Store the slave response into global variable
        Spi::spiRxData.data = SPI_receiveData(EUSCI_B0_BASE);

        // Notify main program that SPI response has been received
        Spi::spiRxData.flagSpiReady = true;
    }

}
#endif // TARGET_HW_MSP432
