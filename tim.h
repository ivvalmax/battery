#ifndef TIM_H_
#define TIM_H_

#include "common.h"
#include "stm32f10x_tim.h"

volatile uint8_t adc_process_flag = 0;

void timInit(void);
void TIM1_UP_IRQHandler(void);


#endif 