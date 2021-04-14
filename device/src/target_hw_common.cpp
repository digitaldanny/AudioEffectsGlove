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
  .externalHwPower = {GPIO_PORT_P4, GPIO_PIN7}, // P4.7
  .bluetoothEn = {GPIO_PORT_P5, GPIO_PIN0}, // P5.0
  .bluetoothState = {GPIO_PORT_P6, GPIO_PIN0}, // P6.0
  .uartTx = {GPIO_PORT_P3, GPIO_PIN3}, // P3.3
  .uartRx = {GPIO_PORT_P3, GPIO_PIN2}, // P3.2
  .i2cSda = {GPIO_PORT_P6, GPIO_PIN4}, // P6.4
  .i2cScl = {GPIO_PORT_P6, GPIO_PIN5}, // P6.5
  .spiClk = {GPIO_PORT_P1, GPIO_PIN5}, // P1.5
  .spiMosi = {GPIO_PORT_P1, GPIO_PIN6}, // P1.6
  .spiMiso = {GPIO_PORT_P1, GPIO_PIN7}, // P1.7
  .spiCs1 = {GPIO_PORT_P3, GPIO_PIN5}, // P3.5
  .lcdRs = {GPIO_PORT_P2, GPIO_PIN3}, // P2.3
  .lcdReset = {GPIO_PORT_P5, GPIO_PIN1}, // P5.1
  .flexAdc = {GPIO_PORT_P5, GPIO_PIN5}, // P5.5
  .flexMux = {
              {GPIO_PORT_P3, GPIO_PIN7}, // P3.7
              {GPIO_PORT_P4, GPIO_PIN1}, // P4.1
              {GPIO_PORT_P4, GPIO_PIN6} // P4.6
  },
  .ocvAdc = {GPIO_PORT_P5, GPIO_PIN2}, // P5.2
  .ccvAdc = {GPIO_PORT_P5, GPIO_PIN4}, // P5.4
  .spareAdc = {GPIO_PORT_P5, GPIO_PIN3}, // P5.3
  .spareGpio = {GPIO_PORT_P2, GPIO_PIN7} // P2.7
};

volatile systemInfo_t systemInfo;
volatile uint32_t millisCount; // millis function would not work with this inside systemInfo variable

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FUNCTION IMPLEMENTATIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: millis
 *This function returns the number of milliseconds at the time. The number starts
 *increasing once the board is powered up and will overflow after days time.
 *
 * DESCRIPTION: SysTick_Handler
 * This interrupt triggers once per millisecond to increment the millis count variable.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
uint32_t millis()
{
    return millisCount;
}

extern "C" void SysTick_Handler(void)
{
    millisCount++;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: initExternalHwPower
 * This function initializes uC IO pin as an output GPIO to control the external
 * glove hardware power enable.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void initExternalHwPower()
{
    // Configure GPIO as output pin with the power disabled by default.
    setExternalHwPower(true);
    MAP_GPIO_setAsOutputPin(SYSIO_PIN_EXTERNALHWPOWER_PORT, SYSIO_PIN_EXTERNALHWPOWER_PIN);
}

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
    delayMs(1000);
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
    for (int i = 0; i < ms; i++)
    {
        delayUs(1260);
    }
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: delayUs
 * Software delay in microseconds (rough approximation).
 * The value is fairly accurate for values <100, but testing with a value of 100000
 * shows that the timer is off by about 8.2% (91.8 ms instead of 100 ms).
 *
 * INPUTS:
 * @ms - Number of microseconds that software delay should last.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void delayUs(uint32_t us)
{
#if TARGET_HW_MSP432
    uint32_t delayCount = 12*us; // 44 is an experimentally chosen multiplier to produce close to desired delay

    // Initialize the timer delay
    Timer32_initModule( TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_disableInterrupt(TIMER32_0_BASE);

    // Wait for timer to finish
    Timer32_haltTimer(TIMER32_0_BASE);
    Timer32_setCount(TIMER32_0_BASE, delayCount);
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
    millisCount = 0;

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

    /* Set GPIO to be Crystal In/Out for HFXT */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_CS_setExternalClockSourceFrequency(32768, 16000000);

    /* Before we start we have to change VCORE to 1 to support the 48MHz frequency */
    // NOTE: This may not be needed anymore since I changed the clock speed to 16 MHz.
    // Not seeing any issues with the clock right now though, so I am leaving the code here.
    MAP_PCM_setCoreVoltageLevel(PCM_AM_LDO_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Starting HFXT and LFXT in non-bypass mode without a timeout. */
    MAP_CS_startHFXT(false);
    //MAP_CS_startLFXT(false);

    /* Initializing the clock source as follows:
    * MCLK = HFXT = 16MHz
    * ACLK = LFXT = 16KHz
    * HSMCLK = HFXT/1 = 16MHz
    * SMCLK = HFXT/1 = 16MHz
    * BCLK = REFO = 32kHz
    */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //MAP_CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // Configuring SysTick to trigger every millisecond
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(MAP_CS_getMCLK() / 1000);
    //MAP_Interrupt_enableSleepOnIsrExit();
    MAP_SysTick_enableInterrupt();

    /* Enabling interrupts */
    MAP_Interrupt_enableMaster();

#endif // TARGET_HW_MSP432
}
