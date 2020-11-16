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
#define TARGET_HW_C2000          1   // Issue #13: Enables firmware specific to C2000 development board.
#define TARGET_HW_PYTHON_CAPABLE (1 & (TARGET_HW_RASPBERRYPI | TARGET_HW_DESKTOP)) // Determines if the selected target hardware can make calls to Python.
#if (TARGET_HW_RASPBERRYPI + TARGET_HW_DESKTOP + TARGET_HW_C2000) > 1
 #error Only 1 hardware target can be active per build.
#endif

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * ENTRY POINTS (Main programs / Unit tests)
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_UNIT_TEST_WIRELESS_API   0   // Issue #8: Enables unit test for the wireless API.
#define ENABLE_UNIT_TEST_NXP_API        0   // Issue #2: Enables unit test for the gyro/accel/mag sensor pack reading.
#define ENABLE_UNIT_TEST_MUX            0   // Issue #7: Enables unit test for setting mux channels. 
#define ENABLE_UNIT_TEST_MUX_C2000      0   // Issue #15: Enables unit test for setting mux channels on the C2000 dev board.
#define ENABLE_UNIT_TEST_ADC            0   // Issue #7: Enables unit test for reading adc channels.
#define ENABLE_UNIT_TEST_ADC_C2000      0   // Issue #14: Enables unit test for reading both adc channels at the same time.
#define ENABLE_UNIT_TEST_UART_C2000     1   // Issue #21: Enables UART (or SCI) loopback test for C2000 dev board using GPIO56 (TX) and GPIO139 (RX).
#define ENABLE_UNIT_TEST_FLEX_SENSORS   0   // Issue #7: Enables unit test for reading all 10 flex sensor voltage levels.
#define ENABLE_MAIN_V1                  0   // Issue #13: Enables main program that will be running

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * WIRELESS API
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_WIRELESS_API_PYTHON  (0 & TARGET_HW_PYTHON_CAPABLE) // Issue #8: Enables calls to server.py for sending/receiving client requests.

#define ENABLE_UART_C2000 (1 & TARGET_HW_C2000) // Issue #21: Enables UART module for transmitting data from C2000 to HC-05 (RS232 Bluetooth chip).

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FLEX SENSOR API
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_MUX_PYTHON       (1 & TARGET_HW_PYTHON_CAPABLE)  // Issue #7: Enables Python module for selecting mux channel.
#define ENABLE_MUX_C2000        (1 & TARGET_HW_C2000)           // Issue #15: Enables C2000 GPIO for selecting mux channel.
#if (ENABLE_MUX_PYTHON + ENABLE_MUX_C2000) > 1
 #error Mux only allows 1 implementation to be active at a time.
#endif

#define ENABLE_ADC_PYTHON       (1 & TARGET_HW_PYTHON_CAPABLE)  // Issue #7: Enables Python module for reading from specific ADC channels.
#define ENABLE_ADC_C2000        (1 & TARGET_HW_C2000)           // Issue #14: Enables C2000 ADC pins.
#if (ENABLE_ADC_PYTHON + ENABLE_ADC_C2000) > 1
 #error ADC only allows 1 implementation to be active at a time.
#endif

#endif
