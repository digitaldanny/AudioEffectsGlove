#ifndef _SRC_WIRELESS_API
#define _SRC_WIRELESS_API

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include <stdio.h>

// Prototype target includes
#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif

namespace WirelessApi {

    enum RequestType_e
    {
        REQ_INVALID = 0xDEADBEEF
    };

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    bool            ConnectToClient         ();
    bool            SendResponse            (char* data, int dataSizeInBytes);
    RequestType_e   RecvRequest             ();
    bool            DisconnectFromClient    ();

    /*
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    * DESCRIPTION: Python
    * The functions below are used when the server.py ServerSocket() class 
    * is being used for sending/receiving requests.
    * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
#if ENABLE_WIRELESS_API_PYTHON
    namespace Python{
        bool            ConnectToClient         ();
        bool            SendResponse            (char* data, int dataSizeInBytes);
        RequestType_e   RecvRequest             ();
        bool            DisconnectFromClient    ();
    }
#endif
}

#endif