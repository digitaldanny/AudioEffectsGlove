#include <stdio.h>
#include "build_switches.h"
#include "unit_tests.h"

#if TARGET_HW_C2000
extern "C" {
 #include "device.h"
}
#endif // TARGET_HW_C2000

// TEMPORARY.. REMOVE THIS
#include "adc_wrapper.h"
int adc_reading;
int adc_channel = 0;

int main()
{
#if TARGET_HW_C2000
    Device_init();                  // Initialize device clock and peripherals
    Device_initGPIO();              // Disable pin locks and enable internal pullups.
    Interrupt_initModule();         // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initVectorTable();    // Initialize the PIE vector table with pointers to the shell ISRs

    // More documentation on these registers here: https://www.ti.com/lit/ug/spru430f/spru430f.pdf
    EALLOW;                         // Enable modifying/reading protected registers.
    EINT;                           // Enable Global Interrupt (INTM)
    ERTM;                           // Enable realtime interrupt (DBGM)
#endif

#if ENABLE_UNIT_TEST_WIRELESS_API
    unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
    unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
    unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

#if ENABLE_UNIT_TEST_MUX_C2000
    unitTest_muxC2000();
#endif // ENABLE_UNIT_TEST_MUX_C2000

#if ENABLE_UNIT_TEST_ADC
    unitTest_adc();
#endif // ENABLE_UNIT_TEST_ADC

#if ENABLE_UNIT_TEST_ADC_C2000

    if(!Adc::Init())
    {
        /* Failed ADC setup */
        ;
    }

    while(1)
    {
        adc_reading = Adc::ReadAdcChannel(adc_channel & 0x1);
        adc_channel++;
        adc_channel &= 0x1;
    }

#endif // ENABLE_UNIT_TEST_ADC_C2000

#if ENABLE_UNIT_TEST_FLEX_SENSORS
    unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#if ENABLE_MAIN_V1
    // Quick test to make sure program runs on C2000
    int variable1 = 0;
    variable1++;
    while(1);
#endif // ENABLE_MAIN_V1

    return 0;
}
