/*
 * SPDX-License-Identifier: MIT
 *
 * Parts follow Linux's i915 driver (MIT); their copyright and
 * permission notices are in NOTICE.
 *
 * inteltest: checks aux/intelgpu (gpu(4) files under /dev/gpu/0) on
 * an Intel GPU of generation 8: contexts, buffer objects, batches on
 * the blitter (BCS) and render (RCS) engines, fences, a fill of an
 * object and of the visible plane, two contexts at the same GPU
 * address apart (PPGTT), a thousand batches in a row, a context of
 * another process.  Prints ok or FAIL a line, and at the end what
 * failed.
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
typedef struct Ctx Ctx;

struct Bo {
	int	id;
	u32int	*p;
	uvlong	size;
	uvlong	gpu;
};

struct Ctx {
	char	id[16];
	int	ctl, sub, fence;
	uvlong	nfence;
};

static char *dir = "/dev/gpu/0";
static int nfail;
static int global;	/* the driver's -G: one address space, the GTT */

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

static char *readall(char*);

/* before a step: where it was if the machine hangs (intelrun's log on the stick) */
static void
step(char *s)
{
	print("-- %s\n", s);
}

static void
fail(char *fmt, ...)
{
	va_list arg;
	char *s;

	va_start(arg, fmt);
	print("FAIL ");
	vfprint(1, fmt, arg);
	print("\n");
	va_end(arg);
	/* the first failure: what the driver knows then */
	if(nfail++ == 0 && (s = readall("status")) != nil)
		print("status at the first failure:\n%s", s);
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

static int
newctx(Ctx *c)
{
	char path[128], buf[64], *f[4];
	int n;

	memset(c, 0, sizeof *c);
	snprint(path, sizeof path, "%s/clone", dir);
	if((c->ctl = open(path, ORDWR)) < 0){
		fail("open %s: %r", path);
		return -1;
	}
	if((n = pread(c->ctl, buf, sizeof buf-1, 0)) <= 0){
		fail("ctx: %r");
		return -1;
	}
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 2 || strcmp(f[0], "ctx") != 0){
		fail("ctx: %s", buf);
		return -1;
	}
	snprint(c->id, sizeof c->id, "%s", f[1]);
	snprint(path, sizeof path, "%s/ctx/%s/submit", dir, c->id);
	if((c->sub = open(path, OWRITE)) < 0){
		fail("open %s: %r", path);
		return -1;
	}
	snprint(path, sizeof path, "%s/ctx/%s/fence", dir, c->id);
	if((c->fence = open(path, ORDWR)) < 0){
		fail("open %s: %r", path);
		return -1;
	}
	ok("context %s", c->id);
	return 0;
}

static void
freectx(Ctx *c)
{
	close(c->ctl);
	close(c->sub);
	close(c->fence);
}

/* a request to the context's ctl, its reply */
static char*
ctl(Ctx *c, char *fmt, ...)
{
	static char buf[256];
	char req[128];
	va_list arg;
	int n;

	va_start(arg, fmt);
	vsnprint(req, sizeof req, fmt, arg);
	va_end(arg);
	if(write(c->ctl, req, strlen(req)) < 0)
		return nil;
	if((n = pread(c->ctl, buf, sizeof buf-1, 0)) < 0)
		return nil;
	buf[n] = 0;
	return buf;
}

/* an object at GPU address at (~0: the driver's choice) */
static int
newboat(Ctx *c, Bo *b, uvlong size, uvlong at)
{
	char *r, *f[6];

	if(at == ~0ULL)
		r = ctl(c, "bo %llud", size);
	else
		r = ctl(c, "bo %llud %#llux", size, at);
	if(r == nil || tokenize(r, f, nelem(f)) != 6 || strcmp(f[0], "bo") != 0){
		fail("bo %llud: %r", size);
		return -1;
	}
	b->id = atoi(f[1]);
	b->size = strtoull(f[4], nil, 0);
	b->gpu = strtoull(f[5], nil, 0);
	b->p = segattach(0, f[2], nil, b->size);
	if(b->p == (void*)-1){
		fail("segattach %s: %r", f[2]);
		return -1;
	}
	memset(b->p, 0, b->size);
	ok("ctx %s bo %d: %llud bytes at gpu %#llux, seg %s at %#p", c->id, b->id, b->size, b->gpu, f[2], b->p);
	return 0;
}

static int
newbo(Ctx *c, Bo *b, uvlong size)
{
	return newboat(c, b, size, ~0ULL);
}

/* the batch at off in b on the engine; its fence */
static uvlong
submit(Ctx *c, Bo *b, ulong off, char *eng)
{
	if(fprint(c->sub, "%d %lud %s", b->id, off, eng) < 0){
		fail("submit on %s: %r", eng);
		return 0;
	}
	return ++c->nfence;
}

