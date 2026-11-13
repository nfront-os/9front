/*
 * SPDX-License-Identifier: MIT
 *
 * segrevoke: segment(3)'s alias segments are revoked: a process that
 * has one attached loses its pages when it is removed (or "revoke" is
 * written), and reading there faults; the memory stays in the segment
 * it came from.  Also: a page of an alias that was never put there
 * faults instead of crashing the kernel.  And phys segments: a part of
 * a physical segment (#G's bar 0 of QEMU's VGA, a GPU object's view in
 * a BAR) shows that part, nothing beyond it, and is revoked the same
 * way; the bar itself is the hostowner's only (none may not attach it).  Prints ok or FAIL lines.
 */
#include <u.h>
#include <libc.h>

static int fails;

static void
fail(char *fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);
	print("FAIL ");
	vfprint(1, fmt, arg);
	va_end(arg);
	print("\n");
	fails++;
}

static int
ctl(char *seg, char *fmt, ...)
{
	char path[64], buf[256];
	va_list arg;
	int fd, n;

	snprint(path, sizeof path, "#g/%s/ctl", seg);
	if((fd = open(path, OWRITE)) < 0)
		return -1;
	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	n = write(fd, buf, n) == n ? 0 : -1;
	close(fd);
	return n;
}

static int
mkseg(char *name)
{
	char path[64];
	int fd;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0700)) < 0)
		return -1;
	close(fd);
	return 0;
}

/* a child that reads the alias until told, then once more: how it ends */
static char*
reader(char *alias, uintptr va, int len, int *pfd)
{
	static char msg[ERRMAX];
	Waitmsg *w;
	char c;
	int pid, p[2];
	ulong *a;

	pipe(p);
	switch(pid = rfork(RFPROC|RFFDG)){
	case -1:
		return "fork";
	case 0:
		close(p[0]);
		if((a = segattach(0, alias, (void*)va, len)) == (void*)-1)
			exits("attach");
		if(a[0] != 0x5e6e0001)
			exits("wrong data");
		write(p[1], "r", 1);	/* attached and read */
		read(p[1], &c, 1);	/* the parent revoked it */
		if(a[0] == 0x5e6e0001)
			exits("still sees the data");
		exits("read after revoke");
	}
	close(p[1]);
	if(read(p[0], &c, 1) != 1)
		return "child did not start";
	*pfd = p[0];
	USED(pid);
	return nil;
}

static void
child(int fd, char *what)
{
	Waitmsg *w;

	write(fd, "g", 1);
	close(fd);
	w = wait();
	if(w == nil)
		fail("%s: no wait message", what);
	else if(strstr(w->msg, "fault") == nil)
		fail("%s: the child ended with '%s', want a fault", what, w->msg);
	else
		print("ok %s: the child faulted (%s)\n", what, w->msg);
	free(w);
}

/*
 * phys: the pages at 0x3000 of bar 0 of the first #G device with one
 * (QEMU's VGA: its framebuffer), a child reading there until revoked
 */
static void
physparts(void)
{
	char bar[32], ph[32], path[64], buf[128], *e;
	ulong *b, *a;
	uvlong va;
	int fd, n;

	if((fd = open("#G", OREAD)) < 0){
		print("ok phys: no #G, skipped\n");
		return;
	}
	close(fd);
	snprint(bar, sizeof bar, "gpu0bar0");
	if((b = segattach(0, bar, nil, 0x10000)) == (void*)-1){
		print("ok phys: no %s (%r), skipped\n", bar);
		return;
	}
	b[0x3000/4] = 0x5e6e0001;
	b[0x2000/4] = 0xbad;
	b[0x5000/4] = 0xbad;
	va = 0x5000800000ULL;
	snprint(ph, sizeof ph, "rvph.%d", getpid());
	if(mkseg(ph) < 0 || ctl(ph, "va %#llux 0x2000 phys %s 0x3000", va, bar) < 0){
		fail("phys segment of %s: %r", bar);
		goto out;
	}
	snprint(path, sizeof path, "#g/%s/ctl", ph);
	if((fd = open(path, OREAD)) >= 0){
		n = read(fd, buf, sizeof buf - 1);
		close(fd);
		buf[n > 0 ? n : 0] = 0;
		print("ok phys ctl: %s", buf);
	}
	/* its first word is bar 0's at 0x3000; it ends after 0x2000 bytes */
	if((a = segattach(0, ph, nil, 0x2000)) == (void*)-1)
		fail("attach %s: %r", ph);
	else{
		if(a[0] != 0x5e6e0001)
			fail("phys: %#lux at its start, want bar 0's %#ux at 0x3000", a[0], 0x5e6e0001);
		else
			print("ok phys: shows bar 0 from 0x3000\n");
		a[1] = 0x5e6e0003;
		if(b[0x3004/4] != 0x5e6e0003)
			fail("phys: a write did not reach bar 0");
		else
			print("ok phys: writes reach bar 0\n");
		segdetach(a);
	}
	switch(rfork(RFPROC|RFFDG)){
	case 0:
		a = segattach(0, ph, nil, 0x2000);
		if(a == (void*)-1)
			exits("attach");
		if(a[0x2000/4] == 0xbad)
			exits("read past its end: bar 0 at 0x5000");
		exits("past its end");
	default:
		{
			Waitmsg *w = wait();

			if(w == nil || strstr(w->msg, "fault") == nil)
				fail("phys: past its end: '%s', want a fault", w ? w->msg : "no wait message");
			else
				print("ok phys: nothing past its end (%s)\n", w->msg);
			free(w);
		}
	}
	/* removing it revokes it */
	if((e = reader(ph, va, 0x2000, &fd)) != nil){
		fail("phys reader: %s", e);
		goto out;
	}
	snprint(path, sizeof path, "#g/%s", ph);
	if(remove(path) < 0)
		fail("remove %s: %r", ph);
	child(fd, "phys remove");
	/* revoke, the segment still there */
	snprint(ph, sizeof ph, "rvph2.%d", getpid());
	if(mkseg(ph) < 0 || ctl(ph, "va %#llux 0x2000 phys %s 0x3000", va + 0x200000, bar) < 0){
		fail("phys segment 2: %r");
		goto out;
	}
	if((e = reader(ph, va + 0x200000, 0x2000, &fd)) != nil){
		fail("phys reader 2: %s", e);
		goto out;
	}
	if(ctl(ph, "revoke") < 0)
		fail("phys revoke: %r");
	child(fd, "phys revoke");
	snprint(path, sizeof path, "#g/%s", ph);
	remove(path);
	/* the bar itself: the hostowner's only, not none's */
	switch(rfork(RFPROC|RFFDG|RFNAMEG)){
	case 0:
		if((fd = open("/dev/user", OWRITE)) < 0 || write(fd, "none", 4) != 4)
			exits("cannot become none");
		close(fd);
		if(segattach(0, bar, nil, 0x10000) != (void*)-1)
			exits("none attached the bar");
		exits(nil);
	default:
		{
			Waitmsg *w = wait();

			if(w == nil || w->msg[0] != 0)
				fail("phys: as none: %s", w ? w->msg : "no wait message");
			else
				print("ok phys: none may not attach %s\n", bar);
			free(w);
		}
	}
	/* a part beyond the physical segment: refused */
	snprint(ph, sizeof ph, "rvph3.%d", getpid());
	if(mkseg(ph) == 0){
		if(ctl(ph, "va %#llux 0x2000 phys %s 0x7fffffff000", va + 0x400000, bar) == 0)
			fail("phys: a part beyond %s was made", bar);
		else
			print("ok phys: a part beyond the segment is refused (%r)\n");
		snprint(path, sizeof path, "#g/%s", ph);
		remove(path);
	}
out:
	segdetach(b);
}

