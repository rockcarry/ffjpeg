/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stdefine.h"
#include "bitstr.h"
#include "huffman.h"
#include "quant.h"
#include "zigzag.h"
#include "dct.h"
#include "jfif.h"

// 预编译开关
#define TEST_JFIF  1

// 内部类型定义
typedef struct {
    // width & height
    int       width;
    int       height;

    // quantization table
    int      *pqtab[16];

    // huffman codec ac
    HUFCODEC *phcac[16];

    // huffman codec dc
    HUFCODEC *phcdc[16];

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

    for (i=0; i<16; i++) {
        if (!jfif->pqtab[i]) continue;
        printf("qtab%d\n", i);
        for (j=0; j<64; j++) {
            printf("%3d,%c", jfif->pqtab[i][j], j%8 == 7 ? '\n' : ' ');
        }
        printf("\n");
    }

    for (i=0; i<16; i++) {
        int size = 16;
        if (!jfif->phcac[i]) continue;
        printf("htabac%d\n", i);
        for (j=0; j<16; j++) {
            size += jfif->phcac[i]->huftab[j];
        }
        for (j=0; j<size; j++) {
            printf("%3d,%c", jfif->phcac[i]->huftab[j], j%16 == 15 ? '\n' : ' ');
        }
        printf("\n\n");
    }

    for (i=0; i<16; i++) {
        int size = 16;
        if (!jfif->phcdc[i]) continue;
        printf("htabdc%d\n", i);
        for (j=0; j<16; j++) {
            size += jfif->phcdc[i]->huftab[j];
        }
        for (j=0; j<size; j++) {
            printf("%3d,%c", jfif->phcdc[i]->huftab[j], j%16 == 15 ? '\n' : ' ');
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

static void dump_du(int *du)
{
    int i;
    for (i=0; i<64; i++) {
        printf("%3d%c", du[i], i % 8 == 7 ? '\n' : ' ');
    }
    printf("\n");
}
#endif

static int ALIGN(int x, int y) {
    // y must be a power of 2.
    return (x + y - 1) & ~(y - 1);
}

static int bitstr_get_bits(void *stream, int n)
{
    int buf = 0;
    while (n--) {
        buf <<= 1;
        buf  |= bitstr_getb(stream);
    }
    return buf;
}

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
                if (!jfif->pqtab[idx]) jfif->pqtab[idx] = calloc(1, 64 * sizeof(int));
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
                    if (!jfif->phcac[idx]) jfif->phcac[idx] = calloc(1, sizeof(HUFCODEC));
                    if ( jfif->phcac[idx]) memcpy(jfif->phcac[idx]->huftab, &buf[1], 16 + size);
                }
                else {
                    if (!jfif->phcdc[idx]) jfif->phcdc[idx] = calloc(1, sizeof(HUFCODEC));
                    if ( jfif->phcdc[idx]) memcpy(jfif->phcdc[idx]->huftab, &buf[1], 16 + size);
                }
            }
            break;
        }
    }

read_data:
    fseek(fp, 0, SEEK_END);
    jfif->datalen = ftell(fp) - offset;
    jfif->databuf = calloc(1, jfif->datalen);
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
    for (i=0; i<16; i++) {
        if (!jfif->pqtab[i]) continue;
        len = 2 + 1 + 64;
        fputc(0xff, fp);
        fputc(0xdb, fp);
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i   , fp);
        for (j=0; j<64; j++) {
            fputc(jfif->pqtab[i][j], fp);
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
    for (i=0; i<16; i++) {
        if (!jfif->phcac[i]) continue;
        fputc(0xff, fp);
        fputc(0xc4, fp);
        len = 2 + 1 + 16;
        for (j=0; j<16; j++) len += jfif->phcac[i]->huftab[j];
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x10, fp);
        fwrite(jfif->phcac[i]->huftab, len - 3, 1, fp);
    }

    // output DHT DC
    for (i=0; i<16; i++) {
        if (!jfif->phcdc[i]) continue;
        fputc(0xff, fp);
        fputc(0xc4, fp);
        len = 2 + 1 + 16;
        for (j=0; j<16; j++) len += jfif->phcdc[i]->huftab[j];
        fputc(len >> 8, fp);
        fputc(len >> 0, fp);
        fputc(i + 0x00, fp);
        fwrite(jfif->phcdc[i]->huftab, len - 3, 1, fp);
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
        if (jfif->pqtab[i]) free(jfif->pqtab[i]);
        if (jfif->phcac[i]) free(jfif->phcac[i]);
        if (jfif->phcdc[i]) free(jfif->phcdc[i]);
    }
    if (jfif->databuf) free(jfif->databuf);
    free(jfif);
}

