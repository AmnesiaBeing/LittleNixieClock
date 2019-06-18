/*
    RGBLED use tim2 chan1
*/

#include "FUN/RGBLED.h"
#include "DRV/WS2812.h"

// #include <stdio.h>

// 维护小灯颜色grb信息的缓冲区，注意颜色存储顺序
// 内存中的排列： grbgrb...
static uint8_t buf[LEDNUM * 3] = {0};

void RGBLED_Init()
{
    WS2812_Init();
    RGBLED_Clear(0, 0, 0);
    RGBLED_Update();
}

// 将所有小灯设置成同一种颜色
void RGBLED_Clear(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < LEDNUM; ++i)
    {
        *(buf + i * 3 + 0) = g;
        *(buf + i * 3 + 1) = r;
        *(buf + i * 3 + 2) = b;
    }
}

// 设置某个小灯为某种颜色
void RGBLED_SetColor(uint8_t r, uint8_t g, uint8_t b, int i)
{
    *(buf + i * 3 + 0) = g;
    *(buf + i * 3 + 1) = r;
    *(buf + i * 3 + 2) = b;
}

// 更新小灯显示
void RGBLED_Update()
{
    WS2812_SendData(buf);
}
