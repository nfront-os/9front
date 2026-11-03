/*
 * SPDX-License-Identifier: MIT
 *
 * libdrm on 9front: the DRM interface Mesa's drivers made for Linux
 * use (xf86drm.h, the i915 ioctls), over gpu(4).
 *
 * A device is /dev/gpu/N served by a driver for it (aux/intelgpu); its
 * render node is /dev/gpu/N/clone, so that every open is a context of
 * its own, as a DRM file descriptor with its GEM objects and address
 * space is. The file descriptor is the context's ctl file; the first
 * ioctl on it finds the context (ctl's reply, ctx n) and opens its
 * submit and fence files.
 *
 *	GEM objects	bo size 0, segattach; mmap: the segment
 *	softpin		bind (the object's canonical address, 48 bits)
 *	execbuffer2	submit bo offset rcs|bcs, one fence per batch
 *	waits		the fence file: wait k, then read (blocks)
 *	syncobjs	in the process: binary ones hold a fence, timeline
 *			ones points (value, fence); a fence is a context's
 *			batch k, or none (signalled)
 *	dma-bufs, sync files	pipes standing for an object or a fence
 *
 * Getparams answer what the driver says (status: sseu) or what i915
 * answers on gen 8; queries (topology, engines, memory regions) fail,
 * and Mesa falls back to getparams, as with older kernels.
 *
 * AMD's GPUs (aux/amdgpu) are amdgpu9.c's: their ioctls, syncobjs and
 * version go to Linux's amdgpu itself, through the context's arena.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <xf86drm.h>
#include <drm-uapi/i915_drm.h>
#include <drm-uapi/dma-buf.h>

#include "amdgpu9.h"

/* as Mesa's build has it (meson: -DETIME=ETIMEDOUT) */
#ifndef ETIME
#define ETIME	ETIMEDOUT
#endif

/* libmesaplan9 (lib/segfault.c): attached here and in the program's other procs when they touch it */
extern void	*mesa9_segattach(const char*, void*, unsigned long long);
extern int	mesa9_segdetach(void*);
extern int	_ERRSTR(char*, unsigned int);
extern int	_FD2PATH(int, char*, int);

enum {
	Maxdev	= 8,		/* /dev/gpu/N */
	Maxfd	= 256,		/* descriptors we know */
	Ncpts	= 64,		/* points of a timeline syncobj kept */
	Cs_hz	= 12500000,	/* gen 8's command streamer timestamp */
};

typedef struct Dev Dev;
typedef struct Fence Fence;
typedef struct Gem Gem;
typedef struct Sync Sync;
typedef struct Fd Fd;
typedef struct Gpu Gpu;

/* a device as /dev/gpu/N says */
struct Gpu {
	int	n;
	char	dir[32];
	uint16_t	vendor, device, subvendor, subdevice;
	uint8_t	revision;
	uint8_t	bus, dev, func;
	int	slicemask, subslicemask, eutotal;
	uint64_t	aperture;
	int	amd;		/* aux/amdgpu: amdgpu9.c */
};

/* a context's batch k; ctx nil: none, signalled */
struct Fence {
	Dev	*ctx;
	uint64_t	k;
};

struct Gem {
	int	used;
	int	id;		/* the driver's */
	char	seg[32];
	uint64_t	va, size;
	char	*p;		/* attached */
	uint64_t	gpu;		/* bound there, 0: not */
	Fence	last;		/* the last batch that used it */
	uint32_t	tiling, stride, caching;
};

struct Sync {
	int	used;
	int	timeline;
	Fence	f;		/* binary: the fence, f.k == 0 && !f.ctx: none */
	int	hasf;
	uint64_t	payload;	/* timeline: signalled up to */
	int	np;
	uint64_t	pv[Ncpts];	/* pending points, ascending */
	Fence	pf[Ncpts];
};

/* a DRM descriptor: a context */
struct Dev {
	Gpu	*gpu;
	int	fd;
	uint64_t	ino;	/* the qid of its ctl, unique */
	int	ctx;		/* the driver's number */
	int	submitfd, statfd;
	uint64_t	done;		/* fences done, as last read */
	int	failed;
	int	nfence;
	Gem	*gem;
	int	ngem;
	Sync	*sync;
	int	nsync;
	Amd	*amd;		/* an AMD GPU's context (amdgpu9.c) */
};

enum { Knone, Kdev, Kdmabuf, Ksyncfile };

/* what a descriptor stands for (pipes: dma-bufs and sync files, by their qid) */
struct Fd {
	int	kind;
	Dev	*dev;
	uint64_t	ino;
	uint32_t	handle;	/* dma-buf: the GEM object */
	Fence	f;		/* sync file */
	int	hasf;
};

static pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
static Gpu gpus[Maxdev];
static int ngpu = -1;
static Fd fds[Maxfd];

static int
seterr(int e)
{
	errno = e;
	return -1;
}

/* with $DRM9DEBUG set: why a call fails, on standard error */
static void
dbg(char *fmt, ...)
{
	static int on = -1;
	va_list arg;
	int e;

	e = errno;
	if(on < 0)
		on = getenv("DRM9DEBUG") != NULL;
	if(on){
		fprintf(stderr, "drm9: ");
		va_start(arg, fmt);
		vfprintf(stderr, fmt, arg);
		va_end(arg);
		fprintf(stderr, "\n");
	}
	errno = e;
}

/* the system's error string (the driver's reply to a failed write) */
static char*
syserr(void)
{
	static char buf[128];

	buf[0] = 0;
	_ERRSTR(buf, sizeof buf);
	_ERRSTR(buf, sizeof buf);	/* put it back */
	return buf;
}

static int
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, O_RDONLY)) < 0)
		return -1;
	m = read(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return -1;
	buf[m] = 0;
	return m;
}

/* the line of a file that starts with key, after it */
static char*
keyline(char *buf, char *key)
{
	char *p;
	int n;

	n = strlen(key);
	for(p = buf; p != NULL && *p != 0; p = strchr(p, '\n') ? strchr(p, '\n')+1 : NULL)
		if(strncmp(p, key, n) == 0 && (p[n] == ' ' || p[n] == '\t'))
			return p + n + 1;
	return NULL;
}

/* the devices: /dev/gpu/N whose driver says device intel vvvv/dddd */
static void
scan(void)
{
	char buf[4096], path[64], *s;
	unsigned v, d, b, dv, fn;
	unsigned char raw[64];
	Gpu *g;
	int i, fd;

	if(ngpu >= 0)
		return;
	ngpu = 0;
	for(i = 0; i < Maxdev; i++){
		snprintf(path, sizeof path, "/dev/gpu/%d/status", i);
		if(readfile(path, buf, sizeof buf) < 0)
			continue;
		if((s = keyline(buf, "device")) == NULL || (strncmp(s, "intel ", 6) != 0 && strncmp(s, "amd ", 4) != 0))
			continue;
		if(sscanf(s + (s[0] == 'a' ? 4 : 6), "%x/%x", &v, &d) != 2)
			continue;
		g = &gpus[ngpu];
		memset(g, 0, sizeof *g);
		g->amd = s[0] == 'a';
		g->n = i;
		snprintf(g->dir, sizeof g->dir, "/dev/gpu/%d", i);
		g->vendor = v;
		g->device = d;
		g->slicemask = 1;
		g->subslicemask = 7;
		g->eutotal = 24;
		if((s = keyline(buf, "sseu")) != NULL)
			sscanf(s, "slices %i subslices %i eus %i", &g->slicemask, &g->subslicemask, &g->eutotal);
		g->aperture = 256ULL<<20;
		/*
		 * the PCI address and revision: the driver's status (every
		 * user may read it), else #G/N/ctl and #$/pci (the hostowner's)
		 */
		if((s = keyline(buf, "pci")) != NULL){
			unsigned rev, sv, sd;
			unsigned long long ap;
			char *t;

			if(sscanf(s, "%u.%u.%u rev %x subsys %x/%x", &b, &dv, &fn, &rev, &sv, &sd) == 6){
				g->bus = b;
				g->dev = dv;
				g->func = fn;
				g->revision = rev;
				g->subvendor = sv;
				g->subdevice = sd;
				if((t = strstr(s, "aperture ")) != NULL && sscanf(t + 9, "%llu", &ap) == 1 && ap != 0)
					g->aperture = ap;
				ngpu++;
				continue;
			}
		}
		snprintf(path, sizeof path, "#G/%d/ctl", i);
		if(readfile(path, buf, sizeof buf) >= 0){
			if((s = keyline(buf, "pci")) != NULL && sscanf(s, "%u.%u.%u", &b, &dv, &fn) == 3){
				g->bus = b;
				g->dev = dv;
				g->func = fn;
				snprintf(path, sizeof path, "#$/pci/%u.%u.%uraw", b, dv, fn);
				if((fd = open(path, O_RDONLY)) >= 0){
					if(pread(fd, raw, sizeof raw, 0) == sizeof raw){
						g->revision = raw[8];
						g->subvendor = raw[0x2c] | raw[0x2d]<<8;
						g->subdevice = raw[0x2e] | raw[0x2f]<<8;
					}
					close(fd);
				}
			}
			if((s = keyline(buf, "bar2")) != NULL){
				unsigned long long pa, len;
				if(sscanf(s, "%lli %lli", &pa, &len) == 2)
					g->aperture = len;
			}
		}
		ngpu++;
	}
}

