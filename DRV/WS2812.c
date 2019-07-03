#include "BSP/BSP.h"
#include "DRV/WS2812.h"

#define IO_OFF() (WS2812_GPIO_PORT->BRR = (uint32_t)WS2812_GPIO_PIN)
#define IO_ON() (WS2812_GPIO_PORT->BSRR = (uint32_t)WS2812_GPIO_PIN)

void WS2812_Init(void)
{
    // PA15已在MX_GPIO_Init()中初始化
    WS2812_SendData(0);
}

// 将grb数据转换为位数据01，注意大小端
static inline void WS2812_Data2Bit(uint8_t *data, uint8_t *bit)
{
    for (int i = 0; i < LEDNUM * 3; ++i)
    {
        uint8_t tmp = *(data + i);
        for (int j = 0; j < 8; ++j)
        {
            *(bit + i * 8 + j) = (tmp & 0x80) ? 1 : 0;
            tmp <<= 1;
        }
    }
}

#define HIGHTIMES 9
#define LOWTIMES 4

// data[LEDNUM*3]按照grb顺序排列
void WS2812_SendData(uint8_t *data)
{
#if 1
    return;
#endif
    uint8_t bit[LEDNUM * 3 * 8];
    WS2812_Data2Bit(data, bit);
    IO_OFF();
    for (int i = 0; i < LEDNUM * 3 * 8; ++i)
    {
        if (bit[i])
        {
            // 1: 总时长1.2us，高电平时长800ns
            IO_ON();
            for (int j = 0; j < HIGHTIMES; ++j)
                ;
            IO_OFF();
            for (int j = 0; j < LOWTIMES; ++j)
                ;
        }
        else
        {
            // 0: 总时长1.2us，高电平时长400ns
            IO_ON();
            for (int j = 0; j < LOWTIMES; ++j)
                ;
            IO_OFF();
            for (int j = 0; j < HIGHTIMES; ++j)
                ;
        }
    }
}
