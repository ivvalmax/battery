#ifndef _UART_H
#define _UART_H

#include "common.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"

void uartInit(void);
void uartByteTransmit(char byte);
void uartStrTransmit(char* string);
void uartNumberTransmit(uint16_t number);

#endif