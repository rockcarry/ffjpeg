#ifndef __FFJPEG_DCT_H__
#define __FFJPEG_DCT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
/* 二维 8x8 的 DCT 变换函数 */
void init_dct_module(void);
void init_fdct_ftab(int *ftab, int *qtab);
void init_idct_ftab(int *ftab, int *qtab);
void fdct2d8x8(int *du, int *ftab);
void idct2d8x8(int *du, int *ftab);

#ifdef __cplusplus
}
#endif

#endif











