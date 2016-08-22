#ifndef __FFJPEG_JFIF_H__
#define __FFJPEG_JFIF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 类型定义 */
typedef (*PFN_JFIF_CB)(int seg_type, void *data, int len);

/* 函数声明 */
void* jfif_load(char *file, PFN_JFIF_CB callback);
void  jfif_free(void *ctxt);

#ifdef __cplusplus
}
#endif

#endif

