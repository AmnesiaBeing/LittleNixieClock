/*
https://github.com/eziya/STM32_HAL_DS3231/blob/master/Src/stm32_ds3231.c
 * stm32_ds3231.c
 *
 *  Created on: 2019. 3. 17.
 *      Author: kiki
 */
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
    MX_I2C1_Init();
}

bool DS3231_GetTime(DS3231_time_t *time)
{
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[7] = {
        0,
    };

    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    time->Sec = B2D(buffer[0] & 0x7F);
    time->Min = B2D(buffer[1] & 0x7F);
    time->Hour = B2D(buffer[2] & 0x3F);
    time->DaysOfWeek = buffer[3] & 0x07;
    time->Date = B2D(buffer[4] & 0x3F);
    time->Month = B2D(buffer[5] & 0x1F);
    time->Year = B2D(buffer[6]);

    return true;
}

bool DS3231_SetTime(DS3231_time_t *time)
{
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[8] = {startAddr, D2B(time->Sec), D2B(time->Min), D2B(time->Hour), time->DaysOfWeek, D2B(time->Date), D2B(time->Month), D2B(time->Year)};
    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

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

bool DS3231_SetAlarm1(uint8_t mode, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t alarmSecond = D2B(sec);
    uint8_t alarmMinute = D2B(min);
    uint8_t alarmHour = D2B(hour);
    uint8_t alarmDate = D2B(date);

    switch (mode)
    {
    case DS3231_ALARM_MODE_ALL_MATCHED:
        break;
    case DS3231_ALARM_MODE_HOUR_MIN_SEC_MATCHED:
        alarmDate |= 0x80;
        break;
    case DS3231_ALARM_MODE_MIN_SEC_MATCHED:
        alarmDate |= 0x80;
        alarmHour |= 0x80;
        break;
    case DS3231_ALARM_MODE_SEC_MATCHED:
        alarmDate |= 0x80;
        alarmHour |= 0x80;
        alarmMinute |= 0x80;
        break;
    case DS3231_ALARM_MODE_ONCE_PER_SECOND:
        alarmDate |= 0x80;
        alarmHour |= 0x80;
        alarmMinute |= 0x80;
        alarmSecond |= 0x80;
        break;
    default:
        break;
    }

    /* Write Alarm Registers */
    uint8_t startAddr = DS3231_REG_ALARM1;
    uint8_t buffer[5] = {startAddr, alarmSecond, alarmMinute, alarmHour, alarmDate};
    if (HAL_I2C_Master_Transmit(&hi2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    /* Enable Alarm1 at Control Register */
    uint8_t ctrlReg = 0x00;
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    ctrlReg |= DS3231_CON_A1IE;
    ctrlReg |= DS3231_CON_INTCN;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);

    return true;
}

void DS3231_ClearAlarm1()
{
    uint8_t ctrlReg;
    uint8_t statusReg;

    /* Clear Control Register */
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    ctrlReg &= ~DS3231_CON_A1IE;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);

    /* Clear Status Register */
    DS3231_ReadRegister(DS3231_REG_STATUS, &statusReg);
    statusReg &= ~DS3231_STA_A1F;
    DS3231_WriteRegister(DS3231_REG_STATUS, statusReg);
}

void DS3231_Set1HzSQW(void)
{
    uint8_t ctrlReg;
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    // 将RS2、RS1置0，表示1Hz方波
    ctrlReg &= ~DS3231_CON_RS1;
    ctrlReg &= ~DS3231_CON_RS2;
    // 将INTCN置0，表示输出方波
    ctrlReg &= ~DS3231_CON_INTCN;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
}

void DS3231_Clear1HzSQW(void)
{
    uint8_t ctrlReg;
    DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
    // 将RS2、RS1置0，表示1Hz方波
    ctrlReg &= ~DS3231_CON_RS1;
    ctrlReg &= ~DS3231_CON_RS2;
    // 将INTCN置1，表示不输出方波
    ctrlReg |= DS3231_CON_INTCN;
    DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
}