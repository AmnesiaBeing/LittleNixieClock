#include "BSP/BSP.h"
#include "DRV/DS3231.h"
#include "cmsis_os.h"

osSemaphoreId DS3231SemHandle;

void DS3231_Init(void)
{
    MX_I2C1_Init();
    osSemaphoreDef(DS3231SemHandle);
    DS3231SemHandle = osSemaphoreCreate(osSemaphore(DS3231SemHandle), 1);
}

static bool DS3231_ReadRegister(uint8_t regAddr, uint8_t *value)
{
    if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, &regAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(&DS3231_I2CHANDLE, DS3231_ADDR, value, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;

    return true;
}

static bool DS3231_WriteRegister(uint8_t regAddr, uint8_t value)
{
    uint8_t buffer[2] = {regAddr, value};
    if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
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

bool DS3231_GetTime(struct tm *time)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);

    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[7] = {0};
    bool ret = false;

    do
    {
        if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
            break;
        if (HAL_I2C_Master_Receive(&DS3231_I2CHANDLE, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            break;

        time->tm_sec = B2D(buffer[0] & 0x7F);
        time->tm_min = B2D(buffer[1] & 0x7F);
        time->tm_hour = B2D(buffer[2] & 0x3F);
        time->tm_wday = buffer[3] & 0x07;
        time->tm_mday = B2D(buffer[4] & 0x3F);
        time->tm_mon = B2D(buffer[5] & 0x1F);
        time->tm_year = B2D(buffer[6]);
        ret = true;
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
    return ret;
}

bool DS3231_SetTime(struct tm *time)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);

    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[8] = {startAddr,
                         D2B(time->tm_sec), D2B(time->tm_min), D2B(time->tm_hour), time->tm_wday,
                         D2B(time->tm_mday), D2B(time->tm_mon), D2B(time->tm_year)};
    bool ret = false;
    do
    {
        if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            break;

        ret = true;
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
    return ret;
}

bool DS3231_ReadTemperature(float *temp)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);

    uint8_t startAddr = DS3231_REG_TEMP;
    uint8_t buffer[2] = {0};
    bool ret = false;

    do
    {
        if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
            break;
        if (HAL_I2C_Master_Receive(&DS3231_I2CHANDLE, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            break;

        int16_t value = (buffer[0] << 8) | (buffer[1]);
        value = (value >> 6);

        *temp = value / 4.0f;
        ret = true;
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
    return ret;
}

bool DS3231_SetAlarm(DS3231_ALARM_MODE mode, struct tm *dt, DS3231_ALARM_ID id)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    bool ret = false;
    do
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
        case DS3231_ALARM_MODE_EVERYDAY:
            alarmDay |= 0x80;
            break;
        case DS3231_ALARM_MODE_EVERYHOUR:
            alarmDay |= 0x80;
            alarmHour |= 0x80;
            break;
        default:
            goto exit;
        }

        uint8_t startAddr;
        uint8_t ctrlReg = 0x00;
        uint8_t mask;
        switch (id)
        {
        case DS3231_ALARM_1:
            startAddr = DS3231_REG_ALARM1;
            mask = DS3231_CON_A1IE;
            break;
        case DS3231_ALARM_2:
            startAddr = DS3231_REG_ALARM2;
            mask = DS3231_CON_A2IE;
            break;
        default:
            goto exit;
        }
        uint8_t buffer[5] = {startAddr, alarmSecond, alarmMinute, alarmHour, alarmDay};
        /* Write Alarm Registers */
        if (HAL_I2C_Master_Transmit(&DS3231_I2CHANDLE, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
            return false;

        /* Enable Alarm at Control Register */
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg |= mask;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
        ret = true;
    } while (0);
exit:
    osSemaphoreRelease(DS3231SemHandle);
    return ret;
}

void DS3231_ClearAlarm(DS3231_ALARM_ID id)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    uint8_t ctrlReg;
    uint8_t statusReg;
    uint8_t maskcon, masksta;
    do
    {
        switch (id)
        {
        case DS3231_ALARM_1:
            maskcon = DS3231_CON_A1IE;
            masksta = DS3231_STA_A1F;
            break;
        case DS3231_ALARM_2:
            maskcon = DS3231_CON_A2IE;
            masksta = DS3231_STA_A2F;
            break;
        default:
            goto exit;
        }
        /* Clear Control Register */
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg &= ~maskcon;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);

        /* Clear Status Register */
        DS3231_ReadRegister(DS3231_REG_STATUS, &statusReg);
        statusReg &= ~masksta;
        DS3231_WriteRegister(DS3231_REG_STATUS, statusReg);
    } while (0);
exit:
    osSemaphoreRelease(DS3231SemHandle);
}

void DS3231_Set1HzSQW(void)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    uint8_t ctrlReg;
    do
    {
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        ctrlReg |= DS3231_CON_BBSQW;
        ctrlReg |= DS3231_CON_EOSC;
        // 将RS2、RS1置0，表示1Hz方波
        ctrlReg &= ~DS3231_CON_RS1;
        ctrlReg &= ~DS3231_CON_RS2;
        // 将INTCN置0，表示输出方波
        ctrlReg &= ~DS3231_CON_INTCN;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
}

void DS3231_Clear1HzSQW(void)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    uint8_t ctrlReg;
    do
    {
        DS3231_ReadRegister(DS3231_REG_CONTROL, &ctrlReg);
        // 将INTCN置1，表示不输出方波，输出闹钟
        ctrlReg |= DS3231_CON_INTCN;
        DS3231_WriteRegister(DS3231_REG_CONTROL, ctrlReg);
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
}

static void (*DS3231_Callback)(void);

void DS3231_SQW_INT_Callback(void)
{
    if (DS3231_Callback)
        DS3231_Callback();
}

void DS3231_Register_Callback(void (*callback)(void))
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    do
    {
        DS3231_Callback = callback;
        HAL_NVIC_EnableIRQ(DS3231_SQW_INT_IRQ);
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
}

void DS3231_Unregister_Callback(void)
{
    osSemaphoreWait(DS3231SemHandle, osWaitForever);
    do
    {
        DS3231_Callback = NULL;
        HAL_NVIC_DisableIRQ(DS3231_SQW_INT_IRQ);
    } while (0);
    osSemaphoreRelease(DS3231SemHandle);
}