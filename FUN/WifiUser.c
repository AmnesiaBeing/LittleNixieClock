
#include "Wifi.h"

osThreadId WifiTaskHandle;
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	Wifi_RxCallBack();
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void WifiTask(void const *argument)
{
	osDelay(3000);
	Wifi_SendStringAndWait("AT\r\n", 1000);
	Wifi_SetRfPower(82);
	Wifi_TcpIp_GetMultiConnection();
	Wifi_TcpIp_Close(0);
	Wifi_TcpIp_Close(1);
	Wifi_TcpIp_Close(2);
	Wifi_TcpIp_Close(3);
	Wifi_TcpIp_Close(4);
	Wifi_TcpIp_SetMultiConnection(true);
	Wifi_GetMode();
	Wifi_Station_DhcpIsEnable();
	Wifi_UserInit();

	//#######################
	while (1)
	{
		Wifi_GetMyIp();
		if ((Wifi.Mode == WifiMode_SoftAp) || (Wifi.Mode == WifiMode_StationAndSoftAp))
			Wifi_SoftAp_GetConnectedDevices();
		Wifi_TcpIp_GetConnectionStatus();
		Wifi_RxClear();
		for (uint8_t i = 0; i < 100; i++)
		{
			if (Wifi.GotNewData == true)
			{
				Wifi.GotNewData = false;
				for (uint8_t ii = 0; ii < 5; ii++)
				{
					if ((strstr(Wifi.TcpIpConnections[ii].Type, "UDP") != NULL) && (Wifi.RxDataConnectionNumber == Wifi.TcpIpConnections[ii].LinkId))
						Wifi_UserGetUdpData(Wifi.RxDataConnectionNumber, Wifi.RxDataLen, Wifi.RxBufferForData);
					if ((strstr(Wifi.TcpIpConnections[ii].Type, "TCP") != NULL) && (Wifi.RxDataConnectionNumber == Wifi.TcpIpConnections[ii].LinkId))
						Wifi_UserGetTcpData(Wifi.RxDataConnectionNumber, Wifi.RxDataLen, Wifi.RxBufferForData);
				}
			}
			osDelay(10);
		}
		Wifi_UserProcess();
	}
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
void Wifi_Init(osPriority Priority)
{

	osThreadDef(WifiTaskName, WifiTask, Priority, 0, _WIFI_TASK_SIZE);
	WifiTaskHandle = osThreadCreate(osThread(WifiTaskName), NULL);
}
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
//#########################################################################################################
