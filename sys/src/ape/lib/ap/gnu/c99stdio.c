/*
 * C99's printf and scanf families, strtof and strtold, over musl's engine
 * (ape/lib/musl: ape/bridge.c): APE's printf reads every
 * floating argument as a double (%Lf), and both lack %a, %F, hh, j, t,
 * z and C99's snprintf result. <stdio.h> and <stdlib.h>
 * give these the C names with asm labels (__apecompat_*): libap keeps its
 * own, and APE's FILE is shared, so output and input interleave with
 * puts, getc & co. as one stream.
 *
 * The engine sets musl's errno numbers in a variable of its own; they
 * become APE's here.
 */
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern int __apecompat_merrno;
extern int __apecompat_mvfprintf(void *, const char *, va_list);
extern int __apecompat_mvfscanf(void *, const char *, va_list);
extern int __apecompat_mvsnprintf(char *, size_t, const char *, va_list);
extern int __apecompat_mvsscanf(const char *, const char *, va_list);
extern float __apecompat_mstrtof(const char *, char **);
extern long double __apecompat_mstrtold(const char *, char **);

/* APE's FILE for the engine's FILEs */
size_t
__apecompat_apewrite(void *f, const void *p, size_t n)
{
	return fwrite(p, 1, n, f);
}

int
__apecompat_apegetc(void *f)
{
	return getc((FILE *)f);
}

int
__apecompat_apeungetc(int c, void *f)
{
	return ungetc(c, f);
}

/* musl's errno numbers (arch/generic/bits/errno.h) as APE's */
static void
seterrno(void)
{
	int e;

	e = __apecompat_merrno;
	if(e == 0)
		return;
	__apecompat_merrno = 0;
	switch(e){
	case 12:	errno = ENOMEM; break;
	case 22:	errno = EINVAL; break;
	case 34:	errno = ERANGE; break;
	case 75:	errno = EOVERFLOW; break;
	case 84:	errno = EILSEQ; break;
	default:	errno = EINVAL; break;
	}
}

int
vfprintf(FILE *f, const char *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvfprintf(f, fmt, arg);
	seterrno();
	return n;
}

int
fprintf(FILE *f, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfprintf(f, fmt, arg);
	va_end(arg);
	return n;
}

int
vprintf(const char *fmt, va_list arg)
{
	return vfprintf(stdout, fmt, arg);
}

int
printf(const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfprintf(stdout, fmt, arg);
	va_end(arg);
	return n;
}

int
vsnprintf(char *s, size_t size, const char *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvsnprintf(s, size, fmt, arg);
	seterrno();
	return n;
}

int
snprintf(char *s, size_t size, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vsnprintf(s, size, fmt, arg);
	va_end(arg);
	return n;
}

int
vsprintf(char *s, const char *fmt, va_list arg)
{
	return vsnprintf(s, INT_MAX, fmt, arg);
}

int
sprintf(char *s, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vsnprintf(s, INT_MAX, fmt, arg);
	va_end(arg);
	return n;
}

int
vfscanf(FILE *f, const char *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvfscanf(f, fmt, arg);
	seterrno();
	return n;
}

int
fscanf(FILE *f, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfscanf(f, fmt, arg);
	va_end(arg);
	return n;
}

int
vscanf(const char *fmt, va_list arg)
{
	return vfscanf(stdin, fmt, arg);
}

int
scanf(const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfscanf(stdin, fmt, arg);
	va_end(arg);
	return n;
}

int
vsscanf(const char *s, const char *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvsscanf(s, fmt, arg);
	seterrno();
	return n;
}

int
sscanf(const char *s, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vsscanf(s, fmt, arg);
	va_end(arg);
	return n;
}

float
strtof(const char *s, char **end)
{
	float v;

	v = __apecompat_mstrtof(s, end);
	seterrno();
	return v;
}

long double
strtold(const char *s, char **end)
{
	long double v;

	v = __apecompat_mstrtold(s, end);
	seterrno();
	return v;
}
