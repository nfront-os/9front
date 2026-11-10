#ifndef __STDLIB_H
#define __STDLIB_H

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port, for the gcc driver's programs: mblen, mbtowc, wctomb, mbstowcs and
 * wcstombs for gcc's 32-bit wchar_t (ap/gnu/multibyte.c; APE's store 16 bits):
 * APE's declarations are renamed while the header is read, the ones below carry
 * the link names __apecompat_*. */
#define mblen		__ape_mblen
#define mbtowc		__ape_mbtowc
#define wctomb		__ape_wctomb
#define mbstowcs	__ape_mbstowcs
#define wcstombs	__ape_wcstombs
#endif
#pragma lib "/$M/lib/ape/libap.a"

#include <stddef.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MB_CUR_MAX 4
#define RAND_MAX 32767

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

#ifdef __cplusplus
extern "C" {
#endif

extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);
extern long long atoll(const char *);
extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);
extern long long int strtoll(const char *, char **, int);
extern unsigned long long int strtoull(const char *, char **, int);
extern int rand(void);
extern void srand(unsigned int seed);
extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern _Noreturn void abort(void);
extern int atexit(void (*func)(void));
extern _Noreturn void exit(int);
extern char *getenv(const char *);
extern int putenv(char *);
#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE)
extern int setenv(const char *, const char *, int);
extern int unsetenv(const char *);
#endif
extern int system(const char *);
extern void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
extern int abs(int);
extern div_t div(int, int);
extern long int labs(long int);
extern ldiv_t ldiv(long int, long int);
extern int mblen(const char *, size_t);
extern int mbtowc(wchar_t *, const char *, size_t);
extern int wctomb(char *, wchar_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);

#ifdef _BSD_SOURCE
extern char *mktemp(char *);
#endif

#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE)
extern int mkstemp(char *);
extern int mkstemps(char *, int);
#endif

/* malloc.c: blocks aligned to a power of two (POSIX, C11) */
extern int posix_memalign(void **, size_t, size_t);
extern void *aligned_alloc(size_t, size_t);

#ifdef __cplusplus
}
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: what C99, C11 and POSIX add: strtof and strtold (musl's,
 * ap/gnu/c99stdio.c), llabs, lldiv, _Exit, quick_exit and at_quick_exit,
 * realpath (ap/gnu) */
#undef mblen
#undef mbtowc
#undef wctomb
#undef mbstowcs
#undef wcstombs

#ifdef __cplusplus
extern "C" {
#endif
typedef struct { long long quot, rem; } lldiv_t;

extern float	strtof(const char *, char **);
extern long double	strtold(const char *, char **);
extern long long	llabs(long long);
extern lldiv_t	lldiv(long long, long long);
extern _Noreturn void	_Exit(int);
extern int	at_quick_exit(void (*)(void));
extern _Noreturn void	quick_exit(int);
extern char	*realpath(const char *, char *);
extern int	mkostemp(char *, int);	/* POSIX.1-2024's (ap/gnu/gnuext.c) */
/* POSIX's ptys: ptyfs' (ap/gnu/gnuext.c on libbsd's _getpty, ptsname) */
extern int	posix_openpt(int);
extern int	grantpt(int);
extern int	unlockpt(int);
extern char	*ptsname(int);

extern int	mblen(const char *, size_t)
		__asm__("__apecompat_mblen");
extern int	mbtowc(wchar_t *, const char *, size_t)
		__asm__("__apecompat_mbtowc");
extern int	wctomb(char *, wchar_t)
		__asm__("__apecompat_wctomb");
extern size_t	mbstowcs(wchar_t *, const char *, size_t)
		__asm__("__apecompat_mbstowcs");
extern size_t	wcstombs(char *, const wchar_t *, size_t)
		__asm__("__apecompat_wcstombs");
#ifdef __cplusplus
}
#endif
/* alloca, which glibc's <stdlib.h> gives with _GNU_SOURCE or _DEFAULT_SOURCE */
#if (defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE)) && !defined(alloca)
#define alloca(n)	__builtin_alloca(n)
#endif
#endif
#endif /* __STDLIB_H */
