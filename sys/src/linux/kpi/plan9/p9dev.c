/*
 * SPDX-License-Identifier: MIT
 *
 * LinuxKPI for 9front: the device, the Plan 9 side (kpi/include/kpi/p9.h).
 * The PCI device from #G (gpu(3)): its ids, BARs as segments, its
 * interrupt; its configuration space from #$/pci; pinned memory as
 * #g segments (segment(3)) with their pages' physical addresses;
 * port I/O through #P.
 */
#include <u.h>
#include <libc.h>
#include "../include/kpi/p9.h"

static char gdir[64];
static int irqfd = -1;
static int ctlfd = -1;
static struct {
	void	*va;
	uvlong	pa, size;
} bars[6];

/*
 * the segments the KPI attaches (pool chunks, BARs): a proc that forked
 * before one was attached does not have it (rfork shares only the
 * segments there are then); its fault on it attaches it (p9_fault)
 */
typedef struct Seg Seg;
struct Seg {
	char	name[64];
	uvlong	va;
	uvlong	size;
};
static Seg segs[512];
static int nsegs;
static Lock segslk;

/*
 * our #g segments (the page pool's chunks, vmaps) go when the process
 * does: their directories removed at exit, a segment is freed once no
 * proc has it attached.  Pinned (sticky), they stayed in memory else:
 * 8 MB a start of aux/i915 -T, the guest out of memory after 18 (the
 * pager spinning in killbig); a crashed process leaves them to
 * p9_segclean at the next start
 */
static void
segexit(void)
{
	char path[96];
	int i;

	lock(&segslk);
	for(i = 0; i < nsegs; i++){
		snprint(path, sizeof path, "#g/%s", segs[i].name);
		remove(path);
	}
	unlock(&segslk);
}

/* the kpi.<pid>.* segments of processes that are gone (crashed) */
void
p9_segclean(void)
{
	char path[96], *e;
	Dir *d;
	long n, i, pid;
	int fd;

	if((fd = open("#g", OREAD)) < 0)
		return;
	n = dirreadall(fd, &d);
	close(fd);
	for(i = 0; i < n; i++){
		if(strncmp(d[i].name, "kpi.", 4) != 0)
			continue;
		pid = strtol(d[i].name + 4, &e, 10);
		if(e == d[i].name + 4 || *e != '.')
			continue;
		snprint(path, sizeof path, "/proc/%ld/status", pid);
		if(access(path, AEXIST) == 0)
			continue;	/* its process is there */
		snprint(path, sizeof path, "#g/%s", d[i].name);
		remove(path);
	}
	free(d);
}

static void
segadd(char *name, void *va, uvlong size)
{
	static int atexitdone;

	lock(&segslk);
	if(!atexitdone){
		atexitdone = 1;
		atexit(segexit);
	}
	if(nsegs < nelem(segs)){
		snprint(segs[nsegs].name, sizeof segs[nsegs].name, "%s", name);
		segs[nsegs].va = (uvlong)(uintptr)va;
		segs[nsegs].size = size;
		nsegs++;
	}else
		fprint(2, "p9dev: more than %d segments\n", nelem(segs));
	unlock(&segslk);
}

/* a note "sys: trap: fault read addr=0x..." on a KPI segment: attach it here, go on */
int
p9_fault(char *note)
{
	char *a, name[64];
	uvlong addr, va, size;
	int i;

	if(strncmp(note, "sys: trap: fault", 16) != 0 || (a = strstr(note, "addr=")) == nil)
		return 0;
	addr = strtoull(a + 5, nil, 0);
	va = size = 0;
	lock(&segslk);
	for(i = 0; i < nsegs; i++)
		if(addr >= segs[i].va && addr < segs[i].va + segs[i].size){
			strcpy(name, segs[i].name);
			va = segs[i].va;
			size = segs[i].size;
			break;
		}
	unlock(&segslk);
	if(i == nsegs)
		return 0;
	if(segattach(0, name, (void*)(uintptr)va, size) == (void*)-1)
		return 0;	/* attached already: a fault of its own */
	return 1;
}

