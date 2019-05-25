#pragma once

#include "usart.h"
#include "gpio.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#########################################################
#define _ESP8266_USART huart1

#define _ESP8266_RX_SIZE 512
#define _ESP8266_RX_FOR_DATA_SIZE 1024
#define _ESP8266_TX_SIZE 256
#define _ESP8266_TASK_SIZE 512

#define _ESP8266_WAIT_TIME_LOW 1000
#define _ESP8266_WAIT_TIME_MED 10000
#define _ESP8266_WAIT_TIME_HIGH 25000
#define _ESP8266_WAIT_TIME_VERYHIGH 60000
//#########################################################
typedef enum
{
    ESP8266_Mode_Error = 0,
    ESP8266_Mode_Station = 1,
    ESP8266_Mode_SoftAp = 2,
    ESP8266_Mode_StationAndSoftAp = 3,
} ESP8266_Mode_t;

typedef enum
{
    ESP8266_EncryptionType_Open = 0,
    ESP8266_EncryptionType_WPA_PSK = 2,
    ESP8266_EncryptionType_WPA2_PSK = 3,
    ESP8266_EncryptionType_WPA_WPA2_PSK = 4,
} ESP8266_EncryptionType_t;
typedef enum
{
    ESP8266_ConnectionStatus_Error = 0,
    ESP8266_ConnectionStatus_GotIp = 2,
    ESP8266_ConnectionStatus_Connected = 3,
    ESP8266_ConnectionStatus_Disconnected = 4,
    ESP8266_ConnectionStatus_ConnectionFail = 5,
} ESP8266_ConnectionStatus_t;

typedef struct
{
    ESP8266_ConnectionStatus_t status;
    uint8_t LinkId;
    char Type[4];
    char RemoteIp[17];
    uint16_t RemotePort;
    uint16_t LocalPort;
    bool RunAsServer;
} ESP8266_Connection_t;
//###################################################################################################
typedef struct
{
    //----------------Usart	Paremeter
    uint8_t usartBuff;
    uint8_t RxBuffer[_ESP8266_RX_SIZE];
    uint8_t TxBuffer[_ESP8266_TX_SIZE];
    uint16_t RxIndex;
    uint8_t RxBufferForData[_ESP8266_RX_FOR_DATA_SIZE];
    uint8_t RxBufferForDataTmp[8];
    uint8_t RxIndexForDataTmp;
    uint16_t RxIndexForData;
    uint16_t RxDataLen;
    uint8_t RxDataConnectionNumber;
    uint32_t RxDataLastTime;
    bool RxIsData;
    bool GotNewData;
    //----------------General	Parameter
    ESP8266_Mode_t Mode;
    char MyIP[16];
    char MyGateWay[16];
    //----------------Station	Parameter
    bool StationDhcp;
    char StationIp[16];
    //----------------SoftAp Parameter
    bool SoftApDhcp;
    char SoftApConnectedDevicesIp[6][16];
    char SoftApConnectedDevicesMac[6][18];
    //----------------TcpIp Parameter
    bool TcpIpMultiConnection;
    uint16_t TcpIpPingAnswer;
    ESP8266_Connection_t TcpIpConnections[5];
    //----------------
} ESP8266_t;
//###################################################################################################
extern ESP8266_t hESP8266;

//###################################################################################################
void ESP8266_UserInit(void);
void ESP8266_UserProcess(void);
void ESP8266_UserGetUdpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data);
void ESP8266_UserGetTcpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data);

//###################################################################################################
void ESP8266_RxCallBack(void);
//###################################################################################################
void ESP8266_Init();
//###################################################################################################
bool ESP8266_Restart(void);
bool ESP8266_DeepSleep(uint16_t DelayMs);
bool ESP8266_FactoryReset(void);
bool ESP8266_Update(void);
bool ESP8266_SetRfPower(uint8_t Power_0_to_82);
//###################################################################################################
bool ESP8266_SetMode(ESP8266_Mode_t ESP8266Mode_);
bool ESP8266_GetMode(void);
bool ESP8266_GetMyIp(void);
//###################################################################################################
bool ESP8266_Station_ConnectToAp(char *SSID, char *Pass, char *MAC);
bool ESP8266_Station_Disconnect(void);
bool ESP8266_Station_DhcpEnable(bool Enable);
bool ESP8266_Station_DhcpIsEnable(void);
bool ESP8266_Station_SetIp(char *IP, char *GateWay, char *NetMask);
//###################################################################################################
bool ESP8266_SoftAp_GetConnectedDevices(void);
bool ESP8266_SoftAp_Create(char *SSID, char *password, uint8_t channel, ESP8266_EncryptionType_t ESP8266EncryptionType, uint8_t MaxConnections_1_to_4, bool HiddenSSID);
//###################################################################################################
bool ESP8266_TcpIp_GetConnectionStatus(void);
bool ESP8266_TcpIp_Ping(char *PingTo);
bool ESP8266_TcpIp_SetMultiConnection(bool EnableMultiConnections);
bool ESP8266_TcpIp_GetMultiConnection(void);
bool ESP8266_TcpIp_StartTcpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut_S);
bool ESP8266_TcpIp_StartUdpConnection(uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t LocalPort);
bool ESP8266_TcpIp_Close(uint8_t LinkId);
bool ESP8266_TcpIp_SetEnableTcpServer(uint16_t PortNumber);
bool ESP8266_TcpIp_SetDisableTcpServer(uint16_t PortNumber);
bool ESP8266_TcpIp_SendDataUdp(uint8_t LinkId, uint16_t dataLen, uint8_t *data);
bool ESP8266_TcpIp_SendDataTcp(uint8_t LinkId, uint16_t dataLen, uint8_t *data);
