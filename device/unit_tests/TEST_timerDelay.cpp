#include "build_switches.h"
#if ENABLE_UNIT_TEST_TIMER_DELAY

#include "entry_points.h"
#include "target_hw_common.h"

#define DELAY_US    delayUs
#define DELAY_MS    delayMs
#define DELAY_FUNC  DELAY_MS
#define DELAY_VALUE 100

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05DeviceName
 * This test infinitely toggles a GPIO pin on and off with a specified delay
 * between. This allows the user to test the accuracy of the delay function
 * with an oscilloscope.
 *
 * This test uses a spare GPIO pin (P2.7 at time of writing the unit test).
 *
 * User can select whether to test with milliseconds or microseconds using
 * the DELAY_FUNC define above.
 *
 * User can set number of ms or us to delay with the DELAY_VALUE define.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int TEST_timerDelay()
{
    uint32_t delay = DELAY_VALUE;
    bool enable = false;
    MAP_GPIO_setAsOutputPin(systemIO.spareGpio.port, systemIO.spareGpio.pin);

    while (1)
    {
        if (enable)
            MAP_GPIO_setOutputHighOnPin(systemIO.spareGpio.port, systemIO.spareGpio.pin);
        else
            MAP_GPIO_setOutputLowOnPin(systemIO.spareGpio.port, systemIO.spareGpio.pin);
        enable = !enable;
        DELAY_FUNC(delay);
    }

    return 0;
}
#endif // ENABLE_UNIT_TEST_TIMER_DELAY
