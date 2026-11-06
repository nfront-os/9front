/*
 * SPDX-License-Identifier: MIT
 *
 * libdrm_amdgpu on 9front, and amdgpu's ioctls, over aux/amdgpu
 * (gpu(4)): what Mesa's RADV (src/amd/common/ac_linux_drm.c) uses.
 *
 * A descriptor is a context of aux/amdgpu, opened by /dev/gpu/N/clone
 * (drm9.c finds it).  Each ioctl RADV makes becomes a request as text
 * (gpu(4)): words written to the context's ctl (CS: its submit file),
 * the reply read back on the same fid; nothing of ours is in the
 * driver's memory or its in ours.  The driver is Linux's amdgpu: its
 * ioctl numbers are Linux's (here BSD's encoding, sys/ioccom.h), its
 * errors named (aux/amdgpu fs.c), here APE's errno values again.
 *
 *	libdrm_amdgpu	device, objects (GEM_CREATE; cpu_map: the driver's
 *			"map", a segment), the GPU's address space (a
 *			manager of its own over AMDGPU_INFO's ranges, as
 *			libdrm's)
 *	dma-buf		amdgpu_bo_export/import of dma-buf descriptors:
 *			the driver's export and import by a token
 *			(gpu(4)), the token as a descriptor
 *			(lib/dmabuf.c); an object shared so is one
 *			amdgpu_bo with references, as libdrm's
 *	not here	userptr objects, sync file descriptors and the
 *			raw PRIME and syncobj fd ioctls, user queues
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <xf86drm.h>
#include <amdgpu.h>
#include <drm-uapi/drm.h>
#include <drm-uapi/amdgpu_drm.h>

#include "amdgpu9.h"

#ifndef ETIME
#define ETIME	ETIMEDOUT
#endif

/* libmesaplan9 (lib/dmabuf.c): a token of gpu(4)'s export as a dma-buf descriptor */
extern int	plan9_dmabuf_fd(const char *token);
extern int	plan9_dmabuf_token(int fd, char *buf, int n);
/* libmesaplan9 (lib/segfault.c): attached here and in the program's other procs when they touch it */
extern void	*mesa9_segattach(const char*, void*, unsigned long long);
extern int	mesa9_segdetach(void*);
extern int	_ERRSTR(char*, unsigned int);

enum {
	Nfid	= 8,		/* ctl and submit fids kept open, each */
	Maxbo	= 4096,
};

struct Amd {
	int	fd;		/* the context's ctl */
	int	ctx;
	char	dir[32];	/* /dev/gpu/N */
	pthread_mutex_t	lk;
	int	tfids[2][Nfid];	/* requests: ctl's, submit's */
	int	ntfids[2];
};

/* DRM9DEBUG=1: the ioctls that fail, as drm9.c's */
static void
dbg(const char *fmt, ...)
{
	static int on = -1;
	va_list arg;
	int e;

	e = errno;
	if(on < 0)
		on = getenv("DRM9DEBUG") != NULL;
	if(on){
		fprintf(stderr, "amdgpu9: ");
		va_start(arg, fmt);
		vfprintf(stderr, fmt, arg);
		va_end(arg);
		fprintf(stderr, "\n");
	}
	errno = e;
}

static int
ctlreq(Amd *a, char *reply, int n, const char *fmt, ...)
{
	char buf[256];
	va_list arg;
	int m;

	va_start(arg, fmt);
	m = vsnprintf(buf, sizeof buf, fmt, arg);
	va_end(arg);
	pthread_mutex_lock(&a->lk);	/* the reply is the context's: one at a time */
	if(write(a->fd, buf, m) != m){
		pthread_mutex_unlock(&a->lk);
		return -1;
	}
	if(reply != NULL){
		m = pread(a->fd, reply, n-1, 0);
		if(m < 0){
			pthread_mutex_unlock(&a->lk);
			return -1;
		}
		reply[m] = 0;
	}
	pthread_mutex_unlock(&a->lk);
	return 0;
}

void
amd9setfd(Amd *a, int fd)
{
	pthread_mutex_lock(&a->lk);
	a->fd = fd;
	pthread_mutex_unlock(&a->lk);
}

Amd*
amd9attach(int fd, const char *dir, int ctx)
{
	Amd *a;

	a = calloc(1, sizeof *a);
	if(a == NULL)
		return NULL;
	a->fd = fd;
	a->ctx = ctx;
	snprintf(a->dir, sizeof a->dir, "%s", dir);
	pthread_mutex_init(&a->lk, NULL);
	return a;
}

/* Linux's errno values (the driver's) as APE's */
static int
apeerrno(long e)
{
	switch(e){
	case 1:		return EPERM;
	case 2:		return ENOENT;
	case 3:		return ESRCH;
	case 4:		return EINTR;
	case 5:		return EIO;
	case 6:		return ENXIO;
	case 7:		return E2BIG;
	case 9:		return EBADF;
	case 11:	return EAGAIN;
	case 12:	return ENOMEM;
	case 13:	return EACCES;
	case 14:	return EFAULT;
	case 16:	return EBUSY;
	case 17:	return EEXIST;
	case 19:	return ENODEV;
	case 22:	return EINVAL;
	case 24:	return EMFILE;
	case 25:	return ENOTTY;
	case 28:	return ENOSPC;
	case 34:	return ERANGE;
	case 35:	return EDEADLK;
	case 38:	return ENOSYS;
	case 61:	return ENODATA;
	case 62:	return ETIME;
	case 75:	return EOVERFLOW;
	case 95:	return EOPNOTSUPP;
	case 110:	return ETIMEDOUT;
	case 125:	return ECANCELED;
	case 512:	return EINTR;	/* ERESTARTSYS */
	}
	return EIO;
}

#define P(x)	((void*)(uintptr_t)(x))
#define U(p)	((uint64_t)(uintptr_t)(p))

/* ioctls that pass descriptors or user memory: none here */
static int
refused(unsigned nr)
{
	switch(nr){
	case 0x2d: case 0x2e:			/* PRIME */
	case 0xC1: case 0xC2: case 0xCF:	/* syncobj fds, eventfd */
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_USERPTR:
	case DRM_COMMAND_BASE + DRM_AMDGPU_USERQ:
	case DRM_COMMAND_BASE + DRM_AMDGPU_USERQ_SIGNAL:
	case DRM_COMMAND_BASE + DRM_AMDGPU_USERQ_WAIT:
		return 1;
	case DRM_COMMAND_BASE + DRM_AMDGPU_FENCE_TO_HANDLE:
		return 0;	/* a syncobj only: checked below */
	}
	return 0;
}

/*
 * DRM9DEBUG=3: the IBs' contents with each CS; for that, which object is
 * at which GPU address (GEM_VA) and where the processor sees it (cpu_map)
 */
enum { Ndbgmap = 512 };
static struct {
	uint32_t	handle;
	uint64_t	va, size;
	void	*cpu;
} dbgmap[Ndbgmap];
static pthread_mutex_t dbglk = PTHREAD_MUTEX_INITIALIZER;
static uint32_t dbguserdata;	/* the last CS's COMPUTE_USER_DATA_0 (DRM9DEBUG=4) */

static int
dbglevel(void)
{
	static int l = -1;

	if(l < 0)
		l = getenv("DRM9DEBUG") != NULL ? atoi(getenv("DRM9DEBUG")) : 0;
	return l;
}

static void
dbgva(uint32_t handle, uint64_t va, uint64_t size, int map)
{
	int i, f;

	pthread_mutex_lock(&dbglk);
	f = -1;
	for(i = 0; i < Ndbgmap; i++){
		if(dbgmap[i].size != 0 && dbgmap[i].handle == handle && dbgmap[i].va == va){
			if(!map)
				dbgmap[i].size = 0;
			f = -2;
			break;
		}
		if(dbgmap[i].size == 0 && f == -1)
			f = i;
	}
	if(map && f >= 0){
		dbgmap[f].handle = handle;
		dbgmap[f].va = va;
		dbgmap[f].size = size;
		dbgmap[f].cpu = NULL;
	}
	pthread_mutex_unlock(&dbglk);
}

static void
dbgcpu(uint32_t handle, void *cpu)
{
	int i;

	pthread_mutex_lock(&dbglk);
	for(i = 0; i < Ndbgmap; i++)
		if(dbgmap[i].size != 0 && dbgmap[i].handle == handle)
			dbgmap[i].cpu = cpu;
	pthread_mutex_unlock(&dbglk);
}

/* n bytes at GPU address va, as the processor sees them */
static void
dbgwords(uint64_t va, uint32_t n)
{
	uint32_t *p, i;
	int k;

	p = NULL;
	pthread_mutex_lock(&dbglk);
	for(k = 0; k < Ndbgmap; k++)
		if(dbgmap[k].size != 0 && dbgmap[k].cpu != NULL && va >= dbgmap[k].va && va + n <= dbgmap[k].va + dbgmap[k].size){
			p = (uint32_t*)((char*)dbgmap[k].cpu + (va - dbgmap[k].va));
			break;
		}
	pthread_mutex_unlock(&dbglk);
	if(p == NULL){
		fprintf(stderr, " not mapped for the processor\n");
		return;
	}
	for(i = 0; i < n/4; i++)
		fprintf(stderr, " %08x", p[i]);
	fprintf(stderr, "\n");
}

/* the dwords at GPU address va, if the processor has them; then the IBs it calls (INDIRECT_BUFFER) */
static void
dbgdump(uint64_t va, uint32_t bytes)
{
	uint32_t *p, i, n, h;
	int k;

	p = NULL;
	pthread_mutex_lock(&dbglk);
	for(k = 0; k < Ndbgmap; k++)
		if(dbgmap[k].size != 0 && dbgmap[k].cpu != NULL && va >= dbgmap[k].va && va + bytes <= dbgmap[k].va + dbgmap[k].size){
			p = (uint32_t*)((char*)dbgmap[k].cpu + (va - dbgmap[k].va));
			break;
		}
	pthread_mutex_unlock(&dbglk);
	if(p == NULL){
		fprintf(stderr, "amdgpu9: ib %#llx: not mapped for the processor\n", (unsigned long long)va);
		return;
	}
	fprintf(stderr, "amdgpu9: ib %#llx:", (unsigned long long)va);
	for(i = 0; i < bytes/4; i++)
		fprintf(stderr, "%s%08x", i % 8 == 0 ? "\namdgpu9:  " : " ", p[i]);
	fprintf(stderr, "\n");
	/* PM4 type 3 packets: header, count+1 dwords; INDIRECT_BUFFER is opcode 0x3f */
	for(i = 0; i < bytes/4; i += n + 2){
		h = p[i];
		if(h >> 30 != 3){
			n = -1;	/* type 2 (one dword) and others: step one */
			continue;
		}
		n = h >> 16 & 0x3fff;
		if((h >> 8 & 0xff) == 0x3f && n == 2 && i + 3 < bytes/4)
			dbgdump((uint64_t)p[i+1] | (uint64_t)p[i+2] << 32, (p[i+3] & 0xfffff) * 4);
		/* SET_SH_REG COMPUTE_USER_DATA_0 (0x2e40: 0x240 in the SH space): a 32-bit pointer, its 48 bytes */
		if((h >> 8 & 0xff) == 0x76 && i + 2 < bytes/4 && (p[i+1] & 0xffff) == 0x240){
			dbguserdata = p[i+2];
			fprintf(stderr, "amdgpu9: COMPUTE_USER_DATA_0 %#x:", p[i+2]);
			dbgwords(0xffff800000000000ULL | p[i+2], 48);
		}
	}
}

