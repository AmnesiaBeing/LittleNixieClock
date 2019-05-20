/*
    HV507 use spi1 and dma1 channel3
*/

#include "main.h"
#include "stdbool.h"

#include "spi.h"

// LE == PB4
#define LE_ON()                                             \
    do                                                      \
    {                                                       \
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); \
    } while (0);

#define LE_OFF()                                              \
    do                                                        \
    {                                                         \
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); \
    } while (0);

#define LE_DELAYINSYSTICK 1

//spi1 when idle clk is high, dat is low, and ramp edge effect

static bool le;

void HV507_Init()
{
    MX_DMA_CHAN3_Init();
    MX_SPI1_Init();
    le = false;
}

void HV507_SendData(uint64_t data)
{
    if (le)
        return;
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)&data, sizeof(data));
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
    {
        le = true;
    }
}

// called every loop to set LE
void HV507_MainLoop()
{
    static t;
    if (le)
    {
        LE_ON();
        for (int i = 0x100; i > 0; --i)
            ;
        LE_OFF();
        le = false;
    }
}