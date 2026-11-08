/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: the file server, gpu(4)'s files: the files every driver
 * serves (../gpufs.c), the outputs and the display (../gpukms.c), and
 * amdgpu's requests of a context: its words, and Linux's ioctls as
 * text (linux/req.c).  The work is the Linux side's (linux/ctx.c,
 * linux/kms.c); this side parses the requests.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "amdgpu.h"
#include "../kmsout.h"
#include "../gpufs.c"
#include "../gpukms.c"

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

static void*
ctxnew(char *uid, int *id, char **reply)
{
	void *lc;

	USED(uid, reply);
	lc = amdgpu_ctx_new(id);
	if(amdgpu_iserr(lc)){
		werrstr("%s", errstr9((int)(long)lc));
		return nil;
	}
	return lc;
}

static char*
ctxctl(Gfid *fa, char **f, int n)
{
	Gctx *c = fa->ctx;
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

static char*
ctxsubmit(Gfid *fa, char **f, int n)
{
	u64g k;
	int x;

	if(n != 3 && n != 4)
		return "usage: bo offset ndw [gfx|compute|sdma]";
	x = 0;	/* gfx */
	if(n == 4){
		if(strcmp(f[3], "compute") == 0)
			x = 1;
		else if(strcmp(f[3], "sdma") == 0)
			x = 2;
		else if(strcmp(f[3], "gfx") != 0)
			return "usage: bo offset ndw [gfx|compute|sdma]";
	}
	x = amdgpu_submit(fa->ctx->lc, atoi(f[0]), strtoul(f[1], nil, 0), strtoul(f[2], nil, 0), x, &k);
	return x < 0 ? errstr9(x) : nil;
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
	Gfid *fa;
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
		fsreplied(fa, r);
		r->ofcall.count = r->ifcall.count;
	}
	free(reply);
	free(f);
	free(buf);
	respond(r, x < 0 ? errstr9(x) : nil);
}

/* readback handle: waits for the copy engine, other requests go on; 1 if it was the word */
static int
readback(Req *r)
{
	char buf[64], *f[3];
	u64g size;
	Gfid *fa;
	uint h;
	int n, x;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		return 0;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) != 2 || strcmp(f[0], "readback") != 0)
		return 0;
	fa = r->fid->aux;
	srvrelease(r->srv);
	x = amdgpu_readback(fa->ctx->lc, strtoul(f[1], nil, 0), &h, &size);
	srvacquire(r->srv);
	if(x < 0){
		respond(r, errstr9(x));
		return 1;
	}
	fsetreply(fa, "readback %ud %llud\n", h, size);
	fsreplied(fa, r);
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
	return 1;
}

/* a context's requests as text, the display's, readback: the others are gpufs' */
static int
fsdrvwrite(Req *r)
{
	switch(KIND(r->fid->file)){
	case Xctxctl:
		if(istext(r)){
			fsreq(r);
			return 1;
		}
		return kmsctlwrite(r) || readback(r);
	case Xctxsubmit:
		if(istext(r)){
			fsreq(r);
			return 1;
		}
	}
	return 0;
}

static void
fencecount(void *lc, uvlong *done, uvlong *n, int *failed)
{
	u64g d, m;

	amdgpu_fence_count(lc, &d, &m, failed);
	*done = d;
	*n = m;
}

static int
fencewait(void *lc, uvlong k, int query)
{
	return amdgpu_fence_wait(lc, k, query);
}

static char*
status(void)
{
	return kmsstatus(amdgpu_status(), amdgpu_freestr);
}

static char*
ctlwrite(Req *r, char **f, int n)
{
	char *e;
	int x;

	if(kmsuser(r, f, n, &e))
		return e;
	if(n == 2 && strcmp(f[0], "ih") == 0 && strcmp(f[1], "process") == 0){
		x = amdgpu_ih_kick();
		return x < 0 ? errstr9(x) : nil;
	}
	if(n == 1 && strcmp(f[0], "waves") == 0){
		x = amdgpu_waves();
		return x < 0 ? errstr9(x) : nil;
	}
	return "unknown ctl message";
}

static int
scanout(void *lc, int gem, int id, int w, int h, int stride)
{
	if(gem)
		return amdgpu_scanout_gem(lc, id, w, h, stride);
	return amdgpu_scanout(lc, id, w, h, stride);
}

static int
overlay(void *lc, int id, int x, int y, int w, int h, int stride)
{
	return amdgpu_overlay(lc, id, x, y, w, h, stride);
}

static Gpudrv drv = {
	ctxnew,
	kmsctxfree,
	ctxctl,
	ctxsubmit,
	fencecount,
	fencewait,
	status,
	kmsctlread,
	ctlwrite,
	kmsoutputs,
	kmsout_hasbacklight,
	kmsoutread,
	kmsoutwrite,
	nil,
	nil,
	fsdrvwrite,
};

static Kmsdrv kdrv = {
	scanout,
	overlay,
	amdgpu_ctx_free,
};

/* serve gpu(4) as srvname, mounted after /dev */
void
serve(char *srvname)
{
	int i;

	reqc = chancreate(sizeof(void*), 0);
	for(i = 0; i < Nreqproc; i++)
		proccreate(reqproc, nil, 1024*1024);
	kmsserve(srvname);
}
