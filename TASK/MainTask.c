#include "DRV/DS3231.h"
#include "DRV/PWR.h"
#include "DRV/Button.h"
#include "DRV/ESP8266/Wifi.h"
#include "DRV/W25QXX.h"
#include "DRV/WS2812.h"

#include "FUN/Anim.h"
#include "FUN/NixieTube.h"
#include "FUN/NTP.h"
#include "FUN/Network.h"

#include "cmsis_os.h"

#include <time.h>
#include <string.h>

static void MainTask(void const *argument);
static void AnimTask(void const *argument);
static void TimeUpdateTask(void const *argument);

osThreadDef(MainTaskName, MainTask, osPriorityNormal, 0, 128);
osThreadDef(AnimTaskName, AnimTask, osPriorityNormal, 0, 128);
osThreadDef(TimeUpdateTaskName, TimeUpdateTask, osPriorityNormal, 0, 128);

osThreadId AnimTaskHandle;
osThreadId ConfigTaskHandle;
osThreadId MainTaskHandle;
osThreadId TimeUpdateTaskHandle;

// 动画线程的信号量
osSemaphoreDef(AnimSemName);
osSemaphoreId AnimSemHandle;
osMessageQDef(AnimQueueName, 3, uint32_t);
osMessageQId AnimQueueHandle;

extern void DebugMode_Init(void);
extern osSemaphoreId DebugModeIntoSemHandle;
extern osSemaphoreId DebugModeExitSemHandle;

// 这个表示来自于DS3231的中断，需要更新时间了。
static bool flag1 = false;
static void callback1(void)
{
    flag1 = true;
}

// 这个表示按钮按下的flag，需要进入配对模式什么的。
static bool flag2 = false;
static void callback2(void)
{
    flag2 = true;
}

// BootTask
/*
    该任务负责：
    1、创建线程播放开机动画直到超时【或者从FLASH中读取到配置信息】
    【1-×、如果从FLASH中读取到上次配置的信息】或电池存在
    1-1、电池存在，说明，不需要马上从互联网获取时间
    1-2、电池不存在，开启一个新的线程来获取时间（网络线程应该跑起来了）
    2、进入正常显示时间模式，并开启一个新的线程创建TCPServer（如果AP都没有连接上，则等待Button中断）
    3、正常显示时间模式，需要持续的显示时间是否超过了9分钟，如果超过了，执行动画
    ×、暂时不考虑FLASH芯片，代码还没写好
    ×、电池不存在且网络不存在，就随意吧
*/
static void MainTask(void const *argument)
{
    // 开启5V电源并做一些初始化工作
    PWR_5V_ON();
    WS2812_Init();
    HV507_Init();

    // 开启VPP的电源
    PWR_VPP_ON();

    // 上述做完，至少能有个开机动画，开个线程
#if 0
    AnimSemHandle = osSemaphoreCreate(osSemaphore(AnimSemName), 1);
    osSemaphoreWait(AnimSemHandle, 0); /* 不能能停在这吧 */
    AnimQueueHandle = osMessageCreate(osMessageQ(AnimQueueName), NULL);
    AnimTaskHandle = osThreadCreate(osThread(AnimTaskName), NULL);
    osMessagePut(AnimQueueHandle, 1, 0); /* 0表示非阻塞 */
#endif
    // 继续初始化
    Button_Init();
    DS3231_Init();
    W25QXX_Init();
#if 1
    Wifi_Init();
    Network_Init();
#endif
#if 0
    DebugMode_Init();
#endif
#if 0
    // 判断配置是否存在，存在就读取其中WifiAP的SSID和PASSWORD（非阻塞）
#endif

#if 0
    // 然后，更新时间
    TimeUpdateTaskHandle = osThreadCreate(osThread(TimeUpdateTaskName), NULL);
#endif

    // 如果连配置信息都木有，emmm，将就吧，直到按钮按下，进入配置模式，总能设置点什么东西的

    // 加载完成，显示时间
    // ok:
    struct tm timer;
    if (!DS3231_GetTime(&timer))
    {
        /* 还有获取失败的？那凉凉了 */
    }
    extern time_t time_dat;
    time_dat = mktime(&timer);

    osSemaphoreWait(AnimSemHandle, 0);

    // MainTask
    // 负责显示时间的循环
    // 那必须是正常工作了啦
    DS3231_Register_Callback(callback1);
    DS3231_Set1HzSQW();

#if 0
    Button_Register_Callback(callback2);
#endif
    while (1)
    {
        if (flag1)
        {
            flag1 = false;
            localtime_r(&time_dat, &timer);
            NixieTube_ShowTM(&timer);
        }
        if (flag2)
        {
            // 按钮按下，表示需要进入配对模式或者再次连接指定的AP热点
            flag2 = false;
            // 不管咋样，给个动画效果，表示，emmm，按钮按下了
            osMessagePut(AnimQueueHandle, 2, 0);
            // 然后就阻塞在这，直到调试模式结束
            osSemaphoreWait(DebugModeExitSemHandle, 0);
        }
    }
}

// freertos和线程安全没有学到家的感觉
// 应该有办法传递某种互斥信号才对的
// 播放特定动画的任务
static void AnimTask(void const *argument)
{
    while (1)
    {
        osEvent ev = osMessageGet(AnimQueueHandle, osWaitForever); /* 阻塞在这，直到要播放某个动画效果 */
        if (ev.status == osEventMessage)
        {
            uint32_t index = ev.value.v;
            void (*p)(void) = &Anim_1;
            switch (index)
            {
            case 1:
                p = &Anim_1;
                break;
            default:
                break;
            }
            p();
            osSemaphoreRelease(AnimSemHandle);
        }
    }
}

static void TimeUpdateTask(void const *argument)
{
    time_t t;
    struct tm timer;
    if (NTP_GetTime(&t))
    {
        extern time_t time_dat;
        time_dat = t;
        localtime_r(&t, &timer);
        DS3231_SetTime(&timer);
    }
}

void Boot(void)
{
    osThreadCreate(osThread(MainTaskName), NULL);
}