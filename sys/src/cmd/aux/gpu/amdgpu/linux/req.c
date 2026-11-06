// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: a context's requests as text (gpu(4)), the Linux side.
 * Each request is words, numbers in C's notation, written to the
 * context's ctl (or submit for cs); the reply is text, read back on the
 * same fid.  The request becomes one of amdgpu's ioctls, its structure
 * and all it points to built here, in memory of the server's own: the
 * proc's user window (kpi/obsd/systm.c) is that memory while drm_ioctl
 * runs, so nothing of the client's is read while Linux works on it, and
 * only what has a word here can be asked for.
 *
 *	gem size align domains flags		GEM_CREATE: gem handle
 *	geminfo handle				GEM_OP GET_GEM_CREATE_INFO:
 *						geminfo handle size align domains flags
 *	place handle domains			GEM_OP SET_PLACEMENT
 *	meta handle				GEM_METADATA get: meta handle flags tiling hex
 *	meta handle flags tiling hex		GEM_METADATA set
 *	va op handle addr offset size flags [out sync point] [in sync ...]
 *						GEM_VA, op map unmap clear replace
 *	idle handle timeout			GEM_WAIT_IDLE: idle done|busy domain
 *	hwctx new priority flags		CTX alloc: hwctx id
 *	hwctx free id
 *	hwctx state id				CTX query_state2: hwctx state flags
 *	pstate id [value]			CTX get/set stable pstate: pstate value
 *	vmid reserve|unreserve			VM
 *	waitcs hwctx ip inst ring seq timeout	WAIT_CS: waitcs done|busy
 *	waitfences all|any timeout hwctx ip inst ring seq ...
 *						WAIT_FENCES: waitfences status first
 *	fromfence hwctx ip inst ring seq	FENCE_TO_HANDLE (a syncobj): fromfence sync
 *	syncnew [signaled]			SYNCOBJ_CREATE: syncnew sync
 *	syncfree sync
 *	syncwait flags timeout sync ...		SYNCOBJ_WAIT: syncwait first
 *	synctwait flags timeout sync point ...	SYNCOBJ_TIMELINE_WAIT: synctwait first
 *	syncreset sync ...			SYNCOBJ_RESET
 *	syncsignal sync ...			SYNCOBJ_SIGNAL
 *	synctsignal sync point ...		SYNCOBJ_TIMELINE_SIGNAL
 *	syncquery flags sync ...		SYNCOBJ_QUERY: syncquery point ...
 *	synctransfer src srcpoint dst dstpoint flags	SYNCOBJ_TRANSFER
 *	info query [args]			INFO: see infos below
 *	version					VERSION: version major minor patch,
 *						then name, date, desc a line each
 *	cs hwctx flags item ...			CS (written to submit): cs seq
 *		ib ip inst ring va bytes flags	an IB
 *		fence handle offset		the user fence
 *		in sync, out sync		binary syncobjs waited for, signalled
 *		twait sync point flags, tsignal sync point flags	timeline ones
 *		bo handle priority		the BO list
 *
 * Domains and flags are names joined by commas or numbers (0: none);
 * timeouts are as amdgpu's ioctls take them, absolute nanoseconds of
 * the monotonic clock.  Errors are Linux's (negative), fs.c names them.
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm.h>
#include <drm/amdgpu_drm.h>
#include "../amdgpu.h"

struct file	*amdgpu_ctx_filp(void *c);	/* ctx.c */

enum {
	Scratch	= 256*1024,	/* a request's structures */
	Maxlist	= 2048,		/* syncobjs, fences, BOs, IBs in one request */
};

/* a request's memory, the user window while its ioctl runs */
struct rq {
	char	*base;
	size_t	n, max;
	char	*reply;
	int	nreply, len;
};

static void *
rqalloc(struct rq *q, size_t n)
{
	void *p;

	n = (n + 15) & ~(size_t)15;
	if (q->n + n > q->max)
		return NULL;
	p = q->base + q->n;
	q->n += n;
	memset(p, 0, n);
	return p;
}

static void
rprint(struct rq *q, const char *fmt, ...)
{
	va_list arg;
	int m;

	if (q->len >= q->nreply - 1)
		return;
	va_start(arg, fmt);
	m = vsnprintf(q->reply + q->len, q->nreply - q->len, fmt, arg);
	va_end(arg);
	q->len += m;
	if (q->len >= q->nreply)
		q->len = q->nreply - 1;
}

static long
rqioctl(void *c, struct rq *q, unsigned int cmd, void *arg, int priv)
{
	long r;

	kpi_user_window(q->base, q->max);
	kpi_user_privileged(priv);
	r = drm_ioctl(amdgpu_ctx_filp(c), cmd, (unsigned long)arg);
	kpi_user_privileged(1);
	kpi_user_window(NULL, 0);
	return r;
}

/*
 * words
 */
static int
num(const char *s, u64 *v)
{
	long long x;
	unsigned long long u;

	if (s[0] == '-') {
		if (kstrtoll(s, 0, &x))
			return -EINVAL;
		*v = (u64)x;
		return 0;
	}
	if (kstrtoull(s, 0, &u))
		return -EINVAL;
	*v = u;
	return 0;
}

#define NUM(s, v)	do { if (num((s), &(v))) return -EINVAL; } while (0)

static int
num32(const char *s, u32 *v)
{
	u64 x;

	/* 32 bits, unsigned or negative */
	if (num(s, &x) || (x > 0xffffffffULL && ((s64)x >= 0 || (s64)x < -0x80000000LL)))
		return -EINVAL;
	*v = (u32)x;
	return 0;
}

#define NUM32(s, v)	do { if (num32((s), &(v))) return -EINVAL; } while (0)

struct name {
	const char	*s;
	u64	bit;
};

