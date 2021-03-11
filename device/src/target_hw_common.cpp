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
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

const systemIO_t systemIO = {
  .pinExternalHwPower = {GPIO_PORT_P4, GPIO_PIN7}
};

volatile systemInfo_t systemInfo;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FUNCTION IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: setExternalHwPower
 * This function enables/disables power to hardware on the Glove PCB.
 *
 * Hardware that is controlled with this function includes:
 * - HC-05 Bluetooth Module
 * - Flex sensor circuitry (amplifier, LPF)
 * - MPU6500 Gyro/Accelerometer
 * - CFAL6448A LCD
 *
 * INPUTS:
 * @enable - If true, enable power to the external hardware. False disables power.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void setExternalHwPower(bool enable)
{
    // Set GPIO output (active low - set low to enable power)
    if (enable)
    {
        GPIO_setOutputLowOnPin(SYSIO_PIN_EXTERNALHWPOWER_PORT, SYSIO_PIN_EXTERNALHWPOWER_PIN);
    }
    else
    {
        GPIO_setOutputHighOnPin(SYSIO_PIN_EXTERNALHWPOWER_PORT, SYSIO_PIN_EXTERNALHWPOWER_PIN);
    }

    // Update system variable
    SYSINFO_EXTERNALHWPOWER_ENABLE = enable;
}

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
    Timer32_setCount(TIMER32_0_BASE, 10*32000*ms);
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
    // Initialize system info struct
    memset((void*)&systemInfo, 0, sizeof(systemInfo_t));

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

    /* Start HFXT */
    MAP_CS_startHFXT(0);

    /* Initialize MCLK to HFXT (48Mhz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize HSMCLK to HFXT/2 (24Mhz) */
    MAP_CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_2);

    /* Initialize SMCLK to HFXT/4 (12Mhz) */
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Enabling interrupts */
    MAP_Interrupt_enableMaster();

#endif // TARGET_HW_MSP432
}
