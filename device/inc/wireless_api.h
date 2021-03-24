#ifndef _SRC_WIRELESS_API
#define _SRC_WIRELESS_API

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include "target_hw_common.h"
#include <stdio.h>

#if TARGET_HW_MSP432
#include "uart_if.h"
#endif // TARGET_HW_MSP432

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

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* ENUMS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if TARGET_HW_MSP432
typedef enum
{
    HC05MODE_DATA,
    HC05MODE_CMD
} hc05Mode;
#endif // TARGET_HW_MSP432

typedef enum
{
    REQ_INVALID = 0xDEADBEEF
} RequestType_e;

namespace WirelessApi {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    bool            Init                    ();
    bool            ConnectToClient         (uint16_t port_num);
    bool            SendResponse            (char* data, int dataSizeInBytes);
    RequestType_e   RecvRequest             ();
    bool            DisconnectFromClient    ();

#if TARGET_HW_MSP432
    namespace MSP432{
        bool            SetMode                 (hc05Mode mode);
        //bool            ConnectToClient         (int port_num);
        //bool            SendResponse            (char* data, int dataSizeInBytes);
        //RequestType_e   RecvRequest             ();
        //bool            DisconnectFromClient    ();
    }
#endif

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
