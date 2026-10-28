/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: segments (#g), files, the CPU's caches, delays.
 * Plan 9 only: no counterpart in Linux.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * a frame buffer of the driver's (display.c: an output's picture): pinned
 * pages, attached here, in the GTT Scanalign-aligned; its GTT address,
 * 0 if there is none
 */
static int nfb;

char*
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, OREAD)) < 0)
		return nil;
	m = readn(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return nil;
	buf[m] = 0;
	return buf;
}

char*
ctlline(char *key, char *buf, int n)
{
	char *s, *e, path[64];
	int l;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if(readfile(path, buf, n) == nil)
		sysfatal("read %s: %r", path);
	l = strlen(key);
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != nil)
			*e++ = 0;
		else
			e = s+strlen(s);
		if(strncmp(s, key, l) == 0 && s[l] == ' ')
			return s+l+1;
	}
	return nil;
}

uvlong
fixedseg(char *name, uvlong va, ulong size, int perm)
{
	char buf[256], path[64], *f[6];
	int fd, n;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|perm)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, ORDWR)) < 0)
		return 0;
	fprint(fd, "va %#llux %#lux fixed", va, size);
	n = pread(fd, buf, sizeof buf-1, 0);
	close(fd);
	if(n <= 0)
		return 0;
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 5 || strcmp(f[3], "fixed") != 0){
		werrstr("%s: not a fixed segment: %s", name, buf);
		return 0;
	}
	return strtoull(f[4], nil, 0);
}

/*
 * a segment of pinned pages, not contiguous: their physical
 * addresses from #g/name/pa, one 64-bit word per page
 */
uvlong*
stickyseg(char *name, uvlong va, ulong size, int perm)
{
	char path[64];
	uchar *b;
	uvlong *pa;
	ulong i, n;
	int fd, j;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|perm)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		return nil;
	if(fprint(fd, "va %#llux %#lux sticky", va, size) < 0){
		close(fd);
		return nil;
	}
	close(fd);
	n = size/4096;
	b = emalloc9p(n*8);
	pa = emalloc9p(n*sizeof pa[0]);
	snprint(path, sizeof path, "#g/%s/pa", name);
	if((fd = open(path, OREAD)) < 0 || readn(fd, b, n*8) != n*8){
		if(fd >= 0)
			close(fd);
		free(b);
		free(pa);
		return nil;
	}
	close(fd);
	for(i = 0; i < n; i++){
		pa[i] = 0;
		for(j = 7; j >= 0; j--)
			pa[i] = pa[i]<<8 | b[i*8+j];
	}
	free(b);
	return pa;
}

void
rmseg(char *name)
{
	char path[64];

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
}

/* the CPU's caches out of the way of an uncached GPU (-U) */
void
flushcache(void *p, ulong n)
{
	uchar *a, *e;

	if(!uncached)
		return;
	a = (uchar*)((uintptr)p & ~63);
	e = (uchar*)p+n;
#if defined(__x86_64__) || defined(__i386__)
	for(; a < e; a += 64)
		__asm__ volatile("clflush (%0)" :: "r"(a) : "memory");
	__asm__ volatile("mfence" ::: "memory");
#else
	USED(a, e);
#endif
}

/* the CPU's caches written back to memory: for what the display reads */
void
clflushrange(void *p, ulong n)
{
	uchar *a, *e;

	a = (uchar*)((uintptr)p & ~63);
	e = (uchar*)p + n;
	for(; a < e; a += 64)
		__asm__ volatile("clflush (%0)" :: "r"(a) : "memory");
	__asm__ volatile("mfence" ::: "memory");
}

ulong
fbnew(ulong size, u32int **p, char *seg, int nseg)
{
	uvlong *pa;
	ulong a;

	size = (size + 4095) & ~4095UL;
	snprint(seg, nseg, "gpu0fb%d", ++nfb);
	if((pa = stickyseg(seg, Fbva + (uvlong)(nfb-1)*Bomax, size, 0700)) == nil)
		return 0;
	if((*p = segattach(0, seg, nil, size)) == (void*)-1){
		free(pa);
		rmseg(seg);
		return 0;
	}
	qlock(&lk);
	if((a = gttalloc(size + Scanalign)) == 0){
		qunlock(&lk);
		segdetach(*p);
		free(pa);
		rmseg(seg);
		return 0;
	}
	a = (a + Scanalign-1) & ~(Scanalign-1);
	gen8_ggtt_insert_entries(a, 0, pa, size);
	qunlock(&lk);
	free(pa);
	return a;
}

void
microwait(int us)
{
	vlong t;

	for(t = nsec(); nsec() - t < us*1000LL; )
		sleep(0);
}