static char*
ctlline(char *key, char *buf, int n)
{
	char *s, *e, path[96];
	int fd, l;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((fd = open(path, OREAD)) < 0)
		return nil;
	l = read(fd, buf, n-1);
	close(fd);
	if(l <= 0)
		return nil;
	buf[l] = 0;
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

int
p9_devopen(const char *dir)
{
	char buf[1024];
	char path[96];

	snprint(gdir, sizeof gdir, "%s", dir);
	if(ctlline("id", buf, sizeof buf) == nil)
		return -1;
	snprint(path, sizeof path, "%s/ctl", gdir);
	ctlfd = open(path, OWRITE);
	return 0;
}

int
p9_devid(unsigned int *vendor, unsigned int *device)
{
	char buf[1024], *s;

	if((s = ctlline("id", buf, sizeof buf)) == nil)
		return -1;
	*vendor = strtoul(s, &s, 16);
	if(*s++ != '/')
		return -1;
	*device = strtoul(s, nil, 16);
	return 0;
}

int
p9_pciaddr(int *bus, int *dev, int *fn)
{
	char buf[1024], *s;

	if((s = ctlline("pci", buf, sizeof buf)) == nil)
		return -1;
	*bus = strtol(s, &s, 10);
	if(*s++ != '.')
		return -1;
	*dev = strtol(s, &s, 10);
	if(*s++ != '.')
		return -1;
	*fn = strtol(s, nil, 10);
	return 0;
}

int
p9_pcicfg(int write, int off, void *buf, int n)
{
	char cfg[1024], *s, *f[2], path[96];
	int fd, r;

	if((s = ctlline("pci", cfg, sizeof cfg)) == nil || tokenize(s, f, nelem(f)) < 1)
		return -1;
	snprint(path, sizeof path, "#$/pci/%sraw", f[0]);
	if((fd = open(path, write ? ORDWR : OREAD)) < 0)
		return -1;
	r = write ? pwrite(fd, buf, n, off) : pread(fd, buf, n, off);
	close(fd);
	return r == n ? 0 : -1;
}

/*
 * physical memory outside the BARs that #G offers ("mem pa size seg":
 * an Intel GPU's OpRegion): the address the server sees pa at, or nil
 */
void*
p9_memmap(unsigned long long pa, unsigned long size)
{
	char buf[1024], path[96], *s, *e, *f[4];
	uvlong spa, ssize;
	uchar *va;
	int fd, n, i;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((fd = open(path, OREAD)) < 0)
		return nil;
	n = read(fd, buf, sizeof buf - 1);
	close(fd);
	if(n <= 0)
		return nil;
	buf[n] = 0;
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != nil)
			*e++ = 0;
		else
			e = s + strlen(s);
		if(strncmp(s, "mem ", 4) != 0 || tokenize(s, f, nelem(f)) < 4)
			continue;
		spa = strtoull(f[1], nil, 0);
		ssize = strtoull(f[2], nil, 0);
		if(pa < spa || pa + size > spa + ssize)
			continue;
		/* attached already (by this or another proc)? */
		lock(&segslk);
		for(i = 0; i < nsegs; i++)
			if(strcmp(segs[i].name, f[3]) == 0)
				break;
		va = i < nsegs ? (uchar*)(uintptr)segs[i].va : nil;
		unlock(&segslk);
		if(va == nil){
			if((va = segattach(0, f[3], nil, ssize)) == (void*)-1)
				return nil;
			segadd(f[3], va, ssize);
		}
		return va + (pa - spa);
	}
	return nil;
}

