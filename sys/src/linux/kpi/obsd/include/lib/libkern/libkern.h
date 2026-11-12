/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, lib/libkern/libkern.h (string functions: kpi/linux/lib.c) */
#ifndef _OBSD_LIBKERN_H
#define _OBSD_LIBKERN_H

#include <sys/types.h>

void	*memcpy(void *, const void *, size_t);
void	*memmove(void *, const void *, size_t);
void	*memset(void *, int, size_t);
int	memcmp(const void *, const void *, size_t);
void	*memchr(const void *, int, size_t);
size_t	strlen(const char *);
size_t	strnlen(const char *, size_t);
char	*strcpy(char *, const char *);
char	*strncpy(char *, const char *, size_t);
char	*strcat(char *, const char *);
char	*strncat(char *, const char *, size_t);
size_t	strlcpy(char *, const char *, size_t);
size_t	strlcat(char *, const char *, size_t);
int	strcmp(const char *, const char *);
int	strncmp(const char *, const char *, size_t);
int	strcasecmp(const char *, const char *);
int	strncasecmp(const char *, const char *, size_t);
char	*strchr(const char *, int);
char	*strrchr(const char *, int);
char	*strstr(const char *, const char *);
char	*strsep(char **, const char *);
long	strtol(const char *, char **, int);
unsigned long	strtoul(const char *, char **, int);
long long	strtoll(const char *, char **, int);
unsigned long long	strtoull(const char *, char **, int);
#define bcopy(s, d, n)	((void)memmove(d, s, n))
#define bzero(p, n)	((void)memset(p, 0, n))
#define bcmp(a, b, n)	memcmp(a, b, n)

static inline int ffs(int x) { return __builtin_ffs(x); }
static inline int fls(int x) { return x ? 32 - __builtin_clz((unsigned)x) : 0; }
static inline int flsl(long x) { return x ? 64 - __builtin_clzl((unsigned long)x) : 0; }
static inline int ffsl(long x) { return __builtin_ffsl(x); }
static inline int imin(int a, int b) { return a < b ? a : b; }
static inline int imax(int a, int b) { return a > b ? a : b; }
static inline unsigned long ulmin(unsigned long a, unsigned long b) { return a < b ? a : b; }
static inline unsigned long ulmax(unsigned long a, unsigned long b) { return a > b ? a : b; }

#endif
