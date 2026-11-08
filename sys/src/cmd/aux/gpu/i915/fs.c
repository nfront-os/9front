/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: the file server, gpu(4)'s files (as aux/intelgpu serves
 * them, so that libdrm9 and Mesa need no change): the files every
 * driver serves (../gpufs.c), the outputs and the display (../gpukms.c),
 * the GPU's error state, and i915's words of a context.  The work is
 * the Linux side's (linux/ctx.c); this side parses the requests.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915.h"
#include "../kmsout.h"
#include "../gpufs.c"
#include "../gpukms.c"

enum {
	Xerror = Xdrv,
};

static void*
ctxnew(char *uid, int *id, char **reply)
{
	void *lc;

	USED(uid, reply);
	lc = i915_ctx_new(id);
	if(i915_iserr(lc)){
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

	if((n == 2 || n == 3) && strcmp(f[0], "bo") == 0){
		gpu = n == 3 ? strtoull(f[2], nil, 0) : ~0ULL;
		e = i915_bo_new(c->lc, strtoull(f[1], nil, 0), &gpu, &id, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		segown(seg, c->uid);
		fsetreply(fa, "bo %d %s %#llux %#llux %#llux\n", id, seg, va, size, gpu);
		return nil;
	}
	if(n == 3 && strcmp(f[0], "bind") == 0){
		e = i915_bo_bind(c->lc, atoi(f[1]), strtoull(f[2], nil, 0));
		return e < 0 ? errstr9(e) : nil;
	}
	/* close: gpu(4)'s word of every driver for letting go of an object */
	if(n == 2 && (strcmp(f[0], "free") == 0 || strcmp(f[0], "close") == 0)){
		e = i915_bo_free(c->lc, atoi(f[1]));
		return e < 0 ? errstr9(e) : nil;
	}
	/* gpu(4)'s export and import: an object between contexts, of any process */
	if(n == 2 && strcmp(f[0], "export") == 0){
		char token[64];

		e = i915_bo_export(c->lc, atoi(f[1]), token, sizeof token);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "export %s\n", token);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "import") == 0){
		e = i915_bo_import(c->lc, f[1], &id, seg, sizeof seg, &va, &size);
		if(e < 0)
			return errstr9(e);
		segown(seg, c->uid);
		fsetreply(fa, "import %d %s %#llux %#llux\n", id, seg, va, size);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "getparam") == 0){
		int v;

		e = i915_getparam(c->lc, atoi(f[1]), &v);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "getparam %d %d\n", atoi(f[1]), v);
		return nil;
	}
	if(n == 2 && strcmp(f[0], "map") == 0 && strcmp(f[1], "plane") == 0){
		uint w, h, stride, bpp;

		/* the screen is the hostowner's (the console's) */
		if(!iseve9p(c->uid))
			return "permission denied: the display is the hostowner's";
		e = i915_map_plane(c->lc, &id, &gpu, &w, &h, &stride, &bpp);
		if(e < 0)
			return errstr9(e);
		fsetreply(fa, "plane %d %#llux %ud %ud %ud %ud\n", id, gpu, stride, bpp, w, h);
		return nil;
	}
	return "unknown ctl message";
}

static char*
ctxsubmit(Gfid *fa, char **f, int n)
{
	u64g k;
	int x;

	if(n != 2 && n != 3)
		return "usage: bo offset [rcs|bcs]";
	x = i915_submit(fa->ctx->lc, atoi(f[0]), strtoul(f[1], nil, 0),
		n == 3 && strcmp(f[2], "bcs") == 0, &k);
	return x < 0 ? errstr9(x) : nil;
}

static void
fencecount(void *lc, uvlong *done, uvlong *n, int *failed)
{
	u64g d, m;

	i915_fence_count(lc, &d, &m, failed);
	*done = d;
	*n = m;
}

static int
fencewait(void *lc, uvlong k, int query)
{
	return i915_fence_wait(lc, k, query);
}

static char*
status(void)
{
	return kmsstatus(i915_status(), i915_freestr);
}

static char*
ctlwrite(Req *r, char **f, int n)
{
	char *e;

	if(kmsuser(r, f, n, &e))
		return e;
	return "unknown ctl message";
}

/* /dev/gpu/0/error: the GPU's error state, read; a write clears it */
static void
tree(File *dir, char *user)
{
	createfile(dir, "error", user, 0664, (void*)Xerror);
}

static int
fsdrvread(Req *r)
{
	long m;

	if(KIND(r->fid->file) != Xerror)
		return 0;
	/* the text is large: as much as asked for, where asked */
	m = i915_error_read(r->ofcall.data, r->ifcall.count, r->ifcall.offset);
	if(m < 0){
		respond(r, errstr9((int)m));
		return 1;
	}
	r->ofcall.count = m;
	respond(r, nil);
	return 1;
}

static int
fsdrvwrite(Req *r)
{
	if(KIND(r->fid->file) == Xerror){
		i915_error_clear();
		r->ofcall.count = r->ifcall.count;
		respond(r, nil);
		return 1;
	}
	return kmsctlwrite(r);
}

static int
scanout(void *lc, int gem, int id, int w, int h, int stride)
{
	if(gem)
		return -22;	/* scanoutgem: amdgpu's (RADV's own objects) */
	return i915_scanout(lc, id, w, h, stride);
}

static int
overlay(void *lc, int id, int x, int y, int w, int h, int stride)
{
	return i915_overlay(lc, id, x, y, w, h, stride);
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
	tree,
	fsdrvread,
	fsdrvwrite,
};

static Kmsdrv kdrv = {
	scanout,
	overlay,
	i915_ctx_free,
};

/* serve gpu(4) as srvname, mounted after /dev */
void
serve(char *srvname)
{
	kmsserve(srvname);
}
