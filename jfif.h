#ifndef __FFJPEG_JFIF_H__
#define __FFJPEG_JFIF_H__

// 包含头文件
#include "bmp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
void* jfif_load(char *file);
int   jfif_save(void *ctxt, char *file);
void  jfif_free(void *ctxt);

int   jfif_decode(void *ctxt, BMP *pb);
void* jfif_encode(BMP *pb);

#ifdef __cplusplus
}
#endif

#endif

