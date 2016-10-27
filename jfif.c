/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdefine.h"
#include "jfif.h"

// 预编译开关
#define TEST_JFIF  0

// 内部类型定义
typedef struct {
    // width & height
    int   width;
    int   height;

    // quantization table
    int   nqtab;
    int  *pqtab[16];

    // huffman table ac
    int   nhtabac;
    BYTE *phtabac[16];

    // huffman table dc
    int   nhtabdc;
    BYTE *phtabdc[16];

    // components
    int comp_num;
    struct {
        int id;
        int samp_factor_v;
        int samp_factor_h;
        int qtab_idx;
        int htab_idx_ac;
        int htab_idx_dc;
    } comp_info[4];

    int   datalen;
    BYTE *databuf;
} JFIF;

/* 内部函数实现 */
#if TEST_JFIF
static void jfif_dump(JFIF *jfif)
{
    int i, j;

    printf("++ jfif dump ++\n");
    printf("width : %d\n", jfif->width );
    printf("height: %d\n", jfif->height);
    printf("\n");

    printf("nqtab : %d\n", jfif->nqtab );
    for (i=0; i<jfif->nqtab; i++) {
        printf("qtab%d\n", i);
        for (j=0; j<64; j++) {
            printf("%3d,%c", jfif->pqtab[i][j], j%8 == 7 ? '\n' : ' ');
        }
        printf("\n");
    }

    printf("nhtabac : %d\n", jfif->nhtabac);
    for (i=0; i<jfif->nqtab; i++) {
        int size = 0;
        printf("htabac%d\n", i);
        for (j=0; j<16; j++) {
            size += jfif->phtabac[i][j];
            printf("%3d, ", jfif->phtabac[i][j]);
        }
        printf("\n");
        for (j=0; j<size; j++) {
            printf("%3d,%c", jfif->phtabac[i][16 + j], j%16 == 15 ? '\n' : ' ');
        }
        printf("\n\n");
    }

    printf("nhtabdc : %d\n", jfif->nhtabdc);
    for (i=0; i<jfif->nqtab; i++) {
        int size = 0;
        printf("htabdc%d\n", i);
        for (j=0; j<16; j++) {
            size += jfif->phtabdc[i][j];
            printf("%3d, ", jfif->phtabdc[i][j]);
        }
        printf("\n");
        for (j=0; j<size; j++) {
            printf("%3d,%c", jfif->phtabdc[i][16 + j], j%16 == 15 ? '\n' : ' ');
        }
        printf("\n\n");
    }

    printf("comp_num : %d\n", jfif->comp_num);
    for (i=0; i<jfif->comp_num; i++) {
        printf("id:%d samp_factor_v:%d samp_factor_h:%d qtab_idx:%d htab_idx_ac:%d htab_idx_dc:%d\n",
            jfif->comp_info[i].id,
            jfif->comp_info[i].samp_factor_v,
            jfif->comp_info[i].samp_factor_h,
            jfif->comp_info[i].qtab_idx,
            jfif->comp_info[i].htab_idx_ac,
            jfif->comp_info[i].htab_idx_dc);
    }
    printf("\n");

    printf("datalen : %d\n", jfif->datalen);
    printf("-- jfif dump --\n");
}
#endif