static const struct name domains[] = {
	{ "cpu", AMDGPU_GEM_DOMAIN_CPU }, { "gtt", AMDGPU_GEM_DOMAIN_GTT },
	{ "vram", AMDGPU_GEM_DOMAIN_VRAM }, { "gds", AMDGPU_GEM_DOMAIN_GDS },
	{ "gws", AMDGPU_GEM_DOMAIN_GWS }, { "oa", AMDGPU_GEM_DOMAIN_OA },
	{ "doorbell", AMDGPU_GEM_DOMAIN_DOORBELL },
	{ NULL, 0 }
};

static const struct name gemflags[] = {
	{ "cpuaccess", AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED },
	{ "nocpuaccess", AMDGPU_GEM_CREATE_NO_CPU_ACCESS },
	{ "uswc", AMDGPU_GEM_CREATE_CPU_GTT_USWC },
	{ "cleared", AMDGPU_GEM_CREATE_VRAM_CLEARED },
	{ "contiguous", AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS },
	{ "local", AMDGPU_GEM_CREATE_VM_ALWAYS_VALID },
	{ "explicitsync", AMDGPU_GEM_CREATE_EXPLICIT_SYNC },
	{ "mqd", AMDGPU_GEM_CREATE_CP_MQD_GFX9 },
	{ "wipe", AMDGPU_GEM_CREATE_VRAM_WIPE_ON_RELEASE },
	{ "encrypted", AMDGPU_GEM_CREATE_ENCRYPTED },
	{ "preemptible", AMDGPU_GEM_CREATE_PREEMPTIBLE },
	{ "discardable", AMDGPU_GEM_CREATE_DISCARDABLE },
	{ "coherent", AMDGPU_GEM_CREATE_COHERENT },
	{ "uncached", AMDGPU_GEM_CREATE_UNCACHED },
	{ "extcoherent", AMDGPU_GEM_CREATE_EXT_COHERENT },
	{ "dcc", AMDGPU_GEM_CREATE_GFX12_DCC },
	{ NULL, 0 }
};

static const struct name vaflags[] = {
	{ "r", AMDGPU_VM_PAGE_READABLE }, { "w", AMDGPU_VM_PAGE_WRITEABLE },
	{ "x", AMDGPU_VM_PAGE_EXECUTABLE }, { "prt", AMDGPU_VM_PAGE_PRT },
	{ "noalloc", AMDGPU_VM_PAGE_NOALLOC },
	{ NULL, 0 }
};

static const struct name syncflags[] = {
	{ "all", DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL },
	{ "submit", DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT },
	{ "available", DRM_SYNCOBJ_WAIT_FLAGS_WAIT_AVAILABLE },
	{ NULL, 0 }
};

/* names joined by commas, or a number */
static int
bits(const char *s, const struct name *t, u64 *v)
{
	char w[32];
	const char *e;
	int i, n;

	if (num(s, v) == 0)
		return 0;
	*v = 0;
	while (*s) {
		e = strchr(s, ',');
		n = e ? e - s : strlen(s);
		if (n >= (int)sizeof w)
			return -EINVAL;
		memcpy(w, s, n);
		w[n] = 0;
		for (i = 0; t[i].s; i++)
			if (strcmp(t[i].s, w) == 0)
				break;
		if (!t[i].s)
			return -EINVAL;
		*v |= t[i].bit;
		s += n;
		if (*s == ',')
			s++;
	}
	return 0;
}

#define BITS(s, t, v)	do { if (bits((s), (t), &(v))) return -EINVAL; } while (0)

static void
pbits(struct rq *q, u64 v, const struct name *t)
{
	int i, any = 0;

	for (i = 0; t[i].s; i++)
		if (v & t[i].bit) {
			rprint(q, "%s%s", any ? "," : "", t[i].s);
			v &= ~t[i].bit;
			any = 1;
		}
	if (v || !any)
		rprint(q, "%s%#llx", any ? "," : "", v);
}

/*
 * objects
 */
static long
gem(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_gem_create *g;
	long r;

	if (n != 5)
		return -EINVAL;
	g = rqalloc(q, sizeof *g);
	NUM(f[1], g->in.bo_size);
	NUM(f[2], g->in.alignment);
	BITS(f[3], domains, g->in.domains);
	BITS(f[4], gemflags, g->in.domain_flags);
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_CREATE, g, priv);
	if (r == 0)
		rprint(q, "gem %u\n", g->out.handle);
	return r;
}

static long
gemop(void *c, struct rq *q, char **f, int n, int priv)
{
	struct drm_amdgpu_gem_op *o;
	struct drm_amdgpu_gem_create_in *ci;
	u64 d;
	long r;

	o = rqalloc(q, sizeof *o);
	if (strcmp(f[0], "geminfo") == 0 && n == 2) {
		ci = rqalloc(q, sizeof *ci);
		NUM32(f[1], o->handle);
		o->op = AMDGPU_GEM_OP_GET_GEM_CREATE_INFO;
		o->value = (u64)(uintptr_t)ci;
		r = rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_OP, o, priv);
		if (r == 0) {
			rprint(q, "geminfo %u %llu %llu ", o->handle, ci->bo_size, ci->alignment);
			pbits(q, ci->domains, domains);
			rprint(q, " ");
			pbits(q, ci->domain_flags, gemflags);
			rprint(q, "\n");
		}
		return r;
	}
	if (strcmp(f[0], "place") == 0 && n == 3) {
		NUM32(f[1], o->handle);
		BITS(f[2], domains, d);
		o->op = AMDGPU_GEM_OP_SET_PLACEMENT;
		o->value = d;
		return rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_OP, o, priv);
	}
	return -EINVAL;
}

static int
unhex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

