#pragma once

#include <stdint.h>

typedef enum
{
    TCPIP_TCP,
    TCPIP_UDP
} TCPIP_PROTOTYPE;

typedef uint8_t LinkID;

#define ERROR_LINKID 0xFF
LinkID Network_ConnectBlock(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, uint32_t timeout);
LinkID Network_ConnectAsyn(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, void (*ConnectCompleteCallback)(bool flag));
void Network_WriteBlock(LinkID id, uint8_t *data, size_t len);
void Network_ReadBlock(LinkID id, uint8_t *data, size_t *len);
void Network_WriteAsyn(LinkID id, uint8_t *data, size_t len, void (*WriteCompleteCallback)(size_t len));
void Network_ReadBlock(LinkID id, size_t len, void (*ReadCompleteCallback)(uint8_t *data, size_t *len));