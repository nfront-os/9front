/*
 * SPDX-License-Identifier: MIT
 *
 * aux/virtiogpu: user-space driver for virtio-gpu, on #G.
 *
 * The device's registers are the memory BARs of #G (gpu0barN),
 * the virtqueue lives in a fixed #g segment, the screen is #G's
 * framebuffer: /dev/draw draws into it (vga type gpu) and the
 * driver copies what #G/0/dirty reports to the host
 * (TRANSFER_TO_HOST_2D, RESOURCE_FLUSH).
 *
 * Up to Nslot commands are in flight; a completion process
 * reads the used ring (on #G/0/irq, or polling with -p).
 *
 * Mounted after /dev, see gpu(4):
 *	/dev/gpu/0/ctl		w: mode WxH
 *	/dev/gpu/0/status
 *	/dev/gpu/0/outputs/Virtual-1/{modes,mode,edid}
 *	/dev/gpu/0/caps/<capset id>
 *	/dev/gpu/0/clone	-> ctx/N/ctl
 *	/dev/gpu/0/ctx/N/{ctl,submit,fence}
 *
 * usage: aux/virtiogpu [-dp] [-m WxH] [-s srvname]
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>

enum {
	/* virtio pci capability types */
	Capcommon = 1,
	Capnotify = 2,
	Capisr = 3,
	Capdevice = 4,
	Capshm = 8,
	Ncap = 9,

	Shmhost = 1,		/* VIRTIO_GPU_SHM_ID_HOST_VISIBLE */

	/* common configuration (virtio 1.0 §4.1.4.3) */
	Cdevfeatsel = 0,
	Cdevfeat = 4,
	Cdrvfeatsel = 8,
	Cdrvfeat = 12,
	Cmsixcfg = 16,
	Cstatus = 20,
	Cqsel = 22,
	Cqsize = 24,
	Cqmsix = 26,
	Cqenable = 28,
	Cqnotifyoff = 30,
	Cqdesc = 32,
	Cqavail = 40,
	Cqused = 48,

	/* device configuration */
	Dnumscanouts = 8,
	Dnumcapsets = 12,

	Sack = 1,
	Sdriver = 2,
	Sdriverok = 4,
	Sfeaturesok = 8,

	Fvirgl = 1<<0,
	Fedid = 1<<1,
	Fblob = 1<<3,
	Fctxinit = 1<<4,
	Fversion1 = 1<<0,	/* bit 32: VIRTIO_F_VERSION_1 */

	Dnext = 1,
	Dwrite = 2,

	/* virtio-gpu commands and responses */
	Getdisplayinfo = 0x0100,
	Rescreate2d,
	Resunref,
	Setscanout,
	Resflush,
	Transfer2d,
	Attachbacking,
	Detachbacking,
	Getcapsetinfo,
	Getcapset,
	Getedid,
	Resassignuuid,
	Rescreateblob,
	Setscanoutblob,

	Ctxcreate = 0x0200,
	Ctxdestroy,
	Ctxattach,
	Ctxdetach,
	Rescreate3d,
	Transfertohost3d,
	Transferfromhost3d,
	Submit3d,
	Resmapblob,
	Resunmapblob,

	Oknodata = 0x1100,
	Okdisplayinfo,
	Okcapsetinfo,
	Okcapset,
	Okedid,
	Okresuuid,
	Okmapinfo,

	Flagfence = 1<<0,
	Flagringidx = 1<<1,
	Nring = 64,		/* VIRTGPU_MAX_RINGS */
	Fwin = 64,		/* fences tracked one by one per context */

	Blobguest = 1,
	Blobhost3d = 2,
	Blobhost3dguest = 3,

	Fmtbgrx = 2,		/* B8G8R8X8_UNORM: Plan 9's x8r8g8b8 */

	Hdrsz = 24,
	Nq = 64,		/* virtqueue entries we use */
	Nslot = 16,		/* commands in flight, 3 descriptors each */

	/* layout of the DMA segment */
	Odesc = 0,
	Oavail = 0x400,
	Oused = 0x800,
	Oslots = 0x4000,
	Slotsz = 0x2000,	/* request, then response */
	Dmasize = 0x40000,
	Dmava = 0x30000000,

	/* fixed #g segments for contexts and guest blobs */
	Ctxva = 0x40000000,
	Ctxvastep = 0x200000,
	Ctxsegsz = 0x100000,
	Nctx = 64,
	Blobva = 0x50000000,
	Blobvaend = 0x7f000000,

	Fbmin = 1920*1200*4,	/* framebuffer at least this big */
};

typedef struct Cap Cap;
typedef struct Mode Mode;
typedef struct Slot Slot;
typedef struct Ctx Ctx;
typedef struct Res Res;
typedef struct Fidaux Fidaux;
typedef struct Hole Hole;

struct Cap
{
	int	bar;
	uvlong	off;
	uvlong	len;
	ulong	mult;	/* notify only */
};

struct Mode
{
	int	w;
	int	h;
};

struct Slot
{
	int	busy;
	int	done;
	int	async;	/* nobody waits: freed on completion */
	Ctx	*ctx;
	uvlong	fence;
};

struct Res
{
	Res	*next;
	int	id;
	int	mem;
	uvlong	size;
	char	seg[32];	/* guest memory: #g segment */
	uintptr	segva;
	uvlong	segpa;
	int	mapped;		/* host memory in the hostmem bar */
	uvlong	mapoff;
	int	mapinfo;
};

struct Ctx
{
	int	id;		/* virtio ctx_id */
	int	ref;		/* open fids */
	int	capset;
	int	inited;
	char	seg[32];
	uintptr	segva;
	uvlong	segpa;
	uvlong	nfence;		/* submitted */
	uvlong	donefence;	/* completed, with all before it */
	uchar	fdone[Fwin];	/* fence k done: fdone[k%Fwin] */
	int	failed;
	Res	*res;
	char	*reply;
	File	*dir, *ctl, *submit, *fence;
};

struct Fidaux
{
	Ctx	*ctx;
	uvlong	fence;		/* fence file: the fence asked for */
	int	query;		/* answer at once instead of waiting */
};

struct Hole
{
	Hole	*next;
	uvlong	off;
	uvlong	len;
};

static int debug;
static int poll;
static char *gdir = "#G/0";

static int gctlfd = -1;
static int irqfd = -1;

static uchar *bar[6];
static uvlong barsize[6];
static char barseg[6][32];
static Cap cap[Ncap];
static Cap shm;			/* host visible memory */
static uchar *common, *isr, *devcfg;
static uchar *notifyp;

static uchar *dma;
static uvlong dmapa;
static u16int qsize;
static u16int lastused;
static ulong features;
static int nslot;

