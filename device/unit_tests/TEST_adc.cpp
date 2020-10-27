#include "build_switches.h"
#if ENABLE_UNIT_TEST_ADC

#include "unit_tests.h"
#include "adc.h"
#include <iostream>
#include <iomanip>

using std::cout;
using std::cin;
using std::endl;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_adc
 * This unit test allows the user to select which adc channel (0-7) to 
 * read from. To exit the program, enter -1.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_adc() {

    Py_Initialize(); // Initialize the Python Interpreter
    
    if (Adc::Init() == true)
    {
        int adcChannel = 0;
        int adcReading = -1;
        int numDecimalPlacesInReading = 2;

        while (true)
        {
            cout << "Select a channel (-1 to exit unit test): $";
            cin >> adcChannel;

            if (adcChannel >= 0)
            {
                if ((adcReading = Adc::ReadAdcChannel(adcChannel)) > -1)
                {
                    cout << "Adc channel: " << adcChannel << " - ";
                    cout << 3.3f*adcReading/1024 << std::setprecision(numDecimalPlacesInReading) << "V" << endl;
                }
                else
                {
                    cout << "Adc read on channel " << adcChannel << " failed." << endl;
                }
            }
            else
            {
                break;
            }
        }
    }

    Py_Finalize(); // Cleanly close the Python Interpreter

    cout << "Adc Unit Test complete." << endl;
    return 0;
}

#endif // ENABLE_UNIT_TEST_ADC