/* meta handle; meta handle flags tiling hex (the UMD's bytes; - none) */
static long
meta(void *c, struct rq *q, char **f, int n, int priv)
{
	struct drm_amdgpu_gem_metadata *m;
	unsigned char *b;
	u32 i, len;
	long r;

	m = rqalloc(q, sizeof *m);
	NUM32(f[1], m->handle);
	if (n == 2) {
		m->op = AMDGPU_GEM_METADATA_OP_GET_METADATA;
		r = rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_METADATA, m, priv);
		if (r)
			return r;
		len = min_t(u32, m->data.data_size_bytes, sizeof m->data.data);
		rprint(q, "meta %u %#llx %#llx ", m->handle, m->data.flags, m->data.tiling_info);
		b = (unsigned char *)m->data.data;
		for (i = 0; i < len; i++)
			rprint(q, "%02x", b[i]);
		rprint(q, "%s\n", len ? "" : "-");
		return 0;
	}
	if (n != 5)
		return -EINVAL;
	m->op = AMDGPU_GEM_METADATA_OP_SET_METADATA;
	NUM(f[2], m->data.flags);
	NUM(f[3], m->data.tiling_info);
	if (strcmp(f[4], "-") != 0) {
		len = strlen(f[4]);
		if (len % 2 || len / 2 > sizeof m->data.data)
			return -EINVAL;
		b = (unsigned char *)m->data.data;
		for (i = 0; i < len / 2; i++) {
			int h = unhex(f[4][2*i]), l = unhex(f[4][2*i+1]);

			if (h < 0 || l < 0)
				return -EINVAL;
			b[i] = h << 4 | l;
		}
		m->data.data_size_bytes = len / 2;
	}
	return rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_METADATA, m, priv);
}

/* va op handle addr offset size flags [out sync point] [in sync ...] */
static long
va(void *c, struct rq *q, char **f, int n, int priv)
{
	static const char *ops[] = { "map", "unmap", "clear", "replace" };
	struct drm_amdgpu_gem_va *v;
	u32 *in;
	u64 x;
	int i, k;

	if (n < 7)
		return -EINVAL;
	v = rqalloc(q, sizeof *v);
	for (i = 0; i < 4; i++)
		if (strcmp(f[1], ops[i]) == 0)
			v->operation = AMDGPU_VA_OP_MAP + i;
	if (!v->operation)
		return -EINVAL;
	NUM32(f[2], v->handle);
	NUM(f[3], v->va_address);
	NUM(f[4], v->offset_in_bo);
	NUM(f[5], v->map_size);
	BITS(f[6], vaflags, x);
	v->flags = x;
	for (i = 7; i < n; ) {
		if (strcmp(f[i], "out") == 0 && i + 2 < n) {
			NUM32(f[i+1], v->vm_timeline_syncobj_out);
			NUM(f[i+2], v->vm_timeline_point);
			i += 3;
		} else if (strcmp(f[i], "in") == 0) {
			k = n - i - 1;
			if (k <= 0 || k > Maxlist || !(in = rqalloc(q, k * sizeof *in)))
				return -EINVAL;
			for (k = 0, i++; i < n; i++, k++)
				NUM32(f[i], in[k]);
			v->num_syncobj_handles = k;
			v->input_fence_syncobj_handles = (u64)(uintptr_t)in;
		} else
			return -EINVAL;
	}
	return rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_VA, v, priv);
}

static long
idle(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_gem_wait_idle *w;
	long r;

	if (n != 3)
		return -EINVAL;
	w = rqalloc(q, sizeof *w);
	NUM32(f[1], w->in.handle);
	NUM(f[2], w->in.timeout);
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_GEM_WAIT_IDLE, w, priv);
	if (r == 0)
		rprint(q, "idle %s %u\n", w->out.status ? "busy" : "done", w->out.domain);
	return r;
}

/*
 * amdgpu contexts, the VM
 */
static long
hwctx(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_ctx *x;
	u64 v;
	long r;

	x = rqalloc(q, sizeof *x);
	if (strcmp(f[0], "pstate") == 0) {
		if (n != 2 && n != 3)
			return -EINVAL;
		NUM32(f[1], x->in.ctx_id);
		x->in.op = n == 2 ? AMDGPU_CTX_OP_GET_STABLE_PSTATE : AMDGPU_CTX_OP_SET_STABLE_PSTATE;
		if (n == 3) {
			NUM(f[2], v);
			x->in.flags = v;
		}
		r = rqioctl(c, q, DRM_IOCTL_AMDGPU_CTX, x, priv);
		if (r == 0 && n == 2)
			rprint(q, "pstate %u\n", x->out.pstate.flags);
		return r;
	}
	if (n == 4 && strcmp(f[1], "new") == 0) {
		x->in.op = AMDGPU_CTX_OP_ALLOC_CTX;
		NUM(f[2], v);
		x->in.priority = (s32)v;
		NUM(f[3], v);
		x->in.flags = v;
		r = rqioctl(c, q, DRM_IOCTL_AMDGPU_CTX, x, priv);
		if (r == 0)
			rprint(q, "hwctx %u\n", x->out.alloc.ctx_id);
		return r;
	}
	if (n == 3 && strcmp(f[1], "free") == 0) {
		x->in.op = AMDGPU_CTX_OP_FREE_CTX;
		NUM32(f[2], x->in.ctx_id);
		return rqioctl(c, q, DRM_IOCTL_AMDGPU_CTX, x, priv);
	}
	if (n == 3 && strcmp(f[1], "state") == 0) {
		x->in.op = AMDGPU_CTX_OP_QUERY_STATE2;
		NUM32(f[2], x->in.ctx_id);
		r = rqioctl(c, q, DRM_IOCTL_AMDGPU_CTX, x, priv);
		if (r == 0)
			rprint(q, "hwctx state %#llx\n", x->out.state.flags);
		return r;
	}
	return -EINVAL;
}

static long
vmid(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_vm *v;

	if (n != 2)
		return -EINVAL;
	v = rqalloc(q, sizeof *v);
	if (strcmp(f[1], "reserve") == 0)
		v->in.op = AMDGPU_VM_OP_RESERVE_VMID;
	else if (strcmp(f[1], "unreserve") == 0)
		v->in.op = AMDGPU_VM_OP_UNRESERVE_VMID;
	else
		return -EINVAL;
	return rqioctl(c, q, DRM_IOCTL_AMDGPU_VM, v, priv);
}

/*
 * fences of submissions
 */
