#include <stdio.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"
#include "HAL/i2c.h"
#include "DRV/DS3231.h"

static bool DS3231_ReadRegister(uint8_t regAddr, uint8_t *value)
{
    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, &regAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(&hi2c, DS3231_ADDR, value, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;

    return true;
}

static bool DS3231_WriteRegister(uint8_t regAddr, uint8_t value)
{
    uint8_t buffer[2] = {regAddr, value};
    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    return true;
}

static uint8_t B2D(uint8_t bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t D2B(uint8_t decimal)
{
    return (((decimal / 10) << 4) | (decimal % 10));
}

void DS3231_Init()
{
    // MX_I2C1_Init();
    // set exti15_10 irqn
}

bool DS3231_GetTime(struct tm *time)
{
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[7] = {
        0,
    };

    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    time->tm_sec = B2D(buffer[0] & 0x7F);
    time->tm_min = B2D(buffer[1] & 0x7F);
    time->tm_hour = B2D(buffer[2] & 0x3F);
    time->tm_wday = buffer[3] & 0x07;
    time->tm_mday = B2D(buffer[4] & 0x3F);
    time->tm_mon = B2D(buffer[5] & 0x1F);
    time->tm_year = B2D(buffer[6]);

    return true;
}

bool DS3231_SetTime(struct tm *time)
{
    portENTER_CRITICAL();
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[8] = {startAddr,
                         D2B(time->tm_sec), D2B(time->tm_min), D2B(time->tm_hour), time->tm_wday,
                         D2B(time->tm_mday), D2B(time->tm_mon), D2B(time->tm_year)};
    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
    {
        portEXIT_CRITICAL();
        return false;
    }
    portEXIT_CRITICAL();
    return true;
}

bool DS3231_ReadTemperature(float *temp)
{
    uint8_t startAddr = DS3231_REG_TEMP;
    uint8_t buffer[2] = {
        0,
    };

    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    int16_t value = (buffer[0] << 8) | (buffer[1]);
    value = (value >> 6);

    *temp = value / 4.0f;
    return true;
}

bool DS3231_SetAlarm(uint8_t mode, struct tm *dt, uint8_t id)
{
    uint8_t day = dt->tm_mday;
    uint8_t hour = dt->tm_hour;
    uint8_t min = dt->tm_min;
    uint8_t sec = dt->tm_sec;

    uint8_t alarmSecond = D2B(sec);
    uint8_t alarmMinute = D2B(min);
    uint8_t alarmHour = D2B(hour);
    uint8_t alarmDay = D2B(day);

    switch (mode)
    {
    case DS3231_ALARM_MODE_ALL_MATCHED:
        break;
    case DS3231_ALARM_MODE_HOUR_MIN_SEC_MATCHED:
        alarmDay |= 0x80;
        break;
    case DS3231_ALARM_MODE_MIN_SEC_MATCHED:
        alarmDay |= 0x80;
        alarmHour |= 0x80;
        break;
    case DS3231_ALARM_MODE_SEC_MATCHED:
        alarmDay |= 0x80;
        alarmHour |= 0x80;
        alarmMinute |= 0x80;
        break;
    case DS3231_ALARM_MODE_ONCE_PER_SECOND:
        alarmDay |= 0x80;
        alarmHour |= 0x80;
        alarmMinute |= 0x80;
        alarmSecond |= 0x80;
        break;
    default:
        break;
    }

    uint8_t startAddr;
    uint8_t buffer[5] = {startAddr, alarmSecond, alarmMinute, alarmHour, alarmDay};
    uint8_t ctrlReg = 0x00;
    switch (id)
    {
    case 1:
        /* Write Alarm 1 Registers */
        startAddr = DS3231_REG_ALARM1;
        if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            return false;

        /* Enable Alarm1 at Control Register */

        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg |= DS3231_CON_A1IE;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
        break;
    case 2:
        /* Write Alarm 2 Registers */
        startAddr = DS3231_REG_ALARM2;
        if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            return false;

        /* Enable Alarm1 at Control Register */

        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg |= DS3231_CON_A2IE;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
        break;
    default:
        return false;
        break;
    }

    return true;
}

void DS3231_ClearAlarm(uint8_t id)
{
    uint8_t ctrlReg;
    uint8_t statusReg;
    switch (id)
    {
    case 1:
        /* Clear Control Register */
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg &= ~DS3231_CON_A1IE;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);

        /* Clear Status Register */
        DS3231_ReadRegister(DS3231_REG_STATUS, &statusReg);
        statusReg &= ~DS3231_STA_A1F;
        DS3231_WriteRegister(DS3231_REG_STATUS, statusReg);
        break;
    case 2:
        /* Clear Control Register */
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg &= ~DS3231_CON_A2IE;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);

        /* Clear Status Register */
        DS3231_ReadRegister(DS3231_REG_STATUS, &statusReg);
        statusReg &= ~DS3231_STA_A2F;
        DS3231_WriteRegister(DS3231_REG_STATUS, statusReg);
        break;
    default:
        break;
    }
}

void DS3231_Set1HzSQW(void)
{
    uint8_t ctrlReg;
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    ctrlReg |= DS3231_CON_BBSQW;
    ctrlReg |= DS3231_CON_EOSC;
    // 将RS2、RS1置0，表示1Hz方波
    ctrlReg &= ~DS3231_CON_RS1;
    ctrlReg &= ~DS3231_CON_RS2;
    // 将INTCN置0，表示输出方波
    ctrlReg &= ~DS3231_CON_INTCN;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
    // HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void DS3231_Clear1HzSQW(void)
{
    uint8_t ctrlReg;
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    // 将INTCN置1，表示不输出方波，输出闹钟
    ctrlReg |= DS3231_CON_INTCN;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
    // HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

extern void NixieTube_Show(const char *str);

void DS3231_SQW_Callback(void)
{
    struct tm t;
    uint8_t h;
    uint8_t m;
    uint8_t s;
    char buf[] = "00:00:00";
    static bool flag = false;
    if (DS3231_GetTime(&t))
    {
        h = t.tm_hour;
        m = t.tm_min;
        s = t.tm_sec;
        flag ? sprintf(buf, "%02d:%02d:%02d", h, m, s) : sprintf(buf, "%02d %02d %02d", h, m, s);
        flag = !flag;
        NixieTube_Show(buf);
    }
}