#include "adc.h"

void adcInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_ADC1EN
									|RCC_APB2ENR_IOPCEN|RCC_APB2ENR_AFIOEN;
	RCC->AHBENR  |= RCC_AHBENR_DMA1EN;
	
	
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE4 | GPIO_CRL_CNF4); // i_out + v_bat port init
	
	GPIOC->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1|
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2|
                    GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
	
	DMA1_Channel1->CCR &= ~DMA_CCR1_EN;

	DMA1_Channel1->CPAR  = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR  = (uint32_t)adc_buff;
	DMA1_Channel1->CNDTR = ADC_BUFF_SIZE;
	
	DMA1_Channel1->CCR =
        DMA_CCR1_MINC |
        DMA_CCR1_CIRC |
        DMA_CCR1_MSIZE_0 |
        DMA_CCR1_PSIZE_0 |
        DMA_CCR1_PL_1;
				
	DMA1_Channel1->CCR |= DMA_CCR1_TCIE | DMA_CCR1_HTIE;
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  DMA1_Channel1->CCR |= DMA_CCR1_EN;
}