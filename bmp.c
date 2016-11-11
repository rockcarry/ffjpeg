// 包含头文件
#include <stdlib.h>
#include <stdio.h>
#include "stdefine.h"
#include "bmp.h"

#define TEST_BMP  0

// 内部类型定义
#pragma pack(1)
typedef struct { 
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
    DWORD  biSize;
    DWORD  biWidth;
    DWORD  biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    DWORD  biXPelsPerMeter;
    DWORD  biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BMPFILEHEADER;
#pragma pack()

/* 内部函数实现 */
static int ALIGN(int x, int y) {
    // y must be a power of 2.
    return (x + y - 1) & ~(y - 1);
}

/* 函数实现 */
int bmp_load(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    int           stride =  0;
    BYTE         *pdata  = NULL;
    int           i;

    fp = fopen(file, "rb");
    if (!fp) return -1;

    fread(&header, sizeof(header), 1, fp);
    pb->width  = header.biWidth;
    pb->height = header.biHeight;
    pb->pdata  = malloc(ALIGN(pb->width * 3, 4) * pb->height);
    if (pb->pdata) {
        stride = ALIGN(pb->width * 3, 4);
        pdata  = (BYTE*)pb->pdata + stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= stride;
            fread(pdata, stride, 1, fp);
        }
    }

    fclose(fp);
    return pb->pdata ? 0 : -1;
}

int bmp_create(BMP *pb, int w, int h)
{
    pb->width  = w;
    pb->height = h;
    pb->pdata  = malloc(ALIGN(w * 3, 4) * h);
    return pb->pdata ? 0 : -1;
}

int bmp_save(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    int           stride = ALIGN(pb->width * 3, 4);
    BYTE         *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biBitCount = 24;
    header.biSizeImage= stride * pb->height;

    fp = fopen(file, "wb");
    if (fp) {
        fwrite(&header, sizeof(header), 1, fp);
        pdata = (BYTE*)pb->pdata + stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= stride;
            fwrite(pdata, stride, 1, fp);
        }
        fclose(fp);
    }

    return fp ? 0 : -1;
}

void bmp_free(BMP *pb)
{
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
    pb->width = pb->height = 0;
}


#if TEST_BMP
int main(void)
{
    BMP bmp = {0};
    bmp_load(&bmp, "test.bmp");
    bmp_save(&bmp, "save.bmp");
    bmp_free(&bmp);
    return 0;
}
#endif










