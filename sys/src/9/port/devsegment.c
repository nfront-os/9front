#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

enum
{
	Qtopdir,
	Qsegdir,
	Qctl,
	Qdata,
	Qpa,	/* 9front-ng: physical addresses of a fixed or sticky segment's pages */
};

#define TYPE(c) 	(int)( (c)->qid.path & 0x7 )
#define SEG(c)	 	( ((c)->qid.path >> 3) & 0xfffff )	/* 9front-ng: was 0x3f; Maxglobal */
#define PATH(s, t) 	( ((s)<<3) | (t) )

typedef struct Globalseg Globalseg;
typedef struct Pagerq Pagerq;
typedef struct Pagerreq Pagerreq;

struct Globalseg
{
	Ref;

	char	*name;
	char	*uid;
	vlong	length;
	long	perm;
	int	alias;	/* 9front-ng: made by "va ... alias" (takes page) or "va ... phys": revoke */
	Pagerq	*q;	/* 9front-ng: made by "pager": a driver reads its pagers' faults from data */

	Segio;
};

/*
 * 9front-ng: pager segments, a driver's memory shown page by page as
 * Linux maps a GPU object: "va VA LEN phys PHYS pager QUEUE" makes a
 * part of the physical segment PHYS (a BAR) without pages; a fault on
 * one queues "NAME PAGE" on the queue QUEUE (a directory made by
 * "pager", its data read by the driver) and waits; the driver puts the
 * pages where the object is ("pages IDX OFF..."), takes them all back
 * when it moves it ("unmap": the mappings flushed everywhere), or says
 * there is none ("nopage": the fault fails).  A removed segment or a
 * queue nobody reads any more fails the faults too.
 */
struct Pagerreq
{
	Pagerreq	*next;
	char	line[1];
};

struct Pagerq
{
	Ref;
	Lock;
	Rendez	r;	/* the driver's read waits here */
	Pagerreq	*head, *tail;
	int	readers;	/* open data files */
	int	dead;		/* removed, or its last reader gone */
};

struct Pager
{
	Lock;
	Rendez	r;	/* faults wait here */
	Segment	*s;
	Pagerq	*q;
	char	*name;
	uintptr	base;	/* the physical segment's start and size */
	uintptr	size;
	uintptr	*pa;	/* each page's address, or ~0 */
	ulong	npage;
	int	asked;	/* a request is queued */
	ulong	nopage;	/* bumped by "nopage" */
	int	dead;
};

/*
 * 9front-ng: was 100 (and SEG reached 64).  GPU drivers show each object
 * a client maps as a segment (a Vulkan program keeps hundreds): the
 * table grows (segmentcreate), doubling, up to Maxglobal; every use
 * holds globalseglock, so an outgrown table is freed
 */
enum { Nglobal0 = 1024, Maxglobal = 1<<20 };
static Globalseg **globalseg;
static int nglobalseg;
static Lock globalseglock;


	Segment* (*_globalsegattach)(char*);
static	Segment* globalsegattach(char *name);

static	Segment* fixedseg(uintptr va, ulong len, int type, uintptr align);

/*
 * 9front-ng: the ranges of SHAREDBASE..SHAREDTOP handed out by "va 0 len",
 * sorted.  Each holds a reference to its segment: the range is free again
 * once that is the only one left (the #g entry removed, every process
 * detached), seen when a range is wanted.
 */
typedef struct Srange Srange;
struct Srange
{
	uintptr	base;
	uintptr	top;
	Segment	*s;	/* nil while the segment is being made */
	Srange	*next;
};
static Srange *sranges;
static Lock srangelock;

/* a free range of len bytes (rounded to 2 MB: page tables of their own) */
static uintptr
srangealloc(uintptr len)
{
	Srange **l, *r, *n, *dead;
	uintptr va;

	len = (len + 2*1024*1024-1) & ~(uintptr)(2*1024*1024-1);
	n = smalloc(sizeof(Srange));
	dead = nil;
	lock(&srangelock);
	for(l = &sranges; (r = *l) != nil;){
		if(r->s != nil && r->s->ref == 1){
			*l = r->next;
			r->next = dead;
			dead = r;
			continue;
		}
		l = &r->next;
	}
	va = SHAREDBASE;
	for(l = &sranges; (r = *l) != nil; l = &r->next){
		if(r->base - va >= len)
			break;
		va = r->top;
	}
	if(va > SHAREDTOP || SHAREDTOP - va < len){
		unlock(&srangelock);
		free(n);
		va = 0;
	}else{
		n->base = va;
		n->top = va + len;
		n->s = nil;
		n->next = *l;
		*l = n;
		unlock(&srangelock);
	}
	while((r = dead) != nil){
		dead = r->next;
		putseg(r->s);
		free(r);
	}
	return va;
}

