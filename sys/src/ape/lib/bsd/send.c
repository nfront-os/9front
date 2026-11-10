/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>

#include "priv.h"

extern int __nosigpipe_begin(void);	/* GCC port: MSG_NOSIGNAL (ap/gnu/nosigpipe.c) */
extern long __nosigpipe_end(int, long);

int
send(int fd, void *a, int n, int flags)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	if(flags & MSG_OOB){
		errno = EOPNOTSUPP;
		return -1;
	}
	if(flags & MSG_NOSIGNAL){	/* GCC port: no SIGPIPE, EPIPE */
		int slot;

		slot = __nosigpipe_begin();
		return __nosigpipe_end(slot, write(fd, a, n));
	}
	return write(fd, a, n);
}

int
recv(int fd, void *a, int n, int flags)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	if(flags & MSG_OOB){
		errno = EOPNOTSUPP;
		return -1;
	}
	return read(fd, a, n);
}
