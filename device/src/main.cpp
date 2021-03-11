#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "unit_tests.h"

/// DEBUG **********
#include "lcd_64x48_bitmap.h"
#include "mpu6500.h"
#include "i2c_if.h"
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

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH
    // Configure GPIO as output pin - TODO: Move this into a different function
    GPIO_setOutputLowOnPin(SYSIO_PIN_EXTERNALHWPOWER_PORT, SYSIO_PIN_EXTERNALHWPOWER_PIN);
    MAP_GPIO_setAsOutputPin(SYSIO_PIN_EXTERNALHWPOWER_PORT, SYSIO_PIN_EXTERNALHWPOWER_PIN);

    while(true)
    {
        setExternalHwPower(true);
        delayMs(1);

        setExternalHwPower(false);
        delayMs(1);
    }
#endif // ENABLE_UNIT_TEST_EXT_PWR_SWITCH

#if ENABLE_UNIT_TEST_I2C
    I2c::init();
    while (true)
    {
        uint8_t tx[3] = {0x12, 0x34, 0x56};
        //uint8_t rx[3] = {0};
        I2c::write(0xAB, 0xCD, 3, tx);
        //I2c::read(0xAB, 0xCD, 3, tx);
        delayMs(1);
    }
#endif // ENABLE_UNIT_TEST_I2C

    //mpu6500Init();
    //while(true)
    //{
    //    mpu6500TestConnection();
    //}

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

#if ENABLE_UNIT_TEST_ADC_MSP432
    unitTest_adcMsp432();
#endif // ENABLE_UNIT_TEST_ADC_MSP432

#if ENABLE_UNIT_TEST_FLEX_SENSORS
    unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME
    //while (1)
    //{
    //    unitTest_hc05DeviceName();
    //}
#endif // ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#if ENABLE_MAIN_V1
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
#endif // ENABLE_MAIN_V1
}