static long
waitcs(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_wait_cs *w;
	long r;

	if (n != 7)
		return -EINVAL;
	w = rqalloc(q, sizeof *w);
	NUM32(f[1], w->in.ctx_id);
	NUM32(f[2], w->in.ip_type);
	NUM32(f[3], w->in.ip_instance);
	NUM32(f[4], w->in.ring);
	NUM(f[5], w->in.handle);
	NUM(f[6], w->in.timeout);
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_WAIT_CS, w, priv);
	if (r == 0)
		rprint(q, "waitcs %s\n", w->out.status ? "busy" : "done");
	return r;
}

/* five words: hwctx ip inst ring seq */
static int
fence5(char **f, struct drm_amdgpu_fence *e)
{
	if (num32(f[0], &e->ctx_id) || num32(f[1], &e->ip_type) || num32(f[2], &e->ip_instance) ||
	    num32(f[3], &e->ring) || num(f[4], &e->seq_no))
		return -EINVAL;
	return 0;
}

static long
waitfences(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_wait_fences *w;
	struct drm_amdgpu_fence *e;
	int k, i;
	long r;

	k = (n - 3) / 5;
	if (n < 8 || (n - 3) % 5 || k > Maxlist)
		return -EINVAL;
	w = rqalloc(q, sizeof *w);
	if (!(e = rqalloc(q, k * sizeof *e)))
		return -ENOMEM;
	if (strcmp(f[1], "all") == 0)
		w->in.wait_all = 1;
	else if (strcmp(f[1], "any") != 0)
		return -EINVAL;
	NUM(f[2], w->in.timeout_ns);
	for (i = 0; i < k; i++)
		if (fence5(f + 3 + 5*i, &e[i]))
			return -EINVAL;
	w->in.fences = (u64)(uintptr_t)e;
	w->in.fence_count = k;
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_WAIT_FENCES, w, priv);
	if (r == 0)
		rprint(q, "waitfences %u %u\n", w->out.status, w->out.first_signaled);
	return r;
}

static long
fromfence(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_fence_to_handle *h;
	long r;

	if (n != 6)
		return -EINVAL;
	h = rqalloc(q, sizeof *h);
	if (fence5(f + 1, &h->in.fence))
		return -EINVAL;
	h->in.what = AMDGPU_FENCE_TO_HANDLE_GET_SYNCOBJ;	/* descriptors: none */
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_FENCE_TO_HANDLE, h, priv);
	if (r == 0)
		rprint(q, "fromfence %u\n", h->out.handle);
	return r;
}

/*
 * syncobjs
 */
/* syncobj handles (and points, timeline: pairs) from f[0] on */
static int
handles(struct rq *q, char **f, int n, int timeline, u32 **h, u64 **pt)
{
	int k, i;

	k = timeline ? n / 2 : n;
	if (n <= 0 || k > Maxlist || (timeline && n % 2))
		return -EINVAL;
	if (!(*h = rqalloc(q, k * sizeof **h)))
		return -ENOMEM;
	if (timeline && !(*pt = rqalloc(q, k * sizeof **pt)))
		return -ENOMEM;
	for (i = 0; i < k; i++)
		if (timeline) {
			if (num32(f[2*i], &(*h)[i]) || num(f[2*i+1], &(*pt)[i]))
				return -EINVAL;
		} else if (num32(f[i], &(*h)[i]))
			return -EINVAL;
	return k;
}

static long
syncreq(void *c, struct rq *q, char **f, int n, int priv)
{
	char *v = f[0] + 4;
	u32 *h = NULL;
	u64 *pt = NULL, x;
	int k;
	long r;

	if (strcmp(v, "new") == 0 && n <= 2) {
		struct drm_syncobj_create *s = rqalloc(q, sizeof *s);

		if (n == 2) {
			if (strcmp(f[1], "signaled") != 0)
				return -EINVAL;
			s->flags = DRM_SYNCOBJ_CREATE_SIGNALED;
		}
		r = rqioctl(c, q, DRM_IOCTL_SYNCOBJ_CREATE, s, priv);
		if (r == 0)
			rprint(q, "syncnew %u\n", s->handle);
		return r;
	}
	if (strcmp(v, "free") == 0 && n == 2) {
		struct drm_syncobj_destroy *s = rqalloc(q, sizeof *s);

		NUM32(f[1], s->handle);
		return rqioctl(c, q, DRM_IOCTL_SYNCOBJ_DESTROY, s, priv);
	}
	if (strcmp(v, "wait") == 0 && n >= 4) {
		struct drm_syncobj_wait *s = rqalloc(q, sizeof *s);

		BITS(f[1], syncflags, x);
		s->flags = x;
		NUM(f[2], x);
		s->timeout_nsec = (s64)x;
		if ((k = handles(q, f + 3, n - 3, 0, &h, NULL)) < 0)
			return k;
		s->handles = (u64)(uintptr_t)h;
		s->count_handles = k;
		r = rqioctl(c, q, DRM_IOCTL_SYNCOBJ_WAIT, s, priv);
		if (r == 0)
			rprint(q, "syncwait %u\n", s->first_signaled);
		return r;
	}
	if (strcmp(v, "twait") == 0 && n >= 5) {
		struct drm_syncobj_timeline_wait *s = rqalloc(q, sizeof *s);

		BITS(f[1], syncflags, x);
		s->flags = x;
		NUM(f[2], x);
		s->timeout_nsec = (s64)x;
		if ((k = handles(q, f + 3, n - 3, 1, &h, &pt)) < 0)
			return k;
		s->handles = (u64)(uintptr_t)h;
		s->points = (u64)(uintptr_t)pt;
		s->count_handles = k;
		r = rqioctl(c, q, DRM_IOCTL_SYNCOBJ_TIMELINE_WAIT, s, priv);
		if (r == 0)
			rprint(q, "synctwait %u\n", s->first_signaled);
		return r;
	}
	if ((strcmp(v, "reset") == 0 || strcmp(v, "signal") == 0) && n >= 2) {
		struct drm_syncobj_array *s = rqalloc(q, sizeof *s);

		if ((k = handles(q, f + 1, n - 1, 0, &h, NULL)) < 0)
			return k;
		s->handles = (u64)(uintptr_t)h;
		s->count_handles = k;
		return rqioctl(c, q, v[0] == 'r' ? DRM_IOCTL_SYNCOBJ_RESET : DRM_IOCTL_SYNCOBJ_SIGNAL, s, priv);
	}
	if (strcmp(v, "tsignal") == 0 && n >= 3) {
		struct drm_syncobj_timeline_array *s = rqalloc(q, sizeof *s);

		if ((k = handles(q, f + 1, n - 1, 1, &h, &pt)) < 0)
			return k;
		s->handles = (u64)(uintptr_t)h;
		s->points = (u64)(uintptr_t)pt;
		s->count_handles = k;
		return rqioctl(c, q, DRM_IOCTL_SYNCOBJ_TIMELINE_SIGNAL, s, priv);
	}
	if (strcmp(v, "query") == 0 && n >= 3) {
		struct drm_syncobj_timeline_array *s = rqalloc(q, sizeof *s);
		int i;

		NUM(f[1], x);
		s->flags = x;
		if ((k = handles(q, f + 2, n - 2, 0, &h, NULL)) < 0)
			return k;
		if (!(pt = rqalloc(q, k * sizeof *pt)))
			return -ENOMEM;
		s->handles = (u64)(uintptr_t)h;
		s->points = (u64)(uintptr_t)pt;
		s->count_handles = k;
		r = rqioctl(c, q, DRM_IOCTL_SYNCOBJ_QUERY, s, priv);
		if (r == 0) {
			rprint(q, "syncquery");
			for (i = 0; i < k; i++)
				rprint(q, " %llu", pt[i]);
			rprint(q, "\n");
		}
		return r;
	}
	if (strcmp(v, "transfer") == 0 && n == 6) {
		struct drm_syncobj_transfer *s = rqalloc(q, sizeof *s);

		NUM32(f[1], s->src_handle);
		NUM(f[2], s->src_point);
		NUM32(f[3], s->dst_handle);
		NUM(f[4], s->dst_point);
		BITS(f[5], syncflags, x);
		s->flags = x;
		return rqioctl(c, q, DRM_IOCTL_SYNCOBJ_TRANSFER, s, priv);
	}
	return -EINVAL;
}

