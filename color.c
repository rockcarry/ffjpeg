/* 包含头文件 */
#include "color.h"

/*
    y = ( 0.299f * r + 0.587f * g + 0.114f * b + 0  );
    u = (-0.169f * r - 0.331f * g + 0.500f * b + 128);
    v = ( 0.500f * r - 0.419f * g - 0.081f * b + 128);

    iu = u - 128; iv = v - 128;
    r = (y               + 1.402f * iv);
    g = (y - 0.344f * iu - 0.714f * iv);
    b = (y + 1.772f * iu              );
*/

// 内部全局变量定义
static short u_0_344[256];
static short u_1_772[256];
static short v_1_402[256];
static short v_0_714[256];

// 函数实现
void init_color_convert(void)
{
    static int inited = 0;
    if (!inited) {
        int i;
        for (i=0; i<256; i++)
        {
            u_0_344[i] = (short)(0.344 * (i - 128));
            u_1_772[i] = (short)(1.772 * (i - 128));
            v_1_402[i] = (short)(1.402 * (i - 128));
            v_0_714[i] = (short)(0.714 * (i - 128));
        }
        inited = 1;
    }
}

void yuv_to_rgb(int y, int u, int v, BYTE *r, BYTE *g, BYTE *b)
{
    int tr = y              + v_1_402[v];
    int tg = y - u_0_344[u] - v_0_714[v];
    int tb = y + u_1_772[u];

    /* 饱和处理 */
    *r = tr < 0 ? 0 : tr < 255 ? tr : 255;
    *g = tg < 0 ? 0 : tg < 255 ? tg : 255;
    *b = tb < 0 ? 0 : tb < 255 ? tb : 255;
}

void rgb_to_yuv(BYTE r, BYTE g, BYTE b, int *y, int *u, int *v)
{
    int ty = (19595 * r + 38470 * g + 7471 * b) >> 16;
    int tu = ((-11076 * r - 21692 * g) >> 16) + (b >> 1) + 128;
    int tv = ((-27460 * g - 5308  * b) >> 16) + (r >> 1) + 128;

    /* 饱和处理 */
    *y = ty;
    *u = tu;
    *v = tv;
}





