#include <stdio.h>
#include "build_switches.h"
#include "unit_tests.h"
#include "setup_target_hw.h"

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

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: setupTargetHw
 * Set up "non-module" hardware registers.
 * Ex of included inits.) Set clock speed to 48 MHz, disable WDT
 * Ex of inits NOT included.) Configuring UART to transmit data over Bluetooth
 *
 * INPUTS: None
 * RETURN: None
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void setupTargetHw()
{
#if TARGET_HW_C2000
    Device_init();                  // Initialize device clock and peripherals
    Device_initGPIO();              // Disable pin locks and enable internal pullups.
    Interrupt_initModule();         // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initVectorTable();    // Initialize the PIE vector table with pointers to the shell ISRs

    // More documentation on these registers here: https://www.ti.com/lit/ug/spru430f/spru430f.pdf
    EALLOW; // Enable modifying/reading protected registers.
    EINT;   // Enable Global Interrupt (INTM)
    ERTM;   // Enable realtime interrupt (DBGM)
#endif // TARGET_HW_C2000

#if TARGET_HW_MSP432

    /* Halting watchdog timer */
    WDT_A_holdTimer();

    /*
     * Initializes Core Clock to Maximum Frequency with highest accuracy
     *  Initializes GPIO for HFXT in and out
     *  Enables HFXT
     *  Sets MSP432 to Power Active Mode to handle 48 MHz
     *  Sets Flash Wait states for 48 MHz
     *  Sets MCLK to 48 MHz
     *  Sets HSMCLK to 24 MHz
     *  Sets SMCLK to 12 MHz
     */
    /* Set GPIO to be Crystal In/Out for HFXT */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Set Core Voltage Level to VCORE1 to handle 48 MHz Speed */
    while(!PCM_setCoreVoltageLevel(PCM_VCORE1));

    /* Set frequency of HFXT and LFXT */
    MAP_CS_setExternalClockSourceFrequency(32000, 48000000);

    /* Set 2 Flash Wait States */
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Setting DCO to 48MHz  */
    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    /* Enabling the FPU for floating point operation */
    // MAP_FPU_enableModule();
    // MAP_FPU_enableLazyStacking();

#endif // TARGET_HW_MSP432
}
