/*
 * SPDX-License-Identifier: MIT
 *
 * inteltest: checks aux/intelgpu (gpu(4) files under /dev/gpu/0) on
 * an Intel GPU of generation 8: a context, buffer objects, batches on
 * the blitter (BCS) and render (RCS) engines, fences, a fill of an
 * object and of the visible plane, a thousand batches in a row.
 * Prints ok or FAIL a line, and at the end what failed.
 *
 * usage: aux/inteltest [-d /dev/gpu/0]
 */
#include <u.h>
#include <libc.h>

enum {
	Mistoredwimm	= 0x20<<23 | 2,	/* gen 8: 4 dwords */
	Usegtt		= 1<<22,
	Mibbend		= 0x0A<<23,
	Minoop		= 0,
	Xycolorblt	= 2<<29 | 0x50<<22,
	Bltwritergba	= 3<<20,
};

typedef struct Bo Bo;
struct Bo {
	int	id;
	u32int	*p;
	uvlong	size;
	ulong	gtt;
};

static char *dir = "/dev/gpu/0";
static int ctlfd, subfd, fencefd;
static int nfail;
static uvlong nfence;

static void
ok(char *fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);
	print("ok ");
	vfprint(1, fmt, arg);
	print("\n");
	va_end(arg);
}

static void
fail(char *fmt, ...)
{
	va_list arg;

	va_start(arg, fmt);
	print("FAIL ");
	vfprint(1, fmt, arg);
	print("\n");
	va_end(arg);
	nfail++;
}

static char*
readall(char *name)
{
	static char buf[4096];
	char path[128];
	int fd, n;

	snprint(path, sizeof path, "%s/%s", dir, name);
	if((fd = open(path, OREAD)) < 0)
		return nil;
	n = readn(fd, buf, sizeof buf-1);
	close(fd);
	if(n < 0)
		return nil;
	buf[n] = 0;
	return buf;
}

/* a request to the context's ctl, its reply */
static char*
ctl(char *fmt, ...)
{
	static char buf[256];
	char req[128];
	va_list arg;
	int n;

	va_start(arg, fmt);
	vsnprint(req, sizeof req, fmt, arg);
	va_end(arg);
	if(write(ctlfd, req, strlen(req)) < 0)
		return nil;
	if((n = pread(ctlfd, buf, sizeof buf-1, 0)) < 0)
		return nil;
	buf[n] = 0;
	return buf;
}

static int
newbo(Bo *b, uvlong size)
{
	char *r, *f[6];

	if((r = ctl("bo %llud", size)) == nil || tokenize(r, f, nelem(f)) != 6 || strcmp(f[0], "bo") != 0){
		fail("bo %llud: %r", size);
		return -1;
	}
	b->id = atoi(f[1]);
	b->size = strtoull(f[4], nil, 0);
	b->gtt = strtoul(f[5], nil, 0);
	b->p = segattach(0, f[2], nil, b->size);
	if(b->p == (void*)-1){
		fail("segattach %s: %r", f[2]);
		return -1;
	}
	ok("bo %d: %llud bytes, gtt %#lux, seg %s at %#p", b->id, b->size, b->gtt, f[2], b->p);
	return 0;
}

/* the batch at off in b on the engine; its fence */
static uvlong
submit(Bo *b, ulong off, char *eng)
{
	if(fprint(subfd, "%d %lud %s", b->id, off, eng) < 0){
		fail("submit on %s: %r", eng);
		return 0;
	}
	return ++nfence;
}

/* 0 when fence k is done, after the driver's wait */
static int
fencewait(uvlong k, vlong *us)
{
	char buf[64];
	vlong t0;
	int n;

	t0 = nsec();
	if(fprint(fencefd, "wait %llud", k) < 0)
		return -1;
	if((n = pread(fencefd, buf, sizeof buf-1, 0)) <= 0)
		return -1;
	buf[n] = 0;
	if(us != nil)
		*us = (nsec() - t0)/1000;
	if(strstr(buf, "done") == nil || strstr(buf, "failed") != nil){
		werrstr("fence %llud: %s", k, buf);
		return -1;
	}
	return 0;
}

static int
storedw(u32int *c, ulong a, u32int v)
{
	c[0] = Mistoredwimm | Usegtt;
	c[1] = a;
	c[2] = 0;
	c[3] = v;
	return 4;
}

static int
bbend(u32int *c, int n)
{
	c[n++] = Mibbend;
	if(n & 1)
		c[n++] = Minoop;
	return n;
}

static int
colorblt(u32int *c, ulong a, int pitch, int x0, int y0, int x1, int y1, u32int color, int bpp)
{
	c[0] = Xycolorblt | (bpp == 32 ? Bltwritergba : 0) | (7-2);
	c[1] = (bpp == 32 ? 3 : 1)<<24 | 0xF0<<16 | pitch;
	c[2] = y0<<16 | x0;
	c[3] = y1<<16 | x1;
	c[4] = a;
	c[5] = 0;
	c[6] = color;
	return 7;
}

static void
status(char *when)
{
	char *s;

	if((s = readall("status")) == nil)
		fail("read status: %r");
	else
		print("status %s:\n%s", when, s);
}

static ulong
nirq(void)
{
	char *s, *p;

	if((s = readall("status")) == nil || (p = strstr(s, "\nirq ")) == nil)
		return 0;
	p = strchr(p+5, ' ');
	return p ? strtoul(p+1, nil, 10) : 0;
}

