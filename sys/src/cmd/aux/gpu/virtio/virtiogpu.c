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
#include "../gpufs.c"

enum {
	/* virtio pci capability types */
	Capcommon = 1,
	Capnotify = 2,
	Capisr = 3,
	Capdevice = 4,
	Capshm = 8,
	Ncap = 9,

	Shmhost = 1,		/* VIRTIO_GPU_SHM_ID_HOST_VISIBLE */

	/* how the host maps a blob (RESOURCE_MAP_BLOB's map_info) */
	Mapcachemask = 0x0f,
	Mapcached = 1,
	Mapuncached = 2,
	Mapwc = 3,

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
	Nvctx = 64,
	Blobva = 0x50000000,
	Blobvaend = 0x7f000000,

	Fbmin = 1920*1200*4,	/* framebuffer at least this big */
};

typedef struct Cap Cap;
typedef struct Mode Mode;
typedef struct Slot Slot;
typedef struct Vctx Vctx;
typedef struct Res Res;
typedef struct Hold Hold;
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
	Vctx	*ctx;
	uvlong	fence;
};

/*
 * a resource, held by the contexts that made or imported it (Hold): on
 * the device while any holds it; mapped in the host visible bar while
 * any of them has it mapped. Exported, a token names it: another
 * context imports it by the token (ctx ctl export, import)
 */
struct Res
{
	Res	*xnext;		/* exported */
	int	id;
	int	ref;		/* contexts holding it */
	int	nmapped;	/* of them, with it mapped */
	char	token[33];	/* exported: its name, else empty */
	int	mem;
	uvlong	size;
	char	seg[32];	/* guest memory: #g segment */
	uintptr	segva;
	uvlong	segpa;
	int	mapped;		/* host memory in the hostmem bar */
	uvlong	mapoff;
	int	mapinfo;
};

struct Hold
{
	Hold	*next;
	Res	*r;
	int	mapped;
};

struct Vctx
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
	Hold	*holds;
	char	*reply;
	File	*dir, *ctl, *submit, *fence;
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
static char barseg[6][32];	/* uncached */
static char barcseg[6][32];	/* write-back (bar N cached), else barseg */
static char barwseg[6][32];	/* write-combining, else barseg */
static char *mapseg(int);
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
static Vctx *scanctx;		/* a client's blob is the scanout (ctx scanout) */
static int scanblob;
static uvlong fbpa;
static ulong fbsize;
static uchar edid[1024];
static int edidlen;

static Vctx *vctxs[Nvctx];	/* under gpufs.c's ctxlock */
static File *capdir;
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
	char buf[1024], key[8], *f[5], *s;

	if(bar[i] != nil)
		return;
	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil)
		sysfatal("%s: no %s", gdir, key);
	if(tokenize(s, f, nelem(f)) < 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: %s is not a memory segment", gdir, key);
	barsize[i] = strtoull(f[1], nil, 0);
	snprint(barseg[i], sizeof barseg[i], "%s", f[2]);
	bar[i] = segattach(0, f[2], nil, barsize[i]);
	if(bar[i] == (uchar*)-1)
		sysfatal("segattach %s: %r", f[2]);
	DBG(2, "bar%d: %s %#llux at %#p\n", i, f[2], barsize[i], bar[i]);
}

/*
 * the size and segments of bar i for the clients, without mapping it:
 * uncached, and write-back and write-combining if #G has them (host
 * visible memory is RAM)
 */
