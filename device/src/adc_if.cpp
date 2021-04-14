#include "build_switches.h"
#include "target_hw_common.h"
#include "adc_if.h"

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

#if TARGET_HW_MSP432
static volatile uint16_t adcReading[NUM_ADC_CONVERSIONS_MAX];
static volatile bool flagAdcReady;
#endif // TARGET_HW_MSP432

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
#elif TARGET_HW_MSP432
    return Adc::MSP432::Init();
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
uint16_t Adc::ReadAdcChannel(uint8_t adc_channel) {
#if ENABLE_ADC_PYTHON
    return Adc::Python::ReadAdcChannel(adc_channel);
#elif ENABLE_ADC_C2000
    return Adc::C2000::ReadAdcChannel(adc_channel);
#elif TARGET_HW_MSP432
    return Adc::MSP432::ReadAdcChannel(adc_channel);
#else
    return false;
#endif // ENABLE_ADC_API_PYTHON
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* MSP432 HANDLERS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_MSP432
bool Adc::MSP432::Init() {

    /* Initializing ADC (MCLK/1/1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);

    /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(systemIO.flexAdc.port,
        systemIO.flexAdc.pin | systemIO.ocvAdc.pin | systemIO.spareAdc.pin | systemIO.ccvAdc.pin,
        GPIO_TERTIARY_MODULE_FUNCTION);

    // Configuring ADC Memory
    // ADC_MEM0 - Flex sensors
    // ADC_MEM1 - UNUSED
    // ADC_MEM2 - Closed Circuit Voltage
    // ADC_MEM3 - Open Circuit Voltage
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM0,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A1, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM2,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A2, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM3,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A3, false);

    /* Enabling the interrupt when a conversion on channel 7 (end of sequence)
     *  is complete and enabling conversions */
    MAP_ADC14_enableInterrupt(ADC_INT3);

    /* Enabling Interrupts */
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();

    /* After triggering conversion, ADC channel automatically increments for conversion */
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    MAP_ADC14_enableConversion();
    return true;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: ReadAdcChannel
 * This function triggers the ADC conversion on all 4 channels and returns
 * the requested channel's value.
 *
 * NOTE - This function is not efficient when reading from all 4 channels
 * sequentially since it will trigger a new conversion each time.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
uint16_t Adc::MSP432::ReadAdcChannel(uint8_t adc_channel) {

    // Clear conversion ready flag
    flagAdcReady = false;

    // Trigger ADC conversion
    MAP_ADC14_toggleConversionTrigger();

    // Wait until the ADC interrupt sets the adc conversion flag
    // before returning the captured adc value.
    while (!flagAdcReady)
    {
        ; //MAP_PCM_gotoLPM0();
    }
    return adcReading[adc_channel];
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: ADC14_IRQHandler
 * This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM3. This signals the end of conversion and the results array is
 * grabbed and placed in resultsBuffer
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 */
extern "C" void ADC14_IRQHandler(void)
{
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if (ADC_INT3 & status)
    {
        adcReading[0] = MAP_ADC14_getResult(ADC_MEM0);
        adcReading[1] = MAP_ADC14_getResult(ADC_MEM1);
        adcReading[2] = MAP_ADC14_getResult(ADC_MEM2);
        adcReading[3] = MAP_ADC14_getResult(ADC_MEM3);
        flagAdcReady = true;
    }
}
#endif // TARGET_HW_MSP432

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* C2000 HANDLERS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_ADC_C2000

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Adc<A/B>ConvComplete_ISR
 * These interrupt read the ADC value after conversions are complete and store
 * them into a global to be accessed by the ReadAdcChannel function.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
__interrupt static void AdcAConvComplete_ISR() {

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
    return;
}

__interrupt static void AdcBConvComplete_ISR() {

    // Read the raw sample value
    adcData[ADC_B].value = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    adcData[ADC_B].capturesSinceLastRead++;

    // Clear the interrupt flag
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    // Check if overflow has occurred
    if(true == ADC_getInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
    }

    // Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    return;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: ConfigAdc<a/b>Epwm<1/2>
 * These functions are used to configure a PWM module to trigger an ADC conversion
 * without using software to trigger the conversion manually.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
static void ConfigAdcaEpwm1()
{
    // Point the interrupt vector at the interrupt handler.
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
}

static void ConfigAdcbEpwm2()
{
    // Point the interrupt vector at the interrupt handler.
    Interrupt_register(INT_ADCB1, &AdcBConvComplete_ISR);

    // Configure ADCB module
    // - Set ADCCLK divider to /4
    // - Set resolution and signal mode and load corresponding trims.
    // - Set pulse positions to late
    // - Power up the ADC and then delay for 1 ms
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCB_BASE);
    DEVICE_DELAY_US(1000);

    // Enable EPWM to generate the ADC conversions without software interaction.
    // - Disable SOCB
    // - Configure the SOC to occur on the first up-count event
    // - Set the compare A value to 2048 and the period to 4096
    // - Freeze the counter
    EPWM_disableADCTrigger(EPWM2_BASE, EPWM_SOC_B);
    EPWM_setADCTriggerSource(EPWM2_BASE, EPWM_SOC_B, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM2_BASE, EPWM_SOC_B, 1);
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, 0x0800);
    EPWM_setTimeBasePeriod(EPWM2_BASE, 0x1000);
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    // Configure ADCB's SOC0 to be triggered by EPWM1
    // - Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    //   sure its flag is cleared.
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCB, ADC_CH_ADCIN2, 15);
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    // Start ePWM1, enabling SOCB and putting the counter in up-count mode
    EPWM_enableADCTrigger(EPWM2_BASE, EPWM_SOC_B);
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP);
    Interrupt_enable(INT_ADCB1);
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Init
 * Set up ADC channels for capturing MCP and PIP joint values. Both ADCs are
 * triggered with EPWM hardware so that a software trigger is not required.
 *
 * EPWM1 triggers ADCA
 * EPWM2 triggers ADCB
 *
 * RETURN:
 * bool - true if HW configuration was successful.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool Adc::C2000::Init() {

    // Initialize the global adcData values to be 0.
    memset((void*) (&adcData), 0, ADC_MAX_NUM_CHAN * sizeof(adcData_t));

    // Configure ADC and EPWM modules for converting ADC values automatically.
    ConfigAdcaEpwm1();
    ConfigAdcbEpwm2();

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
