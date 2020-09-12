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

#if ENABLE_WIRELESS_API_PYTHON
#include "python_common.h"

#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif // ENABLE_WIRELESS_API_PYTHON

#if TARGET_HW_DESKTOP
#include "python3.6/Python.h"
#endif // TARGET_HW_DESKTOP
#endif // ENABLE_WIRELESS_API_PYTHON

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_WIRELESS_API_PYTHON
#define SERVER_MODULE   PROTO_PATH "server"
#define SOCKET_CLASS    "ServerSocket"
#define RECV_REQ_FUNC   "recvRequest"
#define SEND_RESP_FUNC  "sendResponse"
#define CONNECT_FUNC    "__enter__"
#define DISCONNECT_FUNC "__exit__"
#endif // ENABLE_WIRELESS_API_PYTHON

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

    bool            ConnectToClient         (int port_num);
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
        bool            ConnectToClient         (int port_num);
        bool            SendResponse            (char* data, int dataSizeInBytes);
        RequestType_e   RecvRequest             ();
        bool            DisconnectFromClient    ();
    }
#endif
}

#endif