#ifndef _BUILD_SWITCHES
#define _BUILD_SWITCHES

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * TARGET HARDWARE
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define TARGET_HW_RASPBERRYPI    1   // Issue #1: Enables firmware that will only run on the prototype (Raspberry Pi).
#define TARGET_HW_DESKTOP        0   // Issue #9: Enables dummy responses from sensors for Desktop development.
#define TARGET_HW_PYTHON_CAPABLE (1 & (TARGET_HW_RASPBERRYPI | TARGET_HW_DESKTOP)) // Determines if the selected target hardware can make calls to Python.

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * ENTRY POINTS (Main programs / Unit tests)
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_UNIT_TEST_WIRELESS_API   0   // Issue #8: Enables unit test for the wireless API.
#define ENABLE_UNIT_TEST_NXP_API        0   // Issue #2: Enables unit test for the gyro/accel/mag sensor pack reading.
#define ENABLE_UNIT_TEST_MUX            1   // Issue #7: Enables unit test for setting mux channels. 

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * WIRELESS API
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_WIRELESS_API_PYTHON  (0 & TARGET_HW_PYTHON_CAPABLE) // Issue #8: Enables calls to server.py for sending/receiving client requests.

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FLEX SENSOR API
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#define ENABLE_MUX_API_PYTHON       (1 & TARGET_HW_PYTHON_CAPABLE) // Issue #7: Enables Python module for selecting mux channel.

#endif
