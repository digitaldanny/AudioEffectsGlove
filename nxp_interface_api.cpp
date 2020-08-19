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

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_RASPBERRYPI
PyObject *pModule;      // nxp module                
PyObject* pSensorObj;   // nxp.sensor object
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* These functions are the API for interfacing with the fxos8700 and 
* fxas21002c sensors.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

    bool NxpWrapper::Init() {
#if TARGET_HW_RASPBERRYPI
        return RasPi::Init();
#else
        return false;
#endif
    }

    bool NxpWrapper::GetAccelData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetAccelData(sensor_data);
#else
        return false;
#endif
    }

    bool NxpWrapper::GetGyroData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetGyroData(sensor_data);
#else
        return false;
#endif
    }

    bool NxpWrapper::GetMagData(SensorData_t& sensor_data) {
#if TARGET_HW_RASPBERRYPI
        return RasPi::GetMagData(sensor_data);
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

    bool NxpWrapper::RasPi::Init(){
        
        PyObject *pDict;    // nxp module's namespace
        PyObject *pSensor;  // nxp.sensor class    
        
        // Initialize the Python Interpreter
        Py_Initialize();

        // Move PYTHONPATH to current working directory to find nxp module
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("import os");
        PyRun_SimpleString("sys.path.append(os.getcwd())");
        
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
    bool NxpWrapper::RasPi::GetAccelData(SensorData_t& sensor_data){
        PyObject_CallMethod(pSensorObj, ACCEL_FUNC, NULL);
        return true;
    }
    bool NxpWrapper::RasPi::GetGyroData(SensorData_t& sensor_data){
        PyObject_CallMethod(pSensorObj, GYRO_FUNC, NULL);
        return true;
    }
    bool NxpWrapper::RasPi::GetMagData(SensorData_t& sensor_data){
        PyObject_CallMethod(pSensorObj, MAG_FUNC, NULL);
        return true;
    }

    bool NxpWrapper::RasPi::PythonAssert(PyObject* obj)
    {
        if (obj == NULL)
        {
            // Print out the error and exit
            PyObject *ptype, *pvalue, *ptraceback;      
            PyErr_Fetch(&ptype, &pvalue, &ptraceback);
            const char *pStrErrorMessage = PyUnicode_AsUTF8(pvalue);
            printf("NxpWrapper::RasPi::PythonAssert - %s\n", pStrErrorMessage);
            exit(-1); 
        }
        return true;
    }
#endif

int main()
{
    NxpWrapper::SensorData_t accelData;
    printf("TARGET_HW_RASBERRYPI: %d\n", TARGET_HW_RASPBERRYPI);
    printf("nxp.init = %d\n", NxpWrapper::Init());
    NxpWrapper::GetAccelData(accelData);
    NxpWrapper::GetMagData(accelData);
    NxpWrapper::GetGyroData(accelData);
    return 0;
}
