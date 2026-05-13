#include "uart.h"

/**
  * @brief  Init USART1, PORTA, DMA
  * @param  None
  * @retval None
  */
void uartInit(void)
{
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;//port rcc
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //UART1 & UART3 rcc
		RCC->AHBENR  |= RCC_AHBENR_DMA1EN;//DMA init
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
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
    USART1->BRR = (22 << 4)|(12 << 0);

		USART1->CR1 &= ~(USART_CR1_M| USART_CR1_PCE);
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE | USART_CR1_IDLEIE;
		NVIC_EnableIRQ(USART1_IRQn);
		
		USART1->CR2 &= ~USART_CR2_STOP;
		// DMA init
		USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
		
		
		DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
    DMA1_Channel5->CCR = DMA_CCR5_MINC | DMA_CCR5_TCIE | DMA_CCR5_TEIE| DMA_CCR5_CIRC;  // mem incr, TC interrupt
    DMA1_Channel5->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR = (uint32_t)rx_buff;
    DMA1_Channel5->CNDTR = RX_BUFF_SIZE;
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
		
		DMA1_Channel5->CCR |= DMA_CCR5_EN;
}	

void uartRecieveRestart(void)
{
    DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
    DMA1_Channel5->CNDTR = RX_BUFF_SIZE;
	  DMA1_Channel5->CMAR = (uint32_t)rx_buff;
    DMA1_Channel5->CCR |= DMA_CCR5_EN;
		rx_old_pos = 0;
}
/**
  * @brief  Transmit 1 byte of data without DMA
  * @param  byte - data to transmite
  * @retval None
  */
void uartByteTransmit(char byte)
{
	USART1->DR = byte;
	while(!(USART1->SR & USART_SR_TXE));
		
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

static void uartHandleRX(void)
{
    uint8_t pos = (RX_BUFF_SIZE - DMA1_Channel5->CNDTR);
    if (pos == rx_old_pos)
        return;

    if (pos > rx_old_pos)
    {
        uart_frame_t frame;
        if (uartParseFrame(&rx_buff[rx_old_pos], (pos - rx_old_pos), &frame) == 0)
        {
            frame_ready = 1;
            if (frame.cmd == 0x10)
            {
//frame transmit
            }
						if(frame.cmd == 0x02)
						{
						// graseful delay time set
						}
						if(frame.cmd == 0x03)
						{
						// error reset
							rx_error = 0;
						}
						if(frame.cmd == 0x04)
						{
						// transmit frame
							
						}
        }
    }
    else
    {
        uint8_t tmp[RX_BUFF_SIZE];
        uint8_t len1 = (uint16_t)(RX_BUFF_SIZE - rx_old_pos);
			
        memcpy(tmp, &rx_buff[rx_old_pos], len1);
        memcpy(&tmp[len1], &rx_buff[0], pos);

        uart_frame_t frame;
			
        if (uartParseFrame(tmp, (len1 + pos), &frame) == 0)
        {
            frame_ready = 1;
            if (frame.cmd == 0x10)
            {
							// zaglushka
                const uint8_t ans[] = "CMD 0x10 OK";
              //  uart1_send_frame(0x90, ans, sizeof(ans) - 1);
            }
        }
    }

    rx_old_pos = pos;
		rx_done = 0;
}

static int uartParseFrame(const uint8_t *buf, uint8_t buf_len, uart_frame_t *out)
{
    for (uint8_t i = 0; i < buf_len; i++)
    {
        
				if (buf[i] != START_BYTE)
            continue;
				out->start = buf[i];
			
        if (i + 2 >= buf_len)
            return -1;

        uint8_t len = buf[i + 1];
        uint8_t total = (1 + 1 + 1 + len + 2 + 1);

        if (i + total > buf_len)
            return -1;

        if (buf[i + total - 1] != END_BYTE)
            continue;
				out->end = buf[i + total - 1];
				
        out->crc = (uint16_t)buf[i + 1 + 1 + len] |
                          ((uint16_t)buf[i + 1 + 1 + len + 1] << 8);
				
        uint16_t crc_calc = calculateCRC16(&buf[i + 1], (uint16_t)(1 + len));
				
        if (out->crc != crc_calc)
            continue;

        out->len = len;
        out->cmd = buf[i + 2];
				
        if (out->len > RX_BUFF_SIZE - 6)
            return -2;

        memcpy(out->data, &buf[i + 3], out->len);

        return 0;
    }

    return -1;
}

void uartSendFrame(uint8_t cmd, uint8_t *data, uint8_t data_len)
{
    uint16_t idx = 0;

    tx_buf[idx++] = START_BYTE;
    tx_buf[idx++] = data_len;
    tx_buf[idx++] = cmd;

    for (uint8_t i = 0; i < data_len; i++)
        tx_buf[idx++] = data[i];

    uint16_t crc = crc16_modbus(&tx_buf[1], (uint16_t)(2 + data_len));
    tx_buf[idx++] = (uint8_t)(crc & 0xFF);
    tx_buf[idx++] = (uint8_t)(crc >> 8);
    tx_buf[idx++] = END_BYTE;

    uartSendDma(tx_buf, idx);
}

void uartSendDma(const uint8_t *data, uint16_t len)
{
    while (DMA1_Channel4->CCR & DMA_CCR5_EN) {}

    DMA1_Channel4->CCR &= ~DMA_CCR5_EN;
    DMA1->IFCR = DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4 | DMA_IFCR_CHTIF4;
    DMA1_Channel4->CMAR = (uint32_t)data;
    DMA1_Channel4->CNDTR = len;
    DMA1_Channel4->CCR |= DMA_CCR5_EN;
}

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_IDLE)
    {
				
        rx_done = 1;
    }
}

void DMA1_Channel5_IRQHandler(void)  // RX
{
    if (DMA1->ISR & DMA_ISR_TCIF5)
    {
        DMA1->IFCR = DMA_IFCR_CTCIF5;
        //rx_done = 1;
    }
		
    if (DMA1->ISR & DMA_ISR_TEIF5)
    {
        DMA1->IFCR = DMA_IFCR_CTEIF5;
        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
				rx_error = 1;
    }
}


