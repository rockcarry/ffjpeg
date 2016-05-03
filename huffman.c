/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitstr.h"
#include "huffman.h"

#define ENABLE_DEBUG_DUMP  1

/* 内部函数实现 */
/* ++ 用于快速排序的比较函数 */
static int cmp_freq_item(const void *a, const void *b)
{
    return ((HUFCODEITEM*)a)->freq - ((HUFCODEITEM*)b)->freq;
}

static int cmp_depth_item(const void *a, const void *b)
{
    return ((HUFCODEITEM*)a)->depth - ((HUFCODEITEM*)b)->depth;
}

static int cmp_symbol_item(const void *a, const void *b)
{
    return ((HUFCODEITEM*)a)->symbol - ((HUFCODEITEM*)b)->symbol;
}
/* -- 用于快速排序的比较函数 */

#if ENABLE_DEBUG_DUMP
static void int_to_bin_str(int v, char *str, int n) {
    int i;
    for (i=n-1; i>=0; i--) {
        if (v & (1 << i)) {
            *str++ = '1';
        }
        else {
            *str++ = '0';
        }
    }
    *str = '\0';
}

static void dump_huffman_codelist(char *title, HUFCODEITEM *list, int n, int head)
{
    char str[33];
    int  i;
    printf("\n+----%s----+\n", title);
    for (i=0; i<n; i++) {
        int_to_bin_str(list[i].code, str, list[i].depth);
        printf("%c %c, freq:%3d, group:%3d, depth:%d, code:%s\n",
            head == i ? '*' : ' ',
            list[i].symbol, list[i].freq, list[i].group, list[i].depth, str);
    }
}
#endif

/* 函数实现 */
/* 统计符号串中各个符号出现的频率 */
void huffman_stat_freq(HUFCODEITEM codelist[256], void *stream)
{
    int  data;
    int  i;

    /* 初始化频率表 */
    for (i=0; i<256; i++)
    {
        codelist[i].symbol = i;
        codelist[i].freq   = 0;
        codelist[i].group  = i;
        codelist[i].depth  = 1;
        codelist[i].code   = 0;
    }

    /* 统计频率 */
    while (1) {
        data = bitstr_getc(stream);
        if (data == -1) break;
        codelist[data].freq++;
    }
}

BOOL huffman_encode_begin(HUFCODEC *phc)
{
    HUFCODEITEM *codelist = phc->codelist;
    HUFCODEITEM  copylist[256];
    HUFCODEITEM *templist;
    BYTE        *huftab   = phc->huftab;
    int head;
    int group;
    int code;
    int n, i, j, k;

    /* make a copylist which is copy of codelist */
    memcpy(copylist, codelist, sizeof(HUFCODEITEM) * 256);

    /* 对 copylist 按 freq 进行快速排序 */
    qsort(copylist, 256, sizeof(HUFCODEITEM), cmp_freq_item);

    /* 查找出第一个非零频率的符号
       并计算出非零频率的符号个数 */
    for (i=0; i<256 && !copylist[i].freq; i++);
    templist = &copylist[i];
    n = 256 - i;

    /* 初始化头指针和当前分组 */
    head  = 0;
    group = 256;

    while (head < n - 2) /* 在一个 while 循环中计算码长 */
    {
#if ENABLE_DEBUG_DUMP
        // dump sorted code list
        dump_huffman_codelist(" sorted -", templist, n, head);
#endif

        /* 根据 templist 中 head 开始的两个元素的分组情况
           来更新 templist 中 head 之前相应元素的码长和分组 */
        for (i=0; i<head; i++)
        {
            if (  templist[i].group == templist[head + 0].group
               || templist[i].group == templist[head + 1].group)
            {
                templist[i].depth++;       /* 码长值加 1 */
                templist[i].group = group; /* 进行新的分组 */
            }
        }

#if ENABLE_DEBUG_DUMP
        // dump updated code list
        dump_huffman_codelist(" updated ", templist, n, head);
#endif

        /* 合并频率最小的两个项 */
        templist[head + 0].depth++;
        templist[head + 1].depth++;
        templist[head + 0].group = group;
        templist[head + 1].group = group;
        templist[head + 1].freq += templist[head + 0].freq;

        head ++; /* 表头指针 */
        group++; /* 分组编号 */

#if ENABLE_DEBUG_DUMP
        // dump merged code list
        dump_huffman_codelist(" merged -", templist, n, head);
#endif

        /* 对 templist 重新排序 */
        for (i=head; i<n-1; i++)
        {
            if (templist[i].freq > templist[i+1].freq)
            {
                HUFCODEITEM tempitem;
                memcpy(&tempitem,        &(templist[i+0]), sizeof(HUFCODEITEM));
                memcpy(&(templist[i+0]), &(templist[i+1]), sizeof(HUFCODEITEM));
                memcpy(&(templist[i+1]), &tempitem,        sizeof(HUFCODEITEM));
            }
            else break;
        }
    }

    /* 对 templist 按 depth 进行快速排序 */
    qsort(templist, n, sizeof(HUFCODEITEM), cmp_depth_item);

#if ENABLE_DEBUG_DUMP
    // dump done code list
    dump_huffman_codelist(" done ---", templist, n, -1);
#endif

    // 生成 jpeg 格式的哈夫曼表
    memset(huftab, 0, sizeof(huftab));
    for (i=0; i<n; i++) {
        huftab[templist[i].depth - 1]++;
        huftab[i + 16] = templist[i].symbol;
    }

#if ENABLE_DEBUG_DUMP
    printf("\nhuftab:\n");
    for (i=0; i<16; i++) {
        printf("%d ", huftab[i]);
    }
    printf("\n");
    for (i=16; i<272; i++) {
        printf("%c ", huftab[i]);
    }
    printf("\n");
#endif

    k    = 0;
    code = 0;
    for (j=templist[0].depth-1; j<16; j++) {
        for (i=0; i<huftab[j]; i++) {
            templist[k++].code = code++;
        }
        code <<= 1;
    }

#if ENABLE_DEBUG_DUMP
    // dump done code list
    dump_huffman_codelist(" code ---", templist, n, -1);
#endif

    for (i=0; i<n; i++) {
        codelist[templist[i].symbol].depth = templist[i].depth;
        codelist[templist[i].symbol].code  = templist[i].code ;
    }

    return TRUE;
}

