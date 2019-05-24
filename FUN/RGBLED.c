/*
    RGBLED use tim2 chan1
*/

#include "main.h"
#include "tim.h"

#define LEDNUM 6

#define T_H 8
#define T_L 4

void RGBLED_SetColorRGB(uint8_t r, uint8_t g, uint8_t b, int index);

// a r g b
typedef struct _Color_s
{
    uint8_t g;
    uint8_t r;
    uint8_t b;
} Color_t;

static Color_t bufColor[LEDNUM] = {0};
static uint8_t bufOutput[LEDNUM][24] = {0};

void RGBLED_Init()
{
    //MX_TIM2_Init();
    //MX_DMAXXX_Init();
    RGBLED_SetColorRGB(255, 0, 0, 0);
    RGBLED_SetColorRGB(0, 255, 0, 1);
    RGBLED_SetColorRGB(0, 0, 255, 2);
    RGBLED_SetColorRGB(255, 255, 0, 3);
    RGBLED_SetColorRGB(0, 255, 255, 4);
    RGBLED_SetColorRGB(255, 0, 255, 5);
}

void RGBLED_ClearColor(Color_t c)
{
}

void RGBLED_ClearRGB(uint8_t r, uint8_t g, uint8_t b)
{
}

void RGBLED_SetColor(Color_t c, int index)
{
}

void RGBLED_SetColorRGB(uint8_t r, uint8_t g, uint8_t b, int index)
{
    bufColor[index].r = r;
    bufColor[index].g = g;
    bufColor[index].b = b;
}

void RGBLED_SendData()
{
    //此处代码必须可以优化，不过先试试吧
    for (int i = 0; i < LEDNUM * 8 * 3; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            bufOutput[i][j] = (bufColor[i].g & (0x1 << j)) == 1 ? T_H : T_L;
        }
        for (int j = 0; j < 8; ++j)
        {
            bufOutput[i][j] = (bufColor[i].r & (0x1 << j)) == 1 ? T_H : T_L;
        }
        for (int j = 0; j < 8; ++j)
        {
            bufOutput[i][j] = (bufColor[i].b & (0x1 << j)) == 1 ? T_H : T_L;
        }
    }
    //
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)bufOutput, sizeof(bufOutput));
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}