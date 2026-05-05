#include "clock.h"

void clockInit(void)
{
    
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}

    
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    
    RCC->CFGR &= ~(RCC_CFGR_HPRE |
                   RCC_CFGR_PPRE1 |
                   RCC_CFGR_PPRE2 |
                   RCC_CFGR_ADCPRE |
                   RCC_CFGR_PLLSRC |
                   RCC_CFGR_PLLXTPRE |
                   RCC_CFGR_PLLMULL);

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;     // AHB = 64 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;    // APB1 = 32 MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;    // APB2 = 64 MHz
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;    // ADC = 10.6667 MHz

    // 4) PLL source = HSI/2, PLL x16
    RCC->CFGR |= RCC_CFGR_PLLMULL16;     // x16

    
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}

    // 6) SYSCLK = PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    SystemCoreClockUpdate();
}