/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: mux.cpp
 * This module configures the analog muxes connected to the flex sensors.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "adc_wrapper.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* GLOBALS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_ADC_PYTHON
static PyObject *pModule;      // adc module                
static PyObject* pAdcObj;      // adc.Mcp3008 object
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* TOP LEVEL FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Init
 * Initialize GPIO for ADC captures.
 * 
 * INPUTS: N/A
 * RETURN: N/A
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Adc::Init() {
#if ENABLE_ADC_PYTHON
    return Adc::Python::Init();
#else
    return false;
#endif // ENABLE_ADC_API_PYTHON
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: ReadAdcChannel
 * Get a 10-bit ADC reading on the specified channel.
 * 
 * INPUTS:
 * @adc_channel
 *      Integer value from 0-7 to determine which of the 8 adc channels to 
 *      start the conversion and read from.
 * 
 * RETURN:
 * int - ADC reading will be a value from 0-1023. -1 means that the ADC capture failed.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
int Adc::ReadAdcChannel(int adc_channel) {
#if ENABLE_ADC_PYTHON
    return Adc::Python::ReadAdcChannel(adc_channel);
#else
    return false;
#endif // ENABLE_ADC_API_PYTHON
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PYTHON HANDLERS
* These functions are the C interface to the Python implementations. 
* For more details about what each function does, read the function
* descriptions in the "TOP LEVEL FUNCTIONS" section above.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#if ENABLE_ADC_PYTHON

bool Adc::Python::Init() {

    PyObject *pDict;    // adc module's namespace
    PyObject *pAdc;     // adc.Mcp3008 class  

    // Move PYTHONPATH to current working directory to find nxp module
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");

    // Load the module object
    PythonAssert((pModule = PyImport_ImportModule(ADC_MODULE)));

    // Get functions from the module's namespace (PyObject.__dict__)
    PythonAssert((pDict = PyModule_GetDict(pModule)));

    // Get a handle to the mux.Cd4051be class constructor
    PythonAssert((pAdc = PyDict_GetItemString(pDict, ADC_CLASS)));

    // Call the constructor
    if (PyCallable_Check(pAdc)) 
        pAdcObj = PyObject_CallObject(pAdc, NULL);
    else 
        PythonAssert(NULL); // forces an assertion

    // Initialize the GPIO using the mux.__enter__ function.
    PyObject_CallMethod(pAdcObj, ADC_INIT_FUNC, NULL);

    // Remove references to Python objects for garbage collection
    Py_DECREF(pDict);
    Py_DECREF(pAdc);
    return true;    
}

int Adc::Python::ReadAdcChannel(int adc_channel) {

    // Check that the Adc object has not been deallocated.
    if (!PythonAssert(pAdcObj))
    {
        printf("ADC Object does not exist anymore!!\n");
        return -1;
    }

    // Call the mcp3008_adc.Mcp3008.readAdc method
    PyObject* rx = PyObject_CallMethod(pAdcObj, ADC_READ_FUNC, "(i)", adc_channel);

    // Check if the ADC_READ function returned an ADC reading. If not, return an error (-1).
    if (rx == Py_None || !PythonAssert(rx))
    {
        printf("Adc::Python::ReadAdcChannel - Adc capture failed!!\n");
        Py_DECREF(rx);
        return -1;
    }
    else // Valid ADC reading captured. 
    {
        int adcReading = -1;
        int throwAway = -1;
        
        // Parsed received Python Object as an integer
        if (!PyArg_ParseTuple(rx, "ii", &adcReading, &throwAway)) 
        {
            printf("Adc::Python::ReadAdcChannel - Could not parse the adc reading. Is device power on?\n");
            Py_DECREF(rx);
            return -1;
        }

        Py_DECREF(rx);
        return adcReading;
    }
}
#endif // ENABLE_ADC_PYTHON
