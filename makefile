# makefile for ffjpeg project
# written by rockcarry

# 编译器定义
CC      = i586-mingw32msvc-gcc
CCFLAGS = -Wall

# 所有的目标文件
OBJS = \
    color.o   \
    dct.o     \
    quant.o   \
    zigzag.o  \
    bitstr.o  \
    huffman.o \
    jfif.o

# 所有的可执行目标
EXES = \
    ffjpeg.exe \

# 输出的库文件
DLL = ffjpeg.dll

# 编译规则
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



