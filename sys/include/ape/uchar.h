/*
 * APE (GCC port): C11's <uchar.h> (libap, ap/gnu/uchar.c): UTF-32
 * characters of 9front's UTF-8 (mbrtowc's, gcc's wchar_t is 32 bits);
 * no UTF-16 conversions (mbrtoc16, c16rtomb).
 */
#ifndef __APE_UCHAR_H
#define __APE_UCHAR_H
#include <stddef.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#else
typedef __CHAR16_TYPE__	char16_t;
typedef __CHAR32_TYPE__	char32_t;
#endif
#if !defined(__cplusplus) || __cplusplus < 202002L
typedef unsigned char	char8_t;
#endif

extern size_t	mbrtoc32(char32_t *, const char *, size_t, mbstate_t *);
extern size_t	c32rtomb(char *, char32_t, mbstate_t *);

#ifdef __cplusplus
}
#endif
#endif
