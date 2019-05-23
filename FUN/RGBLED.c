/*
    RGBLED use tim2 chan1
*/

#include "main.h"
#include "tim.h"

#define LEDNUM 6

// a r g b
typedef struct _Color_s
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color_t;

static Color_t bufColor[LEDNUM] = {0};
static uint8_t bufOutput[LEDNUM * 3 * 8] = {0};

void RGBLED_Init()
{
    //MX_TIM2_Init();
    //MX_DMAXXX_Init();
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

void RGBLED_SendData()
{
    //
    HAL_TIM_Base_Start_DMA(&htim2, bufOutput, sizeof(bufOutput));
}