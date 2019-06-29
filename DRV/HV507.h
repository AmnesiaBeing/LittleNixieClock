#pragma once

#include "HAL/spi.h"

void HV507_Init(void);
void HV507_SendData(uint64_t data);