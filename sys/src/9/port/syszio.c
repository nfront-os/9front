/*
 * NIX zero-copy I/O, after the NIX kernel (Lsub, Bell Labs, Sandia,
 * Vita Nuova; port/syszio.c; MIT license, see /lib/legal/nix):
 * the system calls zpread, zpwrite and zfree (zread(2)) and the
 * buffers of the zero-copy segments of segment(3) (kmsg, umsg).
 *
 * I/O is performed using an array of Zio structures. Each one points
 * to a buffer and gives its length. A buffer within a zero-copy
 * segment (SG_ZIO) is passed on by reference: whoever gets it gives
 * it back with zfree (or the segment's free file) when done.
 * Buffers of a umsg segment are allocated by its user, freed ones
 * are queued for the user to read from the free file; buffers of a
 * kmsg segment (SG_KZIO) are allocated by the kernel. A buffer
 * elsewhere is copied into the caller's kmsg segment first.
 *
 * Deviations from NIX: zpread/zpwrite with an offset used the
 * channel's offset and the other way round; the free list of a kmsg
 * segment (zmapfree, zmapalloc) lost buffers and could run off its
 * end; a single zwrite to an ordinary file did not free its buffer;
 * kernel copies of buffers are freed on errors; user addresses are
 * checked before the kernel touches them; no debug printing; the
 * devices' zread and zwrite are called, not installed into devtab.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

enum
{
	Maxatomic = 64*1024,	/* largest read by devzread */
	Maxgather = 1024*1024,	/* largest write from several buffers by devzwrite */
	Maxnio = 512,		/* largest io[] */
	Maxzaddr = 64*1024,	/* free addresses kept by a umsg segment */
	Zalign = 64,		/* alignment of buffers in a kmsg segment */
	Incr = 32,
};

typedef struct Map Map;
typedef struct ZMap ZMap;

/* free and allocated buffers of a kmsg segment, by address */
struct Map
{
	Map	*next;
	int	free;
	uintptr	addr;
	uintptr	size;
};

struct ZMap
{
	QLock;
	Map	*map;
};

static int	zmapfree(ZMap*, uintptr);
static uintptr	zmapalloc(ZMap*, uintptr);

/*
 * Called from putseg, when the segment is being destroyed.
 */
void
freezseg(Segment *s)
{
	Zseg *zs;
	ZMap *zp;
	Map *mp;

	zs = &s->zseg;
	free(zs->addr);
	zs->addr = nil;
	zp = zs->map;
	if(zp == nil)
		return;
	while(zp->map != nil){
		mp = zp->map;
		zp->map = mp->next;
		free(mp);
	}
	free(zp);
	zs->map = nil;
}

/*
 * Grow the pool of addresses in s's zseg; s is qlocked.
 */
static int
zgrow(Segment *s)
{
	Zseg *zs;
	uintptr *a;

	zs = &s->zseg;
	if(zs->naddr >= Maxzaddr)
		return -1;
	a = realloc(zs->addr, (zs->naddr+Incr)*sizeof(uintptr));
	if(a == nil)
		return -1;
	zs->addr = a;
	zs->naddr += Incr;
	return 0;
}

/*
 * Take a free address from s's zseg, 0 if none; s is qlocked.
 */
uintptr
zgetaddr(Segment *s)
{
	Zseg *zs;
	uintptr va;

	zs = &s->zseg;
	if(zs->end == 0)
		return 0;
	va = zs->addr[0];
	zs->end--;
	if(zs->end > 0)
		zs->addr[0] = zs->addr[zs->end];
	return va;
}

/*
 * Give the buffer at va back to s; s is qlocked.
 * Wake up any reader of the free file waiting for it.
 * Raises no error: -1 if va is not a buffer of s.
 */
int
zputaddr(Segment *s, uintptr va)
{
	Zseg *zs;

	zs = &s->zseg;
	if((s->type&SG_ZIO) == 0 || va < s->base || va >= s->top)
		return -1;
	if((s->type&SG_KZIO) != 0)
		return zmapfree(zs->map, va);
	if(zs->end == zs->naddr && zgrow(s) < 0)
		return -1;
	zs->addr[zs->end++] = va;
	if(zs->end == 1)
		wakeup(&zs->rr);	/* in case anyone was waiting */
	return 0;
}

