/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: nxp_wrapper.cpp
 * Because the firmware will likely be ported to a microcontroller after the 
 * prototype is complete, the NXP module (currently nxp.py) that interfaces 
 * the accel/gyro/mag sensors will be ported to cpp to run on the microcontroller. 
 * This wrapper class will provide the same functionality whether the build switch 
 * from this issue is enabled or not in order to support identical functionality 
 * on multiple targets.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "nxp_interface_api.h"

#if TARGET_HW_DESKTOP
#include "math.h"
#endif

#if TARGET_HW_RASPBERRYPI
#include "python_common.h"
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_RASPBERRYPI
static PyObject *pModule;      // nxp module                
static PyObject* pSensorObj;   // nxp.sensor object
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* These functions are the API for interfacing with the fxos8700 and 
* fxas21002c sensors.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: Init
     * Set up communication to fxos8700 and fxas21002c devices for receiving 
     * Accel/Gyro/Mag sensor data.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::Init() {
#if TARGET_HW_RASPBERRYPI
        return RasPi::Init();
#else
        return false;
#endif
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetAccelData
     * Get an sensor X,Y,Z reading from the NXP module. For this function, the results
     * will be accelerometer data.
     * 
     * INPUTS:
     * @sensor_data
     *      Reference to the structure that will contain the X,Y,Z sensor readings
     *      from the NXP module.
     * 
     * RETURN:
     * bool - True, reading successful and sensor_data now contains valid data from the NXP module.
     *        False, reading failed and sensor_data contents should be considered garbage.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::GetAccelData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetAccelData(sensor_data);
#elif TARGET_HW_DESKTOP
        return Dummy::GetAccelData(sensor_data);
#else
        return false;
#endif
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetGyroData
     * This function operates the same as GetAccelData except for gyroscope data.
     * To see a more detailed description/inputs/outputs, read the comment above
     * NxpWrapper::GetAccelData.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::GetGyroData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetGyroData(sensor_data);
#elif TARGET_HW_DESKTOP
        return Dummy::GetGyroData(sensor_data);
#else
        return false;
#endif
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetMagData
     * This function operates the same as GetAccelData except for magnetometer data.
     * To see a more detailed description/inputs/outputs, read the comment above
     * NxpWrapper::GetAccelData.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::GetMagData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetMagData(sensor_data);
#elif TARGET_HW_DESKTOP
        return Dummy::GetMagData(sensor_data);
#else
        return false;
#endif
    }

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* RASPBERRY PI SPECIFIC FUNCTIONS
* These functions interface the nxp.py module and implement the functionality
* described in the top level functions above.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_RASPBERRYPI

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: Init
     * Raspberry Pi implementation of NxpWrapper::Init.
     * High level information about this function can be found in the description
     * for NxpWrapper::Init.
     * 
     * NOTE: This function assumes that the Py_Initialize() function has already
     * been called.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::RasPi::Init(){
		
		printf("Entered RaspiInit\n");
        
        PyObject *pDict;    // nxp module's namespace
        PyObject *pSensor;  // nxp.sensor class  

        // Move PYTHONPATH to current working directory to find nxp module
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("import os");
        PyRun_SimpleString("sys.path.append(os.getcwd())");
       	
	    printf(NXP_MODULE);
	    printf("\n");

        // Load the module object (nxp.py)
        PythonAssert((pModule = PyImport_ImportModule(NXP_MODULE)));

        // Get functions from the module's namespace (PyObject.__dict__)
        PythonAssert((pDict = PyModule_GetDict(pModule)));
        
        // Get a handle to the nxp.sensor constructor
        PythonAssert((pSensor = PyDict_GetItemString(pDict, SENSOR_CLASS)));

        // Call the sensor constructor
        if (PyCallable_Check(pSensor)) 
            pSensorObj = PyObject_CallObject(pSensor, NULL);
        else 
            PythonAssert(NULL); // forces an assertion

        // Remove references to Python objects for garbage collection
        Py_DECREF(pDict);
        Py_DECREF(pSensor);

        return true;
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetAccelData
     * Raspberry Pi implementation of NxpWrapper::GetAccelData.
     * High level information about this function can be found in the description
     * for NxpWrapper::GetAccelData.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::RasPi::GetAccelData(SensorData_t& sensor_data){
        PyObject* rx = PyObject_CallMethod(pSensorObj, ACCEL_FUNC, NULL);
        if (!PyArg_ParseTuple(rx, "fff", &sensor_data.x, &sensor_data.y, &sensor_data.z))
        {
            printf("Failed to parse!!\n");
            return false;
        }
        return true;
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetGyroData
     * Raspberry Pi implementation of NxpWrapper::GetGyroData.
     * High level information about this function can be found in the description
     * for NxpWrapper::GetAccelGetGyroDataData.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::RasPi::GetGyroData(SensorData_t& sensor_data){
        PyObject* rx = PyObject_CallMethod(pSensorObj, GYRO_FUNC, NULL);
        if (!PyArg_ParseTuple(rx, "fff", &sensor_data.x, &sensor_data.y, &sensor_data.z))
        {
            printf("Failed to parse!!\n");
            return false;
        }
        return true;
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetMagData
     * Raspberry Pi implementation of NxpWrapper::GetMagData.
     * High level information about this function can be found in the description
     * for NxpWrapper::GetMagData.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::RasPi::GetMagData(SensorData_t& sensor_data){
        PyObject* rx = PyObject_CallMethod(pSensorObj, MAG_FUNC, NULL);
        if (!PyArg_ParseTuple(rx, "fff", &sensor_data.x, &sensor_data.y, &sensor_data.z))
        {
            printf("Failed to parse!!\n");
            return false;
        }
        return true;
    }
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DUMMY FUNCTIONS
* These functions output dummy samples for testing on targets that are not
* attached to the sensors.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_DESKTOP
    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetAccelData
     * Returns samples from a sine wave.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::Dummy::GetAccelData(SensorData_t& sensor_data){
        static int sampleNum = 0;
        float samplingRate = 46875.0f;
        float freqMult = 1.0f;
        float sample = sin(freqMult*2.0f*M_PI*(float)sampleNum/samplingRate);
        sensor_data = {sample, sample, sample};
        sampleNum++;
        return true;
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetGyroData
     * Returns samples from a sine wave.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::Dummy::GetGyroData(SensorData_t& sensor_data){
        static int sampleNum = 0;
        float samplingRate = 46875.0f;
        float freqMult = 2.0f;
        float sample = sin(freqMult*2.0f*M_PI*(float)sampleNum/samplingRate);
        sensor_data = {sample, sample, sample};
        sampleNum++;
        return true;
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: GetMagData
     * Returns samples from a 
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    bool NxpWrapper::Dummy::GetMagData(SensorData_t& sensor_data){
        static int sampleNum = 0;
        float samplingRate = 46875.0f;
        float freqMult = 4.0f;
        float sample = sin(freqMult*2.0f*M_PI*(float)sampleNum/samplingRate);
        sensor_data = {sample, sample, sample};
        sampleNum++;
        return true;
    }
#endif