/*
 * cs hwctx flags item ...: the chunks in the order given, the IBs first
 */
static long
cs(void *c, struct rq *q, char **f, int n, int priv)
{
	union drm_amdgpu_cs *x;
	struct drm_amdgpu_cs_chunk *ch;
	u64 *chp, v;
	int i, nch, maxch;
	long r;

	if (n < 3)
		return -EINVAL;
	x = rqalloc(q, sizeof *x);
	NUM32(f[1], x->in.ctx_id);
	NUM(f[2], v);
	x->in.flags = v;
	maxch = n;	/* at most one chunk a word */
	if (!(chp = rqalloc(q, maxch * sizeof *chp)) || !(ch = rqalloc(q, maxch * sizeof *ch)))
		return -ENOMEM;
	nch = 0;
	for (i = 3; i < n; ) {
		struct drm_amdgpu_cs_chunk *k = &ch[nch];

		if (strcmp(f[i], "ib") == 0 && i + 6 < n) {
			struct drm_amdgpu_cs_chunk_ib *ib = rqalloc(q, sizeof *ib);

			if (!ib)
				return -ENOMEM;
			NUM32(f[i+1], ib->ip_type);
			NUM32(f[i+2], ib->ip_instance);
			NUM32(f[i+3], ib->ring);
			NUM(f[i+4], ib->va_start);
			NUM32(f[i+5], ib->ib_bytes);
			NUM32(f[i+6], ib->flags);
			k->chunk_id = AMDGPU_CHUNK_ID_IB;
			k->length_dw = sizeof *ib / 4;
			k->chunk_data = (u64)(uintptr_t)ib;
			i += 7;
		} else if (strcmp(f[i], "fence") == 0 && i + 2 < n) {
			struct drm_amdgpu_cs_chunk_fence *fe = rqalloc(q, sizeof *fe);

			if (!fe)
				return -ENOMEM;
			NUM32(f[i+1], fe->handle);
			NUM32(f[i+2], fe->offset);
			k->chunk_id = AMDGPU_CHUNK_ID_FENCE;
			k->length_dw = sizeof *fe / 4;
			k->chunk_data = (u64)(uintptr_t)fe;
			i += 3;
		} else if ((strcmp(f[i], "in") == 0 || strcmp(f[i], "out") == 0) && i + 1 < n) {
			struct drm_amdgpu_cs_chunk_sem *s = rqalloc(q, sizeof *s);

			if (!s)
				return -ENOMEM;
			NUM32(f[i+1], s->handle);
			k->chunk_id = f[i][0] == 'i' ? AMDGPU_CHUNK_ID_SYNCOBJ_IN : AMDGPU_CHUNK_ID_SYNCOBJ_OUT;
			k->length_dw = sizeof *s / 4;
			k->chunk_data = (u64)(uintptr_t)s;
			i += 2;
		} else if ((strcmp(f[i], "twait") == 0 || strcmp(f[i], "tsignal") == 0) && i + 3 < n) {
			struct drm_amdgpu_cs_chunk_syncobj *s = rqalloc(q, sizeof *s);

			if (!s)
				return -ENOMEM;
			NUM32(f[i+1], s->handle);
			NUM(f[i+2], s->point);
			BITS(f[i+3], syncflags, v);
			s->flags = v;
			k->chunk_id = f[i][1] == 'w' ? AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT : AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_SIGNAL;
			k->length_dw = sizeof *s / 4;
			k->chunk_data = (u64)(uintptr_t)s;
			i += 4;
		} else if (strcmp(f[i], "bo") == 0) {
			struct drm_amdgpu_bo_list_in *bl = rqalloc(q, sizeof *bl);
			struct drm_amdgpu_bo_list_entry *e;
			int m, j;

			/* the BOs that follow, handle priority each */
			for (m = 0; i + 2 + 3*m < n && strcmp(f[i + 3*m], "bo") == 0; m++)
				;
			if (!bl || m > Maxlist || !(e = rqalloc(q, m * sizeof *e)))
				return -ENOMEM;
			for (j = 0; j < m; j++, i += 3) {
				NUM32(f[i+1], e[j].bo_handle);
				NUM32(f[i+2], e[j].bo_priority);
			}
			bl->bo_number = m;
			bl->bo_info_size = sizeof *e;
			bl->bo_info_ptr = (u64)(uintptr_t)e;
			k->chunk_id = AMDGPU_CHUNK_ID_BO_HANDLES;
			k->length_dw = sizeof *bl / 4;
			k->chunk_data = (u64)(uintptr_t)bl;
		} else
			return -EINVAL;
		chp[nch] = (u64)(uintptr_t)k;
		nch++;
	}
	x->in.num_chunks = nch;
	x->in.chunks = (u64)(uintptr_t)chp;
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_CS, x, priv);
	if (r == 0)
		rprint(q, "cs %llu\n", x->out.handle);
	return r;
}

