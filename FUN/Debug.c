#include "main.h"

//printf

extern UART_HandleTypeDef huart2;

int _write(int fd, char *ptr, int len)
{
    // HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xffff);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)ptr, len);
    return len;
}
