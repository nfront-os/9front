/*
 * SPDX-License-Identifier: MIT
 *
 * amdioctl: amdgpu's own ioctls through aux/amdgpu's arena (gpu(4)), as
 * libdrm_amdgpu makes them on Linux: the device's information
 * (AMDGPU_INFO), a context, two objects in GTT mapped in the GPU's
 * address space (GEM_VA) and for the processor (map), one IB through
 * AMDGPU_CS with its BO list, WAIT_CS, the value the GPU wrote; and an
 * ioctl whose pointer leaves the arena, refused (EFAULT).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * The structures are those of Linux's amdgpu_drm.h and drm.h (MIT).
 */
#include <u.h>
#include <libc.h>

/* _IOC as Linux encodes it */
#define IOC(dir, nr, size)	((u32int)(dir) << 30 | (u32int)(size) << 16 | 'd' << 8 | (nr))
#define IOW(nr, t)	IOC(1, nr, sizeof(t))
#define IOWR(nr, t)	IOC(3, nr, sizeof(t))
#define CMD	0x40	/* DRM_COMMAND_BASE */

typedef struct Info Info;
struct Info {	/* drm_amdgpu_info */
	uvlong	ret;
	u32int	retsize;
	u32int	query;
	u32int	arg[4];
};
typedef struct Devinfo Devinfo;
struct Devinfo {	/* the start of drm_amdgpu_info_device */
	u32int	device_id, chip_rev, external_rev, pci_rev, family, num_shader_engines, num_shader_arrays_per_engine, gpu_counter_freq;
	uvlong	max_engine_clock, max_memory_clock;
	u32int	cu_active_number;
};
typedef struct Vramgtt Vramgtt;
struct Vramgtt {	/* drm_amdgpu_info_vram_gtt */
	uvlong	vram_size, vram_cpu_accessible_size, gtt_size;
};
typedef struct Gemcreate Gemcreate;
struct Gemcreate {	/* drm_amdgpu_gem_create: in; out.handle over bo_size */
	uvlong	bo_size, alignment, domains, domain_flags;
};
typedef struct Ctxreq Ctxreq;
struct Ctxreq {	/* drm_amdgpu_ctx: in; out.alloc.ctx_id over op */
	u32int	op, flags, ctx_id, priority;
};
typedef struct Gemva Gemva;
struct Gemva {	/* drm_amdgpu_gem_va */
	u32int	handle, pad, operation, flags;
	uvlong	va_address, offset_in_bo, map_size;
};
typedef struct Cs Cs;
struct Cs {	/* drm_amdgpu_cs: in; out.handle over ctx_id */
	u32int	ctx_id, bo_list_handle, num_chunks, flags;
	uvlong	chunks;
};
typedef struct Chunk Chunk;
struct Chunk {	/* drm_amdgpu_cs_chunk */
	u32int	chunk_id, length_dw;
	uvlong	chunk_data;
};
typedef struct Ib Ib;
struct Ib {	/* drm_amdgpu_cs_chunk_ib */
	u32int	pad, flags;
	uvlong	va_start;
	u32int	ib_bytes, ip_type, ip_instance, ring;
};
typedef struct Bolist Bolist;
struct Bolist {	/* drm_amdgpu_bo_list_in */
	u32int	operation, list_handle, bo_number, bo_info_size;
	uvlong	bo_info_ptr;
};
typedef struct Boent Boent;
struct Boent {	/* drm_amdgpu_bo_list_entry */
	u32int	bo_handle, bo_priority;
};
typedef struct Waitcs Waitcs;
struct Waitcs {	/* drm_amdgpu_wait_cs: in; out.status over handle */
	uvlong	handle, timeout;
	u32int	ip_type, ip_instance, ring, ctx_id;
};
typedef struct Gemclose Gemclose;
struct Gemclose {	/* drm_gem_close */
	u32int	handle, pad;
};

enum {
	Ginfo	= 0x05, Ggemcreate = 0x00, Gctx = 0x02, Ggemva = 0x08, Gcs = 0x04, Gwaitcs = 0x09,
	Infodev	= 0x16, Infovramgtt = 0x14,
	Domgtt	= 2, Domvram = 4,
	Cpuaccess	= 1<<0, Uswc = 1<<2, Explicitsync = 1<<7,
	Varadvib	= 1<<1 | 1<<3 | 4<<5,	/* readable, executable, MTYPE_UC: RADV's IBs */
	Ctxalloc	= 1,
	Vamap	= 1,
	Vaflags	= 1<<1 | 1<<2 | 1<<3,	/* readable, writeable, executable */
	Chunkib	= 1, Chunkbo = 6,
	Ipgfx	= 0,
};

