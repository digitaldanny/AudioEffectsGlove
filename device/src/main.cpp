#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "entry_points.h"

/// DEBUG **********
#include "lcd_64x48_bitmap.h"
#include "mpu.h"
#include "i2c_if.h"
/// DEBUG **********

int main()
{
    setupTargetHw(); // MSP432 configurations (clock speed, WDT disable, etc)
    initExternalHwPower(); // Initialize external hardware power (off by default)

    //int i = 0;
    //while (true)
    //{
    //    i++;
    //    delayMs(1);
    //}

#if ENABLE_UNIT_TEST
    // Power up the external hardware (needed for some of the unit tests).
    setExternalHwPower(true);
#endif // ENABLE_UNIT_TEST

#if ENABLE_UNIT_TEST_TIMER_DELAY
    TEST_timerDelay();
#endif // ENABLE_UNIT_TEST_TIMER_DELAY

#if ENABLE_UNIT_TEST_LCD_TEXT
    TEST_lcdGraphics();
#endif // ENABLE_UNIT_TEST_LCD_TEXT

#if ENABLE_UNIT_TEST_LCD_DEMO
    TEST_lcdProductDemo();
#endif // ENABLE_UNIT_TEST_LCD_DEMO

#if ENABLE_UNIT_TEST_MPU6050_SENSORDATA_RAW
    TEST_mpu6500RawSensorData();
#endif // ENABLE_UNIT_TEST_MPU6050_SENSORDATA_RAW

#if ENABLE_UNIT_TEST_MPU6050_SENSORDATA
    int ret;
    unsigned int c = 0; //cumulative number of successful MPU/DMP reads
    unsigned int np = 0; //cumulative number of MPU/DMP reads that brought no packet back
    unsigned int err_c = 0; //cumulative number of MPU/DMP reads that brought corrupted packet
    unsigned int err_o = 0; //cumulative number of MPU/DMP reads that had overflow bit set

    I2c::init();

    ret = mympu_open(200);

    while(1)
    {
        ret = mympu_update();

        switch (ret) {
        case 0: c++; break;
        case 1: np++; break;
        case 2: err_o++; break;
        case 3: err_c++; break;
        default:
            break;
        }
        delayMs(1);
    }

#endif // ENABLE_UNIT_TEST_MPU6050_SENSORDATA

#if ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI
    TEST_mpu6500WhoAmISpi();
#endif // ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI

#if ENABLE_UNIT_TEST_MPU6050_WHOAMI_I2C
    TEST_mpu6500WhoAmII2c();
#endif // ENABLE_UNIT_TEST_MPU6050_WHOAMI_I2C

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH
    unitTest_enableExternalHwPower();
#endif // ENABLE_UNIT_TEST_EXT_PWR_SWITCH

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
    unitTest_hc05DeviceName();
#endif // ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#if ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE
    unitTest_hc05RwToSlave();
#endif // ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE

#if ENABLE_HC05_CONFIG_MSTR
    unitTest_hc05ConfigMaster();
#endif // ENABLE_HC05_CONFIG_MSTR

#if ENABLE_MAIN_V1
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
#endif // ENABLE_MAIN_V1
}
