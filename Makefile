# makefile for ffjpeg project
# written by rockcarry

CC      = gcc
AR      = ar
CCFLAGS = -Wall

OBJS = \
    color.o   \
    dct.o     \
    quant.o   \
    zigzag.o  \
    bitstr.o  \
    huffman.o \
    bmp.o     \
    jfif.o

LIB  = libffjpeg.a
PROG = ffjpeg

all : $(LIB) $(PROG)

$(LIB) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

$(PROG) : $(PROG).o $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

clean :
	-rm -f *.o
	-rm -f *.a
	-rm -f *.exe
	-rm -f $(PROG)

# rockcarry
# 2020.2.22
# THE END


