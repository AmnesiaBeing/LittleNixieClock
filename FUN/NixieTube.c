/* 
    HV507 1-64
    N60 N69-N61 N50 N59-N51 ND4 ND3 N49 N40 N48-N41 N30 N39-N33 N31 N32 ND2 ND1 N20 N29-N21 N10 N19-N11
*/

#include "FUN/NixieTube.h"

#include <stdio.h>

// 6个灯，10个数字的移位硬编码表
static uint32_t NTable[6][10] = {
    /*         0,  1,  2,  3,  4,  5,  6,  7,  8,  9*/
    /* N1 */ {54, 63, 62, 61, 60, 59, 58, 57, 56, 55},
    /* N2 */ {44, 53, 52, 51, 50, 49, 48, 47, 46, 45},
    /* N3 */ {32, 40, 41, 39, 38, 37, 36, 35, 34, 33},
    /* N4 */ {23, 31, 30, 29, 28, 27, 26, 25, 24, 22},
    /* N5 */ {10, 19, 18, 17, 16, 15, 14, 13, 12, 11},
    /* N6 */ {0, 9, 8, 7, 6, 5, 4, 3, 2, 1},
};

// static uint64_t NTMask[6] = {
//     /* N1 */ 0x003FFFFFFFFFFFFF,
//     /* N2 */0xFFC00FFFFFFFFFFF,
//     /* N3 */,
//     /* N4 */,
//     /* N5 */,
//     /* N6 */,
// };

#define ND4 20U
#define ND3 21U
#define ND2 42U
#define ND1 43U

#define NDMASK (uint64_t)((1ULL << ND1) | (1ULL << ND2) | (1ULL << ND3) | (1ULL << ND4))

// 编码数组，表示在输入字符串的第几位，0-->N1最右边的辉光管，5-->N6最左边的辉光管
// uint8_t str_off[6] = {};

// 输入字符串"192020"，显示对应的数字
// 第二个参数表示是否显示冒号
// 输入"------"表示不亮
void NixieTube_Show(char *str, bool nd)
{

    printf("%s", str);
    char c;

    uint64_t res = 0;

    for (int i = 0; i < 6; ++i)
    {
        c = str[i] - '0';
        if ((c >= 0) && (c <= 9))
        {
            res |= (1ULL << NTable[i][(int)c]);
        }
        else
        {
        }
    }

    if (nd)
        res |= NDMASK;

    HV507_SendData(res);
}

void NixieTube_Init(void)
{
    HV507_Init();
    HV507_SendData(0);
}