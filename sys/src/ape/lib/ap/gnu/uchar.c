/*
 * C11's UTF-32 conversions (uchar.h): 9front's multibyte encoding is
 * UTF-8 and gcc's wchar_t 32 bits, so they are mbrtowc's and wcrtomb's
 * (ap/gnu/multibyte.c).
 */
#include <uchar.h>
#include <wchar.h>

static mbstate_t internal32;

size_t
mbrtoc32(char32_t *pc, const char *s, size_t n, mbstate_t *ps)
{
	wchar_t w;
	size_t r;

	r = mbrtowc(&w, s, n, ps != NULL ? ps : &internal32);
	if(pc != NULL && s != NULL && r <= n)
		*pc = (char32_t)w;
	return r;
}

size_t
c32rtomb(char *s, char32_t c, mbstate_t *ps)
{
	return wcrtomb(s, (wchar_t)c, ps != NULL ? ps : &internal32);
}
