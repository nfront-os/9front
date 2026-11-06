/*
 * SPDX-License-Identifier: MIT
 *
 * amdpager: VRAM shown to the processor page by page, as Linux maps it
 * (aux/amdgpu: pager segments, segment(3)): more objects mapped than the
 * part of VRAM BAR 0 shows (256 MB without a resized BAR), not pinned,
 * each page written and read back; TTM moves objects out of the visible
 * part and back, taking their pages back and faulting them in again (the
 * driver's status line: "cpu view of vram").  Before, every mapped object
 * stayed pinned there, in one piece, and the 9th map of 32 MB failed.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: amdpager [-n objects] [-m MB each] [dev]
 */
#include <u.h>
#include <libc.h>

static char *dev = "/dev/gpu/0";
static int nfail;
static int cfd;

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
req(char *reply, int nreply, char *fmt, ...)
{
	va_list arg;
	char buf[256];
	int n;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if(write(cfd, buf, n) != n)
		return -1;
	if(reply == nil)
		return 0;
	n = pread(cfd, reply, nreply-1, 0);
	if(n < 0)
		return -1;
	reply[n] = 0;
	return 0;
}

/* the status's line on the processor's view of VRAM */
static char*
pagerline(void)
{
	static char buf[16384];
	char path[128], *p, *e;
	int fd, n;

	snprint(path, sizeof path, "%s/status", dev);
	if((fd = open(path, OREAD)) < 0)
		return "no status";
	n = readn(fd, buf, sizeof buf - 1);
	close(fd);
	buf[n > 0 ? n : 0] = 0;
	if((p = strstr(buf, "cpu view of vram")) == nil)
		return "no cpu view line";
	if((e = strchr(p, '\n')) != nil)
		*e = 0;
	return p;
}

/* a number after word in the pager line */
static vlong
count(char *line, char *word)
{
	char *p;

	if((p = strstr(line, word)) == nil)
		return -1;
	return strtoll(p + strlen(word), nil, 10);
}

void
main(int argc, char **argv)
{
	char path[128], buf[256], *f[8], *line;
	int i, n, mb, bad;
	u32int *px[64], h[64];
	vlong size, pg, back0, back1, faults0, faults1;
	vlong t0, t1;

	n = 16;
	mb = 32;
	ARGBEGIN{
	case 'n':
		n = atoi(EARGF(sysfatal("usage")));
		break;
	case 'm':
		mb = atoi(EARGF(sysfatal("usage")));
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];
	if(n > nelem(px))
		n = nelem(px);
	size = (vlong)mb << 20;

	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(pread(cfd, buf, sizeof buf - 1, 0) <= 0)
		sysfatal("clone: %r");
	line = pagerline();
	ok("%s", line);
	faults0 = count(line, "faults ");
	back0 = count(line, "taken back ");

	/* n objects in VRAM, all mapped: more than BAR 0 shows */
	t0 = nsec();
	for(i = 0; i < n; i++){
		if(req(buf, sizeof buf, "gem %lld 4096 vram cpuaccess", size) < 0 || tokenize(buf, f, nelem(f)) != 2){
			fail("gem %d of %d MB: %r", i, mb);
			break;
		}
		h[i] = strtoul(f[1], nil, 0);
		if(req(buf, sizeof buf, "map %ud", h[i]) < 0 || tokenize(buf, f, nelem(f)) != 5){
			fail("map %d (%d MB mapped before): %r", i, i*mb);
			break;
		}
		if((px[i] = segattach(0, f[2], nil, strtoull(f[4], nil, 0))) == (void*)-1){
			fail("segattach %s: %r", f[2]);
			break;
		}
		/* a word on each page: object and page */
		for(pg = 0; pg < size/4096; pg++)
			px[i][pg*1024] = i<<20 | pg;
	}
	n = i;
	t1 = nsec();
	ok("%d objects of %d MB in VRAM mapped and written: %d MB, in %.2f s", n, mb, n*mb, (t1-t0)/1e9);

	/* all read back: those moved out of BAR 0's part fault in again */
	t0 = nsec();
	bad = 0;
	for(i = 0; i < n && !bad; i++)
		for(pg = 0; pg < size/4096; pg++)
			if(px[i][pg*1024] != (i<<20 | pg)){
				fail("object %d page %lld: %#ux, want %#ux", i, pg, px[i][pg*1024], (u32int)(i<<20 | pg));
				bad = 1;
				break;
			}
	t1 = nsec();
	if(!bad)
		ok("every page read back as written, in %.2f s", (t1-t0)/1e9);

	line = pagerline();
	faults1 = count(line, "faults ");
	back1 = count(line, "taken back ");
	ok("%s", line);
	if(n*mb > 256 && back1 - back0 <= 0)
		fail("%d MB mapped but nothing taken back (moved out of the visible part)", n*mb);
	else
		ok("pages taken back %lld times, faults served %lld", back1 - back0, faults1 - faults0);

	for(i = 0; i < n; i++){
		segdetach(px[i]);
		req(nil, 0, "close %ud", h[i]);
	}
	close(cfd);
	print(nfail ? "amdpager: %d failed\n" : "amdpager: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
