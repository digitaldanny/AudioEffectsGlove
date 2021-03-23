/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: setup_target_hw.h
 * This file contains headers/functions to initialize **non-module** target hardware
 * and common helper functions for hardware setup.
 *
 * Examples of inits in this file) Setting clock speed, disabling WDT
 * Examples of inits NOT in this file) Configuring UART for transmitting Bluetooth data
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#ifndef _SETUP_TARGET_HARDWARE
#define _SETUP_TARGET_HARDWARE

#include <string.h>
#include "build_switches.h"

#if TARGET_HW_C2000
extern "C" {
 #include "device.h"
}
#endif // TARGET_HW_C2000

#if TARGET_HW_MSP432
extern "C" {
 #include "driverlib.h"
}
#endif // TARGET_HW_MSP432

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/// SYSTEM INFO DEFINES ///
#define SYSINFO_EXTERNALHWPOWER_ENABLE  systemInfo.externalHwPowerEnable

/// SYSTEM IO DEFINES ///
#define SYSIO_PIN_EXTERNALHWPOWER_PORT  systemIO.externalHwPower.port
#define SYSIO_PIN_EXTERNALHWPOWER_PIN   systemIO.externalHwPower.pin

#define SYSIO_PIN_BLUETOOTHENABLE_PORT  systemIO.bluetoothEn.port
#define SYSIO_PIN_BLUETOOTHENABLE_PIN   systemIO.bluetoothEn.pin

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * STRUCTS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: pinPort_t
 * This struct contains port/pin parameters for an MSP432 pin.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
typedef struct
{
#ifdef TARGET_HW_MSP432
    uint_fast8_t port;
    uint_fast16_t pin;
#endif // TARGET_HW_MSP432
} pinPort_t;

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: systemInfo_t
 * This struct contains information about the system hardware's
 * state.
 *
 * FIELD DESCRIPTIONS:
 * @externalHwPowerEnable - Issue #45
 *  Is power enabled for other hardware (HC-05, MPU6500, Flex Sensors, etc)?
 *
 * TODO:
 * - Add field for timer32 in use
 * - Add field for target configured
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
typedef struct
{
    bool externalHwPowerEnable;
} systemInfo_t;


/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: systemIO_t
 * This struct contains information about the system hardware's
 * state.
 *
 * FIELD DESCRIPTIONS:
 * @externalHwPower - Issue #45
 *  GPIO pin to control power enable for external hardware (LCD, MPU6500, HC-05, etc)
 * @bluetoothEn - Issue #37
 *  GPIO pin to control whether HC-05 module operates in data or command mode.
 *
 * TODO:
 * - ADC14 pin for mux output
 * - 3 GPIO pins for mux control
 * - 3 SPI lines for LCD
 * - 2 GPIO for RS and CS lines on LCD
 * - 2 I2C lines for MPU6500
 * - 2 UART lines for HC-05
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
typedef struct
{
    pinPort_t externalHwPower;
    pinPort_t bluetoothEn;
    pinPort_t i2cSda;
    pinPort_t i2cScl;
    pinPort_t spiMiso;
    pinPort_t spiMosi;
    pinPort_t spiClk;
    pinPort_t spiCs1;
    pinPort_t spiCs2;
} systemIO_t;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

extern volatile systemInfo_t systemInfo;
extern const systemIO_t systemIO;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void initExternalHwPower();
void setupTargetHw();
void delayMs(uint32_t ms);
void setExternalHwPower(bool enable);

#endif // _SETUP_TARGET_HARDWARE
