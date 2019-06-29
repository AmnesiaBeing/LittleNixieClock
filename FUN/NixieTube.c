/* 
    HV507 1-64
    N60 N69-N61 N50 N59-N51 ND4 ND3 N49 N40 N48-N41 N30 N39-N33 N31 N32 ND2 ND1 N20 N29-N21 N10 N19-N11
    Left<------|-----------|-:---:-|---------------|-------------------|-:---:-|-----------|----->Right
*/
#include "FUN/NixieTube.h"

// 6个灯，10个数字的移位硬编码表
static uint32_t NTable[6][10] = {
    /*         0,  1,  2,  3,  4,  5,  6,  7,  8,  9*/
    /* N1 */ {54, 63, 62, 61, 60, 59, 58, 57, 56, 55}, /* Hour */
    /* N2 */ {44, 53, 52, 51, 50, 49, 48, 47, 46, 45}, /* Hour */
    /* N3 */ {32, 40, 41, 39, 38, 37, 36, 35, 34, 33}, /* Min  */
    /* N4 */ {23, 31, 30, 29, 28, 27, 26, 25, 24, 22}, /* Min  */
    /* N5 */ {10, 19, 18, 17, 16, 15, 14, 13, 12, 11}, /* Sec  */
    /* N6 */ {0, 9, 8, 7, 6, 5, 4, 3, 2, 1},           /* Sec  */
};

// 中间的冒号们
#define ND4 20U
#define ND3 21U
#define ND2 42U
#define ND1 43U

#define NDMASK1 (uint64_t)((1ULL << ND1) | (1ULL << ND2))
#define NDMASK2 (uint64_t)((1ULL << ND3) | (1ULL << ND4))

// 输入字符串"12 34:56"，显示对应的数字和冒号
// 输入"-- -- --"表示不亮
void NixieTube_ShowStr(const char *str)
{
    const static uint8_t map[] = {7, 6, 4, 3, 1, 0};
    char c;
    uint64_t res = 0;

    for (int i = 0; i < 6; i++)
    {
        c = str[map[i]] - '0';
        if ((c >= 0) && (c <= 9))
        {
            res |= (1ULL << NTable[i][(int)c]);
        }
    }

    if (str[2] == ':')
        res |= NDMASK1;
    if (str[5] == ':')
        res |= NDMASK2;

    HV507_SendData(res);
}

// 个位的英文怎么说啊
static inline int GetOneDigit(int a)
{
    return a % 10;
}

static inline int GetTenDigit(int a)
{
    return a / 10;
}

// 输入struct tm结构体，取出其中的时分秒，并显示
// 根据秒的奇偶性确定冒号是否显示：奇显偶不显
// 时区部分不需要在这里考虑
void NixieTube_ShowTM(struct tm *tm)
{
    int hour = tm->tm_hour;
    int min = tm->tm_min;
    int sec = tm->tm_sec;

    uint64_t res = 0;

    // 不想检查数字范围是否在0~99了
    res |= NTable[0][GetTenDigit(hour)];
    res |= NTable[1][GetOneDigit(hour)];
    res |= NTable[2][GetTenDigit(min)];
    res |= NTable[3][GetOneDigit(min)];
    res |= NTable[4][GetTenDigit(sec)];
    res |= NTable[5][GetOneDigit(sec)];
    
    // 如果sec为奇数
    if (sec % 2)
    {
        res |= (NDMASK1 | NDMASK2);
    }

    HV507_SendData(res);
}