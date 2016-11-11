#ifndef __FFJPEG_QUANT_H__
#define __FFJPEG_QUANT_H__

/* 全局变量声明 */
extern int STD_QUANT_TAB[64];

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
void init_fdct_ftab(int *ftab, int *qtab);
void init_idct_ftab(int *ftab, int *qtab);

#ifdef __cplusplus
}
#endif

#endif













