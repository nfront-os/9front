#ifndef	_STDIO_H_
#define	_STDIO_H_

#if defined(__GNUC__) || defined(_APE_GNU)
/*
 * GCC port, for the gcc driver's programs: C99's printf and scanf families over APE's FILE
 * (libap, ap/gnu/c99stdio.c: musl's engine): APE's read every
 * floating argument as a double (%Lf) and lack %a, %F, hh, j, t, z and
 * C99's snprintf result. APE's declarations are renamed while its header
 * is read; ours carry the link names __apecompat_*, so libap keeps its
 * own functions and both use the same FILEs.
 *
 * POSIX.1-2024's asprintf and vasprintf, which APE lacks; popen and
 * pclose, which APE declares only with _BSD_EXTENSION (libbsd; the driver
 * links both).
 */
#define fprintf		__ape_fprintf
#define fscanf		__ape_fscanf
#define printf		__ape_printf
#define scanf		__ape_scanf
#define sprintf		__ape_sprintf
#define snprintf	__ape_snprintf
#define vsnprintf	__ape_vsnprintf
#define sscanf		__ape_sscanf
#define vfprintf	__ape_vfprintf
#define vprintf		__ape_vprintf
#define vsprintf	__ape_vsprintf
#define vfscanf		__ape_vfscanf
#endif
#pragma lib "/$M/lib/ape/libap.a"

/*
 * pANS stdio.h
 */
#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>
/*
 * According to X3J11, there is only one i/o buffer
 * and it must not be occupied by both input and output data.
 *	If rp<wp, we must have state==RD and
 *	if wp<rp, we must have state==WR, so that getc and putc work correctly.
 *	On open, rp, wp and buf are set to 0, so first getc or putc will call _IO_getc
 *	or _IO_putc, which will allocate the buffer.
 *	If setvbuf(., ., _IONBF, .) is called, bufl is set to 0 and
 *	buf, rp and wp are pointed at unbuf.
 *	If setvbuf(., ., _IOLBF, .) is called, _IO_putc leaves wp and rp pointed at the
 *	end of the buffer so that it can be called on each putc to check whether it's got
 *	a newline.  This nonsense is in order to avoid impacting performance of the other
 *	buffering modes more than necessary -- putting the test in putc adds many
 *	instructions that are wasted in non-_IOLBF mode:
 *	#define putc(c, f)	(_IO_ctmp=(c),\
 *				(f)->wp>=(f)->rp || (f)->flags&LINEBUF && _IO_ctmp=='\n'\
 *					?_IO_putc(_IO_ctmp, f)\
 *					:*(f)->wp++=_IO_ctmp)
 *				
 */
typedef struct{
	int fd;		/* UNIX file pointer */
	char flags;	/* bits for must free buffer on close, line-buffered */
	char state;	/* last operation was read, write, position, error, eof */
	char *buf;	/* pointer to i/o buffer */
	char *rp;	/* read pointer (or write end-of-buffer) */
	char *wp;	/* write pointer (or read end-of-buffer) */
	char *lp;	/* actual write pointer used when line-buffering */
	size_t bufl;	/* actual length of buffer */
	char unbuf[1];	/* tiny buffer for unbuffered io (used for ungetc?) */
}FILE;
typedef long long fpos_t;
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif
/*
 * Third arg of setvbuf
 */
#define	_IOFBF	1			/* block-buffered */
#define	_IOLBF	2			/* line-buffered */
#define	_IONBF	3			/* unbuffered */
#define	BUFSIZ	4096			/* size of setbuf buffer */
#define	EOF	(-1)			/* returned on end of file */
#define	FOPEN_MAX	90		/* max files open */
#define	FILENAME_MAX	BUFSIZ		/* silly filename length */
#define	L_tmpnam	20		/* sizeof "/tmp/abcdefghij9999 */
#define	L_cuserid	32		/* maximum size user name */
#define	L_ctermid	32		/* size of name of controlling tty */
#define	SEEK_CUR	1
#define	SEEK_END	2
#define	SEEK_SET	0
#define	TMP_MAX		64		/* very hard to set correctly */
#define	stderr	(&_IO_stream[2])
#define	stdin	(&_IO_stream[0])
#define	stdout	(&_IO_stream[1])
#define	_IO_CHMASK	0377		/* mask for 8 bit characters */

