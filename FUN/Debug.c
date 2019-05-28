#include "FUN/Debug.h"

void Debug_Init(void)
{
    // init uart2 here
    MX_USART2_UART_Init();
    MX_DMA1_Channel3_IRQ_Enable();
}

// for printf in newlib arm-none-eabi-gcc
int _write(int fd, char *ptr, int len)
{
    // HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xffff);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)ptr, len);
    return len;
}