/* the range at va: its segment made (s), or not (nil: the range is freed) */
static void
srangeset(uintptr va, Segment *s)
{
	Srange **l, *r;

	lock(&srangelock);
	for(l = &sranges; (r = *l) != nil; l = &r->next)
		if(r->base == va)
			break;
	if(r != nil && s != nil){
		incref(s);
		r->s = s;
		r = nil;
	}else if(r != nil)
		*l = r->next;
	unlock(&srangelock);
	free(r);
}
static	void	pagerqdead(Pagerq*);
static	void	putpagerq(Pagerq*);

/*
 *  returns with globalseg incref'd
 */
static Globalseg*
getgseg(Chan *c)
{
	int x;
	Globalseg *g;

	x = SEG(c);
	lock(&globalseglock);
	if(x >= nglobalseg)
		panic("getgseg");
	g = globalseg[x];
	if(g != nil)
		incref(&g->Ref);
	unlock(&globalseglock);
	if(g == nil)
		error("global segment disappeared");
	return g;
}

static void
putgseg(Globalseg *g)
{
	if(g == nil || decref(&g->Ref))
		return;
	if(g->Segio.s != nil)
		putseg(g->Segio.s);
	if(g->q != nil){
		pagerqdead(g->q);
		putpagerq(g->q);
	}
	segio(&g->Segio,nil,nil,0,0,0);
	free(g->name);
	free(g->uid);
	free(g);
}

static void
putpagerq(Pagerq *q)
{
	Pagerreq *r;

	if(q == nil || decref(&q->Ref))
		return;
	while((r = q->head) != nil){
		q->head = r->next;
		free(r);
	}
	free(q);
}

static void
pagerqdead(Pagerq *q)
{
	lock(q);
	q->dead = 1;
	unlock(q);
	wakeup(&q->r);
}

/* the page at off's address, if it is there (s->QLock held) */
static int
pagerpa(Pager *pg, uintptr off, uintptr *pa)
{
	ulong i;
	int ok;

	i = off/BY2PG;
	lock(pg);
	ok = i < pg->npage && pg->pa[i] != ~(uintptr)0;
	if(ok)
		*pa = pg->pa[i];
	unlock(pg);
	return ok;
}

typedef struct Pagerwait Pagerwait;
struct Pagerwait
{
	Pager	*pg;
	ulong	i;
	ulong	nopage;
};

static int
pagerready(void *a)
{
	Pagerwait *w = a;
	Pager *pg = w->pg;

	/* the page, an end, or no request outstanding (taken back meanwhile: ask again) */
	return pg->pa[w->i] != ~(uintptr)0 || pg->dead || pg->q->dead || pg->nopage != w->nopage || !pg->asked;
}

/*
 * the fault of a page not there: ask the driver (once a request is
 * outstanding), wait for it; 0: go around (also after a note: the note
 * first, then the fault again), -1: no page to come
 */
static int
pagerwait(Pager *pg, uintptr off)
{
	Pagerwait w;
	Pagerreq *r;
	Pagerq *q;
	int n, waited;

	w.pg = pg;
	w.i = off/BY2PG;
	if(w.i >= pg->npage)
		return -1;
	q = pg->q;
	n = strlen(pg->name) + 32;
	lock(pg);
	w.nopage = pg->nopage;
	unlock(pg);
	r = nil;
	for(waited = 0;; waited++){
		if(r == nil && (r = malloc(sizeof(Pagerreq) + n)) == nil)
			return -1;
		lock(pg);
		if(pg->dead || q->dead || pg->nopage != w.nopage){
			unlock(pg);
			free(r);
			return pg->pa[w.i] != ~(uintptr)0 ? 0 : -1;
		}
		if(pg->pa[w.i] != ~(uintptr)0){
			unlock(pg);
			free(r);
			return 0;
		}
		if(!pg->asked){
			pg->asked = 1;
			snprint(r->line, n, "%s %lud\n", pg->name, w.i);
			r->next = nil;
			lock(q);
			if(q->tail != nil)
				q->tail->next = r;
			else
				q->head = r;
			q->tail = r;
			unlock(q);
			r = nil;
			unlock(pg);
			wakeup(&q->r);
		} else
			unlock(pg);
		if(waserror()){
			free(r);
			return 0;
		}
		tsleep(&pg->r, pagerready, &w, 1000);
		poperror();
		if(up->procctl == Proc_exitme || up->procctl == Proc_exitbig){
			free(r);
			return 0;
		}
		/* a driver that does not answer: say so (once) */
		if(waited == 5)
			print("pager %s page %lud: waiting 5 s (pid %lud)\n", pg->name, w.i, up->pid);
	}
}

static void
pagerfree(Pager *pg)
{
	putpagerq(pg->q);
	free(pg->name);
	free(pg->pa);
	free(pg);
}

static int
havereq(void *a)
{
	Pagerq *q = a;

	return q->head != nil || q->dead;
}

