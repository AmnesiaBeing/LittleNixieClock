/* 为了方便调试，按下按钮进入调试模式，测试某些函数 */

#include "DRV/HV507.h"
#include "FUN/RGBLED.h"

#include "cmsis_os.h"

void DebugMode(void)
{
    // 关闭中断，其他函数就别动了
    HV507_SendData(0);
    RGBLED_Clear(0, 0, 0);
    RGBLED_Update();

    while (1)
        ;
}