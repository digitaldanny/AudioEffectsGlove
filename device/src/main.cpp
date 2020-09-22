#include <stdio.h>
#include "build_switches.h"
#include "unit_tests.h"

int main()
{

#if ENABLE_UNIT_TEST_WIRELESS_API
    unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
    unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
    unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

    return 0;
}