/*
 * What POSIX.1-2024 took from GNU and the BSDs, on APE: pipe2 and
 * mkostemp (their flags: O_CLOEXEC by fcntl's FD_CLOEXEC, which APE's
 * exec honours, O_NONBLOCK, O_APPEND), strchrnul, confstr; and glibc's
 * pthread_setname_np: the thread's proc is named so in its
 * /proc/n/args (9front lets a process write its own), as ps shows it;
 * posix_openpt, grantpt, unlockpt on ptyfs.
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>

static int
setflags(int fd, int flags)
{
	int fl;

	if((flags & O_CLOEXEC) != 0 && fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)
		return -1;
	if((flags & (O_NONBLOCK|O_APPEND)) != 0){
		if((fl = fcntl(fd, F_GETFL)) < 0)
			return -1;
		if(fcntl(fd, F_SETFL, fl | (flags & (O_NONBLOCK|O_APPEND))) < 0)
			return -1;
	}
	return 0;
}

int
pipe2(int *fd, int flags)
{
	int e;

	if((flags & ~(O_CLOEXEC|O_NONBLOCK)) != 0){
		errno = EINVAL;
		return -1;
	}
	if(pipe(fd) < 0)
		return -1;
	if(setflags(fd[0], flags) < 0 || setflags(fd[1], flags) < 0){
		e = errno;
		close(fd[0]);
		close(fd[1]);
		errno = e;
		return -1;
	}
	return 0;
}

int
mkostemp(char *tmpl, int flags)
{
	int fd, e;

	if((flags & ~(O_CLOEXEC|O_APPEND|O_SYNC)) != 0){
		errno = EINVAL;
		return -1;
	}
	if((fd = mkstemp(tmpl)) < 0)
		return -1;
	if(setflags(fd, flags & ~O_SYNC) < 0){
		e = errno;
		close(fd);
		unlink(tmpl);
		errno = e;
		return -1;
	}
	return fd;
}

/* POSIX's confstr: _CS_PATH, where APE's and 9front's programs are (/bin has both) */
size_t
confstr(int name, char *buf, size_t n)
{
	const char *v;

	switch(name){
	case _CS_PATH:
		v = "/bin";
		break;
	default:
		errno = EINVAL;
		return 0;
	}
	if(buf != NULL && n > 0)
		snprintf(buf, n, "%s", v);
	return strlen(v) + 1;
}

char*
strchrnul(const char *s, int c)
{
	while(*s != 0 && *s != (char)c)
		s++;
	return (char*)s;
}

/*
 * POSIX's ptys on ptyfs (libbsd's pty.c: _getpty opens /dev/ptyclone,
 * starting ptyfs if there is none): the slave is ready at once
 */
extern int _getpty(void);

int
posix_openpt(int flags)
{
	int fd;

	if((flags & O_ACCMODE) != O_RDWR){
		errno = EINVAL;
		return -1;
	}
	if((fd = _getpty()) < 0)
		return -1;
	if((flags & O_CLOEXEC) != 0)
		fcntl(fd, F_SETFD, FD_CLOEXEC);
	return fd;
}

int
grantpt(int fd)
{
	(void)fd;
	return 0;
}

int
unlockpt(int fd)
{
	(void)fd;
	return 0;
}

/* struct __pthread's pid (pthread.c) */
extern int __apecompat_pthreadpid(pthread_t);

int
pthread_setname_np(pthread_t t, const char *name)
{
	char path[32];
	int fd, n;

	if(strlen(name) >= 16)
		return ERANGE;	/* glibc's limit */
	snprintf(path, sizeof path, "/proc/%d/args", __apecompat_pthreadpid(t));
	if((fd = open(path, O_WRONLY|O_TRUNC)) < 0)
		return errno;
	n = strlen(name);
	if(write(fd, name, n) != n){
		close(fd);
		return errno;
	}
	close(fd);
	return 0;
}
