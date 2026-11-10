/*
 * drawfs: draw(3) as a file server in user space (../9front-ui PLAN stage
 * 4c). The kernel's devdraw (devdraw.c, its copy), libmemdraw and
 * libmemlayer do the drawing, into a screen image in a segment of #g
 * (drawfs.pid) that its owner (wsys) can attach; this serves their files
 * as #i has them, for programs to bind on /dev:
 *
 *	draw/new, draw/n/{colormap,ctl,data,refresh}, winname
 *
 * and, for the owner, drawfs/screen ("segment chan minx miny maxx maxy
 * width": the screen's memory) and drawfs/dirty (the rectangles of the
 * screen drawn on since the last read, "minx miny maxx maxy" lines; a
 * read waits for one).
 *
 * usage: drawfs [-D] [-c chan] [-r width height] [-s srvname] [-m mtpt]
 */
#include "compat.h"
#include <fcall.h>
#include <9p.h>

/* devdraw.c's file types (the low 4 bits of a qid's path) */
enum {
	Qtopdir		= 0,
	Qnew,
	Qwinname,
	Q3rd,
	Q2nd,
	Qcolormap,
	Qctl,
	Qdata,
	Qrefresh,
	QSHIFT		= 4,
};

/* drawfs' own files: above devdraw's 32 bits */
#define	Qfsdir		(1ULL<<32)
#define	Qfsscreen	(2ULL<<32)
#define	Qfsdirty	(3ULL<<32)

#define	QTYPE(p)	((p)&0xF)
#define	QSLOT(p)	(((p)&0x7FFFFFF0)>>QSHIFT)

static char	*segname;
static Rectangle	screenr;
static ulong	screenchan;
static int	screenwidth;	/* in ulongs */
static Memdata	screendata;

static ulong	cmap[256][3];

static Rectangle	*dirty;
static int	ndirty, adirty;

static Req	*refreshq, *dirtyq;
static int	towake;

static char	Enotfound[] = "file does not exist";
static char	Eperm[] = "permission denied";

/* ---- the screen ---- */

/* a segment of #g for the screen: its owner attaches it by name */
static uchar*
mkscreen(ulong size)
{
	char path[64], buf[64];
	void *v;
	int fd;

	segname = smprint("drawfs.%d", getpid());
	snprint(path, sizeof path, "#g/%s", segname);
	if((fd = create(path, OREAD, DMDIR|0700)) < 0)
		sysfatal("create %s: %r", path);
	close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", segname);
	if((fd = open(path, OWRITE)) < 0)
		sysfatal("open %s: %r", path);
	if(fprint(fd, "va 0 %lud", size) < 0)
		sysfatal("%s: va: %r", path);
	close(fd);
	if((v = segattach(0, segname, nil, 0)) == (void*)-1)
		sysfatal("segattach %s: %r", segname);
	return v;
}

Memdata*
attachscreen(Rectangle *r, ulong *chan, int *depth, int *width, int *softscreen)
{
	*r = screenr;
	*chan = screenchan;
	*depth = chantodepth(screenchan);
	*width = screenwidth;
	*softscreen = 1;
	screendata.ref = 1;
	return &screendata;
}

/* devdraw flushes what it drew on the screen: kept for drawfs/dirty */
void
flushmemscreen(Rectangle r)
{
	int i;

	if(!rectclip(&r, screenr))
		return;
	for(i = 0; i < ndirty; i++)
		if(rectinrect(r, dirty[i]))
			return;
	if(ndirty == adirty){
		adirty = adirty ? 2*adirty : 16;
		dirty = erealloc9p(dirty, adirty*sizeof dirty[0]);
	}
	dirty[ndirty++] = r;
	towake = 1;
}

void
getcolor(ulong i, ulong *r, ulong *g, ulong *b)
{
	i &= 0xFF;
	*r = cmap[i][0];
	*g = cmap[i][1];
	*b = cmap[i][2];
}

int
setcolor(ulong i, ulong r, ulong g, ulong b)
{
	i &= 0xFF;
	cmap[i][0] = r;
	cmap[i][1] = g;
	cmap[i][2] = b;
	return 1;
}