/*
 * INFO: a structure's fields, "name value" a line, by tables of the
 * fields; the client fills its own structure by the names
 */
enum { U32, U64, A32 };
struct field {
	const char	*name;
	int	type;
	size_t	off;
	int	count;		/* A32 */
};
#define F32(s, f)	{ #f, U32, offsetof(struct s, f), 1 }
#define F64(s, f)	{ #f, U64, offsetof(struct s, f), 1 }
#define FA(s, f, n)	{ #f, A32, offsetof(struct s, f), n }
#define FN(s, p, f, t)	{ #p "." #f, t, offsetof(struct s, p.f), 1 }

static const struct field devinfo[] = {
	F32(drm_amdgpu_info_device, device_id), F32(drm_amdgpu_info_device, chip_rev),
	F32(drm_amdgpu_info_device, external_rev), F32(drm_amdgpu_info_device, pci_rev),
	F32(drm_amdgpu_info_device, family), F32(drm_amdgpu_info_device, num_shader_engines),
	F32(drm_amdgpu_info_device, num_shader_arrays_per_engine), F32(drm_amdgpu_info_device, gpu_counter_freq),
	F64(drm_amdgpu_info_device, max_engine_clock), F64(drm_amdgpu_info_device, max_memory_clock),
	F32(drm_amdgpu_info_device, cu_active_number), F32(drm_amdgpu_info_device, cu_ao_mask),
	FA(drm_amdgpu_info_device, cu_bitmap, 16), F32(drm_amdgpu_info_device, enabled_rb_pipes_mask),
	F32(drm_amdgpu_info_device, num_rb_pipes), F32(drm_amdgpu_info_device, num_hw_gfx_contexts),
	F32(drm_amdgpu_info_device, pcie_gen), F64(drm_amdgpu_info_device, ids_flags),
	F64(drm_amdgpu_info_device, virtual_address_offset), F64(drm_amdgpu_info_device, virtual_address_max),
	F32(drm_amdgpu_info_device, virtual_address_alignment), F32(drm_amdgpu_info_device, pte_fragment_size),
	F32(drm_amdgpu_info_device, gart_page_size), F32(drm_amdgpu_info_device, ce_ram_size),
	F32(drm_amdgpu_info_device, vram_type), F32(drm_amdgpu_info_device, vram_bit_width),
	F32(drm_amdgpu_info_device, vce_harvest_config), F32(drm_amdgpu_info_device, gc_double_offchip_lds_buf),
	F64(drm_amdgpu_info_device, prim_buf_gpu_addr), F64(drm_amdgpu_info_device, pos_buf_gpu_addr),
	F64(drm_amdgpu_info_device, cntl_sb_buf_gpu_addr), F64(drm_amdgpu_info_device, param_buf_gpu_addr),
	F32(drm_amdgpu_info_device, prim_buf_size), F32(drm_amdgpu_info_device, pos_buf_size),
	F32(drm_amdgpu_info_device, cntl_sb_buf_size), F32(drm_amdgpu_info_device, param_buf_size),
	F32(drm_amdgpu_info_device, wave_front_size), F32(drm_amdgpu_info_device, num_shader_visible_vgprs),
	F32(drm_amdgpu_info_device, num_cu_per_sh), F32(drm_amdgpu_info_device, num_tcc_blocks),
	F32(drm_amdgpu_info_device, gs_vgt_table_depth), F32(drm_amdgpu_info_device, gs_prim_buffer_depth),
	F32(drm_amdgpu_info_device, max_gs_waves_per_vgt), F32(drm_amdgpu_info_device, pcie_num_lanes),
	FA(drm_amdgpu_info_device, cu_ao_bitmap, 16), F64(drm_amdgpu_info_device, high_va_offset),
	F64(drm_amdgpu_info_device, high_va_max), F32(drm_amdgpu_info_device, pa_sc_tile_steering_override),
	F64(drm_amdgpu_info_device, tcc_disabled_mask), F64(drm_amdgpu_info_device, min_engine_clock),
	F64(drm_amdgpu_info_device, min_memory_clock), F32(drm_amdgpu_info_device, tcp_cache_size),
	F32(drm_amdgpu_info_device, num_sqc_per_wgp), F32(drm_amdgpu_info_device, sqc_data_cache_size),
	F32(drm_amdgpu_info_device, sqc_inst_cache_size), F32(drm_amdgpu_info_device, gl1c_cache_size),
	F32(drm_amdgpu_info_device, gl2c_cache_size), F64(drm_amdgpu_info_device, mall_size),
	F32(drm_amdgpu_info_device, enabled_rb_pipes_mask_hi), F32(drm_amdgpu_info_device, shadow_size),
	F32(drm_amdgpu_info_device, shadow_alignment), F32(drm_amdgpu_info_device, csa_size),
	F32(drm_amdgpu_info_device, csa_alignment), F32(drm_amdgpu_info_device, userq_ip_mask),
	{ NULL }
};

