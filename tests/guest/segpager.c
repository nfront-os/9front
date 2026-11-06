/*
 * segpager: segment(3)'s pager segments (9front-ng, port/devsegment.c), a
 * driver's memory shown page by page as Linux maps a GPU object, here
 * over the first GPU's BAR 0 (devgpu's gpu0bar0: QEMU's VGA, its
 * framebuffer; a guest without a GPU driver running): a
 * pager proc reads the faults from its queue and puts the pages there,
 * first in reverse order, after "unmap" in order, then says "nopage";
 * the writes through the pager segment land on those pages of the
 * framebuffer; a fault with no page to come kills the process; a removed
 * queue ends the pager's read; segment(3)'s table grows past 1024 global
 * segments.  Without gpu0bar0: skipped.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <u.h>
#include <libc.h>

enum {
	N = 16,			/* pages */
	Va = 0x500000000000ULL,	/* the pager segment's address */
};

static int nfail;
static int *mode;		/* shared with the pager proc: 0 reversed, 1 in order, 2 nopage */
static int *nreq;

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

static int
ctl(char *seg, char *fmt, ...)
{
	va_list arg;
	char path[64], buf[512];
	int fd, n, r;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	snprint(path, sizeof path, "#g/%s/ctl", seg);
	if((fd = open(path, OWRITE)) < 0)
		return -1;
	r = write(fd, buf, n) == n ? 0 : -1;
	close(fd);
	return r;
}

static int
mkseg(char *name)
{
	char path[64];
	int fd;

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
	if((fd = create(path, OREAD, DMDIR|0700)) < 0)
		return -1;
	close(fd);
	return 0;
}

/* the pager: each request "segpagers I" answered for all pages */
static void
pager(void)
{
	char buf[256], line[2048], *p, *e;
	int fd, n, i;

	if((fd = open("#g/segpagerq/data", OREAD)) < 0){
		fprint(2, "pager: %r\n");
		exits("open");
	}
	while((n = read(fd, buf, sizeof buf - 1)) > 0){
		buf[n] = 0;
		for(p = buf; (e = strchr(p, '\n')) != nil; p = e+1){
			*e = 0;
			(*nreq)++;
			if(*mode == 2){
				ctl("segpagers", "nopage");
				continue;
			}
			n = snprint(line, sizeof line, "pages 0");
			for(i = 0; i < N; i++)
				n += snprint(line+n, sizeof line - n, " %#x", (*mode == 0 ? N-1-i : i)*4096);
			if(ctl("segpagers", "%s", line) < 0)
				fprint(2, "pager: pages: %r\n");
		}
	}
	exits(nil);
}

void
main(void)
{
	uchar *fb, *p;
	int pid, w;
	Waitmsg *wm;
	int *shared;

	/* devgpu makes its BARs' segments when it is attached */
	if((w = open("#G", OREAD)) >= 0)
		close(w);
	if((fb = segattach(0, "gpu0bar0", nil, N*4096)) == (void*)-1){
		print("ok skipped: no gpu0bar0 (%r)\nsegpager: all passed\n");
		exits(nil);
	}
	shared = segattach(0, "shared", nil, 4096);
	if(shared == (void*)-1)
		sysfatal("shared: %r");
	mode = shared;
	nreq = shared+1;

	if(mkseg("segpagerq") < 0 || ctl("segpagerq", "pager") < 0)
		sysfatal("queue: %r");
	if(mkseg("segpagers") < 0 || ctl("segpagers", "va %#llux %#x phys gpu0bar0 pager segpagerq", Va, N*4096) < 0)
		sysfatal("pager segment: %r");
	ok("a queue and a pager segment of %d pages over gpu0bar0", N);

	switch(pid = rfork(RFPROC|RFMEM)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		pager();
	}

	if((p = segattach(0, "segpagers", nil, N*4096)) == (void*)-1)
		sysfatal("segattach segpagers: %r");

	/* reversed: page i of p is page N-1-i of the framebuffer */
	*mode = 0;
	p[0] = 0x11;
	p[(N-1)*4096 + 5] = 0x22;
	if(fb[(N-1)*4096] != 0x11 || fb[5] != 0x22)
		fail("reversed: fb %#x %#x, want 0x11 0x22", fb[(N-1)*4096], fb[5]);
	else if(*nreq != 1)
		fail("reversed: %d requests, want 1 (all pages at once)", *nreq);
	else
		ok("pages from the pager, reversed: each write on its page of the framebuffer");

	/* unmap, then in order */
	*mode = 1;
	if(ctl("segpagers", "unmap") < 0)
		fail("unmap: %r");
	p[0] = 0x33;
	p[3*4096 + 7] = 0x44;
	if(fb[0] != 0x33 || fb[3*4096+7] != 0x44)
		fail("after unmap: fb %#x %#x, want 0x33 0x44", fb[0], fb[3*4096+7]);
	else if(*nreq != 2)
		fail("after unmap: %d requests, want 2", *nreq);
	else
		ok("unmap: the next touch asks again, the new pages are used");

	/* a system call's copy into it (read from a pipe): the kernel's fault waits for the pager too */
	ctl("segpagers", "unmap");
	{
		int pfd[2];
		char msg[] = "through a pipe";

		if(pipe(pfd) < 0)
			fail("pipe: %r");
		else{
			write(pfd[1], msg, sizeof msg);
			if(read(pfd[0], p + 2*4096 + 100, sizeof msg) != sizeof msg)
				fail("read into the pager segment: %r");
			else if(memcmp(fb + 2*4096 + 100, msg, sizeof msg) != 0)
				fail("read into the pager segment: not on its page of the framebuffer");
			else if(*nreq != 3)
				fail("read into the pager segment: %d requests, want 3", *nreq);
			else
				ok("a read into it (the kernel's copy faults, the pager answers)");
			close(pfd[0]);
			close(pfd[1]);
		}
	}

	/* nopage: the fault kills */
	*mode = 2;
	ctl("segpagers", "unmap");
	switch(pid = fork()){
	case -1:
		fail("fork: %r");
		break;
	case 0:
		p[0] = 0x55;
		exits("not killed");
	default:
		wm = wait();
		if(wm == nil || wm->pid != pid || strstr(wm->msg, "fault") == nil)
			fail("nopage: child %s", wm ? wm->msg : "no wait");
		else
			ok("nopage: the fault kills (%s)", wm->msg);
		free(wm);
	}

	/* the queue removed: the pager's read ends */
	segdetach(p);
	remove("#g/segpagers");
	remove("#g/segpagerq");
	for(w = 0; w < 50; w++){
		if((wm = wait()) != nil){
			if(wm->msg[0] != 0)
				fail("pager exits %s", wm->msg);
			else
				ok("removed queue: the pager's read ends");
			free(wm);
			break;
		}
		sleep(100);
	}
	/* segment(3)'s table of global segments grows: more than it had (1024) */
	{
		char name[32];
		int i, made;

		for(made = 0; made < 3000; made++){
			snprint(name, sizeof name, "segpagerg%d", made);
			if(mkseg(name) < 0)
				break;
		}
		if(made < 3000)
			fail("global segments: %d made, then %r", made);
		else
			ok("3000 global segments at once");
		for(i = 0; i < made; i++){
			snprint(name, sizeof name, "#g/segpagerg%d", i);
			remove(name);
		}
	}

	print(nfail ? "segpager: %d failed\n" : "segpager: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
