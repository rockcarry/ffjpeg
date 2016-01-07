/* 包含头文件 */
#include "quant.h"

/* 全局变量定义 */
int STD_QUANT_TAB[64] =
{
    16,  11,  12,  14,  12,  10,  16,  14,
    13,  14,  18,  17,  16,  19,  24,  40,
    26,  24,  22,  22,  24,  49,  35,  37,
    29,  40,  58,  51,  61,  60,  57,  51,
    56,  55,  64,  72,  92,  78,  64,  68,
    87,  69,  55,  56,  80, 109,  81,  87,
    95,  98, 103, 104, 103,  62,  77, 113,
   121, 112, 100, 120,  92, 101, 103,  99 
};

void QuantizeEncode(int *data, int *qtab)
{
    int i;
    for (i=0; i<64; i++) data[i] /= qtab[i];
}

void QuantizeDecode(int *data, int *qtab)
{
    int i;
    for (i=0; i<64; i++) data[i] *= qtab[i];
}