static const struct field hwip[] = {
	F32(drm_amdgpu_info_hw_ip, hw_ip_version_major), F32(drm_amdgpu_info_hw_ip, hw_ip_version_minor),
	F64(drm_amdgpu_info_hw_ip, capabilities_flags), F32(drm_amdgpu_info_hw_ip, ib_start_alignment),
	F32(drm_amdgpu_info_hw_ip, ib_size_alignment), F32(drm_amdgpu_info_hw_ip, available_rings),
	F32(drm_amdgpu_info_hw_ip, ip_discovery_version), F32(drm_amdgpu_info_hw_ip, userq_num_slots),
	{ NULL }
};

static const struct field vramgtt[] = {
	F64(drm_amdgpu_info_vram_gtt, vram_size), F64(drm_amdgpu_info_vram_gtt, vram_cpu_accessible_size),
	F64(drm_amdgpu_info_vram_gtt, gtt_size),
	{ NULL }
};

#define HEAP(p)	FN(drm_amdgpu_memory_info, p, total_heap_size, U64), FN(drm_amdgpu_memory_info, p, usable_heap_size, U64), \
		FN(drm_amdgpu_memory_info, p, heap_usage, U64), FN(drm_amdgpu_memory_info, p, max_allocation, U64)
static const struct field memory[] = {
	HEAP(vram), HEAP(cpu_accessible_vram), HEAP(gtt),
	{ NULL }
};

static const struct field firmware[] = {
	F32(drm_amdgpu_info_firmware, ver), F32(drm_amdgpu_info_firmware, feature),
	{ NULL }
};

static const struct field vmfault[] = {
	F64(drm_amdgpu_info_gpuvm_fault, addr), F32(drm_amdgpu_info_gpuvm_fault, status),
	F32(drm_amdgpu_info_gpuvm_fault, vmhub),
	{ NULL }
};

static void
pfields(struct rq *q, const struct field *t, const void *s)
{
	const char *b = s;
	int i;

	for (; t->name; t++) {
		rprint(q, "%s", t->name);
		switch (t->type) {
		case U32:
			rprint(q, " %u", *(const u32 *)(b + t->off));
			break;
		case U64:
			rprint(q, " %llu", *(const u64 *)(b + t->off));
			break;
		case A32:
			for (i = 0; i < t->count; i++)
				rprint(q, " %u", ((const u32 *)(b + t->off))[i]);
			break;
		}
		rprint(q, "\n");
	}
}

/* video caps: codec k's fields as "k.name value" */
static void
pvideo(struct rq *q, const struct drm_amdgpu_info_video_caps *v)
{
	int k;

	for (k = 0; k < AMDGPU_INFO_VIDEO_CAPS_CODEC_IDX_COUNT; k++) {
		const struct drm_amdgpu_info_video_codec_info *ci = &v->codec_info[k];

		rprint(q, "%d.valid %u\n%d.max_width %u\n%d.max_height %u\n%d.max_pixels_per_frame %u\n%d.max_level %u\n",
			k, ci->valid, k, ci->max_width, k, ci->max_height, k, ci->max_pixels_per_frame, k, ci->max_level);
	}
}

enum { Kscalar32, Kscalar64, Kfields, Kvideo, Karray32 };
static const struct {
	const char	*name;
	u32	query;
	int	nargs;		/* numbers after the name */
	int	kind;
	size_t	size;		/* the result */
	const struct field	*fields;
} infos[] = {
	{ "accel", AMDGPU_INFO_ACCEL_WORKING, 0, Kscalar32, 4 },
	{ "device", AMDGPU_INFO_DEV_INFO, 0, Kfields, sizeof(struct drm_amdgpu_info_device), devinfo },
	{ "hwip", AMDGPU_INFO_HW_IP_INFO, 2, Kfields, sizeof(struct drm_amdgpu_info_hw_ip), hwip },
	{ "hwipcount", AMDGPU_INFO_HW_IP_COUNT, 1, Kscalar32, 4 },
	{ "fw", AMDGPU_INFO_FW_VERSION, 3, Kfields, sizeof(struct drm_amdgpu_info_firmware), firmware },
	{ "vramgtt", AMDGPU_INFO_VRAM_GTT, 0, Kfields, sizeof(struct drm_amdgpu_info_vram_gtt), vramgtt },
	{ "memory", AMDGPU_INFO_MEMORY, 0, Kfields, sizeof(struct drm_amdgpu_memory_info), memory },
	{ "timestamp", AMDGPU_INFO_TIMESTAMP, 0, Kscalar64, 8 },
	{ "moved", AMDGPU_INFO_NUM_BYTES_MOVED, 0, Kscalar64, 8 },
	{ "evictions", AMDGPU_INFO_NUM_EVICTIONS, 0, Kscalar64, 8 },
	{ "vramfaults", AMDGPU_INFO_NUM_VRAM_CPU_PAGE_FAULTS, 0, Kscalar64, 8 },
	{ "vramusage", AMDGPU_INFO_VRAM_USAGE, 0, Kscalar64, 8 },
	{ "visvramusage", AMDGPU_INFO_VIS_VRAM_USAGE, 0, Kscalar64, 8 },
	{ "gttusage", AMDGPU_INFO_GTT_USAGE, 0, Kscalar64, 8 },
	{ "vmfault", AMDGPU_INFO_GPUVM_FAULT, 0, Kfields, sizeof(struct drm_amdgpu_info_gpuvm_fault), vmfault },
	{ "video", AMDGPU_INFO_VIDEO_CAPS, 1, Kvideo, sizeof(struct drm_amdgpu_info_video_caps) },
	{ "sensor", AMDGPU_INFO_SENSOR, 1, Kscalar32, 4 },
	{ "maxibs", AMDGPU_INFO_MAX_IBS, 0, Karray32, AMDGPU_HW_IP_NUM * 4 },
	{ "mmr", AMDGPU_INFO_READ_MMR_REG, 4, Karray32, 0 },	/* off count instance flags */
};

