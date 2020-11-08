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

#if ENABLE_ADC_C2000
volatile adcData_t adcData[ADC_MAX_NUM_CHAN];
#endif // ENABLE_ADC_C2000

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
#elif ENABLE_ADC_C2000
    return Adc::C2000::Init();
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
#elif ENABLE_ADC_C2000
    return Adc::C2000::ReadAdcChannel(adc_channel);
#else
    return false;
#endif // ENABLE_ADC_API_PYTHON
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* C2000 HANDLERS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_ADC_C2000

__interrupt static void AdcAConvComplete_ISR() {

    // Enable modifying/reading protected registers
    EALLOW;

    // Read the raw sample value
    adcData[ADC_A].value = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcData[ADC_A].capturesSinceLastRead++;

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    // Check if overflow has occurred
    if(true == ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    }

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

    // Disable modifying and reading protected registers
    EDIS;
    return;
}

__interrupt static void AdcBConvComplete_ISR() {

    // Enable modifying/reading protected registers
    EALLOW;

    //adcReading[ADC_B] =

    // Disable modifying and reading protected registers
    EDIS;
    return;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Init
 * Set up ADC channels for capturing MCP and PIP joint values.
 *
 * RETURN:
 * bool - true if HW configuration was successful.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Adc::C2000::Init() {

    // Initialize the global adcData values to be 0.
    memset((void*)&adcData, 0, ADC_MAX_NUM_CHAN*sizeof(adcData_t));

    // Point the interrupt vector at the custom function name.
    Interrupt_register(INT_ADCA1, &AdcAConvComplete_ISR);

    // Configure ADCA module
    // - Set ADCCLK divider to /4
    // - Set resolution and signal mode and load corresponding trims.
    // - Set pulse positions to late
    // - Power up the ADC and then delay for 1 ms

    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    // Enable EPWM to generate the ADC conversions without software interaction.
    // - Disable SOCA
    // - Configure the SOC to occur on the first up-count event
    // - Set the compare A value to 2048 and the period to 4096
    // - Freeze the counter

    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0x0800);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 0x1000);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    // Configure ADCA's SOC0 to be triggered by EPWM1
    // - Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    //   sure its flag is cleared.

    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, 15);
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    // Start ePWM1, enabling SOCA and putting the counter in up-count mode

    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    Interrupt_enable(INT_ADCA1);

    return true;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: ReadAdcChannel
 * Set up ADC channels for capturing MCP and PIP joint values.
 *
 * RETURN:
 * int - Returns the most recent ADC reading if succesful. Returns -1 if failing.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
int Adc::C2000::ReadAdcChannel(int adc_channel) {

    if (adc_channel < 0 || adc_channel > ADC_MAX_NUM_CHAN-1)
    {
        /* provided adc_channel was outside of expected range */
        return -1;
    }

    // Check if enough ADC readings have been captured to return an ADC reading.
    // This reduces the possibility of a mux channel switch causing incorrect ADC values
    // to be read for the specified channel.
    if (adcData[adc_channel].capturesSinceLastRead < ADC_NUM_READINGS_BEFORE_VALID)
        return -1;

    // Reset adc reading counter and return the most recent adc reading.
    adcData[adc_channel].capturesSinceLastRead = 0;
    return (int)adcData[adc_channel].value;
}

#endif // ENABLE_ADC_C2000

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
