/*
 * GCC port: APE's read buffering, select and poll for gcc's APE (in place of
 * plan9/_buf.c, which is kencc's).
 *
 * Plan 9 cannot ask a descriptor whether a read would block. APE reads such a
 * descriptor in a proc of its own into a buffer, which read(2) then takes from
 * (_readbuf); select waits for those procs. _buf.c's way does not work with
 * threads (pthread.c, Rust's std): it forks the copy procs on the stack it
 * runs on, which is shared memory then (a thread's stack from malloc, std's
 * stack slots), attaches its buffers' segment when first needed (the
 * program's other procs do not have it), and lets one proc wait at a time.
 *
 * Here the copy procs are forked RFMEM|RFFDG onto a stack segment of their
 * own (rforkproc: Rust's std, pthread.c, or a plain rfork when main is on the
 * kernel's stack), the buffers are from malloc, and any number of procs wait
 * in poll or select, each on a semaphore of its own: the copy procs wake them
 * all when data or the end comes. They go at exit (killcopies). poll(2) is
 * new: _buf.c has only select.
 */
/* as APE's own (sys/src/ape/lib/ap/plan9/mkfile) */
#define _POSIX_SOURCE
#define _PLAN9_SOURCE
#define _BSD_EXTENSION
#define _LOCK_EXTENSION
#define _RESEARCH_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include "../plan9/lib.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <lock.h>
#include <sys/time.h>
#include <sys/select.h>
#include <poll.h>
#include <signal.h>
#include "../plan9/sys9.h"
#include "../plan9/dir.h"

extern int _SEMACQUIRE(int *, int);
extern int _TSEMACQUIRE(int *, unsigned long);
extern int _SEMRELEASE(int *, int);
/* Rust's std (library/std/src/sys/pal/plan9/proc.rs), pthread.c: if linked */
extern int __plan9_rforkproc(int, void (*)(void *), void *) __attribute__((weak));
extern int __apecompat_rforkproc(int, void (*)(void *), void *) __attribute__((weak));

enum {
	RFFDG_ = 1<<2,
	RFPROC_ = 1<<4,
	RFMEM_ = 1<<5,
	RFNOWAIT_ = 1<<6,
};

typedef struct Buf Buf;
struct Buf {
	int	fd;		/* the descriptor the copy proc reads (its own copy) */
	int	ref;		/* 9front-ng: descriptors of the program with this buffer */
	int	type;		/* 9front-ng: the stream's identity (device, qid), when known */
	unsigned int	dev;
	unsigned long long	path;
	int	pid;		/* of the copy proc */
	int	closed;		/* the program closed fd */
	int	eof;		/* after the data */
	int	r, w;		/* data[r..w] unread */
	int	roomwait;	/* the copy proc waits for room */
	int	room;		/* its semaphore */
	int	datawait;	/* a reader waits for data */
	int	data_;		/* its semaphore */
	unsigned char	data[PERFDMAX];
};

typedef struct Waiter Waiter;
struct Waiter {
	int	sem;
	Waiter	*next;
};

static Lock lk;
static Waiter *waiters;

/* libbsd's listen.c knows these: its procs' note group (_buf.c's) */
int _muxsid = -1;
static int mainpid = -1;	/* of the copy procs' program */

void
_killmuxsid(void)
{
	if(_muxsid != -1 && (mainpid == getpid() || mainpid == -1))
		kill(-_muxsid, SIGTERM);
}

/* fn(arg) in a new proc, rfork(RFPROC|flags), on a stack segment of its own */
static int
rforkproc(int flags, void (*fn)(void *), void *arg)
{
	int pid;

	if(__plan9_rforkproc != NULL)
		return __plan9_rforkproc(flags, fn, arg);
	if(__apecompat_rforkproc != NULL)
		return __apecompat_rforkproc(RFPROC_|flags, fn, arg);
	/* main on the kernel's stack segment: the child has a copy of it */
	pid = _RFORK(RFPROC_|flags);
	if(pid == 0){
		fn(arg);
		_EXITS(NULL);
	}
	return pid;
}

