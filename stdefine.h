/* 标准头文件 */
#ifndef __STDEFINE_H__
#define __STDEFINE_H__

#if defined(WIN32)
#include <windows.h>
#else
#define TRUE     1
#define FALSE    0
typedef int            BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

//++ for fixed point
#define FIXQ          11
#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
//-- for fixed point

#endif


