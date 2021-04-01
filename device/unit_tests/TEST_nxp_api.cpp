#include "build_switches.h"
#if ENABLE_UNIT_TEST_NXP_API

#include "entry_points.h"
#include "nxp_interface_api.h"

int unitTest_nxpApi() {
    NxpWrapper::SensorData_t data;
    Py_Initialize(); // Initialize the Python Interpreter
    printf("TARGET_HW_RASBERRYPI: %d\n", TARGET_HW_RASPBERRYPI);
    printf("nxp.init = %d\n", NxpWrapper::Init());

    for (int i = 0; i < 100; i++)
    {
        NxpWrapper::GetAccelData(data);
        printf("C Accel Data: %f, %f, %f\n", data.x, data.y, data.z);

        NxpWrapper::GetMagData(data);
        printf("C Mag Data: %f, %f, %f\n", data.x, data.y, data.z);
        
        NxpWrapper::GetGyroData(data);
        printf("C Gyro Data: %f, %f, %f\n\n", data.x, data.y, data.z);
    }
    Py_Finalize();
    return 0;
}

#endif // ENABLE_UNIT_TEST_NXP_API
