/*
 * NIX performance counters, pmc(2), pmc(3) (port/devpmc.c), run by
 * tests/nix/pmc.sh:
 *	pmctest tc other	on a TC (wires itself to core 1)
 *	execac -c n /tests/pmctest ac other	on an AC
 *	pmctest stress core ms	reads of core's ctr00 for ms, non-decreasing
 *	execac -c n /tests/pmctest quiet ms	spin ms on the AC without system
 *		calls: the round trips to the TC (/proc/n/core) meanwhile
 * other: another core whose counter 1 is set and read; with -s it
 * spins (an idle AC), so its kernel-mode count must grow.
 * Prints "ok name", "FAIL name: why" or "SKIP name: why" per check.
 */
#include <u.h>
#include <libc.h>
#include <tos.h>

enum {
	N = 10*1000*1000,	/* spin iterations */
};

static int failed;
static int core;
static char *ev;
static int isinstr;

static void
ok(char *name)
{
	print("ok %s\n", name);
}

static void
fail(char *name, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("FAIL %s: %s\n", name, buf);
	failed++;
}

static void
check(int c, char *name, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	if(c){
		ok(name);
		return;
	}
	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	fail(name, "%s", buf);
}

static uvlong sink;

static void
spin(uvlong n)
{
	uvlong x;

	x = 88172645463325252ULL + n;
	while(n-- > 0){
		x ^= x<<13;
		x ^= x>>7;
		x ^= x<<17;
	}
	sink = x;
}

/* the counter by the instruction, as the kernel allows while one is enabled */
static uvlong
hwrdpmc(int i)
{
#if defined(__x86_64__)
	u32int lo, hi;

	__asm__ volatile("rdpmc" : "=a"(lo), "=d"(hi) : "c"(i));
	return (uvlong)hi<<32 | lo;
#elif defined(__aarch64__)
	uvlong v;

	USED(i);
	__asm__ volatile("mrs %0, pmevcntr0_el0" : "=r"(v));
	return v;
#else
	USED(i);
	return ~0ULL;
#endif
}

static char*
readfile(char *name, char *buf, int nbuf)
{
	int fd, n;

	if((fd = open(name, OREAD)) < 0)
		return nil;
	n = read(fd, buf, nbuf-1);
	close(fd);
	if(n < 0)
		return nil;
	buf[n] = 0;
	return buf;
}

static int
writefile(char *name, char *s)
{
	int fd, r;

	if((fd = open(name, OWRITE)) < 0)
		return -1;
	r = write(fd, s, strlen(s));
	close(fd);
	return r == strlen(s)? 0: -1;
}

static uvlong
rdctr(int c, int i)
{
	char name[64], buf[64];

	snprint(name, sizeof name, "#ε/core%4.4d/ctr%2.2d", c, i);
	if(readfile(name, buf, sizeof buf) == nil)
		return ~0ULL;
	return strtoull(buf, nil, 0);
}

static int
ncounters(int c)
{
	char name[64];
	int i;

	for(i = 0; i < 64; i++){
		snprint(name, sizeof name, "#ε/core%4.4d/ctr%2.2d", c, i);
		if(access(name, AEXIST) < 0)
			break;
	}
	return i;
}

/* delta of counter 0 of core over spin(n) */
static uvlong
measure(uvlong n)
{
	uvlong a, b;

	a = rdpmc(0);
	spin(n);
	b = rdpmc(0);
	return b - a;
}

static void
owncore(void)
{
	char name[64], buf[256];
	uvlong a, b, h, d, d2, prev, v;
	int i, mono;

	/* the counter of our own core */
	check(confpmc(-1, 0, PmcUser|PmcEnable, ev) == 0, "confpmc", "%r");
	snprint(name, sizeof name, "#ε/core%4.4d/ctr00ctl", core);
	if(readfile(name, buf, sizeof buf) == nil)
		buf[0] = 0;
	check(strncmp(buf, "enable\nuser\n", 12) == 0, "ctl reads back", "%q", buf);

	mono = 1;
	prev = rdpmc(0);
	for(i = 0; i < 20; i++){
		spin(N/100);
		v = rdpmc(0);
		if(v < prev){
			mono = 0;
			break;
		}
		prev = v;
	}
	check(mono, "monotonic", "%llud after %llud", v, prev);

	d = measure(N);
	if(isinstr)
		check(d >= N && d <= 40ULL*N, "count over a loop", "%llud instructions for %d iterations", d, N);
	else
		check(d >= N/8 && d <= 1000ULL*N, "count over a loop", "%llud cycles for %d iterations", d, N);
	d2 = measure(2*N);
	check(d2*10 >= d*13 && d2*10 <= d*30, "twice the loop, twice the count", "%llud then %llud", d, d2);

	a = rdpmc(0);
	sleep(200);
	b = rdpmc(0);
	check(b - a < d/10, "user only: little while asleep", "%llud asleep, %llud busy", b - a, d);

	a = rdpmc(0);
	h = hwrdpmc(0);
	b = rdpmc(0);
	check(a <= h && h <= b, "the instruction reads it too", "%llud %llud %llud", a, h, b);

	check(confpmc(-1, 0, PmcOs|PmcEnable, ev) == 0, "confpmc os", "%r");
	a = rdpmc(0);
	spin(N);
	b = rdpmc(0);
	check(b - a < d/4, "os only: little in a user loop", "%llud in the kernel, %llud in user mode", b - a, d);

	/* reset, disabled: a value written stays */
	check(confpmc(-1, 0, 0, ev) == 0, "confpmc reset", "%r");
	snprint(name, sizeof name, "#ε/core%4.4d/ctr00", core);
	check(writefile(name, "12345") == 0, "write the counter", "%r");
	a = rdpmc(0);
	spin(N/10);
	b = rdpmc(0);
	check(a == 12345 && b == 12345, "written value, disabled", "%llud %llud", a, b);
}