/* a BAR's address and size, and whether it is I/O ports; -1: none */
int
p9_barinfo(int bar, unsigned long long *pa, unsigned long long *size, int *io)
{
	char buf[1024], key[8], *s, *f[4];

	snprint(key, sizeof key, "bar%d", bar);
	if((s = ctlline(key, buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 3)
		return -1;
	*pa = strtoull(f[0], nil, 0);
	*size = strtoull(f[1], nil, 0);
	*io = strcmp(f[2], "-") == 0;
	return 0;
}

/* configuration space of any PCI device */
int
p9_pcicfgat(int bus, int dev, int fn, int write, int off, void *buf, int n)
{
	char path[96];
	int fd, r;

	snprint(path, sizeof path, "#$/pci/%d.%d.%draw", bus, dev, fn);
	if((fd = open(path, write ? ORDWR : OREAD)) < 0)
		return -1;
	r = write ? pwrite(fd, buf, n, off) : pread(fd, buf, n, off);
	close(fd);
	return r == n ? 0 : -1;
}

/* the PCI devices there are: fn(bus, dev, func, arg) each until it returns nonzero */
int
p9_pcieach(int (*fn)(int, int, int, void*), void *arg)
{
	Dir *d;
	long i, n;
	int fd, b, dv, f, r;
	char *s;

	if((fd = open("#$/pci", OREAD)) < 0)
		return -1;
	n = dirreadall(fd, &d);
	close(fd);
	r = 0;
	for(i = 0; i < n && r == 0; i++){
		s = d[i].name;
		if(strlen(s) < 4 || strcmp(s + strlen(s) - 3, "ctl") != 0)
			continue;
		b = strtol(s, &s, 10);
		if(*s++ != '.')
			continue;
		dv = strtol(s, &s, 10);
		if(*s++ != '.')
			continue;
		f = strtol(s, &s, 10);
		if(strcmp(s, "ctl") != 0)
			continue;
		r = fn(b, dv, f, arg);
	}
	free(d);
	return r;
}

/*
 * a memory BAR write-combining (the kernel's "bar N cached" makes a
 * write-back and a write-combining segment for it)
 */
void*
p9_barwc(int bar)
{
	char buf[1024], key[8], *s, *f[6];
	static void *wc[nelem(bars)];

	if(bar < 0 || bar >= nelem(bars))
		return nil;
	if(wc[bar] != nil)
		return wc[bar];
	if(ctlfd < 0 || fprint(ctlfd, "bar %d cached", bar) < 0)
		return nil;
	snprint(key, sizeof key, "bar%d", bar);
	if((s = ctlline(key, buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 5)
		return nil;
	if((wc[bar] = segattach(0, f[4], nil, strtoull(f[1], nil, 0))) == (void*)-1)
		wc[bar] = nil;
	else
		segadd(f[4], wc[bar], strtoull(f[1], nil, 0));
	return wc[bar];
}

/*
 * the name of a memory BAR's segment (gpu(3)) for a client: wc, its
 * write-combining one (the kernel makes it for "bar N cached"), else
 * the uncached one; 0, or -1
 */
int
p9_barseg(int bar, int wc, char *name, int n)
{
	char buf[1024], key[8], *s, *f[6];
	int nf;

	if(bar < 0 || bar >= nelem(bars))
		return -1;
	if(wc && p9_barwc(bar) == nil)
		return -1;
	snprint(key, sizeof key, "bar%d", bar);
	if((s = ctlline(key, buf, sizeof buf)) == nil || (nf = tokenize(s, f, nelem(f))) < 3)
		return -1;
	if(wc && nf < 5)
		return -1;
	snprint(name, n, "%s", f[wc ? 4 : 2]);
	return 0;
}

void*
p9_bar(int bar, unsigned long long *pa, unsigned long long *size)
{
	char buf[1024], key[8], *s, *f[4];

	if(bar < 0 || bar >= nelem(bars))
		return nil;
	if(bars[bar].va == nil){
		snprint(key, sizeof key, "bar%d", bar);
		if((s = ctlline(key, buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 3)
			return nil;
		bars[bar].pa = strtoull(f[0], nil, 0);
		bars[bar].size = strtoull(f[1], nil, 0);
		if(strcmp(f[2], "-") == 0)	/* an I/O bar */
			return nil;
		if((bars[bar].va = segattach(0, f[2], nil, bars[bar].size)) == (void*)-1){
			bars[bar].va = nil;
			return nil;
		}
		segadd(f[2], bars[bar].va, bars[bar].size);
	}
	if(pa)
		*pa = bars[bar].pa;
	if(size)
		*size = bars[bar].size;
	return bars[bar].va;
}

/*
 * the device's interrupt enabled (#G does it when its irq file is
 * opened): from now on an interrupt waits in #G for p9_irqwait.  Done by
 * request_irq itself; were it left to the irq proc, an interrupt before
 * that proc ran would be lost (amdgpu's ring then waits for its read
 * pointer to be written: RPTR_REARM)
 */
int
p9_irqopen(void)
{
	char path[96];

	if(irqfd >= 0)
		return 0;
	snprint(path, sizeof path, "%s/irq", gdir);
	if((irqfd = open(path, OREAD)) < 0)
		return -1;
	return 0;
}

int
p9_irqwait(void)
{
	char buf[32];

	if(p9_irqopen() < 0)
		return -1;
	if(read(irqfd, buf, sizeof buf) <= 0){
		/* 9front next generation: why the interrupts stop (the irq proc ends) */
		fprint(2, "kpi: %s/irq: %r\n", gdir);
		return -1;
	}
	return 0;
}

void
p9_irqack(void)
{
	if(ctlfd >= 0)
		fprint(ctlfd, "irq ack");
}

/* the caches of all processors written back (#G: x86 only) */
int
p9_wbinvd(void)
{
	if(ctlfd < 0 || fprint(ctlfd, "wbinvd") < 0)
		return -1;
	return 0;
}

/* the machine's memory and what is free, in bytes (#c/swap) */
int
p9_meminfo(unsigned long long *total, unsigned long long *free)
{
	char buf[1024], *f[4], *l, *e;
	uvlong pgsize, used, all;
	int fd, n;

	if((fd = open("/dev/swap", OREAD)) < 0)
		return -1;
	n = read(fd, buf, sizeof buf - 1);
	close(fd);
	if(n <= 0)
		return -1;
	buf[n] = 0;
	*total = *free = 0;
	pgsize = 4096;
	for(l = buf; *l; l = e){
		if((e = strchr(l, '\n')) != nil)
			*e++ = 0;
		else
			e = l + strlen(l);
		if(tokenize(l, f, nelem(f)) < 2)
			continue;
		if(strcmp(f[1], "memory") == 0)
			*total = strtoull(f[0], nil, 0);
		else if(strcmp(f[1], "pagesize") == 0)
			pgsize = strtoull(f[0], nil, 0);
		else if(strcmp(f[1], "user") == 0 && strchr(f[0], '/') != nil){
			used = strtoull(f[0], nil, 0);
			all = strtoull(strchr(f[0], '/') + 1, nil, 0);
			*free = (all - used) * pgsize;
		}
	}
	return *total ? 0 : -1;
}

/*
 * pinned memory: a #g segment, sticky (its pages stay where they are),
 * attached here at a virtual address of ours, 2 MB apart
 */
static Lock seglk;
static uvlong segnext = 0x4000000000ULL;

void*
p9_segalloc(const char *name, unsigned long size, unsigned long long *pa, int contig)
{
	char path[96];
	uchar *b;
	uvlong va;
	ulong i, n;
	void *p;
	int fd, j;

	size = (size + 4095) & ~4095UL;
	lock(&seglk);
	va = segnext;
	segnext += (size + (2<<20) - 1) & ~((2UL<<20) - 1);
	unlock(&seglk);
	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		return nil;
	if(fprint(fd, "va %#llux %#lux sticky%s", va, size, contig ? " contig" : "") < 0){
		close(fd);
		goto err;
	}
	close(fd);
	if((p = segattach(0, (char*)name, nil, size)) == (void*)-1)
		goto err;
	segadd((char*)name, p, size);
	if(pa != nil){
		n = size/4096;
		b = malloc(n*8);
		snprint(path, sizeof path, "#g/%s/pa", name);
		if(b == nil || (fd = open(path, OREAD)) < 0 || readn(fd, b, n*8) != n*8){
			free(b);
			segdetach(p);
			goto err;
		}
		close(fd);
		for(i = 0; i < n; i++){
			pa[i] = 0;
			for(j = 7; j >= 0; j--)
				pa[i] = pa[i]<<8 | b[i*8+j];
		}
		free(b);
	}
	return p;
err:
	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	return nil;
}

/*
 * an alias segment (segment(3)) of n pages taken from sticky segments:
 * page i is the page at offset off[i] of segment src[i]; its address,
 * attached here if attach (else only for others: a client's object)
 */
void*
p9_segalias(const char *name, int n, const char **src, unsigned long long *off, int attach)
{
	char path[96], *buf, *e, *b;
	uvlong va, size;
	void *p;
	int fd, i, j;

	size = (uvlong)n * 4096;
	lock(&seglk);
	va = segnext;
	segnext += (size + (2<<20) - 1) & ~((2ULL<<20) - 1);
	unlock(&seglk);
	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		goto err0;
	if(fprint(fd, "va %#llux %#llux alias", va, size) < 0){
		fprint(2, "%s: alias segment %s: va %#llux %#llux: %r\n", argv0 ? argv0 : "kpi", name, va, size);
		goto err;
	}
	/* a line per run of pages from one source, at most 4 KB */
	buf = malloc(4096);
	if(buf == nil)
		goto err;
	for(i = 0; i < n; i = j){
		e = buf + 4096;
		b = seprint(buf, e, "page %d %s", i, src[i]);
		for(j = i; j < n && strcmp(src[j], src[i]) == 0 && b < e - 32; j++)
			b = seprint(b, e, " %#llux", off[j]);
		if(write(fd, buf, b - buf) != b - buf){
			fprint(2, "%s: alias segment %s: %.*s: %r\n", argv0 ? argv0 : "kpi", name, (int)(b - buf), buf);
			free(buf);
			goto err;
		}
	}
	free(buf);
	close(fd);
	if(!attach)
		return (void*)(uintptr)va;
	if((p = segattach(0, (char*)name, nil, size)) == (void*)-1)
		goto err0;
	segadd((char*)name, p, size);
	return p;
err:
	close(fd);
err0:
	fprint(2, "%s: alias segment %s: %r\n", argv0 ? argv0 : "kpi", name);
	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	return nil;
}

/*
 * a segment showing size bytes at off of the physical segment phys
 * (segment(3)'s phys): a client's view of an object in a BAR, only of
 * that object, revoked when it is removed (p9_segfree); not attached
 * here
 */
void*
p9_segphys(const char *name, const char *phys, unsigned long long off, unsigned long long size)
{
	char path[96];
	uvlong va;
	int fd;

	size = (size + 4095) & ~4095ULL;
	lock(&seglk);
	va = segnext;
	segnext += (size + (2<<20) - 1) & ~((2ULL<<20) - 1);
	unlock(&seglk);
	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		goto err;
	if(fprint(fd, "va %#llux %#llux phys %s %#llux", va, size, phys, off) < 0){
		close(fd);
		goto err;
	}
	close(fd);
	return (void*)(uintptr)va;
err:
	fprint(2, "%s: segment %s of %s at %#llux: %r\n", argv0 ? argv0 : "kpi", name, phys, off);
	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	return nil;
}

/* a pager queue: a #g directory made "pager" (segment(3)) */
int
p9_pagerq(const char *name)
{
	char path[96];
	int fd;

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	if((fd = create(path, OREAD, DMDIR|0700)) < 0)
		return -1;
	close(fd);
	if(p9_segctl(name, "pager") < 0){
		remove(path);
		return -1;
	}
	return 0;
}

/*
 * a pager segment of size bytes of the physical segment phys: no pages
 * until the driver gives them (its faults on queue); not attached here
 */
void*
p9_segpager(const char *name, const char *phys, unsigned long long size, const char *queue)
{
	char path[96];
	uvlong va;
	int fd;

	size = (size + 4095) & ~4095ULL;
	lock(&seglk);
	va = segnext;
	segnext += (size + (2<<20) - 1) & ~((2ULL<<20) - 1);
	unlock(&seglk);
	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) >= 0)
		close(fd);
	if(p9_segctl(name, "va %#llux %#llux phys %s pager %s", va, size, phys, queue) < 0){
		fprint(2, "%s: pager segment %s of %s: %r\n", argv0 ? argv0 : "kpi", name, phys);
		remove(path);
		return nil;
	}
	return (void*)(uintptr)va;
}

int
p9_pagerqopen(const char *name)
{
	char path[96];

	snprint(path, sizeof path, "#g/%s/data", name);
	return open(path, OREAD);
}

long
p9_pagerqread(int fd, char *buf, long n)
{
	return read(fd, buf, n);
}

int
p9_segctl(const char *name, const char *fmt, ...)
{
	char path[96], *buf;
	va_list arg;
	int fd, n, r;

	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0)
		return -1;
	va_start(arg, fmt);
	buf = vsmprint((char*)fmt, arg);
	va_end(arg);
	if(buf == nil){
		close(fd);
		return -1;
	}
	n = strlen(buf);
	r = write(fd, buf, n) == n ? 0 : -1;
	free(buf);
	close(fd);
	return r;
}

void
p9_segfree(const char *name, void *va)
{
	char path[96];

	if(va != nil)
		segdetach(va);
	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
}

/* port I/O */
unsigned int
p9_in(int width, unsigned long port)
{
	static int fd[5] = { -1, -1, -1, -1, -1 };
	static char *name[5] = { nil, "#P/iob", "#P/iow", nil, "#P/iol" };
	uchar b[4];

	if(width != 1 && width != 2 && width != 4)
		return ~0;
	if(fd[width] < 0 && (fd[width] = open(name[width], ORDWR)) < 0)
		return ~0;
	if(pread(fd[width], b, width, port) != width)
		return ~0;
	return width == 1 ? b[0] : width == 2 ? b[0] | b[1]<<8 : b[0] | b[1]<<8 | b[2]<<16 | (uint)b[3]<<24;
}

void
p9_out(int width, unsigned long port, unsigned int v)
{
	static int fd[5] = { -1, -1, -1, -1, -1 };
	static char *name[5] = { nil, "#P/iob", "#P/iow", nil, "#P/iol" };
	uchar b[4];

	if(width != 1 && width != 2 && width != 4)
		return;
	if(fd[width] < 0 && (fd[width] = open(name[width], ORDWR)) < 0)
		return;
	b[0] = v;
	b[1] = v>>8;
	b[2] = v>>16;
	b[3] = v>>24;
	pwrite(fd[width], b, width, port);
}

/* a model specific register (#P/msr: an 8-byte read at its number) */
int
p9_rdmsr(unsigned int msr, unsigned long long *v)
{
	static int fd = -1;
	uvlong x;

	if(fd < 0 && (fd = open("#P/msr", OREAD)) < 0)
		return -1;
	if(pread(fd, &x, 8, msr) != 8)
		return -1;
	*v = x;
	return 0;
}
