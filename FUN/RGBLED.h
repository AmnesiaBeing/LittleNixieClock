#pragma once

#include <stdint.h>

void RGBLED_Init();
// 将所有小灯设置成同一种颜色
void RGBLED_Clear(uint8_t r, uint8_t g, uint8_t b);
// 设置某个小灯为某种颜色
void RGBLED_SetColor(uint8_t r, uint8_t g, uint8_t b, int i);
// 更新小灯显示
void RGBLED_Update();