#include "ESP8266.h"

ESP8266_t hESP8266;

//#########################################################################################################
bool ESP8266_SendRaw(uint8_t *data, uint16_t len)
{
    if (len <= _ESP8266_TX_SIZE)
    {
        memcpy(hESP8266.TxBuffer, data, len);
        if (HAL_UART_Transmit(&_ESP8266_USART, data, len, 100) == HAL_OK)
            return true;
        else
            return false;
    }
    else
        return false;
}
//#########################################################################################################
bool ESP8266_SendString(char *data)
{
    return ESP8266_SendRaw((uint8_t *)data, strlen(data));
}
//#########################################################################################################
bool ESP8266_SendStringAndWait(char *data, uint16_t DelayMs)
{
    if (ESP8266_SendRaw((uint8_t *)data, strlen(data)) == false)
        return false;
    HAL_Delay(DelayMs);
    return true;
}
//#########################################################################################################
bool ESP8266_WaitForString(uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...)
{
    if (result == NULL)
        return false;
    if (CountOfParameter == 0)
        return false;

    *result = 0;

    va_list tag;
    va_start(tag, CountOfParameter);
    char *arg[CountOfParameter];
    for (uint8_t i = 0; i < CountOfParameter; i++)
        arg[i] = va_arg(tag, char *);
    va_end(tag);

    //////////////////////////////////
    for (uint32_t t = 0; t < TimeOut_ms; t += 50)
    {
        HAL_Delay(50);
        for (uint8_t mx = 0; mx < CountOfParameter; mx++)
        {
            if (strstr((char *)hESP8266.RxBuffer, arg[mx]) != NULL)
            {
                *result = mx + 1;
                return true;
            }
        }
    }
    // timeout
    return false;
}
//#########################################################################################################
bool ESP8266_ReturnString(char *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if (result == NULL)
        return false;
    if (WantWhichOne == 0)
        return false;

    char *str = (char *)hESP8266.RxBuffer;

    str = strtok(str, SplitterChars);
    if (str == NULL)
    {
        strcpy(result, "");
        return false;
    }
    while (str != NULL)
    {
        str = strtok(NULL, SplitterChars);
        if (str != NULL)
            WantWhichOne--;
        if (WantWhichOne == 0)
        {
            strcpy(result, str);
            return true;
        }
    }
    strcpy(result, "");
    return false;
}

//#########################################################################################################
bool ESP8266_ReturnStrings(char *InputString, char *SplitterChars, uint8_t CountOfParameter, ...)
{
    if (CountOfParameter == 0)
        return false;
    va_list tag;
    va_start(tag, CountOfParameter);
    char *arg[CountOfParameter];
    for (uint8_t i = 0; i < CountOfParameter; i++)
        arg[i] = va_arg(tag, char *);
    va_end(tag);

    char *str;
    str = strtok(InputString, SplitterChars);
    if (str == NULL)
        return false;
    uint8_t i = 0;
    while (str != NULL)
    {
        str = strtok(NULL, SplitterChars);
        if (str != NULL)
            CountOfParameter--;
        strcpy(arg[i], str);
        i++;
        if (CountOfParameter == 0)
        {
            return true;
        }
    }
    return false;
}
//#########################################################################################################
bool ESP8266_ReturnInteger(int32_t *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if ((char *)hESP8266.RxBuffer == NULL)
        return false;
    if (ESP8266_ReturnString((char *)hESP8266.RxBuffer, WantWhichOne, SplitterChars) == false)
        return false;
    *result = atoi((char *)hESP8266.RxBuffer);
    return true;
}
//#########################################################################################################

