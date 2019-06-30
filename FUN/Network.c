#include "FUN/Network.h"
#include "DRV/ESP8266/Wifi.h"

static localport = 1000;

static bool AvailLinkID[];

LinkID Network_ConnectBlock(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, uint32_t timeout)
{
    LinkID ret;
    switch (type)
    {
    case TCPIP_TCP:
        Wifi_TcpIp_StartTcpConnection();
        break;

    default:
        break;
    }
}

LinkID Network_ConnectAsyn(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, void (*ConnectCompleteCallback)(bool flag))
{
}

void Network_WriteBlock(LinkID id, uint8_t *data, size_t len, uint32_t timeout)
{
}

void Network_ReadBlock(LinkID id, uint8_t *data, size_t *len, uint32_t timeout)
{
}

void Network_WriteAsyn(LinkID id, uint8_t *data, size_t len, void (*WriteCompleteCallback)(size_t len))
{
}

void Network_ReadBlock(LinkID id, size_t len, void (*ReadCompleteCallback)(uint8_t *data, size_t *len))
{
}