/*
 * Allocate a buffer of len bytes in the kmsg segment s.
 */
void*
alloczio(Segment *s, long len)
{
	uintptr va;

	if((s->type&SG_KZIO) == 0 || s->zseg.map == nil)
		error("not a kernel zero-copy segment");
	if(len < 0)
		error(Ebadarg);
	va = zmapalloc(s->zseg.map, len);
	if(va == 0)
		error("kernel zero copy segment exhausted");
	return (void*)va;
}

/*
 * Locate the kernel segment for zero copy of up,
 * return it unlocked with a reference added.
 */
Segment*
getzkseg(void)
{
	Segment *s;
	int i;

	qlock(&up->seglock);
	for(i = 0; i < NSEG; i++){
		s = up->seg[i];
		if(s != nil && (s->type&SG_KZIO) != 0){
			incref(&s->Ref);
			qunlock(&up->seglock);
			return s;
		}
	}
	qunlock(&up->seglock);
	return nil;
}

/*
 * Free the buffer of io and drop the reference to its segment.
 */
void
zrelease(Kzio *io)
{
	Segment *s;

	s = io->seg;
	if(s == nil)
		return;
	io->seg = nil;
	qlock(&s->QLock);
	zputaddr(s, (uintptr)io->data);
	qunlock(&s->QLock);
	putseg(s);
}

/*
 * This is the counterpart of devzread in some sense,
 * it reads in the traditional way from io[]: copies the
 * buffers to a, at most count bytes, and frees them.
 */
long
readzio(Kzio io[], int nio, void *a, long count)
{
	long tot, nr;
	char *p;
	int bad;

	p = a;
	tot = 0;
	bad = 0;
	for(; nio > 0; nio--, io++){
		if(tot < count){
			nr = io->size;
			if(tot + nr > count)
				nr = count - tot;
			/* NIX: the buffer must be in up's address space */
			if(okaddr((uintptr)io->data, nr, 0)){
				memmove(p+tot, io->data, nr);
				tot += nr;
			}else
				bad = 1;
		}
		zrelease(io);
	}
	if(bad && tot == 0)
		error("zero-copy buffer not in this address space");
	return tot;
}

/*
 * Default zero-copy read: read into a buffer
 * of the caller's kmsg segment.
 */
int
devzread(Chan *c, Kzio io[], int nio, usize tot, vlong offset)
{
	Segment *s;
	long n;

	if(nio < 1)
		return 0;
	s = getzkseg();
	if(s == nil)
		error("no kernel segment for zero-copy");
	if(tot > Maxatomic)
		tot = Maxatomic;
	if(waserror()){
		putseg(s);
		nexterror();
	}
	io[0].data = alloczio(s, tot);
	poperror();
	io[0].seg = s;
	io[0].size = 0;
	if(waserror()){
		zrelease(&io[0]);
		nexterror();
	}
	n = devtab[c->type]->read(c, io[0].data, tot, offset);
	poperror();
	if(n <= 0){
		zrelease(&io[0]);
		return 0;
	}
	io[0].size = n;
	return 1;
}

/*
 * Default zero-copy write: write the buffers (several at once
 * through a Block), then free them. Returns the number of
 * buffers written, their sizes say how much of each.
 */
int
devzwrite(Chan *c, Kzio io[], int nio, vlong offset)
{
	int i;
	uvlong tot;
	long n;
	Block *bp;

	if(waserror()){
		for(i = 0; i < nio; i++)
			zrelease(&io[i]);
		nexterror();
	}
	tot = 0;
	for(i = 0; i < nio; i++)
		tot += io[i].size;
	if(nio == 1)
		n = devtab[c->type]->write(c, io[0].data, io[0].size, offset);
	else{
		if(tot > Maxgather)
			error(Etoobig);
		bp = allocb(tot);
		if(waserror()){
			freeb(bp);
			nexterror();
		}
		for(i = 0; i < nio; i++){
			memmove(bp->wp, io[i].data, io[i].size);
			bp->wp += io[i].size;
		}
		poperror();
		n = devtab[c->type]->bwrite(c, bp, offset);
	}
	poperror();
	for(i = 0; i < nio; i++)
		zrelease(&io[i]);
	if(n < 0)
		n = 0;
	for(i = 0; i < nio && n > 0; i++){
		if(n < io[i].size)
			io[i].size = n;
		n -= io[i].size;
	}
	return i;
}