static char *dev = "/dev/gpu/0";
static int nfail;
static int cfd, ifd;
static uchar *arena, *anext;
static uvlong asize;

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
ctl(char *reply, int nreply, char *fmt, ...)
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

/* room in the arena, zeroed */
static void*
amalloc(ulong n)
{
	void *p;

	n = (n + 15) & ~15;
	if(anext + n > arena + asize)
		sysfatal("arena full");
	p = anext;
	anext += n;
	memset(p, 0, n);
	return p;
}

/* the ioctl, its arguments at a in the arena: 0, or -errno */
static long
ioctl(u32int cmd, void *a)
{
	char buf[64];
	int n;

	if(fprint(ifd, "%#ux %#p", cmd, a) < 0)
		return -9999;
	if((n = pread(ifd, buf, sizeof buf - 1, 0)) <= 0)
		return -9999;
	buf[n] = 0;
	return strtol(buf, nil, 10);
}

/* the processor's view of an object: its segment (GTT: its pages; VRAM: its part of BAR 0) */
static u32int*
map(u32int h)
{
	char buf[256], *f[8];
	void *p;

	if(ctl(buf, sizeof buf, "map %ud", h) < 0)
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
	char path[128], buf[256], *f[8];
	Info *in;
	Devinfo *di;
	Vramgtt *vg;
	Ctxreq *cx;
	Gemcreate *gc;
	Gemva *va;
	Cs *cs;
	Chunk *ch;
	Ib *ib;
	Bolist *bl;
	Boent *be;
	Waitcs *wc;
	Gemclose *cl;
	uvlong *chp;
	u32int h[2], ctxid, *cmd, *dst;
	uvlong gpu[2], base;
	long r;
	int ctx, i, radv, unmapped;

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
		base = strtoull(EARGF(sysfatal("usage: amdioctl [-ru] [-b gpuaddr] [dev]")), nil, 0);
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
	snprint(path, sizeof path, "%s/ctx/%d/ioctl", dev, ctx);
	if((ifd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(ctl(buf, sizeof buf, "arena") < 0 || tokenize(buf, f, nelem(f)) != 4)
		sysfatal("arena: %r %s", buf);
	asize = strtoull(f[3], nil, 0);
	arena = segattach(0, f[1], nil, asize);
	if(arena == (void*)-1)
		sysfatal("segattach %s: %r", f[1]);
	/* the pointers in the arena are the server's too: the same address */
	if((uvlong)arena != strtoull(f[2], nil, 0))
		sysfatal("arena at %#p, the server has it at %s", arena, f[2]);
	anext = arena;
	ok("ctx %d, arena %s at %#p, %llud bytes", ctx, f[1], arena, asize);

	/* the device */
	in = amalloc(sizeof *in);
	di = amalloc(512);
	in->ret = (uvlong)di;
	in->retsize = 512;
	in->query = Infodev;
	if((r = ioctl(IOW(CMD+Ginfo, Info), in)) != 0)
		fail("AMDGPU_INFO_DEV_INFO: %ld", r);
	else
		ok("AMDGPU_INFO_DEV_INFO: device %#ux family %ud, %ud CUs, %ud shader engines, gfx clock %llud kHz",
			di->device_id, di->family, di->cu_active_number, di->num_shader_engines, di->max_engine_clock);
	vg = amalloc(sizeof *vg);
	in->ret = (uvlong)vg;
	in->retsize = sizeof *vg;
	in->query = Infovramgtt;
	if((r = ioctl(IOW(CMD+Ginfo, Info), in)) != 0)
		fail("AMDGPU_INFO_VRAM_GTT: %ld", r);
	else
		ok("AMDGPU_INFO_VRAM_GTT: vram %llud MB (%llud visible), gtt %llud MB",
			vg->vram_size>>20, vg->vram_cpu_accessible_size>>20, vg->gtt_size>>20);

	/* a pointer out of the arena is refused */
	in->ret = (uvlong)buf;
	in->query = Infovramgtt;
	r = ioctl(IOW(CMD+Ginfo, Info), in);
	if(r == -14)
		ok("a pointer out of the arena: EFAULT");
	else
		fail("a pointer out of the arena: %ld, not EFAULT", r);

	/* a context */
	cx = amalloc(sizeof *cx);
	cx->op = Ctxalloc;
	if((r = ioctl(IOWR(CMD+Gctx, Ctxreq), cx)) != 0)
		sysfatal("AMDGPU_CTX: %ld", r);
	ctxid = cx->op;	/* out.alloc.ctx_id */
	ok("AMDGPU_CTX: context %ud", ctxid);

	/* two objects in GTT, in the address space, for the processor */
	for(i = 0; i < 2; i++){
		gc = amalloc(sizeof *gc);
		gc->bo_size = 4096;
		gc->alignment = 4096;
		gc->domains = Domgtt;
		if(radv){
			gc->domains = Domgtt | Domvram;
			gc->domain_flags = Cpuaccess | Uswc | Explicitsync;
		}
		if((r = ioctl(IOWR(CMD+Ggemcreate, Gemcreate), gc)) != 0)
			sysfatal("GEM_CREATE: %ld", r);
		h[i] = *(u32int*)gc;	/* out.handle */
		gpu[i] = base + i*0x100000;
		va = amalloc(sizeof *va);
		va->handle = h[i];
		va->operation = Vamap;
		va->flags = Vaflags;
		if(radv && i == 0)
			va->flags = Varadvib;
		va->va_address = gpu[i];
		va->map_size = 4096;
		if((r = ioctl(IOW(CMD+Ggemva, Gemva), va)) != 0)
			sysfatal("GEM_VA: %ld", r);
		ok("GEM_CREATE handle %ud, GEM_VA at %#llux", h[i], gpu[i]);
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
	ib = amalloc(sizeof *ib);
	ib->va_start = gpu[0];
	ib->ib_bytes = 5*4;
	ib->ip_type = Ipgfx;
	be = amalloc(2 * sizeof *be);
	be[0].bo_handle = h[0];
	be[1].bo_handle = h[1];
	bl = amalloc(sizeof *bl);
	bl->bo_number = 2;
	bl->bo_info_size = sizeof *be;
	bl->bo_info_ptr = (uvlong)be;
	ch = amalloc(2 * sizeof *ch);
	ch[0].chunk_id = Chunkib;
	ch[0].length_dw = sizeof *ib / 4;
	ch[0].chunk_data = (uvlong)ib;
	ch[1].chunk_id = Chunkbo;
	ch[1].length_dw = sizeof *bl / 4;
	ch[1].chunk_data = (uvlong)bl;
	chp = amalloc(2 * sizeof *chp);
	chp[0] = (uvlong)&ch[0];
	chp[1] = (uvlong)&ch[1];
	cs = amalloc(sizeof *cs);
	cs->ctx_id = ctxid;
	cs->num_chunks = 2;
	cs->chunks = (uvlong)chp;
	if((r = ioctl(IOWR(CMD+Gcs, Cs), cs)) != 0)
		sysfatal("AMDGPU_CS: %ld", r);
	ok("AMDGPU_CS: sequence %llud", *(uvlong*)cs);
	wc = amalloc(sizeof *wc);
	wc->handle = *(uvlong*)cs;
	wc->timeout = ~0ULL;	/* no limit (amdgpu_gem_timeout) */
	wc->ip_type = Ipgfx;
	wc->ctx_id = ctxid;
	if((r = ioctl(IOWR(CMD+Gwaitcs, Waitcs), wc)) != 0)
		fail("AMDGPU_WAIT_CS: %ld", r);
	else if(wc->handle != 0)
		fail("AMDGPU_WAIT_CS: status %llud (busy)", wc->handle);
	else if(dst[0] != 0x10c71000)
		fail("the GPU wrote %#ux, not 0x10c71000", dst[0]);
	else
		ok("AMDGPU_WAIT_CS: done, the GPU wrote %#ux", dst[0]);

	segdetach(cmd);
	segdetach(dst);
	for(i = 0; i < 2; i++){
		ctl(nil, 0, "unmap %ud", h[i]);
		cl = amalloc(sizeof *cl);
		cl->handle = h[i];
		if((r = ioctl(IOW(0x09, Gemclose), cl)) != 0)
			fail("GEM_CLOSE %ud: %ld", h[i], r);
	}
	ok("objects closed");
	segdetach(arena);
	close(ifd);
	close(cfd);
	print(nfail ? "amdioctl: %d failed\n" : "amdioctl: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
