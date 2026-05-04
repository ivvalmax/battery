#include "uart.h"

/**
  * @brief  Init USART1, PORTA, DMA
  * @param  None
  * @retval None
  */
void uartInit(void)
{
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;//port rcc
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN | RCC_APB2ENR_USART1EN; //UART1 & UART3 rcc
		RCC->AHBENR  |= RCC_AHBENR_DMA1EN;//DMA init
		RCC->APB1ENR |= RCC_APB1ENR_USART1EN;
	
		// port init
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 |
                    GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    GPIOA->CRH |= GPIO_CRH_CNF10_0;
		
		// 8N1, oversampling by 16
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
		// 64MHz, 115200 baud
    USART1->BRR = (22 << USART_BRR_DIV_Mantissa)|(12 << USART_BRR_DIV_Fraction);

		USART1->CR1 &= ~(USART_CR1_M| USART_CR1_PCE);
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
		
		USART1->CR2 &= ~USART_CR2_STOP;
		// DMA init
		USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
		
}	


/**
  * @brief  Transmit 1 byte of data without DMA
  * @param  byte - data to transmite
  * @retval None
  */
void uartByteTransmit(char byte)
{
	USART1->DR = byte;
	while(!(USART1->SR & USART_SR_TXE)){};
		
}

void uartStrTransmit(char* string)
{
	while(*string)
	{
		uartByteTransmit(*string++);
	}
}	

void uartNumberTransmit(uint16_t number)
{
	uint8_t d[5] = {0, 0, 0, 0, 0};
	for(uint8_t i = 0; i < 5; i++)
	{
		d[i] = number%10;
		number /= 10;
	}
	
	for(uint8_t i = 0; i < 5; i++)
	{
		uartByteTransmit(d[4-i]+'0');
	}
	
	uartStrTransmit("\r\n");
}
