#pragma once

#include "main.h"

#include "spi.h"

// #include "mutex.h"

#include <stdbool.h>

void HV507_Init();
void HV507_SendData(uint64_t data);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);