#pragma once

#include <stdint.h>

typedef struct _RTC_Date_s
{
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t DaysOfWeek;
}RTC_Date_t;

typedef struct _RTC_Time_s
{
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;
}RTC_Time_t;
