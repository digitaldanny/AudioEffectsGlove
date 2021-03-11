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
#define SYSIO_PIN_EXTERNALHWPOWER_PORT  systemIO.pinExternalHwPower.port
#define SYSIO_PIN_EXTERNALHWPOWER_PIN   systemIO.pinExternalHwPower.pin

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
    bool externalHwPowerEnable;
} systemInfo_t;


/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: systemIO_t
 * This struct contains information about the system hardware's
 * state.
 *
 * FIELD DESCRIPTIONS:
 * @pinExternalHwPower - Issue #45
 *  GPIO pin to control power enable for external hardware (LCD, MPU6500, HC-05, etc)
 *
 * TODO:
 * - Add field for timer32 in use
 * - Add field for target configured
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
typedef struct
{
    pinPort_t pinExternalHwPower;
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

void setupTargetHw();
void delayMs(uint32_t ms);
void setExternalHwPower(bool enable);

#endif // _SETUP_TARGET_HARDWARE
