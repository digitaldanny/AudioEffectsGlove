#include "build_switches.h"
#if ENABLE_UNIT_TEST_MUX

#include "unit_tests.h"
#include "mux.h"
#include <iostream>

using std::cout;
using std::cin;
using std::endl;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_mux
 * This unit test allows the user to select the mux channel (0-7) through 
 * command line. To exit the program, enter -1.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_mux() {

    Py_Initialize(); // Initialize the Python Interpreter
    
    if (Mux::Init() == true)
    {
        int muxChannel = 0;

        while (true)
        {
            cout << "Select a channel (-1 to exit unit test): $";
            cin >> muxChannel;

            if (muxChannel >= 0)
            {
                if (Mux::SelectMuxChannel(muxChannel))
                    cout << "Setting mux channel " << muxChannel << " was successful." << endl;
                else
                    cout << "Setting mux channel " << muxChannel << " failed." << endl;
            }
            else
            {
                break;
            }
        }
    }

    Py_Finalize(); // Cleanly close the Python Interpreter

    cout << "Mux Unit Test complete." << endl;
    return 0;
}

#endif // ENABLE_UNIT_TEST_MUX