void
main(int argc, char **argv)
{
	char path[128], ctxname[64], *s, *f[8];
	Bo data, batch;
	u32int *c, v;
	uvlong k;
	vlong us, t0;
	ulong irq0, plane, stride;
	int i, n, bad, bpp;

	ARGBEGIN{
	case 'd':
		dir = EARGF(sysfatal("usage: %s [-d dir]", argv0));
		break;
	}ARGEND

	status("before");
	if((s = readall("ctl")) == nil)
		sysfatal("read %s/ctl: %r", dir);
	print("ctl: %s", s);

	snprint(path, sizeof path, "%s/clone", dir);
	if((ctlfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	/* the open's reply: ctx n */
	if((n = pread(ctlfd, ctxname, sizeof ctxname-1, 0)) <= 0)
		sysfatal("ctx: %r");
	ctxname[n] = 0;
	if(tokenize(ctxname, f, nelem(f)) < 2 || strcmp(f[0], "ctx") != 0)
		sysfatal("ctx: %s", ctxname);
	ok("context %s", f[1]);
	snprint(path, sizeof path, "%s/ctx/%s/submit", dir, f[1]);
	if((subfd = open(path, OWRITE)) < 0)
		sysfatal("open %s: %r", path);
	snprint(path, sizeof path, "%s/ctx/%s/fence", dir, f[1]);
	if((fencefd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);

	if(newbo(&data, 65536) < 0 || newbo(&batch, 65536) < 0)
		goto Out;
	memset(data.p, 0, data.size);

	/* 1. the blitter writes a dword */
	n = storedw(batch.p, data.gtt + 0, 0xcafe0001);
	n = bbend(batch.p, n);
	k = submit(&batch, 0, "bcs");
	if(k && fencewait(k, &us) < 0)
		fail("bcs store: %r");
	else if((v = data.p[0]) != 0xcafe0001)
		fail("bcs store: the object holds %#ux, want 0xcafe0001 (cached? try aux/intelgpu -U)", v);
	else
		ok("bcs: MI_STORE_DWORD_IMM, fence after %lld µs, the CPU sees %#ux", us, v);

	/* 2. the render engine writes a dword */
	c = batch.p + 64;
	n = storedw(c, data.gtt + 4, 0xcafe0002);
	n = bbend(c, n);
	k = submit(&batch, 64*4, "rcs");
	if(k && fencewait(k, &us) < 0)
		fail("rcs store: %r");
	else if((v = data.p[1]) != 0xcafe0002)
		fail("rcs store: the object holds %#ux, want 0xcafe0002", v);
	else
		ok("rcs: MI_STORE_DWORD_IMM, fence after %lld µs, the CPU sees %#ux", us, v);

	/* 3. the blitter fills a 64x64 surface in the object */
	c = batch.p + 128;
	n = colorblt(c, data.gtt + 4096, 256, 0, 0, 64, 64, 0x12345678, 32);
	n = bbend(c, n);
	k = submit(&batch, 128*4, "bcs");
	if(k && fencewait(k, &us) < 0)
		fail("bcs fill: %r");
	else{
		bad = 0;
		for(i = 0; i < 64*64; i++)
			if(data.p[1024 + i] != 0x12345678)
				bad++;
		if(bad)
			fail("bcs fill: %d of 4096 pixels wrong (first %#ux)", bad, data.p[1024]);
		else
			ok("bcs: XY_COLOR_BLT 64x64 into the object, fence after %lld µs", us);
	}

	/* 4. the visible plane: a blue rectangle at 600,100 */
	/* ctl: plane A gpuaddr stride bpp w h */
	if((s = readall("ctl")) != nil && tokenize(s, f, nelem(f)) >= 7 && strcmp(f[0], "plane") == 0
	&& (bpp = atoi(f[4])) != 0){
		plane = strtoul(f[2], nil, 0);
		stride = strtoul(f[3], nil, 0);
		c = batch.p + 192;
		n = colorblt(c, plane, stride, 600, 100, 800, 200, bpp == 32 ? 0x0000FF : 0x001F, bpp);
		n = bbend(c, n);
		k = submit(&batch, 192*4, "bcs");
		if(k && fencewait(k, &us) < 0)
			fail("plane fill: %r");
		else
			ok("bcs: blue rectangle at 600,100 on plane %s (look at the screen)", f[1]);
	}else
		print("no plane to draw on\n");

	/* 5. a thousand batches on each engine, every hundredth waited for */
	irq0 = nirq();
	c = batch.p + 256;
	n = storedw(c, data.gtt + 8, 0);
	n = bbend(c, n);
	t0 = nsec();
	for(i = 0; i < 1000; i++){
		k = submit(&batch, 256*4, i & 1 ? "rcs" : "bcs");
		if(k == 0)
			break;
		if(i % 100 == 99 && fencewait(k, nil) < 0){
			fail("batch %d: %r", i);
			break;
		}
	}
	if(i == 1000)
		ok("1000 batches (rcs and bcs) in %lld ms, %lud interrupts", (nsec() - t0)/1000000, nirq() - irq0);

	status("after");
Out:
	close(ctlfd);
	close(subfd);
	close(fencefd);
	print(nfail ? "inteltest: %d failed\n" : "inteltest: all passed\n", nfail);
	exits(nfail ? "fail" : nil);
}
