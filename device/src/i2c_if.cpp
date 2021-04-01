#include "i2c_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define EUSCI_I2C_STATUS_SLAVE_NACK       1

/* EUSCI port and pin selection */
#define EUSCI_I2C_MODULE                  EUSCI_B1_BASE
#define EUSCI_I2C_PORT                    systemIO.i2cScl.port
#define EUSCI_I2C_SCL_PIN                 systemIO.i2cScl.pin
#define EUSCI_I2C_SCL_PIN_FUNCTION        GPIO_PRIMARY_MODULE_FUNCTION
#define EUSCI_I2C_SDA_PIN                 systemIO.i2cSda.pin
#define EUSCI_I2C_SDA_PIN_FUNCTION        GPIO_PRIMARY_MODULE_FUNCTION

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

static uint8_t  *pData;
static volatile uint32_t xferIndex;
static volatile uint32_t numTransferBytes;
static volatile bool stopSent;

/* I2C Master Configuration Parameter */
static volatile eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        12000000,                               // Set during runtime
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold (using manual stop)
        EUSCI_B_I2C_NO_AUTO_STOP
};

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool I2c::init()
{
#ifdef TARGET_HW_MSP432
    return I2c::MSP432::init();
#else
    return false;
#endif
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: read
 * This function implements the I2C read sequence documented in the MPU6050
 * datasheet (see pg 36 in document linked below).
 *
 * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool I2c::read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
#ifdef TARGET_HW_MSP432
    return I2c::MSP432::read(devAddr, regAddr, length, data);
#else
    return false;
#endif
}

bool I2c::write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
#ifdef TARGET_HW_MSP432
    return I2c::MSP432::write(devAddr, regAddr, length, data);
#else
    return false;
#endif
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* MSP432 IMPLEMENTATIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Thanks to "DavidL" who implemented this I2C polling approach in the TI thread below.
// https://e2e.ti.com/support/microcontrollers/msp430/f/msp-low-power-microcontroller-forum/560625/problem-using-driverlib-in-i2c-polled-mode-on-msp432-launchpad