/* the driver's read of its queue: whole lines of requests (copied out after the lock) */
static long
pagerqread(Pagerq *q, char *a, long n)
{
	Pagerreq *r;
	char *buf;
	long m, l;

	sleep(&q->r, havereq, q);
	buf = smalloc(n);
	m = 0;
	lock(q);
	while((r = q->head) != nil){
		l = strlen(r->line);
		if(m + l > n)
			break;
		memmove(buf+m, r->line, l);
		m += l;
		q->head = r->next;
		if(q->head == nil)
			q->tail = nil;
		free(r);
	}
	unlock(q);
	if(waserror()){
		free(buf);
		nexterror();
	}
	if(m == 0 && !q->dead)
		error(Etoosmall);
	memmove(a, buf, m);
	poperror();
	free(buf);
	return m;
}

static Pagerq*
findpagerq(char *name)
{
	Globalseg *g;
	Pagerq *q;
	int x;

	q = nil;
	lock(&globalseglock);
	for(x = 0; x < nglobalseg; x++){
		g = globalseg[x];
		if(g != nil && g->q != nil && strcmp(g->name, name) == 0){
			q = g->q;
			incref(&q->Ref);
			break;
		}
	}
	unlock(&globalseglock);
	if(q == nil)
		error("no such pager queue");
	return q;
}

static Pager*
pagerof(Globalseg *g)
{
	Segment *s;

	s = g->Segio.s;
	if(s == nil || (s->type&SG_TYPE) != SG_PHYSICAL || s->pseg->pager == nil)
		error("not a pager segment");
	return s->pseg->pager;
}

/*
 * "pages IDX OFF...": the pages at the offsets OFF of the physical
 * segment from page IDX on; "ram IDX PA...": memory's pages at the
 * physical addresses PA (a GPU object in GTT, as Linux maps those),
 * mapped cached: marked by the low bit (fault.c)
 */
static void
pagerpages(Pager *pg, char **f, int n, int ram)
{
	uintptr off;
	ulong i, j;

	i = strtoul(f[0], 0, 0);
	if(i >= pg->npage || n-1 > pg->npage - i)
		error(Ebadarg);
	for(j = 1; j < n; j++){
		off = strtoull(f[j], 0, 0);
		if((off & (BY2PG-1)) != 0 || (!ram && off >= pg->size))
			error(Ebadarg);
	}
	lock(pg);
	for(j = 1; j < n; j++)
		pg->pa[i+j-1] = ram ? strtoull(f[j], 0, 0) | 1 : pg->base + strtoull(f[j], 0, 0);
	pg->asked = 0;
	unlock(pg);
	wakeup(&pg->r);
}

/* "unmap": every page gone, the mappings flushed everywhere (a fault meanwhile goes around: tlbflush) */
static void
pagerunmap(Pager *pg)
{
	ulong i;

	lock(pg);
	for(i = 0; i < pg->npage; i++)
		pg->pa[i] = ~(uintptr)0;
	pg->asked = 0;
	unlock(pg);
	if(pg->s->Ref.ref > 1)
		procflushseg(pg->s);
	flushmmu();
	wakeup(&pg->r);		/* a fault waiting: it asks again */
}

static int
segmentgen(Chan *c, char*, Dirtab*, int, int s, Dir *dp)
{
	Qid q;
	Globalseg *g;
	uintptr size;

	switch(TYPE(c)) {
	case Qtopdir:
		if(s == DEVDOTDOT){
			q.vers = 0;
			q.path = PATH(0, Qtopdir);
			q.type = QTDIR;
			devdir(c, q, "#g", 0, eve, 0777, dp);
			break;
		}

		if(s >= nglobalseg)
			return -1;

		lock(&globalseglock);
		g = globalseg[s];
		if(g == nil){
			unlock(&globalseglock);
			return 0;
		}
		q.vers = 0;
		q.path = PATH(s, Qsegdir);
		q.type = QTDIR;
		kstrcpy(up->genbuf, g->name, sizeof up->genbuf);
		devdir(c, q, up->genbuf, 0, g->uid, 0777, dp);
		unlock(&globalseglock);
		break;
	case Qsegdir:
		if(s == DEVDOTDOT){
			q.vers = 0;
			q.path = PATH(0, Qtopdir);
			q.type = QTDIR;
			devdir(c, q, "#g", 0, eve, 0777, dp);
			break;
		}
		/* fall through */
	case Qctl:
	case Qdata:
	case Qpa:
		switch(s){
		case 0:
			g = getgseg(c);
			q.vers = 0;
			q.path = PATH(SEG(c), Qctl);
			q.type = QTFILE;
			devdir(c, q, "ctl", 0, g->uid, g->perm, dp);
			putgseg(g);
			break;
		case 1:
			g = getgseg(c);
			q.vers = 0;
			q.path = PATH(SEG(c), Qdata);
			q.type = QTFILE;
			if(g->Segio.s != nil)
				size = g->Segio.s->top - g->Segio.s->base;
			else
				size = 0;
			devdir(c, q, "data", size, g->uid, g->perm, dp);
			putgseg(g);
			break;
		case 2:
			g = getgseg(c);
			q.vers = 0;
			q.path = PATH(SEG(c), Qpa);
			q.type = QTFILE;
			if(g->Segio.s != nil)
				size = (g->Segio.s->top - g->Segio.s->base)/BY2PG * 8;
			else
				size = 0;
			devdir(c, q, "pa", size, eve, 0400, dp);
			putgseg(g);
			break;
		default:
			return -1;
		}
		break;
	}
	return 1;
}

