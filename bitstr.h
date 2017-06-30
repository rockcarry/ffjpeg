#ifndef __FFJPEG_BITSTR_H__
#define __FFJPEG_BITSTR_H__

// 包含头文件
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    BITSTR_MEM = 0,
    BITSTR_FILE,
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
int   bitstr_get_bits(void *stream, int n);
int   bitstr_put_bits(void *stream, int bits, int n);
int   bitstr_flush(void *stream, int flag);

#ifdef __cplusplus
}
#endif

#endif













