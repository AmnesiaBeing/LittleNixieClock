#include "DRV/ESP8266.h"

//#######################################################################################
void WIFI_Init(void)
{

    ESP8266_Init();

    ESP8266_SetMode(ESP8266_Mode_Station);

    //while (WIFI_Station_ConnectToAp("L70_9587", "33559366", NULL) == false)
}
void WIFI_MainLoop(void)
{
    ESP8266_MainLoop();
}
//#######################################################################################
void WIFI_UserProcess(void)
{
    static uint8_t last = 0;
    if (strstr(hESP8266.MyIP, "0.0.0.0") != NULL)
    {
        if (last != 1)
        {
        }
        last = 1;
    }
    else
    {
        if (last != 2)
        {
            ESP8266_TcpIp_StartTcpConnection(0, hESP8266.MyGateWay, 33333, 10);
        }
        last = 2;
    }
}
//#######################################################################################
void WIFI_UserGetUdpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data)
{
    ESP8266_TcpIp_SendDataUdp(LinkId, 2, (uint8_t *)"OK");
}
//#######################################################################################
void WIFI_UserGetTcpData(uint8_t LinkId, uint16_t DataLen, uint8_t *Data)
{
    ESP8266_TcpIp_SendDataTcp(LinkId, 2, (uint8_t *)"OK");
}
//#######################################################################################
