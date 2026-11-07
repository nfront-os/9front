/*
 * GCC port: mmap for APE, which has none (LLVM maps the files it reads and writes): the
 * memory is from posix_memalign (page aligned), a file's part read into it; a shared, writable
 * mapping goes back to the file at msync and munmap. No sharing between procs beyond the
 * program's own memory, no protection, no executable memory (PROT_EXEC is refused).
 * 9front-ng: except shared memory objects (memfd_create, shm_open): segments of
 * segment(3) at an address the kernel chooses, attached for MAP_SHARED: the same
 * memory in every process that maps one (below, after shm_open).
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
#include <stdint.h>


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

typedef struct Shm Shm;
static void *shmmap(int, size_t, off_t, int);
static int shmunmap(void *);
static Shm *shmfind(void *);
static int shmdata(int);

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
	int rfd;

	(void)addr;
	if(!(flags & MAP_ANONYMOUS) && (flags & MAP_SHARED) && (p = shmmap(fd, len, off, (prot & PROT_WRITE) != 0)) != NULL)
		return p;
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
		/* a private mapping of a shared memory object: a copy of its bytes */
		rfd = shmdata(fd);
		for(done = 0; done < len; done += r){
			r = pread(rfd >= 0 ? rfd : fd, p + done, len - done, off + done);
			if(r < 0){
				if(rfd >= 0)
					close(rfd);
				free(p);
				free(m);
				return MAP_FAILED;
			}
			if(r == 0)
				break;	/* beyond the end: zeros */
		}
		if(rfd >= 0)
			close(rfd);
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
	if(shmunmap(p))
		return 0;
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
	if(shmfind(p) != NULL)
		return 0;	/* shared memory: nothing to write back */
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

/*
 * 9front-ng (../9front-ui PLAN §12): shared memory objects are segments of
 * segment(3) whose address the kernel chooses ("va 0 len"), so that any
 * process attaches one at that address and sees the same memory (Wayland's
 * wl_shm: a client's pool in the compositor).  The object's descriptor is
 * its directory in #g: memfd_create's is opened ORCLOSE (gone with its last
 * descriptor, in whatever process has it), shm_open's is named until
 * shm_unlink.  ftruncate (__shmtruncate) gives it its address and size, on a
 * 64-bit machine 256 MB of addresses (pages only where touched) so that it
 * can grow; mmap(MAP_SHARED) attaches it, munmap of the last such mapping
 * detaches it; a MAP_PRIVATE mapping is a copy of its bytes (its data file).
 */
#include <fcntl.h>
#include <stdio.h>
#include "../plan9/lib.h"
#include "../plan9/sys9.h"

enum {
	Shmreserve = sizeof(void *) == 8 ? 256*1024*1024 : 0,
	OREAD9 = 0,
	OWRITE9 = 1,
	ORCLOSE9 = 64,
	DMDIR9 = 0x80000000,
};

struct Shm {
	char	name[128];
	char	*base;
	size_t	len;
	int	ref;	/* mmaps of it */
	Shm	*next;
};

static Shm *shms;
static int memfdn;

/*
 * A memfd's seals (Linux's F_ADD_SEALS, F_GET_SEALS) and its size as
 * ftruncate gave it (the segment reserves more addresses).  Kept in the
 * process: another that got the descriptor (SCM_RIGHTS) sees none.  One
 * made without MFD_ALLOW_SEALING is sealed against seals.
 */
typedef struct Seal Seal;
struct Seal {
	char	name[128];
	int	seals;
	off_t	size;
	Seal	*next;
};
static Seal *seals;

/* with lk */
static Seal *
sealof(char *name)
{
	Seal *s;

	for(s = seals; s != NULL; s = s->next)
		if(strcmp(s->name, name) == 0)
			return s;
	return NULL;
}

/* the segment fd is the directory of, if it is one */
static int
shmname(int fd, char *name, int n)
{
	char path[256];

	if(fd < 0 || _FD2PATH(fd, path, sizeof path) < 0)
		return 0;
	if(strncmp(path, "#g/", 3) != 0 || path[3] == 0 || strchr(path+3, '/') != NULL)
		return 0;
	snprintf(name, n, "%s", path+3);
	return 1;
}