bool I2c::MSP432::init()
{
    /* I2C initializations currently happen for each read/write call to support burst
     * transfers, so there is no need to set I2C configurations here.
     */

    /* I2C Clock Source Speed */
    //i2cConfig.i2cClk = MAP_CS_getSMCLK();

    /* Select I2C function for I2C_SCL & I2C_SDA */
    GPIO_setAsPeripheralModuleFunctionOutputPin(EUSCI_I2C_PORT, EUSCI_I2C_SCL_PIN,
            EUSCI_I2C_SCL_PIN_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(EUSCI_I2C_PORT, EUSCI_I2C_SDA_PIN,
            EUSCI_I2C_SDA_PIN_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400kbs with no-autostop */
    MAP_I2C_initMaster(EUSCI_I2C_MODULE, (const eUSCI_I2C_MasterConfig *)&i2cConfig);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_I2C_MODULE);

    /* Enable master interrupt for the transferring data */
    //MAP_Interrupt_enableInterrupt(INT_EUSCIB1);
    return true;
}

bool I2c::MSP432::write(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *Data)
{
    /* Todo: Implement a delay */
    /* Wait until ready to write */
    while (MAP_I2C_isBusBusy(EUSCI_I2C_MODULE));

    /* Load device slave address */
    MAP_I2C_setSlaveAddress(EUSCI_I2C_MODULE, ui8Addr);

    /* Send start bit and register */
    MAP_I2C_masterSendMultiByteStart(EUSCI_I2C_MODULE,ui8Reg);

    /* Wait for tx to complete */
    while(!(MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0) &
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    /* Check if slave ACK/NACK */
    if((MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_NAK_INTERRUPT)) &
            EUSCI_B_I2C_NAK_INTERRUPT)
    {
        /* If NACK, set stop bit and exit */
        MAP_I2C_masterSendMultiByteStop(EUSCI_I2C_MODULE);
        return(EUSCI_I2C_STATUS_SLAVE_NACK);
    }

    /* Now write one or more data bytes */
    while(1)
    {
        /* Wait for next INT */
        while(!(MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0) &
                EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        /* If no data to follow, we are done */
        if(ui8ByteCount == 0 )
        {
            MAP_I2C_masterSendMultiByteStop(EUSCI_I2C_MODULE);
            MAP_I2C_clearInterruptFlag(EUSCI_I2C_MODULE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
            return(true);
        }
        /* If more, send the next byte */
        else
        {
            MAP_I2C_masterSendMultiByteNext(EUSCI_I2C_MODULE, *Data++);
        }
        ui8ByteCount--;
    }
}

bool I2c::MSP432::read(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *& Data)
{
    /* Todo: Implement a delay */
    /* Wait until ready */
    while (MAP_I2C_isBusBusy(EUSCI_I2C_MODULE));

    /* Load device slave address */
    MAP_I2C_setSlaveAddress(EUSCI_I2C_MODULE, ui8Addr);

    /* Send start bit and register */
    MAP_I2C_masterSendMultiByteStart(EUSCI_I2C_MODULE,ui8Reg);

    /* Wait for tx to complete */
    while(!(MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0) &
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    /* Check if slave ACK/NACK */
    if((MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_NAK_INTERRUPT)) &
            EUSCI_B_I2C_NAK_INTERRUPT)
    {
        /* If NACK, set stop bit and exit */
        MAP_I2C_masterSendMultiByteStop(EUSCI_I2C_MODULE);
        return(EUSCI_I2C_STATUS_SLAVE_NACK);
    }

    /* Turn off TX and generate RE-Start */
    MAP_I2C_masterReceiveStart(EUSCI_I2C_MODULE);

    /* Wait for start bit to complete */
    while(MAP_I2C_masterIsStartSent(EUSCI_I2C_MODULE));

    if((MAP_I2C_getInterruptStatus(EUSCI_I2C_MODULE, EUSCI_B_I2C_NAK_INTERRUPT)) &
            EUSCI_B_I2C_NAK_INTERRUPT)
    {
        /* If NACK, set stop bit and exit */
        MAP_I2C_masterSendMultiByteStop(EUSCI_I2C_MODULE);
        return(EUSCI_I2C_STATUS_SLAVE_NACK);
    }

    /* Read one or more bytes */
    while(ui8ByteCount)
    {
        /* If reading 1 byte (or last byte), generate the stop to meet the spec */
        if(ui8ByteCount-- == 1)
        {
            *Data++ = MAP_I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);
        }
        else
        {
            /* Wait for next RX interrupt */
            while(!(MAP_I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0) &
                    EUSCI_B_I2C_RECEIVE_INTERRUPT0));

            /* Read the rx byte */
            *Data++ = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        }
    }

    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    return(true);
}

/***********************************************************
  Function: EUSCIB1_IRQHandler
 */
extern "C" void EUSCIB1_IRQHandler(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_I2C_MODULE);
    MAP_I2C_clearInterruptFlag(EUSCI_I2C_MODULE, status);

    /* Receives bytes into the receive buffer. If we have received all bytes,
     * send a STOP condition */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        // Read the next byte from the slave, then increment receive buffer index
        pData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);

        // Read the next byte.
        if (xferIndex != numTransferBytes)
        {
            MAP_I2C_masterReceiveMultiByteStop(EUSCI_I2C_MODULE);
            pData[xferIndex] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_I2C_MODULE);
            xferIndex++;
        }

        // Receive the last byte needed.
        else
        {
            pData[xferIndex] = MAP_I2C_masterReceiveMultiByteFinish(EUSCI_I2C_MODULE);
            MAP_I2C_disableInterrupt(EUSCI_I2C_MODULE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);

            // Set flag to complete read
            stopSent = true;
        }
    }
}
