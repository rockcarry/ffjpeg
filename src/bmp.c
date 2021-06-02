// 包含头文件
#include <stdlib.h>
#include <stdio.h>
#include "stdefine.h"
#include "bmp.h"

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

/* 函数实现 */
int bmp_load(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    BYTE         *pdata  = NULL;
    int           i;

    fp = fopen(file, "rb");
    if (!fp) return -1;

    fread(&header, sizeof(header), 1, fp);
    pb->width  = (int)header.biWidth  > 0 ? (int)header.biWidth  : 0;
    pb->height = (int)header.biHeight > 0 ? (int)header.biHeight : 0;
    pb->stride = ALIGN(pb->width * 3, 4);
    if ((long long)pb->stride * pb->height >= 0x80000000) {
        printf("bmp's width * height is out of range !\n");
        goto done;
    }
    pb->pdata  = malloc((size_t)pb->stride * pb->height);
    if (pb->pdata) {
        pdata  = (BYTE*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fread(pdata, pb->stride, 1, fp);
        }
    }

done:
    if (fp) fclose(fp);
    return pb->pdata ? 0 : -1;
}

int bmp_create(BMP *pb, int w, int h)
{
    pb->width  = abs(w);
    pb->height = abs(h);
    pb->stride = ALIGN(pb->width * 3, 4);
    pb->pdata  = malloc((size_t)pb->stride * h);
    return pb->pdata ? 0 : -1;
}

int bmp_save(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    BYTE         *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + pb->stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biPlanes   = 1;
    header.biBitCount = 24;
    header.biSizeImage= pb->stride * pb->height;

    fp = fopen(file, "wb");
    if (fp) {
        fwrite(&header, sizeof(header), 1, fp);
        pdata = (BYTE*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fwrite(pdata, pb->stride, 1, fp);
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
    pb->width  = 0;
    pb->height = 0;
    pb->stride = 0;
}


#ifdef _TEST_BMP_
int main(void)
{
    BMP bmp = {0};
    bmp_load(&bmp, "test.bmp");
    bmp_save(&bmp, "save.bmp");
    bmp_free(&bmp);
    return 0;
}
#endif










