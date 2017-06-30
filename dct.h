#ifndef __FFJPEG_DCT_H__
#define __FFJPEG_DCT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
/* 二维 8x8 的 DCT 变换函数 */
void fdct2d8x8(int *du);
void idct2d8x8(int *du);

#ifdef __cplusplus
}
#endif

#endif











