/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: nxp_wrapper.cpp
 * Because the firmware will likely be ported to a microcontroller after the 
 * prototype is complete, the NXP module (currently nxp.py) that interfaces 
 * the accel/gyro/mag sensors will be ported to cpp to run on the microcontroller. 
 * This wrapper class will provide the same functionality whether the build switch 
 * from this issue is enabled or not in order to support identical functionality 
 * on multiple targets.
 *
 * RELATED ISSUE: #2
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "build_switches.h"
#include <stdio.h>

// Prototype includes
#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif

struct SensorData_t
{
    float x;
    float y;
    float z;
};

class NxpWrapper {
    
    private:
#if TARGET_HW_RASPBERRYPI
        bool initRaspi() {
            return true;
        }

        bool getAccelDataRaspi(SensorData_t& sensor_data) {
            return true;
        }

        bool getGyroDataRaspi(SensorData_t& sensor_data) {
            return true;
        }

        bool getMagDataRaspi(SensorData_t& sensor_data) {
            return true;
        }
#endif
    
    public:

        bool init() {
#if TARGET_HW_RASPBERRYPI
            return this->initRaspi();
#else
            return false;
#endif
        }

        bool getAccelData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
            return this->getAccelDataRaspi(sensor_data);
#else
            return false;
#endif
        }

        bool getGyroData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
            return this->getGyroDataRaspi(sensor_data);
#else
            return false;
#endif
        }

        bool getMagData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
            return this->getMagDataRaspi(sensor_data);
#else
            return false;
#endif
        }
};

int main()
{
    printf("TARGET_HW_RASBERRYPI: %d\n", TARGET_HW_RASPBERRYPI);
    NxpWrapper nxp;
    printf("nxp.init = %d\n", nxp.init());
    return 0;
}
