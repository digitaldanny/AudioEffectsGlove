#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "unit_tests.h"

/// DEBUG **********
#include "lcd_64x48_bitmap.h"
/// DEBUG **********

int main()
{
    setupTargetHw();

    // lcd_setup();
    // while(true)
    // {
    //     lcd_test();
    // }
    // lcd_loop();

#if ENABLE_UNIT_TEST_WIRELESS_API
    unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
    unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
    unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

#if ENABLE_UNIT_TEST_MUX_LP
    unitTest_muxLp();
#endif // ENABLE_UNIT_TEST_MUX_C2000

#if ENABLE_UNIT_TEST_ADC
    unitTest_adc();
#endif // ENABLE_UNIT_TEST_ADC

#if ENABLE_UNIT_TEST_ADC_C2000
    unitTest_adcC2000();
#endif // ENABLE_UNIT_TEST_ADC_C2000

#if ENABLE_UNIT_TEST_FLEX_SENSORS
    unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#if ENABLE_MAIN_V1
    // Quick test to make sure program runs on MSP432
    int variable1 = 0;
    while(1)
    {
        variable1++;
    }
#endif // ENABLE_MAIN_V1

    return 0;
}
