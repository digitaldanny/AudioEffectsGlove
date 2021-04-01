#include "build_switches.h"
#include "target_hw_common.h"
#include "entry_points.h"
#include "adc_if.h"

#if ENABLE_UNIT_TEST_ADC_MSP432
/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_adcC2000
 * This test expects the user to halt on the ReadAdcChannel function and
 * modify the adc_channel variable in the "Variables" window. This allows
 * the user to manually control the adc channel only using the debugger.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_adcMsp432()
{
    uint16_t adcReading;
    int BREAKPOINT_HERE_FOR_DEBUGGING = 0;
    float normalizedADCRes;

    // Initialize ADC pins, ADC interrupts, and EPWM to automatically
    // trigger conversions.
    if(!Adc::Init())
    {
        /* Failed ADC setup */
        while(1);
    }

    //
    while(1)
    {
        adcReading = Adc::ReadAdcChannel(0); // Flex sensors only using 1 ADC channel

        /// **** BREAKPOINT HERE ***** ///
        normalizedADCRes = (adcReading * 3.3) / 16384;
        /// **** BREAKPOINT HERE ***** ///

        // both adcReading values should be valid values at this point
        BREAKPOINT_HERE_FOR_DEBUGGING++;
    }
}
#endif // ENABLE_UNIT_TEST_ADC_MSP432
