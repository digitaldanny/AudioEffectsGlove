/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "wireless_api.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
static PyObject* pModule;      // server module                
static PyObject* pSocketObj;   // server.ServerSocket object
#endif

bool WirelessApi::ConnectToClient(int port_num)
{
#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
    return Python::ConnectToClient(port_num);
#else
    return false;
#endif
}

bool WirelessApi::SendResponse(char* data, int dataSizeInBytes)
{
#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
    return Python::SendResponse(data, dataSizeInBytes);
#else
    return false;
#endif
}

WirelessApi::RequestType_e WirelessApi::RecvRequest()
{
#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
    return Python::RecvRequest();
#else
    return REQ_INVALID;
#endif
}

bool WirelessApi::DisconnectFromClient()
{
#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
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

#if ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE
bool WirelessApi::Python::ConnectToClient(int port_num)
{
    printf("WirelessApi::Python::ConnectToClient\n");

    PyObject *pDict;    // server.py module's namespace
    PyObject *pSocket;  // server.ServerSocket class    

    // Move PYTHONPATH to current working directory to find nxp module
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    
    // Load the module object (nxp.py)
    PythonAssert((pModule = PyImport_ImportModule(SERVER_MODULE)));

    // Get functions from the module's namespace (PyObject.__dict__)
    PythonAssert((pDict = PyModule_GetDict(pModule)));
    
    // Get a handle to the nxp.sensor constructor
    PythonAssert((pSocket = PyDict_GetItemString(pDict, SOCKET_CLASS)));

    // Call the sensor constructor
    if (PyCallable_Check(pSocket)) 
    {
        PyObject* portNumObj = Py_BuildValue("i", port_num);
        pSocketObj = PyObject_CallObject(pSocket, portNumObj);
    }
    else 
        PythonAssert(NULL); // forces an assertion

    // Remove references to Python objects for garbage collection
    //Py_DECREF(pDict);
    //Py_DECREF(pSocket);

    // Call the connect to socket function to search for a connection
    // with a client.
    printf("C: Listening for connection to client.\n");
    PyObject* conn = PyObject_CallMethod(pSocketObj, OPEN_CONN_FUNC, NULL);
    if (conn == Py_None)
    {
        printf("WirelessApi::Python::ConnectToClient - Could not connect to a client.\n");
        return false;
    }
    else
    {
        printf("WirelessApi::Python::ConnectToClient - Connected successfully.\n");
        return true;
    }
    
}

bool WirelessApi::Python::SendResponse(char* data, int dataSizeInBytes)
{
    printf("WirelessApi::Python::SendResponse\n");
    PyObject* rx = PyObject_CallMethod(pSocketObj, SEND_RESP_FUNC, NULL);
    if (!PyBool_Check(rx))
    {
        printf("WirelessApi::Python::SendResponse - Failed to send!!\n");
        return false;
    }
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
    PyObject_CallMethod(pSocketObj, DISCONNECT_FUNC, NULL);
    Py_DECREF(pModule);
    //Py_DECREF(pSocketObj);
    return true;
}

#endif // ENABLE_WIRELESS_API_PYTHON & TARGET_HW_PYTHON_CAPABLE