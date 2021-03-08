/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: setup_target_hw.cpp
 * This file contains function implementations for common functions specific
 * to the hardware target.
 *
 * Examples of inits in this file) Setting clock speed, disabling WDT
 * Examples of inits NOT in this file) Configuring UART for transmitting Bluetooth data
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "target_hw_common.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FUNCTION IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: delayMs
 * Software delay in milliseconds (rough approximation).
 *
 * INPUTS:
 * @ms - Number of milliseconds that software delay should last.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void delayMs(uint32_t ms)
{
#if TARGET_HW_MSP432
    // Initialize the timer delay
    Timer32_initModule( TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_disableInterrupt(TIMER32_0_BASE);

    // Wait for timer to finish
    Timer32_haltTimer(TIMER32_0_BASE);
    Timer32_setCount(TIMER32_0_BASE, 3*32000*ms);
    Timer32_startTimer(TIMER32_0_BASE, true);

    while(Timer32_getValue(TIMER32_0_BASE) > 0);
#endif // TARGET_HW_MSP432
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
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Enabling interrupts */
    MAP_Interrupt_enableMaster();

#endif // TARGET_HW_MSP432
}
