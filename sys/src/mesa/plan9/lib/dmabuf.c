/*
 * SPDX-License-Identifier: MIT
 *
 * dma-buf fds on 9front: Vulkan's external memory (VK_KHR_external_memory_fd,
 * dma-buf handles) wants a file descriptor for a GPU buffer that another
 * process can import.  gpu(4)'s servers name an exported buffer by a token
 * (ctx ctl "export id" -> "export token"; "import token" in another context,
 * of any process); here a token takes the shape of an fd: one end of a pipe,
 * whose qid tells it from a later file with the same number.  Within a
 * process the fd goes through Vulkan as any dma-buf fd; between processes the
 * token goes as text (a compositor's ctl) and becomes an fd there again.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

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

/* a new fd that stands for the exported buffer token, or -1 */
int
plan9_dmabuf_fd(const char *token)
{
	struct stat st;
	Dmabuf *d;
	int p[2], i;

	if(token == NULL || strlen(token) >= Ntoken || pipe(p) < 0)
		return -1;
	close(p[1]);
	if(fstat(p[0], &st) < 0){
		close(p[0]);
		return -1;
	}
	pthread_mutex_lock(&lk);
	d = NULL;
	for(i = 0; i < ntab; i++)
		if(tab[i].fd < 0 || lookup(tab[i].fd) != &tab[i]){	/* free, or closed since */
			d = &tab[i];
			break;
		}
	if(d == NULL && ntab < Nfd)
		d = &tab[ntab++];
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

/* the token of a dma-buf fd into buf; -1 if fd is none */
int
plan9_dmabuf_token(int fd, char *buf, int n)
{
	Dmabuf *d;
	int r;

	r = -1;
	pthread_mutex_lock(&lk);
	if((d = lookup(fd)) != NULL && (int)strlen(d->token) < n){
		strcpy(buf, d->token);
		r = 0;
	}
	pthread_mutex_unlock(&lk);
	return r;
}
