#ifndef __FFJPEG_HUFFMAN_H__
#define __FFJPEG_HUFFMAN_H__

/* 包含头文件 */
#include "stdefine.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 常量定义 */
#define MAX_HUFFMAN_CODE_LEN  16

extern const BYTE STD_HUFTAB_LUMIN_AC[];
extern const BYTE STD_HUFTAB_LUMIN_DC[];
extern const BYTE STD_HUFTAB_CHROM_AC[];
extern const BYTE STD_HUFTAB_CHROM_DC[];

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

/* 编码器类型定义 */
typedef struct
{
    BYTE         huftab[MAX_HUFFMAN_CODE_LEN + 256]; /* 哈夫曼表 */
    int          first [MAX_HUFFMAN_CODE_LEN];       /* first   */
    int          index [MAX_HUFFMAN_CODE_LEN];       /* index   */
    HUFCODEITEM  codelist[256];/* 编码表 */
    void        *input;        /* input bit stream  */
    void        *output;       /* output bit stream */
} HUFCODEC;


/* 函数声明 */
/* 标准的符号频率统计器 */
void huffman_stat_freq(HUFCODEITEM codelist[256], void *stream);


/* 编码 */
/*
  编码方法：
      1. 初始化 HUFCODEC 中的 freqlist 成员，可以采用
         huffman_stat_freq 进行频率统计，也可以自己编
         码实现频率统计

      2. 调用 huffman_encode_init 函数将会生成 huftab,
         codelist 分别对应了哈夫曼表 (JPEG format) 和
         编码表

      3. 指定 input 和 output 分别代表编码过程中的输入
         数据流和输出数据流

      4. 调用 huffman_encode_run 可以完成自动的编码操
         作编码的结束条件是 input 或 output 到达了尾部

      5. 调用 huffman_encode_done 结束哈夫曼编码

      6. 也可以通过 huffman_encode_step 进行单步的哈夫
         曼编码操作
 */
// flag == 0, init from code freq list
// flag == 1, init from huffman table
void huffman_encode_init(HUFCODEC *phc, int flag);
void huffman_encode_done(HUFCODEC *phc);
BOOL huffman_encode_run (HUFCODEC *phc);

/* 一次只编码一个符号 */
BOOL huffman_encode_step(HUFCODEC *phc, int symbol);


/* 解码 */
/*
  解码方法：
      1. 初始化 HUFCODEC 中的 huftab 成员，huftab
         是 JPEG format 的范式哈夫曼表

      2. 调用 huffman_decode_init 将会根据 huftab
         生成 first 和 index 两个表

      3. 指定 input 和 output 分别代表解码过程中的输
         入数据流和输出数据流

      4. 调用 huffman_decode_run 可以完成自动的解码
         操作解码的结束条件是 input 或 output 到达了
         尾部

      5. 调用 huffman_decode_done 结束哈夫曼解码

      6. 也可以通过 huffman_decode_step 进行单步
         的哈夫曼解码操作，该函数每次调用只会从数
         据流中解码出一个符号
 */
void huffman_decode_init(HUFCODEC *phc);
void huffman_decode_done(HUFCODEC *phc);
BOOL huffman_decode_run (HUFCODEC *phc);

/* 一次只解码一个符号 */
int  huffman_decode_step(HUFCODEC *phc);

#ifdef __cplusplus
}
#endif

#endif






