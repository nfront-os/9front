/*
 * POSIX string functions APE lacks: stpcpy, stpncpy, strndup, which
 * APE's string.h declares but libap does not have, and memmem; and
 * <strings.h>'s strncasecmp with POSIX's size_t length (libbsd's takes an
 * int: SIZE_MAX would be -1, nothing compared), its own (libap is linked
 * after libbsd).
 */
#include <stdlib.h>
#include <string.h>

char*
stpcpy(char *d, const char *s)
{
	while((*d = *s++) != '\0')
		d++;
	return d;
}

char*
stpncpy(char *d, const char *s, size_t n)
{
	size_t l;

	l = strnlen(s, n);
	memcpy(d, s, l);
	memset(d+l, 0, n-l);
	return d+l;
}

char*
strndup(const char *s, size_t n)
{
	size_t l;
	char *d;

	l = strnlen(s, n);
	d = malloc(l+1);
	if(d == NULL)
		return NULL;
	memcpy(d, s, l);
	d[l] = '\0';
	return d;
}

void*
memmem(const void *h, size_t hn, const void *n, size_t nn)
{
	const unsigned char *p, *e;

	if(nn == 0)
		return (void*)h;
	if(hn < nn)
		return NULL;
	e = (const unsigned char*)h + (hn - nn);
	for(p = h; p <= e; p++)
		if(*p == *(const unsigned char*)n && memcmp(p, n, nn) == 0)
			return (void*)p;
	return NULL;
}

int
__apecompat_strncasecmp(const char *s1, const char *s2, size_t n)
{
	int c1, c2;

	for(; n > 0; n--){
		c1 = *(unsigned char *)s1++;
		c2 = *(unsigned char *)s2++;
		if(c1 >= 'A' && c1 <= 'Z')
			c1 += 'a' - 'A';
		if(c2 >= 'A' && c2 <= 'Z')
			c2 += 'a' - 'A';
		if(c1 != c2)
			return c1 - c2;
		if(c1 == 0)
			break;
	}
	return 0;
}
