/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: the file server, gpu(4)'s files (as aux/intelgpu serves
 * them, so that libdrm9 and Mesa need no change): status, clone, the
 * outputs, and the contexts' ctl, submit and fence.  The work is the Linux side's
 * (linux/ctx.c); this side parses the requests and keeps the files.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915.h"
#include "../kmsout.h"

enum {
	Xctl = 1,
	Xstatus,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,
	Xerror,
	Xoutedid,
	Xoutmodes,
	Xoutmode,
	Xoutbacklight,
	Nctx	= 256,
	Nout	= 16,
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

static char *outname[Nout];
static int nout;
static QLock ctxlock;
static Ctx *ctxs[Nctx];
static File *ctxdir;
static Srv fs;

/* a file's kind and context: aux holds the kind, ctx files the context too */
#define KIND(f)		((uintptr)(f)->aux & 0xFF)
#define CTXN(f)		((uintptr)(f)->aux >> 8)

/* an output's mode or backlight (kmsscreen.c); the server's other requests go on */
static char*
outreq(Req *r, char *out, char *spec, int bl, char **s)
{
	char *e;

	srvrelease(r->srv);
	e = outrequest(out, spec, bl, s);
	srvacquire(r->srv);
	return e;
}

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

	lc = i915_ctx_new(&id);
	if(i915_iserr(lc)){
		werrstr("%s", errstr9((int)(long)lc));
		return nil;
	}
	if(id >= Nctx || ctxs[id] != nil){
		i915_ctx_free(lc);
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
	i915_ctx_free(c->lc);
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
	case Xerror:
		{
			long m;

			/* the text is large: as much as asked for, where asked */
			m = i915_error_read(r->ofcall.data, r->ifcall.count, r->ifcall.offset);
			if(m < 0){
				respond(r, errstr9((int)m));
				return;
			}
			r->ofcall.count = m;
			respond(r, nil);
			return;
		}
	case Xoutedid:
		{
			uchar buf[4096];
			int m;

			m = kmsout_edid(outname[CTXN(r->fid->file)], buf, sizeof buf);
			readbuf(r, buf, m < 0 ? 0 : m);
			respond(r, nil);
			return;
		}
	case Xoutmodes:
	case Xoutmode:
		t = KIND(r->fid->file) == Xoutmodes ? kmsout_modes(outname[CTXN(r->fid->file)])
			: kmsout_mode(outname[CTXN(r->fid->file)]);
		s = estrdup9p(t != nil ? t : "");
		kmsout_free(t);
		break;
	case Xoutbacklight:
		t = nil;
		outreq(r, outname[CTXN(r->fid->file)], nil, 1, &t);
		s = estrdup9p(t != nil ? t : "");
		kmsout_free(t);
		break;
	case Xctl:
		t = kmsout_plane();
		s = estrdup9p(t != nil ? t : "plane none\n");
		kmsout_free(t);
		break;
	case Xstatus:
		{
			char *o;

			t = i915_status();
			o = kmsout_status();
			s = smprint("%s%s", t != nil ? t : "", o != nil ? o : "");
			i915_freestr(t);
			kmsout_free(o);
		}
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
			i915_fence_count(c->lc, &done, &n, &failed);
			s = smprint("%llud %llud%s\n", done, n, failed ? " failed" : "");
			break;
		}
		/* a wait blocks: other requests go on meanwhile */
		srvrelease(r->srv);
		d = i915_fence_wait(c->lc, fa->fence, fa->query);
		srvacquire(r->srv);
		if(d < 0){
			respond(r, "no such fence");
			return;
		}
		i915_fence_count(c->lc, &done, &n, &failed);
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
		e = i915_bo_new(c->lc, strtoull(f[1], nil, 0), &gpu, &id, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		setreply(c, "bo %d %s %#llux %#llux %#llux\n", id, seg, va, size, gpu);
		return nil;
	}
	if(n == 3 && strcmp(f[0], "bind") == 0){
		e = i915_bo_bind(c->lc, atoi(f[1]), strtoull(f[2], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 2 && strcmp(f[0], "free") == 0){
		e = i915_bo_free(c->lc, atoi(f[1]));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 2 && strcmp(f[0], "getparam") == 0){
		int v;

		e = i915_getparam(c->lc, atoi(f[1]), &v);
		if(e < 0)
			return errstr9(e);
		setreply(c, "getparam %d %d\n", atoi(f[1]), v);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "map") == 0 && strcmp(f[1], "plane") == 0){
		uint w, h, stride, bpp;

		e = i915_map_plane(c->lc, &id, &gpu, &w, &h, &stride, &bpp);
		if(e < 0)
			return errstr9(e);
		setreply(c, "plane %d %#llux %ud %ud %ud %ud\n", id, gpu, stride, bpp, w, h);
		return nil;
	}
	return "unknown ctl message";
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e, *t;
	Fidaux *fa;
	u64g k;
	int n, x;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	fa = r->fid->aux;
	e = nil;
	switch(KIND(r->fid->file)){
	case Xoutmode:
	case Xoutbacklight:
		if((t = strchr(buf, '\n')) != nil)
			*t = 0;
		e = outreq(r, outname[CTXN(r->fid->file)], buf, KIND(r->fid->file) == Xoutbacklight, nil);
		goto done;
	}
	n = tokenize(buf, f, nelem(f));
	switch(KIND(r->fid->file)){
	case Xerror:
		i915_error_clear();
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		if((n == 2 || n == 5) && strcmp(f[0], "scanout") == 0){
			/* returns when the display shows it: other requests go on */
			srvrelease(r->srv);
			x = i915_scanout(fa->ctx->lc, atoi(f[1]), n == 5 ? atoi(f[2]) : 0,
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
		if(n != 2 && n != 3){
			respond(r, "usage: bo offset [rcs|bcs]");
			return;
		}
		x = i915_submit(fa->ctx->lc, atoi(f[0]), strtoul(f[1], nil, 0),
			n == 3 && strcmp(f[2], "bcs") == 0, &k);
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
done:
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
	createfile(n, "error", user, 0664, (void*)Xerror);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
	nout = kmsout_names(outname, Nout);
	o = createfile(n, "outputs", user, DMDIR|0555, nil);
	for(i = 0; i < nout; i++){
		d = createfile(o, outname[i], user, DMDIR|0555, nil);
		createfile(d, "edid", user, 0444, (void*)(uintptr)(i<<8 | Xoutedid));
		createfile(d, "modes", user, 0444, (void*)(uintptr)(i<<8 | Xoutmodes));
		createfile(d, "mode", user, 0664, (void*)(uintptr)(i<<8 | Xoutmode));
		if(kmsout_hasbacklight(outname[i]))
			createfile(d, "backlight", user, 0664, (void*)(uintptr)(i<<8 | Xoutbacklight));
	}
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
