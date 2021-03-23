#include "i2c_if.h"

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
        12000000,                               // SMCLK = 12MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        //EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD
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

    /* Enable master interrupt for the transferring data */
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    /* I2C initializations currently happen for each read/write call to support burst
     * transfers, so there is no need to set I2C configurations here.
     */
    return true;
}

bool I2c::MSP432::write(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *Data)
{
    return false;
}

/***********************************************************
  Function:
*/
bool I2c::MSP432::read(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t ui8ByteCount, uint8_t *Data)
{
    /* Wait until ready */
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    /* Reset flag so this function waits for receive interrupts to complete */
    stopSent = false;

    /* Set the number of bytes that the RX interrupt should be waiting for */
    numTransferBytes = ui8ByteCount;

    /* Assign Data to local Pointer so that data is returned to calling function's buffer */
    pData = Data;

    /* Disable I2C module to make changes to I2C configurations */
    MAP_I2C_disableModule(EUSCI_B1_BASE);

    /* Setup the number of bytes to receive */
    //i2cConfig.byteCounterThreshold = ui8ByteCount;
    MAP_I2C_initMaster(EUSCI_B1_BASE, (const eUSCI_I2C_MasterConfig *)&i2cConfig);

    /* Load device slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, ui8Addr);

    //MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_NAK_INTERRUPT); /////////////////

    /* Enable and clear the interrupt flag */
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT1 + EUSCI_B_I2C_RECEIVE_INTERRUPT1);
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT1);

    /* Set Master in transmit mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable master interrupt for the transferring data */
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    /* Making sure the last transaction has been completely sent out */
    while (MAP_I2C_masterIsStopSent(EUSCI_B1_BASE) == EUSCI_B_I2C_SENDING_STOP);

    /* Send start and the first byte of the transmit buffer. We have to send
     * two bytes to clean out whatever is in the buffer from a previous
     * send  */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, ui8Reg);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, ui8Reg);

    // Wait for the RX interrupt to read all received data before returning
    while (!stopSent);
    return true;
}

/***********************************************************
  Function: EUSCIB1_IRQHandler
 */
extern "C" void EUSCIB1_IRQHandler(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, status);

    /* If we reached the transmit interrupt, it means we are at index 1 of
     * the transmit buffer. When doing a repeated start, before we reach the
     * last byte we will need to change the mode to receive mode, set the start
     * condition send bit, and then load the final byte into the TXBUF.
     */
    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
        //MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData[1]);
        //MAP_I2C_disableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
        //MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        xferIndex = 0;
        //MAP_I2C_masterReceiveStart(EUSCI_B0_BASE);
        //MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);

    }

    /* Receives bytes into the receive buffer. If we have received all bytes,
     * send a STOP condition */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT1)
    {
        if(xferIndex == numTransferBytes - 2)
        {
            MAP_I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);
            pData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        }
        else if(xferIndex == numTransferBytes - 1)
        {
            pData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
            MAP_I2C_disableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT1);
            MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
            xferIndex = 0;
            stopSent = true;
            MAP_Interrupt_disableSleepOnIsrExit();
        }
        else
        {
            pData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        }

    }
}
