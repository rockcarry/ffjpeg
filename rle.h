#ifndef __FFJPEG_RLE_H__
#define __FFJPEG_RLE_H__

/* 类型定义 */
typedef struct
{
    int size;
    int runlen;
    int ampcode;
} RLEITEM, *PRLEITEM;

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
void RLEEncode(RLEITEM *dest, int *src, int *curdc);
void RLEDecode(int *dest, RLEITEM *src, int *curdc);

#ifdef __cplusplus
}
#endif

#endif










