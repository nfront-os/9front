/*
 * C99's wide character I/O over APE's FILE, which APE lacks: fgetwc,
 * fputwc, ungetwc, fgetws, fputws, fwide and their kin; the wide printf
 * and scanf families through musl's engine (ape/lib/musl: ape/bridge.c);
 * wcscoll, wcsxfrm and wcsftime for the "C" locale (musl's ask the
 * locale). A wide character is its UTF-8 bytes in the stream.
 *
 * ungetwc gives a character's bytes back to APE's FILE (ungetc, into
 * the buffer they came from). Where its buffer has no room (unbuffered,
 * or the character came across a refill), they wait here instead,
 * for as long as the FILE is the same open stream (descriptor and
 * buffer). fwide keeps the orientation here; APE's byte functions do
 * not set it.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

extern int __apecompat_merrno;
extern int __apecompat_mvfwprintf(void *, const wchar_t *, va_list);
extern int __apecompat_mvfwscanf(void *, const wchar_t *, va_list);
extern int __apecompat_mvswprintf(wchar_t *, size_t, const wchar_t *, va_list);
extern int __apecompat_mvswscanf(const wchar_t *, const wchar_t *, va_list);

typedef struct Wstream Wstream;
struct Wstream {
	FILE	*f;
	int	fd;
	char	*buf;
	int	orient;
	int	n;	/* bytes given back, in b[0..n) */
	unsigned char	b[8];
};
static Wstream wstream[FOPEN_MAX];

static Wstream *
lookup(FILE *f, int make)
{
	Wstream *w, *free;

	free = NULL;
	for(w = wstream; w < wstream + FOPEN_MAX; w++){
		if(w->f == f){
			if(w->fd == f->fd && w->buf == f->buf)
				return w;
			memset(w, 0, sizeof *w);	/* closed and reopened */
		}
		if(w->f == NULL && free == NULL)
			free = w;
	}
	if(!make || free == NULL)
		return NULL;
	free->f = f;
	free->fd = f->fd;
	free->buf = f->buf;
	return free;
}

/* the next byte of a wide stream */
int
__apecompat_wgetbyte(void *v)
{
	FILE *f = v;
	Wstream *w;
	int c;

	w = lookup(f, 0);
	if(w != NULL && w->n > 0){
		c = w->b[0];
		memmove(w->b, w->b + 1, --w->n);
		return c;
	}
	return getc(f);
}

/* give back bytes read from a wide stream */
void
__apecompat_wungetbytes(void *v, const unsigned char *p, int n)
{
	FILE *f = v;
	Wstream *w;
	int i;

	w = lookup(f, 0);
	if(w == NULL || w->n == 0){
		for(i = n; i > 0; i--)
			if(ungetc(p[i-1], f) == EOF)
				break;
		if(i == 0)
			return;
		n = i;	/* the first n did not fit */
	}
	w = lookup(f, 1);
	if(w == NULL || w->n + n > (int)sizeof w->b)
		return;
	memmove(w->b + n, w->b, w->n);
	memcpy(w->b, p, n);
	w->n += n;
	if(w->orient == 0)
		w->orient = 1;
}

static void
seterrno(void)
{
	if(__apecompat_merrno == 0)
		return;
	switch(__apecompat_merrno){
	case 12:	errno = ENOMEM; break;
	case 34:	errno = ERANGE; break;
	case 75:	errno = EOVERFLOW; break;
	case 84:	errno = EILSEQ; break;
	default:	errno = EINVAL; break;
	}
	__apecompat_merrno = 0;
}

int
fwide(FILE *f, int mode)
{
	Wstream *w;

	w = lookup(f, mode != 0);
	if(w == NULL)
		return 0;
	if(w->orient == 0 && mode != 0)
		w->orient = mode > 0 ? 1 : -1;
	return w->orient;
}

wint_t
fgetwc(FILE *f)
{
	mbstate_t st;
	wchar_t wc;
	size_t k;
	char b;
	int c;

	memset(&st, 0, sizeof st);
	for(;;){
		c = __apecompat_wgetbyte(f);
		if(c == EOF){
			if(!mbsinit(&st))
				errno = EILSEQ;
			return WEOF;
		}
		b = c;
		k = mbrtowc(&wc, &b, 1, &st);
		if(k == (size_t)-1)
			return WEOF;	/* errno EILSEQ */
		if(k != (size_t)-2)
			return wc;
	}
}

wint_t
getwc(FILE *f)
{
	return fgetwc(f);
}

wint_t
getwchar(void)
{
	return fgetwc(stdin);
}