static void
barinfo(int i)
{
	char buf[1024], key[8], *f[5], *s;
	int n;

	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil)
		sysfatal("%s: no %s", gdir, key);
	if((n = tokenize(s, f, nelem(f))) < 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: %s is not a memory segment", gdir, key);
	barsize[i] = strtoull(f[1], nil, 0);
	snprint(barseg[i], sizeof barseg[i], "%s", f[2]);
	snprint(barcseg[i], sizeof barcseg[i], "%s", n >= 5 ? f[3] : f[2]);
	snprint(barwseg[i], sizeof barwseg[i], "%s", n >= 5 ? f[4] : f[2]);
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
		/* host memory: the clients map it, cached */
		if(fprint(gctlfd, "bar %d cached", shm.bar) < 0)
			fprint(2, "%s: no cached mapping of the host memory: %r\n", argv0);
		barinfo(shm.bar);
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
	Vctx *c;
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
	scanctx = nil;		/* a client's scanout is over */
	scanblob = 0;
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

/*
 * back into the holes, kept in address order and merged with the holes
 * before and after it: unmerged, the memory came apart into pieces too
 * small for a big blob after a few dozen programs (venus then failed
 * with VK_ERROR_OUT_OF_HOST_MEMORY although most of it was free)
 */
static void
shmfree(uvlong off, uvlong len)
{
	Hole *h, *p, **l;

	len = (len + 0xFFF) & ~0xFFFULL;
	p = nil;
	for(l = &holes; (h = *l) != nil && h->off < off; l = &h->next)
		p = h;
	if(p != nil && p->off + p->len == off){
		p->len += len;
		if(h != nil && off + len == h->off){
			p->len += h->len;
			p->next = h->next;
			free(h);
		}
		return;
	}
	if(h != nil && off + len == h->off){
		h->off = off;
		h->len += len;
		return;
	}
	h = emalloc9p(sizeof *h);
	h->off = off;
	h->len = len;
	h->next = *l;
	*l = h;
}

/* free host visible memory: all of it, the biggest piece; with ctxlock */
static void
shmfreecount(uvlong *all, uvlong *big)
{
	Hole *h;

	*all = *big = 0;
	for(h = holes; h != nil; h = h->next){
		*all += h->len;
		if(h->len > *big)
			*big = h->len;
	}
}

static char*
ctxreply(Vctx *c, char *fmt, ...)
{
	va_list arg;

	free(c->reply);
	va_start(arg, fmt);
	c->reply = vsmprint(fmt, arg);
	va_end(arg);
	return nil;
}

static Res *exported;		/* by token */

static Hold*
findhold(Vctx *c, int id)
{
	Hold *h;

	for(h = c->holds; h != nil; h = h->next)
		if(h->r->id == id)
			return h;
	return nil;
}

static Res*
findres(Vctx *c, int id)
{
	Hold *h;

	if((h = findhold(c, id)) == nil)
		return nil;
	return h->r;
}

static void
hold(Vctx *c, Res *r)
{
	Hold *h;

	h = emalloc9p(sizeof *h);
	h->r = r;
	h->mapped = 0;
	h->next = c->holds;
	c->holds = h;
}

/* the reply that names a resource to a context */
static char*
resreply(Vctx *c, Res *r)
{
	if(r->seg[0])
		return ctxreply(c, "res %d %llud seg %s %#p\n", r->id, r->size, r->seg, r->segva);
	return ctxreply(c, "res %d %llud\n", r->id, r->size);
}

static char*
ctxinit(Vctx *c, int capset, char *name)
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
ctxattach(Vctx *c, int id, int attach)
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
blobcreate(Vctx *c, int mem, int flags, uvlong blobid, uvlong size)
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
	r->ref = 1;
	hold(c, r);
	return resreply(c, r);
}

static char*
blobmap(Vctx *c, int id)
{
	uchar rq[Hdrsz+16], rs[Hdrsz+8], *p;
	Hold *hd;
	Res *r;
	uvlong off;

	if((hd = findhold(c, id)) == nil)
		return "unknown resource";
	r = hd->r;
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
	if(!hd->mapped){
		hd->mapped = 1;
		r->nmapped++;
	}
	return ctxreply(c, "map %d %s %#llux %#llux %llud %#ux\n", id,
		mapseg(r->mapinfo), barsize[shm.bar], r->mapoff, r->size, r->mapinfo);
}

/*
 * the segment a client maps a blob through: as the host maps it
 * (map_info), in the host visible bar
 */
