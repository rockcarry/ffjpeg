#ifndef __FFJPEG_JFIF_H__
#define __FFJPEG_JFIF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* º¯ÊýÉùÃ÷ */
void* jfif_load(char *file);
int   jfif_save(void *ctxt, char *file);
void  jfif_free(void *ctxt);

#ifdef __cplusplus
}
#endif

#endif