/* DRM9DEBUG=2: GEM_CREATE's request (the answer overwrites it) */
static void
tracecreate(void *arg)
{
	union drm_amdgpu_gem_create *g = arg;

	if(getenv("DRM9DEBUG") != NULL && atoi(getenv("DRM9DEBUG")) >= 2)
		fprintf(stderr, "amdgpu9: gem_create size %#llx align %#llx domains %#llx flags %#llx\n",
			(unsigned long long)g->in.bo_size, (unsigned long long)g->in.alignment,
			(unsigned long long)g->in.domains, (unsigned long long)g->in.domain_flags);
}

/* DRM9DEBUG=2: every ioctl, with what CS and syncobj waits carry */
static void
trace(unsigned nr, void *arg, int r)
{
	static int on = -1;
	union drm_amdgpu_cs *cs;
	struct drm_amdgpu_cs_chunk *ch;
	struct drm_syncobj_wait *w;
	uint64_t *cp;
	uint32_t *h, i;
	int e;

	e = errno;
	if(on < 0)
		on = getenv("DRM9DEBUG") != NULL && atoi(getenv("DRM9DEBUG")) >= 2;
	if(!on)
		return;
	switch(nr){
	case DRM_COMMAND_BASE + DRM_AMDGPU_CS:
		cs = arg;
		fprintf(stderr, "amdgpu9: cs ctx %u chunks %u bo_list %u: %d", cs->in.ctx_id, cs->in.num_chunks,
			cs->in.bo_list_handle, r);
		cp = (uint64_t*)(uintptr_t)cs->in.chunks;
		for(i = 0; i < cs->in.num_chunks; i++){
			ch = (struct drm_amdgpu_cs_chunk*)(uintptr_t)cp[i];
			fprintf(stderr, " [%u %u]", ch->chunk_id, ch->length_dw);
			if(ch->chunk_id == AMDGPU_CHUNK_ID_IB){
				struct drm_amdgpu_cs_chunk_ib *ib = (void*)(uintptr_t)ch->chunk_data;

				fprintf(stderr, "=ib(flags %#x ip %u/%u ring %u va %#llx %u bytes)", ib->flags, ib->ip_type,
					ib->ip_instance, ib->ring, (unsigned long long)ib->va_start, ib->ib_bytes);
			}
			if(ch->chunk_id == AMDGPU_CHUNK_ID_FENCE){
				struct drm_amdgpu_cs_chunk_fence *fe = (void*)(uintptr_t)ch->chunk_data;

				fprintf(stderr, "=fence(bo %u offset %u)", fe->handle, fe->offset);
			}
			if(ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_SIGNAL || ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT){
				struct drm_amdgpu_cs_chunk_syncobj *sy = (void*)(uintptr_t)ch->chunk_data;
				uint32_t k;

				for(k = 0; k < ch->length_dw * 4 / sizeof *sy; k++)
					fprintf(stderr, "=syncobj(%u flags %#x point %llu)", sy[k].handle, sy[k].flags,
						(unsigned long long)sy[k].point);
			}
			if(ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_OUT || ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_IN)
				fprintf(stderr, "=%u", ((struct drm_amdgpu_cs_chunk_sem*)(uintptr_t)ch->chunk_data)->handle);
		}
		if(r == 0)
			fprintf(stderr, " seq %llu", (unsigned long long)cs->out.handle);
		if(dbglevel() >= 3){
			fprintf(stderr, "\n");
			for(i = 0; i < cs->in.num_chunks; i++){
				ch = (struct drm_amdgpu_cs_chunk*)(uintptr_t)cp[i];
				if(ch->chunk_id == AMDGPU_CHUNK_ID_IB)
					dbgdump(((struct drm_amdgpu_cs_chunk_ib*)(uintptr_t)ch->chunk_data)->va_start,
						((struct drm_amdgpu_cs_chunk_ib*)(uintptr_t)ch->chunk_data)->ib_bytes);
			}
			errno = e;
			return;
		}
		break;
	case 0xc3 /* DRM_IOCTL_SYNCOBJ_WAIT */:
		w = arg;
		h = (uint32_t*)(uintptr_t)w->handles;
		fprintf(stderr, "amdgpu9: syncobj wait flags %#x timeout %lld:", w->flags, (long long)w->timeout_nsec);
		for(i = 0; i < w->count_handles; i++)
			fprintf(stderr, " %u", h[i]);
		fprintf(stderr, ": %d", r);
		break;
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_CREATE: {
		union drm_amdgpu_gem_create *g = arg;

		/* after the ioctl the union holds the answer: the request's fields are gone then */
		fprintf(stderr, "amdgpu9: gem_create: handle %u: %d", g->out.handle, r);
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_VA: {
		struct drm_amdgpu_gem_va *v = arg;

		fprintf(stderr, "amdgpu9: gem_va op %u handle %u flags %#x va %#llx size %#llx: %d", v->operation,
			v->handle, v->flags, (unsigned long long)v->va_address, (unsigned long long)v->map_size, r);
		if(r == 0 && (v->operation == AMDGPU_VA_OP_MAP || v->operation == AMDGPU_VA_OP_UNMAP))
			dbgva(v->handle, v->va_address, v->map_size, v->operation == AMDGPU_VA_OP_MAP);
		break;
	}
	default:
		fprintf(stderr, "amdgpu9: ioctl %#x: %d", nr, r);
	}
	if(r < 0)
		fprintf(stderr, " (%s)", strerror(e));
	fprintf(stderr, "\n");
	errno = e;
}

/*
 * DRM9DEBUG=4: the GPU's view of memory: after a gfx CS, sdma copies n
 * bytes at va (in the CS's address space) into a GTT object of ours;
 * printed beside the processor's view (dbgwords).  Two objects of our
 * own, GTT, at the top of the 32-bit range.
 */
static int inpeek;

static void
dbgpeek(Amd *a, uint32_t ctx, uint64_t va, uint32_t n)
{
	static uint32_t hib, hdst, *ibp, *dstp;
	static int ready;
	const uint64_t ibva = 0xffff8000fff00000ULL, dstva = ibva + 0x10000;
	union drm_amdgpu_gem_create gc;
	struct drm_amdgpu_gem_va gv;
	struct drm_amdgpu_bo_list_entry be[3];
	struct drm_amdgpu_bo_list_in bl;
	struct drm_amdgpu_cs_chunk ch[2];
	struct drm_amdgpu_cs_chunk_ib ib;
	uint64_t chp[2];
	union drm_amdgpu_cs cs;
	union drm_amdgpu_wait_cs w;
	struct timespec ts;
	char reply[256], seg[64];
	unsigned long long sva, ssize;
	uint32_t src, i, *hp[2];
	uint64_t vas[2];
	int k;

	src = 0;
	pthread_mutex_lock(&dbglk);
	for(k = 0; k < Ndbgmap; k++)
		if(dbgmap[k].size != 0 && va >= dbgmap[k].va && va + n <= dbgmap[k].va + dbgmap[k].size)
			src = dbgmap[k].handle;
	pthread_mutex_unlock(&dbglk);
	if(src == 0 || n > 4096)
		return;
	inpeek = 1;
	if(!ready){
		hp[0] = &hib;
		hp[1] = &hdst;
		vas[0] = ibva;
		vas[1] = dstva;
		for(k = 0; k < 2; k++){
			memset(&gc, 0, sizeof gc);
			gc.in.bo_size = 4096;
			gc.in.alignment = 4096;
			gc.in.domains = AMDGPU_GEM_DOMAIN_GTT;
			if(amd9ioctl(a, DRM_IOCTL_AMDGPU_GEM_CREATE, &gc) < 0)
				goto out;
			*hp[k] = gc.out.handle;
			memset(&gv, 0, sizeof gv);
			gv.handle = *hp[k];
			gv.operation = AMDGPU_VA_OP_MAP;
			gv.flags = AMDGPU_VM_PAGE_READABLE | AMDGPU_VM_PAGE_WRITEABLE | AMDGPU_VM_PAGE_EXECUTABLE;
			gv.va_address = vas[k];
			gv.map_size = 4096;
			if(amd9ioctl(a, DRM_IOCTL_AMDGPU_GEM_VA, &gv) < 0)
				goto out;
			if(ctlreq(a, reply, sizeof reply, "map %u", *hp[k]) < 0 ||
			   sscanf(reply, "map %*u %63s %lli %lli", seg, &sva, &ssize) != 3)
				goto out;
			if(k == 0)
				ibp = mesa9_segattach(seg, NULL, ssize);
			else
				dstp = mesa9_segattach(seg, NULL, ssize);
		}
		if(ibp == (void*)-1 || dstp == (void*)-1)
			goto out;
		ready = 1;
	}
	/* sdma_v6_0: COPY_LINEAR, then a NOP: 8 dwords */
	ibp[0] = 1;
	ibp[1] = n - 1;
	ibp[2] = 0;
	ibp[3] = va;
	ibp[4] = va >> 32;
	ibp[5] = (uint32_t)dstva;
	ibp[6] = dstva >> 32;
	ibp[7] = 0;
	memset(dstp, 0xee, n);
	memset(be, 0, sizeof be);
	be[0].bo_handle = hib;
	be[1].bo_handle = hdst;
	be[2].bo_handle = src;
	memset(&bl, 0, sizeof bl);
	bl.bo_number = 3;
	bl.bo_info_size = sizeof be[0];
	bl.bo_info_ptr = (uintptr_t)be;
	memset(&ib, 0, sizeof ib);
	ib.va_start = ibva;
	ib.ib_bytes = 32;
	ib.ip_type = AMDGPU_HW_IP_DMA;
	memset(ch, 0, sizeof ch);
	ch[0].chunk_id = AMDGPU_CHUNK_ID_IB;
	ch[0].length_dw = sizeof ib / 4;
	ch[0].chunk_data = (uintptr_t)&ib;
	ch[1].chunk_id = AMDGPU_CHUNK_ID_BO_HANDLES;
	ch[1].length_dw = sizeof bl / 4;
	ch[1].chunk_data = (uintptr_t)&bl;
	chp[0] = (uintptr_t)&ch[0];
	chp[1] = (uintptr_t)&ch[1];
	memset(&cs, 0, sizeof cs);
	cs.in.ctx_id = ctx;
	cs.in.num_chunks = 2;
	cs.in.chunks = (uintptr_t)chp;
	if(amd9ioctl(a, DRM_IOCTL_AMDGPU_CS, &cs) < 0){
		fprintf(stderr, "amdgpu9: peek %#llx: CS: %s\n", (unsigned long long)va, strerror(errno));
		goto out;
	}
	memset(&w, 0, sizeof w);
	w.in.handle = cs.out.handle;
	w.in.ip_type = AMDGPU_HW_IP_DMA;
	w.in.ctx_id = ctx;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	w.in.timeout = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec + 1000000000ULL;
	if(amd9ioctl(a, DRM_IOCTL_AMDGPU_WAIT_CS, &w) < 0 || w.out.status != 0){
		fprintf(stderr, "amdgpu9: peek %#llx: not done\n", (unsigned long long)va);
		goto out;
	}
	fprintf(stderr, "amdgpu9: peek %#llx gpu:", (unsigned long long)va);
	for(i = 0; i < n/4; i++)
		fprintf(stderr, " %08x", dstp[i]);
	fprintf(stderr, "\namdgpu9: peek %#llx cpu:", (unsigned long long)va);
	dbgwords(va, n);
out:
	inpeek = 0;
}

/*
 * the ioctls as text (gpu(4); aux/amdgpu's linux/req.c)
 */
enum {
	Maxreq	= 32*1024 - 64,	/* a 9P message (devmnt's MAXRPC) */
	Maxrep	= 64*1024,
	Notext	= -2,		/* no request for it */
};

/* the driver's names of Linux's errors (aux/amdgpu fs.c: errstr9) */
static const struct {
	const char	*s;
	int	e;
} errs[] = {
	{ "operation not permitted", EPERM }, { "no such object", ENOENT },
	{ "no such process", ESRCH }, { "interrupted", EINTR }, { "i/o error", EIO },
	{ "no such device or address", ENXIO }, { "too big", E2BIG }, { "bad handle", EBADF },
	{ "try again", EAGAIN }, { "out of memory", ENOMEM }, { "permission denied", EACCES },
	{ "bad address", EFAULT }, { "busy", EBUSY }, { "exists", EEXIST }, { "no device", ENODEV },
	{ "bad request", EINVAL }, { "too many open files", EMFILE }, { "not an ioctl", ENOTTY },
	{ "no space", ENOSPC }, { "out of range", ERANGE }, { "deadlock", EDEADLK },
	{ "not implemented", ENOSYS }, { "no data", ENODATA }, { "timer expired", ETIME },
	{ "overflow", EOVERFLOW }, { "not supported", EOPNOTSUPP },
	{ "GPU address in use or out of range", EINVAL }, { "timed out", ETIMEDOUT },
	{ "canceled", ECANCELED },
};

static int
errnoof(const char *s)
{
	unsigned i;
	int n;

	for(i = 0; i < sizeof errs / sizeof errs[0]; i++)
		if(strcmp(errs[i].s, s) == 0)
			return errs[i].e;
	if(sscanf(s, "error %d", &n) == 1)
		return apeerrno(n);
	return EIO;
}

/* a fid of the context's ctl (0) or submit (1), kept open */
static int
tfidget(Amd *a, int sub)
{
	char path[64];
	int fd;

	pthread_mutex_lock(&a->lk);
	if(a->ntfids[sub] > 0){
		fd = a->tfids[sub][--a->ntfids[sub]];
		pthread_mutex_unlock(&a->lk);
		return fd;
	}
	pthread_mutex_unlock(&a->lk);
	snprintf(path, sizeof path, "%s/ctx/%d/%s", a->dir, a->ctx, sub ? "submit" : "ctl");
	return open(path, O_RDWR);
}

static void
tfidput(Amd *a, int sub, int fd)
{
	pthread_mutex_lock(&a->lk);
	if(a->ntfids[sub] < Nfid){
		a->tfids[sub][a->ntfids[sub]++] = fd;
		fd = -1;
	}
	pthread_mutex_unlock(&a->lk);
	if(fd >= 0)
		close(fd);
}

/* a request being written */
typedef struct Tb Tb;
struct Tb {
	char	s[Maxreq+1];
	int	n;
	int	over;
};

static void
tprint(Tb *b, const char *fmt, ...)
{
	va_list arg;
	int m;

	if(b->over)
		return;
	va_start(arg, fmt);
	m = vsnprintf(b->s + b->n, sizeof b->s - b->n, fmt, arg);
	va_end(arg);
	if(m < 0 || b->n + m >= (int)sizeof b->s)
		b->over = 1;
	else
		b->n += m;
}

/*
 * the request in b, on ctl or submit; its reply in rep (Maxrep bytes, NUL
 * terminated): 0, or -1 and errno
 */
static int
treq(Amd *a, int sub, Tb *b, char *rep)
{
	char err[256];
	long n, m;
	int fd;

	if(b->over)
		return errno = E2BIG, -1;
	if((fd = tfidget(a, sub)) < 0)
		return -1;
	if(write(fd, b->s, b->n) != b->n){
		err[0] = 0;
		_ERRSTR(err, sizeof err);
		tfidput(a, sub, fd);
		dbg("%.*s: %s", b->n, b->s, err);
		return errno = errnoof(err), -1;
	}
	for(n = 0; n < Maxrep - 1; n += m)
		if((m = pread(fd, rep + n, Maxrep - 1 - n, n)) <= 0)
			break;
	rep[n] = 0;
	tfidput(a, sub, fd);
	return 0;
}

/* the reply's words after its first, "verb w1 w2 ..." */
static int
twords(char *rep, const char *verb, char **w, int max)
{
	char *p;
	int n;

	n = 0;
	p = rep;
	while(*p == ' ')
		p++;
	if(strncmp(p, verb, strlen(verb)) != 0)
		return -1;
	p += strlen(verb);
	while(n < max){
		while(*p == ' ' || *p == '\n')
			*p++ = 0;
		if(*p == 0)
			break;
		w[n++] = p;
		while(*p && *p != ' ' && *p != '\n')
			p++;
	}
	return n;
}

#define TU(s)	strtoull((s), NULL, 0)

/* the driver's names of domains (0) and creation flags (1), back to bits */
typedef struct Tname Tname;
struct Tname {
	const char	*s;
	uint64_t	bit;
};

static uint64_t
tbits(char *s, int flags)
{
	static const Tname dom[] = {
		{ "cpu", AMDGPU_GEM_DOMAIN_CPU }, { "gtt", AMDGPU_GEM_DOMAIN_GTT },
		{ "vram", AMDGPU_GEM_DOMAIN_VRAM }, { "gds", AMDGPU_GEM_DOMAIN_GDS },
		{ "gws", AMDGPU_GEM_DOMAIN_GWS }, { "oa", AMDGPU_GEM_DOMAIN_OA },
		{ "doorbell", AMDGPU_GEM_DOMAIN_DOORBELL }, { NULL, 0 }
	}, fl[] = {
		{ "cpuaccess", AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED },
		{ "nocpuaccess", AMDGPU_GEM_CREATE_NO_CPU_ACCESS },
		{ "uswc", AMDGPU_GEM_CREATE_CPU_GTT_USWC }, { "cleared", AMDGPU_GEM_CREATE_VRAM_CLEARED },
		{ "contiguous", AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS },
		{ "local", AMDGPU_GEM_CREATE_VM_ALWAYS_VALID },
		{ "explicitsync", AMDGPU_GEM_CREATE_EXPLICIT_SYNC }, { "mqd", AMDGPU_GEM_CREATE_CP_MQD_GFX9 },
		{ "wipe", AMDGPU_GEM_CREATE_VRAM_WIPE_ON_RELEASE }, { "encrypted", AMDGPU_GEM_CREATE_ENCRYPTED },
		{ "preemptible", AMDGPU_GEM_CREATE_PREEMPTIBLE },
		{ "discardable", AMDGPU_GEM_CREATE_DISCARDABLE }, { "coherent", AMDGPU_GEM_CREATE_COHERENT },
		{ "uncached", AMDGPU_GEM_CREATE_UNCACHED }, { "extcoherent", AMDGPU_GEM_CREATE_EXT_COHERENT },
		{ "dcc", AMDGPU_GEM_CREATE_GFX12_DCC }, { NULL, 0 }
	};
	const Tname *t = flags ? fl : dom;
	uint64_t v;
	char *e;
	int i, n;

	v = 0;
	while(*s){
		n = strcspn(s, ",");
		if(*s >= '0' && *s <= '9')
			v |= strtoull(s, &e, 0);
		else
			for(i = 0; t[i].s != NULL; i++)
				if(strlen(t[i].s) == (size_t)n && strncmp(t[i].s, s, n) == 0)
					v |= t[i].bit;
		s += n;
		if(*s == ',')
			s++;
	}
	return v;
}

/*
 * INFO: the driver's "name value" lines into the caller's structure, by
 * the names of its fields (the structures of Mesa's header and Linux's
 * can differ: what one has and the other not stays zero)
 */
enum { T32, T64, TA32 };
typedef struct Fld Fld;
struct Fld {
	const char	*name;
	int	type;
	size_t	off;
	int	n;
};
#define D32(s, f)	{ #f, T32, offsetof(struct s, f), 1 }
#define D64(s, f)	{ #f, T64, offsetof(struct s, f), 1 }
#define DA(s, f, n)	{ #f, TA32, offsetof(struct s, f), n }
#define DN(s, p, f)	{ #p "." #f, T64, offsetof(struct s, p.f), 1 }
#define DEV	drm_amdgpu_info_device

static const Fld devfld[] = {
	D32(DEV, device_id), D32(DEV, chip_rev), D32(DEV, external_rev), D32(DEV, pci_rev),
	D32(DEV, family), D32(DEV, num_shader_engines), D32(DEV, num_shader_arrays_per_engine),
	D32(DEV, gpu_counter_freq), D64(DEV, max_engine_clock), D64(DEV, max_memory_clock),
	D32(DEV, cu_active_number), D32(DEV, cu_ao_mask), DA(DEV, cu_bitmap, 16),
	D32(DEV, enabled_rb_pipes_mask), D32(DEV, num_rb_pipes), D32(DEV, num_hw_gfx_contexts),
	D32(DEV, pcie_gen), D64(DEV, ids_flags), D64(DEV, virtual_address_offset),
	D64(DEV, virtual_address_max), D32(DEV, virtual_address_alignment), D32(DEV, pte_fragment_size),
	D32(DEV, gart_page_size), D32(DEV, ce_ram_size), D32(DEV, vram_type), D32(DEV, vram_bit_width),
	D32(DEV, vce_harvest_config), D32(DEV, gc_double_offchip_lds_buf), D64(DEV, prim_buf_gpu_addr),
	D64(DEV, pos_buf_gpu_addr), D64(DEV, cntl_sb_buf_gpu_addr), D64(DEV, param_buf_gpu_addr),
	D32(DEV, prim_buf_size), D32(DEV, pos_buf_size), D32(DEV, cntl_sb_buf_size), D32(DEV, param_buf_size),
	D32(DEV, wave_front_size), D32(DEV, num_shader_visible_vgprs), D32(DEV, num_cu_per_sh),
	D32(DEV, num_tcc_blocks), D32(DEV, gs_vgt_table_depth), D32(DEV, gs_prim_buffer_depth),
	D32(DEV, max_gs_waves_per_vgt), D32(DEV, pcie_num_lanes), DA(DEV, cu_ao_bitmap, 16),
	D64(DEV, high_va_offset), D64(DEV, high_va_max), D32(DEV, pa_sc_tile_steering_override),
	D64(DEV, tcc_disabled_mask), D64(DEV, min_engine_clock), D64(DEV, min_memory_clock),
	D32(DEV, tcp_cache_size), D32(DEV, num_sqc_per_wgp), D32(DEV, sqc_data_cache_size),
	D32(DEV, sqc_inst_cache_size), D32(DEV, gl1c_cache_size), D32(DEV, gl2c_cache_size),
	D64(DEV, mall_size), D32(DEV, enabled_rb_pipes_mask_hi), D32(DEV, shadow_size),
	D32(DEV, shadow_alignment), D32(DEV, csa_size), D32(DEV, csa_alignment), D32(DEV, userq_ip_mask),
	{ NULL }
};
#undef DEV

static const Fld hwipfld[] = {
	D32(drm_amdgpu_info_hw_ip, hw_ip_version_major), D32(drm_amdgpu_info_hw_ip, hw_ip_version_minor),
	D64(drm_amdgpu_info_hw_ip, capabilities_flags), D32(drm_amdgpu_info_hw_ip, ib_start_alignment),
	D32(drm_amdgpu_info_hw_ip, ib_size_alignment), D32(drm_amdgpu_info_hw_ip, available_rings),
	D32(drm_amdgpu_info_hw_ip, ip_discovery_version),
	{ NULL }
};

static const Fld vramgttfld[] = {
	D64(drm_amdgpu_info_vram_gtt, vram_size), D64(drm_amdgpu_info_vram_gtt, vram_cpu_accessible_size),
	D64(drm_amdgpu_info_vram_gtt, gtt_size),
	{ NULL }
};

#define HEAP(p)	DN(drm_amdgpu_memory_info, p, total_heap_size), DN(drm_amdgpu_memory_info, p, usable_heap_size), \
		DN(drm_amdgpu_memory_info, p, heap_usage), DN(drm_amdgpu_memory_info, p, max_allocation)
static const Fld memfld[] = {
	HEAP(vram), HEAP(cpu_accessible_vram), HEAP(gtt),
	{ NULL }
};
#undef HEAP

static const Fld fwfld[] = {
	D32(drm_amdgpu_info_firmware, ver), D32(drm_amdgpu_info_firmware, feature),
	{ NULL }
};

static const Fld faultfld[] = {
	D64(drm_amdgpu_info_gpuvm_fault, addr), D32(drm_amdgpu_info_gpuvm_fault, status),
	D32(drm_amdgpu_info_gpuvm_fault, vmhub),
	{ NULL }
};

static const Fld codecfld[] = {
	D32(drm_amdgpu_info_video_codec_info, valid), D32(drm_amdgpu_info_video_codec_info, max_width),
	D32(drm_amdgpu_info_video_codec_info, max_height),
	D32(drm_amdgpu_info_video_codec_info, max_pixels_per_frame),
	D32(drm_amdgpu_info_video_codec_info, max_level),
	{ NULL }
};

/* a field's value at off in out (size bytes): what fits */
static void
tput(unsigned char *out, size_t size, size_t off, int type, uint64_t v)
{
	uint32_t x;

	if(type == T64){
		if(off + 8 <= size)
			memcpy(out + off, &v, 8);
	}else{
		x = v;
		if(off + 4 <= size)
			memcpy(out + off, &x, 4);
	}
}

/*
 * "name v ..." lines into out by fields t; codec: "k.name v" into
 * codec_info[k].  The names only one side knows (a newer header on one of
 * them) go to DRM9DEBUG's output: a field of ours the driver did not send
 * stays 0 without that (tests/gpu/amdall.rc checks for them).
 */
enum { Maxfld = 128 };
static void
tfields(char *rep, const Fld *t, int codec, unsigned char *out, size_t size, unsigned query)
{
	char *line, *next, *name, *v, *e;
	const Fld *f;
	size_t base;
	int i, k;
	char seen[Maxfld];

	memset(seen, 0, sizeof seen);

	for(line = rep; line != NULL && *line; line = next){
		if((next = strchr(line, '\n')) != NULL)
			*next++ = 0;
		name = line;
		if((v = strchr(line, ' ')) == NULL)
			continue;
		*v++ = 0;
		base = 0;
		if(codec){
			k = strtol(name, &e, 10);
			if(*e != '.' || k < 0 || k >= AMDGPU_INFO_VIDEO_CAPS_CODEC_IDX_COUNT)
				continue;
			name = e + 1;
			base = k * sizeof(struct drm_amdgpu_info_video_codec_info);
		}
		for(f = t; f->name != NULL; f++)
			if(strcmp(f->name, name) == 0)
				break;
		if(f->name == NULL){
			dbg("info %#x: %s only in the driver's", query, name);
			continue;	/* Linux's, not in Mesa's header */
		}
		if(f - t < Maxfld)
			seen[f - t] = 1;
		if(f->type == TA32){
			for(i = 0; i < f->n && *v; i++){
				tput(out, size, base + f->off + 4*i, T32, strtoull(v, &e, 0));
				v = e;
			}
		}else
			tput(out, size, base + f->off, f->type, TU(v));
	}
	for(f = t; f->name != NULL && f - t < Maxfld; f++)
		if(!seen[f - t])
			dbg("info %#x: %s only in Mesa's, 0", query, f->name);
}

static int
tinfo(Amd *a, struct drm_amdgpu_info *in, Tb *b, char *rep)
{
	unsigned char *out = P(in->return_pointer);
	size_t size = in->return_size;
	const Fld *t = NULL;
	char *e, *v;
	int codec = 0, i;

	switch(in->query){
	case AMDGPU_INFO_ACCEL_WORKING:	tprint(b, "info accel"); break;
	case AMDGPU_INFO_DEV_INFO:	tprint(b, "info device"); t = devfld; break;
	case AMDGPU_INFO_HW_IP_INFO:
		tprint(b, "info hwip %u %u", in->query_hw_ip.type, in->query_hw_ip.ip_instance);
		t = hwipfld;
		break;
	case AMDGPU_INFO_HW_IP_COUNT:	tprint(b, "info hwipcount %u", in->query_hw_ip.type); break;
	case AMDGPU_INFO_FW_VERSION:
		tprint(b, "info fw %u %u %u", in->query_fw.fw_type, in->query_fw.ip_instance, in->query_fw.index);
		t = fwfld;
		break;
	case AMDGPU_INFO_VRAM_GTT:	tprint(b, "info vramgtt"); t = vramgttfld; break;
	case AMDGPU_INFO_MEMORY:	tprint(b, "info memory"); t = memfld; break;
	case AMDGPU_INFO_TIMESTAMP:	tprint(b, "info timestamp"); break;
	case AMDGPU_INFO_NUM_BYTES_MOVED:	tprint(b, "info moved"); break;
	case AMDGPU_INFO_NUM_EVICTIONS:	tprint(b, "info evictions"); break;
	case AMDGPU_INFO_NUM_VRAM_CPU_PAGE_FAULTS:	tprint(b, "info vramfaults"); break;
	case AMDGPU_INFO_VRAM_USAGE:	tprint(b, "info vramusage"); break;
	case AMDGPU_INFO_VIS_VRAM_USAGE:	tprint(b, "info visvramusage"); break;
	case AMDGPU_INFO_GTT_USAGE:	tprint(b, "info gttusage"); break;
	case AMDGPU_INFO_GPUVM_FAULT:	tprint(b, "info vmfault"); t = faultfld; break;
	case AMDGPU_INFO_VIDEO_CAPS:
		tprint(b, "info video %u", in->video_cap.type);
		t = codecfld;
		codec = 1;
		break;
	case AMDGPU_INFO_SENSOR:	tprint(b, "info sensor %u", in->sensor_info.type); break;
	case AMDGPU_INFO_MAX_IBS:	tprint(b, "info maxibs"); break;
	case AMDGPU_INFO_READ_MMR_REG:
		tprint(b, "info mmr %u %u %u %u", in->read_mmr_reg.dword_offset, in->read_mmr_reg.count,
			in->read_mmr_reg.instance, in->read_mmr_reg.flags);
		break;
	default:
		return errno = EINVAL, -1;	/* no word for it */
	}
	if(treq(a, 0, b, rep) < 0)
		return -1;
	memset(out, 0, size);
	if(t != NULL){
		tfields(rep, t, codec, out, size, in->query);
		return 0;
	}
	/* numbers: one (32 or 64 bits by the size asked for) or 32-bit ones */
	v = rep;
	if(size == 8 && in->query != AMDGPU_INFO_READ_MMR_REG && in->query != AMDGPU_INFO_MAX_IBS){
		tput(out, size, 0, T64, TU(v));
		return 0;
	}
	for(i = 0; (size_t)(4*i + 4) <= size && *v && *v != '\n'; i++){
		tput(out, size, 4*i, T32, strtoull(v, &e, 0));
		v = e;
		while(*v == ' ')
			v++;
	}
	return 0;
}

/* CS: its chunks as words, to the submit file */
static int
tcs(Amd *a, union drm_amdgpu_cs *c, Tb *b, char *rep)
{
	uint64_t *chp = P(c->in.chunks);
	struct drm_amdgpu_cs_chunk *ch;
	char *w[2];
	uint32_t i, j, n;

	if(c->in.bo_list_handle != 0)
		return errno = EOPNOTSUPP, -1;	/* BO lists by handle (BO_LIST): none, a CS names its BOs */
	tprint(b, "cs %u %u", c->in.ctx_id, c->in.flags);
	for(i = 0; i < c->in.num_chunks; i++){
		ch = P(chp[i]);
		switch(ch->chunk_id){
		case AMDGPU_CHUNK_ID_IB: {
			struct drm_amdgpu_cs_chunk_ib *ib = P(ch->chunk_data);

			tprint(b, " ib %u %u %u %#llx %u %u", ib->ip_type, ib->ip_instance, ib->ring,
				(unsigned long long)ib->va_start, ib->ib_bytes, ib->flags);
			break;
		}
		case AMDGPU_CHUNK_ID_FENCE: {
			struct drm_amdgpu_cs_chunk_fence *fe = P(ch->chunk_data);

			tprint(b, " fence %u %u", fe->handle, fe->offset);
			break;
		}
		case AMDGPU_CHUNK_ID_SYNCOBJ_IN:
		case AMDGPU_CHUNK_ID_SYNCOBJ_OUT: {
			struct drm_amdgpu_cs_chunk_sem *s = P(ch->chunk_data);

			n = ch->length_dw * 4 / sizeof *s;
			for(j = 0; j < n; j++)
				tprint(b, " %s %u", ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_IN ? "in" : "out", s[j].handle);
			break;
		}
		case AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT:
		case AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_SIGNAL: {
			struct drm_amdgpu_cs_chunk_syncobj *s = P(ch->chunk_data);

			n = ch->length_dw * 4 / sizeof *s;
			for(j = 0; j < n; j++)
				tprint(b, " %s %u %llu %u", ch->chunk_id == AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT ? "twait" : "tsignal",
					s[j].handle, (unsigned long long)s[j].point, s[j].flags);
			break;
		}
		case AMDGPU_CHUNK_ID_BO_HANDLES: {
			struct drm_amdgpu_bo_list_in *bl = P(ch->chunk_data);
			unsigned char *e = P(bl->bo_info_ptr);

			for(j = 0; j < bl->bo_number; j++){
				struct drm_amdgpu_bo_list_entry *x = (void*)(e + (uint64_t)j * bl->bo_info_size);

				tprint(b, " bo %u %u", x->bo_handle, x->bo_priority);
			}
			break;
		}
		default:
			dbg("CS chunk %u: no word for it", ch->chunk_id);
			return errno = EINVAL, -1;
		}
	}
	if(treq(a, 1, b, rep) < 0)
		return -1;
	if(twords(rep, "cs", w, 1) != 1)
		return errno = EIO, -1;
	c->out.handle = TU(w[0]);
	return 0;
}

/* syncobj handles, and points (timeline) */
static void
tsyncs(Tb *b, uint64_t handles, uint64_t points, uint32_t n)
{
	uint32_t *h = P(handles), i;
	uint64_t *pt = P(points);

	for(i = 0; i < n; i++)
		if(pt != NULL)
			tprint(b, " %u %llu", h[i], (unsigned long long)pt[i]);
		else
			tprint(b, " %u", h[i]);
}

/* as Linux's VERSION: the string s (to the line's end) up to len bytes at dst; its length */
static size_t
tstr(char *dst, size_t len, const char *s)
{
	size_t n;

	n = strcspn(s, "\n");
	if(dst != NULL && len > 0)
		memcpy(dst, s, n < len ? n : len);
	return n;
}

/* the ioctl as text: 0, -1 and errno, or Notext */
static int textioctl1(Amd*, unsigned, void*, Tb*, char*);

static int
textioctl(Amd *a, unsigned nr, void *arg)
{
	char *rep;
	Tb *b;
	int r, e;

	b = malloc(sizeof *b);
	rep = malloc(Maxrep);
	if(b == NULL || rep == NULL){
		free(b);
		free(rep);
		return errno = ENOMEM, -1;
	}
	b->n = 0;
	b->over = 0;
	r = textioctl1(a, nr, arg, b, rep);
	e = errno;
	free(rep);
	free(b);
	errno = e;
	return r;
}

static int
textioctl1(Amd *a, unsigned nr, void *arg, Tb *b, char *rep)
{
	char *w[8];

	switch(nr){
	case 0x00: {	/* VERSION */
		struct drm_version *v = arg;
		char *l;
		int maj, min, pl;

		tprint(b, "version");
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(sscanf(rep, "version %d %d %d", &maj, &min, &pl) != 3)
			return errno = EIO, -1;
		v->version_major = maj;
		v->version_minor = min;
		v->version_patchlevel = pl;
		for(l = strchr(rep, '\n'); l != NULL && l[1]; l = strchr(l + 1, '\n')){
			if(strncmp(l + 1, "name ", 5) == 0)
				v->name_len = tstr(v->name, v->name_len, l + 6);
			else if(strncmp(l + 1, "date ", 5) == 0)
				v->date_len = tstr(v->date, v->date_len, l + 6);
			else if(strncmp(l + 1, "desc ", 5) == 0)
				v->desc_len = tstr(v->desc, v->desc_len, l + 6);
		}
		return 0;
	}
	case 0x0c: {	/* GET_CAP */
		struct drm_get_cap *g = arg;

		tprint(b, "cap %llu", (unsigned long long)g->capability);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "cap", w, 2) != 2)
			return errno = EIO, -1;
		g->value = TU(w[1]);
		return 0;
	}
	case 0x09:	/* GEM_CLOSE */
		tprint(b, "close %u", ((struct drm_gem_close*)arg)->handle);
		return treq(a, 0, b, rep);
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_CREATE: {
		union drm_amdgpu_gem_create *g = arg;

		tprint(b, "gem %llu %llu %#llx %#llx", (unsigned long long)g->in.bo_size,
			(unsigned long long)g->in.alignment, (unsigned long long)g->in.domains,
			(unsigned long long)g->in.domain_flags);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "gem", w, 1) != 1)
			return errno = EIO, -1;
		memset(&g->out, 0, sizeof g->out);
		g->out.handle = TU(w[0]);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_OP: {
		struct drm_amdgpu_gem_op *o = arg;
		struct drm_amdgpu_gem_create_in *ci;

		if(o->op == AMDGPU_GEM_OP_SET_PLACEMENT){
			tprint(b, "place %u %#llx", o->handle, (unsigned long long)o->value);
			return treq(a, 0, b, rep);
		}
		if(o->op != AMDGPU_GEM_OP_GET_GEM_CREATE_INFO)
			return errno = EINVAL, -1;
		tprint(b, "geminfo %u", o->handle);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "geminfo", w, 5) != 5)
			return errno = EIO, -1;
		ci = P(o->value);
		ci->bo_size = TU(w[1]);
		ci->alignment = TU(w[2]);
		ci->domains = tbits(w[3], 0);
		ci->domain_flags = tbits(w[4], 1);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_METADATA: {
		struct drm_amdgpu_gem_metadata *m = arg;
		unsigned char *d = (unsigned char*)m->data.data;
		uint32_t i, len;

		if(m->op == AMDGPU_GEM_METADATA_OP_SET_METADATA){
			len = m->data.data_size_bytes;
			if(len > sizeof m->data.data)
				return errno = EINVAL, -1;
			tprint(b, "meta %u %#llx %#llx ", m->handle, (unsigned long long)m->data.flags,
				(unsigned long long)m->data.tiling_info);
			for(i = 0; i < len; i++)
				tprint(b, "%02x", d[i]);
			tprint(b, "%s", len ? "" : "-");
			return treq(a, 0, b, rep);
		}
		tprint(b, "meta %u", m->handle);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "meta", w, 4) != 4)
			return errno = EIO, -1;
		m->data.flags = TU(w[1]);
		m->data.tiling_info = TU(w[2]);
		len = 0;
		if(strcmp(w[3], "-") != 0)
			for(; w[3][2*len] && w[3][2*len+1] && len < sizeof m->data.data; len++){
				char x[3] = { w[3][2*len], w[3][2*len+1], 0 };

				d[len] = strtoul(x, NULL, 16);
			}
		m->data.data_size_bytes = len;
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_VA: {
		struct drm_amdgpu_gem_va *v = arg;
		static const char *ops[] = { "map", "unmap", "clear", "replace" };
		uint32_t *in, i;

		if(v->operation < AMDGPU_VA_OP_MAP || v->operation > AMDGPU_VA_OP_REPLACE)
			return errno = EINVAL, -1;
		tprint(b, "va %s %u %#llx %#llx %#llx %#x", ops[v->operation - AMDGPU_VA_OP_MAP], v->handle,
			(unsigned long long)v->va_address, (unsigned long long)v->offset_in_bo,
			(unsigned long long)v->map_size, v->flags);
		if(v->vm_timeline_syncobj_out != 0)
			tprint(b, " out %u %llu", v->vm_timeline_syncobj_out, (unsigned long long)v->vm_timeline_point);
		if(v->num_syncobj_handles > 0){
			in = P(v->input_fence_syncobj_handles);
			tprint(b, " in");
			for(i = 0; i < v->num_syncobj_handles; i++)
				tprint(b, " %u", in[i]);
		}
		return treq(a, 0, b, rep);
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_WAIT_IDLE: {
		union drm_amdgpu_gem_wait_idle *wi = arg;

		tprint(b, "idle %u %llu", wi->in.handle, (unsigned long long)wi->in.timeout);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "idle", w, 2) != 2)
			return errno = EIO, -1;
		memset(&wi->out, 0, sizeof wi->out);
		wi->out.status = strcmp(w[0], "busy") == 0;
		wi->out.domain = TU(w[1]);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_CTX: {
		union drm_amdgpu_ctx *x = arg;
		uint32_t op = x->in.op, id = x->in.ctx_id;

		switch(op){
		case AMDGPU_CTX_OP_ALLOC_CTX:
			tprint(b, "hwctx new %d %u", x->in.priority, x->in.flags);
			if(treq(a, 0, b, rep) < 0)
				return -1;
			if(twords(rep, "hwctx", w, 1) != 1)
				return errno = EIO, -1;
			memset(&x->out, 0, sizeof x->out);
			x->out.alloc.ctx_id = TU(w[0]);
			return 0;
		case AMDGPU_CTX_OP_FREE_CTX:
			tprint(b, "hwctx free %u", id);
			return treq(a, 0, b, rep);
		case AMDGPU_CTX_OP_QUERY_STATE2:
			tprint(b, "hwctx state %u", id);
			if(treq(a, 0, b, rep) < 0)
				return -1;
			if(twords(rep, "hwctx state", w, 1) != 1)
				return errno = EIO, -1;
			memset(&x->out, 0, sizeof x->out);
			x->out.state.flags = TU(w[0]);
			return 0;
		case AMDGPU_CTX_OP_GET_STABLE_PSTATE:
		case AMDGPU_CTX_OP_SET_STABLE_PSTATE:
			if(op == AMDGPU_CTX_OP_SET_STABLE_PSTATE){
				tprint(b, "pstate %u %u", id, x->in.flags);
				return treq(a, 0, b, rep);
			}
			tprint(b, "pstate %u", id);
			if(treq(a, 0, b, rep) < 0)
				return -1;
			if(twords(rep, "pstate", w, 1) != 1)
				return errno = EIO, -1;
			memset(&x->out, 0, sizeof x->out);
			x->out.pstate.flags = TU(w[0]);
			return 0;
		}
		return errno = EINVAL, -1;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_VM: {
		union drm_amdgpu_vm *v = arg;

		if(v->in.op == AMDGPU_VM_OP_RESERVE_VMID)
			tprint(b, "vmid reserve");
		else if(v->in.op == AMDGPU_VM_OP_UNRESERVE_VMID)
			tprint(b, "vmid unreserve");
		else
			return errno = EINVAL, -1;
		if(treq(a, 0, b, rep) < 0)
			return -1;
		memset(&v->out, 0, sizeof v->out);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_CS:
		return tcs(a, arg, b, rep);
	case DRM_COMMAND_BASE + DRM_AMDGPU_WAIT_CS: {
		union drm_amdgpu_wait_cs *x = arg;

		tprint(b, "waitcs %u %u %u %u %llu %llu", x->in.ctx_id, x->in.ip_type, x->in.ip_instance,
			x->in.ring, (unsigned long long)x->in.handle, (unsigned long long)x->in.timeout);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "waitcs", w, 1) != 1)
			return errno = EIO, -1;
		memset(&x->out, 0, sizeof x->out);
		x->out.status = strcmp(w[0], "busy") == 0;
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_WAIT_FENCES: {
		union drm_amdgpu_wait_fences *x = arg;
		struct drm_amdgpu_fence *f = P(x->in.fences);
		uint32_t i;

		tprint(b, "waitfences %s %llu", x->in.wait_all ? "all" : "any", (unsigned long long)x->in.timeout_ns);
		for(i = 0; i < x->in.fence_count; i++)
			tprint(b, " %u %u %u %u %llu", f[i].ctx_id, f[i].ip_type, f[i].ip_instance, f[i].ring,
				(unsigned long long)f[i].seq_no);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "waitfences", w, 2) != 2)
			return errno = EIO, -1;
		memset(&x->out, 0, sizeof x->out);
		x->out.status = TU(w[0]);
		x->out.first_signaled = TU(w[1]);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_FENCE_TO_HANDLE: {
		union drm_amdgpu_fence_to_handle *x = arg;
		struct drm_amdgpu_fence *f = &x->in.fence;

		if(x->in.what != AMDGPU_FENCE_TO_HANDLE_GET_SYNCOBJ)
			return errno = EOPNOTSUPP, -1;
		tprint(b, "fromfence %u %u %u %u %llu", f->ctx_id, f->ip_type, f->ip_instance, f->ring,
			(unsigned long long)f->seq_no);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "fromfence", w, 1) != 1)
			return errno = EIO, -1;
		memset(&x->out, 0, sizeof x->out);
		x->out.handle = TU(w[0]);
		return 0;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_INFO:
		return tinfo(a, arg, b, rep);
	case 0xBF: {	/* SYNCOBJ_CREATE */
		struct drm_syncobj_create *s = arg;

		if(s->flags & ~DRM_SYNCOBJ_CREATE_SIGNALED)
			return errno = EINVAL, -1;
		tprint(b, "syncnew%s", s->flags ? " signaled" : "");
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "syncnew", w, 1) != 1)
			return errno = EIO, -1;
		s->handle = TU(w[0]);
		return 0;
	}
	case 0xC0:	/* SYNCOBJ_DESTROY */
		tprint(b, "syncfree %u", ((struct drm_syncobj_destroy*)arg)->handle);
		return treq(a, 0, b, rep);
	case 0xC3: {	/* SYNCOBJ_WAIT */
		struct drm_syncobj_wait *s = arg;

		tprint(b, "syncwait %#x %lld", s->flags, (long long)s->timeout_nsec);
		tsyncs(b, s->handles, 0, s->count_handles);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "syncwait", w, 1) != 1)
			return errno = EIO, -1;
		s->first_signaled = TU(w[0]);
		return 0;
	}
	case 0xCA: {	/* SYNCOBJ_TIMELINE_WAIT */
		struct drm_syncobj_timeline_wait *s = arg;

		tprint(b, "synctwait %#x %lld", s->flags, (long long)s->timeout_nsec);
		tsyncs(b, s->handles, s->points, s->count_handles);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(twords(rep, "synctwait", w, 1) != 1)
			return errno = EIO, -1;
		s->first_signaled = TU(w[0]);
		return 0;
	}
	case 0xC4:	/* SYNCOBJ_RESET */
	case 0xC5: {	/* SYNCOBJ_SIGNAL */
		struct drm_syncobj_array *s = arg;

		tprint(b, "%s", nr == 0xC4 ? "syncreset" : "syncsignal");
		tsyncs(b, s->handles, 0, s->count_handles);
		return treq(a, 0, b, rep);
	}
	case 0xCD: {	/* SYNCOBJ_TIMELINE_SIGNAL */
		struct drm_syncobj_timeline_array *s = arg;

		if(s->flags != 0)
			return errno = EINVAL, -1;
		tprint(b, "synctsignal");
		tsyncs(b, s->handles, s->points, s->count_handles);
		return treq(a, 0, b, rep);
	}
	case 0xCB: {	/* SYNCOBJ_QUERY */
		struct drm_syncobj_timeline_array *s = arg;
		uint64_t *pt = P(s->points);
		char *v, *e;
		uint32_t i;

		tprint(b, "syncquery %#x", s->flags);
		tsyncs(b, s->handles, 0, s->count_handles);
		if(treq(a, 0, b, rep) < 0)
			return -1;
		if(strncmp(rep, "syncquery", 9) != 0)
			return errno = EIO, -1;
		v = rep + 9;
		for(i = 0; i < s->count_handles; i++){
			pt[i] = strtoull(v, &e, 0);
			if(e == v)
				return errno = EIO, -1;
			v = e;
		}
		return 0;
	}
	case 0xCC: {	/* SYNCOBJ_TRANSFER */
		struct drm_syncobj_transfer *s = arg;

		tprint(b, "synctransfer %u %llu %u %llu %#x", s->src_handle, (unsigned long long)s->src_point,
			s->dst_handle, (unsigned long long)s->dst_point, s->flags);
		return treq(a, 0, b, rep);
	}
	}
	return Notext;
}