static void
killpid(int pid)
{
	char path[32];
	int cfd;

	/* proc(3)'s kill: a note could be caught */
	snprintf(path, sizeof path, "/proc/%d/ctl", pid);
	cfd = _OPEN(path, 1);
	if(cfd >= 0){
		_WRITE(cfd, "kill", 4);
		_CLOSE(cfd);
	}
}

/* exit: the copy procs go (they would read on, the console's input too) */
static void
killcopies(void)
{
	Buf *b;
	int i;

	if(getpid() != mainpid)
		return;
	for(i = 0; i < OPEN_MAX; i++){
		b = (Buf *)_fdinfo[i].buf;
		if((_fdinfo[i].flags&FD_BUFFERED) && b != NULL && !b->eof && b->pid > 0)
			killpid(b->pid);
	}
}

static void
semwait(int *s)
{
	while(_SEMACQUIRE(s, 1) < 0)
		;
}

/* called with lk: the data or the end came */
static void
wakeall(Buf *b)
{
	Waiter *w;

	if(b->datawait){
		b->datawait = 0;
		_SEMRELEASE(&b->data_, 1);
	}
	for(w = waiters; w != NULL; w = w->next)
		_SEMRELEASE(&w->sem, 1);
	waiters = NULL;
}

/*
 * The copy proc has a copy of the descriptors (RFFDG), which the program's procs share:
 * all but its own go, as in APE, or it would hold others' connections open. dup(3)'s #d
 * lists them; malloc is fine (RFMEM: the heap and its lock are the program's).
 */
static void
closeothers(int keep)
{
	Dir *d;
	char *p;
	int dfd, fd, i, n;

	/* raw calls: APE's _fdinfo is the program's (RFMEM), not to be touched here */
	dfd = _OPEN("#d", 0);
	if(dfd < 0)
		return;
	n = _dirreadall(dfd, &d);
	for(i = 0; i < n; i++){
		fd = strtol(d[i].name, &p, 10);
		if(*p != '\0' || p == d[i].name || fd == keep || fd == dfd)
			continue;
		_CLOSE(fd);
	}
	_CLOSE(dfd);
	if(n > 0)
		free(d);
}

static void
copyproc(void *v)
{
	Buf *b;
	int n, nzeros;

	b = v;
	closeothers(b->fd);
	for(;;){
		lock(&lk);
		if(b->closed){
			unlock(&lk);
			return;
		}
		if(b->r == b->w)
			b->r = b->w = 0;
		if(PERFDMAX - b->w < READMAX){
			b->roomwait = 1;
			unlock(&lk);
			semwait(&b->room);
			continue;
		}
		unlock(&lk);
		/*
		 * A zero-length read might be a zero-length write or the end:
		 * several, as APE (POSIX read() drops zero-length messages)
		 */
		nzeros = 0;
		do
			n = _READ(b->fd, b->data + b->w, READMAX);
		while(n == 0 && ++nzeros < 3 && !b->closed);
		lock(&lk);
		if(b->closed){
			unlock(&lk);
			return;
		}
		if(n <= 0)
			b->eof = 1;
		else
			b->w += n;
		wakeall(b);
		unlock(&lk);
		if(n <= 0)
			return;
	}
}

/*
 * 9front-ng: the identity of fd's open file if it is a stream (a pipe, a
 * network connection: not a mounted file, whose opens have offsets of their
 * own).  Descriptors of one stream (dup: libwayland reads a client's
 * connection through one and polls another) share one buffer: a copy proc
 * reading for one of them takes the data of all.
 */
static int
ident(int fd, int *type, unsigned int *dev, unsigned long long *path)
{
	Dir *d;

	if((d = _dirfstat(fd)) == NULL)
		return 0;
	if(d->type == 'M' || (d->qid.type & 0x80)){	/* a mounted file; a directory */
		free(d);
		return 0;
	}
	*type = d->type;
	*dev = d->dev;
	*path = d->qid.path;
	free(d);
	return 1;
}

/*
 * 9front-ng: descriptors made by dup of one another, a group each (0: none):
 * one channel, whatever the file, so they share a buffer too (libwayland
 * polls a dup of an fd source and hands its callback the original: wsys'
 * event file, a mounted one, read through both)
 */
static int dupgrp[OPEN_MAX];
static int dupgen;

