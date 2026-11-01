/*
 * NIX named tubes, tsend(2); after NIX's libtube/namedtube.c (Lsub,
 * Bell Labs, Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 * namedtube("seg!tube", ...) is a tube in the global shared segment
 * seg (segment(3), made by newseg(2)), found by its name.
 *
 * Differences to NIX: the directory of the tubes is in the segment
 * (a header with a lock and offsets), not in a list of the process
 * that made it, so unrelated processes find the same tubes (NIX's
 * second process made the segment anew over the first one's);
 * a global segment has the same address in every process, so the
 * tubes' semaphores are the same ones. A tube's slots are a flag byte
 * and the message (NIX's took only the message, one byte short each),
 * as newtube's. The process keeps the segments it attached (a second
 * segattach of one fails).
 */
#include <u.h>
#include <libc.h>
#include <seg.h>
#include <tube.h>

enum
{
	Tnamelen = 16,
	Tsegsz = 64 * 1024,
	Tmagic = 0x7475626e,	/* "nbut" */
};

typedef struct Thdr Thdr;
typedef struct Ntube Ntube;
typedef struct Tseg Tseg;

/* at the start of the segment */
struct Thdr
{
	Lock;
	u32int	magic;
	u32int	end;		/* offset of the free space */
	u32int	first;		/* offset of the first Ntube, 0: none */
};

/* in the segment, followed by its Tube */
struct Ntube
{
	char	name[Tnamelen];
	u32int	next;		/* offset of the next Ntube, 0: none */
};

/* the segments this process attached */
struct Tseg
{
	char	name[Tnamelen];
	Thdr	*h;
	Tseg	*next;
};

static Tseg *segs;
static Lock segslk;

int namedtubedebug;

#define dprint	if(namedtubedebug)print

static int
exists(char *name)
{
	char buf[64];

	if(bind("#g", "/mnt/seg", MREPL|MCREATE) < 0)
		return 0;
	snprint(buf, sizeof buf, "/mnt/seg/%s", name);
	return access(buf, AEXIST) == 0;
}

static Thdr*
seglookup(char *name, int mkit)
{
	Tseg *s;
	Thdr *h;

	lock(&segslk);
	for(s = segs; s != nil; s = s->next)
		if(strcmp(s->name, name) == 0){
			unlock(&segslk);
			return s->h;
		}
	unlock(&segslk);
	if(!mkit && !exists(name)){
		werrstr("segment not found");
		return nil;
	}
	h = newseg(name, 0, Tsegsz);
	if(h == nil && exists(name))
		h = newseg(name, 0, Tsegsz);	/* another process made it just now */
	if(h == nil)
		return nil;
	/* a new segment is zeroed: the first one here sets it up */
	lock(&h->Lock);
	if(h->magic == 0){
		h->magic = Tmagic;
		h->end = sizeof(Thdr);
		h->first = 0;
	}
	unlock(&h->Lock);
	if(h->magic != Tmagic){
		werrstr("segment %s holds no tubes", name);
		return nil;
	}
	s = mallocz(sizeof *s, 1);
	if(s == nil)
		return nil;
	strncpy(s->name, name, sizeof s->name - 1);
	s->h = h;
	lock(&segslk);
	s->next = segs;
	segs = s;
	unlock(&segslk);
	dprint("namedtube: segment %s at %#p\n", name, h);
	return h;
}

static Tube*
tubelookup(Thdr *h, char *name, ulong msz, ulong n, int mkit)
{
	Ntube *nt;
	Tube *t;
	uchar *base;
	u32int o;
	uvlong sz;

	if(msz == 0 || n == 0){
		werrstr("bad argument");
		return nil;
	}
	base = (uchar*)h;
	lock(&h->Lock);
	for(o = h->first; o != 0; o = nt->next){
		nt = (Ntube*)(base + o);
		if(strncmp(nt->name, name, Tnamelen) == 0)
			break;
	}
	if(o == 0 && !mkit){
		unlock(&h->Lock);
		werrstr("tube not found");
		return nil;
	}
	if(o == 0){
		sz = sizeof(Ntube) + sizeof(Tube) + (uvlong)(msz+1)*n;
		sz = (sz + 7) & ~7;
		if(h->end + sz > Tsegsz){
			unlock(&h->Lock);
			werrstr("segment exhausted");
			return nil;
		}
		o = h->end;
		h->end += sz;
		nt = (Ntube*)(base + o);
		memset(nt, 0, sz);
		strncpy(nt->name, name, Tnamelen);
		t = (Tube*)&nt[1];
		t->msz = msz;
		t->tsz = n;
		t->nhole = n;
		nt->next = h->first;
		h->first = o;
	}
	unlock(&h->Lock);
	t = (Tube*)&nt[1];
	if(t->msz != msz){
		werrstr("wrong message size");
		return nil;
	}
	dprint("namedtube: %s at %#p\n", name, t);
	return t;
}

/*
 * The tube segname!tubename (or tubes!name), made with the segment
 * if mkit is set and they are not there.
 */
Tube*
namedtube(char *name, ulong msz, int n, int mkit)
{
	char *dir, *tname;
	Thdr *h;
	Tube *t;

	name = strdup(name);
	if(name == nil)
		return nil;
	tname = utfrune(name, '!');
	if(tname == nil){
		dir = "tubes";
		tname = name;
	}else{
		dir = name;
		*tname++ = 0;
	}
	t = nil;
	if(strlen(dir) >= Tnamelen || strlen(tname) >= Tnamelen)
		werrstr("name too long");
	else if((h = seglookup(dir, mkit)) != nil)
		t = tubelookup(h, tname, msz, n, mkit);
	free(name);
	return t;
}
