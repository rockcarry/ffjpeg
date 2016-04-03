#ifndef __FFJPEG_COLOR_H__
#define __FFJPEG_COLOR_H__

// 包含头文件
#include "stdefine.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_color_convert(void);
void yuv_to_rgb(BYTE y, BYTE u, BYTE v, BYTE *r, BYTE *g, BYTE *b);
void rgb_to_yuv(BYTE r, BYTE g, BYTE b, BYTE *y, BYTE *u, BYTE *v);

#ifdef __cplusplus
}
#endif

#endif











