/*
 * NIX zero-copy I/O tests (zread(2), zp(2), zp(3), segment(3)
 * kmsg/umsg and the free file), run by tests/nix/zio.sh:
 *	ziotest			everything on a time-sharing core
 *	execac -c n ziotest ac	the same on an AC; forked children
 *				(the other side of the pipes) stay on a TC
 * Prints "ok name" or "FAIL name: why" per check.
 */
#include <u.h>
#include <libc.h>
#include <zio.h>
#include <seg.h>

enum
{
	Segsz = 2*1024*1024,
	Bufsz = 8*1024,
	Nbuf = 64,		/* umsg buffers sent through the zp */
	Bigsz = 1536*1024,	/* contiguous in the kmsg segment */
	Filesz = 64*1024,
};

static int failed;
static int onac;
static char *kseg, *useg;
static uintptr ksegsz, usegsz;
static char kname[64], uname[64];
static char big[Bigsz];

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

static uchar
pat(uintptr i, int seed)
{
	return (i*7 + seed*13 + (i>>8)) & 0xff;
}

static void
fill(char *p, long n, int seed)
{
	long i;

	for(i = 0; i < n; i++)
		p[i] = pat(i, seed);
}

/* offset of the first wrong byte, -1 if none */
static long
check(char *p, long n, long off, int seed)
{
	long i;

	for(i = 0; i < n; i++)
		if((uchar)p[i] != pat(off+i, seed))
			return i;
	return -1;
}

static int
inseg(void *p, char *base, uintptr size)
{
	return (char*)p >= base && (char*)p < base+size;
}

/* a global segment of type (kmsg, umsg) at an address the system picks */
static char*
mkseg(char *name, char *type, uintptr *size)
{
	char dir[128], buf[128], *f[4];
	int fd, n;
	void *p;

	snprint(dir, sizeof dir, "/mnt/seg/%s", name);
	fd = create(dir, OREAD, 0640|DMDIR);
	if(fd < 0)
		sysfatal("create %s: %r", dir);
	close(fd);
	snprint(buf, sizeof buf, "%s/ctl", dir);
	fd = open(buf, ORDWR);
	if(fd < 0)
		sysfatal("open %s: %r", buf);
	if(fprint(fd, "%s 0 %d\n", type, Segsz) < 0)
		sysfatal("write %s: %r", buf);
	n = pread(fd, buf, sizeof buf - 1, 0);
	close(fd);
	if(n <= 0)
		sysfatal("read ctl: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) != 3 || strcmp(f[0], type) != 0){
		fail("segment ctl", "%s: %s", type, buf);
		return nil;
	}
	*size = strtoull(f[2], nil, 0);
	p = segattach(0, name, nil, *size);
	if(p == (void*)-1)
		sysfatal("segattach %s: %r", name);
	if((uintptr)p != strtoull(f[1], nil, 0)){
		fail("segment ctl", "%s attached at %#p, ctl says %s", type, p, f[1]);
		return nil;
	}
	return p;
}

/* addresses from the free file of a umsg segment, at most n */
static int
readfree(char *name, uintptr *va, int n)
{
	char path[128], buf[32*19+1], *f[32];
	int fd, i, m, nf;

	snprint(path, sizeof path, "/mnt/seg/%s/free", name);
	fd = open(path, OREAD);
	if(fd < 0)
		return -1;
	for(m = 0; m < n; ){
		i = read(fd, buf, sizeof buf - 1);
		if(i <= 0)
			break;
		buf[i] = 0;
		nf = tokenize(buf, f, nelem(f));
		for(i = 0; i < nf && m < n; i++)
			va[m++] = strtoull(f[i], nil, 0);
	}
	close(fd);
	return m;
}

static void
testsegs(void)
{
	char path[128];
	Dir *d;
	char *p;

	if(kseg != nil && useg != nil && ksegsz == Segsz && usegsz == Segsz)
		ok("kmsg and umsg segments, address picked by the system");
	else
		fail("kmsg and umsg segments", "kseg %#p/%#p useg %#p/%#p", kseg, ksegsz, useg, usegsz);
	snprint(path, sizeof path, "/mnt/seg/%s/free", uname);
	d = dirstat(path);
	if(d != nil){
		ok("umsg has a free file");
		free(d);
	}else
		fail("umsg has a free file", "%r");
	/* libseg: newseg, a plain shared segment with addr 0 */
	snprint(path, sizeof path, "zioaddr%d", getpid());
	p = newseg(path, 0, 64*1024);
	if(p == nil){
		fail("newseg (addr)", "%r");
		return;
	}
	p[0] = 1;
	p[64*1024-1] = 2;
	snprint(path, sizeof path, "/mnt/seg/zioaddr%d/free", getpid());
	d = dirstat(path);
	if(d == nil)
		ok("newseg (addr): plain shared segment, no free file");
	else{
		fail("newseg (addr)", "has a free file");
		free(d);
	}
}