/*
 * Copy the buffer of io into the caller's kmsg segment.
 */
static void
kernzio(Kzio *io)
{
	Segment *s;
	void *data;

	s = getzkseg();
	if(s == nil)
		error("can't use zero copy in this segment");
	if(waserror()){
		putseg(s);
		nexterror();
	}
	data = alloczio(s, io->size);
	poperror();
	memmove(data, io->data, io->size);
	putseg(io->seg);
	io->data = data;
	io->seg = s;
}

static int
ziorw(int fd, Zio *uio, int nio, usize count, vlong offset, int iswrite)
{
	int i, n, isprw;
	Kzio *kio, skio[16];
	uchar *kcopy, skcopy[16];
	Segment *s;
	Chan *c;
	Dev *d;
	vlong tot;

	if(nio <= 0 || nio > Maxnio)
		error("wrong io[] size");
	validaddr((uintptr)uio, sizeof(Zio)*nio, 1);
	c = fdtochan(fd, iswrite? OWRITE: OREAD, 1, 1);
	kio = nil;
	kcopy = nil;
	if(waserror()){
		if(kio != nil){
			/* the caller keeps its buffers, copies are freed */
			for(i = 0; i < nio; i++)
				if(kcopy[i])
					zrelease(&kio[i]);
				else
					putseg(kio[i].seg);
			if(kio != skio)
				free(kio);
		}
		cclose(c);
		nexterror();
	}
	if(c->qid.type & QTDIR)
		error(Eisdir);
	isprw = offset != -1LL;
	if(!isprw)
		offset = c->offset;
	if(offset < 0)
		error(Enegoff);
	if(nio <= nelem(skio)){
		kio = skio;
		kcopy = skcopy;
	}else{
		kio = smalloc(nio*(sizeof(Kzio)+1));
		kcopy = (uchar*)&kio[nio];
	}
	memset(kio, 0, nio*sizeof(Kzio));
	memset(kcopy, 0, nio);
	for(i = 0; i < nio; i++){
		kio[i].Zio = uio[i];
		if(!iswrite){
			kio[i].data = nil;
			continue;
		}
		s = seg(up, (uintptr)kio[i].data, 1);
		if(s == nil)
			error("invalid address in zio");
		incref(&s->Ref);
		kio[i].seg = s;
		qunlock(&s->QLock);
		validaddr((uintptr)kio[i].data, kio[i].size, 0);
		if((s->type&SG_ZIO) == 0){
			/*
			 * It's not a segment where we can report
			 * addresses to anyone once they are free.
			 * So, allocate space in the kernel
			 * and copy the user data there.
			 */
			kernzio(&kio[i]);
			kcopy[i] = 1;
		}
	}

	d = devtab[c->type];
	if(iswrite){
		if(d->zwrite != nil)
			n = d->zwrite(c, kio, nio, offset);
		else
			n = devzwrite(c, kio, nio, offset);
	}else{
		if(d->zread != nil)
			n = d->zread(c, kio, nio, count, offset);
		else
			n = devzread(c, kio, nio, count, offset);
	}
	/* the buffers are the device's (write) or the caller's (read) now */
	tot = 0;
	for(i = 0; i < n; i++){
		uio[i] = kio[i].Zio;
		tot += kio[i].size;
		if(!iswrite){
			putseg(kio[i].seg);
			kio[i].seg = nil;
		}
	}
	if(!isprw){
		/* unlike in syswrite, we update offsets at the end */
		lock(&c->Lock);
		c->devoffset += tot;
		c->offset += tot;
		unlock(&c->Lock);
	}
	poperror();
	cclose(c);
	if(kio != skio)
		free(kio);
	return n;
}

