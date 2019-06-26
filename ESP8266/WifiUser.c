
#include "Wifi.h"

// bool isConnected = false;

//#######################################################################################
void Wifi_UserInit(void)
{
    Wifi_SetMode(WifiMode_Station);
    //Wifi_SetMode(WifiMode_SoftAp);

    while (Wifi_Station_ConnectToAp("testap", "12345678", NULL) == false)
        ;
}
//#######################################################################################
extern bool NTP_GetTimeSend();
void Wifi_UserProcess(void)
{
    static uint8_t last = 0;
    switch (last)
    {
    case 0:
        if (strstr(Wifi.MyIP, "0.0.0.0") == NULL)
        {
            if (last != 1)
                ;
            last = 1;
        }
        break;
    case 1:
        if (Wifi_TcpIp_StartUdpConnection(0, "ntp1.aliyun.com", 123, 10))
            last = 2;
        break;
    case 2:
        if (NTP_GetTimeSend())
            last = 3;
        break;
    default:
        break;
    }
}
//#######################################################################################
void Wifi_UserGetUdpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data)
{
    // Wifi_TcpIp_SendDataUdp(LinkId, 2, (uint8_t *)"OK");
    extern bool NTP_GetTimeRecv(uint8_t * data, uint16_t len);
    if (NTP_GetTimeRecv(Data, DataLen))
    {
        Wifi_TcpIp_Close(0);
    }
}
//#######################################################################################
void Wifi_UserGetTcpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data)
{
    // Wifi_TcpIp_SendDataTcp(LinkId, 2, (uint8_t *)"OK");
}
//#######################################################################################
