/*
 * NIX application core tests, run on an AC by tests/nix/run.sh:
 *	execac -c n /tests/nixtest n
 * Prints "ok name" or "FAIL name: why" per check.
 */
#include <u.h>
#include <libc.h>

static int failed;

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

static int
mycore(void)
{
	char buf[32];
	int fd, n;

	snprint(buf, sizeof buf, "/proc/%d/core", getpid());
	if((fd = open(buf, OREAD)) < 0)
		return -1;
	n = read(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		return -1;
	buf[n] = 0;
	return atoi(buf);
}

/* floating point, kept in registers across system calls */
static void
fputest(void)
{
	double x, y;
	int i;

	x = 1.0;
	y = 0.0;
	for(i = 1; i <= 2000; i++){
		x *= 1.0001;
		y += sqrt((double)i);
		if(i % 100 == 0)
			sleep(0);	/* a round trip to the TC */
	}
	if(fabs(x - 1.2213905) > 1e-6 || fabs(y - 59650.633) > 0.01)
		fail("fpu", "x %g y %g", x, y);
	else
		ok("fpu");
}

/* a note handler on the AC */
static volatile int alarms;

static int
onnote(void*, char *msg)
{
	if(strcmp(msg, "alarm") == 0){
		alarms++;
		return 1;
	}
	return 0;
}

static void
notetest(void)
{
	vlong t0;
	ulong i;

	atnotify(onnote, 1);
	alarm(200);
	t0 = nsec();
	/* mostly no system calls: the IPI gets us off the AC */
	for(i = 0; alarms == 0; i++)
		if((i & 0xFFFFF) == 0 && nsec() - t0 > 5000000000LL)
			break;
	atnotify(onnote, 0);
	if(alarms != 1)
		fail("note", "alarms %d", alarms);
	else
		ok("note");
}

static void
procstatus(int pid)
{
	char buf[512];
	int fd, n;

	snprint(buf, sizeof buf, "/proc/%d/status", pid);
	if((fd = open(buf, OREAD)) < 0)
		return;
	n = read(fd, buf, sizeof buf-1);
	close(fd);
	if(n > 0){
		buf[n] = 0;
		print("	%d: %.80s\n", pid, buf);
	}
}

/*
 * shared memory changed by a child on a TC while we run on the AC
 * and make no system calls: the child fills a shared region, we
 * check it, the child frees it with segfree (procflushseg). Only the
 * IPI of procflushmmu gets our stale page tables off the AC: then
 * we must read zeros, not the old pattern.
 */
enum { Rounds = 20, Size = 256*1024 };
static char regionbuf[Size+4096];
static volatile int phase, round;
static volatile int counter;

static void
flushtest(void)
{
	int i, r, bad, stale, child;
	volatile char *region;
	vlong t0;

	region = (char*)(((uintptr)regionbuf + 4095) & ~4095);
	phase = 0;
	switch(child = rfork(RFPROC|RFMEM)){
	case -1:
		fail("flush", "rfork: %r");
		return;
	case 0:
		for(r = 0; r < Rounds; r++){
			memset((char*)region, r+1, Size);
			round = r;
			phase = 1;
			while(phase == 1)
				sleep(0);
			if(segfree((char*)region, Size) < 0)
				_exits("segfree");
			phase = 3;
			while(phase == 3)
				sleep(0);
		}
		_exits(nil);
	}
	bad = stale = 0;
	for(r = 0; r < Rounds; r++){
		t0 = nsec();
		while(phase != 1)
			if(nsec() - t0 > 30000000000LL){
				fail("flush", "child hangs in round %d, phase %d", r, phase);
				procstatus(child);
				return;
			}
		for(i = 0; i < Size; i += 4096)
			if(region[i] != (char)(round+1))
				bad++;
		phase = 2;
		while(phase != 3)
			;	/* no system calls */
		for(i = 0; i < Size; i += 4096)
			if(region[i] != 0)
				stale++;
		phase = 4;
	}
	waitpid();
	if(bad || stale)
		fail("flush", "%d bad pages, %d stale pages after segfree", bad, stale);
	else
		ok("flush");
}

/* a TC process counts while we spin without system calls */
static void
spintest(void)
{
	int start;
	vlong t0;

	counter = 0;
	switch(rfork(RFPROC|RFMEM)){
	case -1:
		fail("spin", "rfork: %r");
		return;
	case 0:
		while(counter < 1000000)
			counter++;
		_exits(nil);
	}
	start = counter;
	t0 = nsec();
	while(counter < 1000000 && nsec() - t0 < 10000000000LL)
		;
	waitpid();
	if(counter < 1000000)
		fail("spin", "counter %d (from %d)", counter, start);
	else
		ok("spin");
}

void
main(int argc, char *argv[])
{
	int want, core;

	want = argc > 1? atoi(argv[1]): -1;
	core = mycore();
	if(want > 0 && core != want || core <= 0)
		fail("core", "on core %d, want %d", core, want);
	else
		ok("core");
	fputest();
	notetest();
	flushtest();
	spintest();
	core = mycore();
	if(want > 0 && core != want)
		fail("core after", "on core %d, want %d", core, want);
	else
		ok("core after");
	exits(failed? "failed": nil);
}
