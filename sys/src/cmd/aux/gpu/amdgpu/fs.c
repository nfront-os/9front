/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: the file server, gpu(4)'s files as aux/i915 serves them
 * (its fs.c, with amdgpu's requests): ctl, status, clone, the contexts'
 * ctl, submit and fence, the outputs and scanout.  The work is the Linux
 * side's (linux/ctx.c, linux/kms.c); this side parses the requests and
 * keeps the files.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "amdgpu.h"

enum {
	Xctl = 1,
	Xstatus,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,
	Xctxioctl,
	Xoutedid,
	Xoutmodes,
	Xoutmode,
	Nctx	= 256,
	Nout	= 16,
};

typedef struct Ctx Ctx;
typedef struct Fidaux Fidaux;

struct Ctx {
	int	id;
	void	*lc;		/* the Linux side's */
	int	ref;		/* open files */
	File	*dir, *ctl, *submit, *fence, *ioctl;
	char	*reply;
};

struct Fidaux {
	Ctx	*ctx;
	long	ioctlr;		/* the ioctl's result, for the read */
	int	ioctlset;
	u64g	fence;		/* written to fence: wait for it */
	int	query;
};

static char *outname[Nout];
static int nout;
static QLock ctxlock;
static Ctx *ctxs[Nctx];
static File *ctxdir;
static Srv fs;

/*
 * the display's requests go to DC, whose bandwidth calculation needs more
 * stack than lib9p's procs have (32 KB): a proc of its own does them
 */
typedef struct Kreq Kreq;
struct Kreq {
	void	*lc;
	int	free;	/* amdgpu_ctx_free: its picture off the display */
	int	gem;	/* id is a GEM handle of the client's (scanoutgem) */
	int	id, w, h, stride;
	int	r;
	Channel	*done;
};
static Channel *kmsc;

static void
kmsproc(void*)
{
	Kreq *k;

	for(;;){
		k = recvp(kmsc);
		if(k->free)
			amdgpu_ctx_free(k->lc);
		else if(k->gem)
			k->r = amdgpu_scanout_gem(k->lc, k->id, k->w, k->h, k->stride);
		else
			k->r = amdgpu_scanout(k->lc, k->id, k->w, k->h, k->stride);
		sendul(k->done, 1);
	}
}

static void
ctxfree(void *lc)
{
	Kreq k;

	memset(&k, 0, sizeof k);
	k.lc = lc;
	k.free = 1;
	k.done = chancreate(sizeof(ulong), 0);
	sendp(kmsc, &k);
	recvul(k.done);
	chanfree(k.done);
}

/*
 * the clients' ioctls: a pool of procs with big stacks (amdgpu's own
 * code under them), so that one that waits (a syncobj) holds up no other
 */
enum { Nioctlproc = 4 };
typedef struct Ireq Ireq;
struct Ireq {
	void	*lc;
	uint	cmd;
	u64g	addr;
	long	r;
	Channel	*done;
};
static Channel *ioctlc;

static void
ioctlproc(void*)
{
	Ireq *q;

	for(;;){
		q = recvp(ioctlc);
		q->r = amdgpu_ioctl(q->lc, q->cmd, q->addr);
		sendul(q->done, 1);
	}
}

static long
doioctl(void *lc, uint cmd, u64g addr)
{
	Ireq q;

	q.lc = lc;
	q.cmd = cmd;
	q.addr = addr;
	q.done = chancreate(sizeof(ulong), 0);
	sendp(ioctlc, &q);
	recvul(q.done);
	chanfree(q.done);
	return q.r;
}

static int
scanout(void *lc, int gem, int id, int w, int h, int stride)
{
	Kreq k;

	k.lc = lc;
	k.free = 0;
	k.gem = gem;
	k.id = id;
	k.w = w;
	k.h = h;
	k.stride = stride;
	k.done = chancreate(sizeof(ulong), 0);
	sendp(kmsc, &k);
	recvul(k.done);
	chanfree(k.done);
	return k.r;
}

/* a file's kind and context: aux holds the kind, ctx files the context too */
#define KIND(f)		((uintptr)(f)->aux & 0xFF)
#define CTXN(f)		((uintptr)(f)->aux >> 8)

static char*
errstr9(int e)
{
	static char buf[64];

	switch(-e){
	case 2:	return "no such object";
	case 12:	return "out of memory";
	case 14:	return "bad address";
	case 16:	return "too many fences outstanding";
	case 19:	return "no device";
	case 22:	return "bad request";
	case 28:	return "no room in visible VRAM";
	case 98:	return "GPU address in use or out of range";
	}
	snprint(buf, sizeof buf, "error %d", -e);
	return buf;
}

static void
setreply(Ctx *c, char *fmt, ...)
{
	va_list arg;

	free(c->reply);
	va_start(arg, fmt);
	c->reply = vsmprint(fmt, arg);
	va_end(arg);
}

static Ctx*
newctx(void)
{
	char name[16];
	char *user;
	void *lc;
	Ctx *c;
	int id;

	lc = amdgpu_ctx_new(&id);
	if(amdgpu_iserr(lc)){
		werrstr("%s", errstr9((int)(long)lc));
		return nil;
	}
	if(id >= Nctx || ctxs[id] != nil){
		amdgpu_ctx_free(lc);
		werrstr("too many contexts");
		return nil;
	}
	c = emalloc9p(sizeof *c);
	memset(c, 0, sizeof *c);
	c->id = id;
	c->lc = lc;
	user = getuser();
	snprint(name, sizeof name, "%d", id);
	c->dir = createfile(ctxdir, name, user, DMDIR|0555, nil);
	c->ctl = createfile(c->dir, "ctl", user, 0664, (void*)(uintptr)(id<<8 | Xctxctl));
	c->submit = createfile(c->dir, "submit", user, 0222, (void*)(uintptr)(id<<8 | Xctxsubmit));
	c->fence = createfile(c->dir, "fence", user, 0664, (void*)(uintptr)(id<<8 | Xctxfence));
	c->ioctl = createfile(c->dir, "ioctl", user, 0664, (void*)(uintptr)(id<<8 | Xctxioctl));
	ctxs[id] = c;
	setreply(c, "ctx %d\n", id);
	return c;
}

static void
freectx(Ctx *c)
{
	ctxs[c->id] = nil;
	ctxfree(c->lc);
	removefile(c->ioctl);
	removefile(c->fence);
	removefile(c->submit);
	removefile(c->ctl);
	removefile(c->dir);
	closefile(c->ioctl);
	closefile(c->fence);
	closefile(c->submit);
	closefile(c->ctl);
	closefile(c->dir);
	free(c->reply);
	free(c);
}

static void
fsopen(Req *r)
{
	Fidaux *fa;
	Ctx *c;

	switch(KIND(r->fid->file)){
	case Xclone:
		qlock(&ctxlock);
		if((c = newctx()) == nil){
			qunlock(&ctxlock);
			responderror(r);
			return;
		}
		closefile(r->fid->file);
		r->fid->file = c->ctl;
		incref(c->ctl);
		r->ofcall.qid = c->ctl->qid;
		c->ref++;
		qunlock(&ctxlock);
		break;
	case Xctxctl:
	case Xctxsubmit:
	case Xctxfence:
	case Xctxioctl:
		qlock(&ctxlock);
		if((c = ctxs[CTXN(r->fid->file)]) == nil){
			qunlock(&ctxlock);
			respond(r, "context gone");
			return;
		}
		c->ref++;
		qunlock(&ctxlock);
		break;
	default:
		respond(r, nil);
		return;
	}
	fa = emalloc9p(sizeof *fa);
	memset(fa, 0, sizeof *fa);
	fa->ctx = c;
	r->fid->aux = fa;
	respond(r, nil);
}

static void
fsdestroyfid(Fid *f)
{
	Fidaux *fa;
	Ctx *c;

	if((fa = f->aux) == nil)
		return;
	f->aux = nil;
	c = fa->ctx;
	free(fa);
	qlock(&ctxlock);
	if(--c->ref == 0)
		freectx(c);
	qunlock(&ctxlock);
}

static void
fsread(Req *r)
{
	u64g done, n;
	Fidaux *fa;
	Ctx *c;
	char *s, *t;
	int failed, d;

	fa = r->fid->aux;
	switch(KIND(r->fid->file)){
	case Xoutedid:
		{
			uchar buf[4096];
			int m;

			m = amdgpu_output_edid(outname[CTXN(r->fid->file)], buf, sizeof buf);
			readbuf(r, buf, m < 0 ? 0 : m);
			respond(r, nil);
			return;
		}
	case Xoutmodes:
	case Xoutmode:
		t = KIND(r->fid->file) == Xoutmodes ? amdgpu_output_modes(outname[CTXN(r->fid->file)])
			: amdgpu_output_mode(outname[CTXN(r->fid->file)]);
		s = estrdup9p(t != nil ? t : "");
		amdgpu_freestr(t);
		break;
	case Xctl:
		t = amdgpu_plane();
		s = estrdup9p(t != nil ? t : "plane none\n");
		amdgpu_freestr(t);
		break;
	case Xstatus:
		{
			char *o;

			t = amdgpu_status();
			o = amdgpu_outputs_status();
			s = smprint("%s%s", t != nil ? t : "", o != nil ? o : "");
			amdgpu_freestr(t);
			amdgpu_freestr(o);
		}
		break;
	case Xctxctl:
		c = fa->ctx;
		qlock(&ctxlock);
		s = estrdup9p(c->reply != nil ? c->reply : "");
		qunlock(&ctxlock);
		break;
	case Xctxioctl:
		/* the result of the last ioctl written on this fid */
		r->ifcall.offset = 0;
		if(!fa->ioctlset){
			respond(r, "no ioctl");
			return;
		}
		s = smprint("%ld\n", fa->ioctlr);
		break;
	case Xctxfence:
		c = fa->ctx;
		r->ifcall.offset = 0;
		if(fa->fence == 0){
			amdgpu_fence_count(c->lc, &done, &n, &failed);
			s = smprint("%llud %llud%s\n", done, n, failed ? " failed" : "");
			break;
		}
		/* a wait blocks: other requests go on meanwhile */
		srvrelease(r->srv);
		d = amdgpu_fence_wait(c->lc, fa->fence, fa->query);
		srvacquire(r->srv);
		if(d < 0){
			respond(r, "no such fence");
			return;
		}
		amdgpu_fence_count(c->lc, &done, &n, &failed);
		s = smprint("%llud %s%s\n", fa->fence, d ? "done" : "busy", failed ? " failed" : "");
		break;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s);
	free(s);
	respond(r, nil);
}

static char*
ctxctl(Ctx *c, char **f, int n)
{
	char seg[32];
	u64g va, size, gpu;
	int e, id;

	if(n >= 2 && n <= 4 && strcmp(f[0], "bo") == 0){
		u64g off, barsize;
		int vram, i;

		/* bo size [gpuaddr] [vram|gtt] */
		gpu = ~0ULL;
		vram = 0;
		for(i = 2; i < n; i++)
			if(strcmp(f[i], "vram") == 0)
				vram = 1;
			else if(strcmp(f[i], "gtt") == 0)
				vram = 0;
			else
				gpu = strtoull(f[i], nil, 0);
		e = amdgpu_bo_new(c->lc, strtoull(f[1], nil, 0), &gpu, vram, &id, seg, sizeof seg, &va, &size, &off, &barsize);
		if(e < 0)
			return errstr9(e);
		if(vram)
			setreply(c, "bo %d %s %#llux %#llux %#llux %#llux %#llux\n", id, seg, va, size, gpu, off, barsize);
		else
			setreply(c, "bo %d %s %#llux %#llux %#llux\n", id, seg, va, size, gpu);
		return nil;
	}
	if(n == 3 && strcmp(f[0], "bind") == 0){
		e = amdgpu_bo_bind(c->lc, atoi(f[1]), strtoull(f[2], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 2 && strcmp(f[0], "free") == 0){
		e = amdgpu_bo_free(c->lc, atoi(f[1]));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 1 && strcmp(f[0], "arena") == 0){
		e = amdgpu_arena(c->lc, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		setreply(c, "arena %s %#llux %#llux\n", seg, va, size);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "map") == 0){
		u64g off, barsize;

		e = amdgpu_map(c->lc, strtoul(f[1], nil, 0), seg, sizeof seg, &va, &size, &off, &barsize);
		if(e < 0)
			return errstr9(e);
		if(barsize)
			setreply(c, "map %s %s 0 %#llux %#llux %#llux\n", f[1], seg, size, off, barsize);
		else
			setreply(c, "map %s %s %#llux %#llux\n", f[1], seg, va, size);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "unmap") == 0){
		e = amdgpu_unmap(c->lc, strtoul(f[1], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	return "unknown ctl message";
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e;
	Fidaux *fa;
	u64g k;
	int n, x;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	fa = r->fid->aux;
	n = tokenize(buf, f, nelem(f));
	e = nil;
	switch(KIND(r->fid->file)){
	case Xctl:
		if(n == 2 && strcmp(f[0], "ih") == 0 && strcmp(f[1], "process") == 0){
			x = amdgpu_ih_kick();
			if(x < 0)
				e = errstr9(x);
		}else if(n == 1 && strcmp(f[0], "waves") == 0){
			x = amdgpu_waves();
			if(x < 0)
				e = errstr9(x);
		}else
			e = "unknown ctl message";
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		if((n == 2 || n == 5) && strcmp(f[0], "scanout") == 0 || n == 5 && strcmp(f[0], "scanoutgem") == 0){
			/* returns when the display shows it: other requests go on */
			srvrelease(r->srv);
			x = scanout(fa->ctx->lc, f[0][7] == 'g', strtoul(f[1], nil, 0), n == 5 ? atoi(f[2]) : 0,
				n == 5 ? atoi(f[3]) : 0, n == 5 ? atoi(f[4]) : 0);
			srvacquire(r->srv);
			if(x < 0)
				e = errstr9(x);
			break;
		}
		qlock(&ctxlock);
		e = ctxctl(fa->ctx, f, n);
		qunlock(&ctxlock);
		break;
	case Xctxsubmit:
		if(n != 3 && n != 4){
			respond(r, "usage: bo offset ndw [gfx|compute|sdma]");
			return;
		}
		x = 0;	/* gfx */
		if(n == 4){
			if(strcmp(f[3], "compute") == 0)
				x = 1;
			else if(strcmp(f[3], "sdma") == 0)
				x = 2;
			else if(strcmp(f[3], "gfx") != 0){
				respond(r, "usage: bo offset ndw [gfx|compute|sdma]");
				return;
			}
		}
		x = amdgpu_submit(fa->ctx->lc, atoi(f[0]), strtoul(f[1], nil, 0), strtoul(f[2], nil, 0), x, &k);
		if(x < 0)
			e = errstr9(x);
		break;
	case Xctxioctl:
		/* cmd addr: the ioctl, its arguments in the arena; blocks as it does */
		if(n != 2){
			respond(r, "usage: cmd addr");
			return;
		}
		srvrelease(r->srv);
		fa->ioctlr = doioctl(fa->ctx->lc, strtoul(f[0], nil, 0), strtoull(f[1], nil, 0));
		srvacquire(r->srv);
		fa->ioctlset = 1;
		break;
	case Xctxfence:
		if(n == 1){
			fa->query = 0;
			fa->fence = strtoull(f[0], nil, 0);
		}else if(n == 2 && (strcmp(f[0], "wait") == 0 || strcmp(f[0], "query") == 0)){
			fa->query = f[0][0] == 'q';
			fa->fence = strtoull(f[1], nil, 0);
		}else
			e = "usage: [wait|query] fence";
		break;
	default:
		e = "permission denied";
	}
	if(e != nil){
		respond(r, e);
		return;
	}
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

static void
mktree(void)
{
	File *gpu, *n, *o, *d;
	char *user;
	int i;

	user = getuser();
	fs.tree = alloctree(user, user, DMDIR|0555, nil);
	gpu = createfile(fs.tree->root, "gpu", user, DMDIR|0555, nil);
	n = createfile(gpu, "0", user, DMDIR|0555, nil);
	createfile(n, "ctl", user, 0664, (void*)Xctl);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	createfile(n, "clone", user, 0666, (void*)Xclone);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
	nout = amdgpu_outputs(outname, Nout);
	o = createfile(n, "outputs", user, DMDIR|0555, nil);
	for(i = 0; i < nout; i++){
		d = createfile(o, outname[i], user, DMDIR|0555, nil);
		createfile(d, "edid", user, 0444, (void*)(uintptr)(i<<8 | Xoutedid));
		createfile(d, "modes", user, 0444, (void*)(uintptr)(i<<8 | Xoutmodes));
		createfile(d, "mode", user, 0444, (void*)(uintptr)(i<<8 | Xoutmode));
	}
}

/* serve gpu(4) as srvname, mounted after /dev */
void
serve(char *srvname)
{
	int i;

	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	kmsc = chancreate(sizeof(void*), 0);
	proccreate(kmsproc, nil, 1024*1024);
	ioctlc = chancreate(sizeof(void*), 0);
	for(i = 0; i < Nioctlproc; i++)
		proccreate(ioctlproc, nil, 1024*1024);
	mktree();
	threadpostmountsrv(&fs, srvname, "/dev", MAFTER);
}