/* 函数实现 */
void* jfif_load(char *file)
{
    JFIF *jfif   = NULL;
    FILE *fp     = NULL;
    int   header = 0;
    int   type   = 0;
    WORD  size   = 0;
    BYTE *buf    = NULL;
    int   ret    = -1;
    long  offset = 0;
    int   i;

    jfif = calloc(1, sizeof(JFIF));
    buf  = calloc(1, 0x10000);
    if (!jfif || !buf) goto done;

    fp = fopen(file, "rb");
    if (!fp) goto done;

    while (1) {
        do { header = fgetc(fp); } while (header != EOF && header != 0xff); // get header
        do { type   = fgetc(fp); } while (type   != EOF && type   == 0xff); // get type
        if (header == EOF || type == EOF) {
            printf("file eof !\n");
            break;
        }

        if ((type == 0xd8) || (type == 0xd9) || (type == 0x01) || (type >= 0xd0 && type <= 0xd7)) {
            size = 0;
        }
        else {
            size  = fgetc(fp) << 8;
            size |= fgetc(fp) << 0;
            size -= 2;
        }

        fread(buf, size, 1, fp);
        switch (type) {
        case 0xc0: // SOF0
            jfif->width    = (buf[3] << 8) | (buf[4] << 0);
            jfif->height   = (buf[1] << 8) | (buf[2] << 0);
            jfif->comp_num = buf[5];
            for (i=0; i<jfif->comp_num; i++) {
                jfif->comp_info[i].id = buf[6 + i * 3];
                jfif->comp_info[i].samp_factor_v = (buf[7 + i * 3] >> 0) & 0x0f;
                jfif->comp_info[i].samp_factor_h = (buf[7 + i * 3] >> 4) & 0x0f;
                jfif->comp_info[i].qtab_idx      =  buf[8 + i * 3];
            }
            break;

        case 0xda: // SOS
            jfif->comp_num = buf[0];
            for (i=0; i<jfif->comp_num; i++) {
                jfif->comp_info[i].id = buf[1 + i * 2];
                jfif->comp_info[i].htab_idx_ac = (buf[2 + i * 2] >> 0) & 0x0f;
                jfif->comp_info[i].htab_idx_dc = (buf[2 + i * 2] >> 4) & 0x0f;
            }
            offset = ftell(fp);
            ret    = 0;
            goto read_data;
            break;

        case 0xdb: { // DQT
                int idx = buf[0] & 0x0f;
                int f16 = buf[0] & 0xf0;
                jfif->nqtab++;
                if (!jfif->pqtab[idx]) jfif->pqtab[idx] = malloc(64 * sizeof(int));
                if (!jfif->pqtab[idx]) break;
                if (f16) { // 16bit
                    for (i=0; i<64; i++) {
                        jfif->pqtab[idx][i] = (buf[1 + i * 2] << 8) | (buf[2 + i * 2] << 0);
                    }
                }
                else { // 8bit
                    for (i=0; i<64; i++) {
                        jfif->pqtab[idx][i] = buf[1 + i];
                    }
                }
            }
            break;

        case 0xc4: { // DHT
                int idx  = buf[0] & 0x0f;
                int fac  = buf[0] & 0xf0;
                int size = 0;
                for (i=1; i<1+16; i++) size += buf[i];
                if (fac) {
                    jfif->nhtabac++;
                    if (jfif->phtabac[idx]) free(jfif->phtabac[idx]);
                    jfif->phtabac[idx] = malloc(16 + size);
                    if (jfif->phtabac[idx]) memcpy(jfif->phtabac[idx], &buf[1], 16 + size);
                }
                else {
                    jfif->nhtabdc++;
                    if (jfif->phtabdc[idx]) free(jfif->phtabdc[idx]);
                    jfif->phtabdc[idx] = malloc(16 + size);
                    if (jfif->phtabdc[idx]) memcpy(jfif->phtabdc[idx], &buf[1], 16 + size);
                }
            }
            break;
        }
    }

read_data:
    fseek(fp, 0, SEEK_END);
    jfif->datalen = ftell(fp) - offset;
    jfif->databuf = malloc(jfif->datalen);
    if (jfif->databuf) {
        fseek(fp, offset, SEEK_SET);
        fread(jfif->databuf, jfif->datalen, 1, fp);
    }

done:
    if (buf) free  (buf);
    if (fp ) fclose(fp );
    if (ret == -1) {
        jfif_free(jfif);
        jfif = NULL;
    }
    return jfif;
}

