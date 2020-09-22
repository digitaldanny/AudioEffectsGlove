/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: mux.cpp
 * This module configures the analog muxes connected to the flex sensors.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "mux.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_MUX_API_PYTHON
static PyObject *pModule;      // mux module                
static PyObject* pMuxObj;      // mux.Cd4051be object
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Init
 * Initialize GPIO to control the mux selects.
 * 
 * INPUTS: N/A
 * RETURN: N/A
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Mux::Init() {
#if ENABLE_MUX_API_PYTHON
    return Mux::Python::Init();
#else
    return false;
#endif // ENABLE_MUX_API_PYTHON
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: SelectMuxChannel
 * Get an sensor X,Y,Z reading from the NXP module. For this function, the results
 * will be accelerometer data.
 * 
 * INPUTS:
 * @mux_channel
 *      Integer value from 0-7 to configure the 3 mux select pins on the 8:1 analog
 *      muxes for reading flex sensor data.
 * 
 * RETURN:
 * bool - True if mux selection was successful.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Mux::SelectMuxChannel(int mux_channel) {
#if ENABLE_MUX_API_PYTHON
    return Mux::Python::SelectMuxChannel(mux_channel);
#else
    return false;
#endif // ENABLE_MUX_API_PYTHON
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PYTHON HANDLERS
* These functions are the C interface to the Python implementations. 
* For more details about what each function does, read the function
* descriptions in the "TOP LEVEL FUNCTIONS" section above.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#if ENABLE_MUX_API_PYTHON

bool Mux::Python::Init() {

    PyObject *pDict;    // nxp module's namespace
    PyObject *pSensor;  // nxp.sensor class  

    // Move PYTHONPATH to current working directory to find nxp module
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");

    // Load the module object
    PythonAssert((pModule = PyImport_ImportModule(MUX_MODULE)));

    // Get functions from the module's namespace (PyObject.__dict__)
    PythonAssert((pDict = PyModule_GetDict(pModule)));

    // Get a handle to the mux.Cd4051be class constructor
    PythonAssert((pSensor = PyDict_GetItemString(pDict, MUX_CLASS)));

    // Call the constructor
    if (PyCallable_Check(pSensor)) 
        pMuxObj = PyObject_CallObject(pSensor, NULL);
    else 
        PythonAssert(NULL); // forces an assertion

    // Initialize the GPIO using the mux.__enter__ function.
    PyObject_CallMethod(pMuxObj, MUX_INIT_FUNC, NULL);

    // Remove references to Python objects for garbage collection
    Py_DECREF(pDict);
    Py_DECREF(pSensor);
    return true;    
}

bool Mux::Python::SelectMuxChannel(int mux_channel) {
    PyObject* rx = PyObject_CallMethod(pMuxObj, MUX_SET_CH_FUNC, "i", mux_channel, NULL);
    if (rx == Py_False)
    {
        printf("Mux::Python::SelectMuxChannel - Mux select failed!!\n");
        return false;
    }
    return true;
}

#endif // ENABLE_MUX_API_PYTHON