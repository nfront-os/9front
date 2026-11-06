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
#include "../kmsout.h"

enum {
	Xctl = 1,
	Xstatus,
	Xclone,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,
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
	char	*uid;		/* its creator's: its files, its segments */
};

struct Fidaux {
	Ctx	*ctx;
	char	*reply;		/* the reply to the last request on this fid (ctl, submit) */
	vlong	rend;		/* where the request ended: a read from there is the reply's start */
	int	rbase;		/* rend decided by the first read: -1 not yet, else 0 or 1 */
	u64g	fence;		/* written to fence: wait for it */
	int	query;
};

static char *outname[Nout];
static int nout;
static QLock ctxlock;
static Ctx *ctxs[Nctx];
static File *ctxdir;
static Srv fs;
static char *srvname9;	/* serve's: /srv/srvname, /srv/srvname.user */

#include "../usersrv.c"

/*
 * the display's requests go to DC, whose bandwidth calculation needs more
 * stack than lib9p's procs have (32 KB): a proc of its own does them
 */
typedef struct Kreq Kreq;
struct Kreq {
	void	*lc;
	int	free;	/* amdgpu_ctx_free: its picture off the display */
	int	gem;	/* id is a GEM handle of the client's (scanoutgem) */
	int	overlay;	/* overlay: id a GEM handle of the client's at x y, 0 off */
	int	id, w, h, stride, x, y;
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
		else if(k->overlay)
			k->r = amdgpu_overlay(k->lc, k->id, k->x, k->y, k->w, k->h, k->stride);
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

/*
 * the clients' requests as text (linux/req.c): a pool of procs with big
 * stacks (amdgpu's own code under them), so that one that waits (a
 * syncobj) holds up no other
 */
enum { Nreqproc = 4 };
typedef struct Treq Treq;
struct Treq {
	void	*lc;
	char	**f;
	int	nf;
	char	*reply;
	int	nreply;
	int	priv;	/* the client is the hostowner */
	long	r;
	Channel	*done;
};
static Channel *reqc;

static void
reqproc(void*)
{
	Treq *q;

	for(;;){
		q = recvp(reqc);
		q->r = amdgpu_req(q->lc, q->f, q->nf, q->reply, q->nreply, q->priv);
		sendul(q->done, 1);
	}
}

/* a request as text on a request proc: the reply's length or -errno */
static long
doreq(void *lc, char **f, int nf, char *reply, int nreply, int priv)
{
	Treq q;

	memset(&q, 0, sizeof q);
	q.lc = lc;
	q.f = f;
	q.nf = nf;
	q.reply = reply;
	q.nreply = nreply;
	q.priv = priv;
	q.done = chancreate(sizeof(ulong), 0);
	sendp(reqc, &q);
	recvul(q.done);
	chanfree(q.done);
	return q.r;
}

/* a client's object on an overlay plane (id 0: off), on the display's proc */
static int
overlay(void *lc, int id, int x, int y, int w, int h, int stride)
{
	Kreq k;

	memset(&k, 0, sizeof k);
	k.lc = lc;
	k.overlay = 1;
	k.id = id;
	k.x = x;
	k.y = y;
	k.w = w;
	k.h = h;
	k.stride = stride;
	k.done = chancreate(sizeof(ulong), 0);
	sendp(kmsc, &k);
	recvul(k.done);
	chanfree(k.done);
	return k.r;
}

static int
scanout(void *lc, int gem, int id, int w, int h, int stride)
{
	Kreq k;

	memset(&k, 0, sizeof k);
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

/*
 * Linux's errors as text: libdrm9 (amdgpu9.c: errnoof) turns them back
 * into errno values, so each a client tells apart has its own
 */
static char*
errstr9(int e)
{
	static char buf[64];

	switch(-e){
	case 1:	return "operation not permitted";
	case 2:	return "no such object";
	case 3:	return "no such process";
	case 4:	return "interrupted";
	case 5:	return "i/o error";
	case 6:	return "no such device or address";
	case 7:	return "too big";
	case 9:	return "bad handle";
	case 11:	return "try again";
	case 12:	return "out of memory";
	case 13:	return "permission denied";
	case 14:	return "bad address";
	case 16:	return "busy";
	case 17:	return "exists";
	case 19:	return "no device";
	case 22:	return "bad request";
	case 24:	return "too many open files";
	case 25:	return "not an ioctl";
	case 28:	return "no space";
	case 34:	return "out of range";
	case 35:	return "deadlock";
	case 38:	return "not implemented";
	case 61:	return "no data";
	case 62:	return "timer expired";
	case 75:	return "overflow";
	case 95:	return "not supported";
	case 98:	return "GPU address in use or out of range";
	case 110:	return "timed out";
	case 125:	return "canceled";
	}
	snprint(buf, sizeof buf, "error %d", -e);
	return buf;
}

/* the reply to the request on fid fa */
static void
fsetreply(Fidaux *fa, char *fmt, ...)
{
	va_list arg;

	free(fa->reply);
	va_start(arg, fmt);
	fa->reply = vsmprint(fmt, arg);
	va_end(arg);
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
newctx(char *uid)
{
	char name[16];
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
	c->uid = estrdup9p(uid);
	/* its creator's alone */
	snprint(name, sizeof name, "%d", id);
	c->dir = createfile(ctxdir, name, uid, DMDIR|0500, nil);
	c->ctl = createfile(c->dir, "ctl", uid, 0600, (void*)(uintptr)(id<<8 | Xctxctl));
	c->submit = createfile(c->dir, "submit", uid, 0600, (void*)(uintptr)(id<<8 | Xctxsubmit));
	c->fence = createfile(c->dir, "fence", uid, 0600, (void*)(uintptr)(id<<8 | Xctxfence));
	ctxs[id] = c;
	setreply(c, "ctx %d\n", id);
	return c;
}

static void
freectx(Ctx *c)
{
	ctxs[c->id] = nil;
	ctxfree(c->lc);
	removefile(c->fence);
	removefile(c->submit);
	removefile(c->ctl);
	removefile(c->dir);
	closefile(c->fence);
	closefile(c->submit);
	closefile(c->ctl);
	closefile(c->dir);
	free(c->reply);
	free(c->uid);
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
		if((c = newctx(r->fid->uid)) == nil){
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
	if(KIND(r->fid->file) == Xctxctl && c->reply != nil)
		fa->reply = estrdup9p(c->reply);	/* the clone's: "ctx n" */
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
	free(fa->reply);
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
			char *o, *sc;

			t = amdgpu_status();
			o = kmsout_status();
			sc = screenstatus();
			s = smprint("%s%s%s", t != nil ? t : "", o != nil ? o : "", sc != nil ? sc : "");
			free(sc);
			amdgpu_freestr(t);
			amdgpu_freestr(o);
		}
		break;
	case Xctxctl:
	case Xctxsubmit:
		c = fa->ctx;
		/*
		 * the reply from offset 0 (pread) or from where the request
		 * ended (rc: a read after the write): the first read decides
		 */
		if(fa->rbase < 0)
			fa->rbase = fa->rend > 0 && r->ifcall.offset == fa->rend;
		if(fa->rbase > 0 && r->ifcall.offset >= fa->rend)
			r->ifcall.offset -= fa->rend;
		USED(c);
		s = estrdup9p(fa->reply != nil ? fa->reply : "");
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
ctxctl(Fidaux *fa, char **f, int n)
{
	Ctx *c = fa->ctx;
	char seg[32];
	u64g va, size, gpu;
	int e, id;

	if(n >= 2 && n <= 4 && strcmp(f[0], "bo") == 0){
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
		e = amdgpu_bo_new(c->lc, strtoull(f[1], nil, 0), &gpu, vram, &id, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		segown(seg, c->uid);
		fsetreply(fa, "bo %d %s %#llux %#llux %#llux\n", id, seg, va, size, gpu);
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
	if(n == 2 && strcmp(f[0], "map") == 0){
		e = amdgpu_map(c->lc, strtoul(f[1], nil, 0), seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		segown(seg, c->uid);
		fsetreply(fa, "map %s %s %#llux %#llux\n", f[1], seg, va, size);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "unmap") == 0){
		e = amdgpu_unmap(c->lc, strtoul(f[1], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 2 && strcmp(f[0], "close") == 0){
		e = amdgpu_gem_close(c->lc, strtoul(f[1], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n == 2 && strcmp(f[0], "export") == 0){
		char token[64];

		e = amdgpu_export(c->lc, strtoul(f[1], nil, 0), token, sizeof token);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "export %s\n", token);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "syncexport") == 0){
		char token[64];

		e = amdgpu_syncexport(c->lc, strtoul(f[1], nil, 0), token, sizeof token);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "syncexport %s\n", token);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "syncimport") == 0){
		uint h;

		e = amdgpu_syncimport(c->lc, f[1], &h);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "syncimport %ud\n", h);
		return nil;
	}
	/* sync files of the context (linux/ctx.c: amdgpu_syncfile) */
	if(n == 2 && strcmp(f[0], "syncfile") == 0){
		int id;

		e = amdgpu_syncfile(c->lc, strtoul(f[1], nil, 0), &id);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "syncfile %d\n", id);
		return nil;
	}
	if(n == 3 && strcmp(f[0], "syncmerge") == 0){
		int id;

		e = amdgpu_syncmerge(c->lc, atoi(f[1]), atoi(f[2]), &id);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "syncmerge %d\n", id);
		return nil;
	}
	if(n == 3 && strcmp(f[0], "syncin") == 0){
		e = amdgpu_syncin(c->lc, strtoul(f[1], nil, 0), atoi(f[2]));
		return e < 0 ? errstr9(e) : nil;
	}
	if(n >= 2 && strcmp(f[0], "syncclose") == 0){
		int i;

		for(i = 1; i < n; i++)
			amdgpu_syncclose(c->lc, atoi(f[i]));
		return nil;
	}
	if(n == 2 && strcmp(f[0], "import") == 0){
		uint h;

		e = amdgpu_import(c->lc, f[1], &h, &size);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "import %ud %llud\n", h, size);
		return nil;
	}
	return "unknown ctl message";
}

enum { Nreply = 64*1024 };

/* is the request a verb of linux/req.c's (the first word) */
static int
istext(Req *r)
{
	char w[32], *p, *e;
	int n;

	p = r->ifcall.data;
	e = p + r->ifcall.count;
	while(p < e && (*p == ' ' || *p == '\t'))
		p++;
	for(n = 0; p < e && n < sizeof w - 1 && *p != ' ' && *p != '\t' && *p != '\n'; n++)
		w[n] = *p++;
	w[n] = 0;
	return n > 0 && amdgpu_isreq(w);
}

/*
 * a request as text (linux/req.c), up to a message's size: run on an
 * request proc (it may wait), its reply kept for the fid's next read
 */
static void
fsreq(Req *r)
{
	char *buf, **f, *reply;
	Fidaux *fa;
	long x;
	int n, nf;

	fa = r->fid->aux;
	n = r->ifcall.count;
	buf = emalloc9p(n+1);
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	f = emalloc9p((n/2+2) * sizeof *f);
	nf = tokenize(buf, f, n/2+2);
	reply = emalloc9p(Nreply);
	srvrelease(r->srv);
	x = doreq(fa->ctx->lc, f, nf, reply, Nreply, iseve9p(r->fid->uid));
	srvacquire(r->srv);
	if(x >= 0){
		free(fa->reply);
		fa->reply = estrdup9p(reply);
		fa->rend = r->ifcall.offset + r->ifcall.count;
		fa->rbase = -1;
		r->ofcall.count = r->ifcall.count;
	}
	free(reply);
	free(f);
	free(buf);
	respond(r, x < 0 ? errstr9(x) : nil);
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e, *t;
	Fidaux *fa;
	u64g k;
	int n, x;

	switch(KIND(r->fid->file)){
	case Xctxctl:
	case Xctxsubmit:
		if(istext(r)){
			fsreq(r);
			return;
		}
	}
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
	case Xctl:
		if(n == 2 && strcmp(f[0], "ih") == 0 && strcmp(f[1], "process") == 0){
			x = amdgpu_ih_kick();
			if(x < 0)
				e = errstr9(x);
		}else if(n == 1 && strcmp(f[0], "waves") == 0){
			x = amdgpu_waves();
			if(x < 0)
				e = errstr9(x);
		}else if(n == 2 && strcmp(f[0], "user") == 0){
			/* /srv/srvname.user: gpu(4) for that user */
			if(!iseve9p(r->fid->uid))
				e = "permission denied: the hostowner's";
			else
				e = postuser(&fs, srvname9, f[1]);
		}else
			e = "unknown ctl message";
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		if((n == 2 && strcmp(f[1], "off") == 0 || n == 7) && strcmp(f[0], "overlay") == 0){
			/* overlay handle x y w h stride, overlay off: the display is the hostowner's */
			if(!iseve9p(r->fid->uid)){
				respond(r, "permission denied: the display is the hostowner's");
				return;
			}
			srvrelease(r->srv);
			x = overlay(fa->ctx->lc, n == 2 ? 0 : strtoul(f[1], nil, 0), n == 2 ? 0 : atoi(f[2]),
				n == 2 ? 0 : atoi(f[3]), n == 2 ? 0 : atoi(f[4]), n == 2 ? 0 : atoi(f[5]),
				n == 2 ? 0 : atoi(f[6]));
			srvacquire(r->srv);
			if(x < 0)
				e = errstr9(x);
			break;
		}
		if((n == 2 || n == 5) && strcmp(f[0], "scanout") == 0 || n == 5 && strcmp(f[0], "scanoutgem") == 0){
			/* the display is the hostowner's (the console's) */
			if(!iseve9p(r->fid->uid)){
				respond(r, "permission denied: the display is the hostowner's");
				return;
			}
			/* returns when the display shows it: other requests go on */
			srvrelease(r->srv);
			x = scanout(fa->ctx->lc, f[0][7] == 'g', strtoul(f[1], nil, 0), n == 5 ? atoi(f[2]) : 0,
				n == 5 ? atoi(f[3]) : 0, n == 5 ? atoi(f[4]) : 0);
			srvacquire(r->srv);
			if(x < 0)
				e = errstr9(x);
			break;
		}
		/* a request's reply is its own: none unless the request has one */
		free(fa->reply);
		fa->reply = nil;
		qlock(&ctxlock);
		e = ctxctl(fa, f, n);
		qunlock(&ctxlock);
		if(e == nil){
			fa->rend = r->ifcall.offset + r->ifcall.count;
			fa->rbase = -1;
		}
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
	int i;

	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	fs.attach = srvattach;
	fs.aux = estrdup9p(getuser());	/* /srv/srvname is the hostowner's */
	srvname9 = estrdup9p(srvname);
	kmsc = chancreate(sizeof(void*), 0);
	proccreate(kmsproc, nil, 1024*1024);
	reqc = chancreate(sizeof(void*), 0);
	for(i = 0; i < Nreqproc; i++)
		proccreate(reqproc, nil, 1024*1024);
	mktree();
	threadpostmountsrv(&fs, srvname, "/dev", MAFTER);
}