static uint64_t
ino(int fd)
{
	struct stat st;

	if(fstat(fd, &st) < 0)
		return 0;
	return st.st_ino;
}

/*
 * the descriptor the context's requests go to: the one it was found by
 * may be closed (RADV closes its own and keeps libdrm_amdgpu's dup), and
 * its number used again; then fd, which is the context's, takes over
 */
static Dev*
livefd(Dev *d, int fd)
{
	if(d->fd != fd && ino(d->fd) != d->ino){
		d->fd = fd;
		if(d->amd != NULL)
			amd9setfd(d->amd, fd);
	}
	return d;
}

/*
 * the context behind a descriptor, found by its first ioctl: the
 * clone's ctl says ctx n. The descriptor keeps the name it was opened
 * by (/dev/gpu/N/clone, fd2path), the qid is its context's ctl's, which
 * no other file has: a descriptor closed and its number used again
 * tells by that.
 */
static Dev*
getdev(int fd)
{
	char path[128], buf[64], *s;
	uint64_t q;
	Fd *f;
	Dev *d;
	int i, n;

	if(fd < 0 || fd >= Maxfd)
		return NULL;
	if(_FD2PATH(fd, path, sizeof path) < 0)
		return NULL;
	q = ino(fd);
	f = &fds[fd];
	if(f->kind == Kdev && f->dev->ino == q)
		return livefd(f->dev, fd);
	f->kind = Knone;
	/* a descriptor dup'd from a known one (libdrm_amdgpu dups): the same context */
	for(i = 0; i < Maxfd; i++)
		if(i != fd && fds[i].kind == Kdev && fds[i].dev->ino == q){
			f->kind = Kdev;
			f->dev = fds[i].dev;
			return livefd(f->dev, fd);
		}
	scan();
	for(i = 0; i < ngpu; i++){
		n = strlen(gpus[i].dir);
		if(strncmp(path, gpus[i].dir, n) == 0 && path[n] == '/')
			break;
	}
	if(i == ngpu)
		return NULL;
	if((n = pread(fd, buf, sizeof buf-1, 0)) <= 0)
		return NULL;
	buf[n] = 0;
	if(strncmp(buf, "ctx ", 4) != 0)
		return NULL;
	d = calloc(1, sizeof *d);
	if(d == NULL)
		return NULL;
	d->gpu = &gpus[i];
	d->fd = fd;
	d->ino = q;
	d->ctx = atoi(buf+4);
	snprintf(path, sizeof path, "%s/ctx/%d/submit", d->gpu->dir, d->ctx);
	d->submitfd = open(path, O_WRONLY);
	snprintf(path, sizeof path, "%s/ctx/%d/fence", d->gpu->dir, d->ctx);
	d->statfd = open(path, O_RDONLY);
	if(d->submitfd < 0 || d->statfd < 0){
		if(d->submitfd >= 0)
			close(d->submitfd);
		if(d->statfd >= 0)
			close(d->statfd);
		free(d);
		return NULL;
	}
	if((s = strchr(buf, '\n')) != NULL)
		*s = 0;
	if(d->gpu->amd && (d->amd = amd9attach(fd, d->gpu->dir, d->ctx)) == NULL){
		close(d->submitfd);
		close(d->statfd);
		free(d);
		return NULL;
	}
	f->kind = Kdev;
	f->dev = d;
	return d;
}

/* a pipe that stands for a dma-buf or sync file */
static int
newfd(int kind, Dev *d)
{
	int p[2];

	if(pipe(p) < 0)
		return -1;
	close(p[1]);
	if(p[0] >= Maxfd){
		close(p[0]);
		return seterr(EMFILE);
	}
	memset(&fds[p[0]], 0, sizeof fds[p[0]]);
	fds[p[0]].kind = kind;
	fds[p[0]].dev = d;
	fds[p[0]].ino = ino(p[0]);
	return p[0];
}

static Fd*
getfd(int fd, int kind)
{
	if(fd < 0 || fd >= Maxfd || fds[fd].kind != kind || fds[fd].ino != ino(fd))
		return NULL;
	return &fds[fd];
}

/* the driver's ctl: a request, its reply */
static int
ctl(Dev *d, char *reply, int n, char *fmt, ...)
{
	char req[256];
	va_list arg;
	int m;

	va_start(arg, fmt);
	vsnprintf(req, sizeof req, fmt, arg);
	va_end(arg);
	if(write(d->fd, req, strlen(req)) < 0){
		int e = errno;

		dbg("ctl %s: %s (%s)", req, syserr(), strerror(e));
		return seterr(EINVAL);
	}
	if(reply != NULL){
		if((m = pread(d->fd, reply, n-1, 0)) < 0)
			return seterr(EIO);
		reply[m] = 0;
	}
	return 0;
}

/*
 * fences
 */
static int
fencedone(Fence *f)
{
	char buf[64];
	unsigned long long done, n;
	Dev *d;
	int m;

	if((d = f->ctx) == NULL || f->k <= d->done)
		return 1;
	if((m = pread(d->statfd, buf, sizeof buf-1, 0)) <= 0)
		return 1;
	buf[m] = 0;
	if(sscanf(buf, "%llu %llu", &done, &n) == 2 && done > d->done)
		d->done = done;
	if(strstr(buf, "failed") != NULL)
		d->failed = 1;
	return f->k <= d->done;
}

/* block until the fence is done; without lk */
static void
fencewait(Fence f)
{
	char path[128], buf[64];
	Dev *d;
	int fd, n;

	if((d = f.ctx) == NULL || f.k <= d->done)
		return;
	snprintf(path, sizeof path, "%s/ctx/%d/fence", d->gpu->dir, d->ctx);
	if((fd = open(path, O_RDWR)) < 0)
		return;
	snprintf(buf, sizeof buf, "wait %llu", (unsigned long long)f.k);
	if(write(fd, buf, strlen(buf)) >= 0 && (n = read(fd, buf, sizeof buf-1)) > 0){
		buf[n] = 0;
		if(strstr(buf, "failed") != NULL)
			d->failed = 1;
	}
	close(fd);
	pthread_mutex_lock(&lk);
	if(f.k > d->done && !d->failed)
		d->done = f.k;	/* fences of a context complete in order */
	pthread_mutex_unlock(&lk);
}

static int64_t
now(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return (int64_t)t.tv_sec*1000000000LL + t.tv_nsec;
}

