#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "entry_points.h"

int main()
{
    setupTargetHw(); // MSP432 configurations (clock speed, WDT disable, etc)
    initExternalHwPower(); // Initialize external hardware power (on by default)

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

#if ENABLE_UNIT_TEST_STATE_OF_CHARGE
    unitTest_stateOfCharge();
#endif // ENABLE_UNIT_TEST_STATE_OF_CHARGE

#if ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE
    unitTest_hc05RwToSlave();
#endif // ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE

#if ENABLE_MPU6050_ESTIMATE_GYRO_ERROR
    mpu6050GyroError();
#endif // ENABLE_MPU6050_ESTIMATE_GYRO_ERROR

#if ENABLE_HC05_CONFIG_MSTR
    unitTest_hc05ConfigMaster();
#endif // ENABLE_HC05_CONFIG_MSTR

#if ENABLE_HAND_TRACKING_GLOVE
    handTrackingGlove();
#endif // ENABLE_HAND_TRACKING_GLOVE

    while(1); // TRAP CPU - Should not be here!
}