static QLock qlk;		/* queue, slots, fences */
static Rendez qr;
static Slot slot[Nslot];
static int nextres = 1;

static QLock cmdlock;		/* display state */
static Mode disp;		/* what the host wants */
static Mode cur;		/* what we have set */
static int scanres;
static uvlong fbpa;
static ulong fbsize;
static uchar edid[1024];
static int edidlen;

static QLock ctxlock;
static Ctx *ctxs[Nctx];
static File *ctxdir, *capdir;
static Hole *holes;
static uintptr blobva = Blobva;

static Mode stdmodes[] = {
	640, 480,
	800, 600,
	1024, 768,
	1280, 720,
	1280, 800,
	1280, 1024,
	1600, 900,
	1920, 1080,
	1920, 1200,
};

#define DBG	if(debug) fprint

/*
 * register access; volatile, gcc must not merge or reorder them
 */
static u8int
r8(uchar *p, int o)
{
	return *(volatile u8int*)(p+o);
}

static u16int
r16(uchar *p, int o)
{
	return *(volatile u16int*)(p+o);
}

static u32int
r32(uchar *p, int o)
{
	return *(volatile u32int*)(p+o);
}

static void
w8(uchar *p, int o, u8int v)
{
	*(volatile u8int*)(p+o) = v;
}

static void
w16(uchar *p, int o, u16int v)
{
	*(volatile u16int*)(p+o) = v;
}

static void
w32(uchar *p, int o, u32int v)
{
	*(volatile u32int*)(p+o) = v;
}

static void
w64(uchar *p, int o, uvlong v)
{
	w32(p, o, v);
	w32(p, o+4, v>>32);
}

static char*
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, OREAD)) < 0)
		return nil;
	m = readn(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return nil;
	buf[m] = 0;
	return buf;
}

/* value of the line starting with key in #G/0/ctl */
static char*
ctlline(char *key, char *buf, int n)
{
	char *s, *e, path[64];
	int l;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if(readfile(path, buf, n) == nil)
		sysfatal("read %s: %r", path);
	l = strlen(key);
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != nil)
			*e++ = 0;
		else
			e = s+strlen(s);
		if(strncmp(s, key, l) == 0 && s[l] == ' ')
			return s+l+1;
	}
	return nil;
}

static void
mapbar(int i)
{
	char buf[1024], key[8], *f[4], *s;

	if(bar[i] != nil)
		return;
	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil)
		sysfatal("%s: no %s", gdir, key);
	if(tokenize(s, f, nelem(f)) != 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: %s is not a memory segment", gdir, key);
	barsize[i] = strtoull(f[1], nil, 0);
	snprint(barseg[i], sizeof barseg[i], "%s", f[2]);
	bar[i] = segattach(0, f[2], nil, barsize[i]);
	if(bar[i] == (uchar*)-1)
		sysfatal("segattach %s: %r", f[2]);
	DBG(2, "bar%d: %s %#llux at %#p\n", i, f[2], barsize[i], bar[i]);
}

/* the size of bar i, without mapping it */
static void
barinfo(int i)
{
	char buf[1024], key[8], *f[4], *s;

	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil)
		sysfatal("%s: no %s", gdir, key);
	if(tokenize(s, f, nelem(f)) != 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: %s is not a memory segment", gdir, key);
	barsize[i] = strtoull(f[1], nil, 0);
	snprint(barseg[i], sizeof barseg[i], "%s", f[2]);
}

/* virtio capabilities from the pci configuration space */
static void
findcaps(void)
{
	char buf[1024], path[64], *s;
	uchar cfg[256];
	int fd, o, t, n;

	if((s = ctlline("pci", buf, sizeof buf)) == nil)
		sysfatal("%s: no pci line", gdir);
	snprint(path, sizeof path, "#$/pci/%sraw", s);
	if((fd = open(path, OREAD)) < 0)
		sysfatal("open %s: %r", path);
	n = pread(fd, cfg, sizeof cfg, 0);
	close(fd);
	if(n != sizeof cfg)
		sysfatal("read %s: %r", path);
	if((cfg[6] & 0x10) == 0)
		sysfatal("no pci capabilities");
	for(o = cfg[0x34] & ~3; o != 0; o = cfg[o+1] & ~3){
		if(cfg[o] != 9)		/* vendor specific */
			continue;
		t = cfg[o+3];
		if(t == Capshm){
			if(cfg[o+5] != Shmhost || cfg[o+2] < 24)
				continue;
			shm.bar = cfg[o+4];
			shm.off = GBIT32(cfg+o+8) | (uvlong)GBIT32(cfg+o+16)<<32;
			shm.len = GBIT32(cfg+o+12) | (uvlong)GBIT32(cfg+o+20)<<32;
			DBG(2, "shm: bar %d off %#llux len %#llux\n", shm.bar, shm.off, shm.len);
			continue;
		}
		if(t < Capcommon || t > Capdevice || cap[t].len != 0)
			continue;
		cap[t].bar = cfg[o+4];
		cap[t].off = GBIT32(cfg+o+8);
		cap[t].len = GBIT32(cfg+o+12);
		if(t == Capnotify)
			cap[t].mult = GBIT32(cfg+o+16);
		DBG(2, "cap %d: bar %d off %#llux len %#llux\n", t, cap[t].bar, cap[t].off, cap[t].len);
	}
	for(t = Capcommon; t <= Capdevice; t++){
		if(cap[t].len == 0)
			sysfatal("virtio capability %d missing", t);
		if(cap[t].bar >= nelem(bar))
			sysfatal("virtio capability %d: bad bar", t);
		mapbar(cap[t].bar);
	}
	common = bar[cap[Capcommon].bar] + cap[Capcommon].off;
	isr = bar[cap[Capisr].bar] + cap[Capisr].off;
	devcfg = bar[cap[Capdevice].bar] + cap[Capdevice].off;
	if(shm.len != 0){
		if(shm.bar >= nelem(bar))
			sysfatal("virtio shared memory: bad bar");
		barinfo(shm.bar);	/* the clients map it */
		holes = emalloc9p(sizeof(Hole));
		holes->off = shm.off;
		holes->len = shm.len;
	}
}

/*
 * a fixed (physically contiguous) #g segment: create it unless it
 * exists, return its physical address.
 */
static uvlong
fixedseg(char *name, uintptr va, ulong size, int perm)
{
	char buf[256], path[64], *f[6];
	int fd, n;

	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|perm)) >= 0)
		close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, ORDWR)) < 0)
		return 0;
	fprint(fd, "va %#p %#lux fixed", va, size);	/* fails if it exists */
	n = pread(fd, buf, sizeof buf-1, 0);
	close(fd);
	if(n <= 0)
		return 0;
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 5 || strcmp(f[3], "fixed") != 0){
		werrstr("%s: not a fixed segment: %s", name, buf);
		return 0;
	}
	return strtoull(f[4], nil, 0);
}

