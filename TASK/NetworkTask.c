#include "DRV/ESP8266/Wifi.h"
#include "cmsis_os.h"

osThreadId NetworkTaskHandle;

void Network_Task(void const *argument)
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

	//#######################
	while (1)
	{
		Wifi_GetMyIp();
		if ((Wifi.Mode == WifiMode_SoftAp) || (Wifi.Mode == WifiMode_StationAndSoftAp))
			Wifi_SoftAp_GetConnectedDevices();
		Wifi_TcpIp_GetConnectionStatus();
	}
}

void Network_Task_Init(void)
{
	osThreadDef(NetworkTaskName, Network_Task, osPriorityNormal, 0, 128);
	NetworkTaskHandle = osThreadCreate(osThread(NetworkTaskName), NULL);
}
