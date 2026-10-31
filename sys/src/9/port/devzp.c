/*
 * zp(3): zero-copy pipes, after the NIX kernel (Lsub, Bell Labs,
 * Sandia, Vita Nuova; port/devzp.c; MIT license, see /lib/legal/nix).
 *
 * Like pipe(3), but the queues hold buffer descriptors (Kzio),
 * not data: zwrite queues the writer's buffers, zread hands them
 * to the reader (zread(2), port/syszio.c). read and write copy
 * through a buffer of the caller's kmsg segment (segment(3)).
 *
 * Deviations from NIX: data1 read the queue of data; write
 * repeated its first 64K for a longer buffer; the reader slept
 * holding a spin lock and an interrupted read unlocked it twice;
 * writers wait when Qmax buffers are queued (NIX had no flow
 * control); queued buffers are freed when the pipe is; writing
 * a closed pipe posts a note as pipe(3).
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

enum
{
	Incr = 16,
	Maxatomic = 64*1024,
	Qmax = 4096,		/* buffers queued before writers wait */
};

typedef struct ZPipe	ZPipe;
typedef struct Zq	Zq;

struct Zq
{
	Lock;			/* to protect Zq */
	QLock	rlck;		/* one reader at a time */
	Kzio*	io;		/* io[] */
	Kzio*	ep;		/* end pointer */
	int	closed;		/* queue is closed */
	int	waiting;	/* reader is waiting */
	Kzio*	rp;		/* read pointer */
	Kzio*	wp;		/* write pointer */
	Rendez	rr;		/* reader rendez */
	Rendez	wr;		/* writers wait here for room */
};

struct ZPipe
{
	QLock;
	ZPipe	*next;
	int	ref;
	ulong	path;
	Zq	q[2];
	int	qref[2];
};

static struct
{
	Lock;
	ulong	path;
} zpalloc;

enum
{
	Qdir,
	Qdata0,
	Qdata1,
};

static Dirtab zpdir[] =
{
	".",		{Qdir,0,QTDIR},	0,		DMDIR|0500,
	"data",		{Qdata0},	0,		0600,
	"data1",	{Qdata1},	0,		0600,
};
#define NZPDIR 3

#define ZPTYPE(x)	(((unsigned)x)&0x1f)
#define ZPID(x)	((((unsigned)x))>>5)
#define ZPQID(i, t)	((((unsigned)i)<<5)|(t))
#define ZQLEN(q)	((q)->wp - (q)->rp)

static int
zqnotempty(void *x)
{
	Zq *q;

	q = x;
	return ZQLEN(q) != 0 || q->closed != 0;
}

static int
zqnotfull(void *x)
{
	Zq *q;

	q = x;
	return ZQLEN(q) < Qmax || q->closed != 0;
}

/*
 * Take up to nio buffers, at most count bytes, from q.
 * If the first one is larger than count, its rest is copied
 * into a new buffer of the reader's kmsg segment that
 * stays queued.
 * BUG: such a copy might not be needed, but we have
 * no way to reference the same buffer more than once.
 */
