#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "unit_tests.h"

/// DEBUG **********
#include "lcd_64x48_bitmap.h"
#include "mpu.h"
#include "i2c_if.h"
/// DEBUG **********

// Configuration register indices
#define MPU6050_NUM_CONFIGS         3
#define MPU6050_PWR_MGMT_1          0
#define MPU6050_GYRO_CONFIG         1
#define MPU6050_ACCEL_CONFIG        2

// Other
#define MPU6050_DEV_ADDR            0x68
#define MPU6050_SENSOR_ADDR_START   0x3B    // XACCEL_H is the first address to read from to capture all sensor values
#define MPU6050_SENSOR_BURST_LEN    14      // 6 regs Accel X-Z H/L, 2 regs Temp H/L, 6 regs Gyro X-Z H/L

uint8_t sensorBuffer[MPU6050_SENSOR_BURST_LEN];
int16_t accelBuffer[3];
int16_t gyroBuffer[3];

// NOTE - Only append to the end of this array. **ORDER MATTERS**
// Each config contains the following - Register addr, register value
uint8_t mpu6050Configs[MPU6050_NUM_CONFIGS][2] = {
    /* MPU6050_PWR_MGMT_1   */ {0x68, 0x00},
    /* MPU6050_GYRO_CONFIG  */ {0x1B, 0x08}, // +- 500 deg/s
    /* MPU6050_ACCEL_CONFIG */ {0x1C, 0x08} // +- 4g
};

int main()
{
    setupTargetHw(); // MSP432 configurations (clock speed, WDT disable, etc)
    initExternalHwPower(); // Initialize external hardware power (off by default)

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

    I2c::init();

    // Write configuration register values
    for (int iConfig = 0; iConfig < MPU6050_NUM_CONFIGS; iConfig++)
    {
        I2c::write(MPU6050_DEV_ADDR, mpu6050Configs[iConfig][0], 1, &mpu6050Configs[iConfig][1]);
        delayMs(1);
    }

    delayMs(10);

    // Read accelerometer and gyro raw values
    while(1)
    {
        // Update sensor buffer with new readings
        if (!I2c::read(MPU6050_DEV_ADDR, MPU6050_SENSOR_ADDR_START, MPU6050_SENSOR_BURST_LEN, sensorBuffer))
        {
            while (1); // read failed, trap CPU for debugging
        }

        accelBuffer[0] = (sensorBuffer[0] << 8) | (sensorBuffer[1]);
        accelBuffer[1] = (sensorBuffer[2] << 8) | (sensorBuffer[3]);
        accelBuffer[2] = (sensorBuffer[4] << 8) | (sensorBuffer[5]);
        // temp value is stored in indices 6-7
        gyroBuffer[0] = (sensorBuffer[8] << 8) | (sensorBuffer[9]);
        gyroBuffer[1] = (sensorBuffer[10] << 8) | (sensorBuffer[11]);
        gyroBuffer[2] = (sensorBuffer[12] << 8) | (sensorBuffer[13]);

        delayMs(1); // delay here for breakpointing
    }
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

#if ENABLE_MAIN_V1
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
#endif // ENABLE_MAIN_V1
}
