#pragma once

#include <stdint.h>

/*
    配置信息结构体。在FLASH的0x00开始存放，大小为一个扇区256B（方便写入）
*/

#define MAGICNUMBER 'z''z''h'

typedef struct _Config_s
{
    uint64_t MagicNumber; /* 标志魔数， */
}Config_t;