#include "wireless_api.h"

bool WirelessApi::ConnectToClient()
{
#if ENABLE_WIRELESS_API_PYTHON
    return Python::ConnectToClient();
#else
    return false;
#endif
}

bool WirelessApi::SendResponse(char* data, int dataSizeInBytes)
{
#if ENABLE_WIRELESS_API_PYTHON
    return Python::SendResponse(data, dataSizeInBytes);
#else
    return false;
#endif
}

WirelessApi::RequestType_e WirelessApi::RecvRequest()
{
#if ENABLE_WIRELESS_API_PYTHON
    return Python::RecvRequest();
#else
    return REQ_INVALID;
#endif
}

bool WirelessApi::DisconnectFromClient()
{
#if ENABLE_WIRELESS_API_PYTHON
    return Python::DisconnectFromClient();
#else
    return false;
#endif
}

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PYTHON HANDLERS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_WIRELESS_API_PYTHON
bool WirelessApi::Python::ConnectToClient()
{
    printf("WirelessApi::Python::ConnectToClient\n");
    return true;
}

bool WirelessApi::Python::SendResponse(char* data, int dataSizeInBytes)
{
    printf("WirelessApi::Python::SendResponse\n");
    return true;
}

WirelessApi::RequestType_e WirelessApi::Python::RecvRequest()
{
    printf("WirelessApi::Python::RecvRequest\n");
    return REQ_INVALID;
}

bool WirelessApi::Python::DisconnectFromClient()
{
    printf("WirelessApi::Python::DisconnectFromClient\n");
    return true;
}

#endif