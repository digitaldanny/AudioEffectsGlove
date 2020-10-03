#include "build_switches.h"
#if ENABLE_UNIT_TEST_WIRELESS_API

//#include "wireless_api.h"
#include "unit_tests.h"
#include "stdio.h"
#include "wireless_api.h"

#include "wiringPi.h"
#include "wiringSerial.h"
#include <termios.h>
#include <unistd.h>
#include <string.h>

int unitTest_wirelessApi() {

    int uartFd;
    int baudrate = 38400; 

    if (wiringPiSetup() < 0)
    {
        printf("**UART ERROR** - WiringPi setup failed\n");
        return -1;
    }

    if ((uartFd == serialOpen("/dev/serial0", baudrate)) < 0)
    {
        printf("**UART ERROR** - Unable to open UART device\n");
        return -1;
    }

    // Configure serial device 
    struct termios options;
    tcgetattr (uartFd, &options) ;  // Read current options
    options.c_cflag &= ~CSIZE ;     // Clear default character size settings
    options.c_cflag |= CS8 ;        // set character size to 8 bits
    options.c_cflag &= ~PARENB ;    // Disable parity 
    options.c_cflag &= ~CSTOPB;     // Enable single stop bit (clears flag that enables 2-bit stop)
    tcsetattr (uartFd, TCSANOW, &options) ;  // Set new options immediately

    char cmdVersionNo[] = "AT+VERSION?\r\n";
    char cmdSetAsSlave[] = "AT+ROLE=0\r\n";
    char* cmdList[] = {
        cmdVersionNo,
        cmdSetAsSlave
    };
    int cmdCount = 2;
    
    serialFlush(uartFd);

    for (int i = 0; i < cmdCount; i++)
    {
        printf("Sending command: %s.. len: %d\n", cmdList[i], strlen(cmdList[i]));
        serialPuts(uartFd, cmdList[i]);

        // Wait for response from device
        //printf("Waiting for response..\n");
        //while(serialDataAvail(uartFd) <= 0);
        
        for (int j = 0; j < 5; j++)
        {
            printf("Available: %d\n", serialDataAvail(uartFd));
            usleep(300000);
        }

        printf("Response: ");
        while (serialDataAvail(uartFd) > 0)
        {
            printf("%c", serialGetchar(uartFd));
        }
        printf("\n\n");
    }

    serialClose(uartFd);
    return 0;
}

#endif // ENABLE_UNIT_TEST_WIRELESS_API