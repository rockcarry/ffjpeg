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

EXES = \
    ffjpeg.exe \

LIB = ffjpeg.a

all : $(LIB) $(EXES)

$(LIB) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.c stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.exe : %.c %.h $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

%.exe : %.cpp %.h $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

%.exe : %.c $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

%.exe : %.cpp $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

clean :
	-rm -f *.o
	-rm -f *.a
	-rm -f *.exe

# rockcarry
# 2020.2.22
# THE END


