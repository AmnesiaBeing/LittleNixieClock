#include "DRV/HV507.h"

// LE == PB4
#define LE_ON() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
#define LE_OFF() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

void HV507_Init()
{
    // MX_SPI1_Init();
}

void HV507_SendData(uint64_t data)
{
    // 20190618 18：30 测试，总共时间大约150us，SPI分频128，似乎可以快一些
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, sizeof(data), 0xFF);
    // SPI上升沿到锁存脉冲至少需要tDLE=35ns的时间，其实可以不要这个NOP
    __NOP();
    // 数据手册要求>80ns
    LE_ON();
    __NOP();
    __NOP();
    LE_OFF();
}