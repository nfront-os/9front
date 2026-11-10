#include <errno.h>
#include <unistd.h>
#include "lib.h"
#include "sys9.h"

ssize_t
write(int d, const void *buf, size_t nbytes)
{
	int n;

	if(d<0 || d>=OPEN_MAX || !(_fdinfo[d].flags&FD_ISOPEN)){
		errno = EBADF;
		return -1;
	}
	if(_fdinfo[d].oflags&O_APPEND)
		_SEEK(d, 0, 2);
	if(_fdinfo[d].flags&FD_EVENTFD){	/* GCC port: Linux's eventfd: what is written is read here (gnu/epoll.c) */
		extern int __eventfdpeer(int);
		int p;

		if((p = __eventfdpeer(d)) >= 0)
			d = p;
	}
	n = _WRITE(d, buf, nbytes);
	if(n < 0)
		_syserrno();
	return n;
}

/* GCC port: fd is an eventfd (gnu/epoll.c) */
void
__eventfdmark(int fd)
{
	if(fd >= 0 && fd < OPEN_MAX)
		_fdinfo[fd].flags |= FD_EVENTFD;
}