/* fd is closed: no longer in its group */
void
_dupclose(int fd)
{
	if(fd >= 0 && fd < OPEN_MAX)
		dupgrp[fd] = 0;
}

/* called with lk: i and fd read the same: dups of one another, or one stream */
static int
samefile(int i, int fd, int type, unsigned int dev, unsigned long long path, int isstream)
{
	Buf *ob;

	if(dupgrp[fd] != 0 && dupgrp[i] == dupgrp[fd])
		return 1;
	ob = (Buf *)_fdinfo[i].buf;
	return isstream && ob != NULL && ob->type == type && ob->dev == dev && ob->path == path;
}

/* fd read through a copy proc from now on; -1 if it cannot be */
int
_startbuf(int fd)
{
	Fdinfo *f;
	Buf *b;
	int pid;

	if(fd < 0)
		return 0;
	if(fd >= OPEN_MAX){
		errno = EBADF;
		return -1;
	}
	f = &_fdinfo[fd];
	lock(&lk);
	if((f->flags&FD_ISOPEN) == 0){
		unlock(&lk);
		errno = EBADF;
		return -1;
	}
	if(f->flags&FD_BUFFERED){
		unlock(&lk);
		return 0;
	}
	unlock(&lk);
	{
		int type, i, others[OPEN_MAX], nothers, isstream;
		unsigned int dev;
		unsigned long long path;
		Buf *ob;

		type = -1;
		dev = 0;
		path = 0;
		isstream = ident(fd, &type, &dev, &path);
		/* a buffer of the same file (a dup, the stream): this descriptor's too */
		lock(&lk);
		for(i = 0; i < OPEN_MAX; i++){
			ob = (Buf *)_fdinfo[i].buf;
			if(i != fd && (_fdinfo[i].flags&FD_BUFFERED) && ob != NULL && ob->ref > 0 && !ob->closed
			&& samefile(i, fd, type, dev, path, isstream)){
				f->buf = (Muxbuf *)ob;
				f->flags |= FD_BUFFERED;
				ob->ref++;
				unlock(&lk);
				return 0;
			}
		}
		unlock(&lk);
		/* other descriptors of it, not buffered yet: they share the new one */
		nothers = 0;
		for(i = 0; i < OPEN_MAX; i++){
			int t;
			unsigned int dv;
			unsigned long long pa;

			if(i == fd || (_fdinfo[i].flags&(FD_ISOPEN|FD_BUFFERED|FD_BUFFEREDX)) != FD_ISOPEN)
				continue;
			if((dupgrp[fd] != 0 && dupgrp[i] == dupgrp[fd])
			|| (isstream && ident(i, &t, &dv, &pa) && t == type && dv == dev && pa == path))
				others[nothers++] = i;
		}
		if(isstream || nothers > 0){
			lock(&lk);
			if(f->flags&FD_BUFFERED){	/* another thread was quicker */
				unlock(&lk);
				return 0;
			}
			b = calloc(1, sizeof *b);
			if(b == NULL){
				unlock(&lk);
				errno = ENOMEM;
				return -1;
			}
			b->type = isstream ? type : -1;
			b->dev = dev;
			b->path = path;
			b->ref = 1;
			for(i = 0; i < nothers; i++)
				if((_fdinfo[others[i]].flags&(FD_ISOPEN|FD_BUFFERED)) == FD_ISOPEN){
					_fdinfo[others[i]].buf = (Muxbuf *)b;
					_fdinfo[others[i]].flags |= FD_BUFFERED;
					b->ref++;
				}
			goto made;
		}
	}
	lock(&lk);
	if(f->flags&FD_BUFFERED){
		unlock(&lk);
		return 0;
	}
	b = calloc(1, sizeof *b);
	if(b == NULL){
		unlock(&lk);
		errno = ENOMEM;
		return -1;
	}
	b->ref = 1;
	b->type = -1;
made:
	b->fd = fd;
	f->buf = (Muxbuf *)b;
	f->flags |= FD_BUFFERED;
	if(mainpid == -1){
		mainpid = getpid();
		atexit(killcopies);
	}
	unlock(&lk);
	pid = rforkproc(RFMEM_|RFFDG_|RFNOWAIT_, copyproc, b);
	if(pid < 0){
		int i;

		lock(&lk);
		for(i = 0; i < OPEN_MAX; i++)
			if((Buf *)_fdinfo[i].buf == b){
				_fdinfo[i].flags &= ~FD_BUFFERED;
				_fdinfo[i].buf = NULL;
			}
		unlock(&lk);
		free(b);
		_syserrno();
		return -1;
	}
	b->pid = pid;
	return 0;
}

