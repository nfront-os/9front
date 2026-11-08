/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915, aux/amdgpu: what their file servers share beyond gpufs.c
 * (each driver's fs.c includes gpufs.c, then this file, and fills in
 * kdrv): the outputs (kmsout.c, kmsscreen.c), the display's requests
 * of a context (scanout, scanoutgem, overlay: the display is the
 * hostowner's), gpu(4) for several users (usersrv.c: ctl's user), the
 * ctl's plane line.
 *
 * The display's requests go to the driver's KMS side (and amdgpu's DC,
 * whose bandwidth calculation needs more stack than lib9p's procs
 * have, 32 KB): procs of their own do them, a few, so that a scanout
 * and an overlay of different clients meet in kmsout and go in one
 * commit (one proc took them in turn: two vblanks, both at half the
 * rate).  A context goes on one of them too, when no display request
 * runs (kmsrw): its picture off the display first.
 */

typedef struct Kmsdrv Kmsdrv;
struct Kmsdrv {
	/* the context's object id (a GEM handle if gem) on the display, 0 off; < 0 Linux's error */
	int	(*scanout)(void *lc, int gem, int id, int w, int h, int stride);
	/* a GEM handle of the context's on the overlay plane at x y, 0 off; nil: no overlay */
	int	(*overlay)(void *lc, int id, int x, int y, int w, int h, int stride);
	void	(*ctxfree)(void *lc);
};
static Kmsdrv kdrv;	/* the driver's, defined after this file */
static char *srvname9;	/* serve's: /srv/srvname, /srv/srvname.user */

#include "usersrv.c"

typedef struct Kreq Kreq;
struct Kreq {
	void	*lc;
	int	free;	/* the context goes */
	int	gem;	/* id is a GEM handle of the client's (scanoutgem) */
	int	overlay;	/* overlay: id a GEM handle of the client's at x y, 0 off */
	int	id, w, h, stride, x, y;
	int	r;
	Channel	*done;
};
enum { Nkmsproc = 4 };
static Channel *kmsc;
static RWLock kmsrw;	/* a context goes (wlock) when no display request runs (rlock) */

static void
kmsproc(void*)
{
	Kreq *k;

	for(;;){
		k = recvp(kmsc);
		if(k->free){
			wlock(&kmsrw);
			kdrv.ctxfree(k->lc);
			wunlock(&kmsrw);
			sendul(k->done, 1);
			continue;
		}
		rlock(&kmsrw);
		if(k->overlay)
			k->r = kdrv.overlay(k->lc, k->id, k->x, k->y, k->w, k->h, k->stride);
		else
			k->r = kdrv.scanout(k->lc, k->gem, k->id, k->w, k->h, k->stride);
		runlock(&kmsrw);
		sendul(k->done, 1);
	}
}

/* k on a display proc, and its answer */
static int
kmsdo(Kreq *k)
{
	k->done = chancreate(sizeof(ulong), 0);
	sendp(kmsc, k);
	recvul(k->done);
	chanfree(k->done);
	return k->r;
}

/* gpufs' ctxfree: the context goes on a display proc */
static void
kmsctxfree(void *lc)
{
	Kreq k;

	memset(&k, 0, sizeof k);
	k.lc = lc;
	k.free = 1;
	kmsdo(&k);
}

/*
 * a context's display request (r a write to its ctl, f its words):
 * scanout id [w h stride], scanoutgem handle w h stride, overlay handle
 * x y w h stride, overlay off; 1 if it was one (responded)
 */