/* zwrite to an ordinary pipe: the buffers come back through free */
static void
testzwritepipe(void)
{
	int p[2], i, n;
	Zio io[2];
	uintptr va[4];
	char buf[3*Bufsz];
	long r, tot;

	if(pipe(p) < 0)
		sysfatal("pipe: %r");
	fill(useg, Bufsz, 1);
	fill(useg+Bufsz, Bufsz, 2);
	io[0].data = useg;
	io[0].size = 1000;
	io[1].data = useg+Bufsz;
	io[1].size = Bufsz;
	n = zwrite(p[1], io, 2);
	if(n != 2 || io[0].size != 1000 || io[1].size != Bufsz){
		fail("zwrite umsg buffers to a pipe", "n %d %lud %lud: %r", n, io[0].size, io[1].size);
		close(p[0]);
		close(p[1]);
		return;
	}
	for(tot = 0; tot < 1000+Bufsz; tot += r)
		if((r = read(p[0], buf+tot, sizeof buf - tot)) <= 0)
			break;
	if(tot == 1000+Bufsz && check(buf, 1000, 0, 1) < 0 && check(buf+1000, Bufsz, 0, 2) < 0)
		ok("zwrite umsg buffers to a pipe");
	else
		fail("zwrite umsg buffers to a pipe", "read %ld bytes, bad at %ld/%ld", tot,
			check(buf, 1000, 0, 1), check(buf+1000, Bufsz, 0, 2));
	n = readfree(uname, va, 2);
	if(n == 2 && ((va[0] == (uintptr)useg && va[1] == (uintptr)useg+Bufsz)
	|| (va[1] == (uintptr)useg && va[0] == (uintptr)useg+Bufsz)))
		ok("umsg free file returns the written buffers");
	else
		fail("umsg free file returns the written buffers", "n %d %#p %#p", n, va[0], va[1]);

	/* from the data segment: copied into the kmsg segment */
	fill(big, Bufsz, 3);
	io[0].data = big;
	io[0].size = Bufsz;
	n = zwrite(p[1], io, 1);
	tot = 0;
	if(n == 1)
		for(; tot < Bufsz; tot += r)
			if((r = read(p[0], buf+tot, Bufsz - tot)) <= 0)
				break;
	if(n == 1 && tot == Bufsz && check(buf, Bufsz, 0, 3) < 0)
		ok("zwrite from the data segment (kernel copy)");
	else
		fail("zwrite from the data segment (kernel copy)", "n %d tot %ld: %r", n, tot);
	close(p[0]);
	close(p[1]);
	USED(i);
}

