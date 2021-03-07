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
 * COMMON STRUCTS
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
    uint_fast8_t port;
    uint_fast16_t pin;
} pinPort_t;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FUNCTIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

void setupTargetHw();

#endif // _SETUP_TARGET_HARDWARE