int
amd9ioctl(Amd *a, unsigned long req, void *arg)
{
	unsigned nr;
	int r;

	nr = req & 0xff;
	if(refused(nr)){
		dbg("ioctl %#x: not here", nr);
		return errno = EOPNOTSUPP, -1;
	}
	if(nr == DRM_COMMAND_BASE + DRM_AMDGPU_FENCE_TO_HANDLE &&
	   ((union drm_amdgpu_fence_to_handle*)arg)->in.what != AMDGPU_FENCE_TO_HANDLE_GET_SYNCOBJ)
		return errno = EOPNOTSUPP, -1;	/* sync file descriptors: none */
	if(nr == DRM_COMMAND_BASE + DRM_AMDGPU_GEM_CREATE)
		tracecreate(arg);
	r = textioctl(a, nr, arg);
	if(r == Notext){
		dbg("ioctl %#x: no request for it", nr);
		return errno = EOPNOTSUPP, -1;
	}
	trace(nr, arg, r);
	if(r == 0 && !inpeek && dbglevel() >= 4 && nr == DRM_COMMAND_BASE + DRM_AMDGPU_CS && dbguserdata != 0){
		uint32_t ctx = ((union drm_amdgpu_cs*)arg)->in.ctx_id;

		dbgpeek(a, ctx, 0xffff800000000000ULL | dbguserdata, 48);
		dbguserdata = 0;
	}
	if(r < 0){
		if(nr == DRM_COMMAND_BASE + DRM_AMDGPU_INFO)
			dbg("ioctl %#x (INFO query %#x, %u %u): %s", nr, ((struct drm_amdgpu_info*)arg)->query,
				((struct drm_amdgpu_info*)arg)->query_hw_ip.type, ((struct drm_amdgpu_info*)arg)->query_hw_ip.ip_instance,
				strerror(errno));
		else
			dbg("ioctl %#x: %s", nr, strerror(errno));
	}
	return r;
}

