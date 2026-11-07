/*
 * 9front-ng: flock (sys/file.h): granted, Plan 9 has no advisory locks.  A
 * file of its own: a program whose library has a flock (Mesa's) keeps it.
 */
#include <errno.h>
#include <sys/file.h>

int
flock(int fd, int op)
{
	if(fd < 0){
		errno = EBADF;
		return -1;
	}
	if((op & ~LOCK_NB) != LOCK_SH && (op & ~LOCK_NB) != LOCK_EX && (op & ~LOCK_NB) != LOCK_UN){
		errno = EINVAL;
		return -1;
	}
	return 0;
}
