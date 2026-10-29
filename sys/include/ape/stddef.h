#ifndef __STDDEF_H
#define __STDDEF_H

#if defined(__GNUC__) || defined(_APE_GNU)
/*
 * GCC port, for the gcc driver's programs:
 * - wchar_t is the compiler's (C: int, 32 bits, the type of L"..."; C++: the
 *   keyword), not APE's unsigned short: _WCHAR_T keeps APE's typedef out.
 *   APE's functions with wchar_t (mbtowc & co.) store 16 bits; <stdlib.h>
 *   gives their names to 32-bit ones (ap/gnu/multibyte.c).
 * - ptrdiff_t is the compiler's type (long; APE's: long long, the same size), as
 *   libstdc++'s std::ptrdiff_t is: else the two are ambiguous.
 */
#ifndef _WCHAR_T
#define _WCHAR_T
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif
#endif
#define ptrdiff_t	__ape_ptrdiff_t
#endif

#ifndef __STDDEF_ARCH_H
typedef long _ptrdiff_t;
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif
#define offsetof(ty,mem) ((size_t) &(((ty *)0)->mem))

typedef _ptrdiff_t ptrdiff_t;
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif
#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
#undef ptrdiff_t
typedef __PTRDIFF_TYPE__ ptrdiff_t;
/* offsetof as gcc's builtin (APE's null-pointer macro is no constant expression
 * in C++); C11's max_align_t, as gcc's own stddef.h has it */
#undef offsetof
#define offsetof(t, m)	__builtin_offsetof(t, m)
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L) \
	|| (defined __cplusplus && __cplusplus >= 201103L)
typedef struct {
	long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
	long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
#endif
#endif
#endif /* __STDDEF_H */