bool ESP8266_ReturnFloat(float *result, uint8_t WantWhichOne, char *SplitterChars)
{
    if ((char *)hESP8266.RxBuffer == NULL)
        return false;
    if (ESP8266_ReturnString((char *)hESP8266.RxBuffer, WantWhichOne, SplitterChars) == false)
        return false;
    *result = atof((char *)hESP8266.RxBuffer);
    return true;
}
//#########################################################################################################
void ESP8266_RemoveChar(char *str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++)
    {
        *dst = *src;
        if (*dst != garbage)
            dst++;
    }
    *dst = '\0';
}
//#########################################################################################################
void ESP8266_RxClear(void)
{
    memset(hESP8266.RxBuffer, 0, _ESP8266_RX_SIZE);
    hESP8266.RxIndex = 0;
    HAL_UART_Receive_IT(&_ESP8266_USART, &hESP8266.usartBuff, 1);
}
//#########################################################################################################
void ESP8266_TxClear(void)
{
    memset(hESP8266.TxBuffer, 0, _ESP8266_TX_SIZE);
}
//#########################################################################################################
void ESP8266_RxCallBack(void)
{
    //+++ at command buffer
    if (hESP8266.RxIsData == false)
    {
        hESP8266.RxBuffer[hESP8266.RxIndex] = hESP8266.usartBuff;
        if (hESP8266.RxIndex < _ESP8266_RX_SIZE)
            hESP8266.RxIndex++;
    }
    //--- at command buffer
    //+++  data buffer
    else
    {
        if (HAL_GetTick() - hESP8266.RxDataLastTime > 50)
            hESP8266.RxIsData = false;
        //+++ Calculate Data len after +IPD
        if (hESP8266.RxDataLen == 0)
        {
            //+++ Calculate Data len after +IPD ++++++ Multi Connection OFF
            if (hESP8266.TcpIpMultiConnection == false)
            {
                hESP8266.RxBufferForDataTmp[hESP8266.RxIndexForDataTmp] = hESP8266.usartBuff;
                hESP8266.RxIndexForDataTmp++;
                if (hESP8266.RxBufferForDataTmp[hESP8266.RxIndexForDataTmp - 1] == ':')
                {
                    hESP8266.RxDataConnectionNumber = 0;
                    hESP8266.RxDataLen = atoi((char *)&hESP8266.RxBufferForDataTmp[1]);
                }
            }
            //--- Calculate Data len after +IPD ++++++ Multi Connection OFF
            //+++ Calculate Data len after +IPD ++++++ Multi Connection ON
            else
            {
                hESP8266.RxBufferForDataTmp[hESP8266.RxIndexForDataTmp] = hESP8266.usartBuff;
                hESP8266.RxIndexForDataTmp++;
                if (hESP8266.RxBufferForDataTmp[2] == ',')
                {
                    hESP8266.RxDataConnectionNumber = hESP8266.RxBufferForDataTmp[1] - 48;
                }
                if ((hESP8266.RxIndexForDataTmp > 3) && (hESP8266.RxBufferForDataTmp[hESP8266.RxIndexForDataTmp - 1] == ':'))
                    hESP8266.RxDataLen = atoi((char *)&hESP8266.RxBufferForDataTmp[3]);
            }
            //--- Calculate Data len after +IPD ++++++ Multi Connection ON
        }
        //--- Calculate Data len after +IPD
        //+++ Fill Data Buffer
        else
        {
            hESP8266.RxBufferForData[hESP8266.RxIndexForData] = hESP8266.usartBuff;
            if (hESP8266.RxIndexForData < _ESP8266_RX_FOR_DATA_SIZE)
                hESP8266.RxIndexForData++;
            if (hESP8266.RxIndexForData >= hESP8266.RxDataLen)
            {
                hESP8266.RxIsData = false;
                hESP8266.GotNewData = true;
            }
        }
        //--- Fill Data Buffer
    }
    //--- data buffer
    HAL_UART_Receive_IT(&_ESP8266_USART, &hESP8266.usartBuff, 1);
    //+++ check +IPD in At command buffer
    if (hESP8266.RxIndex > 4)
    {
        if ((hESP8266.RxBuffer[hESP8266.RxIndex - 4] == '+') && (hESP8266.RxBuffer[hESP8266.RxIndex - 3] == 'I') && (hESP8266.RxBuffer[hESP8266.RxIndex - 2] == 'P') && (hESP8266.RxBuffer[hESP8266.RxIndex - 1] == 'D'))
        {
            memset(hESP8266.RxBufferForDataTmp, 0, sizeof(hESP8266.RxBufferForDataTmp));
            hESP8266.RxBuffer[hESP8266.RxIndex - 4] = 0;
            hESP8266.RxBuffer[hESP8266.RxIndex - 3] = 0;
            hESP8266.RxBuffer[hESP8266.RxIndex - 2] = 0;
            hESP8266.RxBuffer[hESP8266.RxIndex - 1] = 0;
            hESP8266.RxIndex -= 4;
            hESP8266.RxIndexForData = 0;
            hESP8266.RxIndexForDataTmp = 0;
            hESP8266.RxIsData = true;
            hESP8266.RxDataLen = 0;
            hESP8266.RxDataLastTime = HAL_GetTick();
        }
    }
    //--- check +IPD in At command buffer
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void ESP8266_MainLoop(void)
{
    ESP8266_GetMyIp();
    if ((hESP8266.Mode == ESP8266_Mode_SoftAp) || (hESP8266.Mode == ESP8266_Mode_StationAndSoftAp))
        ESP8266_SoftAp_GetConnectedDevices();
    ESP8266_TcpIp_GetConnectionStatus();
    ESP8266_RxClear();
    //可能需要加入状态机或者延时条件判断（但是在mainloop中不想加入任何的延时程序）
    if (hESP8266.GotNewData == true)
    {
        hESP8266.GotNewData = false;
        for (uint8_t i = 0; i < 5; i++)
        {
            if ((strstr(hESP8266.TcpIpConnections[i].Type, "UDP") != NULL) && (hESP8266.RxDataConnectionNumber == hESP8266.TcpIpConnections[i].LinkId))
                ESP8266_UserGetUdpData(hESP8266.RxDataConnectionNumber, hESP8266.RxDataLen, hESP8266.RxBufferForData);
            if ((strstr(hESP8266.TcpIpConnections[i].Type, "TCP") != NULL) && (hESP8266.RxDataConnectionNumber == hESP8266.TcpIpConnections[i].LinkId))
                ESP8266_UserGetTcpData(hESP8266.RxDataConnectionNumber, hESP8266.RxDataLen, hESP8266.RxBufferForData);
        }
    }

    ESP8266_UserProcess();
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void ESP8266_Init()
{
    HAL_UART_Receive_IT(&_ESP8266_USART, &hESP8266.usartBuff, 1);
    ESP8266_RxClear();
    ESP8266_TxClear();
    //这个程序的延时太多了，很不爽，可能真的要上片上操作系统了？
    HAL_Delay(3000);
    ESP8266_SendStringAndWait("AT\r\n", 1000);
    ESP8266_SetRfPower(82);
    ESP8266_TcpIp_GetMultiConnection();
    ESP8266_TcpIp_Close(0);
    ESP8266_TcpIp_Close(1);
    ESP8266_TcpIp_Close(2);
    ESP8266_TcpIp_Close(3);
    ESP8266_TcpIp_Close(4);
    ESP8266_TcpIp_SetMultiConnection(true);
    ESP8266_GetMode();
    ESP8266_Station_DhcpIsEnable();
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool ESP8266_Restart(void)
{
    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+RST\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);
    return returnVal;
}
//#########################################################################################################
bool ESP8266_DeepSleep(uint16_t DelayMs)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+GSLP=%d\r\n", DelayMs);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_FactoryReset(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+RESTORE\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_Update(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIUPDATE\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(1000 * 60 * 5, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_SetRfPower(uint8_t Power_0_to_82)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+RFPOWER=%d\r\n", Power_0_to_82);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool ESP8266_SetMode(ESP8266_Mode_t ESP8266_Mode_)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWMODE_CUR=%d\r\n", ESP8266_Mode_);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        hESP8266.Mode = ESP8266_Mode_;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_GetMode(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWMODE_CUR?\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        if (ESP8266_ReturnInteger((int32_t *)&result, 1, ":"))
            hESP8266.Mode = (ESP8266_Mode_t)result;
        else
            hESP8266.Mode = ESP8266_Mode_Error;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_GetMyIp(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIFSR\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        sscanf((char *)hESP8266.RxBuffer, "AT+CIFSR\r\r\n+CIFSR:APIP,\"%[^\"]", hESP8266.MyIP);
        sscanf((char *)hESP8266.RxBuffer, "AT+CIFSR\r\r\n+CIFSR:STAIP,\"%[^\"]", hESP8266.MyIP);

        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTA?\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;

        char *str = strstr((char *)hESP8266.RxBuffer, "gateway:");
        if (str == NULL)
            break;
        if (ESP8266_ReturnStrings(str, "\"", 1, hESP8266.MyGateWay) == false)
            break;

        returnVal = true;
    } while (0);

    return returnVal;
}

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool ESP8266_Station_ConnectToAp(char *SSID, char *Pass, char *MAC)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (MAC == NULL)
            sprintf((char *)hESP8266.TxBuffer, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", SSID, Pass);
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CWJAP_CUR=\"%s\",\"%s\",\"%s\"\r\n", SSID, Pass, MAC);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_VERYHIGH, &result, 3, "\r\nOK\r\n", "\r\nERROR\r\n", "\r\nFAIL\r\n") == false)
            break;
        if (result > 1)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_Station_Disconnect(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWQAP\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_Station_DhcpEnable(bool Enable)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWDHCP_CUR=1,%d\r\n", Enable);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        hESP8266.StationDhcp = Enable;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_Station_DhcpIsEnable(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWDHCP_CUR?\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        if (ESP8266_ReturnInteger((int32_t *)&result, 1, ":") == false)
            break;
        switch (result)
        {
        case 0:
            hESP8266.StationDhcp = false;
            hESP8266.SoftApDhcp = false;
            break;
        case 1:
            hESP8266.StationDhcp = false;
            hESP8266.SoftApDhcp = true;
            break;
        case 2:
            hESP8266.StationDhcp = true;
            hESP8266.SoftApDhcp = false;
            break;
        case 3:
            hESP8266.StationDhcp = true;
            hESP8266.SoftApDhcp = true;
            break;
        }
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_Station_SetIp(char *IP, char *GateWay, char *NetMask)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTA_CUR=\"%s\",\"%s\",\"%s\"\r\n", IP, GateWay, NetMask);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        hESP8266.StationDhcp = false;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################

