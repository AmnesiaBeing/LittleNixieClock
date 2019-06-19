#include "FUN/NixieTube.h"
#include "FUN/RGBLED.h"

#include "HAL/tim.h"

/*
    这个文件负责动画效果的管理。
    动画开始，动画结束，下一帧计算。
    每一帧间隔大约2ms。
*/

// rgbled modified flag
static bool rl_mf = false;
// static uint8_t rl_buf[LEDNUM * 3] = {0};
// nixietube modified flag
static bool nt_mf = false;
char nt_buf_old[9] = "00:00:00";
char nt_buf_new[9] = "11:11:11";
char *nt_pbuf = nt_buf_old;

void Anim_Start()
{
    // 开启定时器中断
}

void Anim_Stop()
{
    // 关闭定时器中断
}

// 定时器中断调用
void Anim_Int()
{
    if (rl_mf)
    {
        RGBLED_Update();
        rl_mf = false;
    }
    if (nt_mf)
    {
        NixieTube_Show(nt_pbuf);
        nt_mf = false;
    }
    // 计算下一帧应该显示什么
    /*
        辉光管有一个切换渐变动画，在1s中，分为50个等分，20ms，20ms分成2ms的10个等分。
        在这20ms内应用占空比的概念，使原数字显示的时间和将要显示数字的时间符合以下函数。
        yA=up(1-2^x);yB=1-yA;
    */
    static const int map[50] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
                                2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
                                4, 4, 4, 4, 5, 5, 5, 5, 6, 6,
                                6, 7, 7, 7, 8, 8, 8, 9, 9, 10};
    static int i = 0;
    static int j = 0;

    if (i < map[j])
    {
        nt_pbuf = nt_buf_new;
    }
    else
    {
        nt_pbuf = nt_buf_new;
    }

    if (++i >= 10)
        i = 0;

    if (++j >= 50)
        j = 0;
}

// 开机动画、阴极保护动画（呼吸灯和数字变化）
void Anim_Protect(void)
{
    static const char str[10][9] =
        {"00:00:00", "11 11 11", "22:22:22", "33 33 33", "44:44:44",
         "55 55 55", "66:66:66", "77 77 77", "88:88:88", "99 99 99"};
    uint8_t c = 10; /* 10-240 */

    for (int i = 0; i < 10; ++i, HAL_Delay(100))
    {
        NixieTube_Show(str[i]);
        RGBLED_Clear(c, c, c);
        RGBLED_Update();
        c += 23;
    }
    for (int i = 10; i > 0; --i, HAL_Delay(100))
    {
        NixieTube_Show(str[i]);
        RGBLED_Clear(c, c, c);
        RGBLED_Update();
        c -= 23;
    }
}