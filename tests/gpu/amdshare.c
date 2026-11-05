/*
 * SPDX-License-Identifier: MIT
 *
 * amdshare: an object of aux/amdgpu's between two contexts (gpu(4)'s
 * export and import, ../9front-ui stage 2): context A makes an object in
 * GTT (GEM_CREATE through its arena), writes a pattern through its map,
 * exports it (ctl export handle -> token); context B imports the token
 * (import token -> handle size), maps the handle and reads the pattern,
 * writes one of its own that A reads.  A token nobody exported is
 * refused; once A is gone, so is its token, while B keeps the object.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * The structures are those of Linux's amdgpu_drm.h (MIT).
 */
#include <u.h>
#include <libc.h>

#define IOC(dir, nr, size)	((u32int)(dir) << 30 | (u32int)(size) << 16 | 'd' << 8 | (nr))
#define IOWR(nr, t)	IOC(3, nr, sizeof(t))
#define CMD	0x40	/* DRM_COMMAND_BASE */

typedef struct Gemcreate Gemcreate;
struct Gemcreate {	/* drm_amdgpu_gem_create: in; out.handle over bo_size */
	uvlong	bo_size, alignment, domains, domain_flags;
};

enum {
	Ggemcreate	= 0x00,
	Domgtt	= 2,
	Size	= 64*1024,
};

typedef struct Ctx Ctx;
struct Ctx {
	char	*name;
	int	cfd, ifd;
	uchar	*arena;
	uvlong	asize;
};

static char *dev = "/dev/gpu/0";
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

/* a request to c's ctl and its reply in reply; -1 refused (%r) */
static int
ctl(Ctx *c, char *reply, int nreply, char *fmt, ...)
{
	va_list arg;
	char buf[256];
	int n;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if(write(c->cfd, buf, n) != n)
		return -1;
	n = pread(c->cfd, reply, nreply-1, 0);
	if(n < 0)
		return -1;
	reply[n] = 0;
	return 0;
}

static void
open1(Ctx *c, char *name)
{
	char path[128], buf[256], *f[8];
	int n;

	c->name = name;
	snprint(path, sizeof path, "%s/clone", dev);
	if((c->cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if((n = pread(c->cfd, buf, sizeof buf - 1, 0)) <= 0)
		sysfatal("clone: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 2)
		sysfatal("clone: %s", buf);
	snprint(path, sizeof path, "%s/ctx/%s/ioctl", dev, f[1]);
	if((c->ifd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(ctl(c, buf, sizeof buf, "arena") < 0 || tokenize(buf, f, nelem(f)) != 4)
		sysfatal("arena: %r");
	c->asize = strtoull(f[3], nil, 0);
	if((c->arena = segattach(0, f[1], nil, c->asize)) == (void*)-1)
		sysfatal("segattach %s: %r", f[1]);
	if((uvlong)c->arena != strtoull(f[2], nil, 0))
		sysfatal("arena at %#p, the server has it at %s", c->arena, f[2]);
}

static void
close1(Ctx *c)
{
	segdetach(c->arena);
	close(c->ifd);
	close(c->cfd);
}

/* an object in GTT made by c's GEM_CREATE: its handle */
static u32int
gemcreate(Ctx *c)
{
	Gemcreate *gc;
	char buf[64];
	long r;
	int n;

	gc = (Gemcreate*)c->arena;
	memset(gc, 0, sizeof *gc);
	gc->bo_size = Size;
	gc->alignment = 4096;
	gc->domains = Domgtt;
	if(fprint(c->ifd, "%#ux %#p", IOWR(CMD+Ggemcreate, Gemcreate), gc) < 0)
		sysfatal("GEM_CREATE: %r");
	if((n = pread(c->ifd, buf, sizeof buf - 1, 0)) <= 0)
		sysfatal("GEM_CREATE: %r");
	buf[n] = 0;
	if((r = strtol(buf, nil, 10)) != 0)
		sysfatal("GEM_CREATE: %ld", r);
	return *(u32int*)gc;
}

/* the processor's view of c's object h */
static u32int*
map(Ctx *c, u32int h)
{
	char buf[256], *f[8];
	void *p;

	if(ctl(c, buf, sizeof buf, "map %ud", h) < 0)
		sysfatal("%s: map %ud: %r", c->name, h);
	if(tokenize(buf, f, nelem(f)) != 5)
		sysfatal("%s: map %ud: %s", c->name, h, buf);
	if((p = segattach(0, f[2], nil, strtoull(f[4], nil, 0))) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	return p;
}

static int
check(u32int *w, u32int base, char *what)
{
	int i;

	for(i = 0; i < Size/4; i++)
		if(w[i] != base+i){
			fail("%s: word %d is %#ux, want %#ux", what, i, w[i], base+i);
			return -1;
		}
	ok("%s: all %d words", what, Size/4);
	return 0;
}

void
main(int argc, char **argv)
{
	Ctx a, b;
	char buf[256], token[64], *f[8];
	u32int ha, hb, *wa, *wb;
	int i;

	if(argc > 1)
		dev = argv[1];
	open1(&a, "A");
	open1(&b, "B");

	ha = gemcreate(&a);
	wa = map(&a, ha);
	for(i = 0; i < Size/4; i++)
		wa[i] = 0x5a000000 + i;
	ok("A: object %ud, %d KB written", ha, Size/1024);

	if(ctl(&a, buf, sizeof buf, "export %ud", ha) < 0 || tokenize(buf, f, nelem(f)) != 2 || strcmp(f[0], "export") != 0)
		sysfatal("A: export %ud: %r %s", ha, buf);
	strecpy(token, token+sizeof token, f[1]);
	ok("A: export %ud: token %.8s... (%d digits)", ha, token, strlen(token));
	if(ctl(&a, buf, sizeof buf, "export %ud", ha) == 0 && tokenize(buf, f, nelem(f)) == 2 && strcmp(f[1], token) == 0)
		ok("A: exported again: the same token");
	else
		fail("A: exported again: %s", buf);

	if(ctl(&b, buf, sizeof buf, "import 0123456789abcdef0123456789abcdef") < 0)
		ok("B: a token nobody exported: %r");
	else
		fail("B: a token nobody exported imported: %s", buf);

	if(ctl(&b, buf, sizeof buf, "import %s", token) < 0 || tokenize(buf, f, nelem(f)) != 3 || strcmp(f[0], "import") != 0)
		sysfatal("B: import: %r %s", buf);
	hb = strtoul(f[1], nil, 10);
	ok("B: import: object %ud, %s bytes", hb, f[2]);
	if(strtoull(f[2], nil, 10) < Size)
		fail("B: the object has %s bytes, want %d", f[2], Size);
	wb = map(&b, hb);
	check(wb, 0x5a000000, "B reads what A wrote");
	for(i = 0; i < Size/4; i++)
		wb[i] = 0xb0000000 + i;
	check(wa, 0xb0000000, "A reads what B wrote");

	/* A goes: its token with it, B's object stays */
	close1(&a);
	sleep(500);
	if(ctl(&b, buf, sizeof buf, "import %s", token) < 0)
		ok("B: A gone, its token refused: %r");
	else
		fail("B: A gone, its token still imports: %s", buf);
	check(wb, 0xb0000000, "B's object after A went");
	close1(&b);

	print("amdshare: %s\n", nfail ? "FAILED" : "all passed");
	exits(nfail ? "fail" : nil);
}