static void
rmseg(char *name)
{
	char path[64];

	snprint(path, sizeof path, "#g/%s", name);
	remove(path);
}

/* the virtqueue memory */
static void
dmainit(void)
{
	if((dmapa = fixedseg("gpu0dma", Dmava, Dmasize, 0700)) == 0)
		sysfatal("#g/gpu0dma: %r");
	dma = segattach(0, "gpu0dma", nil, 0);
	if(dma == (uchar*)-1)
		sysfatal("segattach gpu0dma: %r");
	memset(dma, 0, Dmasize);
	DBG(2, "dma: %#p pa %#llux\n", dma, dmapa);
}

static void
setstatus(int s)
{
	w8(common, Cstatus, r8(common, Cstatus) | s);
}

static void
devinit(void)
{
	u32int f0;
	int n;

	w8(common, Cstatus, 0);
	for(n = 0; r8(common, Cstatus) != 0; n++){
		if(n > 1000)
			sysfatal("device reset timed out");
		sleep(1);
	}
	setstatus(Sack);
	setstatus(Sdriver);

	w32(common, Cdevfeatsel, 0);
	f0 = r32(common, Cdevfeat);
	w32(common, Cdevfeatsel, 1);
	if((r32(common, Cdevfeat) & Fversion1) == 0)
		sysfatal("not a virtio 1.0 device");
	features = f0 & (Fvirgl|Fedid|Fblob|Fctxinit);
	if(shm.len == 0)
		features &= ~Fblob;	/* nowhere to map host blobs */
	w32(common, Cdrvfeatsel, 0);
	w32(common, Cdrvfeat, features);
	w32(common, Cdrvfeatsel, 1);
	w32(common, Cdrvfeat, Fversion1);
	setstatus(Sfeaturesok);
	if((r8(common, Cstatus) & Sfeaturesok) == 0)
		sysfatal("device refused the features");

	w16(common, Cmsixcfg, 0xFFFF);
	w16(common, Cqsel, 0);		/* controlq */
	qsize = r16(common, Cqsize);
	if(qsize < 3)
		sysfatal("no controlq");
	if(qsize > Nq)
		qsize = Nq;
	w16(common, Cqsize, qsize);
	nslot = qsize/3;
	if(nslot > Nslot)
		nslot = Nslot;
	w16(common, Cqmsix, 0xFFFF);
	w64(common, Cqdesc, dmapa+Odesc);
	w64(common, Cqavail, dmapa+Oavail);
	w64(common, Cqused, dmapa+Oused);
	notifyp = bar[cap[Capnotify].bar] + cap[Capnotify].off
		+ r16(common, Cqnotifyoff) * cap[Capnotify].mult;
	w16(common, Cqenable, 1);
	lastused = 0;
	setstatus(Sdriverok);
	DBG(2, "virtio-gpu: features %#lux queue %d slots %d\n", features, qsize, nslot);
}

/*
 * the queue
 */
static void
setdesc(int i, uvlong pa, ulong len, int flags, int next)
{
	uchar *d;

	d = dma + Odesc + 16*i;
	w64(d, 0, pa);
	w32(d, 8, len);
	w16(d, 12, flags);
	w16(d, 14, next);
}

static uchar*
slotreq(int s)
{
	return dma + Oslots + s*Slotsz;
}

static uchar*
slotresp(int s)
{
	return dma + Oslots + s*Slotsz + Slotsz/2;
}

/* called with qlk held */
static int
getslot(void)
{
	int i;

	for(;;){
		for(i = 0; i < nslot; i++)
			if(!slot[i].busy){
				memset(&slot[i], 0, sizeof slot[i]);
				slot[i].busy = 1;
				return i;
			}
		rsleep(&qr);
	}
}

/*
 * post the request in slot s: its header part (in the slot),
 * optional data in memory at pa (no copy), the response.
 * called with qlk held.
 */
static void
post(int s, int reqlen, uvlong pa, ulong len, int resplen)
{
	uchar *avail;
	uvlong spa;
	u16int idx;
	int d;

	spa = dmapa + Oslots + s*Slotsz;
	d = 3*s;
	if(len > 0){
		setdesc(d, spa, reqlen, Dnext, d+1);
		setdesc(d+1, pa, len, Dnext, d+2);
		setdesc(d+2, spa+Slotsz/2, resplen, Dwrite, 0);
	} else {
		setdesc(d, spa, reqlen, Dnext, d+1);
		setdesc(d+1, spa+Slotsz/2, resplen, Dwrite, 0);
	}
	memset(slotresp(s), 0, resplen);
	avail = dma + Oavail;
	idx = r16(avail, 2);
	w16(avail, 4 + 2*(idx % qsize), d);
	w16(avail, 2, idx+1);
	w16(notifyp, 0, 0);	/* queue 0 */
}

/* called with qlk held */
static void
reapused(void)
{
	uchar *used, *e;
	Slot *sl;
	Ctx *c;
	int s;

	used = dma + Oused;
	while(r16(used, 2) != lastused){
		e = used + 4 + 8*(lastused % qsize);
		s = r32(e, 0) / 3;
		lastused++;
		if(s < 0 || s >= nslot)
			continue;
		sl = &slot[s];
		sl->done = 1;
		if(sl->ctx != nil)
			DBG(2, "slot %d: ctx %d fence %llud response %#ux\n", s, sl->ctx->id, sl->fence, GBIT32(slotresp(s)));
		if(sl->ctx != nil && sl->fence != 0){
			c = sl->ctx;
			if(GBIT32(slotresp(s)) != Oknodata)
				c->failed = 1;
			c->fdone[sl->fence % Fwin] = 1;
			while(c->donefence < c->nfence && c->fdone[(c->donefence+1) % Fwin])
				c->donefence++;
		}
		if(sl->async)
			sl->busy = 0;
	}
}

static void
completeproc(void)
{
	char buf[32];

	for(;;){
		if(irqfd >= 0){
			if(read(irqfd, buf, sizeof buf) <= 0)
				sysfatal("read irq: %r");
			r8(isr, 0);	/* reading clears it */
		} else
			sleep(1);
		qlock(&qlk);
		reapused();
		rwakeupall(&qr);
		qunlock(&qlk);
		if(irqfd >= 0)
			fprint(gctlfd, "irq ack");
	}
}

/*
 * one command, waiting for its response; the request is rq,
 * optionally followed by len bytes at physical address pa.
 * returns the response type.
 */
