/*
 * SPDX-License-Identifier: MIT
 *
 * libdrm_amdgpu on 9front, and amdgpu's ioctls, over aux/amdgpu
 * (gpu(4)): what Mesa's RADV (src/amd/common/ac_linux_drm.c) uses.
 *
 * A descriptor is a context of aux/amdgpu, opened by /dev/gpu/N/clone
 * (drm9.c finds it); the context's arena is memory at one address here
 * and in the driver. An ioctl's structure and everything it points to
 * (CS chunks, BO lists, INFO's result, syncobj handles, ...) are copied
 * there, the ioctl is written to the context's ioctl file as "cmd addr",
 * and what the driver wrote comes back. The driver is Linux's amdgpu:
 * its ioctl numbers are Linux's (here BSD's encoding, sys/ioccom.h), its
 * errors Linux's errno values (here APE's).
 *
 *	libdrm_amdgpu	device, objects (GEM_CREATE; cpu_map: the driver's
 *			"map", a segment), the GPU's address space (a
 *			manager of its own over AMDGPU_INFO's ranges, as
 *			libdrm's)
 *	not here	userptr objects, dma-buf and sync file descriptors
 *			(PRIME, syncobj fds), user queues
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

extern void	*_SEGATTACH(int, char*, void*, unsigned long);
extern int	_ERRSTR(char*, unsigned int);
extern int	_SEGDETACH(void*);

enum {
	Slot	= 64*1024,	/* the arena in slots, one or more a call */
	Nfid	= 8,		/* ioctl files kept open */
	Maxbo	= 4096,
};

struct Amd {
	int	fd;		/* the context's ctl */
	int	ctx;
	char	dir[32];	/* /dev/gpu/N */
	pthread_mutex_t	lk;
	pthread_cond_t	free;
	unsigned char	*arena;
	uint64_t	asize;
	int	nslot;
	unsigned char	*used;	/* per slot */
	int	fids[Nfid];
	int	nfids;
	unsigned char	*bar0;	/* BAR 0, for VRAM objects' views */
	uint64_t	bar0size;
};

/*
 * the arena
 */
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
	char buf[256], seg[64];
	unsigned long long va, size;
	Amd *a;

	a = calloc(1, sizeof *a);
	if(a == NULL)
		return NULL;
	a->fd = fd;
	a->ctx = ctx;
	snprintf(a->dir, sizeof a->dir, "%s", dir);
	pthread_mutex_init(&a->lk, NULL);
	pthread_cond_init(&a->free, NULL);
	if(ctlreq(a, buf, sizeof buf, "arena") < 0 ||
	   sscanf(buf, "arena %63s %lli %lli", seg, &va, &size) != 3){
		free(a);
		return NULL;
	}
	/* the driver's pointers into it are ours: it attaches where it is */
	a->arena = _SEGATTACH(0, seg, NULL, size);
	if(a->arena == (void*)-1 || (uintptr_t)a->arena != va){
		if(a->arena != (void*)-1)
			_SEGDETACH(a->arena);
		free(a);
		return NULL;
	}
	a->asize = size;
	a->nslot = size / Slot;
	a->used = calloc(a->nslot, 1);
	if(a->used == NULL){
		_SEGDETACH(a->arena);
		free(a);
		return NULL;
	}
	return a;
}

/* n bytes of the arena, contiguous slots; waits for them */
static unsigned char*
aget(Amd *a, uint64_t n, int *first, int *count)
{
	int i, j, k;

	k = (n + Slot - 1) / Slot;
	if(k == 0)
		k = 1;
	if(k > a->nslot)
		return NULL;
	pthread_mutex_lock(&a->lk);
	for(;;){
		for(i = 0; i + k <= a->nslot; i++){
			for(j = 0; j < k && !a->used[i+j]; j++)
				;
			if(j == k){
				memset(a->used + i, 1, k);
				pthread_mutex_unlock(&a->lk);
				*first = i;
				*count = k;
				return a->arena + (uint64_t)i*Slot;
			}
		}
		pthread_cond_wait(&a->free, &a->lk);
	}
}