int jfif_decode(void *ctxt, BYTE *out[4])
{
    JFIF *jfif = (JFIF*)ctxt;
    void *bs   = NULL;
    int   i, c, h, v;
    int   dc[4]= {0};
    int   mcuw, mcuh, mcuc, mcur, mcui, jw, jh;
    int   sfh_max = 0;
    int   sfv_max = 0;

    if (!ctxt || !out) {
        printf("invalid input params !\n");
        return -1;
    }

    //++ calculate mcu info
    for (c=0; c<jfif->comp_num; c++) {
        if (sfh_max < jfif->comp_info[c].samp_factor_h) {
            sfh_max = jfif->comp_info[c].samp_factor_h;
        }
        if (sfv_max < jfif->comp_info[c].samp_factor_v) {
            sfv_max = jfif->comp_info[c].samp_factor_v;
        }
    }
    mcuw = sfh_max * 8;
    mcuh = sfv_max * 8;
    jw = ALIGN(jfif->width , mcuw);
    jh = ALIGN(jfif->height, mcuh);
    mcuc = jw / mcuw;
    mcur = jh / mcuh;
    //-- calculate mcu info

    //++ allocate buffers for output
    memset(out, 0, sizeof(BYTE*) * 4);
    for (c=0; c<jfif->comp_num; c++) {
        out[c] = malloc((jw * jfif->comp_info[c].samp_factor_h / sfh_max) * (jh * jfif->comp_info[c].samp_factor_v / sfv_max));
    }
    //-- allocate buffers for output

    // open bit stream
    bs = bitstr_open(BITSTR_MEM, (char*)jfif->databuf, (char*)jfif->datalen);
    if (!bs) {
        printf("failed to open bitstr for jfif_decode !");
        return -1;
    }

    // init huffman codec
    for (i=0; i<16; i++) {
        if (jfif->phcac[i]) {
            jfif->phcac[i]->input = bs;
            huffman_decode_init(jfif->phcac[i]);
        }
        if (jfif->phcdc[i]) {
            jfif->phcdc[i]->input = bs;
            huffman_decode_init(jfif->phcdc[i]);
        }
    }

    for (mcui=0; mcui<mcuc*mcur; mcui++) {
        for (c=0; c<jfif->comp_num; c++) {
            for (v=0; v<jfif->comp_info[c].samp_factor_v; v++) {
                for (h=0; h<jfif->comp_info[c].samp_factor_h; h++) {
                    HUFCODEC *hcac = jfif->phcac[jfif->comp_info[c].htab_idx_ac];
                    HUFCODEC *hcdc = jfif->phcdc[jfif->comp_info[c].htab_idx_dc];
                    int size, znum, code;
                    int idct[64] = {0};
                    int du  [64] = {0};

                    //+ decode dc
                    size = huffman_decode_step(hcdc) & 0xf;
                    if (size) {
                        code = bitstr_get_bits(bs, size);
                        category_decode(&size, &code);
                    }
                    else {
                        code = 0;
                    }
                    dc[c] += code;
                    du[0]  = dc[c];
                    //- decode dc

                    //+ decode ac
                    for (i=1; i<64; ) {
                        code = huffman_decode_step(hcac);
                        if (code <= 0) break;
                        size = (code >> 0) & 0xf;
                        znum = (code >> 4) & 0xf;
                        i   += znum;
                        code = bitstr_get_bits(bs, size);
                        category_decode(&size, &code);
                        if (i < 64) du[i++] = code;
                    }
                    //- decode ac

                    // de-quantize
                    quantize_decode(du, jfif->pqtab[jfif->comp_info[c].qtab_idx]);

                    // de-zigzag
                    zigzag_decode(du);

                    // idct
                    idct2d8x8(idct, du);

#if TEST_JFIF
                    // dump du
//                  dump_du(idct);
#endif
                }
            }
        }
    }

    // close huffman codec
    for (i=0; i<16; i++) {
        if (jfif->phcac[i]) huffman_decode_done(jfif->phcac[i]);
        if (jfif->phcdc[i]) huffman_decode_done(jfif->phcdc[i]);
    }

    // close bit stream
    bitstr_close(bs);
    return 0;
}

#if TEST_JFIF
int main(int argc, char *argv[])
{
    void *jfif   = NULL;
    BYTE *out[4] = {0};
    int   i;

    if (argc < 2) {
        printf(
            "jfif test program\n"
            "usage: jfif filename\n"
        );
    }

    jfif = jfif_load(argv[1]);
    jfif_decode(jfif, out);
    jfif_dump  (jfif);
    jfif_save  (jfif, "save.jpg");
    jfif_free  (jfif);

    for (i=0; i<4; i++) {
        if (out[i]) free(out[i]);
    }

    return 0;
}
#endif





