#include "BSP/BSP.h"
#include "HAL/usart.h"

void Debug_Init(void)
{
    MX_USART2_UART_Init();
}

// for printf in newlib arm-none-eabi-gcc
int _write(int fd, char *ptr, int len)
{
    HAL_UART_Transmit(&Debug_UARTHANDLE, (uint8_t *)ptr, len, 0xff);
    // HAL_UART_Transmit_DMA(&Debug_UARTHANDLE, (uint8_t *)ptr, len);
    return len;
}
