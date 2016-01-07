/* 包含头文件 */
#include "color.h"

// 内部全局变量定义
static int u_0_344[256];
static int u_1_772[256];
static int v_1_402[256];
static int v_0_714[256];

// 函数实现
void init_color_convert(void)
{
    static int inited = 0;
    int i;

    if (inited) return;
    for (i=0; i<256; i++)
    {
        u_0_344[i] = (int)(0.344 * (i - 128));
        u_1_772[i] = (int)(1.772 * (i - 128));
        v_1_402[i] = (int)(1.402 * (i - 128));
        v_0_714[i] = (int)(0.714 * (i - 128));
    }
    inited = 1;
}

void yuv_to_rgb(int y, int u, int v, BYTE *r, BYTE *g, BYTE *b)
{
    int tr;
    int tg;
    int tb;

    /* YUV 值饱和处理 */
    u = u > 0   ? u : 0;
    v = v > 0   ? v : 0;
    u = u < 255 ? u : 255;
    v = v < 255 ? v : 255;

    tr = y              + v_1_402[v];
    tg = y - u_0_344[u] - v_0_714[v];
    tb = y + u_1_772[u];

    /* RGB 值饱和处理 */
    tr = tr > 0   ? tr : 0;
    tg = tg > 0   ? tg : 0;
    tb = tb > 0   ? tb : 0;
    tr = tr < 255 ? tr : 255;
    tg = tg < 255 ? tg : 255;
    tb = tb < 255 ? tb : 255;

    *r = tr;
    *g = tg;
    *b = tb;
}




