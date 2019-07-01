#include "TASK/Tasks.h"
#include "DRV/DS3231.h"
#include "DRV/PWR.h"
#include "cmsis_os.h"

osThreadId AnimTaskHandle;
osThreadId ConfigTaskHandle;
// osThreadId AnimTaskHandle;
// osThreadId AnimTaskHandle;
// osThreadId AnimTaskHandle;

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
void BootTask(void const *argument)
{
    // 开启5V和VPP的电源
    PWR_5V_ON();
    PWR_VPP_ON();

    // 开机动画线程，NULL表示动画随机
    // osThreadDef(AnimTaskName, AnimTask, osPriorityNormal, 0, 128);
    // AnimTaskHandle = osThreadCreate(osThread(AnimTaskName), NULL);

    // 判断电池是否存在
    bool BAT = false; // = PWR_BAT_IsAvailable();
    // 加载配置
    // Config_Read();
    // 判断配置是否存在
    bool CFG = false; // =

    // 如果电池不存在，配置不存在，测试模式，从串口中读取字符并执行相应的命令

    // 如果电池存在，配置存在，说明已经可以正常工作了

    // 如果电池存在，配置不存在，说明这是第一次启动，需要初始化配置信息，时间也需要矫正

    // 如果电池不存在，配置存在，可能是没电了，说明时间需要矫正

    // 加载完成，显示时间

    // BootTask结束
    osThreadTerminate(NULL);
}

static bool flag1 = false;
static void callback(void)
{
    flag1 = true;
}

// 负责显示时间的循环
void ShowTimeTask(void const *argument)
{
    DS3231_Register_Callback(callback);
    // while()
}

void AnimTask(void const *argument)
{
    uint8_t i = *(uint8_t *)argument;
}