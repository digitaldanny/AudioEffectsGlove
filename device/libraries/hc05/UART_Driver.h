#ifndef HARDWARE_UART_DRIVER_H_
#define HARDWARE_UART_DRIVER_H_

#include "target_hw_common.h"

#define UARTA0_BUFFERSIZE 2048
#define UARTA2_BUFFERSIZE 2048

void UART_Init(uint32_t UART, eUSCI_UART_Config UARTConfig);
void UART_Write(uint32_t UART, uint8_t *Data, uint32_t Size);
uint32_t UART_Read(uint32_t UART, uint8_t *Data, uint32_t Size);
uint32_t UART_Available(uint32_t UART);
void UART_Flush(uint32_t UART);

#endif /* HARDWARE_UART_DRIVER_H_ */
