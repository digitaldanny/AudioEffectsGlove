#include "build_switches.h"
#if ENABLE_UNIT_TEST_WIRELESS_API

//#include "wireless_api.h"
#include "unit_tests.h"
#include "stdio.h"
#include "wireless_api.h"

int unitTest_wirelessApi() {
    printf("Hello! Running TEST_wireless_api.cpp\n");
    Py_Initialize(); // Initialize the Python Interpreter
    WirelessApi::ConnectToClient(9876);
    WirelessApi::DisconnectFromClient();
    Py_Finalize();
    return 0;
}

#endif // ENABLE_UNIT_TEST_WIRELESS_API