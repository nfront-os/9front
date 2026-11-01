/*
 * NIX semaphores and tubes (upsem(2), tsend(2), tube(3)), run by
 * tests/nix/sem.sh on a TC and on an AC:
 *	semtest [tag [test...]]
 *	execac -c n /tests/semtest ac
 * The children (rfork RFPROC|RFMEM) stay on TCs, so on an AC one
 * side of each test runs there and the other on a TC.
 * Prints "ok tag name" or "FAIL tag name: why" per check.
 */
#include <u.h>
#include <libc.h>
#include <tube.h>
#include <seg.h>

static char *tag = "tc";
static int failed;

static void
ok(char *name)
{
	print("ok %s %s\n", tag, name);
}

static void
fail(char *name, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("FAIL %s %s: %s\n", tag, name, buf);
	failed++;
}

/* run f in a child sharing our memory; returns its pid */
static int
spawn(void (*f)(void))
{
	int pid;

	switch(pid = rfork(RFPROC|RFMEM)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		(*f)();
		exits(nil);
	}
	return pid;
}

/* wait for pid; its exit string in msg */
static int
waitfor(int pid, char *msg, int n)
{
	Waitmsg *w;

	while((w = wait()) != nil){
		if(w->pid == pid){
			if(msg != nil)
				strecpy(msg, msg+n, w->msg);
			free(w);
			return 0;
		}
		free(w);
	}
	return -1;
}

enum { Nping = 2000 };

/* the semaphores live in bss, shared by RFMEM */
static int sa, sb, smutex, sk, si, sx, sy;
static int pings, counter;
static int altr;
static int intr, intrerr;

static void
ponger(void)
{
	int i;

	for(i = 0; i < Nping; i++){
		downsem(&sa, 0);
		pings++;
		upsem(&sb);
	}
}

/* ping-pong through two semaphores */
static void
testpingpong(void)
{
	int i, pid;

	sa = sb = 0;
	pings = 0;
	pid = spawn(ponger);
	for(i = 0; i < Nping; i++){
		upsem(&sa);
		downsem(&sb, 0);
		if(pings != i+1){
			fail("pingpong", "round %d: %d pings", i, pings);
			break;
		}
	}
	waitfor(pid, nil, 0);
	if(i == Nping && sa == 0 && sb == 0)
		ok("pingpong");
	else if(i == Nping)
		fail("pingpong", "sa %d sb %d", sa, sb);
}

static void
blockeddown(void)
{
	downsem(&sa, 0);
	pings = 1;
}

/* a down blocks until the up */
static void
testblock(void)
{
	int pid;

	sa = 0;
	pings = 0;
	pid = spawn(blockeddown);
	sleep(300);
	if(pings != 0 || sa != -1){
		fail("block", "before the up: pings %d sa %d", pings, sa);
		upsem(&sa);
		waitfor(pid, nil, 0);
		return;
	}
	upsem(&sa);
	waitfor(pid, nil, 0);
	if(pings == 1 && sa == 0)
		ok("block");
	else
		fail("block", "after the up: pings %d sa %d", pings, sa);
}

/* non-blocking downs */
static void
testnb(void)
{
	sa = 0;
	if(downsem(&sa, 1) != -1 || sa != 0){
		fail("nonblocking", "on 0: sa %d", sa);
		return;
	}
	sa = 2;
	if(downsem(&sa, 1) != 0 || downsem(&sa, 1) != 0 || sa != 0){
		fail("nonblocking", "on 2: sa %d", sa);
		return;
	}
	if(downsem(&sa, 1) != -1 || sa != 0){
		fail("nonblocking", "after: sa %d", sa);
		return;
	}
	ok("nonblocking");
}

enum { Nmutex = 4, Nloop = 2000 };

static void
mutexer(void)
{
	int i, c;

	for(i = 0; i < Nloop; i++){
		downsem(&smutex, 0);
		c = counter;
		if((i & 15) == 0)
			sleep(0);
		counter = c+1;
		upsem(&smutex);
	}
}

/* a semaphore as a lock among Nmutex processes */
static void
testmutex(void)
{
	int i, pid[Nmutex];

	smutex = 1;
	counter = 0;
	for(i = 0; i < Nmutex; i++)
		pid[i] = spawn(mutexer);
	for(i = 0; i < Nmutex; i++)
		waitfor(pid[i], nil, 0);
	if(counter == Nmutex*Nloop && smutex == 1)
		ok("mutex");
	else
		fail("mutex", "counter %d of %d, smutex %d", counter, Nmutex*Nloop, smutex);
}

