#ifndef __FFJPEG_BITSTR_H__
#define __FFJPEG_BITSTR_H__

// 包含头文件
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MEM_BITSTR = 0,
    FILE_BITSTR,
};

/* 函数声明 */
void* bitstr_open (int type, char *file, char *mode);
int   bitstr_close(void *stream);
int   bitstr_getc (void *stream);
int   bitstr_putc (int c, void *stream);
int   bitstr_seek (void *stream, long offset, int origin);
long  bitstr_tell (void *stream);
int   bitstr_getb (void *stream);
int   bitstr_putb (int b, void *stream);
int   bitstr_flush(void *stream);

#ifdef __cplusplus
}
#endif

#endif













