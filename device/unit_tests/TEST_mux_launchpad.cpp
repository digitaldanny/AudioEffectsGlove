#include "build_switches.h"
#if ENABLE_UNIT_TEST_MUX_LP

#include "unit_tests.h"
#include "mux_if.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_mux
 * This test expects the user to halt on the SelectMuxChannel function and
 * modify the mux_channel variable in the "Variables" window. This allows
 * the user to manually control the mux only using the debugger.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_muxLp()
{
    int counter = 0;

    if (!Mux::Init())
    {
        /* Mux initialization failed */
        while(1);
    }

    while (1)
    {
        /// **** BREAKPOINT HERE ***** ///
        Mux::SelectMuxChannel(counter & MUX_CHAN_MAX); // <----
        /// **** BREAKPOINT HERE ***** ///

        counter++;
        counter &= MUX_CHAN_MAX;
    }
}

#endif // ENABLE_UNIT_TEST_MUX_LP