/*
 * libdrm_amdgpu
 */
struct amdgpu_device {
	int	fd;
	int	ref;
	uint32_t	major, minor;
	struct drm_amdgpu_info_device	info;
	struct amdgpu_device	*next;
	pthread_mutex_t	lk;
	struct amdgpu_bo	*shared;	/* exported or imported as dma-bufs: by handle, counted */
	struct Vam	*vam[4];	/* 32-bit, general, high 32-bit, high */
	uint32_t	prtbit;	/* the bit that halves the high range (amdgpu_sw_info_address_prt_wa_control_bit) */
};

struct amdgpu_bo {
	struct amdgpu_device	*dev;
	uint32_t	handle;
	uint64_t	size;
	void	*cpu;		/* mapped: the view */
	int	maps;
	int	seg;		/* the view is a segment of its own (detach) */
	int	refs;		/* on dev->shared: the imports and the maker */
	struct amdgpu_bo	*snext;
};

/* b on dev->shared, if it is not; with dev->lk */
static void
share(struct amdgpu_bo *b)
{
	struct amdgpu_bo *s;

	for(s = b->dev->shared; s != NULL; s = s->snext)
		if(s == b)
			return;
	b->refs = 1;
	b->snext = b->dev->shared;
	b->dev->shared = b;
}

