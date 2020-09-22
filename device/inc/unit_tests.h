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

#endif