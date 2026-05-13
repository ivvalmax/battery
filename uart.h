#ifndef UART_H_
#define UART_H_

#include "common.h"
#include "/inc/stm32f10x_dma.h"
#include "/inc/stm32f10x_usart.h"



void uartInit(void);
void uartRecieveRestart(void);
void uartByteTransmit(char byte);
void uartStrTransmit(char* string);
void uartNumberTransmit(uint16_t number);
uint16_t calculateCRC16(const uint8_t *data, uint16_t length);
static void uartHandleRX(void);
static int uartParseFrame(const uint8_t *buf, uint8_t buf_len, uart_frame_t *out);
void uartSendFrame(uint8_t cmd, uint8_t *data, uint8_t data_len);
void uartSendDma(const uint8_t *data, uint16_t len);
	
void USART1_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);

#endif