static void
segmentinit(void)
{
	_globalsegattach = globalsegattach;
	_pagerpa = pagerpa;
	_pagerwait = pagerwait;
	_pagerfree = pagerfree;
}

static Chan*
segmentattach(char *spec)
{
	return devattach('g', spec);
}

static Walkqid*
segmentwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, 0, 0, segmentgen);
}

static int
segmentstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, 0, 0, segmentgen);
}

static Chan*
segmentopen(Chan *c, int omode)
{
	Globalseg *g;

	switch(TYPE(c)){
	case Qsegdir:
		omode &= ~ORCLOSE;
	case Qtopdir:
		if(omode != 0)
			error(Eisdir);
		break;
	case Qctl:
	case Qdata:
		g = getgseg(c);
		if(waserror()){
			putgseg(g);
			nexterror();
		}
		/*
		 * 9front-ng: the hostowner may open any segment's ctl and data,
		 * as it may attach any (below): a GPU driver gives segments to
		 * its clients' users and still hands their pages in (its pager:
		 * ctl's own words check for the hostowner)
		 */
		if(!iseve())
			devpermcheck(g->uid, g->perm, omode);
		if(TYPE(c) == Qdata && g->q != nil){
			/* a pager queue's data: the faults, for the hostowner (its driver) */
			if(!iseve() || openmode(omode) != OREAD)
				error(Eperm);
			lock(g->q);
			g->q->readers++;
			unlock(g->q);
			c->aux = g;
			poperror();
			break;
		}
		if(TYPE(c) == Qdata && g->Segio.s == nil)
			error("segment not yet allocated");
		c->aux = g;
		poperror();
		break;
	case Qpa:
		/* the pages' addresses are for DMA: only for the hostowner */
		if(!iseve())
			error(Eperm);
		if(omode != OREAD)
			error(Eperm);
		g = getgseg(c);
		if(waserror()){
			putgseg(g);
			nexterror();
		}
		if(g->Segio.s == nil)
			error("segment not yet allocated");
		switch(g->Segio.s->type&SG_TYPE){
		case SG_FIXED:
		case SG_STICKY:
			break;
		default:
			error("segment pages not resident");
		}
		c->aux = g;
		poperror();
		break;
	default:
		panic("segmentopen");
	}
	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;

	return c;
}

static void
segmentremove(Chan *c)
{
	Globalseg *g;
	int x;

	if(TYPE(c) != Qsegdir)
		error(Eperm);
	lock(&globalseglock);
	x = SEG(c);
	g = globalseg[x];
	if(g != nil && strcmp(g->uid, up->user) && !iseve()){
		unlock(&globalseglock);
		error(Eperm);
	}
	globalseg[x] = nil;
	unlock(&globalseglock);
	if(g != nil){
		/* 9front-ng: an alias segment's memory is its owner's again: revoke it */
		if(g->alias && g->Segio.s != nil)
			segrevoke(g->Segio.s);
		/* a pager's faults fail from now on, a queue's too */
		if(g->Segio.s != nil && (g->Segio.s->type&SG_TYPE) == SG_PHYSICAL && g->Segio.s->pseg->pager != nil){
			Pager *pg = g->Segio.s->pseg->pager;

			lock(pg);
			pg->dead = 1;
			unlock(pg);
			wakeup(&pg->r);
		}
		if(g->q != nil)
			pagerqdead(g->q);
		putgseg(g);
	}
}

static void
segmentclose(Chan *c)
{
	if(c->flag & COPEN){
		switch(TYPE(c)){
		case Qsegdir:
			if(c->flag & CRCLOSE)
				segmentremove(c);
			break;
		case Qdata:
			/* 9front-ng: a pager queue's last reader gone (its driver): its faults fail */
			if(((Globalseg*)c->aux)->q != nil){
				Pagerq *q = ((Globalseg*)c->aux)->q;
				int last;

				lock(q);
				last = --q->readers == 0;
				unlock(q);
				if(last)
					pagerqdead(q);
			}
			/* fall through */
		case Qctl:
		case Qpa:
			putgseg(c->aux);
			c->aux = nil;
			break;
		}
	}
}

