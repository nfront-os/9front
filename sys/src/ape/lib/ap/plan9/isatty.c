#include "lib.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys9.h"
#include "dir.h"

int
_isatty(int fd)
{
	char buf[64];

	if(_FD2PATH(fd, buf, sizeof buf) < 0)
		return 0;

	/* might be /mnt/term/dev/cons */
	if(strlen(buf) >= 9 && strcmp(buf+strlen(buf)-9, "/dev/cons") == 0)
		return 1;
	/* GCC port: a pty's slave (ptyfs: /dev/pttyN) */
	return strstr(buf, "/dev/ptty") != 0;
}

/* The FD_ISTTY flag is set via _isatty in _fdsetup or open */
int
isatty(int fd)
{
	if(_fdinfo[fd].flags&FD_ISTTY)
		return 1;
	/* GCC port: open does not set FD_ISTTY (only _fdsetup): one opened later (a pty's slave) */
	if(fd >= 0 && fd < OPEN_MAX && (_fdinfo[fd].flags&FD_ISOPEN) && _isatty(fd)){
		_fdinfo[fd].flags |= FD_ISTTY;
		return 1;
	}
	return 0;
}
