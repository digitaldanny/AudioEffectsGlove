/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: uart_if.cpp
 * This module configures the uart module to send/receive data to the
 * RS232 Bluetooth chip.
 *
 * RX -> GPIO 139
 * TX -> GPIO 56
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "uart_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* FUNCTIONS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool Uart::Init() {
    return false;
}

#if ENABLE_UART_C2000
bool Uart::C2000::Init() {
    return true;
}
#endif // ENABLE_UART_C2000
