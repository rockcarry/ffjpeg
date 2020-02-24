#ifndef __FFJPEG_ZIGZAG_H__
#define __FFJPEG_ZIGZAG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 全局变量声明 */
extern const int ZIGZAG[64];

/* 函数声明 */
void zigzag_encode(int *data);
void zigzag_decode(int *data);

#ifdef __cplusplus
}
#endif

#endif








