#ifndef __FFJPEG_BMP_H__
#define __FFJPEG_BMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 类型定义 */
/* BMP 对象的类型定义 */
typedef struct
{
    int   width;   /* 宽度 */
    int   height;  /* 高度 */
    void *pdata;   /* 指向数据 */
} BMP;

/* 函数声明 */
int  bmp_load  (BMP *pb, char *file);
int  bmp_create(BMP *pb, int w, int h);

int  bmp_save(BMP *pb, char *file);
void bmp_free(BMP *pb);

#ifdef __cplusplus
}
#endif

#endif