/* fd is closed: its copy proc goes, after the last descriptor sharing it */
void
_closebuf(int fd)
{
	Buf *b;
	int done;

	b = (Buf *)_fdinfo[fd].buf;
	if(b == NULL)
		return;
	lock(&lk);
	if(--b->ref > 0){
		/* 9front-ng: other descriptors of the stream read through it still */
		unlock(&lk);
		_fdinfo[fd].buf = NULL;
		return;
	}
	b->closed = 1;
	done = b->eof;
	if(b->roomwait){
		b->roomwait = 0;
		_SEMRELEASE(&b->room, 1);
	}
	wakeall(b);
	unlock(&lk);
	_fdinfo[fd].buf = NULL;
	if(done){
		/* the copy proc has returned or returns without touching b */
		free(b);
		return;
	}
	/* a copy proc in a read: killed; its buffer stays (it might still write) */
	killpid(b->pid);
}

/* read(2) of a buffered fd; noblock: none if there is no data */
int
_readbuf(int fd, void *addr, int nwant, int noblock)
{
	Buf *b;
	int n;

	b = (Buf *)_fdinfo[fd].buf;
	if(b == NULL){	/* 9front-ng: b->fd may be another descriptor of the stream */
		errno = EBADF;
		return -1;
	}
	lock(&lk);
	for(;;){
		n = b->w - b->r;
		if(n > 0 || b->eof || b->closed)
			break;
		if(noblock){
			unlock(&lk);
			errno = EAGAIN;
			return -1;
		}
		b->datawait = 1;
		unlock(&lk);
		semwait(&b->data_);
		lock(&lk);
	}
	if(n > nwant)
		n = nwant;
	memcpy(addr, b->data + b->r, n);
	b->r += n;
	if(b->r == b->w && b->roomwait){
		b->r = b->w = 0;
		b->roomwait = 0;
		_SEMRELEASE(&b->room, 1);
	}
	unlock(&lk);
	return n;
}

/* a forked child cannot read its parent's copy procs' buffers */
void
_detachbuf(void)
{
	int i;

	for(i = 0; i < OPEN_MAX; i++)
		if(_fdinfo[i].flags&FD_BUFFERED){
			_fdinfo[i].flags &= ~FD_BUFFERED;
			_fdinfo[i].buf = NULL;
		}
	_muxsid = -1;
	mainpid = -1;
}

static long long
msnow(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long long)ts.tv_sec*1000 + ts.tv_nsec/1000000;
}

/* called with lk */
static int
ready(struct pollfd *fds, nfds_t n)
{
	nfds_t i;
	int c;
	Fdinfo *f;
	Buf *b;

	c = 0;
	for(i = 0; i < n; i++){
		fds[i].revents = 0;
		if(fds[i].fd < 0)
			continue;
		if(fds[i].fd >= OPEN_MAX || (_fdinfo[fds[i].fd].flags&FD_ISOPEN) == 0){
			fds[i].revents = POLLNVAL;
			c++;
			continue;
		}
		f = &_fdinfo[fds[i].fd];
		/* writing: assumed ready, as APE's select */
		fds[i].revents |= fds[i].events & (POLLOUT|POLLWRNORM|POLLWRBAND);
		b = (Buf *)f->buf;
		if((f->flags&FD_BUFFERED) && b != NULL){
			if(b->w > b->r || b->eof)
				fds[i].revents |= fds[i].events & (POLLIN|POLLRDNORM|POLLRDBAND|POLLPRI);
			if(b->eof && b->w == b->r)
				fds[i].revents |= POLLHUP;
		}
		if(fds[i].revents)
			c++;
	}
	return c;
}

