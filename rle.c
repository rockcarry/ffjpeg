/* 包含头文件 */
#include "stdefine.h"
#include "rle.h"

/* 内部函数实现 */
static void AmpEncode(int *amp, int *size)
{
    WORD absamp = (WORD)abs(*amp);
    WORD mask   = (1 << 15);
    int  i      = 15;
    if (absamp == 0) { *size = 0; return; }
    while (i && !(absamp & mask)) { mask >>= 1; i--; }
    *size = i + 1;
    if (*amp < 0) *amp = (1 << *size) - absamp - 1;
}

static void AmpDecode(int *amp, int size)
{
    if (*amp < (1 << (size - 1))) {
        *amp += 1 - (1 << size);
    }
}

/* 函数实现 */
void RLEEncode(RLEITEM *dest, int *src, int *curdc)
{
    int amp;
    int size;
    int n;
    int i;
    int j;

    /* clear dest buffer */
    memset(dest, 0, 64*sizeof(RLEITEM));

    /* The first item is used for DC */
    amp = src[0] - *curdc;
    AmpEncode(&amp, &size);
    dest[0].runlen  = 0;
    dest[0].size    = size;
    dest[0].ampcode = amp;

    /* update the curdc variable */
    *curdc = src[0];

    /* run length zero encoding */
    for (i=1, j=1, n=0; i<64 && j<64; i++)
    {
        if (src[i] == 0 && n < 15) n++;
        else
        {
            amp = src[i];
            AmpEncode(&amp, &size);
            dest[j].runlen  = n;
            dest[j].size    = size;
            dest[j].ampcode = amp;
            n = 0;
            j++;
        }
    }

    for (j=63; j>=0; j--)
    {
        /* Find the EOB */
        if (dest[j].size != 0) break;
    }

    /* clear to EOB */
    for (j++; j<64; j++)
    {
        dest[j].runlen  = 0;
        dest[j].size    = 0;
        dest[j].ampcode = 0;
    }
}

void RLEDecode(int *dest, RLEITEM *src, int *curdc)
{
    int amp;
    int size;
    int i;
    int j;

    /* clear dest buffer */
    memset(dest, 0, 64 * sizeof(int));

    /* amp decode for DC */
    amp  = src->ampcode;
    size = src->size;
    AmpDecode(&amp, size);
    dest[0] = amp + *curdc;

    /* update the curdc variable */
    *curdc = dest[0];

    /* go to next rle item */
    src++;
    j = 1;

    /* do rle decoding in this while loop */
    while (j < 64)
    {
        /* if we find the EOB then break */
        if (src->runlen == 0 && src->size == 0) break;

        /* expand the rle zeros */
        for (i=0; i<src->runlen && j<63; i++) dest[j++] = 0;

        /* decoding the rle non-zero code */
        amp  = src->ampcode;
        size = src->size;
        AmpDecode(&amp, size);
        dest[j++] = amp;

        /* go to next rle item */
        src++;
    }
}



