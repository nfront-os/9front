/*
 * APE (GCC port): POSIX's <strings.h>; the functions are libbsd's (bsd.h
 * declares them the same way for gcc), strncasecmp through ap/gnu/string.c
 * (libbsd's takes an int length: SIZE_MAX would be -1).
 */
#ifndef __APE_STRINGS_H
#define __APE_STRINGS_H
#pragma lib "/$M/lib/ape/libbsd.a"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
extern int	ffs(int);
extern int	strcasecmp(const char *, const char *);
extern int	strncasecmp(const char *, const char *, size_t) __asm__("__apecompat_strncasecmp");
#ifdef __cplusplus
}
#endif
#endif
