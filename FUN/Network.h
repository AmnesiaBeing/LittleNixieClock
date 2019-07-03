#pragma once

#include "DRV/ESP8266/Wifi.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    TCPIP_TCP,
    TCPIP_UDP,
    TCPIP_UNKNOWN
} TCPIP_PROTOTYPE;

// 代码写得不好，如果用最多的5的话，太浪费空间了
#define MAX_LINKID 3
#define ERROR_LINKID 0xFF

typedef uint8_t LinkID;

LinkID Network_ConnectBlock(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort);
// void Network_ConnectAsyn(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, void (*ConnectCompleteCallback)(bool flag, LinkID id));
bool Network_WriteBlock(LinkID id, uint8_t *data, size_t len);
bool Network_ReadBlock(LinkID id, uint8_t *data, size_t *len, uint32_t timeout);
// void Network_WriteAsyn(LinkID id, uint8_t *data, size_t len, void (*WriteCompleteCallback)(size_t len));
// void Network_ReadAsyn(LinkID id, size_t len, void (*ReadCompleteCallback)(uint8_t *data, size_t *len));

// IN NetworkTask
void Network_Task_Init(void);
void Network_Init(void);

extern uint8_t NetworkBuffer[MAX_LINKID][_WIFI_RX_FOR_DATA_SIZE];
extern bool NetworkBufferAvailable[MAX_LINKID];
extern uint16_t NetworkBufferLen[MAX_LINKID];