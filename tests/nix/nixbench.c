/*
 * NIX: what an application core gives a process, measured.
 *	nixbench [-b] [-r reps]	(-b: memwalk on 2 MB pages, segattach "big")
 * Run it on a time-sharing core and on an AC (execac -c n nixbench)
 * and compare: tests/nix/bench.sh. Prints one line per measurement:
 *	name core mean min max unit
 */
#include <u.h>
#include <libc.h>
#include <tos.h>

static int reps = 5;
static int core;

static double
secs(uvlong c0, uvlong c1)
{
	return (double)(c1 - c0) / _tos->cyclefreq;
}

static int
mycore(void)
{
	char buf[64], *f[2];
	int fd, n;

	snprint(buf, sizeof buf, "/proc/%d/core", getpid());
	if((fd = open(buf, OREAD)) < 0)
		return -1;
	n = read(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		return -1;
	buf[n] = 0;
	if(tokenize(buf, f, 2) < 1)
		return -1;
	return atoi(f[0]);
}

static void
report(char *name, double *v, int n, double scale, char *unit)
{
	double sum, lo, hi;
	int i;

	sum = 0;
	lo = hi = v[0];
	for(i = 0; i < n; i++){
		sum += v[i];
		if(v[i] < lo)
			lo = v[i];
		if(v[i] > hi)
			hi = v[i];
	}
	print("%-10s %d %10.3f %10.3f %10.3f %s\n", name, core,
		sum/n*scale, lo*scale, hi*scale, unit);
}

/* pure computation, no memory traffic, no system calls */
static double
compute(void)
{
	uvlong c0, c1, x;
	ulong i;

	x = 88172645463325252ULL;
	cycles(&c0);
	for(i = 0; i < 400000000UL; i++){
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
	}
	cycles(&c1);
	if(x == 42)
		print("unlikely\n");
	return secs(c0, c1);
}

/* random pointer chase through 64 MB: caches and the TLB */
enum { Memsz = 64*1024*1024, Nslot = Memsz/sizeof(uintptr), Steps = 20000000 };
static uintptr smem[Nslot];
static uintptr *mem = smem;	/* -b: a segment of 2 MB pages */

static void
meminit(void)
{
	ulong i, j, t;
	ulong *perm;

	/* one cycle through all slots, 64 bytes apart (a cache line) */
	perm = malloc(Nslot/8 * sizeof(ulong));
	if(perm == nil)
		sysfatal("malloc: %r");
	for(i = 0; i < Nslot/8; i++)
		perm[i] = i*8;
	srand(1);
	for(i = Nslot/8-1; i > 0; i--){
		j = lrand() % (i+1);
		t = perm[i];
		perm[i] = perm[j];
		perm[j] = t;
	}
	for(i = 0; i < Nslot/8; i++)
		mem[perm[i]] = (uintptr)&mem[perm[(i+1) % (Nslot/8)]];
	free(perm);
}

static double
memwalk(void)
{
	uvlong c0, c1;
	uintptr *p;
	ulong i;

	p = (uintptr*)mem[0];
	cycles(&c0);
	for(i = 0; i < Steps; i++)
		p = (uintptr*)*p;
	cycles(&c1);
	if(p == nil)
		print("unlikely\n");
	return secs(c0, c1);
}

/*
 * interruptions: read the cycle counter for a second and count the
 * gaps over 10µs (a clock tick, a device interrupt, another process)
 */
static double maxgap;

static double
jitter(void)
{
	uvlong c0, c, last, lim, gap, max;
	ulong n;

	lim = _tos->cyclefreq / 100000;	/* 10µs */
	max = 0;
	n = 0;
	cycles(&c0);
	last = c0;
	do{
		cycles(&c);
		gap = c - last;
		if(gap > lim)
			n++;
		if(gap > max)
			max = gap;
		last = c;
	}while(c - c0 < _tos->cyclefreq);
	maxgap = (double)max / _tos->cyclefreq;
	return n;
}

/* one system call: nsec(2) */
static double
syscalls(void)
{
	uvlong c0, c1;
	int i;

	cycles(&c0);
	for(i = 0; i < 20000; i++)
		nsec();
	cycles(&c1);
	return secs(c0, c1) / 20000;
}

void
main(int argc, char *argv[])
{
	double v[32], g[32];
	char *walk;
	int i;

	walk = "memwalk";
	ARGBEGIN{
	case 'b':
		mem = segattach(0, "big", 0, Memsz);
		if(mem == (void*)-1)
			sysfatal("segattach big: %r");
		walk = "memwalk-big";
		break;
	case 'r':
		reps = atoi(EARGF(sysfatal("usage: nixbench [-b] [-r reps]")));
		break;
	}ARGEND
	if(reps < 1 || reps > nelem(v))
		reps = 5;
	core = mycore();
	meminit();

	for(i = 0; i < reps; i++)
		v[i] = compute();
	report("compute", v, reps, 1000, "ms");
	for(i = 0; i < reps; i++)
		v[i] = memwalk();
	report(walk, v, reps, 1000, "ms");
	for(i = 0; i < reps; i++){
		v[i] = jitter();
		g[i] = maxgap;
	}
	report("gaps>10us", v, reps, 1, "per s");
	report("maxgap", g, reps, 1e6, "us");
	for(i = 0; i < reps; i++)
		v[i] = syscalls();
	report("syscall", v, reps, 1e6, "us");
	exits(nil);
}