void
refreshwake(void)
{
	towake = 1;
}

/* ---- waiting reads ---- */

static void
enqueue(Req **q, Req *r)
{
	Req **l;

	r->aux = nil;
	for(l = q; *l != nil; l = (Req**)&(*l)->aux)
		;
	*l = r;
}

static int
unqueue(Req **q, Req *r)
{
	Req **l;

	for(l = q; *l != nil; l = (Req**)&(*l)->aux)
		if(*l == r){
			*l = r->aux;
			return 1;
		}
	return 0;
}

static void
dirtyread(Req *r)
{
	char *s, *e;
	int i;

	s = r->ofcall.data;
	e = s + r->ifcall.count;
	for(i = 0; i < ndirty; i++){
		if(e - s < 4*12+1)
			break;
		s = seprint(s, e, "%d %d %d %d\n", dirty[i].min.x, dirty[i].min.y, dirty[i].max.x, dirty[i].max.y);
	}
	memmove(dirty, dirty+i, (ndirty-i)*sizeof dirty[0]);
	ndirty -= i;
	r->ofcall.count = s - r->ofcall.data;
	respond(r, nil);
}

/* a read of devdraw: 1 done (answered), 0 still nothing to read (refresh) */
static int
doread(Req *r)
{
	long n;

	if(waserror()){
		if(strcmp(errbuf, Eblock) == 0)
			return 0;
		respond(r, errbuf);
		return 1;
	}
	n = drawread(r->fid->aux, r->ofcall.data, r->ifcall.count, r->ifcall.offset);
	poperror();
	r->ofcall.count = n;
	respond(r, nil);
	return 1;
}

/* after a request: the waiting reads that can be answered now */
static void
wake(void)
{
	Req *r, *q;

	while(towake){
		towake = 0;
		q = refreshq;
		refreshq = nil;
		while((r = q) != nil){
			q = r->aux;
			if(!doread(r))
				enqueue(&refreshq, r);
		}
		while(ndirty > 0 && (r = dirtyq) != nil){
			dirtyq = r->aux;
			dirtyread(r);
		}
	}
}

/* ---- the files ---- */

static void
mkqid(Qid *q, uvlong path, int type)
{
	q->path = path;
	q->vers = 0;
	q->type = type;
}

static int
dirgen(int n, Dir *d, void *aux)
{
	uvlong p;
	int s, id;
	char buf[16];

	p = (uvlong)(uintptr)aux;
	memset(d, 0, sizeof *d);
	d->uid = estrdup9p(getuser());
	d->gid = estrdup9p(d->uid);
	d->muid = estrdup9p("");
	d->atime = d->mtime = time(0);
	if(p == Qtopdir){
		switch(n){
		case 0:
			mkqid(&d->qid, Q2nd, QTDIR);
			d->name = estrdup9p("draw");
			d->mode = DMDIR|0555;
			return 0;
		case 1:
			mkqid(&d->qid, Qwinname, QTFILE);
			d->name = estrdup9p("winname");
			d->mode = 0444;
			return 0;
		case 2:
			mkqid(&d->qid, Qfsdir, QTDIR);
			d->name = estrdup9p("drawfs");
			d->mode = DMDIR|0500;
			return 0;
		}
	}else if(p == Q2nd){
		if(n == 0){
			mkqid(&d->qid, Qnew, QTFILE);
			d->name = estrdup9p("new");
			d->mode = 0666;
			return 0;
		}
		/* the clients: the n'th that is there */
		for(s = 1; (id = drawclientid(s)) >= 0; s++)
			if(id > 0 && --n == 0){
				snprint(buf, sizeof buf, "%d", id);
				mkqid(&d->qid, (s<<QSHIFT)|Q3rd, QTDIR);
				d->name = estrdup9p(buf);
				d->mode = DMDIR|0555;
				return 0;
			}
	}else if(p == Qfsdir){
		switch(n){
		case 0:
			mkqid(&d->qid, Qfsscreen, QTFILE);
			d->name = estrdup9p("screen");
			d->mode = 0400;
			return 0;
		case 1:
			mkqid(&d->qid, Qfsdirty, QTFILE);
			d->name = estrdup9p("dirty");
			d->mode = 0400;
			return 0;
		}
	}else if(QTYPE(p) == Q3rd){
		p &= ~0xFULL;
		switch(n){
		case 0:
			mkqid(&d->qid, p|Qcolormap, QTFILE);
			d->name = estrdup9p("colormap");
			d->mode = 0600;
			return 0;
		case 1:
			mkqid(&d->qid, p|Qctl, QTFILE);
			d->name = estrdup9p("ctl");
			d->mode = 0600;
			return 0;
		case 2:
			mkqid(&d->qid, p|Qdata, QTFILE);
			d->name = estrdup9p("data");
			d->mode = 0600;
			return 0;
		case 3:
			mkqid(&d->qid, p|Qrefresh, QTFILE);
			d->name = estrdup9p("refresh");
			d->mode = 0400;
			return 0;
		}
	}
	free(d->uid);
	free(d->gid);
	free(d->muid);
	return -1;
}