static void
alter(void)
{
	int *ss[2];

	ss[0] = &sx;
	ss[1] = &sy;
	altr = altsems(ss, 2);
}

/*
 * altsems blocks on both and takes the one upped; the other one
 * keeps no stale down (NIX lost a ticket there)
 */
static void
testalt(void)
{
	int pid, *ss[2];

	sx = sy = 0;
	altr = -2;
	pid = spawn(alter);
	sleep(300);
	if(altr != -2 || sx != -1 || sy != -1){
		fail("altsems", "before the up: r %d sx %d sy %d", altr, sx, sy);
		upsem(&sx);
		upsem(&sy);
		waitfor(pid, nil, 0);
		return;
	}
	upsem(&sy);
	waitfor(pid, nil, 0);
	if(altr != 1 || sx != 0 || sy != 0){
		fail("altsems", "after the up: r %d sx %d sy %d", altr, sx, sy);
		return;
	}
	ok("altsems blocks");
	upsem(&sx);
	if(downsem(&sx, 1) != 0 || sx != 0){
		fail("altsems leaves the other", "sx %d", sx);
		return;
	}
	ok("altsems leaves the other");
	sx = 0;
	sy = 1;
	ss[0] = &sx;
	ss[1] = &sy;
	if(altsems(ss, 2) != 1 || sx != 0 || sy != 0){
		fail("altsems ready", "sx %d sy %d", sx, sy);
		return;
	}
	ok("altsems ready");
}

enum { Nalt = 3, Naltloop = 3000 };
static int altgot[2];
static Lock altlk;

static void
altproducer(void)
{
	int i;

	for(i = 0; i < Naltloop; i++){
		if(nrand(2))
			upsem(&sx);
		else
			upsem(&sy);
		if((i & 7) == 0)
			sleep(0);
	}
}

static void
altconsumer(void)
{
	int i, r, *ss[2];

	ss[0] = &sx;
	ss[1] = &sy;
	for(i = 0; i < Naltloop; i++){
		r = altsems(ss, 2);
		if(r < 0 || r > 1)
			break;
		lock(&altlk);
		altgot[r]++;
		unlock(&altlk);
	}
}

/* Nalt processes up either of two semaphores, Nalt alt on both: no ticket lost */
static void
testaltstress(void)
{
	int i, pid[2*Nalt];

	sx = sy = 0;
	altgot[0] = altgot[1] = 0;
	for(i = 0; i < Nalt; i++){
		pid[2*i] = spawn(altconsumer);
		pid[2*i+1] = spawn(altproducer);
	}
	for(i = 0; i < 2*Nalt; i++)
		waitfor(pid[i], nil, 0);
	if(sx != 0 || sy != 0 || altgot[0] + altgot[1] != Nalt*Naltloop)
		fail("altsems stress", "sx %d sy %d got %d+%d of %d", sx, sy, altgot[0], altgot[1], Nalt*Naltloop);
	else
		ok("altsems stress");
}

/* the kernel's semalt on its own (libc's altsems tries first) */
static void
testsemalt(void)
{
	int *ss[2];

	sx = 0;
	sy = 1;
	ss[0] = &sx;
	ss[1] = &sy;
	if(semalt(ss, 2) != 1 || sx != 0 || sy != 0){
		fail("semalt", "sx %d sy %d", sx, sy);
		return;
	}
	ss[0] = nil;
	if(semalt(ss, 0) != -1 || semalt(ss, 17) != -1){
		fail("semalt", "bad counts accepted");
		return;
	}
	ok("semalt");
}

static void
killed(void)
{
	downsem(&sk, 0);
	pings = 1;
}

/* a process blocked in downsem dies of a kill note, its down is given back */
static void
testkill(void)
{
	int pid;
	char msg[ERRMAX];

	sk = 0;
	pings = 0;
	pid = spawn(killed);
	sleep(300);
	if(sk != -1){
		fail("kill", "not blocked: sk %d", sk);
		upsem(&sk);
		waitfor(pid, nil, 0);
		return;
	}
	postnote(PNPROC, pid, "kill");
	msg[0] = 0;
	if(waitfor(pid, msg, sizeof msg) < 0){
		fail("kill", "wait: %r");
		return;
	}
	if(pings != 0 || sk != 0)
		fail("kill", "pings %d sk %d msg %s", pings, sk, msg);
	else
		ok("kill");
}

static void
onalarm(void*, char *msg)
{
	if(strcmp(msg, "alarm") == 0)
		noted(NCONT);
	noted(NDFLT);
}

