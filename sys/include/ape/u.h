#ifndef __U_H
#define __U_H
#ifndef _PLAN9_SOURCE
/* This header file is an extension to ANSI/POSIX */
#endif

#define nil		((void*)0)
typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
#if defined(__x86_64__) || defined(__aarch64__)	/* GCC port: LP64 */
/*
 * GCC port: 32 bits, as with kencc and amd64/include/u.h; Plan 9
 * structures shared with the kernel (Tos) and u32int depend on it
 */
typedef unsigned int	ulong;
#else
typedef unsigned long	ulong;
#endif
typedef unsigned int	uint;
typedef signed char	schar;
typedef	long long	vlong;
typedef	unsigned long long uvlong;
typedef	uint		Rune;
typedef 	union FPdbleword FPdbleword;
typedef	char*	p9va_list;

typedef uchar	u8int;
typedef ushort	u16int;
typedef ulong	u32int;
typedef uvlong	u64int;
typedef signed char	s8int;
typedef signed short	s16int;
typedef signed int	s32int;
typedef signed long long	s64int;

#endif
