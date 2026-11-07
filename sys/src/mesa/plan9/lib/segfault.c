/*
 * SPDX-License-Identifier: MIT
 *
 * The segments of the GPU's objects (libdrm9: drm9.c, amdgpu9.c) in
 * every thread of a program.  A thread here is a proc sharing memory
 * (APE's pthreads, rfork RFMEM), and a segment attached after the fork
 * is only in the proc that attached it: zink records on its own thread
 * and wrote to a command buffer RADV had mapped on the program's, a
 * fault ("sys: trap: fault write addr=...") that killed it.  The
 * segments are attached here and kept in a list; a fault on one of them
 * in another proc attaches it there, at the same address (SIGSEGV's
 * handler; APE gives it the note's text and goes on at the faulting
 * instruction when it returns).  What that proc still had there, a
 * segment freed since, is detached first.  Any other fault is the
 * program's: its own handler, or the default (the note kills it).
 */
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

extern void	*_SEGATTACH(int, char*, void*, unsigned long);
extern int	_SEGDETACH(void*);
extern int	_CREATE(char*, int, unsigned long);
extern int	_OPEN(const char*, int);
extern long	_PREAD(int, void*, long, long long);
extern long	_PWRITE(int, void*, long, long long);
extern int	_CLOSE(int);

void	*mesa9_segattach(const char *name, void *at, unsigned long long size);
void	*mesa9_sharedmem(unsigned long long size, int *fdp);
void	mesa9_sharedfree(void *p, int fd);
int	mesa9_segdetach(void *va);

enum { Nseg = 4096 };

typedef struct Seg Seg;
struct Seg {
	uintptr_t	va;
	unsigned long long	size;
	char	name[64];
};

static Seg segs[Nseg];
static volatile int seglk;
static void (*oldsegv)(int);
static int handling;

static void
lock(void)
{
	while(__sync_lock_test_and_set(&seglk, 1))
		;
}

static void
unlock(void)
{
	__sync_lock_release(&seglk);
}

/* APE calls a handler as f(sig, note, ureg) */
static void
segv(int sig, char *note, void *ureg)
{
	char name[64], *a;
	uintptr_t addr, va;
	unsigned long long size;
	void (*old)(int);
	int i;

	va = 0;
	size = 0;
	if(note != NULL && (a = strstr(note, "addr=")) != NULL){
		addr = strtoull(a + 5, NULL, 0);
		lock();
		for(i = 0; i < Nseg; i++)
			if(segs[i].size != 0 && addr >= segs[i].va && addr < segs[i].va + segs[i].size){
				va = segs[i].va;
				size = segs[i].size;
				strcpy(name, segs[i].name);
				break;
			}
		unlock();
		if(size != 0){
			if(_SEGATTACH(0, name, (void*)va, size) != (void*)-1)
				return;	/* attached here: the access again */
			/* a segment freed since, still attached here: away, and again */
			if(_SEGDETACH((void*)va) == 0 && _SEGATTACH(0, name, (void*)va, size) != (void*)-1)
				return;
		}
	}
	/* not one of ours (or attached already: a fault of its own) */
	old = oldsegv;
	if(old != SIG_DFL && old != SIG_IGN && old != SIG_ERR && old != NULL){
		((void (*)(int, char*, void*))old)(sig, note, ureg);
		return;
	}
	signal(SIGSEGV, SIG_DFL);	/* the access again: the default, the note kills */
}

static void
install(void)
{
	struct sigaction sa, osa;

	if(handling)
		return;
	handling = 1;
	memset(&sa, 0, sizeof sa);
	sa.sa_handler = (void (*)(int))segv;
	if(sigaction(SIGSEGV, &sa, &osa) == 0)
		oldsegv = osa.sa_handler;
}

/*
 * the segment name, size bytes (at at, or where it fits): attached here,
 * and in another proc of the program when it touches it
 */
void*
mesa9_segattach(const char *name, void *at, unsigned long long size)
{
	void *p;
	int i;

	p = _SEGATTACH(0, (char*)name, at, size);
	if(p == (void*)-1 || size == 0)
		return p;
	lock();
	install();
	for(i = 0; i < Nseg; i++)
		if(segs[i].size == 0){
			segs[i].va = (uintptr_t)p;
			segs[i].size = size;
			strncpy(segs[i].name, name, sizeof segs[i].name - 1);
			segs[i].name[sizeof segs[i].name - 1] = 0;
			break;
		}
	unlock();
	return p;
}

/* detached here; other procs keep theirs until the address is used again (segv) */
int
mesa9_segdetach(void *va)
{
	int i;

	lock();
	for(i = 0; i < Nseg; i++)
		if(segs[i].size != 0 && segs[i].va == (uintptr_t)va){
			segs[i].size = 0;
			break;
		}
	unlock();
	return _SEGDETACH(va);
}

/*
 * size bytes of the program's memory that every proc of it sees at the
 * same address (what Linux's anonymous files mapped shared are to Mesa,
 * hasvk's state table): a segment of segment(3) named in #g, at an
 * address the kernel chooses ("va 0 len"), attached as mesa9_segattach
 * does, so that a proc that touches it gets it too (an anonymous
 * "shared" segment would be new memory in each).  *fdp holds its
 * directory, opened ORCLOSE: the segment goes with mesa9_sharedfree, or
 * with the program.
 */
enum { OREAD9 = 0, OWRITE9 = 1, ORCLOSE9 = 64, DMDIR9 = 0x80000000 };

void*
mesa9_sharedmem(unsigned long long size, int *fdp)
{
	static int n;
	char name[64], path[96], buf[128], *e;
	unsigned long long va;
	int fd, cfd, m;
	void *p;

	lock();
	snprintf(name, sizeof name, "mesa.%d.%d", getpid(), n++);
	unlock();
	snprintf(path, sizeof path, "#g/%s", name);
	if((fd = _CREATE(path, OREAD9|ORCLOSE9, DMDIR9|0600)) < 0)
		return (void*)-1;
	snprintf(path, sizeof path, "#g/%s/ctl", name);
	if((cfd = _OPEN(path, OWRITE9)) < 0)
		goto fail;
	m = snprintf(buf, sizeof buf, "va 0 %llu", size);
	if(_PWRITE(cfd, buf, m, 0) != m){
		_CLOSE(cfd);
		goto fail;
	}
	_CLOSE(cfd);
	if((cfd = _OPEN(path, OREAD9)) < 0)
		goto fail;
	m = _PREAD(cfd, buf, sizeof buf - 1, 0);
	_CLOSE(cfd);
	if(m <= 3 || strncmp(buf, "va ", 3) != 0)
		goto fail;
	buf[m] = 0;
	va = strtoull(buf + 3, &e, 0);
	if((p = mesa9_segattach(name, (void*)(uintptr_t)va, size)) == (void*)-1)
		goto fail;
	*fdp = fd;
	return p;
fail:
	_CLOSE(fd);
	return (void*)-1;
}

void
mesa9_sharedfree(void *p, int fd)
{
	mesa9_segdetach(p);
	_CLOSE(fd);
}
