#ifndef __FFJPEG_HUFFMAN_H__
#define __FFJPEG_HUFFMAN_H__

/* 包含头文件 */
#include "stdefine.h"

/* 类型定义 */
/* 编码表项类型定义 */
typedef struct
{
    int symbol; /* 符号 */
    int freq;   /* 频率 */
    int group;  /* 分组 */
    int depth;  /* 码长 */
    int code;   /* 码字 */
} HUFCODEITEM;

/* bit stream 读写函数接口 */
typedef int  (*PFNCB_BITSTR_READ )(void *stream, int nbit);
typedef int  (*PFNCB_BITSTR_WRITE)(void *stream, int nbit, int data);

/* 编码器类型定义 */
typedef struct
{
    BYTE               huftab[272];  /* 哈夫曼表     - DE */
    int                first[18];    /* first        - DE */
    int                index[18];    /* index        - D  */
    HUFCODEITEM        codelist[256];/* 编码表       - E  */
    void              *input;        /* input bit stream  */
    void              *output;       /* output bit stream */
    PFNCB_BITSTR_READ  readbits;     /* read bits         */
    PFNCB_BITSTR_WRITE writebits;    /* write bits        */
} HUFCODEC;

#ifdef __cplusplus
extern "C" {
#endif

/* 函数声明 */
/* 标准的符号频率统计器 */
void huffman_stat_freq(HUFCODEITEM codelist[256], void *stream, PFNCB_BITSTR_READ readbits);


/* 编码 */
/*
  编码方法：
      1. 初始化 HUFCODEC 中的 freqlist 成员，可以采用
         huffman_stat_freq 进行频率统计，也可以自己编
         码实现频率统计

      2. 调用 huffman_encode_begin 函数将会生成 huftab,
         codelist 分别对应了哈夫曼表 (JPEG format)和
         编码表

      3. 指定 srcbitstr 和 destbitstr 分别代表编码过程
         中的源数据流和目的数据流

      4. 指定编码进度回调函数指针 pps, 用于编码进度控
         制

      5. 调用 huffman_encode_run 可以完成自动的编码操
         作编码的结束条件是 srcbitstr 或 destbitstr 到
         达了尾部

      6. 调用 huffman_encode_done 结束哈夫曼编码

      7. 也可以不通过 huffman_encode_run 进行编码，在
         huffman_encode_begin 以后，用户可以自己根
         据 codelist 完成编码操作
 */
BOOL huffman_encode_begin(HUFCODEC *phc);
void huffman_encode_done (HUFCODEC *phc);
BOOL huffman_encode_run  (HUFCODEC *phc);

/* 解码 */
/*
  解码方法：
      1. 初始化 HUFCODEC 中的 huftab 成员，hufman
         是 JPEG format 的范式哈夫曼表

      2. 调用 huffman_decode_begin 将会根据 huftab
         生成 first 和 index 两个表

      3. 指定 srcbitstr 和 destbitstr 分别代表解码
         过程中的源数据流和目的数据流

      4. 指定解码进度回调函数指针 pps, 用于解码进
         度控制

      3. 调用 huffman_decode_run 可以完成自动的解码
         操作解码的结束条件是 srcbitstr 或 destbitstr
         到达了尾部

      6. 调用 huffman_decode_done 结束哈夫曼解码

      7. 也可以通过 huffman_decode_one 进行单步
         的哈夫曼解码操作，该函数每次调用只会从数
         据流中解码出一个符号
 */
BOOL huffman_decode_begin(HUFCODEC *phc);
void huffman_decode_done (HUFCODEC *phc);
BOOL huffman_decode_run  (HUFCODEC *phc);

/* 一次只解码出一个符号 */
int  huffman_decode_one  (HUFCODEC *phc);

#ifdef __cplusplus
}
#endif

#endif






