#include "build_switches.h"
#if ENABLE_UNIT_TEST_FLEX_SENSORS

#include "unit_tests.h"
#include "flex_sensors_api.h"
#include <iostream>
#include <iomanip>

// Linux library with sleep function
#if TARGET_HW_RASPBERRYPI
#include "unistd.h"
#endif

using std::cout;
using std::cin;
using std::endl;

#define VOLTAGE_REF     (3.3f)
#define CLEAR_TERM()    printf("\033[2J"); \
                        printf("\033[%d;%dH", 0, 0)

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_flexSensors
 * This unit test infinitely updates terminal with ADC readings from all
 * 10 flex sensors.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_flexSensors() {

#if TARGET_HW_PYTHON_CAPABLE
    Py_Initialize(); // Initialize the Python Interpreter
#endif
    
    if (FlexSensors::Init() == true)
    {
        FlexSensors::finger_e fingersToEvaluate[] = {
            FlexSensors::FINGER_THUMB,
            FlexSensors::FINGER_POINTER,
            FlexSensors::FINGER_MIDDLE,
            FlexSensors::FINGER_RING,
            FlexSensors::FINGER_PINKY 
        };
        int lenFingersToEvaluate = sizeof(fingersToEvaluate)/sizeof(FlexSensors::finger_e);
        int numDecimalPlacesInReading = 2;
        FlexSensors::fingerJoints_t joints;

        while (true)
        {
            for (int i = 0; i < lenFingersToEvaluate; i++)
            {
                if (!FlexSensors::GetJointsData(fingersToEvaluate[i], &joints))
                {
                    cout << "Error while collecting joint data - Finger #" << fingersToEvaluate[i] << endl;
                }

                // Convert 10-bit adc readings to voltage value
                float mcp = VOLTAGE_REF*joints.mcp/1024;
                float pip = VOLTAGE_REF*joints.pip/1024;

                printf("MCP #%d: %.2f\n", fingersToEvaluate[i], mcp);
                printf("PIP #%d: %.2f\n\n", fingersToEvaluate[i], pip);
            }
            CLEAR_TERM();

#if TARGET_HW_RASPBERRYPI
            // Software delay for ~100 microseconds to avoid flickering in
            // terminal output.
            usleep(100000);
#endif
        }
    }

    // Currently will never reach this point..

#if TARGET_HW_PYTHON_CAPABLE
    Py_Finalize(); // Cleanly close the Python Interpreter
#endif

    cout << "SUCCESS: Flex sensor Unit Test complete." << endl;
    return 0;
}

#endif // ENABLE_UNIT_TEST_ADC