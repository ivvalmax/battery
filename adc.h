#ifndef ADC_H_
#define ADC_H_

#include "common.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "tim.h"

#define ADC_BUFF_SIZE 200*6

uint8_t adc_buff[ADC_BUFF_SIZE] = {0};

void adcInit(void);


#endif