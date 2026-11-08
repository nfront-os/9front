/*
 * SPDX-License-Identifier: MIT
 *
 * gpu(4)'s file server, shared by aux/i915, aux/amdgpu and
 * aux/virtiogpu (each driver's file server includes this file and
 * fills in drv, its Gpudrv, after it):
 *
 *	/dev/gpu/0/ctl		the driver's (drv.ctlread, drv.ctlwrite)
 *	/dev/gpu/0/status	drv.status
 *	/dev/gpu/0/outputs/conn/{edid,modes,mode,backlight}	drv.out*
 *	/dev/gpu/0/clone	a new context: open gives its ctl
 *	/dev/gpu/0/ctx/n/ctl	the context's words (drv.ctl), each
 *				request's reply the fid's own, read after it
 *	/dev/gpu/0/ctx/n/submit	drv.submit
 *	/dev/gpu/0/ctx/n/fence	"done n" read; write k (or wait k,
 *				query k): the next read waits for fence k
 *				(drv.fencewait), "k done|busy"
 *
 * and the driver's own files (drv.tree, kinds Xdrv and up, drv.read,
 * drv.write).  A context is its creator's (its files 0600); it goes
 * when its last file is closed (drv.ctxfree).  Plain Plan 9 C, no
 * libthread: aux/virtiogpu serves with postmountsrv.
 */

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
	Xdrv = 0x20,	/* the driver's own files: Xdrv and up */
	Nctx	= 256,
	Nout	= 16,
};

/* a file's kind and number: aux holds the kind, a context's or output's files the number too */
#define KIND(f)		((uintptr)(f)->aux & 0xFF)
#define CTXN(f)		((uintptr)(f)->aux >> 8)

typedef struct Gctx Gctx;
typedef struct Gfid Gfid;
typedef struct Gpudrv Gpudrv;

struct Gctx {
	int	id;
	void	*lc;		/* the driver's */
	int	ref;		/* open files */
	File	*dir, *ctl, *submit, *fence;
	char	*reply;		/* the clone's: "ctx n ...", each ctl fid's first */
	char	*uid;		/* its creator's: its files, its segments */
};

struct Gfid {
	Gctx	*ctx;
	char	*reply;		/* the reply to the last request on this fid (ctl, submit) */
	vlong	rend;		/* where the request ended: a read from there is the reply's start */
	int	rbase;		/* rend decided by the first read: -1 not yet, else 0 or 1 */
	uvlong	fence;		/* written to fence: wait for it */
	int	query;
};

/* what each driver does; nil: not there (a file not served, a request refused) */
struct Gpudrv {
	/* a context for uid: the driver's, its number (< Nctx), its first reply; nil: werrstr */
	void*	(*ctxnew)(char *uid, int *id, char **reply);
	void	(*ctxfree)(void *lc);
	/* the context's words (ctl), and submit; the reply with fsetreply; nil or the error */
	char*	(*ctl)(Gfid*, char **f, int n);
	char*	(*submit)(Gfid*, char **f, int n);
	void	(*fencecount)(void *lc, uvlong *done, uvlong *n, int *failed);
	int	(*fencewait)(void *lc, uvlong k, int query);	/* 1 done, 0 busy, < 0 no such fence */
	char*	(*status)(void);		/* malloc'd */
	char*	(*ctlread)(void);		/* malloc'd */
	char*	(*ctlwrite)(Req*, char **f, int n);
	int	(*outputs)(char **names, int max);
	int	(*hasbacklight)(char *name);
	char*	(*outread)(Req*, char *name, int kind);	/* fills r->ofcall; nil or the error */
	char*	(*outwrite)(Req*, char *name, int kind, char *s);
	void	(*tree)(File *dir, char *user);	/* its own files in /dev/gpu/0 */
	int	(*read)(Req*);		/* 1: done (responded) */
	int	(*write)(Req*);		/* 1: done (responded): its own files, a request it takes as it is */
};

static Gpudrv drv;	/* the driver's, defined after this file */
static char *outname[Nout];
static int nout;
static QLock ctxlock;
static Gctx *ctxs[Nctx];
static File *ctxdir;
static Srv fs;

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
fsetreply(Gfid *fa, char *fmt, ...)
{
	va_list arg;

	free(fa->reply);
	va_start(arg, fmt);
	fa->reply = vsmprint(fmt, arg);
	va_end(arg);
}

/* the request on fa written, its reply set: a read from where it ended is the reply's start */
static void
fsreplied(Gfid *fa, Req *r)
{
	fa->rend = r->ifcall.offset + r->ifcall.count;
	fa->rbase = -1;
}

