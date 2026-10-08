#include <u.h>
#include <libc.h>

Rune*
runevseprint(Rune *buf, Rune *e, char *fmt, va_list args)
{
	Fmt f;

	if(e <= buf)
		return nil;
	f.runes = 1;
	f.start = buf;
	f.to = buf;
	f.stop = e - 1;
	f.flush = nil;
	f.farg = nil;
	f.nfmt = 0;
	va_copy(f.args, args);	/* GCC port: va_list is an array type, can't '=' */
	dofmt(&f, fmt);
	*(Rune*)f.to = '\0';
	return f.to;
}

