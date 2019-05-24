#pragma once

#include "main.h"
#include "tim.h"

//小灯数量
#define LEDNUM 6

//小灯归零码的高低电平持续时间（计时器的脉冲数）
#define TH 8
#define TL 4

void WS2812_Init(void);
void WS2812_SendData(uint8_t *data);