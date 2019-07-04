#include "DRV/ESP8266/Wifi.h"
#include "FUN/Network.h"
#include "cmsis_os.h"

#include <time.h>

osThreadId NetworkTaskHandle;

uint8_t NetworkBuffer[MAX_LINKID][_WIFI_RX_FOR_DATA_SIZE];
bool NetworkBufferAvailable[MAX_LINKID];
uint16_t NetworkBufferLen[MAX_LINKID];

void Network_Task(void const *argument)
{
	osDelay(3000);
	Wifi_SendStringAndWait("AT\r\n", 1000);
	Wifi_SetRfPower(82);
	Wifi_TcpIp_GetMultiConnection();
	for (uint8_t i = 0; i < _WIFI_MAX_TCPIP_CONNECTIONS; ++i)
		Wifi_TcpIp_Close(i);
	Wifi_TcpIp_SetMultiConnection(true);
	Wifi_GetMode();
	Wifi_Station_DhcpIsEnable();
	//#######################
	Wifi_SetMode(WifiMode_Station);
	while (Wifi_Station_ConnectToAp("testap", "12345678", NULL) == false)
		;
	struct tm timer;
	Wifi_STNP_GetTime(&timer);
	//#######################
	while (1)
	{
		Wifi_GetMyIp();
		if ((Wifi.Mode == WifiMode_SoftAp) || (Wifi.Mode == WifiMode_StationAndSoftAp))
			Wifi_SoftAp_GetConnectedDevices();
		Wifi_TcpIp_GetConnectionStatus();
		Wifi_RxClear();
		for (uint8_t i = 0; i < 100; ++i)
		{
			if (Wifi.GotNewData == true)
			{
				for (uint8_t j = 0; j < MAX_LINKID; ++j)
				{
					if (Wifi.RxDataConnectionNumber == Wifi.TcpIpConnections[j].LinkId)
					{
						memcpy(NetworkBuffer[j], Wifi.RxBufferForData, Wifi.RxDataLen);
						NetworkBufferLen[j] = Wifi.RxDataLen;
						NetworkBufferAvailable[j] = true;
					}
				}
				Wifi.GotNewData = false;
			}
			osDelay(10);
		}
	}
}

void Network_Task_Init(void)
{
	memset(NetworkBufferAvailable, false, sizeof(NetworkBufferAvailable));
	osThreadDef(NetworkTaskName, Network_Task, osPriorityNormal, 0, 128);
	NetworkTaskHandle = osThreadCreate(osThread(NetworkTaskName), NULL);
}
