/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: the file server, gpu(4)'s files as aux/i915 serves them
 * (its fs.c, with amdgpu's requests): status, clone, and the contexts'
 * ctl, submit and fence.  The work is the Linux side's (linux/ctx.c);
 * this side parses the requests and keeps the files.  The display
 * (outputs, scanout) follows.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "amdgpu.h"

enum {
	Xstatus = 1,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,
	Nctx	= 256,
};

typedef struct Ctx Ctx;
typedef struct Fidaux Fidaux;

struct Ctx {
	int	id;
	void	*lc;		/* the Linux side's */
	int	ref;		/* open files */
	File	*dir, *ctl, *submit, *fence;
	char	*reply;
};

struct Fidaux {
	Ctx	*ctx;
	u64g	fence;		/* written to fence: wait for it */
	int	query;
};

static QLock ctxlock;
static Ctx *ctxs[Nctx];
static File *ctxdir;
static Srv fs;

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
	case 16:	return "too many fences outstanding";
	case 19:	return "no device";
	case 22:	return "bad request";
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
	ctxs[id] = c;
	setreply(c, "ctx %d\n", id);
	return c;
}

static void
freectx(Ctx *c)
{
	ctxs[c->id] = nil;
	amdgpu_ctx_free(c->lc);
	removefile(c->fence);
	removefile(c->submit);
	removefile(c->ctl);
	removefile(c->dir);
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
	case Xstatus:
		t = amdgpu_status();
		s = estrdup9p(t != nil ? t : "");
		amdgpu_freestr(t);
		break;
	case Xctxctl:
		c = fa->ctx;
		qlock(&ctxlock);
		s = estrdup9p(c->reply != nil ? c->reply : "");
		qunlock(&ctxlock);
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

	if((n == 2 || n == 3) && strcmp(f[0], "bo") == 0){
		gpu = n == 3 ? strtoull(f[2], nil, 0) : ~0ULL;
		e = amdgpu_bo_new(c->lc, strtoull(f[1], nil, 0), &gpu, &id, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
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
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
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
	File *gpu, *n;
	char *user;

	user = getuser();
	fs.tree = alloctree(user, user, DMDIR|0555, nil);
	gpu = createfile(fs.tree->root, "gpu", user, DMDIR|0555, nil);
	n = createfile(gpu, "0", user, DMDIR|0555, nil);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	createfile(n, "clone", user, 0666, (void*)Xclone);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
}

/* serve gpu(4) as srvname, mounted after /dev */
void
serve(char *srvname)
{
	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	mktree();
	threadpostmountsrv(&fs, srvname, "/dev", MAFTER);
}