wint_t
ungetwc(wint_t c, FILE *f)
{
	mbstate_t st;
	char mb[4];
	size_t k;

	if(c == WEOF)
		return WEOF;
	memset(&st, 0, sizeof st);
	k = wcrtomb(mb, c, &st);
	if(k == (size_t)-1)
		return WEOF;
	__apecompat_wungetbytes(f, (unsigned char *)mb, k);
	return c;
}

wint_t
fputwc(wchar_t c, FILE *f)
{
	mbstate_t st;
	char mb[4];
	size_t k;

	memset(&st, 0, sizeof st);
	k = wcrtomb(mb, c, &st);
	if(k == (size_t)-1 || fwrite(mb, 1, k, f) != k)
		return WEOF;
	return c;
}

wint_t
putwc(wchar_t c, FILE *f)
{
	return fputwc(c, f);
}

wint_t
putwchar(wchar_t c)
{
	return fputwc(c, stdout);
}

wchar_t *
fgetws(wchar_t *s, int n, FILE *f)
{
	wint_t c;
	int i, e;

	if(n <= 0)
		return NULL;
	for(i = 0; i < n - 1; ){
		e = errno;
		errno = 0;
		c = fgetwc(f);
		if(c == WEOF){
			/* an encoding error or a read error, or the end */
			if(i == 0 || errno == EILSEQ || ferror(f))
				return NULL;
			errno = e;
			break;
		}
		errno = e;
		s[i++] = c;
		if(c == L'\n')
			break;
	}
	s[i] = 0;
	return s;
}

int
fputws(const wchar_t *s, FILE *f)
{
	for(; *s != 0; s++)
		if(fputwc(*s, f) == WEOF)
			return -1;
	return 0;
}

int
vfwprintf(FILE *f, const wchar_t *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvfwprintf(f, fmt, arg);
	seterrno();
	return n;
}

int
fwprintf(FILE *f, const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfwprintf(f, fmt, arg);
	va_end(arg);
	return n;
}

int
vwprintf(const wchar_t *fmt, va_list arg)
{
	return vfwprintf(stdout, fmt, arg);
}

int
wprintf(const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfwprintf(stdout, fmt, arg);
	va_end(arg);
	return n;
}

int
vswprintf(wchar_t *s, size_t size, const wchar_t *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvswprintf(s, size, fmt, arg);
	seterrno();
	return n;
}

int
swprintf(wchar_t *s, size_t size, const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vswprintf(s, size, fmt, arg);
	va_end(arg);
	return n;
}

int
vfwscanf(FILE *f, const wchar_t *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvfwscanf(f, fmt, arg);
	seterrno();
	return n;
}

int
fwscanf(FILE *f, const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfwscanf(f, fmt, arg);
	va_end(arg);
	return n;
}

int
vwscanf(const wchar_t *fmt, va_list arg)
{
	return vfwscanf(stdin, fmt, arg);
}

int
wscanf(const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vfwscanf(stdin, fmt, arg);
	va_end(arg);
	return n;
}

int
vswscanf(const wchar_t *s, const wchar_t *fmt, va_list arg)
{
	int n;

	n = __apecompat_mvswscanf(s, fmt, arg);
	seterrno();
	return n;
}

int
swscanf(const wchar_t *s, const wchar_t *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vswscanf(s, fmt, arg);
	va_end(arg);
	return n;
}

/* the "C" locale: code point order */
int
wcscoll(const wchar_t *a, const wchar_t *b)
{
	return wcscmp(a, b);
}

size_t
wcsxfrm(wchar_t *d, const wchar_t *s, size_t n)
{
	size_t l;

	l = wcslen(s);
	if(l < n)
		wmemcpy(d, s, l + 1);
	return l;
}

/* strftime on the format in UTF-8, the result back to wide */
size_t
wcsftime(wchar_t *s, size_t n, const wchar_t *fmt, const struct tm *tm)
{
	char *f, *b;
	size_t l, k;

	l = wcstombs(NULL, fmt, 0);
	if(l == (size_t)-1 || n == 0)
		return 0;
	f = malloc(l + 1);
	b = malloc(4 * n);
	if(f == NULL || b == NULL){
		free(f);
		free(b);
		return 0;
	}
	wcstombs(f, fmt, l + 1);
	k = strftime(b, 4 * n, f, tm);
	l = 0;
	if(k > 0 || *f == 0){
		l = mbstowcs(s, b, n);
		if(l == (size_t)-1 || l >= n)
			l = 0;
	}
	free(f);
	free(b);
	return l;
}
