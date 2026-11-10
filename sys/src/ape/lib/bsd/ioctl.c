/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

/*
 * GCC port: a pty's (ptyfs, sys/src/ape/cmd/ptyfs) requests, Linux's
 * numbers (sys/ioctl.h under _APE_GNU): its size, and TIOCSCTTY: the
 * slave becomes the terminal of the caller (after setsid, its own
 * namespace and note group): bound on /dev/cons, its pconsctl on
 * /dev/consctl, the note group told to ptyfs (interrupt, hangup) in
 * the pty's /dev/ptyctlN.
 */
#include "lib.h"
#include "sys9.h"
#include "dir.h"

#define	PTY_TIOCSCTTY	0x540E
#define	PTY_TIOCGWINSZ	0x5413
#define	PTY_TIOCSWINSZ	0x5414
#define	PTY_TIOCNOTTY	0x5422
#define	PTY_NCCS	11	/* termios.h */

struct ptywinsize {
	unsigned short	ws_row;
	unsigned short	ws_col;
	unsigned short	ws_xpixel;
	unsigned short	ws_ypixel;
};

extern int _ptyctl(int, int);	/* ap/plan9/tcgetattr.c: the pty's /dev/ptyctlN */

static int
ptyctl(int fd, char *msg)
{
	int c, n;

	if((c = _ptyctl(fd, 1)) < 0){
		errno = ENOTTY;
		return -1;
	}
	n = strlen(msg);
	if(_WRITE(c, msg, n) != n){
		_syserrno();
		_CLOSE(c);
		return -1;
	}
	_CLOSE(c);
	return 0;
}

extern int _ptynum(int, int*);	/* ap/plan9/tcgetattr.c: by the qid */

/* the pty's number, if fd is its slave */
static int
slaven(int fd)
{
	int n, side;

	if((n = _ptynum(fd, &side)) < 0 || side != 3)
		return -1;
	return n;
}

static int
setctty(int fd)
{
	char cons[32], ctl[32], buf[32];
	int n;

	if((n = slaven(fd)) < 0){
		errno = ENOTTY;
		return -1;
	}
	snprintf(cons, sizeof cons, "/dev/ptty%d", n);
	snprintf(ctl, sizeof ctl, "/dev/pconsctl%d", n);
	if(_BIND(cons, "/dev/cons", MREPL) < 0 || _BIND(ctl, "/dev/consctl", MREPL) < 0){
		_syserrno();
		return -1;
	}
	snprintf(buf, sizeof buf, "IOW note %d", getpid());
	return ptyctl(fd, buf);
}

static int
winsize(int fd, struct ptywinsize *w, int set)
{
	char buf[128], *s;
	long n;
	int i, c;

	if(set){
		snprintf(buf, sizeof buf, "IOW winsize %d %d %d %d", w->ws_row, w->ws_col, w->ws_xpixel, w->ws_ypixel);
		return ptyctl(fd, buf);
	}
	if((c = _ptyctl(fd, 0)) < 0){
		errno = ENOTTY;
		return -1;
	}
	n = _READ(c, buf, sizeof buf - 1);
	_CLOSE(c);
	if(n < 0){
		_syserrno();
		return -1;
	}
	buf[n] = 0;
	/* IOR flags cc... pgrp rows cols xpixels ypixels */
	s = buf;
	for(i = 0; i < 5 + PTY_NCCS + 1 && s != nil; i++)
		if((s = strchr(s, ' ')) != nil)
			s++;
	if(s == nil){
		errno = ENOTTY;
		return -1;
	}
	w->ws_row = strtoul(s, &s, 10);
	w->ws_col = strtoul(s, &s, 10);
	w->ws_xpixel = strtoul(s, &s, 10);
	w->ws_ypixel = strtoul(s, &s, 10);
	return 0;
}

int
ioctl(int fd, unsigned long request, void* arg)
{
	struct stat d;

	if(request == FIONREAD) {
		if(fstat(fd, &d) < 0) {
			errno = EBADF;
			return -1;
		}
		/* this works if the file is buffered somehow */
		*(long*)arg = d.st_size;
		return 0;
	} else if(request == PTY_TIOCSCTTY) {	/* GCC port */
		return setctty(fd);
	} else if(request == PTY_TIOCGWINSZ || request == PTY_TIOCSWINSZ) {	/* GCC port */
		return winsize(fd, arg, request == PTY_TIOCSWINSZ);
	} else if(request == PTY_TIOCNOTTY) {	/* GCC port */
		return 0;
	} else {
		errno = EINVAL;
		return -1;
	}
}
