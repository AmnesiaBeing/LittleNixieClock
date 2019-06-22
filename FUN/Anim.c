#include "FUN/NixieTube.h"
#include "FUN/RGBLED.h"

#include "HAL/tim.h"

#include "cmsis_os.h"
/*
    这个文件负责动画效果的管理。
    动画开始，动画结束，下一帧计算。
    每一帧间隔大约2ms。
*/

// 计算下一帧应该显示什么
/*
        辉光管有一个切换渐变动画，在1s中，分为50个等分，20ms，20ms分成2ms的10个等分。
        在这20ms内应用占空比的概念，使原数字显示的时间和将要显示数字的时间符合以下函数。
        yA=up(1-2^x);yB=1-yA;
    */
// static const int map[50] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//                             0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
//                             2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
//                             4, 4, 4, 4, 5, 5, 5, 5, 6, 6,
//                             6, 7, 7, 7, 8, 8, 8, 9, 9, 10};

// 开机动画、阴极保护动画（呼吸灯和数字变化）
void Anim_Protect(void)
{
    static const char str[10][9] =
        {"00:00:00", "11 11 11", "22:22:22", "33 33 33", "44:44:44",
         "55 55 55", "66:66:66", "77 77 77", "88:88:88", "99 99 99"};
    uint8_t c = 10; /* 10-240 */

    for (int i = 0; i < 10; ++i)
    {
        NixieTube_Show(str[i]);
        RGBLED_Clear(c, c, c);
        RGBLED_Update();
        c += 23;
        osDelay(200);
    }
    for (int i = 10; i > 0; --i)
    {
        NixieTube_Show(str[i]);
        RGBLED_Clear(c, c, c);
        RGBLED_Update();
        c -= 23;
        osDelay(200);
    }
}