/*
 * APE's (GCC port): the bridge from musl's printf and scanf (vfprintf.c,
 * vfscanf.c, built in musl's own headers: scripts/gen/genmusl.py) to APE's
 * FILE. The engine's entry points are the __apecompat_m* below (and musl's
 * vsnprintf, vsscanf, strto[fd]l renamed so, -D); what else it defines
 * under APE's or a program's names is renamed __musl_* (genmusl.py).
 * ap/gnu/stdio.c, in APE's headers, wraps them as printf & co.
 *
 * A bridge FILE is musl's string FILE (as in vsnprintf.c) whose write
 * callback hands the bytes to APE's fwrite, and whose read callback
 * takes one byte at a time from APE's getc into a buffer of one byte,
 * as musl's __stdio_read leaves the last byte in the FILE's buffer
 * (scanf gives a byte back by stepping rpos back): nothing is read ahead
 * of what scanf consumes, and what scanf gives back goes back to APE's
 * FILE (ungetc). The FILEs live for one call in one thread: no
 * locking (lock -1; __lockfile is never called).
 */
#include "stdio_impl.h"
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

/* ap/gnu/stdio.c: APE's FILE */
extern size_t __apecompat_apewrite(void *, const void *, size_t);
extern int __apecompat_apegetc(void *);
extern int __apecompat_apeungetc(int, void *);

/* musl's errno, translated to APE's by the callers (ap/gnu/stdio.c) */
int __apecompat_merrno;

int *
___errno_location(void)
{
	return &__apecompat_merrno;
}

int
__lockfile(FILE *f)
{
	return 0;
}

void
__unlockfile(FILE *f)
{
}

void
__stdio_exit_needed(void)
{
}

static size_t
apewrite(FILE *f, const unsigned char *s, size_t l)
{
	size_t n = f->wpos - f->wbase;

	if (n && __apecompat_apewrite(f->cookie, f->wbase, n) != n)
		goto err;
	if (l && __apecompat_apewrite(f->cookie, s, l) != l)
		goto err;
	f->wpos = f->wbase = f->buf;
	f->wend = f->buf + f->buf_size;
	return l;
err:
	f->wpos = f->wbase = f->wend = 0;
	f->flags |= F_ERR;
	return 0;
}

int
__apecompat_mvfprintf(void *ape, const char *restrict fmt, va_list ap)
{
	unsigned char buf[256];
	FILE f = {
		.lbf = EOF,
		.write = apewrite,
		.lock = -1,
		.buf = buf,
		.buf_size = sizeof buf,
		.cookie = ape,
	};
	int n;

	n = vfprintf(&f, fmt, ap);
	if (f.wpos != f.wbase)
		f.write(&f, 0, 0);
	if (f.flags & F_ERR)
		return -1;
	return n;
}

/* one byte, as musl's __stdio_read with a buffer of one: into f->buf
 * (rpos after it) and to the caller */
static size_t
readone(FILE *f, unsigned char *s, size_t l, int c)
{
	if (l == 0)
		return 0;
	if (c < 0) {
		f->flags |= F_EOF;
		return 0;
	}
	f->buf[0] = c;
	f->rpos = f->rend = f->buf + 1;
	*s = c;
	return 1;
}

static size_t
aperead(FILE *f, unsigned char *s, size_t l)
{
	return readone(f, s, l, l ? __apecompat_apegetc(f->cookie) : 0);
}

int
__apecompat_mvfscanf(void *ape, const char *restrict fmt, va_list ap)
{
	/* UNGET bytes before buf for what scanf gives back */
	unsigned char buf[UNGET+1];
	FILE f = {
		.read = aperead,
		.lock = -1,
		.buf = buf + UNGET,
		.buf_size = 1,
		.cookie = ape,
	};
	int n;

	n = vfscanf(&f, fmt, ap);
	/* the byte scanf looked at and pushed back */
	if (f.rpos && f.rpos < f.rend)
		__apecompat_apeungetc(*f.rpos, ape);
	return n;
}

/*
 * Wide streams. musl's vfwprintf and vfwscanf take characters with
 * fputwc, getwc and ungetwc, and numbers with fprintf and fscanf on the
 * same FILE; musl's own wide functions ask the locale (thread pointer),
 * these are the engine's, for UTF-8 (compat's mbrtowc and wcrtomb, not
 * musl's): a wide character is its bytes in
 * the FILE, as in musl, and ungetwc puts them back before rpos (UNGET).
 * On APE's side the bytes come from compat/lib/wstdio.c, which keeps
 * what a wide stream got back (ungetwc) before APE's FILE.
 */
extern int __apecompat_wgetbyte(void *);
extern void __apecompat_wungetbytes(void *, const unsigned char *, int);

int
fwide(FILE *f, int mode)
{
	return 1;
}

wint_t
fputwc(wchar_t c, FILE *f)
{
	char mb[4];
	mbstate_t st = { 0 };
	size_t l;

	l = wcrtomb(mb, c, &st);
	if (l == (size_t)-1 || fwrite(mb, 1, l, f) != l) {
		f->flags |= F_ERR;
		return WEOF;
	}
	return c;
}

wint_t
getwc(FILE *f)
{
	mbstate_t st = { 0 };
	wchar_t wc;
	char b;
	size_t l;
	int c;

	for (;;) {
		c = getc_unlocked(f);
		if (c == EOF) {
			if (!mbsinit(&st))
				goto ilseq;
			return WEOF;
		}
		b = c;
		l = mbrtowc(&wc, &b, 1, &st);
		if (l == (size_t)-1)
			goto ilseq;
		if (l != (size_t)-2)
			return wc;
	}
ilseq:
	f->flags |= F_ERR;
	errno = EILSEQ;
	return WEOF;
}

wint_t
ungetwc(wint_t c, FILE *f)
{
	char mb[4];
	mbstate_t st = { 0 };
	size_t l;

	if (c == WEOF)
		return WEOF;
	if (!f->rpos)
		__toread(f);
	l = wcrtomb(mb, c, &st);
	if (l == (size_t)-1 || !f->rpos || f->rpos < f->buf - UNGET + l)
		return WEOF;
	memcpy(f->rpos -= l, mb, l);
	f->flags &= ~F_EOF;
	return c;
}

static size_t
wread(FILE *f, unsigned char *s, size_t l)
{
	return readone(f, s, l, l ? __apecompat_wgetbyte(f->cookie) : 0);
}

int
__apecompat_mvfwprintf(void *ape, const wchar_t *restrict fmt, va_list ap)
{
	unsigned char buf[256];
	FILE f = {
		.lbf = EOF,
		.write = apewrite,
		.lock = -1,
		.buf = buf,
		.buf_size = sizeof buf,
		.cookie = ape,
	};
	int n;

	n = vfwprintf(&f, fmt, ap);
	if (f.wpos != f.wbase)
		f.write(&f, 0, 0);
	if (f.flags & F_ERR)
		return -1;
	return n;
}

int
__apecompat_mvfwscanf(void *ape, const wchar_t *restrict fmt, va_list ap)
{
	/* room for ungetwc's bytes before buf */
	unsigned char buf[UNGET+1];
	FILE f = {
		.read = wread,
		.lock = -1,
		.buf = buf + UNGET,
		.buf_size = 1,
		.cookie = ape,
	};
	int n;

	n = vfwscanf(&f, fmt, ap);
	/* what scanf looked at and gave back, back to the stream */
	if (f.rpos && f.rpos < f.rend)
		__apecompat_wungetbytes(ape, f.rpos, f.rend - f.rpos);
	return n;
}
