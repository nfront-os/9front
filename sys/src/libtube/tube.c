/*
 * NIX tubes, tsend(2); after NIX's libtube/tube.c (Lsub, Bell Labs,
 * Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 * Buffered channels between processes sharing memory, made of two
 * optimistic semaphores (upsem(2)): nhole, free slots, and nmsg,
 * messages. A slot is a flag byte and the message.
 *
 * Differences to NIX: the indexes are counted with cas (32 bits;
 * libc's ainc takes a long), coherence() is libc's barrier (NIX had
 * an empty function here), talt returns the index of the entry in a
 * (NIX gave the index among the blocking entries), tsend and trecv
 * return -1 when interrupted by a note. NIX's namedtube is not here:
 * it needs NIX's libseg.
 */
#include <u.h>
#include <libc.h>
#include <tube.h>

extern int cas(uint*, int, int);	/* atom.s */

enum{Block, Dontblock, Already};	/* xsend() nb argument */

static int
iinc(int *p)
{
	int v;

	do
		v = *p;
	while(!cas((uint*)p, v, v+1));
	return v+1;
}

Tube*
newtube(ulong msz, ulong n)
{
	Tube *t;

	t = mallocz(sizeof *t + (msz+1) * n, 1);
	if(t == nil)
		return nil;
	t->msz = msz;
	t->tsz = n;
	t->nhole = n;
	return t;
}

void
freetube(Tube *t)
{
	free(t);
}

static uchar*
tslot(Tube *t, int n)
{
	uchar *c;

	c = (uchar*)&t[1];
	return c + (1+t->msz) * ((uint)n % (uint)t->tsz);
}

static int
xsend(Tube *t, void *p, int nb)
{
	uchar *c;

	assert(t != nil && p != nil);
	if(nb != Already && downsem(&t->nhole, nb) < 0)
		return -1;
	c = tslot(t, iinc(&t->tl) - 1);
	memmove(c+1, p, t->msz);
	coherence();
	*c = 1;
	upsem(&t->nmsg);
	return 0;
}

static int
xrecv(Tube *t, void *p, int nb)
{
	uchar *c;

	assert(t != nil && p != nil);
	if(nb != Already && downsem(&t->nmsg, nb) < 0)
		return -1;
	c = tslot(t, iinc(&t->hd) - 1);
	while(*c == 0)
		sleep(0);
	coherence();
	memmove(p, c+1, t->msz);
	coherence();
	*c = 0;
	upsem(&t->nhole);
	return 0;
}

int
tsend(Tube *t, void *p)
{
	return xsend(t, p, Block);
}

int
trecv(Tube *t, void *p)
{
	return xrecv(t, p, Block);
}

int
nbtsend(Tube *t, void *p)
{
	return xsend(t, p, Dontblock);
}

int
nbtrecv(Tube *t, void *p)
{
	return xrecv(t, p, Dontblock);
}

int
talt(Talt a[], int na)
{
	int i, n, *ai;
	int **ss;

	assert(a != nil && na > 0);
	ss = malloc(sizeof(int*) * na);
	ai = malloc(sizeof(int) * na);
	if(ss == nil || ai == nil){
		free(ss);
		free(ai);
		return -1;
	}
	n = 0;
	for(i = 0; i < na; i++)
		switch(a[i].op){
		case TSND:
			ai[n] = i;
			ss[n++] = &a[i].t->nhole;
			break;
		case TRCV:
			ai[n] = i;
			ss[n++] = &a[i].t->nmsg;
			break;
		case TNBSND:
			if(nbtsend(a[i].t, a[i].m) != -1)
				goto Done;
			break;
		case TNBRCV:
			if(nbtrecv(a[i].t, a[i].m) != -1)
				goto Done;
			break;
		}
	i = -1;
	if(n == 0)
		goto Done;
	i = altsems(ss, n);
	if(i < 0)
		goto Done;
	i = ai[i];
	switch(a[i].op){
	case TSND:
		xsend(a[i].t, a[i].m, Already);
		break;
	case TRCV:
		xrecv(a[i].t, a[i].m, Already);
		break;
	default:
		sysfatal("talt");
	}
Done:
	free(ss);
	free(ai);
	return i;
}
