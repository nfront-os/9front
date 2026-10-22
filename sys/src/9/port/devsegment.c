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
#define SEG(c)	 	( ((c)->qid.path >> 3) & 0x3f )
#define PATH(s, t) 	( ((s)<<3) | (t) )

typedef struct Globalseg Globalseg;
struct Globalseg
{
	Ref;

	char	*name;
	char	*uid;
	vlong	length;
	long	perm;

	Segio;
};

static Globalseg *globalseg[100];
static Lock globalseglock;


	Segment* (*_globalsegattach)(char*);
static	Segment* globalsegattach(char *name);

static	Segment* fixedseg(uintptr va, ulong len);

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
	if(x >= nelem(globalseg))
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
	segio(&g->Segio,nil,nil,0,0,0);
	free(g->name);
	free(g->uid);
	free(g);
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

		if(s >= nelem(globalseg))
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
		devpermcheck(g->uid, g->perm, omode);
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
	if(g != nil)
		putgseg(g);
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
		case Qctl:
		case Qdata:
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

	lock(&globalseglock);
	if(waserror()){
		unlock(&globalseglock);
		putgseg(g);
		nexterror();
	}
	xfree = -1;
	for(x = 0; x < nelem(globalseg); x++){
		if(globalseg[x] == nil){
			if(xfree < 0)
				xfree = x;
		} else if(strcmp(globalseg[x]->name, name) == 0)
			error(Eexist);
	}
	if(xfree < 0)
		error("too many global segments");
	globalseg[xfree] = g;
	unlock(&globalseglock);
	poperror();

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
		if(g->Segio.s == nil)
			error("segment not yet allocated");
		if((g->Segio.s->type&SG_TYPE) == SG_FIXED)
			snprint(buf, sizeof(buf), "va %#p %#p fixed %#p\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base,
				g->Segio.s->map[0]->pages[0]->pa);
		else if((g->Segio.s->type&SG_TYPE) == SG_STICKY)
			snprint(buf, sizeof(buf), "va %#p %#p sticky\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		else
			snprint(buf, sizeof(buf), "va %#p %#p\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		return readstr(voff, a, n, buf);
	case Qdata:
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
			top = PGROUND(va + len);
			va = va&~(BY2PG-1);
			if(va == 0 || top > USTKTOP || top <= va)
				error(Ebadarg);
			len = top - va;
			if(len > SEGMAXSIZE)
				error(Enovmem);
			if(cb->nf >= 4 && strcmp(cb->f[3], "fixed") == 0){
				if(!iseve())
					error(Eperm);
				g->Segio.s = fixedseg(va, len/BY2PG);
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
			} else
				g->Segio.s = newseg(SG_SHARED, va, len/BY2PG);
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
	for(x = 0; x < nelem(globalseg); x++){
		g = globalseg[x];
		if(g != nil && strcmp(g->name, name) == 0)
			goto Found;
	}
	unlock(&globalseglock);
	poperror();
	return nil;
Found:
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
 * user memory pages.
 */
static Segment*
fixedseg(uintptr va, ulong len)
{
	Segment *s;
	Page **f, *p, *l, *h, *t;
	ulong n, i;
	int color;

	s = newseg(SG_FIXED, va, len);
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
		|| (i != 0 ? (l[-1].pa+BY2PG) != l->pa : l->color != color)){
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