int jfif_save(void *ctxt, char *file)
{
    JFIF *jfif = (JFIF*)ctxt;
    FILE *fp   = NULL;
    int   len  = 0;
    int   i, j;
    int   ret  = -1;

    fp = fopen(file, "wb");
    if (!fp) goto done;

    // output SOI
    fputc(0xff, fp);
    fputc(0xd8, fp);

    // output DQT
    for (i=0; i<jfif->nqtab; i++) {
        len = 2 + 1 + 64;
        fputc(0xff, fp);
        fputc(0xdb, fp);
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i   , fp);
        if (jfif->pqtab[i]) {
            for (j=0; j<64; j++) {
                fputc(jfif->pqtab[i][j], fp);
            }
        }
    }

    // output SOF0
    len = 2 + 1 + 2 + 2 + 1 + 3 * jfif->comp_num;
    fputc(0xff, fp);
    fputc(0xc0, fp);
    fputc(len >> 8, fp);
    fputc(len >> 0, fp);
    fputc(8   , fp); // precision 8bit
    fputc(jfif->height >> 8, fp); // height
    fputc(jfif->height >> 0, fp); // height
    fputc(jfif->width  >> 8, fp); // width
    fputc(jfif->width  >> 0, fp); // width
    fputc(jfif->comp_num, fp);
    for (i=0; i<jfif->comp_num; i++) {
        fputc(jfif->comp_info[i].id, fp);
        fputc((jfif->comp_info[i].samp_factor_v << 0)|(jfif->comp_info[i].samp_factor_h << 4), fp);
        fputc(jfif->comp_info[i].qtab_idx, fp);
    }

    // output DHT AC
    for (i=0; i<jfif->nhtabac; i++) {
        fputc(0xff, fp);
        fputc(0xc4, fp);
        len = 2 + 1 + 16;
        for (j=0; j<16; j++) len += jfif->phtabac[i][j];
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x10, fp);
        if (jfif->phtabac[i]) {
            fwrite(jfif->phtabac[i], len - 3, 1, fp);
        }
    }

    // output DHT DC
    for (i=0; i<jfif->nhtabdc; i++) {
        fputc(0xff, fp);
        fputc(0xc4, fp);
        len = 2 + 1 + 16;
        for (j=0; j<16; j++) len += jfif->phtabdc[i][j];
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x00, fp);
        if (jfif->phtabdc[i]) {
            fwrite(jfif->phtabdc[i], len - 3, 1, fp);
        }
    }

    // output SOS
    len = 2 + 1 + 2 * jfif->comp_num + 3;
    fputc(0xff, fp);
    fputc(0xda, fp);
    fputc(len >> 8, fp);
    fputc(len >> 0, fp);
    fputc(jfif->comp_num, fp);
    for (i=0; i<jfif->comp_num; i++) {
        fputc(jfif->comp_info[i].id, fp);
        fputc((jfif->comp_info[i].htab_idx_ac << 0)|(jfif->comp_info[i].htab_idx_dc << 4), fp);
    }
    fputc(0x00, fp);
    fputc(0x00, fp);
    fputc(0x00, fp);

    // output data
    if (jfif->databuf) {
        fwrite(jfif->databuf, jfif->datalen, 1, fp);
    }
    ret = 0;

done:
    if (fp) fclose(fp);
    return ret;
}

void jfif_free(void *ctxt)
{
    JFIF *jfif = (JFIF*)ctxt;
    int   i;
    if (!jfif) return;
    for (i=0; i<16; i++) {
        if (jfif->pqtab[i]  ) free(jfif->pqtab[i]  );
        if (jfif->phtabac[i]) free(jfif->phtabac[i]);
        if (jfif->phtabdc[i]) free(jfif->phtabdc[i]);
    }
    if (jfif->databuf) free(jfif->databuf);
    free(jfif);
}

#if TEST_JFIF
int main(int argc, char *argv[])
{
    void *jfif = NULL;

    if (argc < 2) {
        printf(
            "jfif test program\n"
            "usage: jfif filename\n"
        );
    }

    jfif = jfif_load(argv[1]);
    jfif_dump(jfif);
    jfif_save(jfif, "save.jpg");
    jfif_free(jfif);

    return 0;
}
#endif