static Chan*
segmentcreate(Chan *c, char *name, int omode, ulong perm)
{
	int x, xfree;
	Globalseg *g;

	if(TYPE(c) != Qtopdir)
		error(Eperm);

	if(strcmp("none", up->user) == 0)
		error(Eperm);

	if(strlen(name) >= sizeof(up->genbuf))
		error(Etoolong);

	if(findphysseg(name) != nil)
		error("name collision with physical segment");

	if((perm & DMDIR) == 0)
		error(Ebadarg);

	g = smalloc(sizeof(Globalseg));
	g->Ref.ref = 1;
	g->perm = perm & 0777;	/* honor the caller's requested bits (e.g. +x, needed to exec() a segment) instead of a hardcoded 0660 */
	kstrdup(&g->name, name);
	kstrdup(&g->uid, up->user);

	for(;;){
		Globalseg **t, **o;
		int n;

		lock(&globalseglock);
		if(waserror()){
			unlock(&globalseglock);
			putgseg(g);
			nexterror();
		}
		xfree = -1;
		for(x = 0; x < nglobalseg; x++){
			if(globalseg[x] == nil){
				if(xfree < 0)
					xfree = x;
			} else if(strcmp(globalseg[x]->name, name) == 0)
				error(Eexist);
		}
		if(xfree >= 0){
			globalseg[xfree] = g;
			unlock(&globalseglock);
			poperror();
			break;
		}
		/* 9front-ng: full: a table twice the size, made without the lock (above) */
		n = nglobalseg ? 2*nglobalseg : Nglobal0;
		if(n > Maxglobal)
			error("too many global segments");
		unlock(&globalseglock);
		poperror();
		t = mallocz(n*sizeof(Globalseg*), 1);
		if(t == nil){
			putgseg(g);
			error(Enomem);
		}
		lock(&globalseglock);
		o = nil;
		if(nglobalseg < n){
			memmove(t, globalseg, nglobalseg*sizeof(Globalseg*));
			o = globalseg;
			globalseg = t;
			nglobalseg = n;
		} else
			o = t;	/* another grew it meanwhile */
		unlock(&globalseglock);
		free(o);
	}

	c->qid.path = PATH(xfree, Qsegdir);
	c->qid.type = QTDIR;
	c->qid.vers = 0;
	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;

	return c;
}

/*
 * the physical addresses of the pages of a fixed or sticky segment
 * (never paged out, never moved while the segment lives), one
 * little-endian 64-bit word per page from the segment's base
 */
static long
pagepa(Segment *s, uchar *a, long n, vlong voff)
{
	uchar buf[256];
	uintptr soff, npg;
	uvlong pa;
	Pte *pte;
	Page *pg;
	long i, m;
	int j;

	if(voff < 0 || (voff & 7) != 0)
		error(Ebadarg);
	npg = (s->top - s->base)/BY2PG;
	n &= ~7;
	for(i = 0; i < n && voff/8 < npg; i += m){
		/* not into a while holding the lock: a may lie in s */
		qlock(&s->QLock);
		for(m = 0; m < sizeof buf && i+m < n && voff/8 < npg; m += 8, voff += 8){
			soff = (voff/8)*BY2PG;
			pte = s->map[soff/PTEMAPMEM];
			pg = pte != nil ? pte->pages[(soff&(PTEMAPMEM-1))/BY2PG] : nil;
			if(pg == nil){
				qunlock(&s->QLock);
				error("segment page missing");
			}
			pa = pg->pa;
			for(j = 0; j < 8; j++)
				buf[m+j] = pa >> 8*j;
		}
		qunlock(&s->QLock);
		memmove(a+i, buf, m);
	}
	return i;
}

/*
 * 9front-ng: alias segments.  "va VA LEN alias" makes a sticky segment
 * without pages; "page IDX SRC OFF..." puts there, from page IDX on,
 * the pages at the byte offsets OFF of the sticky segment SRC: the same
 * memory seen elsewhere, contiguous (a GPU driver's vmap of scattered
 * pages, an object's pages for a client).  The pages stay while either
 * segment has them.
 */
static void
aliaspages(Segment *s, char *src, uintptr idx, char **offs, int n)
{
	Globalseg *g;
	Segment *ss;
	Page *pg, **pp;
	Pte *pte;
	uintptr off, va;
	int i, x;

	g = nil;
	lock(&globalseglock);
	for(x = 0; x < nglobalseg; x++)
		if(globalseg[x] != nil && strcmp(globalseg[x]->name, src) == 0){
			g = globalseg[x];
			incref(&g->Ref);
			break;
		}
	unlock(&globalseglock);
	if(g == nil)
		error("no such segment");
	if(waserror()){
		putgseg(g);
		nexterror();
	}
	ss = g->Segio.s;
	if(ss == nil || (ss->type&SG_TYPE) != SG_STICKY)
		error("not a sticky segment");
	for(i = 0; i < n; i++){
		off = strtoull(offs[i], 0, 0);
		va = s->base + (idx+i)*BY2PG;
		if((off & (BY2PG-1)) != 0 || off >= ss->top - ss->base || va < s->base || va >= s->top)
			error(Ebadarg);
		qlock(&ss->QLock);
		pte = ss->map[off/PTEMAPMEM];
		pg = pte != nil ? pte->pages[(off&(PTEMAPMEM-1))/BY2PG] : nil;
		if(pg == nil){
			qunlock(&ss->QLock);
			error("segment page missing");
		}
		incref(&pg->Ref);
		qunlock(&ss->QLock);
		qlock(&s->QLock);
		if((pp = segmap(s, va)) == nil || *pp != nil){
			qunlock(&s->QLock);
			putpage(pg);
			error("alias page taken");
		}
		*pp = pg;
		s->used++;
		qunlock(&s->QLock);
	}
	poperror();
	putgseg(g);
}

