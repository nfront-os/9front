#include "lib.h"
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include "sys9.h"

/*
 * BUG: advisory locking not implemented
 */

#define OFL (O_ACCMODE|O_NONBLOCK|O_APPEND)

int
fcntl(int fd, int cmd, ...)
{
	int arg, i, ans, err;
	Fdinfo *fi, *fans;
	va_list va;
	unsigned long oflags;

	err = 0;
	ans = 0;
	va_start(va, cmd);
	arg = va_arg(va, int);
	va_end(va);
	fi = &_fdinfo[fd];
	if(fd<0 || fd>=OPEN_MAX || !(fi->flags&FD_ISOPEN))
		err = EBADF;
	else switch(cmd){
		case F_DUPFD:
			if(fi->flags&(FD_BUFFERED|FD_BUFFEREDX)){
				err = EGREG;	/* dup of buffered fd not implemented */
				break;
			}
			oflags = fi->oflags;
			/*
			 * 9front-ng: a number free in the kernel too: dup(fd, i)
			 * replaces what i is, and descriptors APE did not open
			 * (a native program's: Rust's std in one with APE's C)
			 * are not in _fdinfo; Mesa's dup took a Rust program's
			 * /srv file. Without a minimum the kernel chooses.
			 */
			if(arg <= 0){
				i = _DUP(fd, -1);
				if(i >= OPEN_MAX){
					_CLOSE(i);
					i = OPEN_MAX;
				}
			}else{
				char buf[2];

				for(i = arg; i<OPEN_MAX; i++)
					if(!(_fdinfo[i].flags&FD_ISOPEN) && _FD2PATH(i, buf, sizeof buf) < 0)
						break;
			}
			if(i == OPEN_MAX)
				err = EMFILE;
			else if(i < 0){
				_syserrno();
				err = errno;
			}else {
				ans = arg <= 0 ? i : _DUP(fd, i);
				if(ans != i){
					if(ans < 0){
						_syserrno();
						err = errno;
					}else
						err = EBADF;
				}else{
					fans = &_fdinfo[ans];
					fans->flags = fi->flags&~FD_CLOEXEC;
					fans->oflags = oflags;
					fans->uid = fi->uid;
					fans->gid = fi->gid;
				}
			}
			break;
		case F_GETFD:
			ans = fi->flags&FD_CLOEXEC;
			break;
		case F_SETFD:
			fi->flags = (fi->flags&~FD_CLOEXEC)|(arg&FD_CLOEXEC);
			break;
		case F_GETFL:
			ans = fi->oflags&OFL;
			break;
		case F_SETFL:
			fi->oflags = (fi->oflags&~OFL)|(arg&OFL);
			break;
		case F_GETLK:
		case F_SETLK:
		case F_SETLKW:
			err = EINVAL;
			break;
		default:
			/* 9front-ng: an unknown command is an error (POSIX); it returned 0 (Mesa's F_DUPFD_CLOEXEC: stdin as the new descriptor) */
			err = EINVAL;
			break;
		}
	if(err){
		errno = err;
		ans = -1;
	}
	return ans;
}
