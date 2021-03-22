#include "i2c_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

static uint8_t  *pData;
static uint8_t  ui8DummyRead;
static uint32_t g_ui32ByteCount;
static bool     burstMode = false;
static volatile eUSCI_status ui8Status;

/* I2C Master Configuration Parameter */
static volatile eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        12000000,                               // SMCLK = 12MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        //EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD
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

bool I2c::MSP432::init()
{
    /* Select I2C function for I2C_SCL & I2C_SDA pins */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(systemIO.i2cScl.port, systemIO.i2cScl.pin, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(systemIO.i2cSda.port, systemIO.i2cSda.pin, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400kbs with autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, (const eUSCI_I2C_MasterConfig *)&i2cConfig);

    /* Set Master in transmit mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable and clear the interrupt flag */
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);

    //Enable master Receive interrupt
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    return true;
}

bool I2c::MSP432::write(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *Data)
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
bool I2c::MSP432::read(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *Data)
{
    /* Todo: Put a delay */
    /* Wait until ready */
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    /* Assign Data to local Pointer */
    pData = Data;

    /* Disable I2C module to make changes */
    MAP_I2C_disableModule(EUSCI_B1_BASE);

    /* Setup the number of bytes to receive */
    i2cConfig.byteCounterThreshold = ui8ByteCount;
    i2cConfig.autoSTOPGeneration = EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD;
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
