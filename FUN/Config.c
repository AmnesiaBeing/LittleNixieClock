#include "FUN/Config.h"
#include "DRV/W25QXX.h"

// 调用该文件的函数之前需确保Flash芯片初始化完成

/* 
    配置信息存放在前10个扇区，初始化时按顺序对这些扇区读取数据，判断哪个时间戳比较大，大的那个作为配置
*/

void Config_Init(void)
{
    // W25QXX_ReadSector();
}