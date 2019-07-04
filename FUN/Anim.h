#pragma once

// 为了达成，动画至少要播放完一次的目标，和播放哪个动画的参数，只能做个结构体了
typedef struct 
{
    volatile bool flag;
    uint8_t index;
}Anim_Ctl_t;

void Anim_1(void);