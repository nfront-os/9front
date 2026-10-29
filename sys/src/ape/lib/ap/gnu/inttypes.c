/*
 * C99's strtoumax, imaxabs and imaxdiv, which APE lacks (it has
 * strtoimax).
 */
#include <inttypes.h>
#include <stdlib.h>

uintmax_t
strtoumax(const char *s, char **end, int base)
{
	return strtoull(s, end, base);
}

intmax_t
imaxabs(intmax_t v)
{
	return v < 0 ? -v : v;
}

imaxdiv_t
imaxdiv(intmax_t n, intmax_t d)
{
	imaxdiv_t r;

	r.quot = n / d;
	r.rem = n % d;
	return r;
}
