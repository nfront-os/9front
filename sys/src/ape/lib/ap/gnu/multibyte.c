/*
 * C99's multibyte and wide character conversions for gcc's wchar_t
 * (32 bits; APE's mbtowc & co. store 16), which APE lacks or has
 * only for its own: mbrtowc, wcrtomb, mbsrtowcs, ... and mbtowc, wctomb,
 * mblen, mbstowcs, wcstombs under their link names __apecompat_* (compat
 * <stdlib.h>). 9front's encoding is UTF-8, always; malformed input
 * (overlong forms, surrogates, beyond 0x10FFFF) is EILSEQ, found at the
 * first byte that makes it so.
 *
 * mbstate_t keeps a partial character: __opaque1 the bits so far,
 * __opaque2 the bytes still to come (low byte) and the bounds of the
 * next one (second byte: 0 any continuation byte, else the lead byte).
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static mbstate_t internal;

static size_t
ilseq(mbstate_t *ps)
{
	ps->__opaque1 = ps->__opaque2 = 0;
	errno = EILSEQ;
	return (size_t)-1;
}

/* is c a valid byte after the lead byte lead (0: a later one) */
static int
contok(int lead, int c)
{
	switch(lead){
	case 0xE0:	return c >= 0xA0 && c <= 0xBF;	/* not overlong */
	case 0xED:	return c >= 0x80 && c <= 0x9F;	/* no surrogates */
	case 0xF0:	return c >= 0x90 && c <= 0xBF;	/* not overlong */
	case 0xF4:	return c >= 0x80 && c <= 0x8F;	/* <= 0x10FFFF */
	default:	return c >= 0x80 && c <= 0xBF;
	}
}

size_t
mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	unsigned int wc, need, lead;
	size_t i;
	int c;

	if(ps == NULL)
		ps = &internal;
	if(s == NULL){
		if(ps->__opaque2 != 0)
			return ilseq(ps);
		return 0;
	}
	if(n == 0)
		return (size_t)-2;
	wc = ps->__opaque1;
	need = ps->__opaque2 & 0xFF;
	lead = ps->__opaque2 >> 8;
	i = 0;
	if(need == 0){
		c = s[i++] & 0xFF;
		if(c < 0x80){
			if(pwc)
				*pwc = c;
			return c != 0;
		}
		if(c < 0xC2 || c > 0xF4)
			return ilseq(ps);
		if(c < 0xE0){
			wc = c & 0x1F;
			need = 1;
		}else if(c < 0xF0){
			wc = c & 0x0F;
			need = 2;
		}else{
			wc = c & 0x07;
			need = 3;
		}
		lead = c;
	}
	for(; need > 0; need--){
		if(i == n){
			ps->__opaque1 = wc;
			ps->__opaque2 = need | lead << 8;
			return (size_t)-2;
		}
		c = s[i++] & 0xFF;
		if(!contok(lead, c))
			return ilseq(ps);
		lead = 0;
		wc = wc << 6 | (c & 0x3F);
	}
	ps->__opaque1 = ps->__opaque2 = 0;
	if(pwc)
		*pwc = wc;
	return wc != 0 ? i : 0;
}

size_t
mbrlen(const char *s, size_t n, mbstate_t *ps)
{
	static mbstate_t st;

	return mbrtowc(NULL, s, n, ps != NULL ? ps : &st);
}

int
mbsinit(const mbstate_t *ps)
{
	return ps == NULL || ps->__opaque2 == 0;
}

