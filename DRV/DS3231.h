/*
 * stm32_ds3231.h
 *
 *  Created on: 2019. 3. 17.
 *      Author: Jihoon Lee
 */
#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define hi2c hi2c1
#define DS3231_ADDR (0x68 << 1)

#define DS3231_REG_TIME 0x00
#define DS3231_REG_ALARM1 0x07
#define DS3231_REG_ALARM2 0x0B
#define DS3231_REG_CONTROL 0x0E
#define DS3231_REG_STATUS 0x0F
#define DS3231_REG_TEMP 0x11

#define DS3231_CON_EOSC 0x80
#define DS3231_CON_BBSQW 0x40
#define DS3231_CON_CONV 0x20
#define DS3231_CON_RS2 0x10
#define DS3231_CON_RS1 0x08
#define DS3231_CON_INTCN 0x04
#define DS3231_CON_A2IE 0x02
#define DS3231_CON_A1IE 0x01

#define DS3231_STA_OSF 0x80
#define DS3231_STA_32KHZ 0x08
#define DS3231_STA_BSY 0x04
#define DS3231_STA_A2F 0x02
#define DS3231_STA_A1F 0x01

typedef enum
{
    DS3231_ALARM_MODE_ALL_MATCHED = 0,
    DS3231_ALARM_MODE_HOUR_MIN_SEC_MATCHED,
    DS3231_ALARM_MODE_MIN_SEC_MATCHED,
    DS3231_ALARM_MODE_SEC_MATCHED,
    DS3231_ALARM_MODE_ONCE_PER_SECOND
} DS3231_AlarmMode;

void DS3231_Init();
bool DS3231_GetTime(struct tm *tm);
bool DS3231_SetTime(struct tm *tm);
bool DS3231_ReadTemperature(float *temp);
bool DS3231_SetAlarm(uint8_t mode, struct tm *dt, uint8_t id);
void DS3231_ClearAlarm(uint8_t id);
void DS3231_Set1HzSQW(void);
void DS3231_Clear1HzSQW(void);