static int
cmdx(uchar *rq, int rqlen, uvlong pa, ulong len, uchar *rs, int rslen)
{
	int s, t;

	if(rqlen > Slotsz/2 || rslen > Slotsz/2)
		sysfatal("cmd: request or response too big");
	qlock(&qlk);
	s = getslot();
	memmove(slotreq(s), rq, rqlen);
	post(s, rqlen, pa, len, rslen);
	while(!slot[s].done)
		rsleep(&qr);
	memmove(rs, slotresp(s), rslen);
	slot[s].busy = 0;
	rwakeupall(&qr);
	qunlock(&qlk);
	t = GBIT32(rs);
	if(t != Oknodata)
		DBG(2, "cmd %#ux: response %#ux\n", GBIT32(rq), t);
	return t;
}

static int
cmd(uchar *rq, int rqlen, uchar *rs, int rslen)
{
	return cmdx(rq, rqlen, 0, 0, rs, rslen);
}

static uchar*
hdr(uchar *p, int type)
{
	memset(p, 0, Hdrsz);
	PBIT32(p, type);
	return p+Hdrsz;
}

static uchar*
ctxhdr(uchar *p, int type, int ctx)
{
	hdr(p, type);
	PBIT32(p+16, ctx);
	return p+Hdrsz;
}

static uchar*
rect(uchar *p, int x, int y, int w, int h)
{
	PBIT32(p, x);
	PBIT32(p+4, y);
	PBIT32(p+8, w);
	PBIT32(p+12, h);
	return p+16;
}

static int
simplecmd(uchar *rq, int len)
{
	uchar rs[Hdrsz];

	return cmd(rq, len, rs, sizeof rs) == Oknodata ? 0 : -1;
}

static int
newresid(void)
{
	int id;

	qlock(&qlk);
	id = nextres++;
	qunlock(&qlk);
	return id;
}

/*
 * display
 */
static void
getdisplayinfo(void)
{
	uchar rq[Hdrsz], rs[Hdrsz+16*24], *p;
	int i;

	hdr(rq, Getdisplayinfo);
	if(cmd(rq, sizeof rq, rs, sizeof rs) != Okdisplayinfo)
		sysfatal("GET_DISPLAY_INFO failed");
	for(i = 0; i < 16; i++){
		p = rs + Hdrsz + 24*i;
		if(GBIT32(p+16) == 0)
			continue;
		DBG(2, "scanout %d: %dx%d\n", i, GBIT32(p+8), GBIT32(p+12));
		if(i == 0){
			disp.w = GBIT32(p+8);
			disp.h = GBIT32(p+12);
		}
	}
	if(disp.w <= 0 || disp.h <= 0){
		disp.w = 1024;
		disp.h = 768;
	}
}

static void
getedid(void)
{
	uchar rq[Hdrsz+8], rs[Hdrsz+8+1024], *p;
	int n;

	if((features & Fedid) == 0)
		return;
	p = hdr(rq, Getedid);
	PBIT32(p, 0);
	PBIT32(p+4, 0);
	if(cmd(rq, sizeof rq, rs, sizeof rs) != Okedid)
		return;
	n = GBIT32(rs+Hdrsz);
	if(n > sizeof edid)
		n = sizeof edid;
	memmove(edid, rs+Hdrsz+8, n);
	edidlen = n;
}

static void
fbinit(ulong need)
{
	char buf[1024], *f[4], *s;

	s = ctlline("fb", buf, sizeof buf);
	if(s != nil && strncmp(s, "none", 4) == 0){
		if(need < Fbmin)
			need = Fbmin;
		if(fprint(gctlfd, "fb %lud", need) < 0)
			sysfatal("allocate framebuffer: %r");
		s = ctlline("fb", buf, sizeof buf);
	}
	if(s == nil || tokenize(s, f, nelem(f)) < 2)
		sysfatal("%s: no framebuffer", gdir);
	fbpa = strtoull(f[0], nil, 0);
	fbsize = strtoul(f[1], nil, 0);
}

static int
vgactl(char *fmt, ...)
{
	char buf[128];
	va_list arg;
	int fd, r;

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if((fd = open("#v/vgactl", OWRITE)) < 0)
		return -1;
	r = write(fd, buf, strlen(buf));
	close(fd);
	DBG(2, "vgactl %s: %d\n", buf, r);
	return r < 0 ? -1 : 0;
}

/* called with cmdlock held */
static void
flushrect(int x0, int y0, int x1, int y1)
{
	uchar rq[Hdrsz+32], *p;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 > cur.w) x1 = cur.w;
	if(y1 > cur.h) y1 = cur.h;
	if(x0 >= x1 || y0 >= y1 || scanres == 0)
		return;
	p = rect(hdr(rq, Transfer2d), x0, y0, x1-x0, y1-y0);
	PBIT64(p, (uvlong)y0*cur.w*4 + x0*4);
	PBIT32(p+8, scanres);
	PBIT32(p+12, 0);
	simplecmd(rq, Hdrsz+32);
	p = rect(hdr(rq, Resflush), x0, y0, x1-x0, y1-y0);
	PBIT32(p, scanres);
	PBIT32(p+4, 0);
	simplecmd(rq, Hdrsz+24);
}

/* debugging: a few pixels of the framebuffer as the driver sees them */
static void
peek(void)
{
	static u32int *fb;
	static int pts[] = { 5, 5, 300, 300, 30, 650 };
	int i;

	if(fb == nil){
		fb = segattach(0, "gpu0fb", nil, fbsize);
		if(fb == (u32int*)-1){
			fb = nil;
			fprint(2, "segattach gpu0fb: %r\n");
			return;
		}
	}
	for(i = 0; i < nelem(pts); i += 2)
		fprint(2, "fb[%d,%d] = %#.8ux\n", pts[i], pts[i+1], fb[pts[i+1]*cur.w + pts[i]]);
}

