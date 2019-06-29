#include "FUN/RGBLED.h"

// 小灯颜色grb信息的缓冲区，注意颜色顺序在内存中的排列： grbgrb...
uint8_t rgbled_buf[LEDNUM * 3] = {0};

// 将所有小灯设置成同一种颜色
void RGBLED_Clear(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < LEDNUM; ++i)
    {
        *(rgbled_buf + i * 3 + 0) = g;
        *(rgbled_buf + i * 3 + 1) = r;
        *(rgbled_buf + i * 3 + 2) = b;
    }
}

// 设置某个小灯为某种颜色
void RGBLED_SetColor(int i, uint8_t r, uint8_t g, uint8_t b)
{
    *(rgbled_buf + i * 3 + 0) = g;
    *(rgbled_buf + i * 3 + 1) = r;
    *(rgbled_buf + i * 3 + 2) = b;
}

// 更新小灯显示
void RGBLED_Update(void)
{
    WS2812_SendData(rgbled_buf);
}
