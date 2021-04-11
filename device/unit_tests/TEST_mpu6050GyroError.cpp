#include "build_switches.h"
#if ENABLE_MPU6050_ESTIMATE_GYRO_ERROR

#include "entry_points.h"
#include "mpu6050_api.h"

#define NUM_GYRO_READINGS   50

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: mpu6050GyroError
 * This function creates an average error of the gyroscope readings.
 * This value will be subtracted from the raw gyroscope readings.
 *
 * NOTE - The gyroscope must be sitting completely still while this test
 * is running to get a good average.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int mpu6050GyroError()
{
    int16_t accelBuf[3];
    int16_t gyroBuf[3];
    int16_t avgGyroError[3];

    int64_t gyroSum[3] = {0};

    Mpu6050Api::init();

    // Add all readings to a sum
    for (uint32_t i = 0; i < NUM_GYRO_READINGS; i++)
    {
        if (!Mpu6050Api::readSensorData(accelBuf, gyroBuf))
        {
            while(1); // I2C read failed - trap CPU for restart
        }

        gyroSum[0] += (int64_t)gyroBuf[0];
        gyroSum[1] += (int64_t)gyroBuf[1];
        gyroSum[2] += (int64_t)gyroBuf[2];
    }

    // Average the sum
    for (uint8_t i = 0; i < 3; i++)
    {
        avgGyroError[i] = (int16_t)((float)gyroSum[i] / (float)NUM_GYRO_READINGS);
    }

    return 0; // BREAKPOINT HERE TO VIEW AVERAGE X, Y, Z ERROR IN avgGyroError
}
#endif // ENABLE_UNIT_TEST_MPU6050_GYRO_ERROR