/* called with cmdlock held */
static char*
setmode(int w, int h)
{
	uchar rq[Hdrsz+48], *p;
	int id;

	if(w < 64 || h < 64 || w > 8192 || h > 8192)
		return "bad mode";
	if((uvlong)w*h*4 > fbsize)
		return "mode does not fit the framebuffer";
	id = newresid();
	p = hdr(rq, Rescreate2d);
	PBIT32(p, id);
	PBIT32(p+4, Fmtbgrx);
	PBIT32(p+8, w);
	PBIT32(p+12, h);
	if(simplecmd(rq, Hdrsz+16) < 0)
		return "RESOURCE_CREATE_2D failed";
	p = hdr(rq, Attachbacking);
	PBIT32(p, id);
	PBIT32(p+4, 1);
	PBIT64(p+8, fbpa);
	PBIT32(p+16, (ulong)w*h*4);
	PBIT32(p+20, 0);
	if(simplecmd(rq, Hdrsz+24) < 0)
		return "RESOURCE_ATTACH_BACKING failed";
	p = rect(hdr(rq, Setscanout), 0, 0, w, h);
	PBIT32(p, 0);
	PBIT32(p+4, id);
	if(simplecmd(rq, Hdrsz+24) < 0)
		return "SET_SCANOUT failed";
	if(scanres != 0){
		p = hdr(rq, Resunref);
		PBIT32(p, scanres);
		PBIT32(p+4, 0);
		simplecmd(rq, Hdrsz+8);
	}
	scanres = id;
	cur.w = w;
	cur.h = h;

	if(vgactl("type gpu") < 0)
		return "vgactl type gpu failed";
	if(vgactl("size %dx%dx32 x8r8g8b8", w, h) < 0)
		return "vgactl size failed";
	if(vgactl("drawinit") < 0)
		return "vgactl drawinit failed";
	vgactl("hwgc soft");
	flushrect(0, 0, w, h);
	if(debug > 1)
		peek();
	return nil;
}

static void
dirtyproc(void)
{
	char buf[64], path[64], *f[4];
	int fd, n;

	snprint(path, sizeof path, "%s/dirty", gdir);
	if((fd = open(path, OREAD)) < 0)
		sysfatal("open %s: %r", path);
	for(;;){
		n = read(fd, buf, sizeof buf-1);
		if(n <= 0)
			sysfatal("read %s: %r", path);
		buf[n] = 0;
		if(tokenize(buf, f, nelem(f)) != 4)
			continue;
		DBG(2, "dirty %s %s %s %s\n", f[0], f[1], f[2], f[3]);
		qlock(&cmdlock);
		flushrect(atoi(f[0]), atoi(f[1]), atoi(f[2]), atoi(f[3]));
		if(debug > 1)
			peek();
		qunlock(&cmdlock);
	}
}

/*
 * 3D: capsets, contexts, blob resources, submissions
 */
static int
ncapsets(void)
{
	return (features & (Fvirgl|Fctxinit)) ? r32(devcfg, Dnumcapsets) : 0;
}

/* capset index i: id, max version, max size */
static int
capsetinfo(int i, int *id, int *ver, int *size)
{
	uchar rq[Hdrsz+8], rs[Hdrsz+16], *p;

	p = hdr(rq, Getcapsetinfo);
	PBIT32(p, i);
	PBIT32(p+4, 0);
	if(cmd(rq, sizeof rq, rs, sizeof rs) != Okcapsetinfo)
		return -1;
	*id = GBIT32(rs+Hdrsz);
	*ver = GBIT32(rs+Hdrsz+4);
	*size = GBIT32(rs+Hdrsz+8);
	return 0;
}

static uchar*
getcapset(int id, int ver, int size, int *n)
{
	uchar rq[Hdrsz+8], *rs, *p;

	if(size <= 0 || Hdrsz+size > Slotsz/2){
		werrstr("capset too big");
		return nil;
	}
	rs = emalloc9p(Hdrsz+size);
	p = hdr(rq, Getcapset);
	PBIT32(p, id);
	PBIT32(p+4, ver);
	if(cmd(rq, sizeof rq, rs, Hdrsz+size) != Okcapset){
		free(rs);
		werrstr("GET_CAPSET failed");
		return nil;
	}
	memmove(rs, rs+Hdrsz, size);
	*n = size;
	return rs;
}

/* host visible memory, first fit; called with ctxlock held */
static uvlong
shmalloc(uvlong len)
{
	Hole *h, **l;
	uvlong off;

	len = (len + 0xFFF) & ~0xFFFULL;
	for(l = &holes; (h = *l) != nil; l = &h->next)
		if(h->len >= len){
			off = h->off;
			h->off += len;
			h->len -= len;
			if(h->len == 0){
				*l = h->next;
				free(h);
			}
			return off;
		}
	return ~0ULL;
}

static void
shmfree(uvlong off, uvlong len)
{
	Hole *h;

	len = (len + 0xFFF) & ~0xFFFULL;
	h = emalloc9p(sizeof *h);	/* no merging: good enough for now */
	h->off = off;
	h->len = len;
	h->next = holes;
	holes = h;
}

static char*
ctxreply(Ctx *c, char *fmt, ...)
{
	va_list arg;

	free(c->reply);
	va_start(arg, fmt);
	c->reply = vsmprint(fmt, arg);
	va_end(arg);
	return nil;
}

static Res*
findres(Ctx *c, int id)
{
	Res *r;

	for(r = c->res; r != nil; r = r->next)
		if(r->id == id)
			return r;
	return nil;
}

static char*
ctxinit(Ctx *c, int capset, char *name)
{
	uchar rq[Hdrsz+72], *p;
	int n;

	if(c->inited)
		return "context already initialized";
	if(capset != 0 && (features & Fctxinit) == 0)
		return "device has no context init";
	if((features & (Fvirgl|Fctxinit)) == 0)
		return "device has no 3d";
	p = ctxhdr(rq, Ctxcreate, c->id);
	n = strlen(name);
	if(n > 63)
		n = 63;
	PBIT32(p, n);
	PBIT32(p+4, capset & 0xFF);
	memset(p+8, 0, 64);
	memmove(p+8, name, n);
	if(simplecmd(rq, Hdrsz+72) < 0)
		return "CTX_CREATE failed";
	c->capset = capset;
	c->inited = 1;
	return ctxreply(c, "ctx %d capset %d\n", c->id, capset);
}

static char*
ctxattach(Ctx *c, int id, int attach)
{
	uchar rq[Hdrsz+8], *p;

	p = ctxhdr(rq, attach ? Ctxattach : Ctxdetach, c->id);
	PBIT32(p, id);
	PBIT32(p+4, 0);
	if(simplecmd(rq, sizeof rq) < 0)
		return attach ? "CTX_ATTACH_RESOURCE failed" : "CTX_DETACH_RESOURCE failed";
	return nil;
}

