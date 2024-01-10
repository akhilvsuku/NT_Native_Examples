#pragma once
#ifdef ARCH_x86
#ifndef _X86_
#define _X86_
#endif
#endif // ARCH_x86

#define bool  _Bool
#define false 0
#define true  1

#ifdef ARCH_x64
#ifndef _AMD64_
#define _AMD64_
#endif
#endif // ARCH_x64

#include <ntddk.h>
#include <sdkddkver.h>
#include <stdarg.h>
#include <ntstrsafe.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define _NO_CRT_STDIO_INLINE
typedef unsigned long       DWORD;
typedef unsigned short      WORD;


#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x)>=0)
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif
