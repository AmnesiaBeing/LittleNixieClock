/* c标准库中和time.h有关的实现 */

// 参考链接：https://blog.csdn.net/makeworks/article/details/78244671

#include <time.h>

time_t time_dat;

#if _DLIB_TIME_USES_64
time_t __time64 (time_t *p) {
    return time_dat;
}
#else
time_t __time32 (time_t *p) {
    return time_dat;
}
#endif

const char * __getzone(void)
{
    // UTC+8 北京时间
    // 下个版本改成可以更改时区的配置
    return ":GMT+8:GMT+9:+0800";
}