/* the GPU's address space: holes, ascending */
typedef struct Hole Hole;
struct Hole {
	uint64_t	start, end;
	Hole	*next;
};
typedef struct Vam Vam;
struct Vam {
	uint64_t	start, end, align;
	Hole	*holes;
};
struct amdgpu_va {
	struct amdgpu_device	*dev;
	Vam	*vam;
	uint64_t	start, size;
};

static pthread_mutex_t devlk = PTHREAD_MUTEX_INITIALIZER;

/* two descriptors of one context (libdrm dedups devices so) */
static int
samefile(int a, int b)
{
	struct stat sa, sb;

	return fstat(a, &sa) == 0 && fstat(b, &sb) == 0 && sa.st_ino == sb.st_ino && sa.st_dev == sb.st_dev;
}
static struct amdgpu_device *devs;

static Vam*
vaminit(uint64_t start, uint64_t end, uint64_t align)
{
	Vam *v;

	v = calloc(1, sizeof *v);
	if(v == NULL)
		return NULL;
	v->start = start;
	v->end = end;
	v->align = align ? align : 4096;
	if(end > start){
		v->holes = calloc(1, sizeof(Hole));
		if(v->holes == NULL){
			free(v);
			return NULL;
		}
		v->holes->start = start;
		v->holes->end = end;
	}
	return v;
}

/* size at want (0: anywhere), aligned; 0 or -1 */
static int
vamalloc(Vam *v, uint64_t size, uint64_t align, uint64_t want, uint64_t *got)
{
	Hole *h, **l, *n;
	uint64_t a;

	if(align < v->align)
		align = v->align;
	for(l = &v->holes; (h = *l) != NULL; l = &h->next){
		if(want){
			if(want < h->start || want + size > h->end)
				continue;
			a = want;
		}else{
			a = (h->start + align - 1) & ~(align - 1);
			if(a + size > h->end || a + size < a)
				continue;
		}
		/* split the hole around [a, a+size) */
		if(a + size < h->end && a > h->start){
			n = calloc(1, sizeof *n);
			if(n == NULL)
				return -1;
			n->start = a + size;
			n->end = h->end;
			n->next = h->next;
			h->end = a;
			h->next = n;
		}else if(a > h->start)
			h->end = a;
		else if(a + size < h->end)
			h->start = a + size;
		else{
			*l = h->next;
			free(h);
		}
		*got = a;
		return 0;
	}
	return -1;
}

static void
vamfree(Vam *v, uint64_t start, uint64_t size)
{
	Hole *h, **l, *n, *p;

	p = NULL;
	for(l = &v->holes; (h = *l) != NULL && h->start < start; l = &h->next)
		p = h;
	n = calloc(1, sizeof *n);
	if(n == NULL)
		return;	/* lost to the manager */
	n->start = start;
	n->end = start + size;
	n->next = h;
	*l = n;
	/* joined with its neighbours */
	if(h != NULL && n->end == h->start){
		n->end = h->end;
		n->next = h->next;
		free(h);
	}
	if(p != NULL && p->end == n->start){
		p->end = n->end;
		p->next = n->next;
		free(n);
	}
}

