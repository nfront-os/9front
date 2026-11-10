#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "sys9.h"
#include "lib.h"
#include "dir.h"

#define	CESC	'\\'
#define	CINTR	0177	/* DEL */
#define	CQUIT	034	/* FS, cntl | */
#define	CERASE	010	/* BS */
#define	CKILL	025 /* cntl u */
#define	CEOF	04	/* cntl d */
#define	CSTART	021	/* cntl q */
#define	CSTOP	023	/* cntl s */
#define	CSWTCH	032	/* cntl z */
#define CEOL	000
#define	CNSWTCH	0

/*
 * GCC port: a pty of ptyfs (sys/src/ape/cmd/ptyfs) is known by its
 * qid, "PTY", its number, its side (fstat's name is the one fd was
 * opened by: ptyclone, or /dev/cons the slave is bound on): its number,
 * *side 2 the master, 3 the slave; -1 if fd is none
 */
int
_ptynum(int fd, int *side)
{
	Dir *d;
	unsigned long long q;
	int t;

	if((d = _dirfstat(fd)) == nil)
		return -1;
	q = d->qid.path;
	free(d);
	t = q & 0xF;
	if((q >> 40) != 0x505459 || (t != 2 && t != 3))
		return -1;
	if(side != nil)
		*side = t;
	return (q >> 4) & 0xFFFFFFF;
}

/*
 * GCC port: a pty's modes are in ptyfs' /dev/ptyctlN (the protocol at
 * the file offset -2 needs negative offsets, which 9front's kernel
 * refuses): opened for the pty of fd (bsd/ioctl.c too); -1 if none
 */
int
_ptyctl(int fd, int omode)
{
	char path[32];
	int n;

	if((n = _ptynum(fd, nil)) < 0)
		return -1;
	snprintf(path, sizeof path, "/dev/ptyctl%d", n);
	return _OPEN(path, omode);
}

static int
isptty(int fd)
{
	/* GCC port: by its qid (_ptynum), either side (Linux's termios on the master too); the name was "ptty" */
	return _ptynum(fd, nil) >= 0;
}

int
tcgetattr(int fd, struct termios *t)
{
	int n;
	char buf[60];

	if(!isptty(fd)) {
		if(isatty(fd)) {
			/* If there is no emulation return sensible defaults */
			t->c_iflag = ISTRIP|ICRNL|IXON|IXOFF;
			t->c_oflag = OPOST|TAB3|ONLCR;
			t->c_cflag = B9600;
			t->c_lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK;
			t->c_cc[VINTR] = CINTR;
			t->c_cc[VQUIT] = CQUIT;
			t->c_cc[VERASE] = CERASE;
			t->c_cc[VKILL] = CKILL;
			t->c_cc[VEOF] = CEOF;
			t->c_cc[VEOL] = CEOL;
			t->c_cc[VSTART] = CSTART;
			t->c_cc[VSTOP] = CSTOP;
			return 0;
		} else {
			errno = ENOTTY;
			return -1;
		}
	}
	if((fd = _ptyctl(fd, 0)) < 0) {	/* GCC port */
		_syserrno();
		return -1;
	}

	n = _READ(fd, buf, 57);
	_CLOSE(fd);
	if(n < 0) {
		_syserrno();
		return -1;
	}

	t->c_iflag = strtoul(buf+4, 0, 16);
	t->c_oflag = strtoul(buf+9, 0, 16);
	t->c_cflag = strtoul(buf+14, 0, 16);
	t->c_lflag = strtoul(buf+19, 0, 16);

	for(n = 0; n < NCCS; n++)
		t->c_cc[n] = strtoul(buf+24+(n*3), 0, 16);

	return 0;
}

/* BUG: ignores optional actions */

int
tcsetattr(int fd, int, const struct termios *t)
{
	int n, i;
	char buf[100];

	if(!isptty(fd)) {
		if(!isatty(fd)) {
			errno = ENOTTY;
			return -1;
		} else
			return 0;
	}
	n = sprintf(buf, "IOW %4.4x %4.4x %4.4x %4.4x ",
		t->c_iflag, t->c_oflag, t->c_cflag, t->c_lflag);

	for(i = 0; i < NCCS; i++)
		n += sprintf(buf+n, "%2.2x ", t->c_cc[i]);

	if((fd = _ptyctl(fd, 1)) < 0) {	/* GCC port */
		_syserrno();
		return -1;
	}

	n = _WRITE(fd, buf, n);
	_CLOSE(fd);
	if(n < 0) {
		_syserrno();
		return -1;
	}

	return 0;
}

int
tcsetpgrp(int fd, pid_t pgrpid)
{
	int n;
	char buf[30];

	if(!isptty(fd)) {
		if(!isatty(fd)) {
			errno = ENOTTY;
			return -1;
		} else
			return 0;
	}
	n = sprintf(buf, "IOW note %d", pgrpid);

	if((fd = _ptyctl(fd, 1)) < 0) {	/* GCC port */
		_syserrno();
		return -1;
	}

	n = _WRITE(fd, buf, n);
	_CLOSE(fd);
	if(n < 0) {
		_syserrno();
		return -1;
	}
	return 0;
}

pid_t
tcgetpgrp(int fd)
{
	int n;
	pid_t pgrp;
	char buf[100];

	if(!isptty(fd)) {
		errno = ENOTTY;
		return -1;
	}
	if((fd = _ptyctl(fd, 0)) < 0) {	/* GCC port */
		_syserrno();
		return -1;
	}
	n = _READ(fd, buf, sizeof(buf)-1);
	_CLOSE(fd);
	if(n < 0) {
		_syserrno();
		return -1;
	}
	pgrp = atoi(buf+24+(NCCS*3));
	return pgrp;
}

/* should do a better job here */

int
tcdrain(int)
{
	errno = ENOTTY;
	return -1;
}

int
tcflush(int, int)
{
	errno = ENOTTY;
	return -1;
}

int
tcflow(int, int)
{
	errno = ENOTTY;
	return -1;
}

int
tcsendbreak(int)
{
	errno = ENOTTY;
	return -1;
}