static void
othercore(int other, int spins)
{
	char name[64], cname[64], buf[256];
	uvlong a, b, c;

	if(other == core || ncounters(other) < 2){
		print("SKIP other core: core %d\n", other);
		return;
	}
	snprint(cname, sizeof cname, "#ε/core%4.4d/ctr01ctl", other);
	snprint(name, sizeof name, "#ε/core%4.4d/ctr01", other);
	check(confpmc(other, 1, 0, ev) == 0, "other core: reset", "%r");
	check(writefile(name, "777") == 0, "other core: write", "%r");
	check(rdctr(other, 1) == 777, "other core: read back", "%llud", rdctr(other, 1));
	check(confpmc(other, 1, PmcOs|PmcUser|PmcEnable, ev) == 0, "other core: enable", "%r");
	if(readfile(cname, buf, sizeof buf) == nil)
		buf[0] = 0;
	check(strncmp(buf, "enable\nuser\nos\n", 15) == 0, "other core: ctl", "%q", buf);
	a = rdctr(other, 1);
	sleep(100);
	b = rdctr(other, 1);
	sleep(100);
	c = rdctr(other, 1);
	if(spins)
		check(a >= 777 && a < b && b < c, "other core: counts", "%llud %llud %llud", a, b, c);
	else
		check(a >= 777 && a <= b && b <= c, "other core: monotonic", "%llud %llud %llud", a, b, c);
	confpmc(other, 1, 0, nil);
}

static void
errors(void)
{
	char name[64];

	snprint(name, sizeof name, "#ε/core%4.4d/ctr00ctl", core);
	check(writefile(name, "set no such event") < 0, "bad event refused", "accepted");
	check(writefile(name, "bogus") < 0, "bad command refused", "accepted");
	snprint(name, sizeof name, "#ε/core%4.4d/ctr%2.2d", core, ncounters(core));
	check(access(name, AEXIST) < 0, "no counter past the last", "%s exists", name);
	check(access("#ε/core0999", AEXIST) < 0, "no core 999", "exists");
}

static void
stress(int c, int ms)
{
	uvlong prev, v;
	vlong end;
	int i;

	end = nsec() + (vlong)ms*1000000;
	prev = rdctr(c, 0);
	for(i = 0; nsec() < end; i++){
		v = rdctr(c, 0);
		if(v == ~0ULL || v < prev){
			print("FAIL stress: read %d: %llud after %llud\n", i, v, prev);
			exits("fail");
		}
		prev = v;
	}
	print("ok stress %d\n", i);
	exits(nil);
}

/* round trips of the process to its TC, from /proc/n/core */
static long
roundtrips(void)
{
	char name[64], buf[64], *f[2];

	snprint(name, sizeof name, "/proc/%d/core", getpid());
	if(readfile(name, buf, sizeof buf) == nil || tokenize(buf, f, 2) != 2)
		return -1;
	return strtol(f[1], nil, 10);
}

/* spin for ms by the cycle counter, no system call */
static void
quiet(int ms)
{
	uvlong t0, t;
	long r0, r1;

	r0 = roundtrips();
	cycles(&t0);
	do
		cycles(&t);
	while(t - t0 < _tos->cyclefreq/1000*ms);
	r1 = roundtrips();
	print("roundtrips %ld\n", r1 - r0);
	exits(nil);
}

static void
wire(int c)
{
	char name[64], buf[32];

	snprint(name, sizeof name, "/proc/%d/ctl", getpid());
	snprint(buf, sizeof buf, "wired %d", c);
	if(writefile(name, buf) < 0)
		sysfatal("wire: %r");
	sleep(10);
}

void
main(int argc, char **argv)
{
	char buf[1024];
	int role, other, spins;

	if(argc >= 4 && strcmp(argv[1], "stress") == 0)
		stress(atoi(argv[2]), atoi(argv[3]));
	if(argc >= 3 && strcmp(argv[1], "quiet") == 0)
		quiet(atoi(argv[2]));
	if(argc < 3)
		sysfatal("usage: pmctest tc|ac other [-s]");
	other = atoi(argv[2]);
	spins = argc > 3 && strcmp(argv[3], "-s") == 0;
	if(strcmp(argv[1], "tc") == 0)
		wire(1);
	core = getcore(&role);
	print("core %d role %d\n", core, role);
	if(strcmp(argv[1], "ac") == 0)
		check(role == NIXAC, "on an AC", "role %d", role);
	else
		check(role == NIXTC && core == 1, "wired to TC 1", "core %d role %d", core, role);

	if(readfile("#ε/ctrdesc", buf, sizeof buf) == nil || ncounters(core) == 0){
		print("SKIP pmc: no counters\n");
		exits(nil);
	}
	print("counters %d\n", ncounters(core));
	if(strstr(buf, "instructions\n") != nil){
		ev = "instructions";
		isinstr = 1;
	}else if(strstr(buf, "cycles\n") != nil)
		ev = "cycles";
	else{
		print("SKIP pmc: neither instructions nor cycles\n");
		exits(nil);
	}
	print("event %s\n", ev);

	owncore();
	othercore(other, spins);
	errors();
	confpmc(-1, 0, 0, nil);
	exits(failed? "fail": nil);
}
