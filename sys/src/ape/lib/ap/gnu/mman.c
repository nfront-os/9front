/*
 * GCC port: mmap for APE, which has none (LLVM maps the files it reads and writes): the
 * memory is from posix_memalign (page aligned), a file's part read into it; a shared, writable
 * mapping goes back to the file at msync and munmap. No sharing between procs beyond the
 * program's own memory, no protection, no executable memory (PROT_EXEC is refused).
 */
#define _POSIX_SOURCE
#define _PLAN9_SOURCE
#define _LOCK_EXTENSION
#define _RESEARCH_SOURCE
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <lock.h>


typedef struct Map Map;
struct Map {
	char	*p;
	size_t	len;
	int	fd;	/* a dup of the file's, if written back; else -1 */
	off_t	off;
	Map	*next;
};

static Lock lk;
static Map *maps;

static int
writeback(Map *m, char *from, size_t n)
{
	size_t done;
	long r;

	if(m->fd < 0)
		return 0;
	for(done = 0; done < n; done += r){
		r = pwrite(m->fd, from + done, n - done, m->off + (from - m->p) + done);
		if(r <= 0)
			return -1;
	}
	return 0;
}

void *
mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)
{
	Map *m;
	char *p;
	size_t done;
	long r;

	(void)addr;
	if(len == 0 || (prot & PROT_EXEC) || len > 0xffffffffUL){
		errno = len == 0 ? EINVAL : ENOMEM;
		return MAP_FAILED;
	}
	m = malloc(sizeof *m);
	if(posix_memalign((void **)&p, 4096, len) != 0)
		p = NULL;
	if(m == NULL || p == NULL){
		free(m);
		free(p);
		errno = ENOMEM;
		return MAP_FAILED;
	}
	memset(p, 0, len);
	m->p = p;
	m->len = len;
	m->off = off;
	m->fd = -1;
	if(!(flags & MAP_ANONYMOUS)){
		for(done = 0; done < len; done += r){
			r = pread(fd, p + done, len - done, off + done);
			if(r < 0){
				free(p);
				free(m);
				return MAP_FAILED;
			}
			if(r == 0)
				break;	/* beyond the end: zeros */
		}
		if((flags & MAP_SHARED) && (prot & PROT_WRITE)){
			m->fd = dup(fd);
			if(m->fd < 0){
				free(p);
				free(m);
				return MAP_FAILED;
			}
		}
	}
	lock(&lk);
	m->next = maps;
	maps = m;
	unlock(&lk);
	return p;
}

static Map *
find(void *p, int unlink)
{
	Map *m, **l;

	lock(&lk);
	for(l = &maps; (m = *l) != NULL; l = &m->next)
		if((char *)p >= m->p && (char *)p < m->p + m->len){
			if(unlink)
				*l = m->next;
			break;
		}
	unlock(&lk);
	return m;
}

int
munmap(void *p, size_t len)
{
	Map *m;
	int r;

	(void)len;
	m = find(p, 1);
	if(m == NULL){
		errno = EINVAL;
		return -1;
	}
	r = writeback(m, m->p, m->len);
	if(m->fd >= 0)
		close(m->fd);
	free(m->p);
	free(m);
	return r;
}

int
msync(void *p, size_t len, int flags)
{
	Map *m;
	size_t n;

	(void)flags;
	m = find(p, 0);
	if(m == NULL){
		errno = ENOMEM;
		return -1;
	}
	n = m->p + m->len - (char *)p;
	if(len < n)
		n = len;
	return writeback(m, p, n);
}

int
mprotect(void *p, size_t len, int prot)
{
	(void)p; (void)len;
	if(prot & PROT_EXEC){
		errno = EACCES;
		return -1;
	}
	return 0;
}

int
madvise(void *p, size_t len, int advice)
{
	(void)p; (void)len; (void)advice;
	return 0;
}

int
posix_madvise(void *p, size_t len, int advice)
{
	(void)p; (void)len; (void)advice;
	return 0;
}

/* POSIX shared memory objects as files in /tmp: no sharing between programs beyond that */
#include <fcntl.h>
#include <stdio.h>

static void
shmpath(char *buf, size_t n, const char *name)
{
	while(*name == '/')
		name++;
	snprintf(buf, n, "/tmp/shm.%s", name);
}

int
shm_open(const char *name, int flags, mode_t mode)
{
	char path[256];

	shmpath(path, sizeof path, name);
	return open(path, flags, mode);
}

int
shm_unlink(const char *name)
{
	char path[256];

	shmpath(path, sizeof path, name);
	return unlink(path);
}
