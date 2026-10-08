/*
 * libc unit tests, built with the gcc port toolchain and run inside
 * 9front (tests/guest/run.sh). They target what host tests cannot see:
 * kencc vs. gcc semantics (unsigned-preserving promotion, enumerator
 * types, 32-bit ulong vs. 64-bit long), the System V ports of the
 * amd64 assembly and the kernel interface (system call stubs, notes,
 * exec, rendezvous).
 *
 * Output: one "ok name" or "FAIL name: detail" line per check, then
 * "libctest: N ok, M failed".
 */
#include <u.h>
#include <libc.h>
#include <bio.h>
#include <regexp.h>

static int nok, nfail;

static int intcmp(void*, void*);

static void
check(char *name, int ok, char *fmt, ...)
{
	char buf[256];
	va_list arg;

	if(ok){
		nok++;
		print("ok %s\n", name);
		return;
	}
	nfail++;
	buf[0] = 0;
	if(fmt != nil){
		va_start(arg, fmt);
		vsnprint(buf, sizeof buf, fmt, arg);
		va_end(arg);
	}
	print("FAIL %s: %s\n", name, buf);
}

static void
testfmt(void)
{
	char buf[128];

	snprint(buf, sizeof buf, "%d %ud %ux %x %lld %llux", -5, 4000000000U, 0xdeadbeef, -1, -1234567890123LL, 0xfedcba9876543210ULL);
	/* print(2): without u the integer verbs, x included, are signed */
	check("fmt integers", strcmp(buf, "-5 4000000000 deadbeef -1 -1234567890123 fedcba9876543210") == 0, "%s", buf);
	snprint(buf, sizeof buf, "%ld %lud %lux", (long)-7, (ulong)0xffffffff, (ulong)0x80000000);
	check("fmt long/ulong", strcmp(buf, "-7 4294967295 80000000") == 0, "%s", buf);
	snprint(buf, sizeof buf, "%5.2f|%g|%e", 3.14159, 0.5, 12345.0);
	check("fmt floating point", strcmp(buf, " 3.14|0.5|1.234500e+04") == 0, "%s", buf);
	snprint(buf, sizeof buf, "%-4s|%4s|%c|%C|%q", "ab", "cd", 'x', 0x263A, "a b");
	check("fmt strings/runes", strcmp(buf, "ab  |  cd|x|☺|'a b'") == 0, "%s", buf);
	werrstr("custom %d", 42);
	snprint(buf, sizeof buf, "%r");
	check("fmt %r/werrstr", strcmp(buf, "custom 42") == 0, "%s", buf);
	snprint(buf, sizeof buf, "%p %#p", (void*)0x1234, (void*)0x1234);
	check("fmt %p/%#p", strcmp(buf, "1234 0x1234") == 0, "%s", buf);
}

static void
teststrto(void)
{
	char *e;

	check("strtol", strtol("-123x", &e, 10) == -123 && *e == 'x', nil);
	check("strtoul 32-bit", strtoul("4294967295", nil, 10) == 4294967295UL, "%lud", strtoul("4294967295", nil, 10));
	check("strtoll", strtoll("-9000000000000", nil, 0) == -9000000000000LL, nil);
	check("strtoull hex", strtoull("0xffffffffffffffff", nil, 0) == ~0ULL, nil);
	check("atoi", atoi("  77") == 77, nil);
	check("strtod", strtod("2.5e3", nil) == 2500.0, nil);
}

static void
testrune(void)
{
	Rune r, rr;
	char buf[UTFmax+1];
	int n, bad;

	bad = 0;
	for(r = 0; r < 0x10FFFF; r += 7){
		if(r >= 0xD800 && r < 0xE000)
			continue;
		n = runetochar(buf, &r);
		if(chartorune(&rr, buf) != n || rr != r){
			bad = r;
			break;
		}
	}
	check("runetochar/chartorune round trip", bad == 0, "rune %#x", bad);
	check("toupperrune/tolowerrune", toupperrune(L'ä') == L'Ä' && tolowerrune(L'Ω') == L'ω', nil);
	check("isalpharune/isdigitrune", isalpharune(L'ß') && !isalpharune('1') && isdigitrune('7'), nil);
	check("utflen/utfrune", utflen("aäb€") == 4 && utfrune("aäb€", L'€') != nil, nil);
}

