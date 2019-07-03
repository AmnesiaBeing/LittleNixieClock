#include "FUN/Network.h"
#include "DRV/ESP8266/Wifi.h"

#include <stdbool.h>

// 肯定用不完
static uint16_t localport = 1000;

static bool AvailLinkID[MAX_LINKID] = {};

static LinkID GetAvailLinkID(void)
{
    for (LinkID i = 0; i < MAX_LINKID; ++i)
    {
        if (AvailLinkID[i])
            return i;
    }
    return ERROR_LINKID;
}

LinkID Network_ConnectBlock(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, uint32_t timeout)
{
    LinkID ret = GetAvailLinkID();
    if (ret == 0xFF)
        return ret;
    switch (type)
    {
    case TCPIP_TCP:
        if (!Wifi_TcpIp_StartTcpConnection(ret, RemoteIp, RemotePort, timeout))
            return 0xFF;
        AvailLinkID[ret] = false;
        break;
    case TCPIP_UDP:
        if (!Wifi_TcpIp_StartUdpConnection(ret, RemoteIp, RemotePort, localport++))
            return 0xFF;
        AvailLinkID[ret] = false;
        break;
    default:
        break;
    }
    return ret;
}

void Network_ConnectAsyn(TCPIP_PROTOTYPE type, char *RemoteIp, uint16_t RemotePort, void (*ConnectCompleteCallback)(bool flag, LinkID id))
{
}

static TCPIP_PROTOTYPE getPrototype(LinkID id)
{
    if (Wifi.TcpIpConnections[id].Type[0] == 'T')
        return TCPIP_TCP;
    if (Wifi.TcpIpConnections[id].Type[0] == 'U')
        return TCPIP_UDP;
    return TCPIP_UNKNOWN;
}

bool Network_WriteBlock(LinkID id, uint8_t *data, size_t len, uint32_t timeout)
{
    switch (getPrototype(id))
    {
    case TCPIP_TCP:
        return Wifi_TcpIp_SendDataTcp(id, len, data);
    case TCPIP_UDP:
        return Wifi_TcpIp_SendDataUdp(id, len, data);
    default:
        break;
    }
    return false;
}

// 要求，data在调用方申请（不想搞malloc）
bool Network_ReadBlock(LinkID id, uint8_t *data, size_t *len, uint32_t timeout)
{
    uint32_t t = HAL_GetTick();
    // osSemaphoreWait(Wifi)
    // if (Wifi.GotNewData)
    // {
    // }
    return false;
}

void Network_WriteAsyn(LinkID id, uint8_t *data, size_t len, void (*WriteCompleteCallback)(size_t len))
{
}

void Network_ReadAsyn(LinkID id, size_t len, void (*ReadCompleteCallback)(uint8_t *data, size_t *len))
{
}

void Network_Init(void)
{
    memset(AvailLinkID, true, MAX_LINKID);
    Network_Task_Init();
}