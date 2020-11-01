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

#if ENABLE_MUX_PYTHON
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
#if ENABLE_MUX_PYTHON
    return Mux::Python::Init();
#elif ENABLE_MUX_C2000
    return Mux::C2000::Init();
#else
    return false;
#endif // ENABLE_MUX_PYTHON
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: SelectMuxChannel
 * Select the mux channel for MCP and PIP muxes. This allows a different finger's joint
 * voltages to be output from the MCP and PIP muxes.
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
#if ENABLE_MUX_PYTHON
    return Mux::Python::SelectMuxChannel(mux_channel);
#elif ENABLE_MUX_C2000
    return Mux::C2000::SelectMuxChannel(mux_channel);
#else
    return false;
#endif // ENABLE_MUX_PYTHON
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* C2000 HANDLERS
* These functions are the C2000 dev board specific implementations of mux.cpp.
* For more details about what each function does, read the function
* descriptions in the "TOP LEVEL FUNCTIONS" section above.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_MUX_C2000

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: Init
 * This function initializes GPIO 2:0 as outputs for select
 * line control on the MCP mux (CD4051BE IC).
 *
 * PINOUT:
 * Gpio 0 -> Mux select 0
 * Gpio 1 -> Mux select 1
 * Gpio 2 -> Mux select 2
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
bool Mux::C2000::Init()
{
    for (unsigned int i = 0; i <= 2; i++)
    {
        GPIO_setDirectionMode(i, GPIO_DIR_MODE_OUT);    // leds set as outputs
        GPIO_setPadConfig(i, GPIO_PIN_TYPE_PULLUP);     // internal pullups enabled
        GPIO_setPinConfig(GPIO_0_GPIO0 + i*0x200);      // set pin's peripheral config mux to be used as a GPIO
    }
    return true;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * SUMMARY: SelectMuxChannel
 * This function's description is the same as the top level
 * function's description.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
bool Mux::C2000::SelectMuxChannel(int mux_channel) {

    // Check that the provided mux channel is within the valid channel range.
    if (mux_channel < MUX_CHAN_MIN || mux_channel > MUX_CHAN_MAX)
    {
        /* provided mux channel is outside of valid channel range */
        return false;
    }

    //
    // read, modify, write to port so other port functionality is not ruined.
    //

    // Read port A's current pin values
    uint32_t portData = GPIO_readPortData(GPIO_PORT_A);

    // Modify port A's pins to reflect mux channel changes
    portData &= 0xFFFFFFF8;     // preserves data from bits 31:3, while clearing bits 2:0
    portData |= mux_channel;    // adds mux channel values to the output port

    // Write port A's pin values back out to the GPIO
    GPIO_writePortData(GPIO_PORT_A, mux_channel); // writes low-true data to LEDs

    return true;
}

#endif // ENABLE_MUX_C2000

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PYTHON HANDLERS
* These functions are the C interface to the Python implementations. 
* For more details about what each function does, read the function
* descriptions in the "TOP LEVEL FUNCTIONS" section above.
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#if ENABLE_MUX_PYTHON

bool Mux::Python::Init() {

    PyObject *pDict;    // mux module's namespace
    PyObject *pMux;  // mux.Cd4051be class  

    // Move PYTHONPATH to current working directory to find nxp module
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");

    // Load the module object
    PythonAssert((pModule = PyImport_ImportModule(MUX_MODULE)));

    // Get functions from the module's namespace (PyObject.__dict__)
    PythonAssert((pDict = PyModule_GetDict(pModule)));

    // Get a handle to the mux.Cd4051be class constructor
    PythonAssert((pMux = PyDict_GetItemString(pDict, MUX_CLASS)));

    // Call the constructor
    if (PyCallable_Check(pMux)) 
        pMuxObj = PyObject_CallObject(pMux, NULL);
    else 
        PythonAssert(NULL); // forces an assertion

    // Initialize the GPIO using the mux.__enter__ function.
    PyObject_CallMethod(pMuxObj, MUX_INIT_FUNC, NULL);

    // Remove references to Python objects for garbage collection
    Py_DECREF(pDict);
    Py_DECREF(pMux);
    return true;    
}

bool Mux::Python::SelectMuxChannel(int mux_channel) {
    PyObject* rx = PyObject_CallMethod(pMuxObj, MUX_SET_CH_FUNC, "i", mux_channel, NULL);
    if (rx == Py_False)
    {
        printf("Mux::Python::SelectMuxChannel - Mux select failed!!\n");
        Py_DECREF(rx);
        return false;
    }
    Py_DECREF(rx);
    return true;
}

#endif // ENABLE_MUX_PYTHON
