/*
 * 9front-ng (../9front-ui PLAN §12): sendmsg and recvmsg, with descriptors
 * passed as SCM_RIGHTS (Wayland's shared memory, its keymaps).  A PF_UNIX
 * socket is a pipe (connect.c), and a pipe keeps the boundaries of writes:
 * sendmsg writes its message in one write, a header first
 *
 *	magic[8] length[4] nfd[4] name[32]...
 *
 * then the data.  Each descriptor goes into /srv under a name of its own
 * (Plan 9's way to hand an open file to another process); recvmsg opens
 * it there and removes it.  recvmsg reads the header alone, then no more
 * of the data than the caller takes: the rest stays in the pipe (counted
 * in the socket's rock), so select and poll still see it.  Data written
 * with write is read as it is (no header).  MSG_DONTWAIT: EAGAIN when no
 * message is there (through APE's read buffer).
 */

/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

/* socket extensions */
#include <sys/uio.h>
#include <sys/socket.h>

#include "priv.h"

enum {
	Hdr = 16,
	Name = 32,
	Maxfd = 28,	/* as Linux's SCM_MAX_FD, about */
	Max = 32768,	/* what a pipe writes at once */
};

static const char magic[8] = "APEmsg\0\1";

extern int _startbuf(int);	/* libap (gnu/buf.c): fd read through a copy proc */
static int nsent;

static void
put4(unsigned char *p, unsigned long v)
{
	p[0] = v;
	p[1] = v>>8;
	p[2] = v>>16;
	p[3] = v>>24;
}

static unsigned long
get4(unsigned char *p)
{
	return p[0] | p[1]<<8 | (unsigned long)p[2]<<16 | (unsigned long)p[3]<<24;
}

/* fd into /srv as name; the descriptor stays the caller's */
static int
post(char *name, int fd)
{
	char path[64], num[16];
	int sfd;

	snprintf(path, sizeof path, "/srv/%s", name);
	sfd = creat(path, 0600);
	if(sfd < 0)
		return -1;
	snprintf(num, sizeof num, "%d", fd);
	if(write(sfd, num, strlen(num)) < 0){
		close(sfd);
		unlink(path);
		return -1;
	}
	close(sfd);
	return 0;
}

long
sendmsg(int fd, const struct msghdr *msg, int flags)
{
	unsigned char *buf, *p;
	struct cmsghdr *c;
	int fds[Maxfd], nfd, i, j, n, len;
	char names[Maxfd][Name];

	if(flags & MSG_OOB){
		errno = EOPNOTSUPP;
		return -1;
	}
	nfd = 0;
	for(c = CMSG_FIRSTHDR(msg); c != NULL; c = CMSG_NXTHDR(msg, c)){
		if(c->cmsg_level != SOL_SOCKET || c->cmsg_type != SCM_RIGHTS)
			continue;
		n = (c->cmsg_len - CMSG_LEN(0)) / sizeof(int);
		for(i = 0; i < n; i++){
			if(nfd == Maxfd){
				errno = EINVAL;
				return -1;
			}
			memcpy(&fds[nfd++], CMSG_DATA(c) + i*sizeof(int), sizeof(int));
		}
	}
	len = 0;
	for(i = 0; i < msg->msg_iovlen; i++)
		len += msg->msg_iov[i].iov_len;
	if(Hdr + nfd*Name + len > Max){
		/* a message the pipe writes at once: the rest for the next call */
		len = Max - Hdr - nfd*Name;
	}
	if((buf = malloc(Hdr + nfd*Name + len)) == NULL){
		errno = ENOBUFS;
		return -1;
	}
	for(i = 0; i < nfd; i++){
		snprintf(names[i], Name, "APEfd.%d.%d", getpid(), nsent++);
		if(post(names[i], fds[i]) < 0){
			while(--i >= 0){
				snprintf((char *)buf, Name+8, "/srv/%s", names[i]);
				unlink((char *)buf);
			}
			free(buf);
			return -1;
		}
	}
	memcpy(buf, magic, 8);
	put4(buf+8, len);
	put4(buf+12, nfd);
	p = buf + Hdr;
	for(i = 0; i < nfd; i++, p += Name){
		memset(p, 0, Name);
		strncpy((char *)p, names[i], Name-1);
	}
	for(i = 0, j = 0; i < msg->msg_iovlen && j < len; i++){
		n = msg->msg_iov[i].iov_len;
		if(n > len - j)
			n = len - j;
		memcpy(p + j, msg->msg_iov[i].iov_base, n);
		j += n;
	}
	n = write(fd, buf, Hdr + nfd*Name + len);
	free(buf);
	if(n < 0){
		for(i = 0; i < nfd; i++){
			char path[Name+8];

			snprintf(path, sizeof path, "/srv/%s", names[i]);
			unlink(path);
		}
		return -1;
	}
	return len;
}

