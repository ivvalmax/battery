#ifndef COMMON_H_
#define COMMON_H_

#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include <string.h>
#include <math.h>


// UART RX/TX

#define START_BYTE      0xAA
#define END_BYTE        0x55

#define RX_BUFF_SIZE 128

typedef struct
{
    uint8_t start;
    uint8_t len;
    uint8_t cmd;
    uint8_t data[64];
    uint16_t crc;
    uint8_t end;
} uart_frame_t;

static uint8_t rx_buff[RX_BUFF_SIZE];
static volatile uint8_t rx_done = 0;
static volatile uint8_t rx_error = 0;
static volatile uint8_t rx_old_pos = 0;
static volatile uint8_t frame_ready = 0;

#endif