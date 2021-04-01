/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "hc05_api.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * TOP LEVEL DEFINITIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
bool Hc05Api::SetMode(hc05Mode_e mode)
{
#if TARGET_HW_MSP432
    return Hc05Api::MSP432::SetMode(mode);
#else
    return false;
#endif
}

bool Hc05Api::IsSlaveConnected()
{
#if TARGET_HW_MSP432
    return Hc05Api::MSP432::IsSlaveConnected();
#else
    return false;
#endif
}

bool Hc05Api::Send(char* txData)
{
#if TARGET_HW_MSP432
    return Hc05Api::MSP432::Send(txData);
#else
    return false;
#endif
}

bool Hc05Api::Recv(char** rxData)
{
#if TARGET_HW_MSP432
    return Hc05Api::MSP432::Recv(rxData);
#else
    return false;
#endif
}

#if TARGET_HW_MSP432
/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * MSP432 DEFINITIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: SetMode
 * Set the HC-05 module into DATA or CMD mode.
 *
 * This module will affect the UART baud rate and will also power cycle the
 * external hardware using the PCh power MOSFET. This function assumes the
 * power switch inits have already been completed.
 *
 * INPUTS:
 * @mode - Selects whether HC-05 is in data or command mode.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Hc05Api::MSP432::SetMode(hc05Mode_e mode)
{
    baudRate_e baudRate;

    // Determine required baudRate and EN setting to place HC-05 is expected mode.
    if (mode == HC05MODE_DATA)
    {
        baudRate = BAUDRATE_9600;
        GPIO_setOutputLowOnPin(systemIO.bluetoothEn.port, systemIO.bluetoothEn.pin);
    }
    else
    {
        baudRate = BAUDRATE_38400;
        GPIO_setOutputHighOnPin(systemIO.bluetoothEn.port, systemIO.bluetoothEn.pin);
    }

    // Initialize UART TX/RX pins for communicating with the HC-05 module.
    if (!Uart::init(baudRate))
    {
        /* Uart initializations failed */
        while (1);
    }

    // Initialize a GPIO for controlling the HC-05 EN pin to select between
    // Data/Command modes.
    MAP_GPIO_setAsOutputPin(systemIO.bluetoothEn.port, systemIO.bluetoothEn.pin);

    // Configure the STATE GPIO for reading if the HC-05 is connected to slave or not.
    MAP_GPIO_setAsInputPin(systemIO.bluetoothState.port, systemIO.bluetoothState.pin);

    // Power cycle to switch the HC-05 mode.
    setExternalHwPower(false);
    setExternalHwPower(true);
    return true;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: IsSlaveConnected
 * Checks if the master HC-05 is paired with the slave HC-05 using the STATE pin.
 *
 * RETURN: True if devices are successfully paired and ready to communicate.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Hc05Api::MSP432::IsSlaveConnected()
{
    return MAP_GPIO_getInputPinValue(systemIO.bluetoothState.port, systemIO.bluetoothState.pin);
}

bool Hc05Api::MSP432::Send(char* txData)
{
    return Uart::send(txData);
}

bool Hc05Api::MSP432::Recv(char** rxData)
{
    return Uart::recv(rxData);
}

#endif // TARGET_HW_MSP432
