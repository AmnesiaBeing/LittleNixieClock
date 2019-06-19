#include "main.h"
#include "usart.h"

void Debug_Init(void)
{
    MX_USART2_UART_Init();
}

// for printf in newlib arm-none-eabi-gcc
int _write(int fd, char *ptr, int len)
{
    // HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xffff);
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xff);
    return len;
}
