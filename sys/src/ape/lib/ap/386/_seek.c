/*
 * GCC port: 8c returns a long long through a hidden pointer, which the
 * kernel fills as seek's first argument; cdecl returns it in %edx:%eax.
 * The stub is __SEEK, wrapped as amd64/_seek.c does.
 */
extern long __SEEK(long long*, int, long long, int);

long long
_SEEK(int fd, long long o, int p)
{
	long long l;

	if(__SEEK(&l, fd, o, p) < 0)
		l = -1;
	return l;
}
