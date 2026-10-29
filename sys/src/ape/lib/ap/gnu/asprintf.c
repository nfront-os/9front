/*
 * asprintf, vasprintf (POSIX.1-2024) over APE's vsnprintf. That returns
 * what vfprintf wrote into the buffer, not C99's length of the whole
 * output: grow the buffer until the output leaves room to spare.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int
vasprintf(char **sp, const char *fmt, va_list arg)
{
	va_list a;
	size_t size;
	int n;
	char *s, *t;

	*sp = NULL;
	s = NULL;
	for(size = 128;; size *= 2){
		t = realloc(s, size);
		if(t == NULL){
			free(s);
			return -1;
		}
		s = t;
		va_copy(a, arg);
		n = vsnprintf(s, size, fmt, a);
		va_end(a);
		if(n < 0){
			free(s);
			return -1;
		}
		if((size_t)n < size-1)
			break;
	}
	*sp = s;
	return n;
}

int
asprintf(char **sp, const char *fmt, ...)
{
	va_list arg;
	int n;

	va_start(arg, fmt);
	n = vasprintf(sp, fmt, arg);
	va_end(arg);
	return n;
}