size_t
wcrtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	unsigned int c;

	if(ps == NULL)
		ps = &internal;
	if(s == NULL)
		return 1;
	c = wc;
	if(c < 0x80){
		s[0] = c;
		return 1;
	}
	if(c < 0x800){
		s[0] = 0xC0 | c >> 6;
		s[1] = 0x80 | (c & 0x3F);
		return 2;
	}
	if(c >= 0xD800 && c <= 0xDFFF)
		return ilseq(ps);
	if(c < 0x10000){
		s[0] = 0xE0 | c >> 12;
		s[1] = 0x80 | (c >> 6 & 0x3F);
		s[2] = 0x80 | (c & 0x3F);
		return 3;
	}
	if(c <= 0x10FFFF){
		s[0] = 0xF0 | c >> 18;
		s[1] = 0x80 | (c >> 12 & 0x3F);
		s[2] = 0x80 | (c >> 6 & 0x3F);
		s[3] = 0x80 | (c & 0x3F);
		return 4;
	}
	return ilseq(ps);
}

wint_t
btowc(int c)
{
	return c >= 0 && c < 0x80 ? (wint_t)c : WEOF;
}

int
wctob(wint_t c)
{
	return c < 0x80 ? (int)c : EOF;
}

/* POSIX: at most n bytes of *src */
size_t
mbsnrtowcs(wchar_t *dst, const char **src, size_t n, size_t len, mbstate_t *ps)
{
	const char *s;
	size_t k, w;
	wchar_t wc;

	if(ps == NULL)
		ps = &internal;
	s = *src;
	for(w = 0; dst == NULL || w < len; w++){
		k = mbrtowc(&wc, s, n, ps);
		if(k == (size_t)-1){
			if(dst != NULL)
				*src = s;
			return k;
		}
		if(k == (size_t)-2){	/* the n bytes end in a character */
			s += n;
			break;
		}
		if(dst != NULL)
			dst[w] = wc;
		if(k == 0){
			if(dst != NULL)
				*src = NULL;
			return w;
		}
		s += k;
		n -= k;
	}
	if(dst != NULL)
		*src = s;
	return w;
}

size_t
mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps)
{
	return mbsnrtowcs(dst, src, (size_t)-1, len, ps);
}

/* POSIX: at most n wide characters of *src */
size_t
wcsnrtombs(char *dst, const wchar_t **src, size_t n, size_t len, mbstate_t *ps)
{
	const wchar_t *s;
	char buf[4];
	size_t k, b;

	if(ps == NULL)
		ps = &internal;
	s = *src;
	for(b = 0; n > 0; s++, n--){
		k = wcrtomb(buf, *s, ps);
		if(k == (size_t)-1){
			if(dst != NULL)
				*src = s;
			return k;
		}
		if(dst != NULL){
			if(b + k > len)
				break;
			memcpy(dst + b, buf, k);
		}
		if(*s == 0){
			if(dst != NULL)
				*src = NULL;
			return b;
		}
		b += k;
	}
	if(dst != NULL)
		*src = s;
	return b;
}

size_t
wcsrtombs(char *dst, const wchar_t **src, size_t len, mbstate_t *ps)
{
	return wcsnrtombs(dst, src, (size_t)-1, len, ps);
}

/* C89's, stateless (UTF-8 has no shift states) */
int
mbtowc(wchar_t *pwc, const char *s, size_t n)
{
	mbstate_t st;
	size_t k;

	if(s == NULL)
		return 0;
	memset(&st, 0, sizeof st);
	k = mbrtowc(pwc, s, n, &st);
	if(k == (size_t)-1 || k == (size_t)-2){
		errno = EILSEQ;
		return -1;
	}
	return k;
}

int
mblen(const char *s, size_t n)
{
	return mbtowc(NULL, s, n);
}

int
wctomb(char *s, wchar_t wc)
{
	mbstate_t st;

	if(s == NULL)
		return 0;
	memset(&st, 0, sizeof st);
	return wcrtomb(s, wc, &st);
}

size_t
mbstowcs(wchar_t *dst, const char *src, size_t len)
{
	mbstate_t st;

	memset(&st, 0, sizeof st);
	return mbsrtowcs(dst, &src, len, &st);
}

size_t
wcstombs(char *dst, const wchar_t *src, size_t len)
{
	mbstate_t st;

	memset(&st, 0, sizeof st);
	return wcsrtombs(dst, &src, len, &st);
}