static void
testdate(void)
{
	Tm tm;
	char buf[64];

	tmtime(&tm, 0, nil);
	check("tmtime epoch", tm.year == 70 && tm.mon == 0 && tm.mday == 1 && tm.hour == 0, "%d-%d-%d", tm.year, tm.mon, tm.mday);
	tmtime(&tm, 1791394412, nil);
	snprint(buf, sizeof buf, "%τ", tmfmt(&tm, "YYYY-MM-DD hh:mm:ss"));
	check("tmfmt 2026", strcmp(buf, "2026-10-07 17:33:32") == 0, "%s", buf);
	check("tmnorm round trip", tmnorm(&tm) == 1791394412, "%lld", tmnorm(&tm));
	tmtime(&tm, 2147483648LL+86400*400, nil);
	check("tmnorm after 2038", tmnorm(&tm) == 2147483648LL+86400*400, "%lld", tmnorm(&tm));
	tmtime(&tm, -86400LL*365, nil);
	check("tmtime before epoch", tm.year == 69 && tmnorm(&tm) == -86400LL*365, "year %d", tm.year);
	check("tmparse", tmparse(&tm, "YYYY-MM-DD", "1999-12-31", nil, nil) != nil && tmnorm(&tm) == 946598400, nil);
}

static void
testtime(void)
{
	vlong a, b;
	long t;

	a = nsec();
	sleep(20);
	b = nsec();
	check("nsec monotonic, sleep(20)", b - a >= 15*1000*1000LL, "delta %lld", b - a);
	t = time(nil);
	check("time vs nsec", t - b/1000000000LL <= 1 && t > 1600000000, "time %ld", t);
}

static void
testmem(void)
{
	char *p, *q;
	int i, a[100];

	p = malloc(100000);
	memset(p, 'a', 100000);
	q = realloc(p, 200000);
	check("malloc/realloc keep contents", q != nil && q[99999] == 'a', nil);
	free(q);
	for(i = 0; i < 100; i++)
		a[i] = (i*37) % 100;
	qsort(a, 100, sizeof a[0], (int(*)(void*, void*))intcmp);
	check("qsort", a[0] == 0 && a[50] == 50 && a[99] == 99, nil);
	check("muldiv/umuldiv", muldiv(-7, 3, 2) == -10 && umuldiv(4000000000U, 3, 4) == 3000000000U, nil);
}

static int
intcmp(void *a, void *b)
{
	return *(int*)a - *(int*)b;
}

static jmp_buf jb;

static void
deep(int n)
{
	if(n == 0)
		longjmp(jb, 7);
	deep(n-1);
}

static void
testjmp(void)
{
	volatile int v;
	int r;

	v = 1;
	r = setjmp(jb);
	if(r == 0){
		v = 2;
		deep(10);
	}
	check("setjmp/longjmp", r == 7 && v == 2, "r=%d v=%d", r, v);
}

static int gotnote;

static void
handler(void*, char *msg)
{
	if(strcmp(msg, "libctest") == 0)
		gotnote = 1;
	noted(NCONT);
}

static void
testnote(void)
{
	notify(handler);
	postnote(PNPROC, getpid(), "libctest");
	sleep(50);
	check("notify/postnote", gotnote, nil);
	notify(nil);
}

static void
testnotifynil(void)
{
	Waitmsg *w;

	switch(fork()){
	case 0:
		notify(handler);
		notify(nil);
		postnote(PNPROC, getpid(), "byenote");
		sleep(1000);
		exits("survived");
	}
	w = wait();
	check("notify(nil): note gets the default action", w != nil && strstr(w->msg, "byenote") != nil,
		"%s", w ? w->msg : "no wait");
	free(w);
}

static void
testliterals(void)
{
	char *s;

	/* 8l keeps string literals in the data segment (amd64/rodata.ld) */
	s = "literal";
	s[0] = 'L';
	check("string literals are writable", s[0] == 'L', nil);
}

