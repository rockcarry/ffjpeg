/* 包含头文件 */
#include "stdefine.h"
#include "rle.h"

/* 内部函数实现 */
static void category_encode(int *size, int *code)
{
    WORD amp  = (WORD)abs(*code);
    WORD mask = (1 << 15);
    int  i    = 15;
    if (amp == 0) { *size = 0; return; }
    while (i && !(amp & mask)) { mask >>= 1; i--; }
    *size = i + 1;
    if (*code < 0) *code = (1 << *size) - amp - 1;
}

static void category_decode(int *size, int *code)
{
    if (*code < (1 << (*size - 1))) {
        *code += 1 - (1 << *size);
    }
}

/* 函数实现 */
void rle_encode(RLEITEM *dest, int *src, int *curdc)
{
    int n, i, j;

    /* the first item is used for dc */
    dest[0].znum = 0;
    dest[0].code = src[0] - *curdc;
    category_encode(&dest[0].size, &dest[0].code);

    /* update the curdc value */
    *curdc = src[0];

    /* run length zero encoding */
    for (i=1, j=1, n=0; i<64 && j<64; i++)
    {
        if (src[i] == 0 && n < 15) n++;
        else
        {
            dest[j].znum = n;
            dest[j].code = src[i];
            category_encode(&dest[j].size, &dest[j].code);
            n = 0; // reset zero counter
            j++;   // next rle item
        }
    }

    for (j=63; j>=0; j--)
    {
        /* find the eob */
        if (dest[j].size != 0) break;
    }

    /* clear to eob */
    for (j++; j<64; j++)
    {
        dest[j].znum = 0;
        dest[j].size = 0;
        dest[j].code = 0;
    }
}

void rle_decode(int *dest, RLEITEM *src, int *curdc)
{
    int size;
    int code;
    int i;
    int j;

    /* clear dest buffer */
    memset(dest, 0, 64 * sizeof(int));

    /* category decode for dc */
    size = src->size;
    code = src->code;

    category_decode(&size, &code);
    dest[0] = code + *curdc;

    /* update the curdc value */
    *curdc = dest[0];

    /* go to next rle item */
    src++;
    j = 1;

    /* do rle decoding in this while loop */
    while (j < 64)
    {
        /* if we find the eob then break */
        if (src->znum == 0 && src->size == 0) break;

        /* expand the rle zeros */
        for (i=0; i<src->znum && j<63; i++) dest[j++] = 0;

        /* decoding the rle non-zero code */
        size = src->size;
        code = src->code;

        category_decode(&size, &code);
        dest[j++] = code;

        /* go to next rle item */
        src++;
    }
}



