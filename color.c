/* 包含头文件 */
#include "color.h"

/*
    y = ( 0.299f * r + 0.587f * g + 0.114f * b - 128);
    u = (-0.169f * r - 0.331f * g + 0.500f * b + 0  );
    v = ( 0.500f * r - 0.419f * g - 0.081f * b + 0  );

    r = (y              + 1.402f * v);
    g = (y - 0.344f * u - 0.714f * v);
    b = (y + 1.772f * u             );
*/

void yuv_to_rgb(int y, int u, int v, BYTE *r, BYTE *g, BYTE *b)
{
    int tr = (y + 128) + (( 91881 * v) >> 16);
    int tg = (y + 128) - (( 22544 * u + 46793 * v) >> 16);
    int tb = (y + 128) + ((116130 * u) >> 16);

    /* 饱和处理 */
    *r = tr < 0 ? 0 : tr < 255 ? tr : 255;
    *g = tg < 0 ? 0 : tg < 255 ? tg : 255;
    *b = tb < 0 ? 0 : tb < 255 ? tb : 255;
}

void rgb_to_yuv(BYTE r, BYTE g, BYTE b, int *y, int *u, int *v)
{
    *y = (( 19595 * r + 38470 * g + 7471 * b) >> 16) - 128;
    *u = ((-11076 * r - 21692 * g) >> 16) + (b >> 1);
    *v = ((-27460 * g - 5308  * b) >> 16) + (r >> 1);
}





