#pragma once

#include "gpio.h"

void Button_Init(void);
void Button_Register_Callback(void (*callback)(void));
void Button_Unregister_Callback(void);

// 由中断函数调用
void Button_Int_Callback(void);