/* called with ctxlock held */
static char*
blobcreate(Ctx *c, int mem, int flags, uvlong blobid, uvlong size)
{
	uchar rq[Hdrsz+48], *p;
	Res *r;
	int n;

	if((features & Fblob) == 0)
		return "device has no blob resources";
	if(!c->inited)
		return "context not initialized";
	if(size == 0 || size > 1ULL<<32)
		return "bad size";
	size = (size + 0xFFF) & ~0xFFFULL;
	r = emalloc9p(sizeof *r);
	memset(r, 0, sizeof *r);
	r->id = newresid();
	r->mem = mem;
	r->size = size;
	n = 0;
	if(mem == Blobguest || mem == Blobhost3dguest){
		if(blobva + size > Blobvaend){
			free(r);
			return "out of guest blob address space";
		}
		snprint(r->seg, sizeof r->seg, "gpu0blob%d", r->id);
		r->segva = blobva;
		if((r->segpa = fixedseg(r->seg, r->segva, size, 0770)) == 0){
			rmseg(r->seg);
			free(r);
			return "no contiguous memory for the blob";
		}
		blobva += (size + Ctxvastep-1) & ~(uintptr)(Ctxvastep-1);
		n = 1;
	}
	p = ctxhdr(rq, Rescreateblob, c->id);
	PBIT32(p, r->id);
	PBIT32(p+4, mem);
	PBIT32(p+8, flags);
	PBIT32(p+12, n);
	PBIT64(p+16, blobid);
	PBIT64(p+24, size);
	if(n){
		PBIT64(p+32, r->segpa);
		PBIT32(p+40, size);
		PBIT32(p+44, 0);
	}
	if(simplecmd(rq, Hdrsz+32+16*n) < 0){
		if(r->seg[0])
			rmseg(r->seg);
		free(r);
		return "RESOURCE_CREATE_BLOB failed";
	}
	r->next = c->res;
	c->res = r;
	if(r->seg[0])
		return ctxreply(c, "res %d %llud seg %s %#p\n", r->id, size, r->seg, r->segva);
	return ctxreply(c, "res %d %llud\n", r->id, size);
}

static char*
blobmap(Ctx *c, int id)
{
	uchar rq[Hdrsz+16], rs[Hdrsz+8], *p;
	Res *r;
	uvlong off;

	if((r = findres(c, id)) == nil)
		return "unknown resource";
	if(r->mapped)
		goto Reply;
	if(shm.len == 0)
		return "no host visible memory";
	if((off = shmalloc(r->size)) == ~0ULL)
		return "out of host visible memory";
	p = hdr(rq, Resmapblob);
	PBIT32(p, id);
	PBIT32(p+4, 0);
	PBIT64(p+8, off - shm.off);
	if(cmd(rq, sizeof rq, rs, sizeof rs) != Okmapinfo){
		shmfree(off, r->size);
		return "RESOURCE_MAP_BLOB failed";
	}
	r->mapped = 1;
	r->mapoff = off;
	r->mapinfo = GBIT32(rs+Hdrsz);
Reply:
	return ctxreply(c, "map %d %s %#llux %#llux %llud %#ux\n", id,
		barseg[shm.bar], barsize[shm.bar], r->mapoff, r->size, r->mapinfo);
}

static char*
blobunmap(Res *r)
{
	uchar rq[Hdrsz+8], *p;

	if(!r->mapped)
		return nil;
	p = hdr(rq, Resunmapblob);
	PBIT32(p, r->id);
	PBIT32(p+4, 0);
	if(simplecmd(rq, sizeof rq) < 0)
		return "RESOURCE_UNMAP_BLOB failed";
	shmfree(r->mapoff, r->size);
	r->mapped = 0;
	return nil;
}

/* called with ctxlock held */
static char*
resunref(Ctx *c, int id)
{
	uchar rq[Hdrsz+8], *p;
	Res *r, **l;
	char *e;

	for(l = &c->res; (r = *l) != nil; l = &r->next)
		if(r->id == id)
			break;
	if(r == nil)
		return "unknown resource";
	if((e = blobunmap(r)) != nil)
		return e;
	p = hdr(rq, Resunref);
	PBIT32(p, id);
	PBIT32(p+4, 0);
	simplecmd(rq, sizeof rq);
	*l = r->next;
	if(r->seg[0])
		rmseg(r->seg);
	free(r);
	return nil;
}

/* called with qlk held */
static int
fencedone(Ctx *c, uvlong k)
{
	if(k <= c->donefence)
		return 1;
	if(k > c->nfence)
		return 0;
	return c->fdone[k % Fwin];
}

/* SUBMIT_3D of len bytes at off in the context's segment, on a ring */
static char*
submit(Ctx *c, ulong off, ulong len, int ring, uvlong *fence)
{
	uchar *p;
	int s;

	if(!c->inited)
		return "context not initialized";
	if(off > Ctxsegsz || len > Ctxsegsz - off || (len & 3) != 0)
		return "bad command buffer";
	if(ring < 0 || ring >= Nring)
		return "bad ring";
	qlock(&qlk);
	if(c->nfence+1 - c->donefence > Fwin){
		qunlock(&qlk);
		return "too many fences in flight";
	}
	s = getslot();
	p = ctxhdr(slotreq(s), Submit3d, c->id);
	c->nfence++;	/* not inside PBIT64: it evaluates its argument 8 times */
	c->fdone[c->nfence % Fwin] = 0;
	PBIT32(slotreq(s)+4, Flagfence | (ring ? Flagringidx : 0));
	PBIT64(slotreq(s)+8, c->nfence);
	slotreq(s)[20] = ring;
	PBIT32(p, len);
	PBIT32(p+4, 0);
	slot[s].async = 1;
	slot[s].ctx = c;
	slot[s].fence = c->nfence;
	*fence = c->nfence;
	post(s, Hdrsz+8, c->segpa+off, len, Hdrsz);
	qunlock(&qlk);
	return nil;
}

static void
fencewait(Ctx *c, uvlong k)
{
	qlock(&qlk);
	while(!fencedone(c, k))
		rsleep(&qr);
	qunlock(&qlk);
}

/*
 * file server
 */
enum {
	Xctl,
	Xstatus,
	Xmodes,
	Xmode,
	Xedid,
	Xclone,
	Xcap,
	Xctxctl,
	Xctxsubmit,
	Xctxfence,

	Xcapbase = 0x100,	/* Xcap: aux is Xcapbase + capset index */
	Xctxbase = 0x1000,	/* ctx files: aux is Xctxbase*(1+ctx)+X */
};

static Srv fs;
static int capid[16], capver[16], capsize[16];

static int
ftype(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	if(a >= Xctxbase)
		return a % Xctxbase;
	if(a >= Xcapbase)
		return Xcap;
	return a;
}

static Ctx*
fctx(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	if(a < Xctxbase)
		return nil;
	return ctxs[a/Xctxbase - 1];
}