static int
zqread(Zq *q, Kzio io[], int nio, usize count)
{
	int i;
	usize tot, nr;
	Kzio qio;
	Segment *s;
	char *p, *data;

	qlock(&q->rlck);
	if(waserror()){
		qunlock(&q->rlck);
		nexterror();
	}
	lock(q);
	while(q->closed == 0 && ZQLEN(q) == 0){
		q->waiting++;
		unlock(q);
		sleep(&q->rr, zqnotempty, q);
		lock(q);
	}
	i = 0;
	for(tot = 0; ZQLEN(q) > 0 && i < nio && tot < count; tot += nr){
		qio = *q->rp;
		nr = qio.size;
		if(tot + nr > count){
			if(i > 0)
				break;
			/* only we take from q (rlck), writers just add */
			unlock(q);
			nr = count - tot;
			s = getzkseg();
			if(s == nil)
				error("no kernel segment for zero-copy");
			if(waserror()){
				putseg(s);
				nexterror();
			}
			p = qio.data;
			if(!okaddr((uintptr)p+nr, qio.size-nr, 0))
				error("zero-copy buffer not in this address space");
			data = alloczio(s, qio.size-nr);
			memmove(data, p+nr, qio.size-nr);
			poperror();
			lock(q);
			q->rp->data = data;
			q->rp->size = qio.size-nr;
			q->rp->seg = s;
			io[i] = qio;
			io[i].size = nr;
			i++;
			tot += nr;
			break;
		}
		io[i++] = qio;
		q->rp++;
	}
	if(q->rp == q->wp)
		q->rp = q->wp = q->io;
	unlock(q);
	wakeup(&q->wr);
	poperror();
	qunlock(&q->rlck);
	return i;
}

static void
zqclosed(Chan *c)
{
	/* avoid notes when the pipe is a mounted queue, as pipe(3) */
	if((c->flag & CMSG) == 0)
		postnote(up, 1, "sys: write on closed pipe", NUser);
	error(Ehungup);
}

/*
 * Queue nio buffers, wait while Qmax are queued.
 */
static long
zqwrite(Chan *c, Zq *q, Kzio io[], int nio)
{
	int ei, ri, wi, awake;
	Kzio *nq;
	Proc *p;

	lock(q);
	while(q->closed == 0 && ZQLEN(q) >= Qmax){
		unlock(q);
		sleep(&q->wr, zqnotfull, q);
		lock(q);
	}
	if(q->closed){
		unlock(q);
		zqclosed(c);
	}
	if(q->wp + nio > q->ep){
		if(q->rp > q->io){
			memmove(q->io, q->rp, ZQLEN(q)*sizeof q->io[0]);
			q->wp = q->io + ZQLEN(q);
			q->rp = q->io;
		}
		if(q->wp + nio > q->ep){
			ei = q->ep - q->io;
			ei += nio + Incr;
			ri = q->rp - q->io;
			wi = q->wp - q->io;
			nq = realloc(q->io, ei*sizeof q->io[0]);
			if(nq == nil){
				unlock(q);
				error(Enomem);
			}
			q->io = nq;
			q->ep = q->io + ei;
			q->rp = q->io + ri;
			q->wp = q->io + wi;
		}
	}
	memmove(q->wp, io, nio*sizeof io[0]);
	q->wp += nio;
	awake = q->waiting;
	if(awake)
		q->waiting--;
	unlock(q);
	if(awake){
		p = wakeup(&q->rr);

		/* if we just wokeup a higher priority process, let it run */
		if(p != nil && p->priority > up->priority)
			sched();
	}
	return nio;
}

/*
 * Free the queued buffers.
 */
static void
zqflush(Zq *q)
{
	Kzio *io, *e;
	int n;

	lock(q);
	n = ZQLEN(q);
	io = nil;
	if(n > 0){
		io = malloc(n*sizeof io[0]);
		if(io != nil)
			memmove(io, q->rp, n*sizeof io[0]);
		else
			n = 0;	/* BUG: buffers lost */
	}
	q->rp = q->wp = q->io;
	unlock(q);
	for(e = io+n; io != nil && e > io; e--)
		zrelease(e-1);
	free(io);
}

static void
zqclose(Zq *q)
{
	q->closed = 1;
	zqflush(q);
	wakeup(&q->rr);
	wakeup(&q->wr);
}

static void
zqhangup(Zq *q)
{
	q->closed = 1;
	wakeup(&q->rr);
	wakeup(&q->wr);
}

static void
zqreopen(Zq *q)
{
	q->closed = 0;
}

/*
 *  create a zp, no streams are created until an open
 */