#ifdef __cplusplus
extern "C" {
#endif

extern int remove(const char *);
extern int rename(const char *, const char *);
extern FILE *tmpfile(void);
extern char *tmpnam(char *);
extern int fclose(FILE *);
extern int fflush(FILE *);
extern FILE *fopen(const char *, const char *);
extern FILE *freopen(const char *, const char *, FILE *);
extern void setbuf(FILE *, char *);
extern int setvbuf(FILE *, char *, int, size_t);
extern int fprintf(FILE *, const char *, ...);
extern int fscanf(FILE *, const char *, ...);
extern int printf(const char *, ...);
extern int scanf(const char *, ...);
extern int sprintf(char *, const char *, ...);
extern int snprintf(char *, size_t, const char *, ...);
extern int vsnprintf(char *, size_t, const char *, va_list);
extern int sscanf(const char *, const char *, ...);
extern int vfprintf(FILE *, const char *, va_list);
extern int vprintf(const char *, va_list);
extern int vsprintf(char *, const char *, va_list);
extern int vfscanf(FILE *, const char *, va_list);
extern int fgetc(FILE *);
extern char *fgets(char *, int, FILE *);
extern int fputc(int, FILE *);
extern int fputs(const char *, FILE *);
extern int getc(FILE *);
#define	getc(f)	((f)->rp>=(f)->wp?_IO_getc(f):*(f)->rp++&_IO_CHMASK)
extern int _IO_getc(FILE *f);
extern int getchar(void);
#define	getchar()	getc(stdin)
extern char *gets(char *);
extern int putc(int, FILE *);
#define	putc(c, f) ((f)->wp>=(f)->rp?_IO_putc(c, f):(int)(*(unsigned char*)(f)->wp++=(c)&_IO_CHMASK))
extern int _IO_putc(int, FILE *);
extern int putchar(int);
#define	putchar(c)	putc(c, stdout)
extern int puts(const char *);
extern int ungetc(int, FILE *);
extern size_t fread(void *, size_t, size_t, FILE *);
extern size_t fwrite(const void *, size_t, size_t, FILE *);
extern int fgetpos(FILE *, fpos_t *);
extern int fseek(FILE *, long, int);
extern int fseeko(FILE *, off_t, int);
extern int fsetpos(FILE *, const fpos_t *);
extern long ftell(FILE *);
extern off_t ftello(FILE *);
extern void rewind(FILE *);
extern void clearerr(FILE *);
extern int feof(FILE *);
extern int ferror(FILE *);
extern void perror(const char *);
extern FILE _IO_stream[FOPEN_MAX];

#ifdef _POSIX_SOURCE
extern int fileno(FILE *);
extern FILE* fdopen(int, const char*);
extern FILE *open_memstream(char **, size_t *);	/* 9front-ng: POSIX.1-2008 */
#ifndef _SSIZE_T
#define _SSIZE_T
typedef long ssize_t;
#endif
extern ssize_t getdelim(char **, size_t *, int, FILE *);	/* 9front-ng: POSIX.1-2008 */
extern ssize_t getline(char **, size_t *, FILE *);
extern char *ctermid(char *);
#endif

#ifdef _REENTRANT_SOURCE
extern char *tmpnam_r(char *);
extern char *ctermid_r(char *);
#endif

#ifdef _BSD_EXTENSION
#pragma lib "/$M/lib/ape/libbsd.a"
extern FILE *popen(char *, char *);
extern int	pclose(FILE *);
#endif

#ifdef __cplusplus
}
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
#undef fprintf
#undef fscanf
#undef printf
#undef scanf
#undef sprintf
#undef snprintf
#undef vsnprintf
#undef sscanf
#undef vfprintf
#undef vprintf
#undef vsprintf
#undef vfscanf

#ifdef __cplusplus
extern "C" {
#endif
extern int	fprintf(FILE *, const char *, ...)
		__asm__("__apecompat_fprintf");
extern int	fscanf(FILE *, const char *, ...)
		__asm__("__apecompat_fscanf");
extern int	printf(const char *, ...)
		__asm__("__apecompat_printf");
extern int	scanf(const char *, ...)
		__asm__("__apecompat_scanf");
extern int	sprintf(char *, const char *, ...)
		__asm__("__apecompat_sprintf");
extern int	snprintf(char *, size_t, const char *, ...)
		__asm__("__apecompat_snprintf");
extern int	sscanf(const char *, const char *, ...)
		__asm__("__apecompat_sscanf");
extern int	vfprintf(FILE *, const char *, va_list)
		__asm__("__apecompat_vfprintf");
extern int	vfscanf(FILE *, const char *, va_list)
		__asm__("__apecompat_vfscanf");
extern int	vprintf(const char *, va_list)
		__asm__("__apecompat_vprintf");
extern int	vscanf(const char *, va_list)
		__asm__("__apecompat_vscanf");
extern int	vsnprintf(char *, size_t, const char *, va_list)
		__asm__("__apecompat_vsnprintf");
extern int	vsprintf(char *, const char *, va_list)
		__asm__("__apecompat_vsprintf");
extern int	vsscanf(const char *, const char *, va_list)
		__asm__("__apecompat_vsscanf");

extern int	asprintf(char **, const char *, ...)
		__attribute__((__format__(__printf__, 2, 3)));
extern int	vasprintf(char **, const char *, va_list)
		__attribute__((__format__(__printf__, 2, 0)));
#ifndef _BSD_EXTENSION
extern FILE	*popen(const char *, const char *);
extern int	pclose(FILE *);
#endif
#ifdef __cplusplus
}
#endif
#endif
#endif
