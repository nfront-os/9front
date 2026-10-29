/*
 * APE (GCC port): <wchar.h> (libap). wchar_t is
 * the compiler's, 32 bits (<stddef.h>); the encoding UTF-8.
 * The restartable conversions (ap/gnu/multibyte.c), wide I/O and
 * wcscoll, wcsxfrm, wcsftime (wstdio.c), the wide printf, scanf and
 * wcsto* (musl's engine, sys/src/ape/lib/musl/ape/bridge.c), the string functions
 * (musl's).
 */
#ifndef __APE_WCHAR
#define __APE_WCHAR

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WINT_TYPE__
#define __WINT_TYPE__ int
#endif
typedef __WINT_TYPE__ wint_t;

#ifndef WINT_MIN
#define WINT_MIN	__WINT_MIN__
#define WINT_MAX	__WINT_MAX__
#endif
#ifndef WCHAR_MIN
#define WCHAR_MIN	__WCHAR_MIN__
#define WCHAR_MAX	__WCHAR_MAX__
#endif
#ifndef WEOF
#define WEOF		((wint_t)-1)
#endif

/* a partial character (ap/gnu/multibyte.c) */
typedef struct {
	unsigned __opaque1, __opaque2;
} mbstate_t;

extern wint_t	btowc(int);
extern int	wctob(wint_t);
extern int	mbsinit(const mbstate_t *);
extern size_t	mbrlen(const char *, size_t, mbstate_t *);
extern size_t	mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
extern size_t	wcrtomb(char *, wchar_t, mbstate_t *);
extern size_t	mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);
extern size_t	wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
extern size_t	mbsnrtowcs(wchar_t *, const char **, size_t, size_t, mbstate_t *);
extern size_t	wcsnrtombs(char *, const wchar_t **, size_t, size_t, mbstate_t *);

extern wchar_t	*wcscpy(wchar_t *, const wchar_t *);
extern wchar_t	*wcsncpy(wchar_t *, const wchar_t *, size_t);
extern wchar_t	*wcscat(wchar_t *, const wchar_t *);
extern wchar_t	*wcsncat(wchar_t *, const wchar_t *, size_t);
extern int	wcscmp(const wchar_t *, const wchar_t *);
extern int	wcsncmp(const wchar_t *, const wchar_t *, size_t);
extern int	wcscoll(const wchar_t *, const wchar_t *);
extern size_t	wcsxfrm(wchar_t *, const wchar_t *, size_t);
extern wchar_t	*wcschr(const wchar_t *, wchar_t);
extern wchar_t	*wcsrchr(const wchar_t *, wchar_t);
extern size_t	wcscspn(const wchar_t *, const wchar_t *);
extern size_t	wcsspn(const wchar_t *, const wchar_t *);
extern wchar_t	*wcspbrk(const wchar_t *, const wchar_t *);
extern wchar_t	*wcstok(wchar_t *, const wchar_t *, wchar_t **);
extern size_t	wcslen(const wchar_t *);
extern wchar_t	*wcsstr(const wchar_t *, const wchar_t *);
extern wchar_t	*wcswcs(const wchar_t *, const wchar_t *);
extern wchar_t	*wmemchr(const wchar_t *, wchar_t, size_t);
extern int	wmemcmp(const wchar_t *, const wchar_t *, size_t);
extern wchar_t	*wmemcpy(wchar_t *, const wchar_t *, size_t);
extern wchar_t	*wmemmove(wchar_t *, const wchar_t *, size_t);
extern wchar_t	*wmemset(wchar_t *, wchar_t, size_t);
extern float	wcstof(const wchar_t *, wchar_t **);
extern double	wcstod(const wchar_t *, wchar_t **);
extern long double	wcstold(const wchar_t *, wchar_t **);
extern long	wcstol(const wchar_t *, wchar_t **, int);
extern unsigned long	wcstoul(const wchar_t *, wchar_t **, int);
extern long long	wcstoll(const wchar_t *, wchar_t **, int);
extern unsigned long long	wcstoull(const wchar_t *, wchar_t **, int);
extern int	fwide(FILE *, int);
extern int	wprintf(const wchar_t *, ...);
extern int	fwprintf(FILE *, const wchar_t *, ...);
extern int	swprintf(wchar_t *, size_t, const wchar_t *, ...);
extern int	vwprintf(const wchar_t *, va_list);
extern int	vfwprintf(FILE *, const wchar_t *, va_list);
extern int	vswprintf(wchar_t *, size_t, const wchar_t *, va_list);
extern int	wscanf(const wchar_t *, ...);
extern int	fwscanf(FILE *, const wchar_t *, ...);
extern int	swscanf(const wchar_t *, const wchar_t *, ...);
extern int	vwscanf(const wchar_t *, va_list);
extern int	vfwscanf(FILE *, const wchar_t *, va_list);
extern int	vswscanf(const wchar_t *, const wchar_t *, va_list);
extern wint_t	fgetwc(FILE *);
extern wint_t	getwc(FILE *);
extern wint_t	getwchar(void);
extern wint_t	fputwc(wchar_t, FILE *);
extern wint_t	putwc(wchar_t, FILE *);
extern wint_t	putwchar(wchar_t);
extern wchar_t	*fgetws(wchar_t *, int, FILE *);
extern int	fputws(const wchar_t *, FILE *);
extern wint_t	ungetwc(wint_t, FILE *);
extern size_t	wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm *);
extern wchar_t	*wcsdup(const wchar_t *);
extern size_t	wcsnlen(const wchar_t *, size_t);
extern wchar_t	*wcpcpy(wchar_t *, const wchar_t *);
extern wchar_t	*wcpncpy(wchar_t *, const wchar_t *, size_t);
extern int	wcscasecmp(const wchar_t *, const wchar_t *);
extern int	wcsncasecmp(const wchar_t *, const wchar_t *, size_t);
extern int	wcwidth(wchar_t);
extern int	wcswidth(const wchar_t *, size_t);

#ifdef __cplusplus
}
#endif
#endif