/* zread and zpread from a file: buffers in the kmsg segment */
static void
testzreadfile(void)
{
	char path[64];
	int fd, n, i;
	Zio io[4], hold[64];
	int nhold;
	long bad;

	snprint(path, sizeof path, "/tmp/ziodata.%d", getpid());
	fd = create(path, ORDWR, 0644);
	if(fd < 0)
		sysfatal("create %s: %r", path);
	fill(big, Filesz, 4);
	if(write(fd, big, Filesz) != Filesz)
		sysfatal("write %s: %r", path);
	close(fd);
	fd = open(path, OREAD);
	if(fd < 0)
		sysfatal("open %s: %r", path);

	n = zread(fd, io, nelem(io), 100);
	bad = n == 1 ? check(io[0].data, io[0].size, 0, 4) : -2;
	if(n == 1 && io[0].size == 100 && inseg(io[0].data, kseg, ksegsz) && bad < 0)
		ok("zread a file into the kmsg segment");
	else
		fail("zread a file into the kmsg segment", "n %d size %lud data %#p bad %ld: %r",
			n, n > 0? io[0].size: 0, n > 0? io[0].data: nil, bad);
	if(n > 0)
		zfree(io, n);
	n = zread(fd, io, nelem(io), 100);
	if(n == 1 && io[0].size == 100 && check(io[0].data, 100, 100, 4) < 0)
		ok("zread moves the offset");
	else
		fail("zread moves the offset", "n %d", n);
	if(n > 0)
		zfree(io, n);
	n = zpread(fd, io, nelem(io), 300, 5000);
	if(n == 1 && io[0].size == 300 && check(io[0].data, 300, 5000, 4) < 0)
		ok("zpread at an offset");
	else
		fail("zpread at an offset", "n %d", n);
	if(n > 0)
		zfree(io, n);
	n = zread(fd, io, nelem(io), 100);
	if(n == 1 && check(io[0].data, 100, 200, 4) < 0)
		ok("zpread keeps the offset");
	else
		fail("zpread keeps the offset", "n %d", n);
	if(n > 0)
		zfree(io, n);
	if(io[0].data == nil && io[0].size == 0)
		ok("zfree clears the Zio");
	else
		fail("zfree clears the Zio", "%#p %lud", io[0].data, io[0].size);

	/* 2 MB kmsg: about 32 reads of 64K without zfree exhaust it */
	nhold = 0;
	for(i = 0; i < nelem(hold); i++){
		n = zpread(fd, &hold[nhold], 1, Filesz, 0);
		if(n != 1)
			break;
		nhold++;
	}
	if(n < 0 && nhold >= 16 && nhold < 40)
		ok("kmsg segment exhausted without zfree");
	else
		fail("kmsg segment exhausted without zfree", "%d reads, n %d: %r", nhold, n);
	zfree(hold, nhold);
	n = zpread(fd, io, 1, Filesz, 0);
	if(n == 1 && io[0].size == Filesz && check(io[0].data, Filesz, 0, 4) < 0)
		ok("zfree gives the kmsg buffers back");
	else
		fail("zfree gives the kmsg buffers back", "n %d: %r", n);
	if(n > 0)
		zfree(io, n);
	/* the freed buffers coalesce: 1.5 MB in one piece */
	close(fd);
	fd = open("/dev/null", OWRITE);
	io[0].data = big;
	io[0].size = Bigsz;
	n = zwrite(fd, io, 1);
	if(n == 1 && io[0].size == Bigsz)
		ok("freed kmsg buffers coalesce (1.5 MB kernel copy)");
	else
		fail("freed kmsg buffers coalesce (1.5 MB kernel copy)", "n %d: %r", n);
	close(fd);
	remove(path);

	/* errors */
	io[0].data = big;
	io[0].size = 10;
	werrstr("");
	zfree(io, 1);
	rerrstr(path, sizeof path);
	if(strstr(path, "zero-copy") != nil)
		ok("zfree of a buffer outside zero-copy segments fails");
	else
		fail("zfree of a buffer outside zero-copy segments fails", "'%s'", path);
	io[0].data = kseg+12345;
	werrstr("");
	zfree(io, 1);
	rerrstr(path, sizeof path);
	if(strstr(path, "not a zero-copy buffer") != nil)
		ok("zfree of a kmsg address not allocated fails");
	else
		fail("zfree of a kmsg address not allocated fails", "'%s'", path);
	if(zread(0, io, 0, 10) < 0 && zread(0, io, 1000, 10) < 0)
		ok("zread with a bad nio fails");
	else
		fail("zread with a bad nio fails", "no error");
}

/* the child of the zp tests: on a TC (forked from an AC it stays on a TC) */
static void
childcore(char *name)
{
	int role;

	getcore(&role);
	if(onac && role != NIXTC)
		print("FAIL %s: child on core role %d\n", name, role);
}

/*
 * zero copy through a zp: the child sends Nbuf umsg buffers,
 * the parent zreads, checks and zfrees them, the child gets
 * them back from the free file
 */
static void
testzp(void)
{
	int fd[2], i, n, nio, got, bad, status;
	Zio io[8];
	uintptr va[Nbuf];
	Waitmsg *w;
	char *p;

	if(zp(fd) < 0){
		fail("zp", "%r");
		return;
	}
	switch(fork()){
	case -1:
		sysfatal("fork: %r");
	case 0:
		close(fd[0]);
		childcore("zp zero copy");
		for(i = 0; i < Nbuf; i++){
			p = useg + i*Bufsz;
			fill(p, Bufsz, 10+i);
			io[0].data = p;
			io[0].size = Bufsz;
			if(zwrite(fd[1], io, 1) != 1){
				print("FAIL zp zero copy: zwrite %d: %r\n", i);
				exits("zwrite");
			}
		}
		n = readfree(uname, va, Nbuf);
		bad = 0;
		for(i = 0; i < n; i++)
			if(va[i] < (uintptr)useg || va[i] >= (uintptr)useg+Nbuf*Bufsz
			|| (va[i]-(uintptr)useg) % Bufsz != 0)
				bad++;
		if(n != Nbuf || bad)
			print("FAIL zp: free file gives the buffers back: %d of %d, %d bad\n", n, Nbuf, bad);
		else
			print("ok zp: free file gives the buffers back\n");
		close(fd[1]);
		exits(nil);
	}
	close(fd[1]);
	got = 0;
	bad = 0;
	for(;;){
		nio = zread(fd[0], io, nelem(io), 4*Bufsz);
		if(nio <= 0)
			break;
		for(i = 0; i < nio; i++){
			if(io[i].size != Bufsz || !inseg(io[i].data, useg, usegsz)
			|| check(io[i].data, Bufsz, 0, 10+((char*)io[i].data-useg)/Bufsz) >= 0
			|| ((char*)io[i].data-useg)/Bufsz != got)
				bad++;
			got++;
		}
		zfree(io, nio);
	}
	if(nio == 0 && got == Nbuf && bad == 0)
		ok("zp zero copy: umsg buffers by reference, in order");
	else
		fail("zp zero copy: umsg buffers by reference, in order", "nio %d got %d bad %d: %r", nio, got, bad);
	close(fd[0]);
	w = wait();
	status = w != nil && w->msg[0] == 0;
	free(w);
	if(!status)
		fail("zp zero copy", "child failed");
}