/* its data file, opened for reading, if fd is a segment's; else -1 */
static int
shmdata(int fd)
{
	char name[128], path[160];

	if(!shmname(fd, name, sizeof name))
		return -1;
	snprintf(path, sizeof path, "#g/%s/data", name);
	return open(path, O_RDONLY);
}

/* its address and size, if it has them yet */
static int
shmva(char *name, uintptr_t *va, size_t *len)
{
	char path[160], buf[128], *e;
	int fd, n;

	snprintf(path, sizeof path, "#g/%s/ctl", name);
	if((fd = _OPEN(path, OREAD9)) < 0)
		return 0;
	n = _PREAD(fd, buf, sizeof buf-1, 0);
	_CLOSE(fd);
	if(n <= 3 || strncmp(buf, "va ", 3) != 0)
		return 0;
	buf[n] = 0;
	*va = strtoull(buf+3, &e, 0);
	*len = strtoull(e, NULL, 0);
	return 1;
}

static Shm *
shmfind(void *p)
{
	Shm *s;

	lock(&lk);
	for(s = shms; s != NULL; s = s->next)
		if((char *)p >= s->base && (char *)p < s->base + s->len)
			break;
	unlock(&lk);
	return s;
}

/* the size ftruncate gave a memfd */
static void
sealsize(char *name, off_t length)
{
	Seal *sl;

	lock(&lk);
	if((sl = sealof(name)) != NULL)
		sl->size = length;
	unlock(&lk);
}

/* fcntl's F_ADD_SEALS and F_GET_SEALS (plan9/fcntl.c): the seals, or -1 */
int
__shmseals(int fd, int cmd, int arg)
{
	char name[128];
	Seal *sl;
	int r;

	if(!shmname(fd, name, sizeof name)){
		errno = EINVAL;	/* not a memfd */
		return -1;
	}
	lock(&lk);
	if((sl = sealof(name)) == NULL){
		unlock(&lk);
		errno = EINVAL;	/* shm_open's, or one of another process */
		return -1;
	}
	if(cmd == F_GET_SEALS){
		r = sl->seals;
		unlock(&lk);
		return r;
	}
	if(sl->seals & F_SEAL_SEAL){
		unlock(&lk);
		errno = EPERM;
		return -1;
	}
	sl->seals |= arg;
	unlock(&lk);
	return 0;
}

/* ftruncate of a segment's descriptor (-2: not one) */
int
__shmtruncate(int fd, off_t length)
{
	char name[128], path[160], buf[64];
	uintptr_t va;
	size_t len, size;
	int cfd, n;

	if(!shmname(fd, name, sizeof name))
		return -2;
	{
		Seal *sl;
		int e;

		e = 0;
		lock(&lk);
		if((sl = sealof(name)) != NULL){
			if((length < sl->size && (sl->seals & F_SEAL_SHRINK)) ||
			   (length > sl->size && (sl->seals & F_SEAL_GROW)))
				e = EPERM;
		}
		unlock(&lk);
		if(e){
			errno = e;
			return -1;
		}
	}
	if(shmva(name, &va, &len)){
		if((size_t)length <= len){
			sealsize(name, length);
			return 0;
		}
		errno = EFBIG;	/* beyond what it reserved */
		return -1;
	}
	if(length == 0)
		return 0;
	size = length;
	if(size < Shmreserve)
		size = Shmreserve;
	snprintf(path, sizeof path, "#g/%s/ctl", name);
	if((cfd = _OPEN(path, OWRITE9)) < 0){
		_syserrno();
		return -1;
	}
	n = snprintf(buf, sizeof buf, "va 0 %llu", (unsigned long long)size);
	if(_PWRITE(cfd, buf, n, 0) != n){
		_syserrno();
		_CLOSE(cfd);
		return -1;
	}
	_CLOSE(cfd);
	sealsize(name, length);
	return 0;
}

