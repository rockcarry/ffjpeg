/* 包含头文件 */
#include "zigzag.h"

/* 内部全局变量定义 */
const int ZIGZAG[64] = {
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

/* 函数实现 */
void zigzag_encode(int *data)
{
    int buf[64], i;
    for (i=0; i<64; i++) buf [i] = data[ZIGZAG[i]];
    for (i=0; i<64; i++) data[i] = buf[i];
}

void zigzag_decode(int *data)
{
    int buf[64], i;
    for (i=0; i<64; i++) buf [ZIGZAG[i]] = data[i];
    for (i=0; i<64; i++) data[i] = buf[i];
}








