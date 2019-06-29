#include "BSP/BSP.h"
#include "DRV/HV507.h"

// LE == PB4
#define LE_ON() (HV507_LE_GPIO_PORT->BSRR=(uint32_t)HV507_LE_GPIO_PIN)
#define LE_OFF() (HV507_LE_GPIO_PORT->BRR=(uint32_t)HV507_LE_GPIO_PIN)

void HV507_Init(void)
{
    //MX_GPIO_Init() has LE Init
    MX_SPI1_Init();
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    HAL_SPI_Transmit(&HV507_SPIHANDLE, (uint8_t *)&buf, sizeof(buf), 0xFF);
}

void HV507_SendData(uint64_t data)
{
    // 20190618 18：30 测试，总共时间大约150us，SPI分频128，似乎可以快一些
    HAL_SPI_Transmit(&HV507_SPIHANDLE, (uint8_t *)&data, sizeof(data), 0xFF);
    // SPI上升沿到锁存脉冲至少需要tDLE=35ns的时间，其实可以不要这个NOP
    __NOP();
    // 数据手册要求>80ns
    LE_ON();
    __NOP();
    __NOP();
    __NOP();
    LE_OFF();
}