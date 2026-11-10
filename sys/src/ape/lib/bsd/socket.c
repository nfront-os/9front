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

#include "priv.h"

#include <lock.h>

static Lock _sock_lock;
static Rock *_sock_rock;

Rock*
_sock_findrock(int fd, struct stat *dp)
{
	Rock *r;
	struct stat d;

	if(dp == 0)
		dp = &d;
	if(fstat(fd, dp) < 0)
		return 0;
	lock(&_sock_lock);
	for(r = _sock_rock; r; r = r->next){
		if(r->inode == dp->st_ino
		&& r->dev == dp->st_dev)
			break;
	}
	unlock(&_sock_lock);
	return r;
}

Rock*
_sock_newrock(int fd)
{
	Rock *r;
	struct stat d;

	if(fstat(fd, &d) < 0)
		return 0;
	lock(&_sock_lock);
	for(r = _sock_rock; r; r = r->next){
		if(r->inode == d.st_ino
		&& r->dev == d.st_dev)
			break;
	}
	if(r == 0){
		r = malloc(sizeof(Rock));
		if(r == 0){
			unlock(&_sock_lock);
			return 0;
		}
		r->dev = d.st_dev;
		r->inode = d.st_ino;
		r->next = _sock_rock;
		_sock_rock = r;
	}
	unlock(&_sock_lock);
	r->rest = 0;
	memset(&r->raddr, 0, sizeof(r->raddr));
	memset(&r->addr, 0, sizeof(r->addr));
	r->reserved = 0;
	r->other = -1;
	return r;
}

int
_sock_data(int cfd, char *net, int domain, int stype, int protocol, Rock **rp)
{
	int n, fd;
	Rock *r;
	char name[Ctlsize];

	/* get the data file name */
	n = read(cfd, name, sizeof(name)-1);
	if(n < 0){
		close(cfd);
		errno = ENOBUFS;
		return -1;
	}
	name[n] = 0;
	n = strtoul(name, 0, 0);
	snprintf(name, sizeof name, "/net/%s/%d/data", net, n);

	/* open data file */
	fd = open(name, O_RDWR);
	close(cfd);
	if(fd < 0){
		errno = ENOBUFS;
		return -1;
	}

	/* hide stuff under the rock */
	snprintf(name, sizeof name, "/net/%s/%d/ctl", net, n);
	r = _sock_newrock(fd);
	if(r == 0){
		close(fd);
		errno = ENOBUFS;
		return -1;
	}
	r->domain = domain;
	r->stype = stype;
	r->protocol = protocol;
	strcpy(r->ctl, name);
	if(rp)
		*rp = r;
	return fd;
}

static int
socket1(int domain, int stype, int protocol)
{
	Rock *r;
	int cfd, fd, n;
	int pfd[2];
	char *net;

	switch(domain){
	case PF_INET:
	case PF_INET6:
		/* get a free network directory */
		switch(stype){
		case SOCK_DGRAM:
			net = "udp";
			cfd = open("/net/udp/clone", O_RDWR);
			break;
		case SOCK_STREAM:
			net = "tcp";
			cfd = open("/net/tcp/clone", O_RDWR);
			break;
		case SOCK_RDM:
			net = "il";
			cfd = open("/net/il/clone", O_RDWR);
			break;
		default:
			errno = EPROTONOSUPPORT;
			return -1;
		}
		if(cfd < 0)
			return -1;
		return _sock_data(cfd, net, domain, stype, protocol, 0);
	case PF_UNIX:
		if(pipe(pfd) < 0)
			return -1;
		r = _sock_newrock(pfd[0]);
		if(r == 0){
			close(pfd[0]);
			close(pfd[1]);
			errno = ENOBUFS;
			return -1;
		}
		r->domain = domain;
		r->stype = stype;
		r->protocol = protocol;
		r->other = pfd[1];
		return pfd[0];
	default:
		errno = EPROTONOSUPPORT;
		return -1;
	}
}

/*
 * 9front-ng: Linux's SOCK_CLOEXEC and SOCK_NONBLOCK in the type (libwayland's
 * sockets); no wait is taken but not done (a pipe's read waits)
 */
int
socket(int domain, int stype, int protocol)
{
	int fd;

	fd = socket1(domain, stype & ~(SOCK_CLOEXEC|SOCK_NONBLOCK), protocol);
	if(fd >= 0 && (stype & SOCK_CLOEXEC))
		fcntl(fd, F_SETFD, FD_CLOEXEC);
	return fd;
}

/* 9front-ng: what getsockopt asks of a socket (ap/gnu/posix.c): its domain and type; -1 if fd is none */
int
__apecompat_sockinfo(int fd, int *domain, int *stype)
{
	Rock *r;

	if((r = _sock_findrock(fd, 0)) == 0)
		return -1;
	*domain = r->domain;
	*stype = r->stype;
	return 0;
}

/* 9front-ng: Linux's accept4: accept, the new descriptor with SOCK_CLOEXEC, SOCK_NONBLOCK */
int
accept4(int fd, void *a, int *alen, int flags)
{
	int nfd;

	if((flags & ~(SOCK_CLOEXEC|SOCK_NONBLOCK)) != 0){
		errno = EINVAL;
		return -1;
	}
	if((nfd = accept(fd, a, alen)) < 0)
		return -1;
	if(flags & SOCK_CLOEXEC)
		fcntl(nfd, F_SETFD, FD_CLOEXEC);
	if(flags & SOCK_NONBLOCK)
		fcntl(nfd, F_SETFL, fcntl(nfd, F_GETFL) | O_NONBLOCK);
	return nfd;
}

int
issocket(int fd)
{
	Rock *r;

	r = _sock_findrock(fd, 0);
	return (r != 0);
}

/*
 * probably should do better than this
 */
int getsockopt(int, int, int, void *, int *)
{
	return -1;
}

int setsockopt(int, int, int, void *, int)
{
	return 0;
}

