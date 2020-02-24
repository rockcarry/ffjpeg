# makefile for ffjpeg project
# written by rockcarry

CC      = gcc
AR      = ar
CCFLAGS = -Wall
SRCS    = src

OBJS = \
    $(SRCS)/color.o   \
    $(SRCS)/dct.o     \
    $(SRCS)/quant.o   \
    $(SRCS)/zigzag.o  \
    $(SRCS)/bitstr.o  \
    $(SRCS)/huffman.o \
    $(SRCS)/bmp.o     \
    $(SRCS)/jfif.o

PROG = ffjpeg
LIB  = lib$(PROG).a
SUFF = 

ifeq ($(OS),Windows_NT)
    SUFF := .exe
endif

all : $(PROG)$(SUFF)

$(LIB) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(SRCS)/%.o : $(SRCS)/%.c $(SRCS)/%.h $(SRCS)/stdefine.h
	$(CC) $(CCFLAGS) -o $@ $< -c

$(PROG)$(SUFF) : $(SRCS)/$(PROG).o $(LIB)
	$(CC) $(CCFLAGS) -o $@ $< $(LIB)

clean :
	-rm -f $(SRCS)/*.o $(LIB) $(PROG)$(SUFF)

# rockcarry
# 2020.2.24
# THE END