static Ctx*
newctx(void)
{
	char name[16];
	Ctx *c;
	int i;
	char *user;

	for(i = 0; i < Nctx; i++)
		if(ctxs[i] == nil)
			break;
	if(i == Nctx){
		werrstr("too many contexts");
		return nil;
	}
	c = emalloc9p(sizeof *c);
	memset(c, 0, sizeof *c);
	c->id = i+1;
	snprint(c->seg, sizeof c->seg, "gpu0ctx%d", c->id);
	c->segva = Ctxva + i*Ctxvastep;
	if((c->segpa = fixedseg(c->seg, c->segva, Ctxsegsz, 0770)) == 0){
		rmseg(c->seg);
		free(c);
		return nil;
	}
	ctxs[i] = c;
	user = getuser();
	snprint(name, sizeof name, "%d", c->id);
	c->dir = createfile(ctxdir, name, user, DMDIR|0555, nil);
	c->ctl = createfile(c->dir, "ctl", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxctl));
	c->submit = createfile(c->dir, "submit", user, 0220, (void*)(uintptr)(Xctxbase*(i+1)+Xctxsubmit));
	c->fence = createfile(c->dir, "fence", user, 0660, (void*)(uintptr)(Xctxbase*(i+1)+Xctxfence));
	ctxreply(c, "ctx %d seg %s %#p %#ux\n", c->id, c->seg, c->segva, Ctxsegsz);
	return c;
}

/* last fid gone: wait for its work, destroy it on the device */
static void
freectx(Ctx *c)
{
	uchar rq[Hdrsz];

	qlock(&qlk);
	while(c->donefence < c->nfence)	/* all of them, on every ring */
		rsleep(&qr);
	qunlock(&qlk);
	while(c->res != nil)
		resunref(c, c->res->id);
	if(c->inited){
		ctxhdr(rq, Ctxdestroy, c->id);
		simplecmd(rq, sizeof rq);
	}
	removefile(c->ctl);
	removefile(c->submit);
	removefile(c->fence);
	removefile(c->dir);
	closefile(c->ctl);
	closefile(c->submit);
	closefile(c->fence);
	closefile(c->dir);
	rmseg(c->seg);
	ctxs[c->id-1] = nil;
	free(c->reply);
	free(c);
}

static char*
modestr(void)
{
	return smprint("%dx%d\n", cur.w, cur.h);
}

static char*
statusstr(void)
{
	char buf[64], *s, *t;
	int i;

	s = smprint("device virtio-gpu\npci %s\nfeatures%s%s%s%s\nqueue %d slots %d\nirq %s\n"
		"display %dx%d\nmode %dx%d\nresource %d\nfb %#llux %lud\n",
		ctlline("pci", buf, sizeof buf),
		features & Fvirgl ? " virgl" : "", features & Fedid ? " edid" : "",
		features & Fblob ? " blob" : "", features & Fctxinit ? " ctxinit" : "",
		qsize, nslot, irqfd >= 0 ? "on" : "poll", disp.w, disp.h, cur.w, cur.h, scanres, fbpa, fbsize);
	if(shm.len != 0){
		t = smprint("%shostmem %s %#llux %#llux\n", s, barseg[shm.bar], shm.off, shm.len);
		free(s);
		s = t;
	}
	for(i = 0; i < nelem(ctxs); i++)
		if(ctxs[i] != nil){
			t = smprint("%sctx %d capset %d fence %llud/%llud%s\n", s, ctxs[i]->id,
				ctxs[i]->capset, ctxs[i]->donefence, ctxs[i]->nfence,
				ctxs[i]->failed ? " failed" : "");
			free(s);
			s = t;
		}
	return s;
}

static char*
modesstr(void)
{
	char *s, *t;
	int i;

	s = smprint("%dx%d preferred\n", disp.w, disp.h);
	for(i = 0; i < nelem(stdmodes); i++){
		if((uvlong)stdmodes[i].w*stdmodes[i].h*4 > fbsize)
			continue;
		t = smprint("%s%dx%d\n", s, stdmodes[i].w, stdmodes[i].h);
		free(s);
		s = t;
	}
	return s;
}

static void
fsopen(Req *r)
{
	Fidaux *fa;
	Ctx *c;

	switch(ftype(r->fid->file)){
	case Xclone:
		qlock(&ctxlock);
		if((c = newctx()) == nil){
			qunlock(&ctxlock);
			responderror(r);
			return;
		}
		closefile(r->fid->file);
		r->fid->file = c->ctl;
		incref(c->ctl);
		r->ofcall.qid = c->ctl->qid;
		c->ref++;
		qunlock(&ctxlock);
		break;
	case Xctxctl:
	case Xctxsubmit:
	case Xctxfence:
		qlock(&ctxlock);
		if((c = fctx(r->fid->file)) == nil){
			qunlock(&ctxlock);
			respond(r, "context gone");
			return;
		}
		c->ref++;
		qunlock(&ctxlock);
		break;
	default:
		respond(r, nil);
		return;
	}
	fa = emalloc9p(sizeof *fa);
	memset(fa, 0, sizeof *fa);
	fa->ctx = c;
	r->fid->aux = fa;
	respond(r, nil);
}

static void
fsdestroyfid(Fid *f)
{
	Fidaux *fa;
	Ctx *c;

	if((fa = f->aux) == nil)
		return;
	f->aux = nil;
	c = fa->ctx;
	free(fa);
	qlock(&ctxlock);
	if(--c->ref == 0)
		freectx(c);
	qunlock(&ctxlock);
}

static void
fsread(Req *r)
{
	Fidaux *fa;
	Ctx *c;
	char *s;
	uchar *data;
	int i, n;

	fa = r->fid->aux;
	switch(ftype(r->fid->file)){
	case Xctl:
	case Xmode:
		qlock(&cmdlock);
		s = modestr();
		qunlock(&cmdlock);
		break;
	case Xstatus:
		qlock(&ctxlock);
		s = statusstr();
		qunlock(&ctxlock);
		break;
	case Xmodes:
		s = modesstr();
		break;
	case Xedid:
		readbuf(r, edid, edidlen);
		respond(r, nil);
		return;
	case Xcap:
		i = (uintptr)r->fid->file->aux - Xcapbase;
		if((data = getcapset(capid[i], capver[i], capsize[i], &n)) == nil){
			responderror(r);
			return;
		}
		readbuf(r, data, n);
		free(data);
		respond(r, nil);
		return;
	case Xctxctl:
		c = fa->ctx;
		qlock(&ctxlock);
		s = estrdup9p(c->reply != nil ? c->reply : "");
		qunlock(&ctxlock);
		break;
	case Xctxfence:
		c = fa->ctx;
		r->ifcall.offset = 0;
		if(fa->fence == 0){
			s = smprint("%llud %llud%s\n", c->donefence, c->nfence, c->failed ? " failed" : "");
			break;
		}
		if(fa->fence > c->nfence){
			respond(r, "no such fence");
			return;
		}
		if(!fa->query){
			srvrelease(r->srv);
			fencewait(c, fa->fence);
			srvacquire(r->srv);
		}
		qlock(&qlk);
		n = fencedone(c, fa->fence);
		qunlock(&qlk);
		s = smprint("%llud %s%s\n", fa->fence, n ? "done" : "busy", c->failed ? " failed" : "");
		break;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s);
	free(s);
	respond(r, nil);
}