/*
 * int zpread(int fd, Zio io[], int nio, usize count, vlong offset);
 */
uintptr
syszpread(uchar *list)
{
	int fd, nio;
	usize count;
	vlong offset;
	Zio *io;

	fd = ARG(list, int);
	io = ARG(list, Zio*);
	nio = ARG(list, int);
	count = ARG(list, usize);
	offset = ARG(list, vlong);
	return (uintptr)ziorw(fd, io, nio, count, offset, 0);
}

/*
 * int zpwrite(int fd, Zio io[], int nio, vlong offset);
 */
uintptr
syszpwrite(uchar *list)
{
	int fd, nio;
	vlong offset;
	Zio *io;

	fd = ARG(list, int);
	io = ARG(list, Zio*);
	nio = ARG(list, int);
	offset = ARG(list, vlong);
	return (uintptr)ziorw(fd, io, nio, 0, offset, 1);
}

/*
 * void zfree(Zio io[], int nio);
 */
uintptr
syszfree(uchar *list)
{
	Zio *io;
	int nio, i, r;
	uintptr va;
	Segment *s;

	io = ARG(list, Zio*);
	nio = ARG(list, int);
	if(nio < 0 || nio > Maxnio)
		error("wrong io[] size");
	validaddr((uintptr)io, sizeof(Zio)*nio, 1);
	for(i = 0; i < nio; i++){
		va = (uintptr)io[i].data;
		s = seg(up, va, 1);
		if(s == nil)
			error("invalid address in zio");
		if((s->type&SG_ZIO) == 0){
			qunlock(&s->QLock);
			error("segment is not a zero-copy segment");
		}
		r = zputaddr(s, va);
		qunlock(&s->QLock);
		if(r < 0)
			error("not a zero-copy buffer");
		io[i].data = nil;
		io[i].size = 0;
	}
	return 0;
}

/*
 * This must go, but for now, we use Zmaps
 * to allocate messages within the shared kernel segment.
 * This is a simple first fit with a single fragment list.
 */
void
newzmap(Segment *s)
{
	ZMap *zp;
	Map *mp;

	if((s->type&SG_KZIO) == 0)
		panic("newzmap but not SG_KZIO");
	if(s->zseg.map != nil)
		panic("newzmap: already allocated");
	zp = smalloc(sizeof(ZMap));
	mp = smalloc(sizeof(Map));
	mp->free = 1;
	mp->addr = s->base;
	mp->size = s->top - s->base;
	zp->map = mp;
	s->zseg.map = zp;
}

static int
zmapfree(ZMap *rmap, uintptr addr)
{
	Map *mp, *prev, *next;

	if(rmap == nil)
		return -1;
	qlock(rmap);
	prev = nil;
	for(mp = rmap->map; mp != nil; mp = mp->next){
		if(mp->addr == addr)
			break;
		prev = mp;
	}
	if(mp == nil || mp->free){
		qunlock(rmap);
		return -1;
	}
	mp->free = 1;
	next = mp->next;
	if(next != nil && next->free){
		mp->size += next->size;
		mp->next = next->next;
		free(next);
	}
	if(prev != nil && prev->free){
		prev->size += mp->size;
		prev->next = mp->next;
		free(mp);
	}
	qunlock(rmap);
	return 0;
}

static uintptr
zmapalloc(ZMap *rmap, uintptr size)
{
	Map *mp, *nmp;

	size = ROUND(size, Zalign);
	if(size == 0)
		size = Zalign;
	qlock(rmap);
	for(mp = rmap->map; mp != nil; mp = mp->next)
		if(mp->free && mp->size >= size)
			break;
	if(mp == nil){
		qunlock(rmap);
		return 0;
	}
	if(mp->size > size){
		nmp = malloc(sizeof *nmp);
		if(nmp == nil){
			qunlock(rmap);
			return 0;
		}
		*nmp = *mp;
		nmp->addr += size;
		nmp->size -= size;
		nmp->free = 1;
		mp->size = size;
		mp->next = nmp;
	}
	mp->free = 0;
	qunlock(rmap);
	return mp->addr;
}
