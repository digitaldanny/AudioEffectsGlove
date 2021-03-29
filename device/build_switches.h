/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: build_switches.h
 * This file contains all switches that are used to determine features or
 * unit tests that will be compiled.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#ifndef _BUILD_SWITCHES
#define _BUILD_SWITCHES

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * TARGET HARDWARE
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define TARGET_HW_RASPBERRYPI    0   // Issue #1: Enables firmware that will only run on the prototype (Raspberry Pi).
#define TARGET_HW_DESKTOP        0   // Issue #9: Enables dummy responses from sensors for Desktop development.
#define TARGET_HW_C2000          0   // Issue #13: Enables firmware specific to C2000 development board.
#define TARGET_HW_MSP432         1   // Enables firmware specific to MSP432 launchpad.
#define TARGET_HW_PYTHON_CAPABLE (1 & (TARGET_HW_RASPBERRYPI | TARGET_HW_DESKTOP)) // Determines if the selected target hardware can make calls to Python.
#if (TARGET_HW_RASPBERRYPI + TARGET_HW_DESKTOP + TARGET_HW_C2000 + TARGET_HW_MSP432) > 1
 #error Only 1 hardware target can be active per build.
#endif

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * ENTRY POINTS (Main programs / Unit tests)
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_UNIT_TEST                (1)
#define ENABLE_UNIT_TEST_TIMER_DELAY    (0 & ENABLE_UNIT_TEST)  // Issue NONE: Infinitely toggle using ms and us delays. Requires oscilloscope connections.
#define ENABLE_UNIT_TEST_WIRELESS_API   (0 & ENABLE_UNIT_TEST)   // Issue #8: Enables unit test for the wireless API.
#define ENABLE_UNIT_TEST_NXP_API        (0 & ENABLE_UNIT_TEST)   // Issue #2: Enables unit test for the gyro/accel/mag sensor pack reading.
#define ENABLE_UNIT_TEST_MUX            (0 & ENABLE_UNIT_TEST)   // Issue #7: Enables unit test for setting mux channels.
#define ENABLE_UNIT_TEST_MUX_LP         (0 & ENABLE_UNIT_TEST)   // Issue #15: Enables unit test for setting mux channels on the C2000 dev board.
#define ENABLE_UNIT_TEST_ADC            (0 & ENABLE_UNIT_TEST)   // Issue #7: Enables unit test for reading adc channels.
#define ENABLE_UNIT_TEST_ADC_C2000      (0 & ENABLE_UNIT_TEST)   // Issue #14: Enables unit test for reading both adc channels at the same time.
#define ENABLE_UNIT_TEST_ADC_MSP432     (0 & ENABLE_UNIT_TEST)   // Issue #36: Enables unit test for reading ADC values from flex amplifier.
#define ENABLE_UNIT_TEST_FLEX_SENSORS   (0 & ENABLE_UNIT_TEST)   // Issue #7: Enables unit test for reading all 10 flex sensor voltage levels.
#define ENABLE_UNIT_TEST_HC05_DEVICE_NAME (0 & ENABLE_UNIT_TEST)   // Issue #37: Enable test to read back HC-05 "NAME" register (returns "HC-05" by default)
#define ENABLE_UNIT_TEST_I2C            (0 & ENABLE_UNIT_TEST)   // Issue #39: Enable basic I2C write/read test to be read on logic analyzer.
#define ENABLE_UNIT_TEST_EXT_PWR_SWITCH (0 & ENABLE_UNIT_TEST)  // Issue #45: Enable test that toggles external power switch. Power output be manually validated with DAD board.
#define ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI (0 & ENABLE_UNIT_TEST) // Issue #47: Enable SPI read from MPU6500's WHO_AM_I register.
#define ENABLE_UNIT_TEST_MPU6500_WHOAMI_I2C (0 & ENABLE_UNIT_TEST) // Issue #47: Enable I2C read from MPU6500's WHOI_AM_I register.
#define ENABLE_UNIT_TEST_LCD_DEMO       (0 & ENABLE_UNIT_TEST)  // Issue #46: Enable demo animations to run on CFAL6448A lcd over SPI.
#define ENABLE_UNIT_TEST_LCD_TEXT       (1 & ENABLE_UNIT_TEST)  // Issue #46: Draws custom text and image to the LCD.
#define ENABLE_MAIN_V1                  (0)   // Issue #13: Enables main program that will be running

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * WIRELESS API
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_WIRELESS_API_PYTHON  (0 & TARGET_HW_PYTHON_CAPABLE) // Issue #8: Enables calls to server.py for sending/receiving client requests.

#endif