static int gotalarm;

static void
alarmhandler(void*, char *msg)
{
	if(strcmp(msg, "alarm") == 0){
		gotalarm = 1;
		noted(NCONT);
	}
	noted(NDFLT);
}

static void
testalarm(void)
{
	vlong t;
	long left;

	notify(alarmhandler);
	gotalarm = 0;
	t = nsec();
	alarm(300);
	sleep(1000);	/* interrupted by the alarm note */
	t = nsec() - t;
	check("alarm(300) fires after ~300ms", gotalarm && t >= 250*1000000LL && t < 900*1000000LL,
		"note %d after %lld ms", gotalarm, t/1000000);
	gotalarm = 0;
	alarm(30*1000);
	sleep(100);
	left = alarm(0);
	check("alarm(30000) pending after 100ms", !gotalarm && left > 29000 && left <= 30000,
		"note %d, %ld ms left", gotalarm, left);
	notify(nil);
}

static void
testexec(void)
{
	int p[2], n;
	char buf[64];
	Waitmsg *w;

	pipe(p);
	switch(fork()){
	case 0:
		close(p[0]);
		dup(p[1], 1);
		execl("/bin/echo", "echo", "exec", "ok", nil);
		exits("execl failed");
	}
	close(p[1]);
	n = readn(p[0], buf, sizeof buf - 1);
	buf[n > 0 ? n : 0] = 0;
	close(p[0]);
	w = wait();
	check("fork/execl/pipe/wait", strcmp(buf, "exec ok\n") == 0 && w != nil && w->msg[0] == 0,
		"%q %s", buf, w ? w->msg : "no wait");
	free(w);
}

static void
testrendez(void)
{
	void *v;

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case 0:
		rendezvous((void*)0x1234, (void*)0x5678);
		exits(nil);
	}
	v = rendezvous((void*)0x1234, (void*)0x9abc);
	check("rendezvous exchanges values", v == (void*)0x5678, "%p", v);
}

static void
testfile(void)
{
	int fd;
	Dir *d;
	long n;
	Biobuf *b;
	char *l;

	fd = create("/tmp/libctest", ORDWR, 0666);
	check("create", fd >= 0, "%r");
	if(fd < 0)
		return;
	fprint(fd, "line one\nline two\n");
	check("seek end", seek(fd, 0, 2) == 18, "%lld", seek(fd, 0, 2));
	{
		char buf[4];
		check("seek set + read", seek(fd, 5, 0) == 5 && read(fd, buf, 3) == 3 && memcmp(buf, "one", 3) == 0, nil);
	}
	close(fd);
	d = dirstat("/tmp/libctest");
	check("dirstat length", d != nil && d->length == 18, nil);
	free(d);
	b = Bopen("/tmp/libctest", OREAD);
	l = b ? Brdline(b, '\n') : nil;
	check("Bopen/Brdline", l != nil && strncmp(l, "line one", 8) == 0, nil);
	if(b)
		Bterm(b);
	remove("/tmp/libctest");
	fd = open("/bin", OREAD);
	n = fd >= 0 ? dirreadall(fd, &d) : -1;
	check("dirreadall /bin", n > 100, "%ld entries", n);
	if(n > 0)
		free(d);
	close(fd);
}

static void
testregexp(void)
{
	Reprog *re;
	Resub m[2];

	re = regcomp("b(c+)d");
	memset(m, 0, sizeof m);
	check("regcomp/regexec", re != nil && regexec(re, "abcccde", m, 2) && m[1].sp != nil
		&& m[1].ep - m[1].sp == 3, nil);
	free(re);
}

void
main(void)
{
	tmfmtinstall();
	quotefmtinstall();
	testfmt();
	teststrto();
	testrune();
	testdate();
	testtime();
	testmem();
	testjmp();
	testnote();
	testnotifynil();
	testliterals();
	testalarm();
	testexec();
	testrendez();
	testfile();
	testregexp();
	print("libctest: %d ok, %d failed\n", nok, nfail);
	exits(nfail ? "failures" : nil);
}
