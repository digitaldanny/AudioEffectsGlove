#include <stdio.h>
#include "nxp_interface_api.h"

int main()
{
    NxpWrapper::SensorData_t data;
    printf("TARGET_HW_RASBERRYPI: %d\n", TARGET_HW_RASPBERRYPI);
    printf("nxp.init = %d\n", NxpWrapper::Init());

    NxpWrapper::GetAccelData(data);
    printf("C Accel Data: %f, %f, %f\n", data.x, data.y, data.z);

    NxpWrapper::GetMagData(data);
    printf("C Mag Data: %f, %f, %f\n", data.x, data.y, data.z);
    
    NxpWrapper::GetGyroData(data);
    printf("C Gyro Data: %f, %f, %f\n", data.x, data.y, data.z);
    return 0;
}