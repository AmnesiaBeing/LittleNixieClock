#include "main.h"

#include <stdio.h>

void Button_Init(void)
{
    // GPIO Init
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

void Button_Callback(void)
{
    printf("btn press\n");
}