/* the directory's entry of qid path p, or -1 */
static int
statof(uvlong p, Dir *d)
{
	uvlong dir;
	int n;

	if(p == Qtopdir){
		memset(d, 0, sizeof *d);
		mkqid(&d->qid, Qtopdir, QTDIR);
		d->name = estrdup9p("/");
		d->uid = estrdup9p(getuser());
		d->gid = estrdup9p(d->uid);
		d->muid = estrdup9p("");
		d->mode = DMDIR|0555;
		return 0;
	}
	if(p == Q2nd || p == Qwinname || p == Qfsdir)
		dir = Qtopdir;
	else if(p == Qfsscreen || p == Qfsdirty)
		dir = Qfsdir;
	else if(p == Qnew || QTYPE(p) == Q3rd)
		dir = Q2nd;
	else
		dir = (p & ~0xFULL) | Q3rd;
	for(n = 0; dirgen(n, d, (void*)(uintptr)dir) == 0; n++){
		if(d->qid.path == p)
			return 0;
		free(d->name);
		free(d->uid);
		free(d->gid);
		free(d->muid);
	}
	return -1;
}

static void
fsattach(Req *r)
{
	mkqid(&r->fid->qid, Qtopdir, QTDIR);
	r->ofcall.qid = r->fid->qid;
	respond(r, nil);
}

static char*
fswalk1(Fid *fid, char *name, Qid *qid)
{
	uvlong p;
	int s, n;
	Dir d;

	p = fid->qid.path;
	if(strcmp(name, "..") == 0){
		if(p == Q2nd || p == Qfsdir)
			p = Qtopdir;
		else if(QTYPE(p) == Q3rd)
			p = Q2nd;
		mkqid(qid, p, QTDIR);
		fid->qid = *qid;
		return nil;
	}
	if(QTYPE(p) == Q2nd && p == Q2nd && strcmp(name, "new") != 0){
		/* a client by its id */
		n = atoi(name);
		if(n <= 0 || (s = drawclientslot(n)) == 0)
			return Enotfound;
		mkqid(qid, (s<<QSHIFT)|Q3rd, QTDIR);
		fid->qid = *qid;
		return nil;
	}
	for(n = 0; dirgen(n, &d, (void*)(uintptr)p) == 0; n++){
		s = strcmp(d.name, name);
		free(d.name);
		free(d.uid);
		free(d.gid);
		free(d.muid);
		if(s == 0){
			*qid = d.qid;
			fid->qid = *qid;
			return nil;
		}
	}
	return Enotfound;
}

static void
fsstat(Req *r)
{
	if(statof(r->fid->qid.path, &r->d) < 0){
		respond(r, Enotfound);
		return;
	}
	respond(r, nil);
}