/*
 * GEM objects
 */
static Gem*
getgem(Dev *d, uint32_t h)
{
	if(h == 0 || h > d->ngem || !d->gem[h-1].used)
		return NULL;
	return &d->gem[h-1];
}

static int
gemcreate(Dev *d, uint64_t size, uint32_t *handle)
{
	char reply[256], *f[6], *p;
	Gem *g, *ng;
	int i, n;

	for(i = 0; i < d->ngem; i++)
		if(!d->gem[i].used)
			break;
	if(i == d->ngem){
		ng = realloc(d->gem, (d->ngem + 64) * sizeof *ng);
		if(ng == NULL)
			return seterr(ENOMEM);
		memset(ng + d->ngem, 0, 64 * sizeof *ng);
		d->gem = ng;
		d->ngem += 64;
	}
	if(ctl(d, reply, sizeof reply, "bo %llu 0", (unsigned long long)size) < 0)
		return -1;
	/* bo id seg va size gpuaddr */
	n = 0;
	for(p = strtok(reply, " \n"); p != NULL && n < 6; p = strtok(NULL, " \n"))
		f[n++] = p;
	if(n != 6 || strcmp(f[0], "bo") != 0)
		return seterr(ENOMEM);
	g = &d->gem[i];
	memset(g, 0, sizeof *g);
	g->used = 1;
	g->id = atoi(f[1]);
	snprintf(g->seg, sizeof g->seg, "%s", f[2]);
	g->va = strtoull(f[3], NULL, 0);
	g->size = strtoull(f[4], NULL, 0);
	*handle = i+1;
	return 0;
}

static char*
gemmap(Gem *g)
{
	void *p;

	if(g->p == NULL){
		p = mesa9_segattach(g->seg, (void*)(uintptr_t)g->va, g->size);
		if(p == (void*)-1)
			return NULL;
		g->p = p;
	}
	return g->p;
}

static int
gemclose(Dev *d, uint32_t h)
{
	Gem *g;

	if((g = getgem(d, h)) == NULL)
		return seterr(ENOENT);
	if(g->p != NULL)
		mesa9_segdetach(g->p);
	ctl(d, NULL, 0, "free %d", g->id);
	memset(g, 0, sizeof *g);
	return 0;
}

/*
 * syncobjs
 */
static Sync*
getsync(Dev *d, uint32_t h)
{
	if(h == 0 || h > d->nsync || !d->sync[h-1].used)
		return NULL;
	return &d->sync[h-1];
}

/* signalled points off a timeline */
static void
syncreap(Sync *s)
{
	int i;

	for(i = 0; i < s->np && fencedone(&s->pf[i]); i++)
		if(s->pv[i] > s->payload)
			s->payload = s->pv[i];
	if(i > 0){
		memmove(s->pv, s->pv+i, (s->np-i) * sizeof s->pv[0]);
		memmove(s->pf, s->pf+i, (s->np-i) * sizeof s->pf[0]);
		s->np -= i;
	}
}

/* point v (0: the binary fence) gets fence f */
static int
syncadd(Sync *s, uint64_t v, Fence f)
{
	if(v == 0){
		s->f = f;
		s->hasf = 1;
		return 0;
	}
	s->timeline = 1;
	syncreap(s);
	if(s->np > 0 && v <= s->pv[s->np-1])
		return seterr(EINVAL);
	if(v <= s->payload)
		return 0;
	if(s->np == Ncpts){
		/* make room: the oldest is waited for */
		pthread_mutex_unlock(&lk);
		fencewait(s->pf[0]);
		pthread_mutex_lock(&lk);
		syncreap(s);
	}
	if(f.ctx == NULL || fencedone(&f)){
		/* signalled at once: everything before it is too, in order */
		s->payload = v;
		s->np = 0;
		return 0;
	}
	s->pv[s->np] = v;
	s->pf[s->np] = f;
	s->np++;
	return 0;
}

enum { Snofence, Spending, Sdone };

/* the state of point v (0: binary) of a syncobj, its fence when pending */
static int
syncstate(Sync *s, uint64_t v, Fence *f)
{
	int i;

	if(v == 0 && !s->timeline){
		if(!s->hasf)
			return Snofence;
		*f = s->f;
		return fencedone(f) ? Sdone : Spending;
	}
	if(v == 0)
		v = s->payload + (s->np > 0 ? 1 : 0);
	syncreap(s);
	if(v <= s->payload)
		return Sdone;
	for(i = 0; i < s->np; i++)
		if(s->pv[i] >= v){
			*f = s->pf[i];
			return Spending;
		}
	return Snofence;
}

/* the fence that stands for point v, for execbuffer waits and transfers */
static int
syncfence(Sync *s, uint64_t v, Fence *f)
{
	memset(f, 0, sizeof *f);
	switch(syncstate(s, v, f)){
	case Snofence:
		return -1;
	case Sdone:
		memset(f, 0, sizeof *f);
		break;
	}
	return 0;
}

static int
syncwait(Dev *d, uint32_t *h, uint64_t *pts, unsigned n, int64_t timeout, unsigned flags, uint32_t *first)
{
	Fence f, pend;
	Sync *s;
	int ndone, havepend, st;
	unsigned i;

	for(;;){
		pthread_mutex_lock(&lk);
		ndone = 0;
		havepend = 0;
		for(i = 0; i < n; i++){
			if((s = getsync(d, h[i])) == NULL){
				pthread_mutex_unlock(&lk);
				return seterr(ENOENT);
			}
			st = syncstate(s, pts != NULL ? pts[i] : 0, &f);
			if(st == Spending && (flags & DRM_SYNCOBJ_WAIT_FLAGS_WAIT_AVAILABLE))
				st = Sdone;
			if(st == Sdone){
				ndone++;
				if(!(flags & DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL)){
					if(first != NULL)
						*first = i;
					pthread_mutex_unlock(&lk);
					return 0;
				}
				continue;
			}
			if(st == Snofence && !(flags & DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT)){
				pthread_mutex_unlock(&lk);
				return seterr(EINVAL);
			}
			if(st == Spending && !havepend){
				pend = f;
				havepend = 1;
			}
		}
		pthread_mutex_unlock(&lk);
		if(ndone == n){
			if(first != NULL)
				*first = 0;
			return 0;
		}
		if(timeout <= now())
			return seterr(ETIME);
		/* without a time limit (and with all of them needed) block on a fence */
		if(havepend && timeout == INT64_MAX && (flags & DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL || n == 1))
			fencewait(pend);
		else
			usleep(500);
	}
}

/*
 * the i915 ioctls
 */
/*
 * facts of the hardware the driver knows (aux/i915: i915's own
 * GETPARAM); what this library can do over gpu(4) stays its own
 */
static int
driverparam(Dev *d, int param, int *v)
{
	char reply[64];

	switch(param){
	case I915_PARAM_CHIPSET_ID:
	case I915_PARAM_REVISION:
	case I915_PARAM_CS_TIMESTAMP_FREQUENCY:
	case I915_PARAM_SLICE_MASK:
	case I915_PARAM_SUBSLICE_MASK:
	case I915_PARAM_EU_TOTAL:
	case I915_PARAM_HAS_LLC:
		break;
	default:
		return -1;
	}
	if(ctl(d, reply, sizeof reply, "getparam %d", param) < 0)
		return -1;	/* aux/intelgpu: no getparam */
	return sscanf(reply, "getparam %*d %d", v) == 1 ? 0 : -1;
}

