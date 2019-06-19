#pragma once

#include "stm32l4xx_hal.h"

#include <stdio.h>
#define LOG printf

/* 状态机 */
typedef enum
{
    ST_INIT, /* 0 */
    ST_NORMAL,
    ST_UNDERCONTROL,
} ST_t;

extern ST_t status;

void Error_Handler(void);