static int
devioctl(struct amdgpu_device *d, unsigned long req, void *arg)
{
	return drmIoctl(d->fd, req, arg) < 0 ? -errno : 0;
}

/* the number of the highest bit set, from 1; 0 for none (Mesa's util_last_bit64) */
static int
lastbit64(uint64_t v)
{
	int n;

	for(n = 0; v != 0; n++)
		v >>= 1;
	return n;
}

/*
 * as libdrm (amdgpu_needs_smem_prt_wa): GFX 6 to 12 but 9 and 11.5.6
 * reserve half the address space for RADV's PRT workaround.  The
 * version as amdgpu_query_gfx_version has it: the IP discovery's, else
 * (GFX 6-8) major.minor of the HW IP info.
 */
static int
needsprtwa(struct amdgpu_device *d)
{
	struct drm_amdgpu_info in;
	struct drm_amdgpu_info_hw_ip ip;
	uint32_t n, v, maj;

	n = 0;
	memset(&in, 0, sizeof in);
	in.return_pointer = U(&n);
	in.return_size = sizeof n;
	in.query = AMDGPU_INFO_HW_IP_COUNT;
	in.query_hw_ip.type = AMDGPU_HW_IP_GFX;
	if(devioctl(d, DRM_IOCTL_AMDGPU_INFO, &in) < 0 || n == 0)
		return 0;
	memset(&ip, 0, sizeof ip);
	memset(&in, 0, sizeof in);
	in.return_pointer = U(&ip);
	in.return_size = sizeof ip;
	in.query = AMDGPU_INFO_HW_IP_INFO;
	in.query_hw_ip.type = AMDGPU_HW_IP_GFX;
	if(devioctl(d, DRM_IOCTL_AMDGPU_INFO, &in) < 0)
		return 0;
	if(d->minor >= 48 && ip.ip_discovery_version)
		v = ip.ip_discovery_version;
	else
		v = ip.hw_ip_version_major << 16 | ip.hw_ip_version_minor << 8;
	maj = v >> 16 & 0xff;
	return maj >= 6 && maj <= 12 && maj != 9 && v != (11 << 16 | 5 << 8 | 6);
}

int
amdgpu_device_initialize(int fd, uint32_t *major, uint32_t *minor, amdgpu_device_handle *out)
{
	struct amdgpu_device *d;
	struct drm_amdgpu_info in;
	drmVersionPtr v;
	uint64_t start, max, lo, hi, align;
	int r, prt;

	pthread_mutex_lock(&devlk);
	for(d = devs; d != NULL; d = d->next)
		if(samefile(d->fd, fd)){
			d->ref++;
			*major = d->major;
			*minor = d->minor;
			*out = d;
			pthread_mutex_unlock(&devlk);
			return 0;
		}
	pthread_mutex_unlock(&devlk);
	if((v = drmGetVersion(fd)) == NULL)
		return -errno;
	if(strcmp(v->name, "amdgpu") != 0){
		drmFreeVersion(v);
		return -ENODEV;
	}
	d = calloc(1, sizeof *d);
	if(d == NULL){
		drmFreeVersion(v);
		return -ENOMEM;
	}
	/* as libdrm: a descriptor of its own, the caller may close its one */
	if((d->fd = dup(fd)) < 0){	/* APE: no F_DUPFD_CLOEXEC */
		free(d);
		return -errno;
	}
	fcntl(d->fd, F_SETFD, FD_CLOEXEC);
	d->ref = 1;
	d->major = v->version_major;
	d->minor = v->version_minor;
	drmFreeVersion(v);
	pthread_mutex_init(&d->lk, NULL);
	memset(&in, 0, sizeof in);
	in.return_pointer = U(&d->info);
	in.return_size = sizeof d->info;
	in.query = AMDGPU_INFO_DEV_INFO;
	if((r = devioctl(d, DRM_IOCTL_AMDGPU_INFO, &in)) < 0){
		close(d->fd);
		free(d);
		return r;
	}
	/*
	 * as libdrm divides the space (amdgpu_va_manager_init2): below 4 GB,
	 * the rest, and the same high.  RADV's workaround for scalar loads
	 * from NULL PRT pages: the high range (or the low one, if there is no
	 * high) is two halves, told apart by its highest bit that is not the
	 * same at both ends; objects go in the lower half, a sparse buffer's
	 * address has the bit set, its twin without it is mapped to zeros
	 * (radv_amdgpu_bo.c); shaders mask the bit off their scalar loads
	 * (ac_nir_fixup_smem_loads_null_prt).  ~0: no workaround.
	 */
	prt = needsprtwa(d);
	d->prtbit = ~0U;
	align = d->info.virtual_address_alignment;
	lo = d->info.virtual_address_offset;
	hi = d->info.virtual_address_max;
	start = lo;
	max = hi < 0x100000000ULL ? hi : 0x100000000ULL;
	d->vam[0] = vaminit(start, max, align);
	start = max;
	if(prt && d->info.high_va_max == 0){
		d->prtbit = lastbit64(lo ^ hi) - 1;
		max = hi ^ (1ULL << d->prtbit);
	}else
		max = hi > 0x100000000ULL ? hi : 0x100000000ULL;
	d->vam[1] = vaminit(start, max, align);
	lo = d->info.high_va_offset;
	hi = d->info.high_va_max;
	start = lo;
	max = (start & ~0xffffffffULL) + 0x100000000ULL;
	if(hi < max)
		max = hi;
	d->vam[2] = vaminit(start, max, align);
	start = max;
	if(prt && hi != 0){
		d->prtbit = lastbit64(lo ^ hi) - 1;
		max = hi ^ (1ULL << d->prtbit);
	}else if(hi > (start & ~0xffffffffULL) + 0x100000000ULL)
		max = hi;
	else
		max = (start & ~0xffffffffULL) + 0x100000000ULL;
	d->vam[3] = vaminit(start, max, align);
	pthread_mutex_lock(&devlk);
	d->next = devs;
	devs = d;
	pthread_mutex_unlock(&devlk);
	*major = d->major;
	*minor = d->minor;
	*out = d;
	return 0;
}

int
amdgpu_device_deinitialize(amdgpu_device_handle d)
{
	struct amdgpu_device **l;

	pthread_mutex_lock(&devlk);
	if(--d->ref > 0){
		pthread_mutex_unlock(&devlk);
		return 0;
	}
	for(l = &devs; *l != NULL; l = &(*l)->next)
		if(*l == d){
			*l = d->next;
			break;
		}
	pthread_mutex_unlock(&devlk);
	close(d->fd);
	free(d);	/* the holes go with the process */
	return 0;
}

int
amdgpu_device_get_fd(amdgpu_device_handle d)
{
	return d->fd;
}

const char*
amdgpu_get_marketing_name(amdgpu_device_handle d)
{
	switch(d->info.device_id){
	case 0x7480:	return "AMD Radeon RX 7600 / 7600 XT";
	case 0x7483:	return "AMD Radeon RX 7600M";
	case 0x744c:	return "AMD Radeon RX 7900 XT / XTX";
	case 0x747e:	return "AMD Radeon RX 7800 XT / 7700 XT";
	}
	return NULL;
}

int
amdgpu_query_sw_info(amdgpu_device_handle d, enum amdgpu_sw_info info, void *value)
{
	uint32_t *v = value;

	switch(info){
	case amdgpu_sw_info_address32_hi:
		if(d->vam[2]->end > d->vam[2]->start)
			*v = (d->vam[2]->end - 1) >> 32;
		else
			*v = (d->vam[0]->end - 1) >> 32;
		return 0;
	case amdgpu_sw_info_address_prt_wa_control_bit:
		*v = d->prtbit;
		return 0;
	}
	return -EINVAL;
}

int
amdgpu_query_uq_fw_area_info(amdgpu_device_handle d, unsigned type, unsigned inst,
	struct drm_amdgpu_info_uq_fw_areas *info)
{
	(void)d; (void)type; (void)inst; (void)info;
	return -EINVAL;	/* no user queues */
}

int
amdgpu_va_range_alloc(amdgpu_device_handle d, enum amdgpu_gpu_va_range type, uint64_t size,
	uint64_t align, uint64_t want, uint64_t *got, amdgpu_va_handle *out, uint64_t flags)
{
	struct amdgpu_va *va;
	Vam *v;
	int i, r;

	(void)type;
	i = (flags & AMDGPU_VA_RANGE_HIGH) ? 2 : 0;
	if(!(flags & AMDGPU_VA_RANGE_32_BIT))
		i++;
	v = d->vam[i];
	size = (size + v->align - 1) & ~(v->align - 1);
	va = calloc(1, sizeof *va);
	if(va == NULL)
		return -ENOMEM;
	pthread_mutex_lock(&d->lk);
	r = vamalloc(v, size, align, want, got);
	if(r < 0 && !want && !(flags & AMDGPU_VA_RANGE_32_BIT) && i == 1)
		r = vamalloc(d->vam[0], size, align, 0, got), v = d->vam[0];	/* as libdrm: the 32-bit part too */
	pthread_mutex_unlock(&d->lk);
	if(r < 0){
		dbg("va_range_alloc %#llx at %#llx flags %#llx: no room", (unsigned long long)size,
			(unsigned long long)want, (unsigned long long)flags);
		free(va);
		return -ENOMEM;
	}
	va->dev = d;
	va->vam = v;
	va->start = *got;
	va->size = size;
	*out = va;
	return 0;
}

