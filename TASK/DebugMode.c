#include "DRV/HV507.h"
#include "DRV/ESP8266/Wifi.h"

#include "FUN/RGBLED.h"

#include "cmsis_os.h"

#define NIXIETUBE_SSID "NixieTube"
#define NIXIETUBE_PASS "emmmmmmmm"

#define SERVERPORT 9607

void DebugModeTask(void const *argument);

osThreadDef(DebugModeTaskName, DebugModeTask, osPriorityNormal, 0, 128);
osThreadId DebugModeTaskHandle;
osSemaphoreId DebugModeIntoSemHandle;
osSemaphoreId DebugModeExitSemHandle;

void DebugModeTask(void const *argument)
{
    osSemaphoreWait(DebugModeIntoSemHandle, 0);
    if (!Wifi_SoftAp_Create(NIXIETUBE_SSID, NIXIETUBE_PASS, 2, WifiEncryptionType_WPA2_PSK, 1, false))
    {
        // ?
    }
    if (!Wifi_TcpIp_SetEnableTcpServer(SERVERPORT))
    {
        // ?
    }
}

void DebugMode_Init(void)
{
    osSemaphoreWait(DebugModeIntoSemHandle, 0);
    osSemaphoreWait(DebugModeExitSemHandle, 0);
    DebugModeTaskHandle = osThreadCreate(osThread(DebugModeTaskName), NULL);
}