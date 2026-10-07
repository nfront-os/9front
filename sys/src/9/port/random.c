#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

#include	<libsec.h>

/* machine specific hardware random number generator */
void (*hwrandbuf)(void*, ulong) = nil;

static struct
{
	QLock;
	Chachastate cs;
} *rs;

typedef struct Seedbuf Seedbuf;
struct Seedbuf
{
	ulong		randomcount;
	uchar		buf[64];
	uchar		nbuf;
	uchar		next;
	ushort		bits;

	SHA2_512state	ds;
};

static void
randomsample(Ureg*, Timer *t)
{
	Seedbuf *s = t->ta;

	if(s->randomcount == 0 || s->nbuf >= sizeof(s->buf))
		return;
	s->bits = (s->bits<<2) ^ s->randomcount;
	s->randomcount = 0;
	if(++s->next < 8/2)
		return;
	s->next = 0;
	s->buf[s->nbuf++] ^= s->bits;
}

static void
randomseed(void*)
{
	Seedbuf *s;

	s = secalloc(sizeof(Seedbuf));

	if(hwrandbuf != nil){
		/*
		 * Not a GCC-port fix: a hardware RNG instruction already
		 * gives sufficient entropy on its own, so skip the slow
		 * scheduling-jitter collection loop below entirely instead
		 * of still waiting for it to independently fill s->nbuf.
		 * front's original code fills s->buf here but then always
		 * falls through to the jitter loop regardless -- harmless
		 * on real hardware (plenty of genuine timing jitter to
		 * harvest quickly) but pathologically slow under emulators
		 * with little true timing jitter (e.g. QEMU TCG), where the
		 * loop can take minutes instead of the expected seconds.
		 */
		(*hwrandbuf)(s->buf, sizeof(s->buf));
		s->nbuf = sizeof(s->buf);
	} else {
		/* Frequency close but not equal to HZ */
		up->Timer.tns = (vlong)(MS2HZ+3)*1000000LL;
		up->Timer.tmode = Tperiodic;
		up->Timer.tt = nil;
		up->Timer.ta = s;
		up->Timer.tf = randomsample;
		timeradd(&up->Timer);
		while(s->nbuf < sizeof(s->buf)){
			if(++s->randomcount <= 100000)
				continue;
			if(anyhigher())
				sched();
		}
		timerdel(&up->Timer);
	}

	sha2_512(s->buf, sizeof(s->buf), s->buf, &s->ds);
	setupChachastate(&rs->cs, s->buf, 32, s->buf+32, 12, 20);
	qunlock(&rs->QLock);

	secfree(s);

	pexit("", 1);
}

void
randominit(void)
{
	rs = secalloc(sizeof(*rs));
	qlock(&rs->QLock);	/* randomseed() unlocks once seeded */
	kproc("randomseed", randomseed, nil);
}

ulong
randomread(void *p, ulong n)
{
	Chachastate c;

	if(n == 0)
		return 0;

	if(hwrandbuf != nil)
		(*hwrandbuf)(p, n);

	/* copy chacha state, rekey and increment iv */
	qlock(&rs->QLock);
	c = rs->cs;
	chacha_encrypt((uchar*)&rs->cs.input[4], 32, &c);
	if(++rs->cs.input[13] == 0)
		if(++rs->cs.input[14] == 0)
			++rs->cs.input[15];
	qunlock(&rs->QLock);

	/* encrypt the buffer, can fault */
	chacha_encrypt((uchar*)p, n, &c);

	/* prevent state leakage */
	memset(&c, 0, sizeof(c));

	return n;
}

/* used by fastrand() */
void
genrandom(uchar *p, int n)
{
	randomread(p, n);
}

/* used by rand(),nrand() */
long
lrand(void)
{
	/* xoroshiro128+ algorithm */
	static int seeded = 0;
	static uvlong s[2];
	static Lock lk;
	ulong r;

	if(seeded == 0){
		randomread(s, sizeof(s));
		seeded = (s[0] | s[1]) != 0;
	}

	lock(&lk);
	r = (s[0] + s[1]) >> 33;
	s[1] ^= s[0];
 	s[0] = (s[0] << 55 | s[0] >> 9) ^ s[1] ^ (s[1] << 14);
 	s[1] = (s[1] << 36 | s[1] >> 28);
	unlock(&lk);

 	return r;
}