void
main(void)
{
	char src[32], al[32], path[64];
	ulong *s;
	int fd;
	char *e;
	uvlong va;

	snprint(src, sizeof src, "rvsrc.%d", getpid());
	snprint(al, sizeof al, "rval.%d", getpid());
	va = 0x5000000000ULL;
	if(mkseg(src) < 0 || ctl(src, "va %#llux 0x2000 sticky", va) < 0)
		sysfatal("sticky segment: %r");
	if((s = segattach(0, src, nil, 0x2000)) == (void*)-1)
		sysfatal("attach %s: %r", src);
	s[0] = 0x5e6e0001;
	s[1024] = 0x5e6e0002;

	/* removing the alias revokes it */
	if(mkseg(al) < 0 || ctl(al, "va %#llux 0x2000 alias", va + 0x200000) < 0 || ctl(al, "page 0 %s 0", src) < 0)
		sysfatal("alias: %r");
	if((e = reader(al, va + 0x200000, 0x2000, &fd)) != nil)
		sysfatal("reader: %s", e);
	snprint(path, sizeof path, "#g/%s", al);
	if(remove(path) < 0)
		fail("remove %s: %r", al);
	child(fd, "remove");
	if(s[0] != 0x5e6e0001)
		fail("the source lost its data: %#lux", s[0]);
	else
		print("ok the source keeps its data\n");

	/* revoke, the alias still there */
	snprint(al, sizeof al, "rval2.%d", getpid());
	if(mkseg(al) < 0 || ctl(al, "va %#llux 0x2000 alias", va + 0x400000) < 0 || ctl(al, "page 0 %s 0", src) < 0)
		sysfatal("alias 2: %r");
	if((e = reader(al, va + 0x400000, 0x2000, &fd)) != nil)
		sysfatal("reader: %s", e);
	if(ctl(al, "revoke") < 0)
		fail("revoke: %r");
	child(fd, "revoke");
	snprint(path, sizeof path, "#g/%s", al);
	remove(path);

	/* a page never put there: a fault, not a kernel panic */
	snprint(al, sizeof al, "rval3.%d", getpid());
	if(mkseg(al) < 0 || ctl(al, "va %#llux 0x2000 alias", va + 0x600000) < 0 || ctl(al, "page 0 %s 0", src) < 0)
		sysfatal("alias 3: %r");
	switch(rfork(RFPROC|RFFDG)){
	case 0:
		s = segattach(0, al, (void*)(va + 0x600000), 0x2000);
		if(s == (void*)-1)
			exits("attach");
		/* volatile: both ways end alike, so gcc -O2 dropped a plain read */
		if(((volatile ulong*)s)[1024] == 0)
			exits("hole read");
		exits("hole read");
	default:
		{
			Waitmsg *w = wait();

			if(w == nil || strstr(w->msg, "fault") == nil)
				fail("hole: '%s', want a fault", w ? w->msg : "no wait message");
			else
				print("ok a page not there faults (%s)\n", w->msg);
			free(w);
		}
	}
	snprint(path, sizeof path, "#g/%s", al);
	remove(path);
	snprint(path, sizeof path, "#g/%s", src);
	segdetach(s);
	remove(path);
	physparts();
	if(fails)
		exits("fail");
	print("segrevoke: ok\n");
	exits(nil);
}