static int
getparam(Dev *d, struct drm_i915_getparam *p)
{
	int v;

	if(driverparam(d, p->param, &v) == 0){
		*p->value = v;
		return 0;
	}
	switch(p->param){
	case I915_PARAM_CHIPSET_ID:		v = d->gpu->device; break;
	case I915_PARAM_REVISION:		v = d->gpu->revision; break;
	case I915_PARAM_HAS_EXECBUF2:
	case I915_PARAM_HAS_WAIT_TIMEOUT:
	case I915_PARAM_HAS_EXEC_SOFTPIN:
	case I915_PARAM_HAS_EXEC_NO_RELOC:
	case I915_PARAM_HAS_EXEC_HANDLE_LUT:
	case I915_PARAM_HAS_EXEC_BATCH_FIRST:
	case I915_PARAM_HAS_EXEC_FENCE_ARRAY:
	case I915_PARAM_HAS_EXEC_ASYNC:
	case I915_PARAM_HAS_EXEC_CAPTURE:
	case I915_PARAM_HAS_EXEC_TIMELINE_FENCES:
	case I915_PARAM_HAS_CONTEXT_ISOLATION:
	case I915_PARAM_HAS_LLC:
	case I915_PARAM_HAS_GEM:
	case I915_PARAM_HAS_RELAXED_DELTA:
	case I915_PARAM_HAS_RELAXED_FENCING:
		v = 1;
		break;
	case I915_PARAM_HAS_BLT:		v = 1; break;
	case I915_PARAM_HAS_BSD:
	case I915_PARAM_HAS_BSD2:
	case I915_PARAM_HAS_VEBOX:		v = 0; break;
	case I915_PARAM_MMAP_VERSION:		v = 0; break;	/* no write-combining mmap: LLC */
	case I915_PARAM_CS_TIMESTAMP_FREQUENCY:	v = Cs_hz; break;
	case I915_PARAM_SLICE_MASK:		v = d->gpu->slicemask; break;
	case I915_PARAM_SUBSLICE_MASK:		v = d->gpu->subslicemask; break;
	case I915_PARAM_EU_TOTAL:		v = d->gpu->eutotal; break;
	case I915_PARAM_NUM_FENCES_AVAIL:	v = 0; break;
	default:
		return seterr(EINVAL);
	}
	*p->value = v;
	return 0;
}

static int
ctxparam(Dev *d, struct drm_i915_gem_context_param *p, int set)
{
	if(p->ctx_id > 1)
		return seterr(ENOENT);
	switch(p->param){
	case I915_CONTEXT_PARAM_GTT_SIZE:
		if(set)
			return seterr(EINVAL);
		p->value = 1ULL<<48;
		return 0;
	case I915_CONTEXT_PARAM_RECOVERABLE:
	case I915_CONTEXT_PARAM_BANNABLE:
		if(!set)
			p->value = 0;
		return 0;
	case I915_CONTEXT_PARAM_PRIORITY:
		/* one priority: the default */
		if(set && (int64_t)p->value != 0)
			return seterr(EPERM);
		if(!set)
			p->value = 0;
		return 0;
	}
	(void)d;
	return seterr(EINVAL);
}