static void
interrupted(void)
{
	char err[ERRMAX];

	notify(onalarm);
	alarm(300);
	intr = downsem(&si, 0);
	rerrstr(err, sizeof err);
	intrerr = strstr(err, "interrupt") != nil;
}

/* a note that is handled makes downsem fail with "interrupted" */
static void
testintr(void)
{
	int pid;

	si = 0;
	intr = -2;
	pid = spawn(interrupted);
	waitfor(pid, nil, 0);
	if(intr != -1 || !intrerr || si != 0)
		fail("interrupted", "downsem %d, errstr %d, si %d", intr, intrerr, si);
	else
		ok("interrupted");
}

static void
badsleep(void)
{
	semsleep((int*)0x10, 0);
}

/* a bad address kills the caller, as for other system calls */
static void
testbad(void)
{
	char msg[ERRMAX];

	msg[0] = 0;
	waitfor(spawn(badsleep), msg, sizeof msg);
	if(strstr(msg, "bad address") == nil)
		fail("bad address", "exit %s", msg);
	else
		ok("bad address");
}

enum { Ntube = 5000, Tsz = 4 };

static Tube *t1, *t2;
static int tbad;

static void
tsender(void)
{
	int i;

	for(i = 0; i < Ntube; i++)
		tsend(t1, &i);
}

/* Ntube messages through a tube of Tsz */
static void
testtube(void)
{
	int i, v, pid;

	t1 = newtube(sizeof(int), Tsz);
	if(t1 == nil){
		fail("tube", "newtube: %r");
		return;
	}
	pid = spawn(tsender);
	for(i = 0; i < Ntube; i++){
		trecv(t1, &v);
		if(v != i){
			fail("tube", "message %d is %d", i, v);
			break;
		}
	}
	waitfor(pid, nil, 0);
	if(i == Ntube && t1->nmsg == 0 && t1->nhole == Tsz)
		ok("tube");
	else if(i == Ntube)
		fail("tube", "nmsg %d nhole %d", t1->nmsg, t1->nhole);
	freetube(t1);
}

/*
 * named tubes: found by name, also by an unrelated process (a fork
 * that execs semtest anew: it has none of our memory but the segment)
 */
enum { Nnamed = 500 };

static void
namedpeer(void)
{
	Tube *t;
	int i;

	t = namedtube("ntest!pp", sizeof(int), 4, 0);
	if(t == nil)
		exits(smprint("peer: namedtube: %r"));
	for(i = 0; i < Nnamed; i++)
		if(tsend(t, &i) < 0)
			exits("peer: tsend");
	exits(nil);
}

static void
testnamed(void)
{
	Tube *t, *t2;
	Waitmsg *w;
	int i, v, pid;

	if(namedtube("ntnone!x", sizeof(int), 4, 0) != nil){
		fail("named", "found a tube in a segment never made");
		return;
	}
	t = namedtube("ntest!pp", sizeof(int), 4, 1);
	if(t == nil){
		fail("named", "namedtube: %r");
		return;
	}
	t2 = namedtube("ntest!pp", sizeof(int), 4, 0);
	if(t2 != t){
		fail("named", "a second lookup gives %#p, not %#p", t2, t);
		return;
	}
	if(namedtube("ntest!pp", sizeof(vlong), 4, 0) != nil){
		fail("named", "a wrong message size is taken");
		return;
	}
	if(namedtube("ntest!other", sizeof(int), 4, 0) != nil){
		fail("named", "found a tube never made");
		return;
	}
	ok("named: lookups");
	switch(pid = fork()){
	case -1:
		fail("named", "fork: %r");
		return;
	case 0:
		execl(argv0, argv0, "namedpeer", nil);
		exits("exec");
	}
	/* a peer that fails sends nothing: no waiting for ever */
	notify(onalarm);
	alarm(20000);
	for(i = 0; i < Nnamed; i++){
		if(trecv(t, &v) < 0 || v != i){
			fail("named", "message %d is %d", i, v);
			break;
		}
	}
	alarm(0);
	notify(nil);
	w = wait();
	if(w == nil || w->pid != pid || w->msg[0] != 0)
		fail("named", "peer: %s", w != nil? w->msg: "no wait");
	else if(i == Nnamed)
		ok("named: an unrelated process sends through it");
	free(w);
}