//#########################################################################################################

//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool ESP8266_SoftAp_GetConnectedDevices(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWLIF\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        ESP8266_RemoveChar((char *)hESP8266.RxBuffer, '\r');
        ESP8266_ReturnStrings((char *)hESP8266.RxBuffer, "\n,", 10, hESP8266.SoftApConnectedDevicesIp[0], hESP8266.SoftApConnectedDevicesMac[0], hESP8266.SoftApConnectedDevicesIp[1], hESP8266.SoftApConnectedDevicesMac[1], hESP8266.SoftApConnectedDevicesIp[2], hESP8266.SoftApConnectedDevicesMac[2], hESP8266.SoftApConnectedDevicesIp[3], hESP8266.SoftApConnectedDevicesMac[3], hESP8266.SoftApConnectedDevicesIp[4], hESP8266.SoftApConnectedDevicesMac[4]);
        for (uint8_t i = 0; i < 6; i++)
        {
            if ((hESP8266.SoftApConnectedDevicesIp[i][0] < '0') || (hESP8266.SoftApConnectedDevicesIp[i][0] > '9'))
                hESP8266.SoftApConnectedDevicesIp[i][0] = 0;
            if ((hESP8266.SoftApConnectedDevicesMac[i][0] < '0') || (hESP8266.SoftApConnectedDevicesMac[i][0] > '9'))
                hESP8266.SoftApConnectedDevicesMac[i][0] = 0;
        }

        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_SoftAp_Create(char *SSID, char *password, uint8_t channel, ESP8266_EncryptionType_t ESP8266_EncryptionType, uint8_t MaxConnections_1_to_4, bool HiddenSSID)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n", SSID, password, channel, ESP8266_EncryptionType, MaxConnections_1_to_4, HiddenSSID);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
