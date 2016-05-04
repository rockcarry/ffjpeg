// 包含头文件
#include <stdlib.h>
#include "stdefine.h"
#include "bitstr.h"

/* 内部类型定义 */
typedef struct {
    int   bitbuf;
    int   bitnum;
    FILE *fp;
} BITSTR_CONTEXT;

/* 函数实现 */
void* bitstr_open(char *file, char *mode)
{
    BITSTR_CONTEXT *context = malloc(sizeof(BITSTR_CONTEXT));
    if (!context) return NULL;

    context->bitbuf = 0;
    context->bitnum = 0;
    context->fp     = fopen(file, mode);
    if (!context->fp) {
        free(context);
        return NULL;
    }
    else return context;
}

int bitstr_close(void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    fclose(context->fp);
    free  (context);
    return 0;
}

int bitstr_getc(void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    return fgetc(context->fp);
}

int bitstr_putc(int c, void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    return fputc(c, context->fp);
}

size_t bitstr_read(void *buffer, size_t size, size_t count, void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    return fread(buffer, size, count, context->fp);
}

size_t bitstr_write(void *buffer, size_t size, size_t count, void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    return fwrite(buffer, size, count, context->fp);
}

int bitstr_seek(void *stream, long offset, int origin)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    context->bitbuf = 0;
    context->bitnum = 0;
    return fseek(context->fp, offset, origin);
}

long bitstr_tell(void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;
    return ftell(context->fp);
}

int bitstr_getb(void *stream)
{
    int bit;
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum == 0) {
        context->bitbuf = fgetc(context->fp);
        context->bitnum = 8;
        if (context->bitbuf == EOF) {
            return EOF;
        }
    }
    
    bit = context->bitbuf & (1 << 0);
    context->bitbuf >>= 1;
    context->bitnum--;
    return bit;
}

int bitstr_putb(int b, void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum == 8) {
        if (EOF == fputc(context->bitbuf & 0xff, context->fp)) {
            return EOF;
        }
        context->bitbuf = 0;
        context->bitnum = 0;
    }

    b &= (1 << 0);
    context->bitbuf |= (b << context->bitnum);
    context->bitnum++;
    return b;
}

int bitstr_flush(void *stream)
{
    BITSTR_CONTEXT *context = (BITSTR_CONTEXT*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum != 0) {
        if (EOF == fputc(context->bitbuf & 0xff, context->fp)) {
            return EOF;
        }
        context->bitbuf = 0;
        context->bitnum = 0;
    }
    return fflush(context->fp);
}