static char*
mapseg(int mapinfo)
{
	switch(mapinfo & Mapcachemask){
	case Mapcached:
		return barcseg[shm.bar];
	case Mapwc:
		return barwseg[shm.bar];
	}
	return barseg[shm.bar];
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
/*
 * scan out the draw screen again, after a client's blob; called with
 * cmdlock held
 */
static void
scanoutfb(void)
{
	uchar rq[Hdrsz+24], *p;

	if(scanctx == nil)
		return;
	p = rect(hdr(rq, Setscanout), 0, 0, cur.w, cur.h);
	PBIT32(p, 0);
	PBIT32(p+4, scanres);
	simplecmd(rq, sizeof rq);
	scanctx = nil;
	scanblob = 0;
	flushrect(0, 0, cur.w, cur.h);
}

/*
 * ctx scanout: a client's host blob (linear x8r8g8b8, the mode's
 * size, stride bytes a row) becomes what the display shows, instead
 * of the draw screen, until it says scanout 0 or goes; each scanout
 * shows the blob as it is then (a swapchain present)
 */
static char*
blobscanout(Vctx *c, int id, int w, int h, int stride)
{
	uchar rq[Hdrsz+72], *p;
	Res *r;
	char *e;

	e = nil;
	qlock(&cmdlock);
	if(id == 0){
		if(scanctx == c)
			scanoutfb();
		goto Out;
	}
	if(scanctx != nil && scanctx != c){
		e = "scanout in use";
		goto Out;
	}
	if((r = findres(c, id)) == nil || r->mem == Blobguest){
		e = "not a host blob";
		goto Out;
	}
	if(w != cur.w || h != cur.h){
		e = "not the mode's size";
		goto Out;
	}
	if(stride < w*4 || (uvlong)stride*h > r->size){
		e = "bad stride";
		goto Out;
	}
	p = rect(hdr(rq, Setscanoutblob), 0, 0, w, h);
	PBIT32(p, 0);		/* scanout */
	PBIT32(p+4, id);
	PBIT32(p+8, w);
	PBIT32(p+12, h);
	PBIT32(p+16, Fmtbgrx);
	PBIT32(p+20, 0);
	memset(p+24, 0, 32);
	PBIT32(p+24, stride);	/* strides[0]; offsets all 0 */
	if(simplecmd(rq, sizeof rq) < 0){
		e = "SET_SCANOUT_BLOB failed";
		goto Out;
	}
	scanctx = c;
	scanblob = id;
	p = rect(hdr(rq, Resflush), 0, 0, w, h);
	PBIT32(p, id);
	PBIT32(p+4, 0);
	simplecmd(rq, Hdrsz+24);
Out:
	qunlock(&cmdlock);
	return e;
}

static char*
resunref(Vctx *c, int id)
{
	uchar rq[Hdrsz+8], *p;
	Hold *hd, **l;
	Res *r, **x;
	char *e;

	for(l = &c->holds; (hd = *l) != nil; l = &hd->next)
		if(hd->r->id == id)
			break;
	if(hd == nil)
		return "unknown resource";
	r = hd->r;
	if(scanctx == c && scanblob == id){
		qlock(&cmdlock);
		scanoutfb();
		qunlock(&cmdlock);
	}
	if(hd->mapped)
		r->nmapped--;
	*l = hd->next;
	free(hd);
	if(--r->ref > 0){
		/* others hold it: this context lets go of it on the device */
		ctxattach(c, id, 0);
		if(r->nmapped == 0 && (e = blobunmap(r)) != nil)
			return e;
		return nil;
	}
	if((e = blobunmap(r)) != nil)
		return e;
	p = hdr(rq, Resunref);
	PBIT32(p, id);
	PBIT32(p+4, 0);
	simplecmd(rq, sizeof rq);
	for(x = &exported; *x != nil; x = &(*x)->xnext)
		if(*x == r){
			*x = r->xnext;
			break;
		}
	if(r->seg[0])
		rmseg(r->seg);
	free(r);
	return nil;
}

/*
 * export: a token that names resource id, for import by another context
 * (of another process) of this server; 128 random bits, as hard to
 * guess as a capability should be
 */
static char*
resexport(Vctx *c, int id)
{
	Res *r;

	if((r = findres(c, id)) == nil)
		return "unknown resource";
	if(r->token[0] == 0){
		snprint(r->token, sizeof r->token, "%08ux%08ux%08ux%08ux",
			truerand(), truerand(), truerand(), truerand());
		r->xnext = exported;
		exported = r;
	}
	return ctxreply(c, "export %s\n", r->token);
}

/* import: the resource a token names, held by this context too */
static char*
resimport(Vctx *c, char *token)
{
	Res *r;
	char *e;

	if(!c->inited)
		return "context not initialized";
	for(r = exported; r != nil; r = r->xnext)
		if(strcmp(r->token, token) == 0)
			break;
	if(r == nil)
		return "no such export";
	if(findres(c, r->id) == nil){
		if((e = ctxattach(c, r->id, 1)) != nil)
			return e;
		hold(c, r);
		r->ref++;
	}
	return resreply(c, r);
}

/* called with qlk held */
static int
fencedone(Vctx *c, uvlong k)
{
	if(k <= c->donefence)
		return 1;
	if(k > c->nfence)
		return 0;
	return c->fdone[k % Fwin];
}

/* SUBMIT_3D of len bytes at off in the context's segment, on a ring */
static char*
submit(Vctx *c, ulong off, ulong len, int ring, uvlong *fence)
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
fencewait(Vctx *c, uvlong k)
{
	qlock(&qlk);
	while(!fencedone(c, k))
		rsleep(&qr);
	qunlock(&qlk);
}

/*
 * file server: gpu(4)'s files as every driver serves them (../gpufs.c),
 * the capability sets (caps/id), the output Virtual-1, and virtio-gpu's
 * words of a context
 */
enum {
	Xcap = Xdrv,	/* caps/id: aux is Xcap | capset index<<8 */
};

static int capid[16], capver[16], capsize[16];

/* a context for the client (gpufs' ctxnew, under its ctxlock): its id is virtio's ctx_id */
static void*
ctxnew(char *uid, int *id, char **reply)
{
	Vctx *c;
	int i;

	USED(uid);
	for(i = 0; i < Nvctx; i++)
		if(vctxs[i] == nil)
			break;
	if(i == Nvctx){
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
	vctxs[i] = c;
	*id = c->id;
	*reply = smprint("ctx %d seg %s %#p %#ux\n", c->id, c->seg, c->segva, Ctxsegsz);
	return c;
}

/* last fid gone: wait for its work, destroy it on the device */
static void
ctxfree(void *lc)
{
	uchar rq[Hdrsz];
	Vctx *c = lc;

	qlock(&qlk);
	while(c->donefence < c->nfence)	/* all of them, on every ring */
		rsleep(&qr);
	qunlock(&qlk);
	qlock(&cmdlock);
	if(scanctx == c)
		scanoutfb();
	qunlock(&cmdlock);
	while(c->holds != nil)
		resunref(c, c->holds->r->id);
	if(c->inited){
		ctxhdr(rq, Ctxdestroy, c->id);
		simplecmd(rq, sizeof rq);
	}
	rmseg(c->seg);
	vctxs[c->id-1] = nil;
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
		uvlong all, big;

		shmfreecount(&all, &big);	/* the caller holds ctxlock */
		t = smprint("%shostmem %s %#llux %#llux free %#llux biggest %#llux\n", s, barseg[shm.bar],
			shm.off, shm.len, all, big);
		free(s);
		s = t;
	}
	if(scanctx != nil){
		t = smprint("%sscanout ctx %d res %d\n", s, scanctx->id, scanblob);
		free(s);
		s = t;
	}
	for(i = 0; i < nelem(vctxs); i++)
		if(vctxs[i] != nil){
			t = smprint("%sctx %d capset %d fence %llud/%llud%s\n", s, vctxs[i]->id,
				vctxs[i]->capset, vctxs[i]->donefence, vctxs[i]->nfence,
				vctxs[i]->failed ? " failed" : "");
			free(s);
			s = t;
		}
	return s;
}

static char*
status(void)
{
	char *s;

	qlock(&ctxlock);
	s = statusstr();
	qunlock(&ctxlock);
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

/* the mode WxH; nil or the error */
static char*
mode(char *spec)
{
	char *e;
	int w, h;

	w = strtol(spec, &e, 10);
	if(*e != 'x')
		return "usage: WxH";
	h = strtol(e+1, nil, 10);
	qlock(&cmdlock);
	e = setmode(w, h);
	qunlock(&cmdlock);
	return e;
}

static char*
vctxctl(Vctx *c, char **f, int n)
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
		Hold *hd;

		if((hd = findhold(c, atoi(f[1]))) == nil)
			return "unknown resource";
		if(hd->mapped){
			hd->mapped = 0;
			if(--hd->r->nmapped == 0)
				return blobunmap(hd->r);
		}
		return nil;
	}
	if(n == 2 && strcmp(f[0], "export") == 0)
		return resexport(c, atoi(f[1]));
	if(n == 2 && strcmp(f[0], "import") == 0)
		return resimport(c, f[1]);
	/* close: gpu(4)'s word of every driver for letting go of an object */
	if(n == 2 && (strcmp(f[0], "unref") == 0 || strcmp(f[0], "close") == 0))
		return resunref(c, atoi(f[1]));
	if(n == 2 && strcmp(f[0], "scanout") == 0 && atoi(f[1]) == 0)
		return blobscanout(c, 0, 0, 0, 0);
	if(n == 5 && strcmp(f[0], "scanout") == 0)
		return blobscanout(c, atoi(f[1]), atoi(f[2]), atoi(f[3]), atoi(f[4]));
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

/* the context's words (under gpufs' ctxlock); their reply (ctxreply) the fid's */
static char*
ctxctl(Gfid *fa, char **f, int n)
{
	Vctx *c = fa->ctx->lc;
	char *e;

	e = vctxctl(c, f, n);
	if(e == nil && c->reply != nil){
		free(fa->reply);
		fa->reply = c->reply;
		c->reply = nil;
	}
	return e;
}

static char*
ctxsubmit(Gfid *fa, char **f, int n)
{
	uvlong fence;

	if(n != 2 && n != 3)
		return "usage: offset length [ring]";
	return submit(fa->ctx->lc, strtoul(f[0], nil, 0), strtoul(f[1], nil, 0),
		n == 3 ? atoi(f[2]) : 0, &fence);
}

static void
fencecount(void *lc, uvlong *done, uvlong *n, int *failed)
{
	Vctx *c = lc;

	*done = c->donefence;
	*n = c->nfence;
	*failed = c->failed;
}

static int
fencewait1(void *lc, uvlong k, int query)
{
	Vctx *c = lc;
	int n;

	if(k > c->nfence)
		return -1;
	if(!query)
		fencewait(c, k);
	qlock(&qlk);
	n = fencedone(c, k);
	qunlock(&qlk);
	return n;
}

static char*
ctlread(void)
{
	char *s;

	qlock(&cmdlock);
	s = modestr();
	qunlock(&cmdlock);
	return s;
}

static char*
ctlwrite(Req*, char **f, int n)
{
	if(n != 2 || strcmp(f[0], "mode") != 0)
		return "usage: mode WxH";
	return mode(f[1]);
}

static int
outputs(char **names, int max)
{
	if(max < 1)
		return 0;
	names[0] = "Virtual-1";
	return 1;
}

static char*
outread(Req *r, char*, int kind)
{
	char *s;

	switch(kind){
	case Xoutedid:
		readbuf(r, edid, edidlen);
		return nil;
	case Xoutmodes:
		s = modesstr();
		break;
	case Xoutmode:
		s = ctlread();
		break;
	default:
		return "no backlight";
	}
	readstr(r, s);
	free(s);
	return nil;
}

static char*
outwrite(Req*, char*, int kind, char *s)
{
	char *f[2];

	if(kind != Xoutmode)
		return "no backlight";
	if(tokenize(s, f, nelem(f)) < 1)
		return "usage: WxH";
	return mode(f[0]);
}

/* caps/id: the capability sets */
static void
tree(File *dir, char *user)
{
	char name[16];
	int i, nc;

	capdir = createfile(dir, "caps", user, DMDIR|0555, nil);
	nc = ncapsets();
	if(nc > nelem(capid))
		nc = nelem(capid);
	for(i = 0; i < nc; i++){
		if(capsetinfo(i, &capid[i], &capver[i], &capsize[i]) < 0)
			continue;
		DBG(2, "capset %d: id %d version %d size %d\n", i, capid[i], capver[i], capsize[i]);
		snprint(name, sizeof name, "%d", capid[i]);
		createfile(capdir, name, user, 0444, (void*)(uintptr)(i<<8 | Xcap));
	}
}

static int
fsdrvread(Req *r)
{
	uchar *data;
	int i, n;

	if(KIND(r->fid->file) != Xcap)
		return 0;
	i = CTXN(r->fid->file);
	if((data = getcapset(capid[i], capver[i], capsize[i], &n)) == nil){
		responderror(r);
		return 1;
	}
	readbuf(r, data, n);
	free(data);
	respond(r, nil);
	return 1;
}

static Gpudrv drv = {
	ctxnew,
	ctxfree,
	ctxctl,
	ctxsubmit,
	fencecount,
	fencewait1,
	status,
	ctlread,
	ctlwrite,
	outputs,
	nil,
	outread,
	outwrite,
	tree,
	fsdrvread,
	nil,
};

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

	gpufsinit(features & (Fvirgl|Fctxinit));
	postmountsrv(&fs, srvname, "/dev", MAFTER);
	exits(nil);
}