static Gctx*
newctx(char *uid)
{
	char name[16], *reply;
	void *lc;
	Gctx *c;
	int id;

	reply = nil;
	if((lc = drv.ctxnew(uid, &id, &reply)) == nil)
		return nil;
	if(id < 0 || id >= Nctx || ctxs[id] != nil){
		drv.ctxfree(lc);
		free(reply);
		werrstr("too many contexts");
		return nil;
	}
	c = emalloc9p(sizeof *c);
	memset(c, 0, sizeof *c);
	c->id = id;
	c->lc = lc;
	c->uid = estrdup9p(uid);
	c->reply = reply != nil ? reply : smprint("ctx %d\n", id);
	/* its creator's alone */
	snprint(name, sizeof name, "%d", id);
	c->dir = createfile(ctxdir, name, uid, DMDIR|0500, nil);
	c->ctl = createfile(c->dir, "ctl", uid, 0600, (void*)(uintptr)(id<<8 | Xctxctl));
	c->submit = createfile(c->dir, "submit", uid, 0600, (void*)(uintptr)(id<<8 | Xctxsubmit));
	c->fence = createfile(c->dir, "fence", uid, 0600, (void*)(uintptr)(id<<8 | Xctxfence));
	ctxs[id] = c;
	return c;
}

static void
freectx(Gctx *c)
{
	ctxs[c->id] = nil;
	drv.ctxfree(c->lc);
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
	Gfid *fa;
	Gctx *c;

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
	Gfid *fa;
	Gctx *c;

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
	uvlong done, n;
	Gfid *fa;
	Gctx *c;
	char *s, *e;
	int failed, d;

	if(drv.read != nil && drv.read(r))
		return;
	fa = r->fid->aux;
	switch(KIND(r->fid->file)){
	case Xoutedid:
	case Xoutmodes:
	case Xoutmode:
	case Xoutbacklight:
		e = drv.outread(r, outname[CTXN(r->fid->file)], KIND(r->fid->file));
		respond(r, e);
		return;
	case Xctl:
		s = drv.ctlread != nil ? drv.ctlread() : nil;
		break;
	case Xstatus:
		s = drv.status();
		break;
	case Xctxctl:
	case Xctxsubmit:
		/*
		 * the reply from offset 0 (pread) or from where the request
		 * ended (rc: a read after the write): the first read decides
		 */
		if(fa->rbase < 0)
			fa->rbase = fa->rend > 0 && r->ifcall.offset == fa->rend;
		if(fa->rbase > 0 && r->ifcall.offset >= fa->rend)
			r->ifcall.offset -= fa->rend;
		s = estrdup9p(fa->reply != nil ? fa->reply : "");
		break;
	case Xctxfence:
		c = fa->ctx;
		r->ifcall.offset = 0;
		if(fa->fence == 0){
			drv.fencecount(c->lc, &done, &n, &failed);
			s = smprint("%llud %llud%s\n", done, n, failed ? " failed" : "");
			break;
		}
		/* a wait blocks: other requests go on meanwhile */
		srvrelease(r->srv);
		d = drv.fencewait(c->lc, fa->fence, fa->query);
		srvacquire(r->srv);
		if(d < 0){
			respond(r, "no such fence");
			return;
		}
		drv.fencecount(c->lc, &done, &n, &failed);
		s = smprint("%llud %s%s\n", fa->fence, d ? "done" : "busy", failed ? " failed" : "");
		break;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s != nil ? s : "");
	free(s);
	respond(r, nil);
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e, *t;
	Gfid *fa;
	int n;

	if(drv.write != nil && drv.write(r))
		return;
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
		e = drv.outwrite != nil ? drv.outwrite(r, outname[CTXN(r->fid->file)], KIND(r->fid->file), buf)
			: "permission denied";
		goto done;
	}
	n = tokenize(buf, f, nelem(f));
	switch(KIND(r->fid->file)){
	case Xctl:
		e = drv.ctlwrite != nil ? drv.ctlwrite(r, f, n) : "unknown ctl message";
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		/* a request's reply is its own: none unless the request has one */
		free(fa->reply);
		fa->reply = nil;
		qlock(&ctxlock);
		e = drv.ctl(fa, f, n);
		qunlock(&ctxlock);
		if(e == nil)
			fsreplied(fa, r);
		break;
	case Xctxsubmit:
		free(fa->reply);
		fa->reply = nil;
		e = drv.submit(fa, f, n);
		if(e == nil)
			fsreplied(fa, r);
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

/* gpu(4)'s tree; clone and the contexts if ctxs */
static void
mktree(int withctxs)
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
	if(withctxs){
		createfile(n, "clone", user, 0666, (void*)Xclone);
		ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
	}
	nout = drv.outputs != nil ? drv.outputs(outname, Nout) : 0;
	o = createfile(n, "outputs", user, DMDIR|0555, nil);
	for(i = 0; i < nout; i++){
		d = createfile(o, outname[i], user, DMDIR|0555, nil);
		createfile(d, "edid", user, 0444, (void*)(uintptr)(i<<8 | Xoutedid));
		createfile(d, "modes", user, 0444, (void*)(uintptr)(i<<8 | Xoutmodes));
		createfile(d, "mode", user, 0664, (void*)(uintptr)(i<<8 | Xoutmode));
		if(drv.hasbacklight != nil && drv.hasbacklight(outname[i]))
			createfile(d, "backlight", user, 0664, (void*)(uintptr)(i<<8 | Xoutbacklight));
	}
	if(drv.tree != nil)
		drv.tree(n, user);
}

/* the Srv's functions; the caller adds attach and posts it */
static void
gpufsinit(int withctxs)
{
	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	mktree(withctxs);
}
