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
	Qfree,		/* NIX: free buffers of zero-copy segments */
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

/* NIX: zero-copy segments (kmsg, umsg) and their free file, after NIX's port/devsegment.c */
static	uintptr	placeseg(uintptr len);
static	long	zfreeread(Segment*, char*, long);
static	void	zfreewrite(Segment*, char*, long);

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
	case Qfree:	/* NIX */
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
		case 2:	/* NIX: only for zero-copy segments */
			g = getgseg(c);
			if(g->Segio.s == nil || (g->Segio.s->type&SG_ZIO) == 0){
				putgseg(g);
				return -1;
			}
			q.vers = 0;
			q.path = PATH(SEG(c), Qfree);
			q.type = QTFILE;
			devdir(c, q, "free", 0, g->uid, g->perm, dp);
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
	case Qfree:	/* NIX */
		g = getgseg(c);
		if(waserror()){
			putgseg(g);
			nexterror();
		}
		devpermcheck(g->uid, g->perm, omode);
		if(TYPE(c) == Qdata && g->Segio.s == nil)
			error("segment not yet allocated");
		if(TYPE(c) == Qfree && (g->Segio.s == nil || (g->Segio.s->type&SG_ZIO) == 0))
			error("not a zero-copy segment");	/* NIX */
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
		case Qfree:	/* NIX */
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
		else if(g->Segio.s->type&SG_ZIO)	/* NIX */
			snprint(buf, sizeof(buf), "%s %#p %#p\n", (g->Segio.s->type&SG_KZIO)? "kmsg": "umsg",
				g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		else
			snprint(buf, sizeof(buf), "va %#p %#p\n", g->Segio.s->base, g->Segio.s->top-g->Segio.s->base);
		return readstr(voff, a, n, buf);
	case Qdata:
		return segio(&g->Segio,g->Segio.s,a,n,voff,1);
	case Qfree:	/* NIX */
		return zfreeread(g->Segio.s, a, n);
	default:
		panic("segmentread");
	}
}

/*
 * NIX: segment types of the ctl messages kmsg (kernel zero-copy buffers),
 * umsg (user zero-copy buffers) and addr (shared, as va)
 */
static int
zsegtype(char *name)
{
	if(strcmp(name, "kmsg") == 0)
		return SG_SHARED|SG_ZIO|SG_KZIO;
	if(strcmp(name, "umsg") == 0)
		return SG_SHARED|SG_ZIO;
	if(strcmp(name, "addr") == 0)
		return SG_SHARED;
	return -1;
}

/*
 * NIX: a system-wide address for a segment, from the middle
 * of the user address space down, with a guard page above.
 * BUG: we allocate virtual addresses but never give them
 * back when the segment is destroyed.
 */
static uintptr
placeseg(uintptr len)
{
	static Lock lck;
	static uintptr va;
	uintptr v;

	len = PGROUND(len) + BY2PG;
	lock(&lck);
	if(va == 0)
		va = ((USTKTOP-USTKSIZE)/2) & ~(uintptr)(BY2PG-1);
	if(len >= va/2){
		unlock(&lck);
		error(Enovmem);
	}
	va -= len;
	v = va;
	unlock(&lck);
	return v;
}

enum{
	PTRSIZE = 19,	/* "0x1234567812345678 " */
	Nfree = 32,	/* addresses per read of free */
};

static int
znotempty(void *x)
{
	return ((Zseg*)x)->end != 0;
}

/*
 * NIX: read of free: wait for buffers given back to the umsg
 * segment s, return their addresses, PTRSIZE bytes each.
 */
static long
zfreeread(Segment *s, char *a, long n)
{
	uintptr va[Nfree];
	char buf[PTRSIZE+1];
	int i, nva;

	if(n < PTRSIZE)
		error("read buffer too small");
	if(s->type&SG_KZIO)
		error("the kernel frees kmsg buffers");
	for(;;){
		qlock(&s->QLock);
		if((va[0] = zgetaddr(s)) != 0)
			break;
		qunlock(&s->QLock);
		sleep(&s->zseg.rr, znotempty, &s->zseg);
	}
	for(nva = 1; nva < Nfree && (nva+1)*PTRSIZE <= n; nva++)
		if((va[nva] = zgetaddr(s)) == 0)
			break;
	qunlock(&s->QLock);
	/* a may be in s: touch it unlocked */
	for(i = 0; i < nva; i++){
		snprint(buf, sizeof buf, "0x%016llux ", (uvlong)va[i]);
		memmove(a+i*PTRSIZE, buf, PTRSIZE);
	}
	return nva*PTRSIZE;
}

/*
 * NIX: write of free: give the buffers at the addresses back.
 */
static void
zfreewrite(Segment *s, char *a, long n)
{
	char *buf, *p, *e;
	uintptr va;
	int r;

	if(n > 64*1024)
		error(Etoobig);
	buf = smalloc(n+1);
	if(waserror()){
		free(buf);
		nexterror();
	}
	memmove(buf, a, n);
	buf[n] = 0;
	for(p = buf;; p = e){
		while(*p == ' ' || *p == '\t' || *p == '\n')
			p++;
		if(*p == 0)
			break;
		va = strtoull(p, &e, 0);
		if(e == p)
			error(Ebadarg);
		qlock(&s->QLock);
		r = zputaddr(s, va);
		qunlock(&s->QLock);
		if(r < 0)
			error("not a zero-copy buffer");
	}
	free(buf);
	poperror();
}

static long
segmentwrite(Chan *c, void *a, long n, vlong voff)
{
	Cmdbuf *cb;
	Globalseg *g;
	uintptr va, top, len;
	int type;

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
		} else if(cb->nf > 0 && (type = zsegtype(cb->f[0])) != -1){
			/* NIX: "kmsg|umsg|addr va len", va 0: the system picks one */
			if(g->Segio.s != nil)
				error("already has a virtual address");
			if(cb->nf < 3)
				error(Ebadarg);
			va = strtoull(cb->f[1], 0, 0);
			len = strtoull(cb->f[2], 0, 0);
			if(len == 0 || len > SEGMAXSIZE)
				error(Ebadarg);
			if(va == 0)
				va = placeseg(len);
			top = PGROUND(va + len);
			va = va&~(BY2PG-1);
			if(va == 0 || top > USTKTOP || top <= va)
				error(Ebadarg);
			len = top - va;
			if(len > SEGMAXSIZE)
				error(Enovmem);
			g->Segio.s = newseg(type, va, len/BY2PG);
			if(type & SG_KZIO)
				newzmap(g->Segio.s);
		} else
			error(Ebadctl);
		free(cb);
		poperror();
		return n;
	case Qdata:
		return segio(&g->Segio,g->Segio.s,a,n,voff,0);
	case Qfree:	/* NIX */
		zfreewrite(g->Segio.s, a, n);
		return n;
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
		if(l->Ref.ref != 0 || l->image != nil
		|| i != 0 ? (l[-1].pa+BY2PG) != l->pa : l->color != color){
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

