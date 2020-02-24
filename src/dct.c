/* 包含头文件 */
#include "stdefine.h"
#include "dct.h"

#if 1 /* 快速的整数运算版本 */
/* 内部常量定义 */
#define DCTSIZE  8

/* 内部全局变量定义 */
static const float AAN_DCT_FACTOR[DCTSIZE] = {
    1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
    1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
};

static int g_inited         =  0;
static int g_fdctfactor[64] = {0};
static int g_idctfactor[64] = {0};

void init_dct_module(void)
{
    int   i, j;
    float factor[64];

    // check inited
    if (g_inited) return;
    else g_inited = 1;

    // fdct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        g_fdctfactor[i] = FLOAT2FIX(factor[i]);
    }

    // idct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        g_idctfactor[i] = FLOAT2FIX(factor[i]);
    }
}

void init_fdct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] / qtab[i]);
    }
}

void init_idct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] * qtab[i]);
    }
}

/* 函数实现 */
void fdct2d8x8(int *data, int *ftab)
{
    int tmp0,  tmp1,  tmp2,  tmp3;
    int tmp4,  tmp5,  tmp6,  tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5, z11, z13;
    int *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * FLOAT2FIX(0.707106781f) >> FIXQ; /* c4 */
        dataptr[2] = tmp13 + z1;  /* phase 5 */
        dataptr[6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * FLOAT2FIX(0.382683433f) >> FIXQ;  /* c6 */
        z2 = (FLOAT2FIX(0.541196100f) * tmp10 >> FIXQ) + z5;     /* c2-c6 */
        z4 = (FLOAT2FIX(1.306562965f) * tmp12 >> FIXQ) + z5;     /* c2+c6 */
        z3 = tmp11 * FLOAT2FIX(0.707106781f) >> FIXQ;            /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += DCTSIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[DCTSIZE * 4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * FLOAT2FIX(0.707106781f) >> FIXQ; /* c4 */
        dataptr[DCTSIZE * 2] = tmp13 + z1;  /* phase 5 */
        dataptr[DCTSIZE * 6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * FLOAT2FIX(0.382683433f) >> FIXQ;  /* c6 */
        z2 = (FLOAT2FIX(0.541196100f) * tmp10 >> FIXQ) + z5;     /* c2-c6 */
        z4 = (FLOAT2FIX(1.306562965f) * tmp12 >> FIXQ) + z5;     /* c2+c6 */
        z3 = tmp11 * FLOAT2FIX(0.707106781f) >> FIXQ;            /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCTSIZE * 5] = z13 + z2; /* phase 6 */
        dataptr[DCTSIZE * 3] = z13 - z2;
        dataptr[DCTSIZE * 1] = z11 + z4;
        dataptr[DCTSIZE * 7] = z11 - z4;

        dataptr++;  /* advance pointer to next column */
    }

    ftab = ftab ? ftab : g_fdctfactor;
    for (ctr=0; ctr<64; ctr++) {
        data[ctr]  *= ftab[ctr];
        data[ctr] >>= FIXQ + 2;
    }
}

void idct2d8x8(int *data, int *ftab)
{
    int  tmp0,  tmp1,  tmp2,  tmp3;
    int  tmp4,  tmp5,  tmp6,  tmp7;
    int  tmp10, tmp11, tmp12, tmp13;
    int  z5, z10, z11, z12, z13;
    int *dataptr;
    int  ctr;

    ftab = ftab ? ftab : g_idctfactor;
    for (ctr=0; ctr<64; ctr++) {
        data[ctr] *= ftab[ctr];
    }

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        if ( dataptr[1] + dataptr[2] + dataptr[3] + dataptr[4]
           + dataptr[5] + dataptr[6] + dataptr[7] == 0 )
        {
            dataptr[1] = dataptr[0];
            dataptr[2] = dataptr[0];
            dataptr[3] = dataptr[0];
            dataptr[4] = dataptr[0];
            dataptr[5] = dataptr[0];
            dataptr[6] = dataptr[0];
            dataptr[7] = dataptr[0];

            dataptr += DCTSIZE;
            continue;
        }

        /* Even part */
        tmp0 = dataptr[0];
        tmp1 = dataptr[2];
        tmp2 = dataptr[4];
        tmp3 = dataptr[6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= FLOAT2FIX(1.414213562f); tmp12 >>= FIXQ;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[1];
        tmp5 = dataptr[3];
        tmp6 = dataptr[5];
        tmp7 = dataptr[7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= FLOAT2FIX(1.414213562f); tmp11 >>= FIXQ;

        z5 = (z10 + z12) * FLOAT2FIX(1.847759065f) >> FIXQ;  /*  2 * c2 */
        tmp10 = (FLOAT2FIX( 1.082392200f) * z12 >> FIXQ) - z5; /*  2 * (c2 - c6) */
        tmp12 = (FLOAT2FIX(-2.613125930f) * z10 >> FIXQ) + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[0] = tmp0 + tmp7;
        dataptr[7] = tmp0 - tmp7;
        dataptr[1] = tmp1 + tmp6;
        dataptr[6] = tmp1 - tmp6;
        dataptr[2] = tmp2 + tmp5;
        dataptr[5] = tmp2 - tmp5;
        dataptr[4] = tmp3 + tmp4;
        dataptr[3] = tmp3 - tmp4;

        dataptr += DCTSIZE;
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        /* Even part */
        tmp0 = dataptr[DCTSIZE * 0];
        tmp1 = dataptr[DCTSIZE * 2];
        tmp2 = dataptr[DCTSIZE * 4];
        tmp3 = dataptr[DCTSIZE * 6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= FLOAT2FIX(1.414213562f); tmp12 >>= FIXQ;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[DCTSIZE * 1];
        tmp5 = dataptr[DCTSIZE * 3];
        tmp6 = dataptr[DCTSIZE * 5];
        tmp7 = dataptr[DCTSIZE * 7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= FLOAT2FIX(1.414213562f); tmp11 >>= FIXQ;

        z5 = (z10 + z12) * FLOAT2FIX(1.847759065f) >> FIXQ;  /*  2 * c2 */
        tmp10 = (FLOAT2FIX( 1.082392200f) * z12 >> FIXQ) - z5; /*  2 * (c2 - c6) */
        tmp12 = (FLOAT2FIX(-2.613125930f) * z10 >> FIXQ) + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[DCTSIZE * 0] = tmp0 + tmp7;
        dataptr[DCTSIZE * 7] = tmp0 - tmp7;
        dataptr[DCTSIZE * 1] = tmp1 + tmp6;
        dataptr[DCTSIZE * 6] = tmp1 - tmp6;
        dataptr[DCTSIZE * 2] = tmp2 + tmp5;
        dataptr[DCTSIZE * 5] = tmp2 - tmp5;
        dataptr[DCTSIZE * 4] = tmp3 + tmp4;
        dataptr[DCTSIZE * 3] = tmp3 - tmp4;

        dataptr++; /* advance pointers to next column */
    }
}
#endif

#if 0 /* 快速的浮点数运算版本 */

/* 内部常量定义 */
#define DCTSIZE  8

/* 内部全局变量定义 */
static float aandctfactor[DCTSIZE] = {
    1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
    1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
};

static float dctfactor[64] = {0};

/* 内部函数实现 */
static initdctfactor()
{
    int i, j;
    for (i=0; i<DCTSIZE; i++)
        for (j=0; j<DCTSIZE; j++)
            dctfactor[i * 8 + j] = aandctfactor[i] * aandctfactor[j];
}

/* 函数实现 */
void fdct2d8x8(float *data)
{
    float tmp0, tmp1, tmp2, tmp3;
    float tmp4, tmp5, tmp6, tmp7;
    float tmp10, tmp11, tmp12, tmp13;
    float z1, z2, z3, z4, z5, z11, z13;
    float *dataptr;
    int   ctr;

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * 0.707106781f; /* c4 */
        dataptr[2] = tmp13 + z1;  /* phase 5 */
        dataptr[6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * 0.382683433f; /* c6 */
        z2 = 0.541196100f * tmp10 + z5;      /* c2-c6 */
        z4 = 1.306562965f * tmp12 + z5;      /* c2+c6 */
        z3 = tmp11 * 0.707106781f;           /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += DCTSIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[DCTSIZE * 4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * 0.707106781f; /* c4 */
        dataptr[DCTSIZE * 2] = tmp13 + z1;  /* phase 5 */
        dataptr[DCTSIZE * 6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * 0.382683433f; /* c6 */
        z2 = 0.541196100f * tmp10 + z5;      /* c2-c6 */
        z4 = 1.306562965f * tmp12 + z5;      /* c2+c6 */
        z3 = tmp11 * 0.707106781f;           /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCTSIZE * 5] = z13 + z2; /* phase 6 */
        dataptr[DCTSIZE * 3] = z13 - z2;
        dataptr[DCTSIZE * 1] = z11 + z4;
        dataptr[DCTSIZE * 7] = z11 - z4;

        dataptr++;  /* advance pointer to next column */
    }
}

void idct2d8x8(float *data)
{
    float  tmp0,  tmp1,  tmp2,  tmp3;
    float  tmp4,  tmp5,  tmp6,  tmp7;
    float  tmp10, tmp11, tmp12, tmp13;
    float  z5, z10, z11, z12, z13;
    float *dataptr;
    int    ctr;

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        /* Even part */
        tmp0 = dataptr[0];
        tmp1 = dataptr[2];
        tmp2 = dataptr[4];
        tmp3 = dataptr[6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= 1.414213562f;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[1];
        tmp5 = dataptr[3];
        tmp6 = dataptr[5];
        tmp7 = dataptr[7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;  /*  2 * c2 */
        tmp10 =  1.082392200f * z12 - z5; /*  2 * (c2 - c6) */
        tmp12 = -2.613125930f * z10 + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[0] = tmp0 + tmp7;
        dataptr[7] = tmp0 - tmp7;
        dataptr[1] = tmp1 + tmp6;
        dataptr[6] = tmp1 - tmp6;
        dataptr[2] = tmp2 + tmp5;
        dataptr[5] = tmp2 - tmp5;
        dataptr[4] = tmp3 + tmp4;
        dataptr[3] = tmp3 - tmp4;

        dataptr += DCTSIZE;
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        /* Even part */
        tmp0 = dataptr[DCTSIZE * 0];
        tmp1 = dataptr[DCTSIZE * 2];
        tmp2 = dataptr[DCTSIZE * 4];
        tmp3 = dataptr[DCTSIZE * 6];

        tmp10 = tmp0 + tmp2;    /* phase 3 */
        tmp11 = tmp0 - tmp2;

        tmp13  = tmp1 + tmp3;   /* phases 5-3 */
        tmp12  = tmp1 - tmp3;   /* 2 * c4 */
        tmp12 *= 1.414213562f;
        tmp12 -= tmp13;

        tmp0 = tmp10 + tmp13;   /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = dataptr[DCTSIZE * 1];
        tmp5 = dataptr[DCTSIZE * 3];
        tmp6 = dataptr[DCTSIZE * 5];
        tmp7 = dataptr[DCTSIZE * 7];

        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7   = z11 + z13;   /* phase 5 */
        tmp11  = z11 - z13;   /* 2 * c4 */
        tmp11 *= 1.414213562f;

        z5 = (z10 + z12) * 1.847759065f;  /*  2 * c2 */
        tmp10 =  1.082392200f * z12 - z5; /*  2 * (c2 - c6) */
        tmp12 = -2.613125930f * z10 + z5; /* -2 * (c2 + c6) */

        tmp6 = tmp12 - tmp7;    /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dataptr[DCTSIZE * 0] = tmp0 + tmp7;
        dataptr[DCTSIZE * 7] = tmp0 - tmp7;
        dataptr[DCTSIZE * 1] = tmp1 + tmp6;
        dataptr[DCTSIZE * 6] = tmp1 - tmp6;
        dataptr[DCTSIZE * 2] = tmp2 + tmp5;
        dataptr[DCTSIZE * 5] = tmp2 - tmp5;
        dataptr[DCTSIZE * 4] = tmp3 + tmp4;
        dataptr[DCTSIZE * 3] = tmp3 - tmp4;

        dataptr++; /* advance pointers to next column */
    }
}

/* how to use */
static void dcttest(void)
{
    float data[64];
    int   i;

    /* FDCT */
    fdct2d8x8(data);
    for (i=0; i<64; i++) data[i] /= dctfactor[i];
    for (i=0; i<64; i++) data[i] /= 8.0f;

    /* IDCT */
    for (i=0; i<64; i++) data[i] *= dctfactor[i];
    idct2d8x8(data);
    for (i=0; i<64; i++) data[i] /= 8.0f;
}

#endif

#if 0 /* 矩阵变换版本 */
/* 内部常量定义 */
#define M_PI  3.14159265358979323846f

/* 内部全局变量定义 */
static float fdctmatrix[8][8] = {0};  /* fdct 变换矩阵 */
static float idctmatrix[8][8] = {0};  /* idct 变换矩阵 */

/* 内部函数定义 */
static float c(int u)
{
    if (u == 0) return (float)sqrt(1.0f / 8.0f);
    else        return (float)sqrt(2.0f / 8.0f);
}

/* 初始化 dct 变换矩阵 */
void initdctmatrix(void)
{
    static int inited = 0;
    int    u, x;

    /* 避免重复初始化 */
    if (inited) return;

    /* init fdct matrix */
    for (u=0; u<8; u++)
    {
        for (x=0; x<8; x++)
        {
            fdctmatrix[u][x] = (float)(c(u) * cos((2.0f * x + 1.0f) * u * M_PI / 16.0f));
        }
    }

    /* init idct matrix */
    for (u=0; u<8; u++)
    {
        for (x=0; x<8; x++)
        {
            idctmatrix[x][u] = (float)(c(u) * cos((2.0f * x + 1.0f) * u * M_PI / 16.0f));
        }
    }

    inited = 1;
}

/* 1d 8-points forward dct */
void fdct1d8(float *u, float *x)
{
    int i;
    int j;

    for (i=0; i<8; i++)
    {
        u[i] = 0.0f;
        for (j=0; j<8; j++)
        {
            u[i] += fdctmatrix[i][j] * x[j];
        }
    }
}

/* 1d 8-points invert dct */
void idct1d8(float *x, float *u)
{
    int i;
    int j;

    for (i=0; i<8; i++)
    {
        x[i] = 0.0f;
        for (j=0; j<8; j++)
        {
            x[i] += idctmatrix[i][j] * u[j];
        }
    }
}

/* 2d 8x8 forward dct */
void fdct2d8x8(float *data)
{
    float temp[64];
    int   i, j;

    /* 初始化变换矩阵 */
    initdctmatrix();

    /* 逐行进行 1d fdct */
    for (i=0; i<8; i++)
    {
        fdct1d8(temp + 8 * i, data + 8 * i);
    }

    /* 转置矩阵 */
    for (i=0; i<8; i++)
        for (j=0; j<8; j++)
            *(data + 8 * i + j) = *(temp + 8 * j + i);

    /* 逐行进行 1d fdct */
    for (i=0; i<8; i++)
    {
        fdct1d8(temp + 8 * i, data + 8 * i);
    }

    /* 转置矩阵 */
    for (i=0; i<8; i++)
        for (j=0; j<8; j++)
            *(data + 8 * i + j) = *(temp + 8 * j + i);
}

/* 2d 8x8 invert dct */
void idct2d8x8(float *data)
{
    float temp[64];
    int   i, j;

    /* 初始化变换矩阵 */
    initdctmatrix();

    /* 逐行进行 1d idct */
    for (i=0; i<8; i++)
    {
        idct1d8(temp + 8 * i, data + 8 * i);
    }

    /* 转置矩阵 */
    for (i=0; i<8; i++)
        for (j=0; j<8; j++)
            *(data + 8 * i + j) = *(temp + 8 * j + i);

    /* 逐行进行 1d idct */
    for (i=0; i<8; i++)
    {
        idct1d8(temp + 8 * i, data + 8 * i);
    }

    /* 转置矩阵 */
    for (i=0; i<8; i++)
        for (j=0; j<8; j++)
            *(data + 8 * i + j) = *(temp + 8 * j + i);
}
#endif

#if 0 /* 数学表达式版本 */
/* 内部常量定义 */
#define M_PI  3.14159265358979323846f

/* 函数实现 */
static float alpha(int n)
{
    if (n == 0) return 1.0f / (float)sqrt(8);
    else        return 1.0f / 2.0f;
}

void fdct2d8x8(float *data)
{
    int u, v;
    int x, y, i;
    float buf[64];
    float temp;

    for (u=0; u<8; u++)
    {
        for (v=0; v<8; v++)
        {
            temp = 0;
            for (x=0; x<8; x++)
            {
                for (y=0; y<8; y++)
                {
                    temp += data[y * 8 + x]
                          * (float)cos((2.0f * x + 1.0f) / 16.0f * u * M_PI)
                          * (float)cos((2.0f * y + 1.0f) / 16.0f * v * M_PI);
                }
            }
            buf[v * 8 + u] = alpha(u) * alpha(v) * temp;
        }
    }

    for (i=0; i<64; i++) data[i] = buf[i];
}

void idct2d8x8(float *data)
{
    int u, v;
    int x, y, i;
    float buf[64];
    float temp;

    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            temp = 0;
            for (u=0; u<8; u++)
            {
                for (v=0; v<8; v++)
                {
                    temp += alpha(u) * alpha(v) * (data[v * 8 + u]
                          * (float)cos((2.0f * x + 1.0f) / 16.0f * u * M_PI)
                          * (float)cos((2.0f * y + 1.0f) / 16.0f * v * M_PI));
                }
            }
            buf[y * 8 + x] = (char)(temp + 0.5);
        }
    }

    for (i=0; i<64; i++) data[i] = buf[i];
}
#endif