bool ESP8266_TcpIp_GetConnectionStatus(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTATUS\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;

        char *str = strstr((char *)hESP8266.RxBuffer, "\nSTATUS:");
        if (str == NULL)
            break;
        str = strchr(str, ':');
        str++;
        for (uint8_t i = 0; i < 5; i++)
            hESP8266.TcpIpConnections[i].status = (ESP8266_ConnectionStatus_t)atoi(str);
        str = strstr((char *)hESP8266.RxBuffer, "+CIPSTATUS:");
        for (uint8_t i = 0; i < 5; i++)
        {
            sscanf(str, "+CIPSTATUS:%d,\"%3s\",\"%[^\"]\",%d,%d,%d", (int *)&hESP8266.TcpIpConnections[i].LinkId, hESP8266.TcpIpConnections[i].Type, hESP8266.TcpIpConnections[i].RemoteIp, (int *)&hESP8266.TcpIpConnections[i].RemotePort, (int *)&hESP8266.TcpIpConnections[i].LocalPort, (int *)&hESP8266.TcpIpConnections[i].RunAsServer);
            str++;
            str = strstr(str, "+CIPSTATUS:");
            if (str == NULL)
                break;
        }
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_Ping(char *PingTo)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+PING=\"%s\"\r\n", PingTo);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_MED, &result, 3, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        if (ESP8266_ReturnInteger((int32_t *)&hESP8266.TcpIpPingAnswer, 2, "+") == false)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPMUX=%d\r\n", EnableMultiConnections);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        hESP8266.TcpIpMultiConnection = EnableMultiConnections;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_GetMultiConnection(void)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPMUX?\r\n");
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        if (ESP8266_ReturnInteger((int32_t *)&result, 1, ":") == false)
            break;
        hESP8266.TcpIpMultiConnection = (bool)result;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPSERVER=1,%d\r\n", RemotePort);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n", RemoteIp, RemotePort, TimeOut);
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n", LinkId, RemoteIp, RemotePort, TimeOut);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_HIGH, &result, 3, "OK", "CONNECT", "ERROR") == false)
            break;
        if (result == 3)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_StartUdpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t LocalPort)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTART=\"UDP\",\"%s\",%d,%d\r\n", RemoteIp, RemotePort, LocalPort);
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d\r\n", LinkId, RemoteIp, RemotePort, LocalPort);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_HIGH, &result, 3, "OK", "ALREADY", "ERROR") == false)
            break;
        if (result == 3)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_Close(uint8_t LinkId)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPCLOSE\r\n");
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPCLOSE=%d\r\n", LinkId);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_SetEnableTcpServer(uint16_t PortNumber)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
        {
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPMUX=1\r\n");
            if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
                break;
            if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
                break;
            hESP8266.TcpIpMultiConnection = true;
            ESP8266_RxClear();
        }
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSERVER=1,%d\r\n", PortNumber);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_SetDisableTcpServer(uint16_t PortNumber)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        sprintf((char *)hESP8266.TxBuffer, "AT+CIPSERVER=0,%d\r\n", PortNumber);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (result == 2)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_SendDataUdp(uint8_t LinkId, uint16_t dataLen, uint8_t *data)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSERVER=0\r\n");
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSEND=%d,%d\r\n", LinkId, dataLen);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, ">", "ERROR") == false)
            break;
        if (result == 2)
            break;
        ESP8266_RxClear();
        ESP8266_SendRaw(data, dataLen);
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
bool ESP8266_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data)
{

    uint8_t result;
    bool returnVal = false;
    do
    {
        ESP8266_RxClear();
        if (hESP8266.TcpIpMultiConnection == false)
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSENDBUF=%d\r\n", dataLen);
        else
            sprintf((char *)hESP8266.TxBuffer, "AT+CIPSENDBUF=%d,%d\r\n", LinkId, dataLen);
        if (ESP8266_SendString((char *)hESP8266.TxBuffer) == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 3, ">", "ERROR", "busy") == false)
            break;
        if (result > 1)
            break;
        ESP8266_RxClear();
        ESP8266_SendRaw(data, dataLen);
        if (ESP8266_WaitForString(_ESP8266_WAIT_TIME_LOW, &result, 2, "OK", "ERROR") == false)
            break;
        returnVal = true;
    } while (0);

    return returnVal;
}
//#########################################################################################################
