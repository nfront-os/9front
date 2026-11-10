/*
 * drawfs' core for a program that serves draw(3) itself (wsys, with
 * APE's build: sys/src/ape/lib/devdraw): devdraw.c's channels behind
 * plain calls, errors as strings. The screen's memory is the caller's
 * (drawcore_init); what devdraw draws on it is kept as rectangles
 * (drawcore_dirty). Every call holds one lock: devdraw keeps one error
 * stack (compat.c) and its own lock is not recursive.
 */
#include "compat.h"
#include "core.h"
#include <time.h>

static QLock	corelk;

/*
 * what memdraw is asked to do (hwdraw sees every draw first): the classes
 * its paths are for, calls and pixels each, on the screen or elsewhere;
 * and the time devdraw takes (drawcore_write): where drawing on the GPU
 * would pay (../9front-ui PLAN stage 4c)
 */
enum {
	Sfill,	/* a colour, no mask or a full one (memoptdraw: memset) */
	Scopy,	/* an image, no mask, S or an opaque one (memoptdraw: memmove) */
	Schar,	/* a colour through a mask (chardraw: text) */
	Sblend,	/* the rest (alphadraw) */
	Nstat,
};
static struct {
	uvlong	calls;
	uvlong	pixels;
} stat[Nstat][2];	/* [class][on the screen] */
static uvlong	writens, writes;
static Rectangle	screenr;
static ulong	screenchan;
static int	screenwidth;
static Memdata	screendata;
static ulong	cmap[256][3];
static Rectangle	dirty[64];
static int	ndirty;
static int	towake;

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

void
flushmemscreen(Rectangle r)
{
	int i;

	if(!rectclip(&r, screenr))
		return;
	for(i = 0; i < ndirty; i++)
		if(rectinrect(r, dirty[i]))
			return;
	if(ndirty == nelem(dirty)){
		/* full: one rectangle for all of them */
		for(i = 1; i < ndirty; i++)
			combinerect(&dirty[0], dirty[i]);
		combinerect(&dirty[0], r);
		ndirty = 1;
		return;
	}
	dirty[ndirty++] = r;
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

static uvlong
nsnow(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return (uvlong)t.tv_sec*1000000000ULL + t.tv_nsec;
}

/* memdraw's hook, before its own paths: counted, done by them */
int
hwdraw(Memdrawparam *p)
{
	int c, onscreen;

	if(p->state&Simplesrc){
		if(p->mask == memopaque || (p->state&Fullmask))
			c = Sfill;
		else
			c = Schar;
	}else if(p->mask == memopaque && (p->op == S || (p->op == SoverD && !(p->src->flags&Falpha))))
		c = Scopy;
	else
		c = Sblend;
	onscreen = p->dst->data == &screendata;
	stat[c][onscreen].calls++;
	stat[c][onscreen].pixels += (uvlong)Dx(p->r)*Dy(p->r);
	return 0;
}

/* the counts so far, a line each: class calls pixels (screen and not), devdraw's time */
int
drawcore_stats(char *buf, int n)
{
	static char *name[Nstat] = { "fill", "copy", "char", "blend" };
	char *p, *e;
	int i;

	qlock(&corelk);
	p = buf;
	e = buf+n;
	for(i = 0; i < Nstat; i++)
		p = seprint(p, e, "%s %llud %llud screen %llud %llud\n", name[i],
			stat[i][0].calls, stat[i][0].pixels, stat[i][1].calls, stat[i][1].pixels);
	p = seprint(p, e, "writes %llud ms %llud\n", writes, writens/1000000);
	qunlock(&corelk);
	return p-buf;
}

static void
seterr(char *err, int nerr)
{
	if(err != nil && nerr > 0)
		snprint(err, nerr, "%s", errbuf);
}

int
drawcore_init(void *mem, int minx, int miny, int maxx, int maxy, char *chan, char *err, int nerr)
{
	int ok, depth;

	qlock(&corelk);
	memimageinit();
	screenr = Rect(minx, miny, maxx, maxy);
	if((screenchan = strtochan(chan)) == 0 || (depth = chantodepth(screenchan)) == 0){
		snprint(err, nerr, "bad channel %s", chan);
		qunlock(&corelk);
		return -1;
	}
	screenwidth = wordsperline(screenr, depth);
	screendata.base = mem;
	screendata.bdata = mem;
	screendata.allocd = 0;
	drawcmap();
	ok = drawinit();
	qunlock(&corelk);
	if(!ok){
		snprint(err, nerr, "no screen image");
		return -1;
	}
	return 0;
}

int
drawcore_width(void)
{
	return screenwidth;
}

void*
drawcore_open(uvlong path, int mode, uvlong *npath, char *err, int nerr)
{
	Chan *c;

	qlock(&corelk);
	c = emallocz(sizeof *c);
	c->qid.path = path;
	c->qid.type = QTFILE;
	if(waserror()){
		seterr(err, nerr);
		free(c);
		qunlock(&corelk);
		return nil;
	}
	drawopen(c, mode);
	poperror();
	*npath = c->qid.path;
	qunlock(&corelk);
	return c;
}

long
drawcore_read(void *c, void *buf, long n, vlong off, char *err, int nerr)
{
	long r;

	qlock(&corelk);
	if(waserror()){
		r = strcmp(errbuf, Eblock) == 0 ? -2 : -1;
		if(r == -1)
			seterr(err, nerr);
		qunlock(&corelk);
		return r;
	}
	r = drawread(c, buf, n, off);
	poperror();
	qunlock(&corelk);
	return r;
}

long
drawcore_write(void *c, void *buf, long n, char *err, int nerr)
{
	long r;
	uvlong t;

	qlock(&corelk);
	if(waserror()){
		seterr(err, nerr);
		qunlock(&corelk);
		return -1;
	}
	t = nsnow();
	r = drawwrite(c, buf, n, 0);
	writens += nsnow()-t;
	writes++;
	poperror();
	qunlock(&corelk);
	return r;
}

void
drawcore_close(void *c)
{
	qlock(&corelk);
	if(!waserror()){
		drawclose(c);
		poperror();
	}
	free(c);
	qunlock(&corelk);
}

int
drawcore_clientslot(int id)
{
	int s;

	qlock(&corelk);
	s = drawclientslot(id);
	qunlock(&corelk);
	return s;
}

int
drawcore_clientid(int slot)
{
	int id;

	qlock(&corelk);
	id = drawclientid(slot);
	qunlock(&corelk);
	return id;
}

/* the rectangles drawn on since the last call (minx miny maxx maxy each), their number */
int
drawcore_dirty(int *r, int max)
{
	int i, n;

	qlock(&corelk);
	n = ndirty < max ? ndirty : max;
	for(i = 0; i < n; i++){
		r[4*i+0] = dirty[i].min.x;
		r[4*i+1] = dirty[i].min.y;
		r[4*i+2] = dirty[i].max.x;
		r[4*i+3] = dirty[i].max.y;
	}
	memmove(dirty, dirty+n, (ndirty-n)*sizeof dirty[0]);
	ndirty -= n;
	qunlock(&corelk);
	return n;
}

/* 1 if a refresh read may have something now (and forget it) */
int
drawcore_wake(void)
{
	int w;

	qlock(&corelk);
	w = towake;
	towake = 0;
	qunlock(&corelk);
	return w;
}