static void
fsopen(Req *r)
{
	Chan *c;
	uvlong p;

	p = r->fid->qid.path;
	if(r->fid->qid.type & QTDIR){
		if((r->ifcall.mode & 3) != OREAD){
			respond(r, Eperm);
			return;
		}
		respond(r, nil);
		return;
	}
	if(p == Qfsscreen || p == Qfsdirty){
		respond(r, nil);
		return;
	}
	c = emallocz(sizeof *c);
	c->qid = r->fid->qid;
	if(waserror()){
		free(c);
		respond(r, errbuf);
		return;
	}
	drawopen(c, r->ifcall.mode);
	poperror();
	r->fid->aux = c;
	r->fid->qid = c->qid;	/* new: the client's ctl now */
	r->ofcall.qid = c->qid;
	respond(r, nil);
	wake();
}

static void
fsread(Req *r)
{
	char buf[256];
	uvlong p;

	p = r->fid->qid.path;
	if(r->fid->qid.type & QTDIR){
		dirread9p(r, dirgen, (void*)(uintptr)p);
		respond(r, nil);
		return;
	}
	if(p == Qfsscreen){
		snprint(buf, sizeof buf, "%s %s %d %d %d %d %d\n", segname, chantostr(buf+128, screenchan),
			screenr.min.x, screenr.min.y, screenr.max.x, screenr.max.y, screenwidth);
		readstr(r, buf);
		respond(r, nil);
		return;
	}
	if(p == Qfsdirty){
		if(ndirty > 0)
			dirtyread(r);
		else
			enqueue(&dirtyq, r);
		return;
	}
	if(!doread(r))
		enqueue(&refreshq, r);
	wake();
}

static void
fswrite(Req *r)
{
	long n;

	if(waserror()){
		respond(r, errbuf);
		wake();
		return;
	}
	n = drawwrite(r->fid->aux, r->ifcall.data, r->ifcall.count, r->ifcall.offset);
	poperror();
	r->ofcall.count = n;
	respond(r, nil);
	wake();
}

static void
fsflush(Req *r)
{
	Req *o;

	o = r->oldreq;
	if(unqueue(&refreshq, o) || unqueue(&dirtyq, o))
		respond(o, "interrupted");
	respond(r, nil);
}

static void
fsdestroyfid(Fid *fid)
{
	Chan *c;

	if((c = fid->aux) == nil)
		return;
	fid->aux = nil;
	if(!waserror()){
		drawclose(c);
		poperror();
	}
	free(c);
	wake();
}

static Srv fs = {
	.attach = fsattach,
	.walk1 = fswalk1,
	.stat = fsstat,
	.open = fsopen,
	.read = fsread,
	.write = fswrite,
	.flush = fsflush,
	.destroyfid = fsdestroyfid,
};

static void
usage(void)
{
	fprint(2, "usage: %s [-D] [-c chan] [-r width height] [-s srvname] [-m mtpt]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *srv, *mtpt, *cs;
	int wd, ht, depth;
	ulong size;
	uchar *v;

	srv = nil;
	mtpt = nil;
	cs = "x8r8g8b8";
	wd = 1024;
	ht = 768;
	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 'c':
		cs = EARGF(usage());
		break;
	case 'r':
		wd = atoi(EARGF(usage()));
		ht = atoi(EARGF(usage()));
		break;
	case 's':
		srv = EARGF(usage());
		break;
	case 'm':
		mtpt = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0 || wd <= 0 || ht <= 0)
		usage();
	if((screenchan = strtochan(cs)) == 0 || (depth = chantodepth(screenchan)) == 0)
		sysfatal("bad channel %s", cs);
	if(srv == nil && mtpt == nil)
		mtpt = "/dev";

	memimageinit();
	screenr = Rect(0, 0, wd, ht);
	screenwidth = wordsperline(screenr, depth);
	size = screenwidth*sizeof(ulong)*ht;
	v = mkscreen(size);
	screendata.base = (ulong*)v;
	screendata.bdata = v;
	screendata.allocd = 0;
	drawcmap();
	if(!drawinit())
		sysfatal("no screen image");
	postmountsrv(&fs, srv, mtpt, MBEFORE);
	exits(nil);
}