/* non-blocking: empty and full */
static void
testnbtube(void)
{
	int i, v;

	t1 = newtube(sizeof(int), Tsz);
	if(nbtrecv(t1, &v) != -1){
		fail("nbtube", "nbtrecv on an empty tube");
		return;
	}
	for(i = 0; i < Tsz; i++)
		if(nbtsend(t1, &i) != 0){
			fail("nbtube", "nbtsend %d", i);
			return;
		}
	i = 99;
	if(nbtsend(t1, &i) != -1){
		fail("nbtube", "nbtsend on a full tube");
		return;
	}
	for(i = 0; i < Tsz; i++)
		if(nbtrecv(t1, &v) != 0 || v != i){
			fail("nbtube", "nbtrecv %d: %d", i, v);
			return;
		}
	if(t1->nmsg != 0 || t1->nhole != Tsz){
		fail("nbtube", "nmsg %d nhole %d", t1->nmsg, t1->nhole);
		return;
	}
	freetube(t1);
	ok("nbtube");
}

static void
latesender(void)
{
	int v;

	sleep(300);
	v = 42;
	tsend(t2, &v);
}

/* talt blocks on two tubes, gets the message of the second */
static void
testtalt(void)
{
	int pid, r, v1, v2, v3;
	Talt a[3];

	t1 = newtube(sizeof(int), Tsz);
	t2 = newtube(sizeof(int), Tsz);
	v1 = v2 = v3 = -1;
	a[0].t = t1; a[0].m = &v1; a[0].op = TRCV;
	a[1].t = t1; a[1].m = &v3; a[1].op = TNBRCV;
	a[2].t = t2; a[2].m = &v2; a[2].op = TRCV;
	pid = spawn(latesender);
	r = talt(a, 3);
	waitfor(pid, nil, 0);
	if(r != 2 || v2 != 42 || v1 != -1 || t1->nmsg != 0 || t2->nmsg != 0)
		fail("talt", "r %d v1 %d v2 %d nmsg %d %d", r, v1, v2, t1->nmsg, t2->nmsg);
	else
		ok("talt");
	/* a non-blocking entry that can proceed is taken at once */
	v1 = 7;
	tsend(t1, &v1);
	v1 = v3 = -1;
	r = talt(a, 3);
	if(r != 1 || v3 != 7 || v1 != -1 || t1->nmsg != 0)
		fail("talt ready", "r %d v1 %d v3 %d", r, v1, v3);
	else
		ok("talt ready");
	freetube(t1);
	freetube(t2);
}

/* #=: the kernel's entry points, by kernel processes */
static void
testkernel(void)
{
	int fd, i, n;
	uintptr p;

	fd = open("#=/ctl", ORDWR);
	if(fd < 0){
		fail("kernel", "open #=/ctl: %r");
		return;
	}
	/* 10 upsems by the kernel, at 300 ms */
	sa = 0;
	if(fprint(fd, "up %#p", &sa) < 0){
		fail("kernel upsem", "ctl: %r");
		close(fd);
		return;
	}
	for(i = 0; i < 10; i++)
		if(downsem(&sa, 0) < 0)
			break;
	if(i == 10 && sa == 0)
		ok("kernel upsem");
	else
		fail("kernel upsem", "%d downs, sa %d", i, sa);

	/* 10 downsems by the kernel */
	sb = 0;
	fprint(fd, "down %#p", &sb);
	sleep(300);
	n = sb;
	for(i = 0; i < 10; i++){
		upsem(&sb);
		sleep(10);
	}
	for(i = 0; i < 100 && sb != 0; i++)
		sleep(10);
	if(sb == 0 && n == -1)
		ok("kernel downsem");
	else
		fail("kernel downsem", "sb %d, before the ups %d", sb, n);

	/* 10 pointers sent by the kernel */
	t1 = newtube(sizeof(uintptr), 5);
	fprint(fd, "send %#p", t1);
	for(i = 0; i < 10; i++){
		p = 0;
		trecv(t1, &p);
		if(p != 0x1000+i)
			break;
	}
	if(i == 10)
		ok("kernel tsend");
	else
		fail("kernel tsend", "message %d is %#p", i, p);

	/* 10 received */
	t2 = newtube(sizeof(uintptr), 5);
	fprint(fd, "recv %#p", t2);
	for(i = 0; i < 10; i++){
		p = i;
		tsend(t2, &p);
	}
	for(i = 0; i < 100 && t2->nhole != 5; i++)
		sleep(10);
	if(t2->nhole == 5 && t2->nmsg == 0)
		ok("kernel trecv");
	else
		fail("kernel trecv", "nhole %d nmsg %d", t2->nhole, t2->nmsg);

	/* a tube of ints is no tube of pointers */
	t1->msz = 3;
	if(fprint(fd, "send %#p", t1) >= 0)
		fail("kernel tube check", "msz 3 accepted");
	else
		ok("kernel tube check");
	close(fd);
}

