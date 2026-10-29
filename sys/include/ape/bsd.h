#ifndef _BSD_EXTENSION
    This header file is an extension to ANSI/POSIX
#endif

#ifndef __BSD_H_
#define __BSD_H_
#pragma src "/sys/src/ape/lib/bsd"
#pragma lib "/$M/lib/ape/libbsd.a"

#ifndef __TYPES_H
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

/* ifndefs because X11 stuff (ugh) */
#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: as gcc knows them (its built-ins) and <strings.h> declares them;
 * libbsd's functions, strncasecmp through ap/gnu/string.c (libbsd's takes an
 * int: SIZE_MAX would be -1) */
#ifndef bcopy
extern void	bcopy(const void*, void*, size_t);
#endif
#ifndef bcmp
extern int	bcmp(const void*, const void*, size_t);
#endif
#ifndef bzero
extern void	bzero(void*, size_t);
#endif
extern int	ffs(int);
extern int	strcasecmp(const char*, const char*);
extern int	strncasecmp(const char*, const char*, size_t) __asm__("__apecompat_strncasecmp");
#else
#ifndef bcopy
extern void	bcopy(void*, void*, size_t);
#endif
#ifndef bcmp
extern int	bcmp(void*, void*, size_t);
#endif
#ifndef bzero
extern void	bzero(void*, size_t);
#endif
extern int	ffs(unsigned int);
extern int	strcasecmp(char*, char*);
extern int	strncasecmp(char*, char*,int);
#endif
extern void	bhappy(void*);
extern int	rresvport(int*);
extern int	rcmd(char**, int, char*, char*, char*, int*);

extern int	getopt(int, char**, char*);
extern int	opterr;
extern int	optind;
extern int	optopt;
extern char	*optarg;
extern char	*mktemp(char *);
extern char	*sys_errlist[];
extern int		sys_nerr;

extern int	gethostname(char*, int);

#ifdef __cplusplus
}
#endif

#endif
