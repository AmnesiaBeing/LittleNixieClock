#include "main.h"

//#define EN_5V PA5
//VPP_EN PA4

void PWR_5V_ON(void)
{
    LOG("PWR_5V_ON");
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

void PWR_5V_OFF(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

void PWR_VPP_ON(void)
{
    LOG("PWR_VPP_ON");
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void PWR_VPP_OFF(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}
