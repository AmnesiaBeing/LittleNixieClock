#include "DRV/WS2812.h"

#include <stdio.h>

typedef struct
{
    const uint64_t head;
    uint16_t buf[LEDNUM * 3 * 8];
    const uint64_t tail;
} dma_buf_t;

static dma_buf_t dma_buf = {.head = 0,
                            .tail = 0};

static uint16_t *buf = dma_buf.buf;

//小灯归零码的高低电平持续时间（计时器的脉冲数）
// #define TH 61
// #define TL 28

void WS2812_Init(void)
{
    // MX_TIM2_Init();
    
}

#define PL GPIOA->BRR = (uint32_t)GPIO_PIN_15;
#define PH GPIOA->BSRR = (uint32_t)GPIO_PIN_15;

// data[LEDNUM*3]按照grb顺序排列
void WS2812_SendData(uint8_t *data)
{
    PL;
    // 注意大小端
    for (int i = 0; i < LEDNUM * 3; ++i)
    {
        uint8_t tmp = *(data + i);
        for (int j = 0; j < 8; ++j)
        {
            *(buf + i * 8 + j) = (tmp & 0x80) ? 1 : 0;
            tmp <<= 1;
        }
    }
    for (int i = 0; i < 144; ++i)
    {
        if (buf[i])
        {
            // 1
            PH;
            for (int j = 0; j < 3; ++j)
                ;
            PL;
            for (int j = 0; j < 2; ++j)
                ;
        }
        else
        {
            // 0
            PH;
            for (int j = 0; j < 1; ++j)
                ;
            PL;
            for (int j = 0; j < 3; ++j)
                ;
        }
    }
    PL;
    // uint16_t test[] = {0, 0, 26, 26, 63, 63, 0, 0};
    // HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)test, sizeof(test));
}

// void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
// {
//     HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
// }