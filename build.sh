#!/bin/bash
gcc -Wall -o color.o color.c -c
gcc -Wall -o dct.o dct.c -c
gcc -Wall -o quant.o quant.c -c
gcc -Wall -o zigzag.o zigzag.c -c
gcc -Wall -o bitstr.o bitstr.c -c
gcc -Wall -o huffman.o huffman.c -c
gcc -Wall -o bmp.o bmp.c -c
gcc -Wall -o jfif.o jfif.c -c
gcc -Wall -o ffjpeg.o ffjpeg.c -c
gcc color.o dct.o quant.o zigzag.o bitstr.o huffman.o \
bmp.o jfif.o ffjpeg.o -o ffjpeg
