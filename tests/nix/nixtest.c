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

/* /proc/n/core: the core and the round trips to it */
static int
coreinfo(ulong *nicc)
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
	if(tokenize(buf, f, 2) != 2)
		return -1;
	if(nicc != nil)
		*nicc = strtoul(f[1], nil, 10);
	return atoi(f[0]);
}

static int
mycore(void)
{
	return coreinfo(nil);
}

/*
 * the pages were faulted in before we came to the AC (prepaging):
 * touching 256 bss pages we never used takes no round trips but
 * those of reading /proc/n/core (open, read, close)
 */
enum { Npre = 256 };
static char prebuf[Npre*4096];

/* round trips for touching npg pages of a new shared segment */
static long
segtouch(int npg)
{
	ulong n0, n1;
	char *p;
	int i;

	p = segattach(0, "shared", nil, npg*4096);
	if(p == (char*)-1)
		return -1;
	coreinfo(&n0);
	for(i = 0; i < npg; i++)
		p[i*4096] = i;
	coreinfo(&n1);
	segdetach(p);
	return n1 - n0;
}

/*
 * segattach prepages while prepaging is on (execac set it);
 * rfork(RFCPREPAGE) turns it off, rfork(RFPREPAGE) on again
 */
static void
segprepagetest(void)
{
	long on, off, again;

	on = segtouch(64);
	rfork(RFCPREPAGE);
	off = segtouch(64);
	rfork(RFPREPAGE);
	again = segtouch(64);
	if(on < 0 || on > 16)
		fail("segattach prepage", "%ld round trips for 64 pages", on);
	else
		ok("segattach prepage");
	if(off < 64 || again < 0 || again > 16)
		fail("rfork RFCPREPAGE/RFPREPAGE", "%ld off, %ld on again", off, again);
	else
		ok("rfork RFCPREPAGE/RFPREPAGE");
}

static void
prepagetest(void)
{
	ulong n0, n1;
	int i;

	if(coreinfo(&n0) <= 0){
		fail("prepage", "no core info");
		return;
	}
	for(i = 0; i < Npre; i++)
		prebuf[i*4096] = i;
	coreinfo(&n1);
	if(n1 - n0 > 16)
		fail("prepage", "%lud round trips for %d pages", n1 - n0, Npre);
	else
		ok("prepage");
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

/* getcore(2), rfork RFCCORE/RFCORE; last: they move the process */
static void
coretest(int want)
{
	int role, c, pid;
	Waitmsg *w;
	char *p;

	c = getcore(&role);
	if(c != want || role != NIXAC)
		fail("getcore", "core %d role %d, want %d %d", c, role, want, NIXAC);
	else
		ok("getcore");

	/* a child asking for an AC: one if there is a free one, else a TC */
	switch(pid = rfork(RFPROC|RFCORE|RFFDG)){
	case -1:
		fail("rfork RFPROC|RFCORE", "rfork: %r");
		break;
	case 0:
		sleep(0);	/* enters the kernel: moves */
		sleep(0);
		c = getcore(&role);
		exits(role == NIXAC && c > 0 && c == mycore()? "ac":
			role == NIXTC && mycore() == 0? "tc": "bad");
	default:
		w = wait();
		/* the message is "name pid: status" */
		if(w == nil || w->pid != pid || (p = strrchr(w->msg, ' ')) == nil
		|| strcmp(p+1, "ac") != 0 && strcmp(p+1, "tc") != 0)
			fail("rfork RFPROC|RFCORE", "child: %s", w != nil? w->msg: "no wait");
		else
			ok("rfork RFPROC|RFCORE");
		free(w);
	}

	rfork(RFCCORE);
	sleep(0);	/* the round trip that takes us back */
	c = getcore(&role);
	if(role != NIXTC || mycore() != 0)
		fail("rfork RFCCORE", "core %d role %d, proc core %d", c, role, mycore());
	else
		ok("rfork RFCCORE");

	if(rfork(RFCORE) < 0){
		fail("rfork RFCORE", "rfork: %r");
		return;
	}
	sleep(0);
	c = getcore(&role);
	if(role != NIXAC || c <= 0 || mycore() != c)
		fail("rfork RFCORE", "core %d role %d, proc core %d", c, role, mycore());
	else
		ok("rfork RFCORE");
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
	prepagetest();
	segprepagetest();
	fputest();
	notetest();
	flushtest();
	spintest();
	core = mycore();
	if(want > 0 && core != want)
		fail("core after", "on core %d, want %d", core, want);
	else
		ok("core after");
	coretest(want);
	exits(failed? "failed": nil);
}
