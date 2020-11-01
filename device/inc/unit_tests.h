#ifndef _SRC_UNIT_TESTS
#define _SRC_UNIT_TESTS

#include "build_switches.h"

#if ENABLE_UNIT_TEST_WIRELESS_API
int unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
int unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
int unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

#if ENABLE_UNIT_TEST_MUX_C2000
int unitTest_muxC2000();
#endif // ENABLE_UNIT_TEST_MUX_C2000

#if ENABLE_UNIT_TEST_ADC
int unitTest_adc();
#endif // ENABLE_UNIT_TEST_ADC

#if ENABLE_UNIT_TEST_FLEX_SENSORS
int unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#endif