/* 0 when fence k is done, after the driver's wait */
static int
fencewait(Ctx *c, uvlong k, vlong *us)
{
	char buf[64];
	vlong t0;
	int n;

	t0 = nsec();
	if(fprint(c->fence, "wait %llud", k) < 0)
		return -1;
	if((n = pread(c->fence, buf, sizeof buf-1, 0)) <= 0)
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
storedw(u32int *c, uvlong a, u32int v)
{
	c[0] = Mistoredwimm | (global ? Usegtt : 0);
	c[1] = a;
	c[2] = a>>32;
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
colorblt(u32int *c, uvlong a, int pitch, int x0, int y0, int x1, int y1, u32int color, int bpp)
{
	c[0] = Xycolorblt | (bpp == 32 ? Bltwritergba : 0) | (7-2);
	c[1] = (bpp == 32 ? 3 : 1)<<24 | 0xF0<<16 | pitch;
	c[2] = y0<<16 | x0;
	c[3] = y1<<16 | x1;
	c[4] = a;
	c[5] = a>>32;
	c[6] = color;
	return 7;
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

/* status' power line: RC6 residency (ms), the frequency asked for and the minimum (MHz) */
static int
power(uvlong *rc6, int *asked, int *min)
{
	char *s, *p;

	if((s = readall("status")) == nil || (p = strstr(s, "power rc6 on ")) == nil)
		return -1;
	*rc6 = strtoull(p+13, &p, 10);
	if((p = strstr(p, "asked ")) == nil)
		return -1;
	*asked = strtol(p+6, &p, 10);
	if((p = strstr(p, "min ")) == nil)
		return -1;
	*min = strtol(p+4, nil, 10);
	return 0;
}

/* one store on an engine, checked by the CPU */
static void
store(Ctx *c, Bo *batch, int bdw, Bo *data, int ddw, u32int v, char *eng)
{
	vlong us;
	uvlong k;
	u32int got;
	int n;

	n = storedw(batch->p + bdw, data->gpu + ddw*4, v);
	bbend(batch->p + bdw, n);
	k = submit(c, batch, bdw*4, eng);
	if(k == 0)
		return;
	if(fencewait(c, k, &us) < 0)
		fail("ctx %s %s store: %r", c->id, eng);
	else if((got = data->p[ddw]) != v)
		fail("ctx %s %s store: the object holds %#ux, want %#ux (cached? try aux/intelgpu -U)",
			c->id, eng, got, v);
	else
		ok("ctx %s %s: MI_STORE_DWORD_IMM, fence after %lld µs, the CPU sees %#ux", c->id, eng, us, v);
}

/* the checks, once; the failures */
static int
suite(void)
{
	char *s, *f[8];
	Ctx a, b;
	Bo data, batch, bdata, bbatch, big, bigbatch, hbatch, hdata, tdata;
	Waitmsg *w;
	u32int *c;
	uvlong k;
	vlong us, t0;
	ulong irq0, stride;
	int i, n, bad, bpp, nfail0;

	nfail0 = nfail;
	memset(&data, 0, sizeof data);
	memset(&batch, 0, sizeof batch);

	if(newctx(&a) < 0 || newbo(&a, &data, 65536) < 0 || newbo(&a, &batch, 65536) < 0)
		goto Out;

	/* 1, 2: each engine writes a dword */
	step("1. bcs store");
	store(&a, &batch, 0, &data, 0, 0xcafe0001, "bcs");
	step("2. rcs store (the context's first render batch: workarounds, null render state)");
	store(&a, &batch, 64, &data, 1, 0xcafe0002, "rcs");

	/* 3. the blitter fills a 64x64 surface in the object */
	step("3. the blitter fills a 64x64 surface in the object");
	c = batch.p + 128;
	n = colorblt(c, data.gpu + 4096, 256, 0, 0, 64, 64, 0x12345678, 32);
	bbend(c, n);
	k = submit(&a, &batch, 128*4, "bcs");
	if(k && fencewait(&a, k, &us) < 0)
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

	/* 4. the visible plane, mapped into the context: a blue rectangle at 600,100 */
	step("4. the visible plane, mapped into the context: a blue rectangle at 600,100");
	if((s = ctl(&a, "map plane")) != nil && tokenize(s, f, nelem(f)) >= 7 && strcmp(f[0], "plane") == 0
	&& (bpp = atoi(f[4])) != 0){
		stride = strtoul(f[3], nil, 0);
		c = batch.p + 192;
		n = colorblt(c, strtoul(f[2], nil, 0), stride, 600, 100, 800, 200, bpp == 32 ? 0x0000FF : 0x001F, bpp);
		bbend(c, n);
		k = submit(&a, &batch, 192*4, "bcs");
		if(k && fencewait(&a, k, &us) < 0)
			fail("plane fill: %r");
		else
			ok("bcs: blue rectangle at 600,100 on the plane at gpu %s (look at the screen)", f[2]);
	}else
		fail("map plane: %r");

	/* 5. a second context: its first object at the same GPU address, apart */
	step("5. a second context: its first object at the same GPU address, apart");
	if(newctx(&b) < 0 || newbo(&b, &bdata, 65536) < 0 || newbo(&b, &bbatch, 65536) < 0)
		goto Out;
	if(global)
		print("global gtt (-G): no isolation to check\n");
	else if(bdata.gpu != data.gpu)
		print("the contexts' objects are at %#llux and %#llux: isolation not checked\n", data.gpu, bdata.gpu);
	else{
		store(&a, &batch, 320, &data, 2, 0xaaaa0001, "bcs");
		store(&b, &bbatch, 0, &bdata, 2, 0xbbbb0002, "bcs");
		store(&b, &bbatch, 64, &bdata, 3, 0xbbbb0003, "rcs");
		if(data.p[2] == 0xaaaa0001 && data.p[3] == 0 && bdata.p[2] == 0xbbbb0002 && bdata.p[3] == 0xbbbb0003)
			ok("ctx %s and ctx %s at gpu %#llux: each wrote its own object", a.id, b.id, data.gpu);
		else
			fail("isolation: ctx %s's object holds %#ux %#ux, ctx %s's %#ux %#ux", a.id, data.p[2],
				data.p[3], b.id, bdata.p[2], bdata.p[3]);
	}

	/* 6. a thousand batches, alternating contexts and engines, every hundredth waited for */
	step("6. a thousand batches, alternating contexts and engines, every hundredth waited for");
	irq0 = nirq();
	n = storedw(batch.p + 384, data.gpu + 16, 0);
	bbend(batch.p + 384, n);
	n = storedw(bbatch.p + 384, bdata.gpu + 16, 0);
	bbend(bbatch.p + 384, n);
	t0 = nsec();
	for(i = 0; i < 1000; i++){
		if(i & 2)
			k = submit(&a, &batch, 384*4, i & 1 ? "rcs" : "bcs");
		else
			k = submit(&b, &bbatch, 384*4, i & 1 ? "rcs" : "bcs");
		if(k == 0)
			break;
		if(i % 100 == 99 && (fencewait(&a, a.nfence, nil) < 0 || fencewait(&b, b.nfence, nil) < 0)){
			fail("batch %d: %r", i);
			break;
		}
	}
	if(i == 1000)
		ok("1000 batches (2 contexts, rcs and bcs) in %lld ms, %lud interrupts",
			(nsec() - t0)/1000000, nirq() - irq0);

	/* 7. that context goes, with its objects; one in another process */
	step("7. that context goes, with its objects; one in another process");
	segdetach(bdata.p);
	segdetach(bbatch.p);
	freectx(&b);
	switch(fork()){
	case -1:
		fail("fork: %r");
		break;
	case 0:
		if(newctx(&b) < 0 || newbo(&b, &bdata, 4096) < 0 || newbo(&b, &bbatch, 4096) < 0)
			exits("fail");
		store(&b, &bbatch, 0, &bdata, 0, 0x5eed0001, "rcs");
		exits(nfail ? "fail" : nil);
	default:
		if((w = wait()) == nil)
			fail("wait: %r");
		else{
			if(w->msg[0])
				fail("the other process: %s", w->msg);
			free(w);
		}
	}

	/* 8. the render engine's workarounds, written into each new context, read back */
	step("8. the render engine's workarounds, written into each new context, read back");
	if((s = readall("status")) == nil || (s = strstr(s, "rcs workarounds ")) == nil)
		fail("status: no rcs workarounds line");
	else{
		i = atoi(s+16);
		if((s = strchr(s, '/')) != nil && i == atoi(s+1))
			ok("rcs workarounds: %d set", i);
		else
			fail("rcs workarounds: only %d set", i);
	}

	/*
	 * 9. a 16 MB object (scattered pages): the blitter fills all of it,
	 * then the render engine writes each page's number into it, from
	 * a 1 MB batch; the CPU checks every dword
	 */
	step("9. a 16 MB object (scattered pages)");
	if(newbo(&a, &big, 16*1024*1024) < 0 || newbo(&a, &bigbatch, 1024*1024) < 0)
		goto Out;
	c = bigbatch.p;
	n = colorblt(c, big.gpu, 4096, 0, 0, 1024, 4096, 0x5a5a5a5a, 32);
	bbend(c, n);
	k = submit(&a, &bigbatch, 0, "bcs");
	if(k && fencewait(&a, k, &us) < 0)
		fail("16 MB fill: %r");
	else{
		c = bigbatch.p + 64;
		n = 0;
		for(i = 0; i < 4096; i++)
			n += storedw(c + n, big.gpu + i*4096 + 4, 0xd0000000 | i);
		bbend(c, n);
		k = submit(&a, &bigbatch, 64*4, "rcs");
		if(k && fencewait(&a, k, &t0) < 0)
			fail("16 MB stores: %r");
		else{
			bad = 0;
			for(i = 0; i < 4*1024*1024; i++)
				if(big.p[i] != ((i & 1023) == 1 ? 0xd0000000 | i>>10 : 0x5a5a5a5a)){
					if(bad++ == 0)
						fail("16 MB: dword %d (page %d) holds %#ux", i, i>>10, big.p[i]);
				}
			if(bad)
				fail("16 MB: %d dwords wrong", bad);
			else
				ok("16 MB object: bcs fill in %lld µs, 4096 rcs stores in %lld µs, all pages right", us, t0);
		}
	}
	segdetach(big.p);
	segdetach(bigbatch.p);

	/*
	 * 11. power: the GT sleeps when idle (RC6 residency grows by a
	 * third of a pause at least) and runs faster under load (RPS: the
	 * frequency asked for rises above the minimum)
	 */
	step("11. RC6 when idle, RPS under load");
	{
		uvlong r0, r1;
		int f0, fbusy, fmin;

		if(power(&r0, &f0, &fmin) < 0)
			fail("status: no power line (or -P)");
		else{
			sleep(1500);
			power(&r1, &f0, &fmin);
			if(r1 >= r0 + 500)
				ok("rc6: %llud ms of 1500 ms idle", r1 - r0);
			else
				fail("rc6: only %llud ms of 1500 ms idle", r1 - r0);
			for(i = 0; i < 300; i++)
				if((k = submit(&a, &bigbatch, 0, "bcs")) == 0)
					break;
			sleep(300);
			power(&r1, &fbusy, &fmin);
			if(k != 0 && fencewait(&a, k, nil) < 0)
				fail("rps load: %r");
			else if(fbusy > fmin)
				ok("rps: %d MHz asked under load (min %d)", fbusy, fmin);
			else
				fail("rps: still %d MHz under load (min %d)", fbusy, fmin);
		}
	}

	/*
	 * 10. addresses of the client's choice, as hasvk's (softpin): a
	 * batch at 4 GB, objects at 1 TB and 128 TB - 1 GB, stores on both
	 * engines; then one object moved with bind
	 */
	step("10. objects at the client's addresses (4 GB, 1 TB, 128 TB - 1 GB), bind");
	if(global)
		print("global gtt (-G): no addresses of the client's choice\n");
	else if(newboat(&a, &hbatch, 65536, 0x100000000ULL) == 0
	&& newboat(&a, &hdata, 65536, 0x10000000000ULL) == 0
	&& newboat(&a, &tdata, 65536, 0x7FFFC0000000ULL) == 0){
		store(&a, &hbatch, 0, &hdata, 5, 0x48b00001, "rcs");
		store(&a, &hbatch, 64, &hdata, 6, 0x48b00002, "bcs");
		store(&a, &hbatch, 128, &tdata, 7, 0x48b00003, "rcs");
		if(ctl(&a, "bind %d %#llux", hdata.id, 0x20000000000ULL) == nil)
			fail("bind: %r");
		else{
			hdata.gpu = 0x20000000000ULL;
			store(&a, &hbatch, 192, &hdata, 8, 0x48b00004, "bcs");
			if(hdata.p[5] == 0x48b00001 && hdata.p[6] == 0x48b00002)
				ok("bind: the object at %#llux keeps its contents", hdata.gpu);
			else
				fail("bind: the object holds %#ux %#ux after the move", hdata.p[5], hdata.p[6]);
		}
		if(ctl(&a, "bo 4096 %#llux", 0x100000000ULL) != nil)
			fail("an object at an address in use: no error");
		else
			ok("an object at an address in use: %r");
		segdetach(hbatch.p);
		segdetach(hdata.p);
		segdetach(tdata.p);
	}

	if((s = readall("status")) != nil)
		print("status after:\n%s", s);
Out:
	segdetach(data.p);
	segdetach(batch.p);
	freectx(&a);
	return nfail - nfail0;
}

void
main(int argc, char **argv)
{
	char *s;

	ARGBEGIN{
	case 'd':
		dir = EARGF(sysfatal("usage: %s [-d dir]", argv0));
		break;
	}ARGEND

	if((s = readall("status")) == nil)
		sysfatal("read %s/status: %r", dir);
	print("status before:\n%s", s);
	global = strstr(s, "global gtt") != nil;
	if((s = readall("ctl")) == nil)
		sysfatal("read %s/ctl: %r", dir);
	print("ctl: %s", s);

	suite();
	print(nfail ? "inteltest: %d failed\n" : "inteltest: all passed\n", nfail);
	exits(nfail ? "fail" : nil);
}
