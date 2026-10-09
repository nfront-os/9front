#include <u.h>
#include <libc.h>

/*
 * GCC port: 8c returns a vlong through a hidden pointer, which the
 * kernel fills as seek's first argument; cdecl returns it in %edx:%eax.
 * The stub is _seek, wrapped as amd64/_seek.c does.
 */
extern int _seek(vlong*, int, vlong, int);

vlong
seek(int fd, vlong o, int p)
{
	vlong l;

	if(_seek(&l, fd, o, p) < 0)
		l = -1LL;
	return l;
}