static Chan*
zpattach(char *spec)
{
	ZPipe *p;
	Chan *c;

	c = devattach(L'∏', spec);
	p = malloc(sizeof(ZPipe));
	if(p == nil)
		exhausted("memory");
	p->ref = 1;

	lock(&zpalloc);
	p->path = ++zpalloc.path;
	unlock(&zpalloc);

	mkqid(&c->qid, ZPQID(2*p->path, Qdir), 0, QTDIR);
	c->aux = p;
	c->dev = 0;
	return c;
}

static int
zpgen(Chan *c, char*, Dirtab *tab, int ntab, int i, Dir *dp)
{
	Qid q;
	int len;
	ZPipe *p;

	if(i == DEVDOTDOT){
		devdir(c, c->qid, "#∏", 0, eve, DMDIR|0555, dp);
		return 1;
	}
	i++;	/* skip . */
	if(tab==0 || i>=ntab)
		return -1;

	tab += i;
	p = c->aux;
	switch((ulong)tab->qid.path){
	case Qdata0:
		len = ZQLEN(&p->q[0]);
		break;
	case Qdata1:
		len = ZQLEN(&p->q[1]);
		break;
	default:
		len = tab->length;
		break;
	}
	mkqid(&q, ZPQID(ZPID(c->qid.path), tab->qid.path), 0, QTFILE);
	devdir(c, q, tab->name, len, eve, tab->perm, dp);
	return 1;
}

static Walkqid*
zpwalk(Chan *c, Chan *nc, char **name, int nname)
{
	Walkqid *wq;
	ZPipe *p;

	wq = devwalk(c, nc, name, nname, zpdir, NZPDIR, zpgen);
	if(wq != nil && wq->clone != nil && wq->clone != c){
		p = c->aux;
		qlock(p);
		p->ref++;
		if(c->flag & COPEN){
			switch(ZPTYPE(c->qid.path)){
			case Qdata0:
				p->qref[0]++;
				break;
			case Qdata1:
				p->qref[1]++;
				break;
			}
		}
		qunlock(p);
	}
	return wq;
}

static int
zpstat(Chan *c, uchar *db, int n)
{
	ZPipe *p;
	Dir dir;

	p = c->aux;

	switch(ZPTYPE(c->qid.path)){
	case Qdir:
		devdir(c, c->qid, ".", 0, eve, DMDIR|0555, &dir);
		break;
	case Qdata0:
		devdir(c, c->qid, "data", ZQLEN(&p->q[0]), eve, 0600, &dir);
		break;
	case Qdata1:
		devdir(c, c->qid, "data1", ZQLEN(&p->q[1]), eve, 0600, &dir);
		break;
	default:
		panic("zpstat");
	}
	n = convD2M(&dir, db, n);
	if(n < BIT16SZ)
		error(Eshortstat);
	return n;
}

/*
 *  if the stream doesn't exist, create it
 */
static Chan*
zpopen(Chan *c, int omode)
{
	ZPipe *p;

	if(c->qid.type & QTDIR){
		if(omode != OREAD)
			error(Ebadarg);
		c->mode = omode;
		c->flag |= COPEN;
		c->offset = 0;
		return c;
	}

	p = c->aux;
	qlock(p);
	switch(ZPTYPE(c->qid.path)){
	case Qdata0:
		p->qref[0]++;
		break;
	case Qdata1:
		p->qref[1]++;
		break;
	}
	qunlock(p);

	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;
	c->iounit = Maxatomic;	/* should we care? */
	return c;
}