static int
kmswrite(Req *r, char **f, int n)
{
	Gfid *fa;
	Kreq k;
	int x;

	fa = r->fid->aux;
	memset(&k, 0, sizeof k);
	if((n == 2 || n == 5) && strcmp(f[0], "scanout") == 0 || n == 5 && strcmp(f[0], "scanoutgem") == 0){
		k.gem = f[0][7] == 'g';
		k.id = strtoul(f[1], nil, 0);
		if(n == 5){
			k.w = atoi(f[2]);
			k.h = atoi(f[3]);
			k.stride = atoi(f[4]);
		}
	}else if((n == 2 && strcmp(f[1], "off") == 0 || n == 7) && strcmp(f[0], "overlay") == 0){
		if(kdrv.overlay == nil){
			respond(r, "no overlay plane");
			return 1;
		}
		k.overlay = 1;
		if(n == 7){
			k.id = strtoul(f[1], nil, 0);
			k.x = atoi(f[2]);
			k.y = atoi(f[3]);
			k.w = atoi(f[4]);
			k.h = atoi(f[5]);
			k.stride = atoi(f[6]);
		}
	}else
		return 0;
	/* the display is the hostowner's (the console's) */
	if(!iseve9p(r->fid->uid)){
		respond(r, "permission denied: the display is the hostowner's");
		return 1;
	}
	k.lc = fa->ctx->lc;
	/* returns when the display shows it: other requests go on */
	srvrelease(r->srv);
	x = kmsdo(&k);
	srvacquire(r->srv);
	if(x < 0){
		respond(r, errstr9(x));
		return 1;
	}
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
	return 1;
}

/* a context's ctl write as kmswrite's words: 1 if it was one of them (responded) */
static int
kmsctlwrite(Req *r)
{
	char buf[256], *f[8];
	int n;

	if(KIND(r->fid->file) != Xctxctl)
		return 0;
	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	n = tokenize(buf, f, nelem(f));
	return n > 0 && kmswrite(r, f, n);
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

static int
kmsoutputs(char **names, int max)
{
	return kmsout_names(names, max);
}

static char*
kmsoutread(Req *r, char *name, int kind)
{
	uchar buf[4096];
	char *s, *t;
	int m;

	switch(kind){
	case Xoutedid:
		m = kmsout_edid(name, buf, sizeof buf);
		readbuf(r, buf, m < 0 ? 0 : m);
		return nil;
	case Xoutmodes:
	case Xoutmode:
		t = kind == Xoutmodes ? kmsout_modes(name) : kmsout_mode(name);
		break;
	default:	/* Xoutbacklight */
		t = nil;
		outreq(r, name, nil, 1, &t);
		break;
	}
	s = estrdup9p(t != nil ? t : "");
	kmsout_free(t);
	readstr(r, s);
	free(s);
	return nil;
}

static char*
kmsoutwrite(Req *r, char *name, int kind, char *s)
{
	return outreq(r, name, s, kind == Xoutbacklight, nil);
}

/* ctl's plane line: draw(3)'s screen */
static char*
kmsctlread(void)
{
	char *t, *s;

	t = kmsout_plane();
	s = estrdup9p(t != nil ? t : "plane none\n");
	kmsout_free(t);
	return s;
}

/* ctl's user: /srv/srvname.user, gpu(4) for that user; 1 if it was the word, *e its error */
static int
kmsuser(Req *r, char **f, int n, char **e)
{
	if(n != 2 || strcmp(f[0], "user") != 0)
		return 0;
	if(!iseve9p(r->fid->uid))
		*e = "permission denied: the hostowner's";
	else
		*e = postuser(&fs, srvname9, f[1]);
	return 1;
}

/* the status: the driver's d (freed with dfree), the outputs', the screen's */
static char*
kmsstatus(char *d, void (*dfree)(char*))
{
	char *o, *sc, *s;

	o = kmsout_status();
	sc = screenstatus();
	s = smprint("%s%s%s", d != nil ? d : "", o != nil ? o : "", sc != nil ? sc : "");
	free(sc);
	dfree(d);
	kmsout_free(o);
	return s;
}

/* serve gpu(4) as srvname, mounted after /dev, for several users */
static void
kmsserve(char *srvname)
{
	int i;

	gpufsinit(1);
	fs.attach = srvattach;
	fs.aux = estrdup9p(getuser());	/* /srv/srvname is the hostowner's */
	srvname9 = estrdup9p(srvname);
	kmsc = chancreate(sizeof(void*), 0);
	for(i = 0; i < Nkmsproc; i++)
		proccreate(kmsproc, nil, 1024*1024);
	threadpostmountsrv(&fs, srvname, "/dev", MAFTER);
}
