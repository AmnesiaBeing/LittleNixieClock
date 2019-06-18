#include "DRV/HV507.h"

// SPI没调通，还是使用GPIO好了

// LE == PB4
#define LE_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
#define LE_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

void HV507_Init()
{
    MX_SPI1_Init();
}

void HV507_SendData(uint64_t data)
{
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, sizeof(data), 0xFF);
    // SPI上升沿到锁存脉冲至少需要tDLE=35ns的时间，这里长一些好了
    for (int i = 0; i < 10; ++i)
        ;
    // 数据手册要求>80ns
    LE_ON();
    for (int i = 0; i < 10; ++i)
        ;
    LE_OFF();
}