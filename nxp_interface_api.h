#ifndef _NXP_WRAPPER
#define _NXP_WRAPPER

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include <stdio.h>

// Prototype target includes
#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Prototype target defines
#if TARGET_HW_RASPBERRYPI
#define NXP_MODULE      "nxp"
#define SENSOR_CLASS    "sensor"
#define ACCEL_FUNC      "accel"
#define MAG_FUNC        "mag"
#define GYRO_FUNC       "gyro"
#endif

/*
* +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
* DESCRIPTION: RasPi
* The functions below are only used when the program is being run on
* the Raspberry Pi prototype.
* +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
namespace NxpWrapper {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * STRUCTS
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    
    /*
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    * DESCRIPTION: SensorData_t
    * This struct will store Accel/Gyro/Mag data from the fxos8700 and 
    * fxas21002c devices.
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    struct SensorData_t
    {
        float x;
        float y;
        float z;
    };

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    bool Init();
    bool GetAccelData(SensorData_t& sensor_data);
    bool GetGyroData(SensorData_t& sensor_data);
    bool GetMagData(SensorData_t& sensor_data);

    /*
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    * DESCRIPTION: RasPi
    * The functions below are only used when the program is being run on
    * the Raspberry Pi prototype.
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
#if TARGET_HW_RASPBERRYPI
    namespace RasPi{
        bool Init();
        bool GetAccelData(SensorData_t& sensor_data);
        bool GetGyroData(SensorData_t& sensor_data);
        bool GetMagData(SensorData_t& sensor_data);
        bool PythonAssert(PyObject* obj);
    }
#endif
};

#endif