static int
nkprocs(char *name)
{
	int fd, i, n, nd;
	Dir *d;
	char buf[128], *f[2];

	fd = open("/proc", OREAD);
	if(fd < 0)
		return -1;
	nd = dirreadall(fd, &d);
	close(fd);
	n = 0;
	for(i = 0; i < nd; i++){
		snprint(buf, sizeof buf, "/proc/%s/status", d[i].name);
		if((fd = open(buf, OREAD)) < 0)
			continue;
		buf[0] = 0;
		buf[read(fd, buf, sizeof buf - 1)] = 0;
		close(fd);
		if(tokenize(buf, f, 2) >= 1 && strcmp(f[0], name) == 0)
			n++;
	}
	free(d);
	return n;
}

/* #=: a file descriptor and two tubes */
static void
testtio(void)
{
	int fd, p[2], q[2], n, i, base, lk;
	char buf[64], m[8];

	/* one semtest at a time: it counts the kernel processes tin and tout (srv(3) refuses a second create) */
	for(i = 0; (lk = create("/srv/semtest.lock", OWRITE|ORCLOSE, 0600)) < 0 && i < 500; i++)
		sleep(20);

	t1 = newtube(sizeof m, 4);
	t2 = newtube(sizeof m, 4);
	if(pipe(p) < 0 || pipe(q) < 0){
		fail("tio", "pipe: %r");
		return;
	}
	base = nkprocs("tin") + nkprocs("tout");
	fd = open("#=/ctl", ORDWR);
	if(fd < 0 || fprint(fd, "%d %#p 0", p[0], t1) < 0){
		fail("tio", "in: %r");
		return;
	}
	n = open("#=/ctl", ORDWR);
	if(n < 0 || fprint(n, "%d 0 %#p", q[1], t2) < 0){
		fail("tio", "out: %r");
		return;
	}
	write(p[1], "hello", 5);
	memset(m, 'x', sizeof m);
	trecv(t1, m);
	if(memcmp(m, "hello\0\0\0", 8) != 0){
		fail("tio in", "got %.8s", m);
	}else
		ok("tio in");
	tsend(t2, "abcdefgh");
	i = read(q[0], buf, sizeof buf);
	if(i != 8 || memcmp(buf, "abcdefgh", 8) != 0)
		fail("tio out", "read %d", i);
	else
		ok("tio out");
	seek(fd, 0, 0);
	i = read(fd, buf, sizeof buf-1);
	buf[i > 0 ? i : 0] = 0;
	snprint(m, sizeof m, "%d", p[0]);
	if(strncmp(buf, m, strlen(m)) != 0)
		fail("tio conf", "read %s", buf);
	else
		ok("tio conf");
	if(nkprocs("tin") + nkprocs("tout") != base+2)
		fail("tio procs", "%d processes", nkprocs("tin") + nkprocs("tout") - base);
	else
		ok("tio procs");
	close(fd);
	close(n);
	for(i = 0; i < 50 && nkprocs("tin") + nkprocs("tout") != base; i++)
		sleep(20);
	if(nkprocs("tin") + nkprocs("tout") != base)
		fail("tio close", "%d processes left", nkprocs("tin") + nkprocs("tout") - base);
	else
		ok("tio close");
	close(p[0]);
	close(p[1]);
	close(q[0]);
	close(q[1]);
	if(lk >= 0)
		close(lk);
}

static struct {
	char	*name;
	void	(*f)(void);
} tests[] = {
	"nb",		testnb,
	"block",	testblock,
	"pingpong",	testpingpong,
	"mutex",	testmutex,
	"alt",		testalt,
	"altstress",	testaltstress,
	"semalt",	testsemalt,
	"kill",		testkill,
	"intr",		testintr,
	"bad",		testbad,
	"tube",		testtube,
	"nbtube",	testnbtube,
	"talt",		testtalt,
	"named",	testnamed,
	"kernel",	testkernel,
	"tio",		testtio,
};

/* semtest [tag [test...]]: all tests, or those named */
void
main(int argc, char *argv[])
{
	int i, j, role;

	argv0 = argv[0];	/* testnamed execs us */
	if(argc > 1 && strcmp(argv[1], "namedpeer") == 0)
		namedpeer();
	if(argc > 1)
		tag = argv[1];
	print("semtest %s on core %d\n", tag, getcore(&role));
	for(i = 0; i < nelem(tests); i++){
		for(j = 2; j < argc; j++)
			if(strcmp(argv[j], tests[i].name) == 0)
				break;
		if(argc <= 2 || j < argc)
			(*tests[i].f)();
	}
	print("semtest %s done, %d failed\n", tag, failed);
	exits(failed ? "failed" : nil);
}
