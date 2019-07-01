#pragma once

#include "FUN/RGBLED.h"

#include <stdint.h>

/*
    配置信息结构体。在FLASH的0x00开始存放，大小为一个扇区256B（方便写入）
*/

// emmmmmmmmmmmmmmmmmmm
#define MAGICNUMBER 0x621172314F60

typedef struct _Config_s
{
    uint64_t MagicNumber;           /* 标志魔数，表示这个FLASH曾经也是被写入过的 */
    uint32_t ConfigVersion;         /* 该配置信息结构体的版本号 */
    uint32_t LastModifiedTimeStamp; /* 上一次写入的时间戳，用32位的time_t转就行 */
    char WifiSSID[32];              /* 别跟我说ssid那么长 */
    char WifiPassword[32];          /* 别跟我说密码也那么长 */
    uint32_t AlarmMusicOffset;      /* 暂时只支持一首闹钟曲子吧，曲子在FLASH中的偏移量，以字节为单位 */
    uint8_t AlarmNum;               /* 剩下的都是闹钟，闹钟数量最大为6吧 */
    uint32_t Alarm[6];              /* 闹钟的时间 */
    /* 剩下的存什么想不到了 */
} Config_t;

// sizeof(Config_t)=152