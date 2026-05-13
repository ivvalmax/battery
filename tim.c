#include "tim.h"

void timInit()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM2->PSC = 63;
	TIM2->ARR = 99;
	TIM2->CNT = 0;
	TIM2->CR2 = 0;
	TIM2->CR2 |= TIM_CR1_CEN|TIM_CR2_MMS_1;
	TIM2->EGR |= TIM_EGR_UG;
	
	TIM1->PSC = 63;
	TIM1->ARR = 9999;
	TIM1->CNT = 0;
	TIM1->CR2 = 0;
	TIM1->CR1 |= TIM_CR1_CEN|TIM_CR1_ARPE;
	TIM1->CR2 |= TIM_CR2_UG;
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM1->SR & TIM_SR_UIF)
	{
		TIM1->SR &= ~TIM_SR_UIF;
		adc_process_flag = 1;
	}
}