static int
execbuf(Dev *d, struct drm_i915_gem_execbuffer2 *eb)
{
	struct drm_i915_gem_exec_object2 *obj;
	struct drm_i915_gem_exec_fence *fa;
	struct drm_i915_gem_execbuffer_ext_timeline_fences *tl;
	struct i915_user_extension *ext;
	uint64_t *vals, addr, k;
	unsigned i, nf, eng;
	char req[64], reply[256];
	Fence f, mine;
	Gem *g, *batch;
	Sync *s;

	if(eb->buffer_count == 0)
		return seterr(EINVAL);
	if(eb->rsvd1 > 1)
		return seterr(ENOENT);
	obj = (void*)(uintptr_t)eb->buffers_ptr;
	fa = NULL;
	vals = NULL;
	nf = 0;
	if(eb->flags & I915_EXEC_USE_EXTENSIONS){
		for(ext = (void*)(uintptr_t)eb->cliprects_ptr; ext != NULL; ext = (void*)(uintptr_t)ext->next_extension)
			if(ext->name == DRM_I915_GEM_EXECBUFFER_EXT_TIMELINE_FENCES){
				tl = (void*)ext;
				fa = (void*)(uintptr_t)tl->handles_ptr;
				vals = (void*)(uintptr_t)tl->values_ptr;
				nf = tl->fence_count;
			}else
				return seterr(EINVAL);
	}else if(eb->flags & I915_EXEC_FENCE_ARRAY){
		fa = (void*)(uintptr_t)eb->cliprects_ptr;
		nf = eb->num_cliprects;
	}
	switch(eb->flags & I915_EXEC_RING_MASK){
	case I915_EXEC_DEFAULT:
	case I915_EXEC_RENDER:	eng = 0; break;
	case I915_EXEC_BLT:	eng = 1; break;
	default:
		return seterr(EINVAL);
	}

	pthread_mutex_lock(&lk);
	/* the objects where the client pinned them */
	for(i = 0; i < eb->buffer_count; i++){
		if(obj[i].relocation_count != 0){
			pthread_mutex_unlock(&lk);
			dbg("execbuffer: object %u has relocations", i);
			return seterr(EINVAL);	/* softpin only */
		}
		if((g = getgem(d, obj[i].handle)) == NULL){
			pthread_mutex_unlock(&lk);
			dbg("execbuffer: object %u: no handle %u", i, obj[i].handle);
			return seterr(ENOENT);
		}
		if(obj[i].flags & EXEC_OBJECT_PINNED){
			addr = obj[i].offset & ((1ULL<<48) - 1);	/* canonical to 48 bits */
			if(addr != g->gpu){
				if(ctl(d, NULL, 0, "bind %d %#llx", g->id, (unsigned long long)addr) < 0){
					pthread_mutex_unlock(&lk);
					return seterr(EINVAL);
				}
				g->gpu = addr;
			}
		}else if(g->gpu == 0){
			pthread_mutex_unlock(&lk);
			dbg("execbuffer: object %u (handle %u) not pinned and not mapped", i, obj[i].handle);
			return seterr(EINVAL);
		}else
			obj[i].offset = g->gpu;
	}
	/* waits: on other contexts' batches here (ours run in order) */
	for(i = 0; i < nf; i++){
		if(!(fa[i].flags & I915_EXEC_FENCE_WAIT))
			continue;
		if((s = getsync(d, fa[i].handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		if(syncfence(s, vals != NULL ? vals[i] : 0, &f) < 0){
			pthread_mutex_unlock(&lk);
			dbg("execbuffer: wait on syncobj %u point %llu: no fence", fa[i].handle,
				(unsigned long long)(vals != NULL ? vals[i] : 0));
			return seterr(EINVAL);
		}
		if(f.ctx != NULL && f.ctx != d){
			pthread_mutex_unlock(&lk);
			fencewait(f);
			pthread_mutex_lock(&lk);
		}
	}
	batch = getgem(d, obj[(eb->flags & I915_EXEC_BATCH_FIRST) ? 0 : eb->buffer_count-1].handle);
	snprintf(req, sizeof req, "%d %u %s", batch->id, eb->batch_start_offset, eng == 0 ? "rcs" : "bcs");
	if(write(d->submitfd, req, strlen(req)) < 0){
		dbg("submit %s: %s", req, syserr());
		pthread_mutex_unlock(&lk);
		return seterr(EIO);
	}
	k = ++d->nfence;
	/* the driver's count: our submissions are all there are on the context */
	if(pread(d->statfd, reply, sizeof reply-1, 0) > 0){
		unsigned long long done, n;
		if(sscanf(reply, "%llu %llu", &done, &n) == 2){
			k = n;
			d->nfence = n;
			if(done > d->done)
				d->done = done;
		}
	}
	mine.ctx = d;
	mine.k = k;
	for(i = 0; i < eb->buffer_count; i++)
		getgem(d, obj[i].handle)->last = mine;
	for(i = 0; i < nf; i++){
		if(!(fa[i].flags & I915_EXEC_FENCE_SIGNAL))
			continue;
		if((s = getsync(d, fa[i].handle)) != NULL)
			syncadd(s, vals != NULL ? vals[i] : 0, mine);
	}
	pthread_mutex_unlock(&lk);
	return 0;
}

static int
gemwait(Dev *d, struct drm_i915_gem_wait *w)
{
	Fence f;
	Gem *g;
	int64_t end;

	pthread_mutex_lock(&lk);
	if((g = getgem(d, w->bo_handle)) == NULL){
		pthread_mutex_unlock(&lk);
		return seterr(ENOENT);
	}
	f = g->last;
	if(fencedone(&f)){
		pthread_mutex_unlock(&lk);
		w->timeout_ns = w->timeout_ns < 0 ? w->timeout_ns : w->timeout_ns;
		return 0;
	}
	pthread_mutex_unlock(&lk);
	/*
	 * negative: no limit; so is one past INT64_MAX ns from now (Mesa
	 * waits with INT64_MAX: now() + that overflowed, an instant ETIME
	 * as soon as a batch took longer than the first look)
	 */
	end = now();
	if(w->timeout_ns < 0 || w->timeout_ns > INT64_MAX - end){
		fencewait(f);
		return 0;
	}
	end += w->timeout_ns;
	for(;;){
		pthread_mutex_lock(&lk);
		if(fencedone(&f)){
			pthread_mutex_unlock(&lk);
			w->timeout_ns = end - now() > 0 ? end - now() : 0;
			return 0;
		}
		pthread_mutex_unlock(&lk);
		if(now() >= end){
			w->timeout_ns = 0;
			return seterr(ETIME);
		}
		usleep(200);
	}
}

static int
query(Dev *d, struct drm_i915_query *q)
{
	struct drm_i915_query_item *it;
	unsigned i;

	(void)d;
	it = (void*)(uintptr_t)q->items_ptr;
	for(i = 0; i < q->num_items; i++)
		it[i].length = -EINVAL;	/* none: Mesa falls back to getparams */
	return 0;
}

static int
i915ioctl(Dev *d, unsigned long req, void *arg)
{
	uint32_t h;
	Gem *g;
	char *p;
	int r;

	switch(req){
	case DRM_IOCTL_I915_GETPARAM:
		return getparam(d, arg);
	case DRM_IOCTL_I915_QUERY:
		return query(d, arg);
	case DRM_IOCTL_I915_GEM_GET_APERTURE: {
		struct drm_i915_gem_get_aperture *a = arg;
		a->aper_size = d->gpu->aperture;
		a->aper_available_size = d->gpu->aperture;
		return 0;
	}
	case DRM_IOCTL_I915_GEM_CONTEXT_CREATE: {
		struct drm_i915_gem_context_create *c = arg;
		c->ctx_id = 1;	/* the descriptor's context */
		return 0;
	}
	case DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT: {
		struct drm_i915_gem_context_create_ext *c = arg;
		struct i915_user_extension *ext;
		struct drm_i915_gem_context_create_ext_setparam *sp;
		if(c->flags & I915_CONTEXT_CREATE_FLAGS_USE_EXTENSIONS)
			for(ext = (void*)(uintptr_t)c->extensions; ext != NULL; ext = (void*)(uintptr_t)ext->next_extension){
				if(ext->name != I915_CONTEXT_CREATE_EXT_SETPARAM)
					return seterr(EINVAL);
				sp = (void*)ext;
				if(ctxparam(d, &sp->param, 1) < 0)
					return -1;
			}
		c->ctx_id = 1;
		return 0;
	}
	case DRM_IOCTL_I915_GEM_CONTEXT_DESTROY:
		return 0;
	case DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM:
		return ctxparam(d, arg, 0);
	case DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM:
		return ctxparam(d, arg, 1);
	case DRM_IOCTL_I915_GET_RESET_STATS: {
		struct drm_i915_reset_stats *rs = arg;
		pthread_mutex_lock(&lk);
		fencedone(&(Fence){d, ~0ULL});
		rs->reset_count = d->failed;
		rs->batch_active = d->failed;
		rs->batch_pending = 0;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_CREATE: {
		struct drm_i915_gem_create *c = arg;
		pthread_mutex_lock(&lk);
		r = gemcreate(d, c->size, &h);
		if(r == 0){
			c->handle = h;
			c->size = getgem(d, h)->size;
		}
		pthread_mutex_unlock(&lk);
		return r;
	}
	case DRM_IOCTL_GEM_CLOSE: {
		struct drm_gem_close *c = arg;
		pthread_mutex_lock(&lk);
		r = gemclose(d, c->handle);
		pthread_mutex_unlock(&lk);
		return r;
	}
	case DRM_IOCTL_I915_GEM_MMAP: {
		struct drm_i915_gem_mmap *m = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(d, m->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		if(m->offset + m->size > g->size || (p = gemmap(g)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(EINVAL);
		}
		m->addr_ptr = (uintptr_t)(p + m->offset);
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_SET_CACHING:
	case DRM_IOCTL_I915_GEM_GET_CACHING: {
		struct drm_i915_gem_caching *c = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(d, c->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		if(req == DRM_IOCTL_I915_GEM_SET_CACHING)
			g->caching = c->caching;
		else
			c->caching = g->caching != 0 ? g->caching : I915_CACHING_CACHED;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_SET_TILING: {
		struct drm_i915_gem_set_tiling *t = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(d, t->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		g->tiling = t->tiling_mode;
		g->stride = t->stride;
		t->swizzle_mode = I915_BIT_6_SWIZZLE_NONE;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_GET_TILING: {
		struct drm_i915_gem_get_tiling *t = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(d, t->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		t->tiling_mode = g->tiling;
		t->swizzle_mode = I915_BIT_6_SWIZZLE_NONE;
		t->phys_swizzle_mode = I915_BIT_6_SWIZZLE_NONE;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_SET_DOMAIN:
		return 0;	/* the LLC keeps processor and GPU coherent */
	case DRM_IOCTL_I915_GEM_MADVISE: {
		struct drm_i915_gem_madvise *m = arg;
		m->retained = 1;
		return 0;
	}
	case DRM_IOCTL_I915_GEM_BUSY: {
		struct drm_i915_gem_busy *b = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(d, b->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		b->busy = !fencedone(&g->last);
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_I915_GEM_WAIT:
		return gemwait(d, arg);
	case DRM_IOCTL_I915_GEM_EXECBUFFER2:
	case DRM_IOCTL_I915_GEM_EXECBUFFER2_WR:
		return execbuf(d, arg);
	case DRM_IOCTL_PRIME_HANDLE_TO_FD: {
		struct drm_prime_handle *ph = arg;
		pthread_mutex_lock(&lk);
		if(getgem(d, ph->handle) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		if((r = newfd(Kdmabuf, d)) < 0){
			pthread_mutex_unlock(&lk);
			return -1;
		}
		fds[r].handle = ph->handle;
		ph->fd = r;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DRM_IOCTL_PRIME_FD_TO_HANDLE: {
		struct drm_prime_handle *ph = arg;
		Fd *f;
		pthread_mutex_lock(&lk);
		if((f = getfd(ph->fd, Kdmabuf)) == NULL || f->dev != d){
			pthread_mutex_unlock(&lk);
			return seterr(EINVAL);
		}
		ph->handle = f->handle;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	}
	return seterr(EINVAL);
}

/* on a dma-buf: sync files of the object's last batch */
static int
dmabufioctl(Fd *f, unsigned long req, void *arg)
{
	Gem *g;
	int r;

	switch(req){
	case DMA_BUF_IOCTL_EXPORT_SYNC_FILE: {
		struct dma_buf_export_sync_file *e = arg;
		pthread_mutex_lock(&lk);
		if((g = getgem(f->dev, f->handle)) == NULL || (r = newfd(Ksyncfile, f->dev)) < 0){
			pthread_mutex_unlock(&lk);
			return seterr(EINVAL);
		}
		fds[r].f = g->last;
		fds[r].hasf = 1;
		e->fd = r;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DMA_BUF_IOCTL_IMPORT_SYNC_FILE: {
		struct dma_buf_import_sync_file *im = arg;
		Fd *s;
		pthread_mutex_lock(&lk);
		if((s = getfd(im->fd, Ksyncfile)) == NULL || (g = getgem(f->dev, f->handle)) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(EINVAL);
		}
		if(s->hasf && s->f.ctx != NULL && !fencedone(&s->f))
			g->last = s->f;
		pthread_mutex_unlock(&lk);
		return 0;
	}
	case DMA_BUF_IOCTL_SYNC:
		return 0;
	}
	return seterr(EINVAL);
}

/*
 * ioctl: ours on the descriptors we know, else APE's (libbsd: FIONREAD)
 */
int
ioctl(int fd, unsigned long req, void *arg)
{
	struct stat st;
	Dev *d;
	Fd *f;

	if((req >> 8 & 0xff) == DRM_IOCTL_BASE){
		pthread_mutex_lock(&lk);
		d = getdev(fd);
		pthread_mutex_unlock(&lk);
		if(d == NULL){
			dbg("ioctl %#lx on %d: not a GPU context", req, fd);
			return seterr(ENOTTY);
		}
		if(d->amd != NULL)
			return amd9ioctl(d->amd, req, arg);
		if(i915ioctl(d, req, arg) < 0){
			dbg("ioctl %#lx (nr %#lx): %s", req, req & 0xff, strerror(errno));
			return -1;
		}
		return 0;
	}
	if((req >> 8 & 0xff) == DMA_BUF_BASE){
		pthread_mutex_lock(&lk);
		f = getfd(fd, Kdmabuf);
		pthread_mutex_unlock(&lk);
		if(f == NULL)
			return seterr(ENOTTY);
		return dmabufioctl(f, req, arg);
	}
	if(req == FIONREAD){
		if(fstat(fd, &st) < 0)
			return seterr(EBADF);
		*(long*)arg = st.st_size;
		return 0;
	}
	return seterr(EINVAL);
}

int
drmIoctl(int fd, unsigned long req, void *arg)
{
	int r;

	do
		r = ioctl(fd, req, arg);
	while(r == -1 && (errno == EINTR || errno == EAGAIN));
	return r;
}

/*
 * a driver's own ioctl (libdrm's drmCommand*: DRM_COMMAND_BASE + idx,
 * size bytes at data); 0 or -errno
 */
static int
command(int fd, unsigned long inout, unsigned long idx, void *data, unsigned long size)
{
	return drmIoctl(fd, _IOC(inout, DRM_IOCTL_BASE, DRM_COMMAND_BASE + idx, size), data) < 0 ? -errno : 0;
}

int
drmCommandWriteRead(int fd, unsigned long idx, void *data, unsigned long size)
{
	return command(fd, IOC_INOUT, idx, data, size);
}

int
drmCommandWrite(int fd, unsigned long idx, void *data, unsigned long size)
{
	return command(fd, IOC_IN, idx, data, size);
}

int
drmCommandRead(int fd, unsigned long idx, void *data, unsigned long size)
{
	return command(fd, IOC_OUT, idx, data, size);
}

/*
 * devices
 */
static drmDevicePtr
mkdevice(Gpu *g)
{
	drmDevicePtr d;
	char *s;
	int i;

	d = calloc(1, sizeof *d + DRM_NODE_MAX*sizeof(char*) + DRM_NODE_MAX*64 +
		sizeof(drmPciBusInfo) + sizeof(drmPciDeviceInfo));
	if(d == NULL)
		return NULL;
	d->nodes = (char**)(d + 1);
	s = (char*)(d->nodes + DRM_NODE_MAX);
	for(i = 0; i < DRM_NODE_MAX; i++){
		d->nodes[i] = s + i*64;
		snprintf(d->nodes[i], 64, "%s/clone", g->dir);
	}
	d->available_nodes = 1<<DRM_NODE_PRIMARY | 1<<DRM_NODE_RENDER;
	d->bustype = DRM_BUS_PCI;
	d->businfo.pci = (drmPciBusInfoPtr)(s + DRM_NODE_MAX*64);
	d->deviceinfo.pci = (drmPciDeviceInfoPtr)(d->businfo.pci + 1);
	d->businfo.pci->domain = 0;
	d->businfo.pci->bus = g->bus;
	d->businfo.pci->dev = g->dev;
	d->businfo.pci->func = g->func;
	d->deviceinfo.pci->vendor_id = g->vendor;
	d->deviceinfo.pci->device_id = g->device;
	d->deviceinfo.pci->subvendor_id = g->subvendor;
	d->deviceinfo.pci->subdevice_id = g->subdevice;
	d->deviceinfo.pci->revision_id = g->revision;
	return d;
}

int
drmGetDevices2(uint32_t flags, drmDevicePtr devs[], int max)
{
	int i, n;

	(void)flags;
	pthread_mutex_lock(&lk);
	scan();
	n = 0;
	for(i = 0; i < ngpu; i++){
		if(devs != NULL && n < max)
			devs[n] = mkdevice(&gpus[i]);
		n++;
	}
	pthread_mutex_unlock(&lk);
	return devs != NULL && n > max ? max : n;
}

/* the names of modifiers (for debug output): none known here */
char*
drmGetFormatModifierName(uint64_t modifier)
{
	(void)modifier;
	return NULL;
}

int
drmGetDevices(drmDevicePtr devs[], int max)
{
	return drmGetDevices2(0, devs, max);
}

int
drmGetDevice2(int fd, uint32_t flags, drmDevicePtr *dev)
{
	Dev *d;

	(void)flags;
	pthread_mutex_lock(&lk);
	d = getdev(fd);
	pthread_mutex_unlock(&lk);
	if(d == NULL)
		return -ENODEV;
	if((*dev = mkdevice(d->gpu)) == NULL)
		return -ENOMEM;
	return 0;
}

int
drmGetDevice(int fd, drmDevicePtr *dev)
{
	return drmGetDevice2(fd, 0, dev);
}

/* the device whose node has the id (stat's st_rdev, as RADV keeps it) */
int
drmGetDeviceFromDevId(dev_t id, uint32_t flags, drmDevicePtr *dev)
{
	struct stat st;
	char path[64];
	int i;

	(void)flags;
	pthread_mutex_lock(&lk);
	scan();
	for(i = 0; i < ngpu; i++){
		snprintf(path, sizeof path, "%s/clone", gpus[i].dir);
		if(stat(path, &st) == 0 && st.st_rdev == id){
			*dev = mkdevice(&gpus[i]);
			pthread_mutex_unlock(&lk);
			return *dev != NULL ? 0 : -ENOMEM;
		}
	}
	pthread_mutex_unlock(&lk);
	return -ENODEV;
}

void
drmFreeDevice(drmDevicePtr *dev)
{
	if(dev != NULL){
		free(*dev);
		*dev = NULL;
	}
}

void
drmFreeDevices(drmDevicePtr devs[], int n)
{
	int i;

	if(devs == NULL)
		return;
	for(i = 0; i < n; i++)
		drmFreeDevice(&devs[i]);
}

int
drmDevicesEqual(drmDevicePtr a, drmDevicePtr b)
{
	if(a == NULL || b == NULL || a->bustype != b->bustype)
		return 0;
	return memcmp(a->businfo.pci, b->businfo.pci, sizeof(drmPciBusInfo)) == 0;
}

int
drmGetNodeTypeFromFd(int fd)
{
	Dev *d;

	pthread_mutex_lock(&lk);
	d = getdev(fd);
	pthread_mutex_unlock(&lk);
	return d != NULL ? DRM_NODE_RENDER : -1;
}

static char*
nodename(int fd)
{
	char buf[64];
	Dev *d;

	pthread_mutex_lock(&lk);
	d = getdev(fd);
	pthread_mutex_unlock(&lk);
	if(d == NULL)
		return NULL;
	snprintf(buf, sizeof buf, "%s/clone", d->gpu->dir);
	return strdup(buf);
}

char*
drmGetDeviceNameFromFd2(int fd)
{
	return nodename(fd);
}

char*
drmGetPrimaryDeviceNameFromFd(int fd)
{
	return nodename(fd);
}

char*
drmGetRenderDeviceNameFromFd(int fd)
{
	return nodename(fd);
}

/* the AMD context of a descriptor (amdgpu9.c), or NULL */
Amd*
amd9of(int fd)
{
	Dev *d;

	pthread_mutex_lock(&lk);
	d = getdev(fd);
	pthread_mutex_unlock(&lk);
	return d != NULL ? d->amd : NULL;
}

/* amdgpu's version, as libdrm asks it: the lengths, then the strings */
static drmVersionPtr
amdversion(int fd)
{
	struct drm_version dv;
	drmVersionPtr v;

	memset(&dv, 0, sizeof dv);
	if(drmIoctl(fd, DRM_IOCTL_VERSION, &dv) < 0)
		return NULL;
	if((v = calloc(1, sizeof *v)) == NULL)
		return NULL;
	dv.name = calloc(1, dv.name_len + 1);
	dv.date = calloc(1, dv.date_len + 1);
	dv.desc = calloc(1, dv.desc_len + 1);
	if(dv.name == NULL || dv.date == NULL || dv.desc == NULL || drmIoctl(fd, DRM_IOCTL_VERSION, &dv) < 0){
		free(dv.name);
		free(dv.date);
		free(dv.desc);
		free(v);
		return NULL;
	}
	v->version_major = dv.version_major;
	v->version_minor = dv.version_minor;
	v->version_patchlevel = dv.version_patchlevel;
	v->name = dv.name;
	v->name_len = dv.name_len;
	v->date = dv.date;
	v->date_len = dv.date_len;
	v->desc = dv.desc;
	v->desc_len = dv.desc_len;
	return v;
}

drmVersionPtr
drmGetVersion(int fd)
{
	drmVersionPtr v;
	Dev *d;

	pthread_mutex_lock(&lk);
	d = getdev(fd);
	pthread_mutex_unlock(&lk);
	if(d == NULL)
		return NULL;
	if(d->amd != NULL)
		return amdversion(fd);
	if((v = calloc(1, sizeof *v)) == NULL)
		return NULL;
	v->version_major = 1;
	v->version_minor = 6;
	v->name = strdup("i915");
	v->name_len = 4;
	v->date = strdup("20260101");
	v->date_len = 8;
	v->desc = strdup("Intel graphics, gpu(4)");
	v->desc_len = strlen(v->desc);
	return v;
}

void
drmFreeVersion(drmVersionPtr v)
{
	if(v == NULL)
		return;
	free(v->name);
	free(v->date);
	free(v->desc);
	free(v);
}

int
drmGetCap(int fd, uint64_t cap, uint64_t *val)
{
	if(amd9of(fd) != NULL){
		struct drm_get_cap c;

		memset(&c, 0, sizeof c);
		c.capability = cap;
		if(drmIoctl(fd, DRM_IOCTL_GET_CAP, &c) < 0)
			return -errno;
		*val = c.value;
		return 0;
	}
	switch(cap){
	case DRM_CAP_SYNCOBJ:
	case DRM_CAP_SYNCOBJ_TIMELINE:
		*val = 1;
		return 0;
	case DRM_CAP_PRIME:
		*val = DRM_PRIME_CAP_IMPORT | DRM_PRIME_CAP_EXPORT;
		return 0;
	}
	*val = 0;
	return -EINVAL;
}

int drmSetClientCap(int fd, uint64_t cap, uint64_t val) { (void)fd; (void)cap; (void)val; return -EINVAL; }
int drmGetMagic(int fd, drm_magic_t *m) { (void)fd; *m = 0; return -EINVAL; }
int drmAuthMagic(int fd, drm_magic_t m) { (void)fd; (void)m; return -EINVAL; }
int drmSetMaster(int fd) { (void)fd; return -EINVAL; }
int drmDropMaster(int fd) { (void)fd; return -EINVAL; }
int drmIsMaster(int fd) { (void)fd; return 0; }

int
drmPrimeHandleToFD(int fd, uint32_t handle, uint32_t flags, int *out)
{
	struct drm_prime_handle ph;

	memset(&ph, 0, sizeof ph);
	ph.handle = handle;
	ph.flags = flags;
	if(drmIoctl(fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &ph) < 0)
		return -1;
	*out = ph.fd;
	return 0;
}

int
drmPrimeFDToHandle(int fd, int pfd, uint32_t *handle)
{
	struct drm_prime_handle ph;

	memset(&ph, 0, sizeof ph);
	ph.fd = pfd;
	if(drmIoctl(fd, DRM_IOCTL_PRIME_FD_TO_HANDLE, &ph) < 0)
		return -1;
	*handle = ph.handle;
	return 0;
}

int
drmCloseBufferHandle(int fd, uint32_t handle)
{
	struct drm_gem_close c;

	memset(&c, 0, sizeof c);
	c.handle = handle;
	return drmIoctl(fd, DRM_IOCTL_GEM_CLOSE, &c);
}

/*
 * syncobjs
 */
static Dev*
syncdev(int fd)
{
	Dev *d;

	pthread_mutex_lock(&lk);
	if((d = getdev(fd)) == NULL){
		pthread_mutex_unlock(&lk);
		errno = EBADF;
	}
	return d;	/* with lk */
}

int
drmSyncobjCreate(int fd, uint32_t flags, uint32_t *handle)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_create c = { .flags = flags };

		if(drmIoctl(fd, DRM_IOCTL_SYNCOBJ_CREATE, &c) < 0)
			return -1;
		*handle = c.handle;
		return 0;
	}
	Sync *ns;
	Dev *d;
	int i;

	if((d = syncdev(fd)) == NULL)
		return -1;
	for(i = 0; i < d->nsync; i++)
		if(!d->sync[i].used)
			break;
	if(i == d->nsync){
		ns = realloc(d->sync, (d->nsync + 64) * sizeof *ns);
		if(ns == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOMEM);
		}
		memset(ns + d->nsync, 0, 64 * sizeof *ns);
		d->sync = ns;
		d->nsync += 64;
	}
	memset(&d->sync[i], 0, sizeof d->sync[i]);
	d->sync[i].used = 1;
	if(flags & DRM_SYNCOBJ_CREATE_SIGNALED)
		d->sync[i].hasf = 1;	/* a fence of no batch: signalled */
	*handle = i+1;
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjDestroy(int fd, uint32_t handle)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_destroy c = { .handle = handle };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_DESTROY, &c);
	}
	Sync *s;
	Dev *d;

	if((d = syncdev(fd)) == NULL)
		return -1;
	if((s = getsync(d, handle)) == NULL){
		pthread_mutex_unlock(&lk);
		return seterr(ENOENT);
	}
	memset(s, 0, sizeof *s);
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjReset(int fd, const uint32_t *h, uint32_t n)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_array a = { .handles = (uint64_t)(uintptr_t)h, .count_handles = n };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_RESET, &a);
	}
	Sync *s;
	Dev *d;
	uint32_t i;

	if((d = syncdev(fd)) == NULL)
		return -1;
	for(i = 0; i < n; i++)
		if((s = getsync(d, h[i])) != NULL){
			s->hasf = 0;
			memset(&s->f, 0, sizeof s->f);
		}
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjSignal(int fd, const uint32_t *h, uint32_t n)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_array a = { .handles = (uint64_t)(uintptr_t)h, .count_handles = n };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_SIGNAL, &a);
	}
	Fence none;
	Sync *s;
	Dev *d;
	uint32_t i;

	if((d = syncdev(fd)) == NULL)
		return -1;
	memset(&none, 0, sizeof none);
	for(i = 0; i < n; i++)
		if((s = getsync(d, h[i])) != NULL)
			syncadd(s, 0, none);
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjTimelineSignal(int fd, const uint32_t *h, uint64_t *pts, uint32_t n)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_timeline_array a = { .handles = (uint64_t)(uintptr_t)h,
			.points = (uint64_t)(uintptr_t)pts, .count_handles = n };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TIMELINE_SIGNAL, &a);
	}
	Fence none;
	Sync *s;
	Dev *d;
	uint32_t i;

	if((d = syncdev(fd)) == NULL)
		return -1;
	memset(&none, 0, sizeof none);
	for(i = 0; i < n; i++)
		if((s = getsync(d, h[i])) != NULL)
			syncadd(s, pts[i], none);
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjWait(int fd, uint32_t *h, unsigned n, int64_t timeout, unsigned flags, uint32_t *first)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_wait w = { .handles = (uint64_t)(uintptr_t)h, .timeout_nsec = timeout,
			.count_handles = n, .flags = flags };

		if(drmIoctl(fd, DRM_IOCTL_SYNCOBJ_WAIT, &w) < 0)
			return -errno;
		if(first != NULL)
			*first = w.first_signaled;
		return 0;
	}
	Dev *d;

	if((d = syncdev(fd)) == NULL)
		return -1;
	pthread_mutex_unlock(&lk);
	return syncwait(d, h, NULL, n, timeout, flags, first);
}

int
drmSyncobjTimelineWait(int fd, uint32_t *h, uint64_t *pts, unsigned n, int64_t timeout, unsigned flags, uint32_t *first)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_timeline_wait w = { .handles = (uint64_t)(uintptr_t)h,
			.points = (uint64_t)(uintptr_t)pts, .timeout_nsec = timeout, .count_handles = n, .flags = flags };

		if(drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TIMELINE_WAIT, &w) < 0)
			return -errno;
		if(first != NULL)
			*first = w.first_signaled;
		return 0;
	}
	Dev *d;

	if((d = syncdev(fd)) == NULL)
		return -1;
	pthread_mutex_unlock(&lk);
	return syncwait(d, h, pts, n, timeout, flags, first);
}

int
drmSyncobjQuery2(int fd, uint32_t *h, uint64_t *pts, uint32_t n, uint32_t flags)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_timeline_array a = { .handles = (uint64_t)(uintptr_t)h,
			.points = (uint64_t)(uintptr_t)pts, .count_handles = n, .flags = flags };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_QUERY, &a);
	}
	Sync *s;
	Dev *d;
	uint32_t i;

	(void)flags;
	if((d = syncdev(fd)) == NULL)
		return -1;
	for(i = 0; i < n; i++){
		if((s = getsync(d, h[i])) == NULL){
			pthread_mutex_unlock(&lk);
			return seterr(ENOENT);
		}
		syncreap(s);
		if(flags & DRM_SYNCOBJ_QUERY_FLAGS_LAST_SUBMITTED)
			pts[i] = s->np > 0 ? s->pv[s->np-1] : s->payload;
		else
			pts[i] = s->payload;
	}
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjQuery(int fd, uint32_t *h, uint64_t *pts, uint32_t n)
{
	return drmSyncobjQuery2(fd, h, pts, n, 0);
}

int
drmSyncobjTransfer(int fd, uint32_t dst, uint64_t dstpt, uint32_t src, uint64_t srcpt, uint32_t flags)
{
	if(amd9of(fd) != NULL){
		struct drm_syncobj_transfer t = { .src_handle = src, .dst_handle = dst, .src_point = srcpt,
			.dst_point = dstpt, .flags = flags };

		return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TRANSFER, &t);
	}
	Sync *ds, *ss;
	Fence f;
	Dev *d;
	int r;

	(void)flags;
	if((d = syncdev(fd)) == NULL)
		return -1;
	if((ds = getsync(d, dst)) == NULL || (ss = getsync(d, src)) == NULL){
		pthread_mutex_unlock(&lk);
		return seterr(ENOENT);
	}
	if(syncfence(ss, srcpt, &f) < 0){
		pthread_mutex_unlock(&lk);
		return seterr(EINVAL);
	}
	r = syncadd(ds, dstpt, f);
	pthread_mutex_unlock(&lk);
	return r;
}

/* a syncobj as a descriptor (opaque): its handle travels with a sync file's fence */
int
drmSyncobjHandleToFD(int fd, uint32_t handle, int *out)
{
	if(amd9of(fd) != NULL)
		return seterr(EOPNOTSUPP);	/* no sync file descriptors for amdgpu here */
	return drmSyncobjExportSyncFile(fd, handle, out);
}

int
drmSyncobjFDToHandle(int fd, int sfd, uint32_t *handle)
{
	if(amd9of(fd) != NULL)
		return seterr(EOPNOTSUPP);	/* no sync file descriptors for amdgpu here */
	if(drmSyncobjCreate(fd, 0, handle) < 0)
		return -1;
	if(drmSyncobjImportSyncFile(fd, *handle, sfd) < 0){
		drmSyncobjDestroy(fd, *handle);
		return -1;
	}
	return 0;
}

int
drmSyncobjExportSyncFile(int fd, uint32_t handle, int *out)
{
	if(amd9of(fd) != NULL)
		return seterr(EOPNOTSUPP);	/* no sync file descriptors for amdgpu here */
	Fence f;
	Sync *s;
	Dev *d;
	int r;

	if((d = syncdev(fd)) == NULL)
		return -1;
	if((s = getsync(d, handle)) == NULL){
		pthread_mutex_unlock(&lk);
		return seterr(ENOENT);
	}
	if(syncfence(s, 0, &f) < 0){
		pthread_mutex_unlock(&lk);
		return seterr(EINVAL);
	}
	if((r = newfd(Ksyncfile, d)) < 0){
		pthread_mutex_unlock(&lk);
		return -1;
	}
	fds[r].f = f;
	fds[r].hasf = 1;
	*out = r;
	pthread_mutex_unlock(&lk);
	return 0;
}

int
drmSyncobjImportSyncFile(int fd, uint32_t handle, int sfd)
{
	if(amd9of(fd) != NULL)
		return seterr(EOPNOTSUPP);	/* no sync file descriptors for amdgpu here */
	Sync *s;
	Dev *d;
	Fd *f;
	int r;

	if((d = syncdev(fd)) == NULL)
		return -1;
	if((s = getsync(d, handle)) == NULL || (f = getfd(sfd, Ksyncfile)) == NULL){
		pthread_mutex_unlock(&lk);
		return seterr(EINVAL);
	}
	r = syncadd(s, 0, f->f);
	pthread_mutex_unlock(&lk);
	return r;
}

/* 9front: a GEM object on the display (gpu(4)'s ctx scanout); 0: the screen again */
int
drm9Scanout(int fd, uint32_t handle, uint32_t w, uint32_t h, uint32_t stride)
{
	Dev *d;
	Gem *g;
	int r;

	pthread_mutex_lock(&lk);
	if((d = getdev(fd)) == NULL){
		pthread_mutex_unlock(&lk);
		dbg("scanout: %d is not a GPU context", fd);
		return seterr(EBADF);
	}
	if(handle == 0)
		r = ctl(d, NULL, 0, "scanout 0");
	else if(d->amd != NULL)	/* amdgpu: the client's own GEM handle (aux/amdgpu's scanoutgem) */
		r = ctl(d, NULL, 0, "scanoutgem %u %u %u %u", handle, w, h, stride);
	else if((g = getgem(d, handle)) == NULL){
		dbg("scanout: no GEM object %u", handle);
		r = seterr(ENOENT);
	}else
		r = ctl(d, NULL, 0, "scanout %d %u %u %u", g->id, w, h, stride);
	pthread_mutex_unlock(&lk);
	return r;
}