static void
aput(Amd *a, int first, int count)
{
	pthread_mutex_lock(&a->lk);
	memset(a->used + first, 0, count);
	pthread_cond_broadcast(&a->free);
	pthread_mutex_unlock(&a->lk);
}

/* an ioctl file of the context: kept open, one per call at a time */
static int
fidget(Amd *a)
{
	char path[64];
	int fd;

	pthread_mutex_lock(&a->lk);
	if(a->nfids > 0){
		fd = a->fids[--a->nfids];
		pthread_mutex_unlock(&a->lk);
		return fd;
	}
	pthread_mutex_unlock(&a->lk);
	snprintf(path, sizeof path, "%s/ctx/%d/ioctl", a->dir, a->ctx);
	return open(path, O_RDWR);
}

static void
fidput(Amd *a, int fd)
{
	pthread_mutex_lock(&a->lk);
	if(a->nfids < Nfid){
		a->fids[a->nfids++] = fd;
		fd = -1;
	}
	pthread_mutex_unlock(&a->lk);
	if(fd >= 0)
		close(fd);
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

/* the ioctl with its arguments at p in the arena: 0, or -1 and errno */
static int
call(Amd *a, unsigned long lcmd, void *p)
{
	char buf[64];
	long r;
	int fd, n;

	if((fd = fidget(a)) < 0)
		return -1;
	n = snprintf(buf, sizeof buf, "%#lx %#llx", lcmd, (unsigned long long)(uintptr_t)p);
	if(write(fd, buf, n) != n || (n = pread(fd, buf, sizeof buf - 1, 0)) <= 0){
		close(fd);
		errno = EIO;
		return -1;
	}
	fidput(a, fd);
	buf[n] = 0;
	r = strtol(buf, NULL, 10);
	if(r < 0){
		errno = apeerrno(-r);
		return -1;
	}
	return 0;
}

/*
 * marshalling: the structure and what it points to, copied into the
 * arena (a first pass only counts), and back
 */
typedef struct M M;
struct M {
	unsigned char	*base;	/* nil: counting */
	uint64_t	n;
};

/* room for n bytes: the copy of src (or zeros) */
static void*
mput(M *m, const void *src, uint64_t n)
{
	void *p;

	n = (n + 15) & ~(uint64_t)15;
	if(m->base == NULL){
		m->n += n;
		return (void*)(uintptr_t)16;	/* not used when counting */
	}
	p = m->base + m->n;
	m->n += n;
	if(src != NULL)
		memcpy(p, src, n);
	else
		memset(p, 0, n);
	return p;
}

#define P(x)	((void*)(uintptr_t)(x))
#define U(p)	((uint64_t)(uintptr_t)(p))

/*
 * the nested parts of an ioctl's arguments s (in the arena already):
 * copied in (dir 0) or back out to the caller's (dir 1, orig: the
 * caller's structure)
 */
static int
nested(M *m, unsigned nr, void *s, void *orig, int out)
{
	switch(nr){
	case 0x00: {	/* VERSION */
		struct drm_version *v = s, *o = orig;

		if(!out){
			v->name = o->name_len ? mput(m, NULL, o->name_len) : NULL;
			v->date = o->date_len ? mput(m, NULL, o->date_len) : NULL;
			v->desc = o->desc_len ? mput(m, NULL, o->desc_len) : NULL;
		}else{
			if(o->name && v->name)
				memcpy(o->name, v->name, o->name_len < v->name_len ? o->name_len : v->name_len);
			if(o->date && v->date)
				memcpy(o->date, v->date, o->date_len < v->date_len ? o->date_len : v->date_len);
			if(o->desc && v->desc)
				memcpy(o->desc, v->desc, o->desc_len < v->desc_len ? o->desc_len : v->desc_len);
			o->version_major = v->version_major;
			o->version_minor = v->version_minor;
			o->version_patchlevel = v->version_patchlevel;
			o->name_len = v->name_len;
			o->date_len = v->date_len;
			o->desc_len = v->desc_len;
			return 1;	/* copied: the pointers are the caller's */
		}
		break;
	}
	case 0xC3: {	/* SYNCOBJ_WAIT */
		struct drm_syncobj_wait *w = s, *o = orig;

		if(!out)
			w->handles = U(mput(m, P(o->handles), (uint64_t)o->count_handles * 4));
		else{
			o->first_signaled = w->first_signaled;
			return 1;
		}
		break;
	}
	case 0xC4:	/* SYNCOBJ_RESET */
	case 0xC5: {	/* SYNCOBJ_SIGNAL */
		struct drm_syncobj_array *y = s, *o = orig;

		if(!out)
			y->handles = U(mput(m, P(o->handles), (uint64_t)o->count_handles * 4));
		else
			return 1;
		break;
	}
	case 0xCA: {	/* SYNCOBJ_TIMELINE_WAIT */
		struct drm_syncobj_timeline_wait *w = s, *o = orig;

		if(!out){
			w->handles = U(mput(m, P(o->handles), (uint64_t)o->count_handles * 4));
			w->points = U(mput(m, P(o->points), (uint64_t)o->count_handles * 8));
		}else{
			o->first_signaled = w->first_signaled;
			return 1;
		}
		break;
	}
	case 0xCB:	/* SYNCOBJ_QUERY */
	case 0xCD: {	/* SYNCOBJ_TIMELINE_SIGNAL */
		struct drm_syncobj_timeline_array *y = s, *o = orig;

		if(!out){
			y->handles = U(mput(m, P(o->handles), (uint64_t)o->count_handles * 4));
			y->points = U(mput(m, P(o->points), (uint64_t)o->count_handles * 8));
		}else{
			if(nr == 0xCB && o->points)
				memcpy(P(o->points), P(y->points), (uint64_t)o->count_handles * 8);
			return 1;
		}
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_BO_LIST: {
		union drm_amdgpu_bo_list *l = s, *o = orig;

		if(!out)
			l->in.bo_info_ptr = U(mput(m, P(o->in.bo_info_ptr),
				(uint64_t)o->in.bo_number * o->in.bo_info_size));
		else{
			o->out = l->out;
			return 1;
		}
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_CS: {
		union drm_amdgpu_cs *c = s, *o = orig;
		uint64_t *chp, *och;
		struct drm_amdgpu_cs_chunk *ch, *oc;
		struct drm_amdgpu_bo_list_in *bl, *obl;
		uint32_t i;

		if(out){
			o->out = c->out;
			return 1;
		}
		och = P(o->in.chunks);
		chp = mput(m, NULL, (uint64_t)o->in.num_chunks * 8);
		c->in.chunks = U(chp);
		for(i = 0; i < o->in.num_chunks; i++){
			oc = P(och[i]);
			ch = mput(m, oc, sizeof *ch);
			if(m->base != NULL)
				chp[i] = U(ch);
			if(m->base == NULL){
				mput(m, NULL, (uint64_t)oc->length_dw * 4);
				if(oc->chunk_id == AMDGPU_CHUNK_ID_BO_HANDLES){
					obl = P(oc->chunk_data);
					mput(m, NULL, (uint64_t)obl->bo_number * obl->bo_info_size);
				}
				continue;
			}
			ch->chunk_data = U(mput(m, P(oc->chunk_data), (uint64_t)oc->length_dw * 4));
			if(oc->chunk_id == AMDGPU_CHUNK_ID_BO_HANDLES){
				bl = P(ch->chunk_data);
				obl = P(oc->chunk_data);
				bl->bo_info_ptr = U(mput(m, P(obl->bo_info_ptr), (uint64_t)obl->bo_number * obl->bo_info_size));
			}
		}
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_INFO: {
		struct drm_amdgpu_info *in = s, *o = orig;

		if(!out)
			in->return_pointer = U(mput(m, NULL, o->return_size));
		else{
			memcpy(P(o->return_pointer), P(in->return_pointer), o->return_size);
			return 1;
		}
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_GEM_OP: {
		struct drm_amdgpu_gem_op *g = s, *o = orig;

		if(o->op != AMDGPU_GEM_OP_GET_GEM_CREATE_INFO)
			break;
		if(!out)
			g->value = U(mput(m, NULL, sizeof(struct drm_amdgpu_gem_create_in)));
		else{
			memcpy(P(o->value), P(g->value), sizeof(struct drm_amdgpu_gem_create_in));
			return 1;
		}
		break;
	}
	case DRM_COMMAND_BASE + DRM_AMDGPU_WAIT_FENCES: {
		union drm_amdgpu_wait_fences *w = s, *o = orig;

		if(!out)
			w->in.fences = U(mput(m, P(o->in.fences), (uint64_t)o->in.fence_count * sizeof(struct drm_amdgpu_fence)));
		else{
			o->out = w->out;
			return 1;
		}
		break;
	}
	}
	return 0;
}

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
				ibp = _SEGATTACH(0, seg, NULL, ssize);
			else
				dstp = _SEGATTACH(0, seg, NULL, ssize);
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
	ibp[5] = dstva;
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

int
amd9ioctl(Amd *a, unsigned long req, void *arg)
{
	unsigned nr, size, dir;
	unsigned long lcmd;
	unsigned char *p;
	int first, count, r;
	M m;
	void *s;

	nr = req & 0xff;
	size = (req >> 16) & IOCPARM_MASK;
	if(refused(nr)){
		dbg("ioctl %#x: not here", nr);
		return errno = EOPNOTSUPP, -1;
	}
	if(nr == DRM_COMMAND_BASE + DRM_AMDGPU_FENCE_TO_HANDLE &&
	   ((union drm_amdgpu_fence_to_handle*)arg)->in.what != AMDGPU_FENCE_TO_HANDLE_GET_SYNCOBJ)
		return errno = EOPNOTSUPP, -1;	/* sync file descriptors: none */
	/* Linux's encoding: 1 the caller writes (IOC_IN), 2 it reads (IOC_OUT) */
	dir = ((req & IOC_IN) ? 1 : 0) | ((req & IOC_OUT) ? 2 : 0);
	lcmd = (unsigned long)dir << 30 | (unsigned long)size << 16 | DRM_IOCTL_BASE << 8 | nr;
	/* the size of the copy */
	m.base = NULL;
	m.n = 0;
	s = mput(&m, NULL, size);
	if(m.n > 0 && size > 0){
		/* count the nested parts on a copy of the caller's structure */
		unsigned char tmp[4096];

		if(size > sizeof tmp)
			return errno = EINVAL, -1;
		memcpy(tmp, arg, size);
		nested(&m, nr, tmp, arg, 0);
	}
	if((p = aget(a, m.n, &first, &count)) == NULL)
		return errno = E2BIG, -1;
	m.base = p;
	m.n = 0;
	s = mput(&m, arg, size);
	nested(&m, nr, s, arg, 0);
	if(nr == DRM_COMMAND_BASE + DRM_AMDGPU_GEM_CREATE)
		tracecreate(s);
	r = call(a, lcmd, s);
	trace(nr, s, r);
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
	/*
	 * what the driver wrote: through the structure's pointers whatever its
	 * direction (INFO is an IOW: its result goes to return_pointer), the
	 * structure itself if the caller reads it
	 */
	if(r == 0 && !nested(&m, nr, s, arg, 1) && (dir & 2))
		memcpy(arg, s, size);
	aput(a, first, count);
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
	struct Vam	*vam[4];	/* 32-bit, general, high 32-bit, high */
	uint32_t	prtbit;	/* the bit that halves the high range (amdgpu_sw_info_address_prt_wa_control_bit) */
};

struct amdgpu_bo {
	struct amdgpu_device	*dev;
	uint32_t	handle;
	uint64_t	size;
	void	*cpu;		/* mapped: the view */
	int	maps;
	int	vram;		/* the view is in BAR 0 */
	int	seg;		/* the view is a segment of its own (detach) */
};

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

int
amdgpu_device_initialize(int fd, uint32_t *major, uint32_t *minor, amdgpu_device_handle *out)
{
	struct amdgpu_device *d;
	struct drm_amdgpu_info in;
	drmVersionPtr v;
	uint64_t start, max, hmax, align;
	int r;

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
	/* as libdrm divides the space: below 4 GB, the rest, and the same high */
	align = d->info.virtual_address_alignment;
	start = d->info.virtual_address_offset;
	max = d->info.virtual_address_max < 0x100000000ULL ? d->info.virtual_address_max : 0x100000000ULL;
	d->vam[0] = vaminit(start, max, align);
	start = max;
	max = d->info.virtual_address_max > 0x100000000ULL ? d->info.virtual_address_max : 0x100000000ULL;
	d->vam[1] = vaminit(start, max, align);
	/*
	 * RADV's workaround for scalar loads from NULL PRT pages: the high
	 * range is two halves, told apart by one bit; objects go in the lower
	 * half, a sparse buffer's address has the bit set, its twin without it
	 * is mapped to zeros (radv_amdgpu_bo.c); shaders mask the bit off
	 * their scalar loads (ac_nir_fixup_smem_loads_null_prt).  So the bit,
	 * and the allocator below it.
	 */
	d->prtbit = 0;
	while(d->prtbit < 63 && (d->info.high_va_max - d->info.high_va_offset) >> (d->prtbit + 2) != 0)
		d->prtbit++;
	hmax = d->info.high_va_offset + (1ULL << d->prtbit);
	start = d->info.high_va_offset;
	max = (start & ~0xffffffffULL) + 0x100000000ULL;
	if(d->info.high_va_max < max)
		max = d->info.high_va_max;
	d->vam[2] = vaminit(start, max, align);
	start = max;
	max = hmax > (start & ~0xffffffffULL) + 0x100000000ULL ?
		hmax : (start & ~0xffffffffULL) + 0x100000000ULL;
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
		*v = d->prtbit;	/* (~0 here made RADV mask bit 63 off every scalar load: 1ull << 255) */
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
	switch(type){
	case amdgpu_bo_handle_type_kms:
	case amdgpu_bo_handle_type_kms_noimport:
		*shared = b->handle;
		return 0;
	default:
		return -EINVAL;	/* no flink names, no dma-buf descriptors */
	}
}

int
amdgpu_bo_import(amdgpu_device_handle d, enum amdgpu_bo_handle_type type, uint32_t shared,
	struct amdgpu_bo_import_result *out)
{
	struct drm_amdgpu_gem_op op;
	struct drm_amdgpu_gem_create_in info;
	struct amdgpu_bo *b;
	int r;

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

/* the driver's view of the object: GTT a segment of its own, VRAM where BAR 0 shows it */
int
amdgpu_bo_cpu_map(amdgpu_bo_handle b, void **cpu)
{
	Amd *a;
	char reply[256], seg[64];
	unsigned long long va, size, off, barsize;
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
	if(sscanf(reply, "map %*u %63s %lli %lli %lli %lli", seg, &va, &size, &off, &barsize) == 5){
		/* VRAM: BAR 0, once for the descriptor */
		pthread_mutex_lock(&a->lk);
		if(a->bar0 == NULL){
			a->bar0 = _SEGATTACH(0, seg, NULL, barsize);
			if(a->bar0 == (void*)-1)
				a->bar0 = NULL;
			a->bar0size = barsize;
		}
		pthread_mutex_unlock(&a->lk);
		if(a->bar0 == NULL || off + size > a->bar0size){
			dbg("map %u: BAR 0 %s: %s", b->handle, seg, strerror(errno));
			ctlreq(a, NULL, 0, "unmap %u", b->handle);
			return -ENOMEM;
		}
		p = a->bar0 + off;
		b->vram = 1;
	}else if(sscanf(reply, "map %*u %63s %lli %lli", seg, &va, &size) == 3){
		p = _SEGATTACH(0, seg, NULL, size);
		if(p == (void*)-1){
			dbg("map %u: segattach %s %#llx: %s", b->handle, seg, size, strerror(errno));
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
		_SEGDETACH(p);
	b->seg = b->vram = 0;
	if((a = amd9of(b->dev->fd)) != NULL)
		ctlreq(a, NULL, 0, "unmap %u", b->handle);
	return 0;
}

int
amdgpu_bo_free(amdgpu_bo_handle b)
{
	struct drm_gem_close cl;

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
