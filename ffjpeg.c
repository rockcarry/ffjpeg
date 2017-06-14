#include <stdlib.h>
#include <stdio.h>
#include "jfif.h"
#include "color.h"
#include "bmp.h"

int main(int argc, char *argv[])
{
    void *jfif = NULL;
    BMP   bmp  = {0};

    if (argc < 2) {
        printf(
            "jfif test program\n"
            "usage: jfif filename\n"
        );
        return 0;
    }

    // init color convert
    init_color_convert();

    jfif = jfif_load(argv[1]);
    jfif_decode(jfif, &bmp);
    jfif_save  (jfif, "save.jpg");
    jfif_free  (jfif);

    bmp_save(&bmp, "decode.bmp");
    bmp_free(&bmp);

    return 0;
}