static long
info(void *c, struct rq *q, char **f, int n, int priv)
{
	struct drm_amdgpu_info *in;
	u32 a[4];
	void *res;
	size_t size;
	int i, k;
	long r;

	if (n < 2)
		return -EINVAL;
	for (k = 0; k < (int)ARRAY_SIZE(infos); k++)
		if (strcmp(infos[k].name, f[1]) == 0)
			break;
	if (k == (int)ARRAY_SIZE(infos) || n != 2 + infos[k].nargs)
		return -EINVAL;
	for (i = 0; i < infos[k].nargs; i++)
		NUM32(f[2+i], a[i]);
	in = rqalloc(q, sizeof *in);
	in->query = infos[k].query;
	size = infos[k].size;
	switch (in->query) {
	case AMDGPU_INFO_HW_IP_INFO:
		in->query_hw_ip.type = a[0];
		in->query_hw_ip.ip_instance = a[1];
		break;
	case AMDGPU_INFO_HW_IP_COUNT:
		in->query_hw_ip.type = a[0];
		break;
	case AMDGPU_INFO_FW_VERSION:
		in->query_fw.fw_type = a[0];
		in->query_fw.ip_instance = a[1];
		in->query_fw.index = a[2];
		break;
	case AMDGPU_INFO_VIDEO_CAPS:
		in->video_cap.type = a[0];
		break;
	case AMDGPU_INFO_SENSOR:
		in->sensor_info.type = a[0];
		break;
	case AMDGPU_INFO_READ_MMR_REG:
		if (a[1] == 0 || a[1] > 256)
			return -EINVAL;
		in->read_mmr_reg.dword_offset = a[0];
		in->read_mmr_reg.count = a[1];
		in->read_mmr_reg.instance = a[2];
		in->read_mmr_reg.flags = a[3];
		size = a[1] * 4;
		break;
	}
	if (!(res = rqalloc(q, size)))
		return -ENOMEM;
	in->return_pointer = (u64)(uintptr_t)res;
	in->return_size = size;
	r = rqioctl(c, q, DRM_IOCTL_AMDGPU_INFO, in, priv);
	if (r)
		return r;
	switch (infos[k].kind) {
	case Kscalar32:
		rprint(q, "%u\n", *(u32 *)res);
		break;
	case Kscalar64:
		rprint(q, "%llu\n", *(u64 *)res);
		break;
	case Kfields:
		pfields(q, infos[k].fields, res);
		break;
	case Kvideo:
		pvideo(q, res);
		break;
	case Karray32:
		for (i = 0; i < (int)(size / 4); i++)
			rprint(q, "%s%u", i ? " " : "", ((u32 *)res)[i]);
		rprint(q, "\n");
		break;
	}
	return 0;
}

/* version: DRM's VERSION: "version major minor patch" and name, date, desc a line each */
static long
version(void *c, struct rq *q, char **f, int n, int priv)
{
	struct drm_version *v;
	long r;

	if (n != 1)
		return -EINVAL;
	v = rqalloc(q, sizeof *v);
	v->name_len = v->date_len = v->desc_len = 128;
	v->name = rqalloc(q, 129);
	v->date = rqalloc(q, 129);
	v->desc = rqalloc(q, 129);
	r = rqioctl(c, q, DRM_IOCTL_VERSION, v, priv);
	if (r == 0) {
		v->name[min_t(size_t, v->name_len, 128)] = 0;
		v->date[min_t(size_t, v->date_len, 128)] = 0;
		v->desc[min_t(size_t, v->desc_len, 128)] = 0;
		rprint(q, "version %d %d %d\nname %s\ndate %s\ndesc %s\n", v->version_major, v->version_minor,
			v->version_patchlevel, v->name, v->date, v->desc);
	}
	return r;
}

static const struct {
	const char	*verb;
	long	(*fn)(void *, struct rq *, char **, int, int);
} verbs[] = {
	{ "gem", gem }, { "geminfo", gemop }, { "place", gemop }, { "meta", meta },
	{ "va", va }, { "idle", idle },
	{ "hwctx", hwctx }, { "pstate", hwctx }, { "vmid", vmid },
	{ "waitcs", waitcs }, { "waitfences", waitfences }, { "fromfence", fromfence },
	{ "syncnew", syncreq }, { "syncfree", syncreq }, { "syncwait", syncreq }, { "synctwait", syncreq },
	{ "syncreset", syncreq }, { "syncsignal", syncreq }, { "synctsignal", syncreq },
	{ "syncquery", syncreq }, { "synctransfer", syncreq },
	{ "info", info }, { "cs", cs }, { "version", version },
};

/* is f[0] a verb of these */
int
amdgpu_isreq(const char *verb)
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(verbs); i++)
		if (strcmp(verbs[i].verb, verb) == 0)
			return 1;
	return 0;
}

/*
 * the request f[0..n-1]: its reply in reply (at most nreply bytes, NUL
 * terminated); its length, or -errno
 */
int
amdgpu_req(void *c, char **f, int n, char *reply, int nreply, int priv)
{
	struct rq q;
	long r;
	int i;

	if (n < 1 || nreply < 2)
		return -EINVAL;
	for (i = 0; i < (int)ARRAY_SIZE(verbs); i++)
		if (strcmp(verbs[i].verb, f[0]) == 0)
			break;
	if (i == (int)ARRAY_SIZE(verbs))
		return -EINVAL;
	memset(&q, 0, sizeof q);
	q.max = Scratch;
	q.base = kvzalloc(q.max, GFP_KERNEL);
	if (!q.base)
		return -ENOMEM;
	q.reply = reply;
	q.nreply = nreply;
	reply[0] = 0;
	r = verbs[i].fn(c, &q, f, n, priv);
	kvfree(q.base);
	if (r < 0)
		return (int)r;
	return q.len;
}