static char*
ctxctl(Ctx *c, char **f, int n)
{
	char *e;
	int i;

	if(n >= 2 && strcmp(f[0], "init") == 0)
		return ctxinit(c, atoi(f[1]), n > 2 ? f[2] : "plan9");
	if(n == 5 && strcmp(f[0], "blob") == 0)
		return blobcreate(c, atoi(f[1]), atoi(f[2]), strtoull(f[3], nil, 0), strtoull(f[4], nil, 0));
	if(n == 2 && strcmp(f[0], "map") == 0)
		return blobmap(c, atoi(f[1]));
	if(n == 2 && strcmp(f[0], "unmap") == 0){
		Res *r;

		if((r = findres(c, atoi(f[1]))) == nil)
			return "unknown resource";
		return blobunmap(r);
	}
	if(n == 2 && strcmp(f[0], "unref") == 0)
		return resunref(c, atoi(f[1]));
	if(n == 2 && (strcmp(f[0], "attach") == 0 || strcmp(f[0], "detach") == 0)){
		i = atoi(f[1]);
		if(findres(c, i) == nil)
			return "unknown resource";
		if((e = ctxattach(c, i, f[0][0] == 'a')) != nil)
			return e;
		return nil;
	}
	return "unknown ctl message";
}

static void
fswrite(Req *r)
{
	char buf[256], *f[8], *e;
	Fidaux *fa;
	uvlong fence;
	int n, w, h;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	n = tokenize(buf, f, nelem(f));
	fa = r->fid->aux;
	switch(ftype(r->fid->file)){
	case Xctl:
		if(n != 2 || strcmp(f[0], "mode") != 0){
			respond(r, "usage: mode WxH");
			return;
		}
		f[0] = f[1];
		/* wet floor */
	case Xmode:
		if(n < 1){
			respond(r, "usage: WxH");
			return;
		}
		w = strtol(f[0], &e, 10);
		if(*e != 'x'){
			respond(r, "usage: WxH");
			return;
		}
		h = strtol(e+1, nil, 10);
		qlock(&cmdlock);
		e = setmode(w, h);
		qunlock(&cmdlock);
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		qlock(&ctxlock);
		e = ctxctl(fa->ctx, f, n);
		qunlock(&ctxlock);
		break;
	case Xctxsubmit:
		if(n != 2 && n != 3){
			respond(r, "usage: offset length [ring]");
			return;
		}
		e = submit(fa->ctx, strtoul(f[0], nil, 0), strtoul(f[1], nil, 0),
			n == 3 ? atoi(f[2]) : 0, &fence);
		break;
	case Xctxfence:
		if(n == 1){
			fa->query = 0;
			fa->fence = strtoull(f[0], nil, 0);
		} else if(n == 2 && (strcmp(f[0], "wait") == 0 || strcmp(f[0], "query") == 0)){
			fa->query = f[0][0] == 'q';
			fa->fence = strtoull(f[1], nil, 0);
		} else {
			respond(r, "usage: [wait|query] fence");
			return;
		}
		e = nil;
		break;
	default:
		e = "permission denied";
	}
	if(e != nil){
		respond(r, e);
		return;
	}
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

static void
mktree(void)
{
	File *gpu, *n, *outs, *o;
	char *user, name[16];
	int i, nc;

	user = getuser();
	fs.tree = alloctree(user, user, DMDIR|0555, nil);
	gpu = createfile(fs.tree->root, "gpu", user, DMDIR|0555, nil);
	n = createfile(gpu, "0", user, DMDIR|0555, nil);
	createfile(n, "ctl", user, 0664, (void*)Xctl);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	outs = createfile(n, "outputs", user, DMDIR|0555, nil);
	o = createfile(outs, "Virtual-1", user, DMDIR|0555, nil);
	createfile(o, "modes", user, 0444, (void*)Xmodes);
	createfile(o, "mode", user, 0664, (void*)Xmode);
	createfile(o, "edid", user, 0444, (void*)Xedid);
	capdir = createfile(n, "caps", user, DMDIR|0555, nil);
	nc = ncapsets();
	if(nc > nelem(capid))
		nc = nelem(capid);
	for(i = 0; i < nc; i++){
		if(capsetinfo(i, &capid[i], &capver[i], &capsize[i]) < 0)
			continue;
		DBG(2, "capset %d: id %d version %d size %d\n", i, capid[i], capver[i], capsize[i]);
		snprint(name, sizeof name, "%d", capid[i]);
		createfile(capdir, name, user, 0444, (void*)(uintptr)(Xcapbase+i));
	}
	if(features & (Fvirgl|Fctxinit)){
		createfile(n, "clone", user, 0666, (void*)Xclone);
		ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
	}
}

static void
usage(void)
{
	fprint(2, "usage: %s [-dp] [-m WxH] [-s srvname]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char path[64], *srvname, *m, *e;
	int w, h;

	srvname = "gpu.0";
	w = h = 0;
	ARGBEGIN{
	case 'd':
		debug++;
		break;
	case 'p':
		poll = 1;
		break;
	case 'm':
		m = EARGF(usage());
		w = strtol(m, &e, 10);
		if(*e != 'x')
			usage();
		h = strtol(e+1, nil, 10);
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();

	qr.l = &qlk;
	snprint(path, sizeof path, "%s/ctl", gdir);
	if((gctlfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	fprint(gctlfd, "bme on");
	findcaps();
	dmainit();
	if(!poll){
		snprint(path, sizeof path, "%s/irq", gdir);
		if((irqfd = open(path, OREAD)) < 0)
			fprint(2, "%s: no interrupts, polling: %r\n", argv0);
		else
			fprint(gctlfd, "irq ack");
	}
	devinit();

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		completeproc();
		exits(nil);
	}

	getdisplayinfo();
	getedid();
	if(w == 0){
		w = disp.w;
		h = disp.h;
	}
	fbinit((ulong)w*h*4);
	qlock(&cmdlock);
	e = setmode(w, h);
	qunlock(&cmdlock);
	if(e != nil)
		sysfatal("mode %dx%d: %s", w, h, e);
	fprint(2, "%s: %dx%d, fb %#llux %lud, irq %s\n", argv0, cur.w, cur.h,
		fbpa, fbsize, irqfd >= 0 ? "on" : "poll");

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		dirtyproc();
		exits(nil);
	}

	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	mktree();
	postmountsrv(&fs, srvname, "/dev", MAFTER);
	exits(nil);
}
