#include "DRV/WS2812.h"

static uint8_t buf[LEDNUM * 3 * 8];

//小灯归零码的高低电平持续时间（计时器的脉冲数）
#define TH 8
#define TL 4

void WS2812_Init(void)
{
    MX_TIM2_Init();
}

// data[LEDNUM*3]按照grb顺序排列
void WS2812_SendData(uint8_t *data)
{
    for (int i = 0; i < LEDNUM * 3; ++i)
    {
        uint8_t tmp = *(data + i);
        for (int j = 0; j < 8; ++j)
        {
            *(buf + i * 8 + j) = (tmp & 0x01) == 1 ? TH : TL;
            tmp <<= 1;
        }
    }
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)buf, sizeof(buf));
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}