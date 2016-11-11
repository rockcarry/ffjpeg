#ifndef __FFJPEG_DCT_H__
#define __FFJPEG_DCT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 全局变量声明 */
extern int FDCT_FACTOR_TAB[64];
extern int IDCT_FACTOR_TAB[64];

/* 函数声明 */
/* 二维 8x8 的 DCT 变换函数 */
void fdct2d8x8(int *du, int *ftab);
void idct2d8x8(int *du, int *ftab);

#ifdef __cplusplus
}
#endif

#endif











