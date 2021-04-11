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

volatile eUSCI_status ui8Status;

uint8_t  *pData;
uint8_t  ui8DummyRead;
uint32_t g_ui32ByteCount;
bool     burstMode = false;

/* I2C Master Configuration Parameter */
volatile eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        0,
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD                // Autostop
};

void initI2C(void);
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount);
bool readBurstI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint32_t ui32ByteCount);

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool I2c::init()
{
#ifdef TARGET_HW_MSP432
    initI2C();
    return true;
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
    return readBurstI2C(devAddr, regAddr, data, length);
#else
    return false;
#endif
}

bool I2c::write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
#ifdef TARGET_HW_MSP432
    return writeI2C(devAddr, regAddr, data, length);
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

void initI2C(void)
{
    /* I2C Clock Soruce Speed */
    i2cConfig.i2cClk = MAP_CS_getSMCLK();

    /* Select I2C function for I2C_SCL(P6.5) & I2C_SDA(P6.4) */
    GPIO_setAsPeripheralModuleFunctionOutputPin(EUSCI_I2C_PORT, EUSCI_I2C_SCL_PIN | EUSCI_I2C_SDA_PIN,
            EUSCI_I2C_SCL_PIN_FUNCTION);

    /* Set interrupt to highest priority */
    NVIC_SetPriority(EUSCIB1_IRQn, 0);

    /* Initializing I2C Master to SMCLK at 400kbs with autostop */
//    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);
}

/***********************************************************
  Function:
*/
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint8_t ui8ByteCount)
{
    /* Wait until ready to write */
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    /* Assign Data to local Pointer */
    pData = Data;

    /* Disable I2C module to make changes */
    MAP_I2C_disableModule(EUSCI_B1_BASE);

    /* Setup the number of bytes to transmit + 1 to account for the register byte */
    i2cConfig.byteCounterThreshold = ui8ByteCount + 1;
    MAP_I2C_initMaster(EUSCI_B1_BASE, (const eUSCI_I2C_MasterConfig *)&i2cConfig);

    /* Load device slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, ui8Addr);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable master STOP, TX and NACK interrupts */
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT + EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Set our local state to Busy */
    ui8Status = eUSCI_BUSY;

    /* Send start bit and register */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE,ui8Reg);

    /* Enable master interrupt for the remaining data */
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    // NOW WAIT FOR DATA BYTES TO BE SENT
    while(ui8Status == eUSCI_BUSY)
    {
#ifdef USE_LPM
        MAP_PCM_gotoLPM0();
#else
        __no_operation();
#endif
    }

    /* Disable interrupts */
    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT + EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    MAP_Interrupt_disableInterrupt(INT_EUSCIB1);

    if(ui8Status == eUSCI_NACK)
    {
        return(false);
    }
    else
    {
        return(true);
    }
}

/***********************************************************
  Function:
*/
bool readBurstI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *Data, uint32_t ui32ByteCount)
{
    /* Todo: Put a delay */
    /* Wait until ready */
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    /* Assign Data to local Pointer */
    pData = Data;

    /* Disable I2C module to make changes */
    MAP_I2C_disableModule(EUSCI_B1_BASE);

    /* Setup the number of bytes to receive */
    i2cConfig.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    g_ui32ByteCount = ui32ByteCount;
    burstMode = true;
    MAP_I2C_initMaster(EUSCI_B1_BASE, (const eUSCI_I2C_MasterConfig *)&i2cConfig);

    /* Load device slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, ui8Addr);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable master STOP and NACK interrupts */
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT);

    /* Set our local state to Busy */
    ui8Status = eUSCI_BUSY;

    /* Send start bit and register */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE,ui8Reg);

    /* Enable master interrupt for the remaining data */
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    /* NOTE: If the number of bytes to receive = 1, then as target register is being shifted
     * out during the write phase, UCBxTBCNT will be counted and will trigger STOP bit prematurely
     * If count is > 1, wait for the next TXBUF empty interrupt (just after reg value has been
     * shifted out
     */
    while(ui8Status == eUSCI_BUSY)
    {
        if(MAP_I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0))
        {
            ui8Status = eUSCI_IDLE;
        }
    }

    ui8Status = eUSCI_BUSY;

    /* Turn off TX and generate RE-Start */
    MAP_I2C_masterReceiveStart(EUSCI_B1_BASE);

    /* Enable RX interrupt */
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);

    /* Wait for all data be received */
    while(ui8Status == eUSCI_BUSY)
    {

#ifdef USE_LPM
        MAP_PCM_gotoLPM0();
#else
        __no_operation();
#endif
    }

    /* Disable interrupts */
    MAP_I2C_disableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    MAP_Interrupt_disableInterrupt(INT_EUSCIB1);

    if(ui8Status == eUSCI_NACK)
    {
        return(false);
    }
    else
    {
        return(true);
    }
}

/***********************************************************
  Function: EUSCIB1_IRQHandler
 */
extern "C" void EUSCIB1_IRQHandler(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, status);

    if (status & EUSCI_B_I2C_NAK_INTERRUPT)
    {
        /* Generate STOP when slave NACKS */
        MAP_I2C_masterSendMultiByteStop(EUSCI_B1_BASE);

        /* Clear any pending TX interrupts */
        MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

        /* Set our local state to NACK received */
        ui8Status = eUSCI_NACK;
    }

    if (status & EUSCI_B_I2C_START_INTERRUPT)
    {
        /* Change our local state */
        ui8Status = eUSCI_START;
    }

    if (status & EUSCI_B_I2C_STOP_INTERRUPT)
    {
        /* Change our local state */
        ui8Status = eUSCI_STOP;
    }

    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        /* RX data */
        *pData++ = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        ui8DummyRead= MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);

        if (burstMode)
        {
            g_ui32ByteCount--;
            if (g_ui32ByteCount == 1)
            {
                burstMode = false;

                /* Generate STOP */
                MAP_I2C_masterSendMultiByteStop(EUSCI_B1_BASE);
            }
        }
    }

    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
        /* Send the next data */
        MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, *pData++);
    }

#ifdef USE_LPM
    MAP_Interrupt_disableSleepOnIsrExit();
#endif
}
