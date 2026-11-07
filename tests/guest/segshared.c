/*
 * segshared: segments whose address the kernel chooses (9front-ng,
 * segment(3) "va 0 len"): each in the shared region at an address no
 * other one has, so that any process can attach any of them (stage 6 of
 * ../9front-ui: Wayland's shared memory).  Two segments get different
 * ranges; another process (rfork without shared memory: its own address
 * space) attaches them by name at those addresses and sees what this one
 * wrote, and this one what it wrote; segattach's own choice (va 0) lies
 * outside the region; a segment removed and detached everywhere gives
 * its range back; a segment given an address of its own in the region is
 * refused.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <u.h>
#include <libc.h>

static int nfail;

static void
ok(char *fmt, ...)
{
	va_list arg;
	char buf[256];

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("ok %s\n", buf);
}

static void
fail(char *fmt, ...)
{
	va_list arg;
	char buf[256];

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("FAIL %s\n", buf);
	nfail++;
}

/* a segment of len bytes, its address chosen by the kernel; 0 on failure */
static uintptr
mkseg(char *name, ulong len)
{
	char path[64], buf[128], *f[4];
	int fd, n;

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	if((fd = create(path, OREAD, DMDIR|0777)) < 0){
		fail("create %s: %r", path);
		return 0;
	}
	close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, ORDWR)) < 0){
		fail("open %s: %r", path);
		return 0;
	}
	if(fprint(fd, "va 0 %lud", len) < 0){
		fail("%s: va 0 %lud: %r", name, len);
		close(fd);
		return 0;
	}
	n = pread(fd, buf, sizeof buf-1, 0);
	close(fd);
	if(n <= 0){
		fail("%s: read ctl: %r", name);
		return 0;
	}
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 3 || strcmp(f[0], "va") != 0){
		fail("%s: ctl says %s", name, buf);
		return 0;
	}
	return strtoull(f[1], nil, 0);
}

static void
rmseg(char *name)
{
	char path[64];

	snprint(path, sizeof path, "#g/%s", name);
	if(remove(path) < 0)
		fail("remove %s: %r", path);
}

void
main(void)
{
	enum { Len = 3*1024*1024 };
	uintptr a, b, c, d;
	char *pa, *pb, *e;
	int pid, fd;
	Waitmsg *w;
	void *v;

	a = mkseg("segshared.a", Len);
	b = mkseg("segshared.b", 4096);
	if(a == 0 || b == 0)
		exits("fail");
	if(a != b && (a+Len <= b || b+4096 <= a))
		ok("two segments, two ranges: %#p %#p", a, b);
	else
		fail("ranges overlap: %#p %#p", a, b);

	/* an address of its own in the region: the kernel's to give */
	remove("#g/segshared.e");
	if((fd = create("#g/segshared.e", OREAD, DMDIR|0777)) < 0)
		fail("create #g/segshared.e: %r");
	else{
		close(fd);
		if((fd = open("#g/segshared.e/ctl", ORDWR)) < 0)
			fail("open #g/segshared.e/ctl: %r");
		else{
			if(fprint(fd, "va %#p 4096", b+64*1024*1024) >= 0)
				fail("va %#p 4096 in the region taken", b+64*1024*1024);
			else
				ok("an address of its own in the region refused: %r");
			close(fd);
		}
		rmseg("segshared.e");
	}

	pa = segattach(0, "segshared.a", nil, 0);
	pb = segattach(0, "segshared.b", nil, 0);
	if(pa != (char*)a || pb != (char*)b){
		fail("attached at %#p %#p, want %#p %#p", pa, pb, a, b);
		exits("fail");
	}
	strcpy(pa+Len-64, "from the parent");
	strcpy(pb, "b too");

	/* a process with its own address space (no RFMEM): attaches by name */
	switch(pid = rfork(RFPROC|RFFDG)){
	case -1:
		fail("rfork: %r");
		break;
	case 0:
		segdetach(pa);
		segdetach(pb);
		pa = segattach(0, "segshared.a", nil, 0);
		pb = segattach(0, "segshared.b", nil, 0);
		if(pa != (char*)a || pb != (char*)b)
			exits("attach");
		if(strcmp(pa+Len-64, "from the parent") != 0 || strcmp(pb, "b too") != 0)
			exits("contents");
		strcpy(pa, "from the child");
		exits(nil);
	default:
		while((w = wait()) != nil && w->pid != pid)
			free(w);
		e = w != nil ? w->msg : "no wait";
		if(e[0] == 0)
			ok("another process attached both at their addresses, saw the bytes");
		else
			fail("child: %s", e);
		free(w);
		if(strcmp(pa, "from the child") == 0)
			ok("its write seen here");
		else
			fail("the child's write not seen: %.20q", pa);
	}

	/* segattach's own choice: below the stack, above the region */
	v = segattach(0, "shared", nil, 1024*1024);
	if(v == (void*)-1)
		fail("segattach shared, va 0: %r");
	else{
		if((uintptr)v < b+4096)
			fail("segattach chose %#p, not above the region's segments", v);
		else
			ok("segattach chose %#p, outside the region", v);
		segdetach(v);
	}

	/* removed and detached everywhere: the range is free again */
	segdetach(pa);
	segdetach(pb);
	rmseg("segshared.a");
	rmseg("segshared.b");
	c = mkseg("segshared.c", Len);
	d = mkseg("segshared.d", 4096);
	if(c == a)
		ok("a freed range given again: %#p", c);
	else
		fail("new segment at %#p, not the freed %#p", c, a);
	if(d != 0 && d != c)
		ok("and the next: %#p", d);
	rmseg("segshared.c");
	if(d != 0)
		rmseg("segshared.d");

	if(nfail){
		print("segshared: %d failed\n", nfail);
		exits("fail");
	}
	print("segshared: all passed\n");
	exits(nil);
}
