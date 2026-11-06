/*
 * SPDX-License-Identifier: MIT
 *
 * amdcs: a submission through aux/amdgpu's requests as text (gpu(4)), as
 * libdrm9 makes them for RADV: the device's information (info device,
 * info vramgtt), an amdgpu context (hwctx), two objects in GTT mapped in
 * the GPU's address space (gem, va) and for the processor (map), one IB
 * (cs, its BOs named), waitcs, the value the GPU wrote.  A context has no
 * ioctl file: nothing of a client's memory reaches Linux.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <u.h>
#include <libc.h>

enum {
	Varadvib	= 1<<1 | 1<<3 | 4<<5,	/* readable, executable, MTYPE_UC: RADV's IBs */
	Vaflags	= 1<<1 | 1<<2 | 1<<3,	/* readable, writeable, executable */
};

static char *dev = "/dev/gpu/0";
static int nfail;
static int cfd, sfd;

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

/* the request on fd, its reply in reply: -1 refused (%r) */
static int
req(int fd, char *reply, int nreply, char *fmt, ...)
{
	va_list arg;
	char buf[1024];
	int n;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if(write(fd, buf, n) != n)
		return -1;
	n = pread(fd, reply, nreply-1, 0);
	if(n < 0)
		return -1;
	reply[n] = 0;
	return 0;
}

/* the value of line "name v" in s */
static char*
field(char *s, char *name)
{
	static char v[64];
	char *p;
	int n;

	n = strlen(name);
	for(p = s; p != nil && *p; p = strchr(p, '\n')){
		if(*p == '\n')
			p++;
		if(strncmp(p, name, n) == 0 && p[n] == ' '){
			strecpy(v, v+sizeof v, p+n+1);
			if((p = strchr(v, '\n')) != nil)
				*p = 0;
			return v;
		}
	}
	return "?";
}

