/*
 * NIX optimistic semaphores, upsem(2); after NIX's libc/9sys/upsem.c
 * (Lsub, Bell Labs, Sandia, Vita Nuova; MIT license, see
 * /lib/legal/nix). The kernel's side is port/syssem.c.
 *
 * The semaphore is an int, as in NIX: 32 bits for every compiler.
 * libc's ainc and adec take a long (64 bits with gcc on amd64 and
 * arm64), so the increments are made here with cas (atom.s).
 * A blocking downsem or altsems interrupted by a note returns -1.
 */
#include <u.h>
#include <libc.h>

extern int cas(uint*, int, int);	/* atom.s */

typedef struct Cnt Cnt;	/* debug */
struct Cnt
{
	int	sleep;
	int	zsleep;
	int	wake;
	int	zwake;
	int	uup;
	int	kup;
	int	udown;
	int	kdown;
	int	ualt;
	int	kalt;
};

#define dprint	if(semdebug)fprint

int semtrytimes = 100;
int semdebug;

static Cnt c;

static int
iinc(int *p)
{
	int v;

	do
		v = *p;
	while(!cas((uint*)p, v, v+1));
	return v+1;
}

static int
idec(int *p)
{
	int v;

	do
		v = *p;
	while(!cas((uint*)p, v, v-1));
	return v-1;
}

void
upsem(int *s)
{
	int n;

	assert(s != nil);
	coherence();	/* what the holder wrote before the unit it gives */
	n = iinc(s);
	dprint(2, "upsem: %#p = %d\n", s, n);
	if(n <= 0){
		iinc(&c.kup);
		semwakeup(s);
	}else
		iinc(&c.uup);
}

int
downsem(int *s, int dontblock)
{
	int n;
	int i;

	assert(s != nil);
	/* busy wait */
	for(i = 0; *s <= 0 && i < semtrytimes; i++)
		sleep(0);

	if(*s <= 0 && dontblock)
		return -1;
	n = idec(s);
	dprint(2, "downsem: %#p = %d\n", s, n);
	if(n < 0){
		iinc(&c.kdown);
		n = semsleep(s, dontblock);
		if(dontblock == 0)
			dprint(2, "downsem: %#p awaken\n", s);
		return n;
	}
	iinc(&c.udown);
	return 0;
}

int
altsems(int *ss[], int n)
{
	int i, w, r;

	assert(ss != nil);
	assert(n > 0);

	/* busy wait */
	for(w = 0; w < semtrytimes; w++){
		for(i = 0; i < n; i++)
			if(ss[i] == nil)
				sysfatal("altsems: nil sem");
			else if(*ss[i] > 0)
				break;
		if(i < n)
			break;
		sleep(0);
	}

	for(i = 0; i < n; i++)
		if(downsem(ss[i], 1) != -1){
			iinc(&c.ualt);
			return i;
		}

	iinc(&c.kalt);
	r = semalt(ss, n);
	return r;
}

void
semstats(void)
{
	print("sleep: %d\n", c.sleep);
	print("zsleep: %d\n", c.zsleep);
	print("wake: %d\n", c.wake);
	print("zwake: %d\n", c.zwake);
	print("uup: %d\n", c.uup);
	print("kup: %d\n", c.kup);
	print("udown: %d\n", c.udown);
	print("kdown: %d\n", c.kdown);
	print("ualt: %d\n", c.ualt);
	print("kalt: %d\n", c.kalt);
}
