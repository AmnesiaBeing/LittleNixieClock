#include "BSP/BSP.h"
#include "DRV/PWR.h"

inline void PWR_5V_ON(void)
{
    HAL_GPIO_WritePin(V5_GPIO_PORT, V5_GPIO_PIN, GPIO_PIN_SET);
}

inline void PWR_5V_OFF(void)
{
    HAL_GPIO_WritePin(V5_GPIO_PORT, V5_GPIO_PIN, GPIO_PIN_RESET);
}

inline void PWR_VPP_ON(void)
{
    HAL_GPIO_WritePin(VPP_GPIO_PORT, VPP_GPIO_PIN, GPIO_PIN_SET);
}

inline void PWR_VPP_OFF(void)
{
    HAL_GPIO_WritePin(VPP_GPIO_PORT, VPP_GPIO_PIN, GPIO_PIN_RESET);
}
