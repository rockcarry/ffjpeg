/* 包含头文件 */
#include "color.h"

/*
    y = ( 0.2990f * r + 0.5870f * g + 0.1140f * b - 128);
    u = (-0.1678f * r - 0.3313f * g + 0.5000f * b + 0  );
    v = ( 0.5000f * r - 0.4187f * g - 0.0813f * b + 0  );

    r = (y                + 1.40200f * v);
    g = (y - 0.34414f * u - 0.71414f * v);
    b = (y + 1.77200f * u               );
*/
// yuv - 32bit signed fixed q11
// rgb - 8bits unsigned integer
void yuv_to_rgb(int y, int u, int v, BYTE *r, BYTE *g, BYTE *b)
{
    int tr, tg, tb;

    y += 128 << FIXQ;
    tr = (y + (FLOAT2FIX(1.40200f) * v >> FIXQ)) >> FIXQ;
    tg = (y - (FLOAT2FIX(0.34414f) * u >> FIXQ) - (FLOAT2FIX(0.71414f) * v >> FIXQ)) >> FIXQ;
    tb = (y + (FLOAT2FIX(1.77200f) * u >> FIXQ)) >> FIXQ;

    /* 饱和处理 */
    *r = tr < 0 ? 0 : tr < 255 ? tr : 255;
    *g = tg < 0 ? 0 : tg < 255 ? tg : 255;
    *b = tb < 0 ? 0 : tb < 255 ? tb : 255;
}

// rgb - 8bits unsigned integer
// yuv - 32bit signed fixed q2
void rgb_to_yuv(BYTE r, BYTE g, BYTE b, int *y, int *u, int *v)
{
    *y = FLOAT2FIX( 0.2990f) * r + FLOAT2FIX(0.5870f) * g + FLOAT2FIX(0.1140f) * b - (128 << FIXQ);
    *u = FLOAT2FIX(-0.1678f) * r - FLOAT2FIX(0.3313f) * g + FLOAT2FIX(0.5000f) * b;
    *v = FLOAT2FIX( 0.5000f) * r - FLOAT2FIX(0.4187f) * g - FLOAT2FIX(0.0813f) * b;
    *y >>= FIXQ - 2;
    *u >>= FIXQ - 2;
    *v >>= FIXQ - 2;
}





