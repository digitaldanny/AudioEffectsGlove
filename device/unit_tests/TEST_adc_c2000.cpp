#include "build_switches.h"
#if ENABLE_UNIT_TEST_ADC_C2000

#include "entry_points.h"
#include "adc_wrapper.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_adcC2000
 * This test expects the user to halt on the ReadAdcChannel function and
 * modify the adc_channel variable in the "Variables" window. This allows
 * the user to manually control the adc channel only using the debugger.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_adcC2000()
{
    int adcReading[2] = {0};
    int BREAKPOINT_HERE_FOR_DEBUGGING = 0;

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
        /// **** BREAKPOINT HERE ***** ///
        while ((adcReading[0] = Adc::ReadAdcChannel(0)) < 0);
        while ((adcReading[1] = Adc::ReadAdcChannel(1)) < 0);
        /// **** BREAKPOINT HERE ***** ///

        // both adcReading values should be valid values at this point
        BREAKPOINT_HERE_FOR_DEBUGGING++;
    }
}

#endif // ENABLE_UNIT_TEST_ADC_C2000
