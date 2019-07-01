#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    TCPIP_TCP,
    TCPIP_UDP
} TCPIP_PROTOTYPE;

#define MAX_LINKID _WIFI_MAX_TCPIP_CONNECTIONS
#define ERROR_LINKID 0xFF

typedef uint8_t LinkID;

LinkID Network_ConnectBlock(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, uint32_t timeout);
LinkID Network_ConnectAsyn(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, void (*ConnectCompleteCallback)(bool flag));
void Network_WriteBlock(LinkID id, uint8_t *data, size_t len, uint32_t timeout);
void Network_ReadBlock(LinkID id, uint8_t *data, size_t *len, uint32_t timeout);
void Network_WriteAsyn(LinkID id, uint8_t *data, size_t len, void (*WriteCompleteCallback)(size_t len));
void Network_ReadAsyn(LinkID id, size_t len, void (*ReadCompleteCallback)(uint8_t *data, size_t *len));

// IN NetworkTask
void Network_Task_Init(void);
void Network_Init(void);