void huffman_encode_done(HUFCODEC *phc)
{
}

BOOL huffman_encode_run(HUFCODEC *phc)
{
    /* 检查输入输出数据流的有效性 */
    if (!phc->input || !phc->output) return FALSE;

    /* 对输入码流进行编码并输出 */
    while (1) {
        int data = bitstr_getc(phc->input);
        if (data == -1) break;

        {
            int code, i;
            code = phc->codelist[data].code;
            for (i=phc->codelist[data].depth-1; i>=0; i--) {
                if (EOF == bitstr_putbit((code & (1 << i)) ? 1 : 0, phc->output)) {
                    return FALSE;
                }
            }
        }
    }

    /* 返回成功 */
    return TRUE;
}

BOOL huffman_decode_begin(HUFCODEC *phc)
{
    return FALSE;
}

void huffman_decode_done (HUFCODEC *phc)
{
}

BOOL huffman_decode_run  (HUFCODEC *phc)
{
    return FALSE;
}

int  huffman_decode_one  (HUFCODEC *phc)
{
    return -1;
}


#if ENABLE_DEBUG_DUMP
int main(void)
{
    HUFCODEC hufcodec;

    hufcodec.codelist['H'].symbol = 'H';
    hufcodec.codelist['H'].freq   = 1;
    hufcodec.codelist['H'].group  = 'H';
    hufcodec.codelist['H'].depth  = 1;

    hufcodec.codelist['F'].symbol = 'F';
    hufcodec.codelist['F'].freq   = 2;
    hufcodec.codelist['F'].group  = 'F';
    hufcodec.codelist['F'].depth  = 1;

    hufcodec.codelist['C'].symbol = 'C';
    hufcodec.codelist['C'].freq   = 3;
    hufcodec.codelist['C'].group  = 'C';
    hufcodec.codelist['C'].depth  = 1;

    hufcodec.codelist['J'].symbol = 'J';
    hufcodec.codelist['J'].freq   = 4;
    hufcodec.codelist['J'].group  = 'J';
    hufcodec.codelist['J'].depth  = 1;

    hufcodec.codelist['I'].symbol = 'I';
    hufcodec.codelist['I'].freq   = 5;
    hufcodec.codelist['I'].group  = 'I';
    hufcodec.codelist['I'].depth  = 1;

    hufcodec.codelist['D'].symbol = 'D';
    hufcodec.codelist['D'].freq   = 7;
    hufcodec.codelist['D'].group  = 'D';
    hufcodec.codelist['D'].depth  = 1;

    hufcodec.codelist['A'].symbol = 'A';
    hufcodec.codelist['A'].freq   = 8;
    hufcodec.codelist['A'].group  = 'A';
    hufcodec.codelist['A'].depth  = 1;

    hufcodec.codelist['E'].symbol = 'E';
    hufcodec.codelist['E'].freq   = 12;
    hufcodec.codelist['E'].group  = 'E';
    hufcodec.codelist['E'].depth  = 1;

    hufcodec.codelist['K'].symbol = 'K';
    hufcodec.codelist['K'].freq   = 9;
    hufcodec.codelist['K'].group  = 'K';
    hufcodec.codelist['K'].depth  = 1;

    hufcodec.codelist['B'].symbol = 'B';
    hufcodec.codelist['B'].freq   = 16;
    hufcodec.codelist['B'].group  = 'B';
    hufcodec.codelist['B'].depth  = 1;

    hufcodec.codelist['G'].symbol = 'G';
    hufcodec.codelist['G'].freq   = 32;
    hufcodec.codelist['G'].group  = 'G';
    hufcodec.codelist['G'].depth  = 1;

    huffman_encode_begin(&hufcodec);
    huffman_encode_done (&hufcodec);

    return 0;
}
#endif






