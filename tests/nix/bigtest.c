/*
 * Segments of 2 MB pages (segattach(2) class "big"), run by
 * tests/nix/big.sh:
 *	bigtest			on a time-sharing core
 *	execac -c n bigtest	the same on an AC (forked children on a TC)
 * Prints "ok name" or "FAIL name: why" per check, then "done n failed".
 */
#include <u.h>
#include <libc.h>

enum
{
	MB = 1024*1024,
	Big = 2*MB,
	Segsz = 8*MB,
};

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

static void
check(char *name, int c, char *why)
{
	if(c)
		ok(name);
	else
		fail(name, "%s", why);
}

static uintptr
pat(uintptr i, int seed)
{
	return i*2654435761UL + seed;
}

/* one word in each 4K page */
static void
fill(uintptr *p, uintptr n, int seed)
{
	uintptr i;

	for(i = 0; i < n/sizeof(uintptr); i += 4096/sizeof(uintptr))
		p[i] = pat(i, seed);
}

static uintptr
cmp(uintptr *p, uintptr n, int seed)
{
	uintptr i;

	for(i = 0; i < n/sizeof(uintptr); i += 4096/sizeof(uintptr))
		if(p[i] != pat(i, seed))
			return i*sizeof(uintptr)+1;
	return 0;
}

static int
iszero(uchar *p, uintptr n)
{
	uintptr i;

	for(i = 0; i < n; i++)
		if(p[i] != 0)
			return 0;
	return 1;
}

/* the size of the segment at va in /proc/$pid/segment, or 0 */
static uintptr
segsize(void *va, char *type)
{
	char buf[4096], *f[8], *l, *e;
	int fd, n;
	uintptr b, t;

	snprint(buf, sizeof buf, "/proc/%d/segment", getpid());
	if((fd = open(buf, OREAD)) < 0)
		return 0;
	n = readn(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		return 0;
	buf[n] = 0;
	for(l = buf; l < buf+n; l = e+1){
		if((e = strchr(l, '\n')) == nil)
			e = buf+n;
		*e = 0;
		if(tokenize(l, f, nelem(f)) < 3)
			continue;
		b = strtoull(f[1], nil, 16);
		t = strtoull(f[2], nil, 16);
		if(b == (uintptr)va){
			strecpy(type, type+16, f[0]);
			return t-b;
		}
	}
	return 0;
}

void
main(void)
{
	uintptr *p, *q, off;
	char type[16];
	Waitmsg *w;
	int pid;

	p = segattach(0, "big", 0, Segsz);
	if(p == (void*)-1){
		fail("segattach big", "%r");
		print("done %d failed\n", failed);
		exits("fail");
	}
	ok("segattach big");
	check("aligned to 2 MB", ((uintptr)p & (Big-1)) == 0, smprint("va %#p", p));
	type[0] = 0;
	check("/proc/n/segment: Sticky, 8 MB", segsize(p, type) == Segsz && strcmp(type, "Sticky") == 0,
		smprint("%s %#p", type, segsize(p, type)));
	check("zeroed", iszero((uchar*)p, Segsz), "not zero");
	fill(p, Segsz, 1);
	off = cmp(p, Segsz, 1);
	check("write and read every page", off == 0, smprint("at %#p", off-1));

	/* shared across fork, like a shared segment */
	switch(pid = fork()){
	case -1:
		fail("fork", "%r");
		break;
	case 0:
		if(cmp(p, Segsz, 1) != 0)
			exits("child sees other data");
		fill(p, Segsz, 2);
		exits(nil);
	default:
		w = wait();
		check("the child sees it", w != nil && w->msg[0] == 0, w != nil? w->msg: "no wait");
		free(w);
		off = cmp(p, Segsz, 2);
		check("the child's writes are seen", off == 0, smprint("at %#p", off-1));
	}
	USED(pid);

	check("segbrk refused", segbrk(p, (char*)p+Segsz+Big) == (void*)-1, "it grew");

	/* rounded to 2 MB */
	q = segattach(0, "big", 0, 1);
	if(q == (void*)-1)
		fail("1 byte: one 2 MB page", "%r");
	else{
		check("1 byte: one 2 MB page", segsize(q, type) == Big && ((uintptr)q & (Big-1)) == 0,
			smprint("va %#p size %#p", q, segsize(q, type)));
		segdetach(q);
	}

	/* far too much (segattach's len is a ulong): an error, not a hang */
	q = segattach(0, "big", 0, 0xFFE00000UL);
	check("4 GB refused", q == (void*)-1, "attached");
	if(q != (void*)-1)
		segdetach(q);

	/* only the hostowner: contiguous memory is scarce */
	switch(fork()){
	case -1:
		fail("none refused", "fork: %r");
		break;
	case 0:
		{
			int fd;
			char err[ERRMAX];

			if((fd = open("/dev/user", OWRITE)) < 0 || write(fd, "none", 4) != 4)
				exits("can't become none");
			close(fd);
			if(segattach(0, "big", 0, Big) != (void*)-1)
				exits("none attached a big segment");
			rerrstr(err, sizeof err);
			exits(strstr(err, "perm") != nil? nil: err);
		}
	default:
		w = wait();
		check("none refused", w != nil && w->msg[0] == 0, w != nil? w->msg: "no wait");
		free(w);
	}

	/* gone with segdetach; 4K pages at the same place work again */
	if(segdetach(p) < 0)
		fail("segdetach", "%r");
	else
		ok("segdetach");
	q = segattach(0, "memory", p, Segsz);
	if(q != p)
		fail("memory at the same va", "%p: %r", q);
	else{
		check("memory at the same va: zero", iszero((uchar*)q, Segsz), "old data");
		fill(q, Segsz, 3);
		off = cmp(q, Segsz, 3);
		check("memory at the same va: write and read", off == 0, smprint("at %#p", off-1));
		segdetach(q);
	}

	print("done %d failed\n", failed);
	exits(failed? "fail": nil);
}