/* n bytes of the pipe into the caller's buffers */
static int
scatter(int fd, struct msghdr *msg, int n)
{
	char *buf;
	int i, m, j;

	if((buf = malloc(n > 0 ? n : 1)) == NULL){
		errno = ENOBUFS;
		return -1;
	}
	n = read(fd, buf, n);
	for(i = 0, j = 0; n > 0 && i < msg->msg_iovlen && j < n; i++){
		m = msg->msg_iov[i].iov_len;
		if(m > n - j)
			m = n - j;
		memcpy(msg->msg_iov[i].iov_base, buf + j, m);
		j += m;
	}
	free(buf);
	return n;
}

long
recvmsg(int fd, struct msghdr *msg, int flags)
{
	unsigned char hdr[Hdr], names[Maxfd*Name];
	char path[Name+8];
	struct cmsghdr *c;
	struct stat st;
	Rock *r;
	int i, n, want, nfd, len, got, nf, cloexec, room;
	int fds[Maxfd];

	if(flags & MSG_OOB){
		errno = EOPNOTSUPP;
		return -1;
	}
	msg->msg_flags = 0;
	want = 0;
	for(i = 0; i < msg->msg_iovlen; i++)
		want += msg->msg_iov[i].iov_len;
	r = _sock_findrock(fd, &st);
	nf = 0;
	if(r == NULL || r->rest == 0){
		/* a new message: its header, or plain data */
		if(flags & MSG_DONTWAIT){
			int fl;

			/*
			 * no wait (libwayland reads until EAGAIN): through APE's
			 * read buffer (poll's), which knows whether data is there
			 */
			if(_startbuf(fd) < 0)
				return -1;
			fl = fcntl(fd, F_GETFL);
			fcntl(fd, F_SETFL, fl|O_NONBLOCK);
			n = read(fd, hdr, Hdr);
			fcntl(fd, F_SETFL, fl);
		}else
			n = read(fd, hdr, Hdr);
		if(n <= 0){
			msg->msg_controllen = 0;
			return n;
		}
		if(n < Hdr || memcmp(hdr, magic, 8) != 0){
			/* written with write: what came is data */
			for(i = 0, got = 0; i < msg->msg_iovlen && got < n; i++){
				int m = msg->msg_iov[i].iov_len;

				if(m > n - got)
					m = n - got;
				memcpy(msg->msg_iov[i].iov_base, hdr + got, m);
				got += m;
			}
			if(got < n)
				msg->msg_flags |= MSG_TRUNC;
			msg->msg_controllen = 0;
			return got;
		}
		len = get4(hdr+8);
		nfd = get4(hdr+12);
		if(nfd > Maxfd || read(fd, names, nfd*Name) != nfd*Name){
			errno = EIO;
			return -1;
		}
		cloexec = (flags & MSG_CMSG_CLOEXEC) != 0;
		for(i = 0; i < nfd; i++){
			names[i*Name + Name-1] = 0;
			snprintf(path, sizeof path, "/srv/%s", (char *)names + i*Name);
			fds[nf] = open(path, O_RDWR);
			if(fds[nf] < 0)
				fds[nf] = open(path, O_RDONLY);
			unlink(path);
			if(fds[nf] >= 0){
				if(cloexec)
					fcntl(fds[nf], F_SETFD, FD_CLOEXEC);
				nf++;
			}
		}
		if(r != NULL)
			r->rest = len;
	}else
		len = r->rest;

	/* the descriptors into the caller's control buffer, what fits */
	if(nf > 0){
		room = msg->msg_control != NULL && msg->msg_controllen >= CMSG_LEN(sizeof(int)) ?
			(msg->msg_controllen - CMSG_LEN(0)) / sizeof(int) : 0;
		if(room > nf)
			room = nf;
		if(room > 0){
			c = CMSG_FIRSTHDR(msg);
			c->cmsg_level = SOL_SOCKET;
			c->cmsg_type = SCM_RIGHTS;
			c->cmsg_len = CMSG_LEN(room*sizeof(int));
			memcpy(CMSG_DATA(c), fds, room*sizeof(int));
			msg->msg_controllen = CMSG_SPACE(room*sizeof(int));
		}else
			msg->msg_controllen = 0;
		for(i = room; i < nf; i++)
			close(fds[i]);
		if(room < nf)
			msg->msg_flags |= MSG_CTRUNC;
	}else
		msg->msg_controllen = 0;

	if(len == 0)
		return 0;
	n = scatter(fd, msg, want < len ? want : len);
	if(n > 0 && r != NULL)
		r->rest = len - n;
	return n;
}