int
amdgpu_va_range_free(amdgpu_va_handle va)
{
	if(va == NULL)
		return 0;
	pthread_mutex_lock(&va->dev->lk);
	vamfree(va->vam, va->start, va->size);
	pthread_mutex_unlock(&va->dev->lk);
	free(va);
	return 0;
}

int
amdgpu_va_range_query(amdgpu_device_handle d, enum amdgpu_gpu_va_range type, uint64_t *start, uint64_t *end)
{
	if(type != amdgpu_gpu_va_range_general)
		return -EINVAL;
	*start = d->vam[1]->start;
	*end = d->vam[1]->end;
	return 0;
}

int
amdgpu_bo_alloc(amdgpu_device_handle d, struct amdgpu_bo_alloc_request *req, amdgpu_bo_handle *out)
{
	union drm_amdgpu_gem_create c;
	struct amdgpu_bo *b;
	int r;

	memset(&c, 0, sizeof c);
	c.in.bo_size = req->alloc_size;
	c.in.alignment = req->phys_alignment;
	c.in.domains = req->preferred_heap;
	c.in.domain_flags = req->flags;
	if((r = devioctl(d, DRM_IOCTL_AMDGPU_GEM_CREATE, &c)) < 0){
		dbg("bo_alloc %#llx heap %#x flags %#llx: %d", (unsigned long long)req->alloc_size,
			req->preferred_heap, (unsigned long long)req->flags, r);
		return r;
	}
	b = calloc(1, sizeof *b);
	if(b == NULL){
		struct drm_gem_close cl = { .handle = c.out.handle };

		devioctl(d, DRM_IOCTL_GEM_CLOSE, &cl);
		return -ENOMEM;
	}
	b->dev = d;
	b->handle = c.out.handle;
	b->size = req->alloc_size;
	*out = b;
	return 0;
}

int
amdgpu_bo_export(amdgpu_bo_handle b, enum amdgpu_bo_handle_type type, uint32_t *shared)
{
	char reply[128], token[64];
	Amd *a;
	int fd;

	switch(type){
	case amdgpu_bo_handle_type_kms:
	case amdgpu_bo_handle_type_kms_noimport:
		*shared = b->handle;
		return 0;
	case amdgpu_bo_handle_type_dma_buf_fd:
		/* the driver's export: a token, as a descriptor here */
		if((a = amd9of(b->dev->fd)) == NULL)
			return -EBADF;
		if(ctlreq(a, reply, sizeof reply, "export %u", b->handle) < 0)
			return -errno;
		if(sscanf(reply, "export %63s", token) != 1)
			return -EIO;
		if((fd = plan9_dmabuf_fd(token)) < 0)
			return -EMFILE;
		pthread_mutex_lock(&b->dev->lk);
		share(b);
		pthread_mutex_unlock(&b->dev->lk);
		*shared = fd;
		return 0;
	default:
		return -EINVAL;	/* no flink names */
	}
}

/*
 * a syncobj as a descriptor (opaque, as SYNCOBJ_HANDLE_TO_FD's): the
 * driver's syncexport names it by a token, the token is the descriptor
 * (lib/dmabuf.c); syncimport in another context, of any process, gives
 * a handle of the same syncobj there
 */
int
amd9syncexport(Amd *a, uint32_t handle, int *out)
{
	char reply[128], token[64];
	int fd;

	if(ctlreq(a, reply, sizeof reply, "syncexport %u", handle) < 0)
		return -errno;
	if(sscanf(reply, "syncexport %63s", token) != 1)
		return -EIO;
	if((fd = plan9_dmabuf_fd(token)) < 0)
		return -EMFILE;
	*out = fd;
	return 0;
}

int
amd9syncimport(Amd *a, int fd, uint32_t *handle)
{
	char token[64], reply[128];
	unsigned h;

	if(plan9_dmabuf_token(fd, token, sizeof token) < 0)
		return -EINVAL;
	if(ctlreq(a, reply, sizeof reply, "syncimport %s", token) < 0)
		return -errno;
	if(sscanf(reply, "syncimport %u", &h) != 1)
		return -EIO;
	*handle = h;
	return 0;
}

/* a dma-buf descriptor's object: imported by its token */
static int
importdmabuf(amdgpu_device_handle d, int fd, struct amdgpu_bo_import_result *out)
{
	char token[64], reply[128];
	struct amdgpu_bo *b;
	unsigned long long size;
	unsigned h;
	Amd *a;

	if(plan9_dmabuf_token(fd, token, sizeof token) < 0)
		return -EINVAL;
	if((a = amd9of(d->fd)) == NULL)
		return -EBADF;
	if(ctlreq(a, reply, sizeof reply, "import %s", token) < 0)
		return -errno;
	if(sscanf(reply, "import %u %llu", &h, &size) != 2)
		return -EIO;
	/* the same object again (exported here, or imported before): one more reference */
	pthread_mutex_lock(&d->lk);
	for(b = d->shared; b != NULL; b = b->snext)
		if(b->handle == h){
			b->refs++;
			pthread_mutex_unlock(&d->lk);
			out->buf_handle = b;
			out->alloc_size = b->size;
			return 0;
		}
	b = calloc(1, sizeof *b);
	if(b == NULL){
		pthread_mutex_unlock(&d->lk);
		return -ENOMEM;
	}
	b->dev = d;
	b->handle = h;
	b->size = size;
	share(b);
	pthread_mutex_unlock(&d->lk);
	out->buf_handle = b;
	out->alloc_size = size;
	return 0;
}

int
amdgpu_bo_import(amdgpu_device_handle d, enum amdgpu_bo_handle_type type, uint32_t shared,
	struct amdgpu_bo_import_result *out)
{
	struct drm_amdgpu_gem_op op;
	struct drm_amdgpu_gem_create_in info;
	struct amdgpu_bo *b;
	int r;

	if(type == amdgpu_bo_handle_type_dma_buf_fd)
		return importdmabuf(d, shared, out);
	if(type != amdgpu_bo_handle_type_kms && type != amdgpu_bo_handle_type_kms_noimport)
		return -EINVAL;
	memset(&op, 0, sizeof op);
	op.handle = shared;
	op.op = AMDGPU_GEM_OP_GET_GEM_CREATE_INFO;
	op.value = U(&info);
	if((r = devioctl(d, DRM_IOCTL_AMDGPU_GEM_OP, &op)) < 0)
		return r;
	b = calloc(1, sizeof *b);
	if(b == NULL)
		return -ENOMEM;
	b->dev = d;
	b->handle = shared;
	b->size = info.bo_size;
	out->buf_handle = b;
	out->alloc_size = info.bo_size;
	return 0;
}

int
amdgpu_create_bo_from_user_mem(amdgpu_device_handle d, void *cpu, uint64_t size, amdgpu_bo_handle *out)
{
	(void)d; (void)cpu; (void)size; (void)out;
	return -EINVAL;	/* the driver cannot reach the process's memory */
}

/* the driver's view of the object: a segment of its own (GTT: its pages; VRAM: its part of BAR 0) */
int
amdgpu_bo_cpu_map(amdgpu_bo_handle b, void **cpu)
{
	Amd *a;
	char reply[256], seg[64];
	unsigned long long va, size;
	void *p;

	pthread_mutex_lock(&b->dev->lk);
	if(b->cpu != NULL){
		b->maps++;
		*cpu = b->cpu;
		pthread_mutex_unlock(&b->dev->lk);
		return 0;
	}
	pthread_mutex_unlock(&b->dev->lk);
	if((a = amd9of(b->dev->fd)) == NULL)
		return -EBADF;
	if(ctlreq(a, reply, sizeof reply, "map %u", b->handle) < 0){
		int e = errno;

		reply[0] = 0;
		_ERRSTR(reply, sizeof reply);	/* the driver's reason */
		dbg("map %u: %s", b->handle, reply);
		return -e;
	}
	if(sscanf(reply, "map %*u %63s %lli %lli", seg, &va, &size) == 3){
		p = mesa9_segattach(seg, NULL, size);
		if(p == (void*)-1){
			char why[128];

			why[0] = 0;
			_ERRSTR(why, sizeof why);	/* the raw call leaves errno alone: the kernel's reason */
			dbg("map %u: segattach %s %#llx: %s", b->handle, seg, size, why);
			ctlreq(a, NULL, 0, "unmap %u", b->handle);
			return -ENOMEM;
		}
		b->seg = 1;
	}else{
		dbg("map %u: %s", b->handle, reply);
		return -EIO;
	}
	pthread_mutex_lock(&b->dev->lk);
	b->cpu = p;
	b->maps = 1;
	pthread_mutex_unlock(&b->dev->lk);
	if(dbglevel() >= 3)
		dbgcpu(b->handle, p);
	*cpu = p;
	return 0;
}

int
amdgpu_bo_cpu_unmap(amdgpu_bo_handle b)
{
	Amd *a;
	void *p;

	pthread_mutex_lock(&b->dev->lk);
	if(b->cpu == NULL || --b->maps > 0){
		pthread_mutex_unlock(&b->dev->lk);
		return 0;
	}
	p = b->cpu;
	b->cpu = NULL;
	pthread_mutex_unlock(&b->dev->lk);
	if(b->seg)
		mesa9_segdetach(p);
	b->seg = 0;
	if((a = amd9of(b->dev->fd)) != NULL)
		ctlreq(a, NULL, 0, "unmap %u", b->handle);
	return 0;
}

int
amdgpu_bo_free(amdgpu_bo_handle b)
{
	struct drm_gem_close cl;
	struct amdgpu_bo **l;

	/* shared: the last reference closes it */
	pthread_mutex_lock(&b->dev->lk);
	for(l = &b->dev->shared; *l != NULL; l = &(*l)->snext)
		if(*l == b){
			if(--b->refs > 0){
				pthread_mutex_unlock(&b->dev->lk);
				return 0;
			}
			*l = b->snext;
			break;
		}
	pthread_mutex_unlock(&b->dev->lk);

	if(b->cpu != NULL){
		b->maps = 1;
		amdgpu_bo_cpu_unmap(b);
	}
	memset(&cl, 0, sizeof cl);
	cl.handle = b->handle;
	devioctl(b->dev, DRM_IOCTL_GEM_CLOSE, &cl);
	free(b);
	return 0;
}