/* read and write on a zp copy through the kmsg segments; data1 to data */
static void
testzpcopy(void)
{
	int fd[2], n;
	long tot, r;
	Zio io[4];

	if(zp(fd) < 0){
		fail("zp", "%r");
		return;
	}
	switch(fork()){
	case -1:
		sysfatal("fork: %r");
	case 0:
		close(fd[1]);
		childcore("zp write");
		fill(big, 200*1024, 20);
		/* longer than 64K: queued in pieces */
		if(write(fd[0], big, 200*1024) != 200*1024)
			print("FAIL zp write 200K: %r\n");
		fill(big, Bufsz, 21);
		if(write(fd[0], big, Bufsz) != Bufsz)
			print("FAIL zp write 8K: %r\n");
		close(fd[0]);
		exits(nil);
	}
	close(fd[0]);
	/* reads of 3000 bytes split the queued buffers */
	for(tot = 0; tot < 200*1024; tot += r){
		r = 200*1024 - tot;
		if(r > 3000)
			r = 3000;
		if((r = read(fd[1], useg+Nbuf*Bufsz+tot, r)) <= 0)
			break;
	}
	if(tot == 200*1024 && check(useg+Nbuf*Bufsz, tot, 0, 20) < 0)
		ok("zp write 200K, read in pieces (data to data1)");
	else
		fail("zp write 200K, read in pieces (data to data1)", "tot %ld bad at %ld", tot,
			check(useg+Nbuf*Bufsz, tot, 0, 20));
	/* zread with a small count: the rest stays queued */
	n = zread(fd[1], io, nelem(io), 1000);
	if(n == 1 && io[0].size == 1000 && check(io[0].data, 1000, 0, 21) < 0)
		ok("zread less than a queued buffer");
	else
		fail("zread less than a queued buffer", "n %d size %lud", n, n > 0? io[0].size: 0);
	if(n > 0)
		zfree(io, n);
	n = zread(fd[1], io, nelem(io), Bufsz);
	if(n == 1 && io[0].size == Bufsz-1000 && inseg(io[0].data, kseg, ksegsz)
	&& check(io[0].data, Bufsz-1000, 1000, 21) < 0)
		ok("zread the rest, copied into the kmsg segment");
	else
		fail("zread the rest, copied into the kmsg segment", "n %d size %lud", n, n > 0? io[0].size: 0);
	if(n > 0)
		zfree(io, n);
	n = zread(fd[1], io, nelem(io), Bufsz);
	if(n == 0)
		ok("zread at the end of the zp");
	else
		fail("zread at the end of the zp", "n %d: %r", n);
	close(fd[1]);
	free(wait());
}

void
main(int argc, char **argv)
{
	int role;

	onac = argc > 1 && strcmp(argv[1], "ac") == 0;
	getcore(&role);
	if(onac){
		if(role == NIXAC)
			ok("ziotest on an application core");
		else
			fail("ziotest on an application core", "role %d", role);
	}
	if(bind("#g", "/mnt/seg", MREPL|MCREATE) < 0)
		sysfatal("bind #g: %r");
	snprint(kname, sizeof kname, "ziok%d", getpid());
	snprint(uname, sizeof uname, "ziou%d", getpid());
	kseg = mkseg(kname, "kmsg", &ksegsz);
	useg = mkseg(uname, "umsg", &usegsz);
	if(kseg == nil || useg == nil)
		exits("segments");
	testsegs();
	testzwritepipe();
	testzreadfile();
	testzp();
	testzpcopy();
	print("done %d failed\n", failed);
	exits(failed? "failed": nil);
}
