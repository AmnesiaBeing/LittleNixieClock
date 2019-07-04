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

osThreadId AnimTaskHandle;
osThreadId ConfigTaskHandle;
osThreadId MainTaskHandle;
osThreadId TimeUpdateTaskHandle;

static void MainTask(void const *argument);
static void AnimTask(void const *argument);
static void TimeUpdateTask(void const *argument);

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

    // 上述做完，至少能有个开机动画，开个线程，动画线程里有osDelay，优先级高一点没事的
    Anim_Ctl_t ac = {false, 1};
    osThreadDef(AnimTaskName, AnimTask, osPriorityNormal, 0, 128);
    AnimTaskHandle = osThreadCreate(osThread(AnimTaskName), (void *)&ac);

    // 继续初始化
    Button_Init();
    DS3231_Init();
    W25QXX_Init();
#if 1
    Wifi_Init();
    Network_Init();
#endif
    // // 判断电池是否存在
    // // bool BAT = false; // = PWR_BAT_IsAvailable();
    // // 加载配置
    // // Config_Read();
    // // 判断配置是否存在
    // // 不存在则使用默认配置
    // // bool CFG = false; // = Config_Init();

    // // 如果电池存在，配置存在，说明已经可以正常工作了
    // // 如果电池存在，配置不存在，说明这是第一次启动，需要初始化配置信息，时间也需要矫正
    // // 如果电池不存在，配置存在，可能是没电了，说明时间需要矫正
    // osThreadDef(TimeUpdateTaskName, TimeUpdateTask, osPriorityNormal, 0, 128);
    // TimeUpdateTaskHandle = osThreadCreate(osThread(TimeUpdateTaskName), NULL);
    // // 如果电池不存在，配置不存在，测试模式，从串口中读取字符并执行相应的命令
    // 加载完成，显示时间
    // ok:
    struct tm timer;
    if (!DS3231_GetTime(&timer))
    {
        /* 还有获取失败的？那凉凉了 */
    }
    extern time_t time_dat;
    time_dat = mktime(&timer);
    // 这样结束进程真的可以吗？上锁结束自己
    ac.flag = false;
    while (!ac.flag)
        ;

    // MainTask
    // 负责显示时间的循环
    // 那必须是正常工作了啦
    DS3231_Register_Callback(callback1);
    DS3231_Set1HzSQW();

    extern void DebugMode(void);
    Button_Register_Callback(callback2);

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
            Anim_Ctl_t ac = {false, 1};
            // osThreadDef(AnimTaskName, AnimTask, osPriorityNormal, 0, 128);
            // AnimTaskHandle = osThreadCreate(osThread(AnimTaskName), (void *)&ac);
            while (1)
                ;
        }
    }
}

// freertos和线程安全没有学到家的感觉
// 应该有办法传递某种互斥信号才对的，而不是这么丑陋地传个可修改的bool，而且还是线程不安全的情况下
// 播放特定动画的任务
static void AnimTask(void const *argument)
{
    Anim_Ctl_t *const ac = (Anim_Ctl_t *)argument;
    volatile bool *const flag = &(ac->flag);
    void (*p)(void);
    switch (ac->index)
    {
    case 1:
        p = &Anim_1;
        break;
    default:
        p = &Anim_1;
        break;
    }

    do
    {
        p();
    } while (*flag);

    *flag = true;
    while (1)
        ;
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
    osThreadDef(MainTaskName, MainTask, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(MainTaskName), NULL);
}