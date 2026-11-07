#include "lib.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "dir.h"

extern int __shmtruncate(int, off_t);

int
ftruncate(int fd, off_t length)
{
	Dir d;
	int r;

	if(length < 0){
		errno = EINVAL;
		return -1;
	}
	/* 9front-ng: a shared memory object (gnu/mman.c): a segment, sized once */
	if((r = __shmtruncate(fd, length)) != -2)
		return r;
	_nulldir(&d);
	d.length = length;
	if(_dirfwstat(fd, &d) < 0){
		_syserrno();
		return -1;
	}
	return 0;
}
