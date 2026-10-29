/*
 * pread and pwrite (POSIX), which APE lacks: libap's system call stubs,
 * checked and with errno as APE's read and write do it (plan9/read.c).
 */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

/* libap (plan9/sys9.h, plan9/lib.h) */
extern long _PREAD(int, void *, long, long long);
extern long _PWRITE(int, void *, long, long long);
extern void _syserrno(void);

ssize_t
pread(int fd, void *buf, size_t n, off_t off)
{
	long r;

	if(fcntl(fd, F_GETFD) < 0)	/* EBADF, as APE's read */
		return -1;
	r = _PREAD(fd, buf, n, off);
	if(r < 0)
		_syserrno();
	return r;
}

ssize_t
pwrite(int fd, const void *buf, size_t n, off_t off)
{
	long r;

	if(fcntl(fd, F_GETFD) < 0)
		return -1;
	r = _PWRITE(fd, (void *)buf, n, off);
	if(r < 0)
		_syserrno();
	return r;
}