static void
zpclose(Chan *c)
{
	ZPipe *p;

	p = c->aux;
	qlock(p);

	if(c->flag & COPEN){
		/*
		 *  closing either side hangs up the stream
		 */
		switch(ZPTYPE(c->qid.path)){
		case Qdata0:
			p->qref[0]--;
			if(p->qref[0] == 0){
				zqhangup(&p->q[1]);
				zqclose(&p->q[0]);
			}
			break;
		case Qdata1:
			p->qref[1]--;
			if(p->qref[1] == 0){
				zqhangup(&p->q[0]);
				zqclose(&p->q[1]);
			}
			break;
		}
	}

	/*
	 *  if both sides are closed, they are reusable
	 */
	if(p->qref[0] == 0 && p->qref[1] == 0){
		zqreopen(&p->q[0]);
		zqreopen(&p->q[1]);
	}

	/*
	 *  free the structure on last close
	 */
	p->ref--;
	if(p->ref == 0){
		qunlock(p);
		zqflush(&p->q[0]);
		zqflush(&p->q[1]);
		free(p->q[0].io);
		free(p->q[1].io);
		free(p);
	} else
		qunlock(p);
}

static long
zpread(Chan *c, void *va, long n, vlong)
{
	ZPipe *p;
	Kzio io[32];	/* might read less than we could */
	int nio;

	p = c->aux;

	switch(ZPTYPE(c->qid.path)){
	case Qdir:
		return devdirread(c, va, n, zpdir, NZPDIR, zpgen);
	case Qdata0:
		nio = zqread(&p->q[0], io, nelem(io), n);
		return readzio(io, nio, va, n);
	case Qdata1:
		nio = zqread(&p->q[1], io, nelem(io), n);
		return readzio(io, nio, va, n);
	default:
		panic("zpread");
	}
	return -1;	/* not reached */
}

static int
zpzread(Chan *c, Kzio io[], int nio, usize n, vlong offset)
{
	ZPipe *p;

	p = c->aux;

	switch(ZPTYPE(c->qid.path)){
	case Qdir:
		return devzread(c, io, nio, n, offset);
	case Qdata0:
		return zqread(&p->q[0], io, nio, n);
	case Qdata1:
		return zqread(&p->q[1], io, nio, n);
	default:
		panic("zpzread");
	}
	return -1;	/* not reached */
}

static Zq*
zpwq(Chan *c)
{
	ZPipe *p;

	p = c->aux;
	switch(ZPTYPE(c->qid.path)){
	case Qdata0:
		return &p->q[1];
	case Qdata1:
		return &p->q[0];
	default:
		panic("zpwrite");
	}
	return nil;	/* not reached */
}

/*
 *  a write to a closed zp causes a note to be sent to
 *  the process.
 *  If the data is already in a SG_ZIO segment, we shouldn't
 *  be copying it again, probably.
 */
static long
zpwrite(Chan *c, void *va, long n, vlong)
{
	Kzio io;
	long tot, nw;
	Segment *s;
	Zq *q;
	char *cp;

	if(n <= 0)
		return n;
	q = zpwq(c);
	s = getzkseg();
	if(s == nil)
		error("no kernel segment for zero-copy");
	if(waserror()){
		putseg(s);
		nexterror();
	}
	cp = va;
	for(tot = 0; tot < n; tot += nw){
		nw = n - tot;
		if(nw > Maxatomic)
			nw = Maxatomic;
		io.data = alloczio(s, nw);
		incref(&s->Ref);
		io.seg = s;
		io.size = nw;
		if(waserror()){
			zrelease(&io);
			nexterror();
		}
		memmove(io.data, cp + tot, nw);
		zqwrite(c, q, &io, 1);
		poperror();
	}
	poperror();
	putseg(s);
	return n;
}

static int
zpzwrite(Chan *c, Kzio io[], int nio, vlong)
{
	return zqwrite(c, zpwq(c), io, nio);
}

Dev zpdevtab = {
	L'∏',
	"zp",

	devreset,
	devinit,
	devshutdown,
	zpattach,
	zpwalk,
	zpstat,
	zpopen,
	devcreate,
	zpclose,
	zpread,
	devbread,
	zpwrite,
	devbwrite,
	devremove,
	devwstat,
	nil,		/* power */
	nil,		/* config */
	zpzread,
	zpzwrite,
};