static long
segmentread(Chan *c, void *a, long n, vlong voff)
{
	Globalseg *g;
	char buf[128];

	if(c->qid.type == QTDIR)
		return devdirread(c, a, n, (Dirtab *)0, 0L, segmentgen);

	g = c->aux;
	switch(TYPE(c)){
	case Qctl:
		if(g->q != nil)
			return readstr(voff, a, n, g->q->dead ? "pager dead\n" : "pager\n");
		if(g->Segio.s == nil)
			error("segment not yet allocated");
		if((g->Segio.s->type&SG_TYPE) == SG_FIXED)
			snprint(buf, sizeof(buf), "va %#p %#p fixed %#p\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base,
				g->Segio.s->map[0]->pages[0]->pa);
		else if((g->Segio.s->type&SG_TYPE) == SG_STICKY)
			snprint(buf, sizeof(buf), "va %#p %#p sticky\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		else if((g->Segio.s->type&SG_TYPE) == SG_PHYSICAL && g->Segio.s->pseg->pager != nil){
			Pager *pg = g->Segio.s->pseg->pager;
			ulong i, have;

			/* 9front-ng: a pager segment, its pages there, a fault waiting for the driver */
			lock(pg);
			for(have = i = 0; i < pg->npage; i++)
				have += pg->pa[i] != ~(uintptr)0;
			snprint(buf, sizeof(buf), "va %#p %#p phys %s pager %s: %lud of %lud pages%s%s%s\n",
				g->Segio.s->base, g->Segio.s->top-g->Segio.s->base, g->Segio.s->pseg->name,
				pg->q->dead ? "(queue gone)" : "", have, pg->npage, pg->asked ? ", asked" : "",
				pg->dead ? ", dead" : "", (g->Segio.s->type&SG_FAULT) ? " revoked" : "");
			unlock(pg);
		} else if((g->Segio.s->type&SG_TYPE) == SG_PHYSICAL)
			snprint(buf, sizeof(buf), "va %#p %#p phys %s %#p%s\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base,
				g->Segio.s->pseg->name, g->Segio.s->pseg->pa, (g->Segio.s->type&SG_FAULT) ? " revoked" : "");
		else
			snprint(buf, sizeof(buf), "va %#p %#p\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		return readstr(voff, a, n, buf);
	case Qdata:
		if(g->q != nil)
			return pagerqread(g->q, a, n);
		return segio(&g->Segio,g->Segio.s,a,n,voff,1);
	case Qpa:
		return pagepa(g->Segio.s, a, n, voff);
	default:
		panic("segmentread");
	}
}

static long
segmentwrite(Chan *c, void *a, long n, vlong voff)
{
	Cmdbuf *cb;
	Globalseg *g;
	uintptr va, top, len;

	if(c->qid.type == QTDIR)
		error(Eperm);

	g = c->aux;
	switch(TYPE(c)){
	case Qctl:
		cb = parsecmd(a, n);
		if(waserror()){
			free(cb);
			nexterror();
		}
		if(cb->nf > 0 && strcmp(cb->f[0], "va") == 0){
			if(g->Segio.s != nil)
				error("already has a virtual address");
			if(cb->nf < 3)
				error(Ebadarg);
			va = strtoull(cb->f[1], 0, 0);
			len = strtoull(cb->f[2], 0, 0);
			if(va == 0 && cb->nf == 3){
				/*
				 * 9front-ng: the kernel chooses: a range of the
				 * shared region no other such segment has
				 */
				if(len == 0 || len > SEGMAXSIZE)
					error(Ebadarg);
				len = PGROUND(len);
				if((va = srangealloc(len)) == 0)
					error(Enovmem);
				if(waserror()){
					srangeset(va, nil);
					nexterror();
				}
				g->Segio.s = newseg(SG_SHARED, va, len/BY2PG);
				poperror();
				srangeset(va, g->Segio.s);
				free(cb);
				poperror();
				return n;
			}
			top = PGROUND(va + len);
			va = va&~(BY2PG-1);
			if(va == 0 || top > USTKTOP || top <= va)
				error(Ebadarg);
			/*
			 * 9front-ng: the shared region is the kernel's to give
			 * ("va 0 len"): its addresses stay unique
			 */
			if(va < SHAREDTOP && top > SHAREDBASE)
				error("in the shared region");
			len = top - va;
			if(len > SEGMAXSIZE)
				error(Enovmem);
			if(cb->nf >= 4 && strcmp(cb->f[3], "fixed") == 0){
				if(!iseve())
					error(Eperm);
				g->Segio.s = fixedseg(va, len/BY2PG, SG_FIXED, BY2PG);
			} else if(cb->nf >= 5 && strcmp(cb->f[3], "sticky") == 0 && strcmp(cb->f[4], "contig") == 0){
				uintptr align;

				/*
				 * 9front-ng: sticky, physically contiguous and aligned
				 * to its size (up to 2 MB), as a buddy allocator's
				 * blocks are: a GPU driver's pool of pages, whose
				 * higher orders must be so (Linux's alloc_pages)
				 */
				if(!iseve())
					error(Eperm);
				for(align = BY2PG; align < len && align < 2*1024*1024; align <<= 1)
					;
				g->Segio.s = fixedseg(va, len/BY2PG, SG_STICKY, align);
			} else if(cb->nf >= 4 && strcmp(cb->f[3], "sticky") == 0){
				Segment *s;

				if(!iseve())
					error(Eperm);
				s = newseg(SG_STICKY, va, len/BY2PG);
				if(waserror()){
					putseg(s);
					nexterror();
				}
				for(; va < s->top; va += BY2PG)
					segpage(s, fillpage(newpage(va, nil), 0));
				poperror();
				g->Segio.s = s;
			} else if(cb->nf == 7 && strcmp(cb->f[3], "phys") == 0 && strcmp(cb->f[5], "pager") == 0){
				Physseg *ps, *pp;
				Segment *s;
				Pager *pg;
				Pagerq *q;
				ulong i;

				/* 9front-ng: a pager segment (above), no pages yet */
				if(!iseve())
					error(Eperm);
				if((ps = findphysseg(cb->f[4])) == nil)
					error("no such physical segment");
				q = findpagerq(cb->f[6]);
				pg = malloc(sizeof(Pager));
				if(pg != nil)
					pg->pa = malloc((len/BY2PG)*sizeof(uintptr));
				if(pg == nil || pg->pa == nil){
					free(pg);
					putpagerq(q);
					error(Enomem);
				}
				pg->npage = len/BY2PG;
				for(i = 0; i < pg->npage; i++)
					pg->pa[i] = ~(uintptr)0;
				pg->q = q;
				pg->base = ps->pa;
				pg->size = ps->size;
				kstrdup(&pg->name, g->name);
				if(waserror()){
					pagerfree(pg);
					nexterror();
				}
				s = newseg(ps->attr, va, len/BY2PG);
				poperror();
				pp = (Physseg*)(s+1);
				*pp = *ps;
				pp->size = len;
				pp->pager = pg;
				pg->s = s;
				s->pseg = pp;
				g->Segio.s = s;
				g->alias = 1;
			} else if(cb->nf >= 6 && strcmp(cb->f[3], "phys") == 0){
				Physseg *ps, *pp;
				uvlong off;
				Segment *s;

				/*
				 * 9front-ng: a part of a physical segment (a GPU
				 * object in a BAR) for a client, revoked as an
				 * alias segment is
				 */
				if(!iseve())
					error(Eperm);
				if((ps = findphysseg(cb->f[4])) == nil)
					error("no such physical segment");
				off = strtoull(cb->f[5], 0, 0);
				if((off & (BY2PG-1)) != 0 || off > ps->size || len > ps->size - off)
					error(Ebadarg);
				s = newseg(ps->attr, va, len/BY2PG);
				pp = (Physseg*)(s+1);
				*pp = *ps;
				pp->pa = ps->pa + off;
				pp->size = len;
				s->pseg = pp;
				g->Segio.s = s;
				g->alias = 1;
			} else if(cb->nf >= 4 && strcmp(cb->f[3], "alias") == 0){
				if(!iseve())
					error(Eperm);
				g->Segio.s = newseg(SG_STICKY, va, len/BY2PG);
				g->alias = 1;
			} else
				g->Segio.s = newseg(SG_SHARED, va, len/BY2PG);
		} else if(cb->nf == 1 && strcmp(cb->f[0], "pager") == 0){
			/* 9front-ng: a pager queue (above) */
			if(!iseve())
				error(Eperm);
			if(g->Segio.s != nil || g->q != nil)
				error("already a segment");
			g->q = smalloc(sizeof(Pagerq));
			g->q->Ref.ref = 1;
		} else if(cb->nf >= 2 && (strcmp(cb->f[0], "pages") == 0 || strcmp(cb->f[0], "ram") == 0)){
			if(!iseve())
				error(Eperm);
			pagerpages(pagerof(g), cb->f+1, cb->nf-1, cb->f[0][0] == 'r');
		} else if(cb->nf == 1 && strcmp(cb->f[0], "unmap") == 0){
			if(!iseve())
				error(Eperm);
			pagerunmap(pagerof(g));
		} else if(cb->nf == 1 && strcmp(cb->f[0], "nopage") == 0){
			Pager *pg;

			if(!iseve())
				error(Eperm);
			pg = pagerof(g);
			lock(pg);
			pg->nopage++;
			pg->asked = 0;
			unlock(pg);
			wakeup(&pg->r);
		} else if(cb->nf == 1 && strcmp(cb->f[0], "revoke") == 0){
			/* the memory went back to its owner: nobody keeps seeing it */
			if(g->Segio.s == nil || !g->alias)
				error("not an alias segment");
			if(!iseve())
				error(Eperm);
			segrevoke(g->Segio.s);
		} else if(cb->nf >= 4 && strcmp(cb->f[0], "page") == 0){
			if(g->Segio.s == nil || !g->alias || (g->Segio.s->type&SG_TYPE) != SG_STICKY)
				error("not an alias segment");
			if(!iseve())
				error(Eperm);
			aliaspages(g->Segio.s, cb->f[2], strtoull(cb->f[1], 0, 0), cb->f+3, cb->nf-3);
		} else
			error(Ebadctl);
		free(cb);
		poperror();
		return n;
	case Qdata:
		return segio(&g->Segio,g->Segio.s,a,n,voff,0);
	default:
		panic("segmentwrite");
	}
}

static int
segmentwstat(Chan *c, uchar *dp, int n)
{
	Globalseg *g;
	Dir *d;

	if(c->qid.type == QTDIR)
		error(Eperm);

	g = getgseg(c);
	if(waserror()){
		putgseg(g);
		nexterror();
	}
	if(strcmp(g->uid, up->user) && !iseve())
		error(Eperm);
	d = smalloc(sizeof(Dir)+n);
	if(waserror()){
		free(d);
		nexterror();
	}
	n = convM2D(dp, n, &d[0], (char*)&d[1]);
	if(n == 0)
		error(Eshortstat);
	if(!emptystr(d->uid))
		kstrdup(&g->uid, d->uid);
	if(d->mode != (ulong)~0UL)	/* GCC port: ~0UL is 64-bit, ulong is 32-bit */
		g->perm = d->mode&0777;
	free(d);
	poperror();
	putgseg(g);
	poperror();

	return n;
}

/*
 *  called by segattach()
 */
static Segment*
globalsegattach(char *name)
{
	int x;
	Globalseg *g;
	Segment *s;

	lock(&globalseglock);
	if(waserror()){
		unlock(&globalseglock);
		nexterror();
	}
	for(x = 0; x < nglobalseg; x++){
		g = globalseg[x];
		if(g != nil && strcmp(g->name, name) == 0)
			goto Found;
	}
	unlock(&globalseglock);
	poperror();
	return nil;
Found:
	/*
	 * 9front-ng: the hostowner may attach any (it may wstat any to
	 * itself anyway): a GPU driver keeps the segments it gave to its
	 * clients' users, its other procs attach them when they touch them
	 */
	if(!iseve())
		devpermcheck(g->uid, g->perm, ORDWR);
	s = g->Segio.s;
	if(s == nil)
		error("global segment not assigned a virtual address");
	incref(&s->Ref);
	unlock(&globalseglock);
	poperror();
	return s;
}

/*
 * allocate a fixed segment with sequential run of of adjacent
 * user memory pages.  9front-ng: or a sticky one (type), the run's
 * physical start aligned to align
 */
static Segment*
fixedseg(uintptr va, ulong len, int type, uintptr align)
{
	Segment *s;
	Page **f, *p, *l, *h, *t;
	ulong n, i;
	int color;

	s = newseg(type, va, len);
	if(waserror()){
		putseg(s);
		nexterror();
	}

	lock(&palloc.Lock);
	i = 0;
	l = palloc.pages;
	color = getpgcolor(va);
	for(n = palloc.user; n >= len; n--, l++){
		/*
		 * a used page ends a run: with the ?: on its own (it binds
		 * weaker than ||) a used page after its predecessor counted,
		 * and every such run made a walk of the whole free list
		 * before the retry -- seconds with gigabytes of memory
		 */
		if(l->Ref.ref != 0 || l->image != nil
		|| (i != 0 ? (l[-1].pa+BY2PG) != l->pa : l->color != color || (l->pa & (align-1)) != 0)){
		Retry:
			i = 0;
			continue;
		}
		if(++i < len)
			continue;

		i = 0;
		h = t = nil;
		f = &palloc.head;
		while((p = *f) != nil){
			if(p > l-len && p <= l){
				*f = p->next;
				if((p->next = h) == nil)
					t = p;
				h = p;
				if(++i < len)
					continue;
				break;
			}
			f = &p->next;
		}
		if(i != len){
			if(h != nil){
				t->next = palloc.head;
				palloc.head = h;
			}
			goto Retry;
		}
		palloc.freecount -= i;
		unlock(&palloc.Lock);

		p = l-len;
		do {
			p++;
			p->Ref.ref = 1;
			p->va = va;
			va += BY2PG;
			p->modref = 0;
			if(waserror()){
				while(++p <= l)
					freepages(p, p, 1);
				nexterror();
			}
			segpage(s, fillpage(p, 0));
			poperror();
		} while(p != l);

		poperror();
		return s;
	}
	unlock(&palloc.Lock);

	error(Enomem);
}

Dev segmentdevtab = {
	'g',
	"segment",

	devreset,
	segmentinit,
	devshutdown,
	segmentattach,
	segmentwalk,
	segmentstat,
	segmentopen,
	segmentcreate,
	segmentclose,
	segmentread,
	devbread,
	segmentwrite,
	devbwrite,
	segmentremove,
	segmentwstat,
};

