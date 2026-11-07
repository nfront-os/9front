/*
 * SPDX-License-Identifier: MIT
 *
 * dma-buf fds on 9front: Vulkan's external memory (VK_KHR_external_memory_fd,
 * dma-buf handles) wants a file descriptor for a GPU buffer that another
 * process can import.  gpu(4)'s servers name an exported buffer by a token
 * (ctx ctl "export id" -> "export token"; "import token" in another context,
 * of any process); here a token takes the shape of an fd: the read end of a
 * pipe that holds "dmabuf token\n" (the write end closed), whose qid tells
 * it from a later file with the same number.  Within a process the fd goes
 * through Vulkan as any dma-buf fd, its token from the table; another
 * process that got the fd (APE's SCM_RIGHTS: the same pipe, through /srv;
 * a Wayland compositor, wlsys(4)) reads the token from the pipe, once: that
 * process keeps it in its table.  Between processes the token may also go
 * as text (wsys' ctl).
  */
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int	plan9_dmabuf_fd(const char *token);
int	plan9_dmabuf_token(int fd, char *buf, int n);

enum { Nfd = 256, Ntoken = 64 };

typedef struct Dmabuf Dmabuf;
struct Dmabuf {
	int	fd;	/* -1: free */
	ino_t	ino;	/* the pipe's qid path */
	char	token[Ntoken];
};

static Dmabuf tab[Nfd];
static int ntab;
static pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;

/* the entry of fd if it still is the pipe it was made as; with lk */
static Dmabuf*
lookup(int fd)
{
	struct stat st;
	int i;

	if(fstat(fd, &st) < 0)
		return NULL;
	for(i = 0; i < ntab; i++)
		if(tab[i].fd == fd && tab[i].ino == st.st_ino)
			return &tab[i];
	return NULL;
}

/* a free entry, or NULL; with lk */
static Dmabuf*
newentry(void)
{
	int i;

	for(i = 0; i < ntab; i++)
		if(tab[i].fd < 0 || lookup(tab[i].fd) != &tab[i])	/* free, or closed since */
			return &tab[i];
	if(ntab < Nfd)
		return &tab[ntab++];
	return NULL;
}

/* a new fd that stands for the exported buffer token, or -1 */
int
plan9_dmabuf_fd(const char *token)
{
	struct stat st;
	Dmabuf *d;
	char msg[Ntoken+16];
	int p[2], n;

	if(token == NULL || strlen(token) >= Ntoken || pipe(p) < 0)
		return -1;
	n = snprintf(msg, sizeof msg, "dmabuf %s\n", token);
	if(write(p[1], msg, n) != n || fstat(p[0], &st) < 0){
		close(p[0]);
		close(p[1]);
		return -1;
	}
	close(p[1]);
	pthread_mutex_lock(&lk);
	d = newentry();
	if(d == NULL){
		pthread_mutex_unlock(&lk);
		close(p[0]);
		return -1;
	}
	d->fd = p[0];
	d->ino = st.st_ino;
	strcpy(d->token, token);
	pthread_mutex_unlock(&lk);
	return p[0];
}

/*
 * the token of a pipe made by plan9_dmabuf_fd in another process, read
 * from it: only when its message is there whole (stat's length), so
 * that another file is neither waited for nor read
 */
static int
readtoken(int fd, struct stat *st, char *buf, int n)
{
	char msg[Ntoken+16], *e;
	int m;

	if(!S_ISFIFO(st->st_mode) || st->st_size < 9 || st->st_size >= (off_t)sizeof msg)
		return -1;
	if((m = read(fd, msg, st->st_size)) != st->st_size)
		return -1;
	msg[m] = 0;
	if(strncmp(msg, "dmabuf ", 7) != 0 || (e = strchr(msg, '\n')) == NULL || e[1] != 0)
		return -1;
	*e = 0;
	if((int)strlen(msg+7) >= n)
		return -1;
	strcpy(buf, msg+7);
	return 0;
}

/* the token of a dma-buf fd into buf; -1 if fd is none */
int
plan9_dmabuf_token(int fd, char *buf, int n)
{
	struct stat st;
	Dmabuf *d;
	int i, r;

	if(fstat(fd, &st) < 0)
		return -1;
	r = -1;
	pthread_mutex_lock(&lk);
	/* the pipe, under this or another descriptor (a dup, one received) */
	d = NULL;
	for(i = 0; i < ntab; i++)
		if(tab[i].fd >= 0 && tab[i].ino == st.st_ino){
			d = &tab[i];
			break;
		}
	if(d == NULL && (d = newentry()) != NULL){
		if(readtoken(fd, &st, d->token, sizeof d->token) < 0){
			d->fd = -1;
			d = NULL;
		}
	}
	if(d != NULL){
		d->fd = fd;
		d->ino = st.st_ino;
		if((int)strlen(d->token) < n){
			strcpy(buf, d->token);
			r = 0;
		}
	}
	pthread_mutex_unlock(&lk);
	return r;
}
