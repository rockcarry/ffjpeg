# makefile for ffjpeg project
# written by rockcarry

CC      = gcc
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

DLL = ffjpeg.dll

all : $(DLL) $(EXES)

$(DLL) : $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $(OBJS) --share

%.o : %.c %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp %.h stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.c stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.o : %.cpp stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

%.exe : %.c %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL)

%.exe : %.cpp %.h $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL)

%.exe : %.c $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL)

%.exe : %.cpp $(DLL)
	$(CC) $(CCFLAGS) -o $@ $< $(DLL)

clean :
	-rm -f *.o
	-rm -f *.dll
	-rm -f *.exe

# rockcarry
# 2016.1.5
# THE END