static void *
shmmap(int fd, size_t len, off_t off, int writable)
{
	char name[128];
	uintptr_t va;
	size_t slen;
	Shm *s;
	void *p;

	if(!shmname(fd, name, sizeof name))
		return NULL;
	if(writable){
		Seal *sl;
		int sealed;

		lock(&lk);
		sealed = (sl = sealof(name)) != NULL && (sl->seals & (F_SEAL_WRITE|F_SEAL_FUTURE_WRITE));
		unlock(&lk);
		if(sealed){
			errno = EPERM;
			return MAP_FAILED;
		}
	}
	if(!shmva(name, &va, &slen) || off < 0 || (size_t)off > slen || len > slen - off){
		errno = ENXIO;	/* no size yet (ftruncate first), or beyond it */
		return MAP_FAILED;
	}
	lock(&lk);
	for(s = shms; s != NULL; s = s->next)
		if(strcmp(s->name, name) == 0){
			s->ref++;
			unlock(&lk);
			return s->base + off;
		}
	unlock(&lk);
	if((s = malloc(sizeof *s)) == NULL){
		errno = ENOMEM;
		return MAP_FAILED;
	}
	p = _SEGATTACH(0, name, NULL, 0);
	if(p == (void *)-1){
		_syserrno();
		free(s);
		return MAP_FAILED;
	}
	snprintf(s->name, sizeof s->name, "%s", name);
	s->base = p;
	s->len = slen;
	s->ref = 1;
	lock(&lk);
	s->next = shms;
	shms = s;
	unlock(&lk);
	return s->base + off;
}

/* munmap of a segment's mapping: 1 if it was one */
static int
shmunmap(void *p)
{
	Shm *s, **l;

	lock(&lk);
	for(l = &shms; (s = *l) != NULL; l = &s->next)
		if((char *)p >= s->base && (char *)p < s->base + s->len)
			break;
	if(s == NULL){
		unlock(&lk);
		return 0;
	}
	if(--s->ref > 0){
		unlock(&lk);
		return 1;
	}
	*l = s->next;
	unlock(&lk);
	_SEGDETACH(s->base);
	free(s);
	return 1;
}

/* a segment's directory as a descriptor APE knows */
static int
shmfd(int fd, int cloexec)
{
	Fdinfo *fi;

	if(fd < 0){
		_syserrno();
		return -1;
	}
	if(fd >= OPEN_MAX){
		_CLOSE(fd);
		errno = EMFILE;
		return -1;
	}
	fi = &_fdinfo[fd];
	memset(fi, 0, sizeof *fi);
	fi->flags = FD_ISOPEN | (cloexec ? FD_CLOEXEC : 0);
	fi->oflags = O_RDWR;
	fi->uid = -2;
	fi->gid = -2;
	return fd;
}

int
memfd_create(const char *name, unsigned int flags)
{
	char path[128];
	Seal *sl;
	int n, fd;

	(void)name;
	lock(&lk);
	n = memfdn++;
	unlock(&lk);
	snprintf(path, sizeof path, "#g/ape.memfd.%d.%d", getpid(), n);
	fd = shmfd(_CREATE(path, OREAD9|ORCLOSE9, DMDIR9|0600), flags & MFD_CLOEXEC);
	if(fd >= 0 && (sl = malloc(sizeof *sl)) != NULL){
		snprintf(sl->name, sizeof sl->name, "ape.memfd.%d.%d", getpid(), n);
		sl->seals = flags & (MFD_ALLOW_SEALING|MFD_NOEXEC_SEAL) ? 0 : F_SEAL_SEAL;
		sl->size = 0;
		lock(&lk);
		sl->next = seals;
		seals = sl;
		unlock(&lk);
	}
	return fd;
}

static void
shmpath(char *buf, size_t n, const char *name)
{
	while(*name == '/')
		name++;
	snprintf(buf, n, "#g/ape.shm.%s", name);
}

int
shm_open(const char *name, int flags, mode_t mode)
{
	char path[256];
	int fd;

	shmpath(path, sizeof path, name);
	fd = _OPEN(path, OREAD9);
	if(fd >= 0 && (flags & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL)){
		_CLOSE(fd);
		errno = EEXIST;
		return -1;
	}
	if(fd < 0 && (flags & O_CREAT))
		fd = _CREATE(path, OREAD9, DMDIR9|(mode&0777));
	return shmfd(fd, 1);
}

int
shm_unlink(const char *name)
{
	char path[256];

	shmpath(path, sizeof path, name);
	if(_REMOVE(path) < 0){
		_syserrno();
		return -1;
	}
	return 0;
}