/* the processor's view of an object: its segment (GTT: its pages; VRAM: its part of BAR 0) */
static u32int*
map(u32int h)
{
	char buf[256], *f[8];
	void *p;

	if(req(cfd, buf, sizeof buf, "map %ud", h) < 0)
		sysfatal("map %ud: %r", h);
	if(tokenize(buf, f, nelem(f)) != 5)
		sysfatal("map %ud: %s", h, buf);
	if((p = segattach(0, f[2], nil, strtoull(f[4], nil, 0))) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	ok("map %ud: %s", h, f[2]);
	return p;
}

void
main(int argc, char **argv)
{
	char path[128], buf[4096], *f[8];
	u32int h[2], ctxid, *cmd, *dst;
	uvlong gpu[2], base, seq;
	int ctx, i, radv, unmapped;
	Dir *d;

	base = 0x200000000ULL;
	radv = 0;
	unmapped = 0;
	ARGBEGIN{
	case 'u':	/* the IB writes where nothing is mapped: a page fault, which amdgpu should report */
		unmapped = 1;
		break;
	case 'r':	/* objects as RADV makes its command buffers: VRAM or GTT, explicit sync, the IB uncached */
		radv = 1;
		break;
	case 'b':	/* the objects' GPU addresses (RADV takes them from the top: 0xffff800000000000) */
		base = strtoull(EARGF(sysfatal("usage: amdcs [-ru] [-b gpuaddr] [dev]")), nil, 0);
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if((i = pread(cfd, buf, sizeof buf - 1, 0)) <= 0)
		sysfatal("clone: %r");
	buf[i] = 0;
	if(tokenize(buf, f, nelem(f)) < 2)
		sysfatal("clone: %s", buf);
	ctx = atoi(f[1]);
	snprint(path, sizeof path, "%s/ctx/%d/submit", dev, ctx);
	if((sfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	ok("ctx %d", ctx);
	snprint(path, sizeof path, "%s/ctx/%d/ioctl", dev, ctx);
	if((d = dirstat(path)) == nil)
		ok("no ioctl file: %r");
	else{
		fail("%s is there", path);
		free(d);
	}

	/* the device */
	if(req(cfd, buf, sizeof buf, "info device") < 0)
		fail("info device: %r");
	else
		ok("info device: device %s family %s, %s CUs, %s shader engines, gfx clock %s kHz",
			field(buf, "device_id"), field(buf, "family"), field(buf, "cu_active_number"),
			field(buf, "num_shader_engines"), field(buf, "max_engine_clock"));
	if(req(cfd, buf, sizeof buf, "info vramgtt") < 0)
		fail("info vramgtt: %r");
	else
		ok("info vramgtt: vram %llud MB (%llud visible), gtt %llud MB",
			strtoull(field(buf, "vram_size"), nil, 0)>>20,
			strtoull(field(buf, "vram_cpu_accessible_size"), nil, 0)>>20,
			strtoull(field(buf, "gtt_size"), nil, 0)>>20);
	if(req(cfd, buf, sizeof buf, "info nosuch") < 0)
		ok("info nosuch: %r");
	else
		fail("info nosuch: %s", buf);

	/* an amdgpu context */
	if(req(cfd, buf, sizeof buf, "hwctx new 0 0") < 0 || tokenize(buf, f, nelem(f)) != 2)
		sysfatal("hwctx new: %r");
	ctxid = strtoul(f[1], nil, 0);
	ok("hwctx: context %ud", ctxid);

	/* two objects in GTT, in the address space, for the processor */
	for(i = 0; i < 2; i++){
		if(req(cfd, buf, sizeof buf, "gem 4096 4096 %s %s", radv ? "gtt,vram" : "gtt",
		   radv ? "cpuaccess,uswc,explicitsync" : "0") < 0 || tokenize(buf, f, nelem(f)) != 2)
			sysfatal("gem: %r");
		h[i] = strtoul(f[1], nil, 0);
		gpu[i] = base + i*0x100000;
		if(req(cfd, buf, sizeof buf, "va map %ud %#llux 0 4096 %#ux", h[i], gpu[i],
		   radv && i == 0 ? Varadvib : Vaflags) < 0)
			sysfatal("va map: %r");
		ok("gem handle %ud, va map at %#llux", h[i], gpu[i]);
	}
	cmd = map(h[0]);
	dst = map(h[1]);
	ok("map: the processor sees both");

	/* one IB: WRITE_DATA (PM4) into the second object */
	dst[0] = 0;
	cmd[0] = 3U << 30 | 0x37 << 8 | 3 << 16;	/* PACKET3(WRITE_DATA, 3) */
	cmd[1] = 5 << 8 | 1 << 20;		/* memory, confirm */
	cmd[2] = gpu[1] + (unmapped ? 64<<20 : 0);
	cmd[3] = (gpu[1] + (unmapped ? 64<<20 : 0)) >> 32;
	cmd[4] = 0x10c71000;
	if(req(sfd, buf, sizeof buf, "cs %ud 0 ib 0 0 0 %#llux 20 0 bo %ud 0 bo %ud 0", ctxid, gpu[0], h[0], h[1]) < 0 ||
	   tokenize(buf, f, nelem(f)) != 2)
		sysfatal("cs: %r");
	seq = strtoull(f[1], nil, 0);
	ok("cs: sequence %llud", seq);
	if(req(cfd, buf, sizeof buf, "waitcs %ud 0 0 0 %llud 0xffffffffffffffff", ctxid, seq) < 0)
		fail("waitcs: %r");
	else if(strncmp(buf, "waitcs done", 11) != 0)
		fail("waitcs: %s", buf);
	else if(dst[0] != 0x10c71000)
		fail("the GPU wrote %#ux, not 0x10c71000", dst[0]);
	else
		ok("waitcs: done, the GPU wrote %#ux", dst[0]);

	segdetach(cmd);
	segdetach(dst);
	for(i = 0; i < 2; i++)
		if(req(cfd, buf, sizeof buf, "close %ud", h[i]) < 0)
			fail("close %ud: %r", h[i]);
	ok("objects closed");
	if(req(cfd, buf, sizeof buf, "hwctx free %ud", ctxid) < 0)
		fail("hwctx free %ud: %r", ctxid);
	close(sfd);
	close(cfd);
	print(nfail ? "amdcs: %d failed\n" : "amdcs: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
