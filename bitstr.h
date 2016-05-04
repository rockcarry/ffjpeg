#ifndef __FFJPEG_BITSTR_H__
#define __FFJPEG_BITSTR_H__

// 包含头文件
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
void*  bitstr_open (char *file, char *mode);
int    bitstr_close(void *stream);
int    bitstr_getc (void *stream);
int    bitstr_putc (int c, void *stream);
size_t bitstr_read (void *buffer, size_t size, size_t count, void *stream);
size_t bitstr_write(void *buffer, size_t size, size_t count, void *stream);
int    bitstr_seek (void *stream, long offset, int origin);
long   bitstr_tell (void *stream);
int    bitstr_getb (void *stream);
int    bitstr_putb (int b, void *stream);
int    bitstr_flush(void *stream);

#ifdef __cplusplus
}
#endif

#endif













