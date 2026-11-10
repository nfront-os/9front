#ifndef __STRING_H_
#define __STRING_H_

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port, for the gcc driver's programs: APE declares strndup and strdup
 * without const (and strndup without an implementation): the POSIX ones below */
#define strndup	__ape_strndup
#define strdup	__ape_strdup
#endif
#pragma lib "/$M/lib/ape/libap.a"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void *memcpy(void *, const void *, size_t);
extern void *memccpy(void*, void*, int, size_t);
extern void *memmove(void *, const void *, size_t);
extern char *strcpy(char *, const char *);
extern char *strncpy(char *, const char *, size_t);
extern char *strcat(char *, const char *);
extern char *strncat(char *, const char *, size_t);
extern int memcmp(const void *, const void *, size_t);
extern int strcmp(const char *, const char *);
extern int strcoll(const char *, const char *);
extern char *strdup(char*);
extern char *strndup(char*, size_t);
extern int strncmp(const char *, const char *, size_t);
extern size_t strxfrm(char *, const char *, size_t);
extern void *memchr(const void *, int, size_t);
extern char *strchr(const char *, int);
extern size_t strcspn(const char *, const char *);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern size_t strspn(const char *, const char *);
extern char *strstr(const char *, const char *);
extern char *strtok(char *, const char *);
extern void *memset(void *, int, size_t);
extern char *strerror(int);
extern size_t strlen(const char *);
extern size_t strnlen(const char *, size_t);

#ifdef _REENTRANT_SOURCE
extern char *strerror_r(int, const char *, int);
extern char *strtok_r(char *, const char *, char **);
#endif

#ifdef _BSD_EXTENSION
#include <bsd.h>
#endif

#ifdef __cplusplus
}
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
#undef strndup
#undef strdup
/* GCC port: <string.h> as POSIX.1-2008 has it: stpcpy, stpncpy, strndup, memmem
 * (ap/gnu/string.c), strsignal (ap/gnu/posix.c), strcasecmp & co. from
 * <strings.h> as glibc, musl and the BSDs have them here too; strtok_r is
 * libap's, APE declares it only with _REENTRANT_SOURCE (and its strerror_r) */
#include <strings.h>
#ifdef __cplusplus
extern "C" {
#endif
extern char	*strdup(const char *);
extern char	*stpcpy(char *, const char *);
extern char	*stpncpy(char *, const char *, size_t);
extern char	*strndup(const char *, size_t);
extern char	*strtok_r(char *, const char *, char **);
extern void	*memmem(const void *, size_t, const void *, size_t);
extern char	*strsignal(int);
extern char	*strchrnul(const char *, int);	/* GNU's, the BSDs' (ap/gnu/gnuext.c) */
#ifdef __cplusplus
}
#endif
#endif
#endif