int
poll(struct pollfd *fds, nfds_t n, int ms)
{
	nfds_t i;
	Waiter *w, **l;
	long long deadline, left;
	int c, r;

	for(i = 0; i < n; i++)
		if(fds[i].fd >= 0 && fds[i].fd < OPEN_MAX
		&& (fds[i].events & (POLLIN|POLLRDNORM|POLLRDBAND|POLLPRI))
		&& (_fdinfo[fds[i].fd].flags&(FD_ISOPEN|FD_BUFFERED)) == FD_ISOPEN)
			if(_startbuf(fds[i].fd) < 0)
				return -1;
	deadline = ms > 0 ? msnow() + ms : 0;
	/*
	 * the copy procs walk the waiters and release their semaphores: shared
	 * memory, not the stack (main's is the kernel's stack segment, the
	 * copy procs have copies of it)
	 */
	w = NULL;
	for(;;){
		lock(&lk);
		c = ready(fds, n);
		if(c > 0 || ms == 0){
			unlock(&lk);
			free(w);
			return c;
		}
		if(w == NULL && (w = malloc(sizeof *w)) == NULL){
			unlock(&lk);
			errno = ENOMEM;
			return -1;
		}
		w->sem = 0;
		w->next = waiters;
		waiters = w;
		unlock(&lk);
		if(ms < 0)
			r = _SEMACQUIRE(&w->sem, 1);
		else{
			left = deadline - msnow();
			r = left > 0 ? _TSEMACQUIRE(&w->sem, left) : 0;
		}
		lock(&lk);
		for(l = &waiters; *l != NULL; l = &(*l)->next)
			if(*l == w){
				*l = w->next;
				break;
			}
		unlock(&lk);
		if(r < 0){
			free(w);
			errno = EINTR;
			return -1;
		}
		if(r == 0 && ms > 0 && msnow() >= deadline){
			lock(&lk);
			c = ready(fds, n);
			unlock(&lk);
			free(w);
			return c;
		}
	}
}

int
select(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tv)
{
	struct pollfd p[FD_SETSIZE];
	int i, n, c, ms;

	if(nfds > FD_SETSIZE)
		nfds = FD_SETSIZE;
	n = 0;
	for(i = 0; i < nfds; i++){
		p[n].fd = i;
		p[n].events = 0;
		if(rfds && FD_ISSET(i, rfds))
			p[n].events |= POLLIN;
		if(wfds && FD_ISSET(i, wfds))
			p[n].events |= POLLOUT;
		if(efds && FD_ISSET(i, efds))
			p[n].events |= POLLPRI;
		if(p[n].events)
			n++;
	}
	ms = tv == NULL ? -1 : tv->tv_sec*1000 + (tv->tv_usec+999)/1000;
	c = poll(p, n, ms);
	if(c < 0)
		return -1;
	if(rfds)
		FD_ZERO(rfds);
	if(wfds)
		FD_ZERO(wfds);
	if(efds)
		FD_ZERO(efds);
	c = 0;
	for(i = 0; i < n; i++){
		if(rfds && (p[i].revents & (POLLIN|POLLHUP)))
			FD_SET(p[i].fd, rfds), c++;
		if(wfds && (p[i].revents & POLLOUT))
			FD_SET(p[i].fd, wfds), c++;
		if(efds && (p[i].revents & (POLLERR|POLLNVAL)))
			FD_SET(p[i].fd, efds), c++;
	}
	return c;
}

/*
 * 9front-ng: dup (fcntl F_DUPFD): to is in from's group; when from is
 * buffered, to reads through the same buffer (was EGREG)
 */
int
_dupbuf(int from, int to)
{
	Buf *b;

	lock(&lk);
	if(from >= 0 && from < OPEN_MAX && to >= 0 && to < OPEN_MAX){
		if(dupgrp[from] == 0)
			dupgrp[from] = ++dupgen;
		dupgrp[to] = dupgrp[from];
	}
	b = (Buf *)_fdinfo[from].buf;
	if((_fdinfo[from].flags&FD_BUFFERED) && b != NULL){
		b->ref++;
		_fdinfo[to].buf = (Muxbuf *)b;
		_fdinfo[to].flags |= FD_BUFFERED;
	}
	unlock(&lk);
	return 0;
}
