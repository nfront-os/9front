/*
 * SPDX-License-Identifier: MIT
 *
 * aux/virtiogpu: user-space driver for virtio-gpu (2D), on #G.
 *
 * The device's registers are the memory BARs of #G (gpu0barN),
 * the virtqueue lives in a fixed #g segment, the screen is #G's
 * framebuffer: /dev/draw draws into it (vga type gpu) and the
 * driver copies what #G/0/dirty reports to the host
 * (TRANSFER_TO_HOST_2D, RESOURCE_FLUSH).
 *
 * Mounted after /dev:
 *	/dev/gpu/0/ctl		w: mode WxH
 *	/dev/gpu/0/status
 *	/dev/gpu/0/outputs/Virtual-1/{modes,mode,edid}
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

	Sack = 1,
	Sdriver = 2,
	Sdriverok = 4,
	Sfeaturesok = 8,

	Fedid = 1<<1,		/* VIRTIO_GPU_F_EDID */
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

	Oknodata = 0x1100,
	Okdisplayinfo,
	Okcapsetinfo,
	Okcapset,
	Okedid,

	Fmtbgrx = 2,		/* B8G8R8X8_UNORM: Plan 9's x8r8g8b8 */

	Hdrsz = 24,
	Nq = 64,		/* virtqueue entries we use */

	/* layout of the DMA segment */
	Odesc = 0,
	Oavail = 0x400,
	Oused = 0x800,
	Oreq = 0x1000,
	Oresp = 0x2000,
	Dmasize = 0x10000,
	Dmava = 0x30000000,

	Fbmin = 1920*1200*4,	/* framebuffer at least this big */
};

typedef struct Cap Cap;
struct Cap
{
	int	bar;
	ulong	off;
	ulong	len;
	ulong	mult;	/* notify only */
};

typedef struct Mode Mode;
struct Mode
{
	int	w;
	int	h;
};

static int debug;
static int poll;
static char *gdir = "#G/0";

static int gctlfd = -1;
static int irqfd = -1;

static uchar *bar[6];
static Cap cap[5];
static uchar *common, *isr, *devcfg;
static uchar *notifyp;

static uchar *dma;
static uvlong dmapa;
static u16int qsize;
static u16int lastused;
static ulong features;

static QLock cmdlock;

static Mode disp;	/* what the host wants */
static Mode cur;	/* what we have set */
static int resid;
static uvlong fbpa;
static ulong fbsize;
static uchar edid[1024];
static int edidlen;
static char *initerr;

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
	uvlong size;

	if(bar[i] != nil)
		return;
	snprint(key, sizeof key, "bar%d", i);
	if((s = ctlline(key, buf, sizeof buf)) == nil)
		sysfatal("%s: no %s", gdir, key);
	if(tokenize(s, f, nelem(f)) != 3 || strcmp(f[2], "-") == 0)
		sysfatal("%s: %s is not a memory segment", gdir, key);
	size = strtoull(f[1], nil, 0);
	bar[i] = segattach(0, f[2], nil, size);
	if(bar[i] == (uchar*)-1)
		sysfatal("segattach %s: %r", f[2]);
	DBG(2, "bar%d: %s %#llux at %#p\n", i, f[2], size, bar[i]);
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
		if(t < Capcommon || t > Capdevice || cap[t].len != 0)
			continue;
		cap[t].bar = cfg[o+4];
		cap[t].off = GBIT32(cfg+o+8);
		cap[t].len = GBIT32(cfg+o+12);
		if(t == Capnotify)
			cap[t].mult = GBIT32(cfg+o+16);
		DBG(2, "cap %d: bar %d off %#lux len %#lux\n", t, cap[t].bar, cap[t].off, cap[t].len);
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
}

/* the virtqueue memory: a fixed (contiguous) #g segment */
static void
dmainit(void)
{
	char buf[256], *f[6];
	int fd, n;

	if((fd = create("#g/gpu0dma", OREAD, DMDIR|0700)) >= 0)
		close(fd);
	if((fd = open("#g/gpu0dma/ctl", ORDWR)) < 0)
		sysfatal("open #g/gpu0dma/ctl: %r");
	fprint(fd, "va %#ux %#ux fixed", Dmava, Dmasize);	/* fails if it exists */
	n = pread(fd, buf, sizeof buf-1, 0);
	close(fd);
	if(n <= 0)
		sysfatal("read #g/gpu0dma/ctl: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 5 || strcmp(f[3], "fixed") != 0)
		sysfatal("#g/gpu0dma: not a fixed segment: %s", buf);
	dmapa = strtoull(f[4], nil, 0);
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
	features = f0 & Fedid;
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
	if(qsize == 0)
		sysfatal("no controlq");
	if(qsize > Nq)
		qsize = Nq;
	w16(common, Cqsize, qsize);
	w16(common, Cqmsix, 0xFFFF);
	w64(common, Cqdesc, dmapa+Odesc);
	w64(common, Cqavail, dmapa+Oavail);
	w64(common, Cqused, dmapa+Oused);
	notifyp = bar[cap[Capnotify].bar] + cap[Capnotify].off
		+ r16(common, Cqnotifyoff) * cap[Capnotify].mult;
	w16(common, Cqenable, 1);
	lastused = 0;
	setstatus(Sdriverok);
	DBG(2, "virtio-gpu: features %#lux queue %d\n", features, qsize);
}

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

static void
waitused(void)
{
	char buf[32];
	uchar *used;

	used = dma + Oused;
	while(r16(used, 2) == lastused){
		if(irqfd < 0){
			sleep(1);
			continue;
		}
		if(read(irqfd, buf, sizeof buf) <= 0)
			sysfatal("read irq: %r");
		r8(isr, 0);	/* reading clears it */
		fprint(gctlfd, "irq ack");
	}
	lastused++;
}

/*
 * one command, synchronous: request in desc 0, response in desc 1.
 * returns the response type.
 */
static int
cmd(uchar *rq, int rqlen, uchar *rs, int rslen)
{
	uchar *avail;
	u16int idx;
	int t;

	memmove(dma+Oreq, rq, rqlen);
	memset(dma+Oresp, 0, rslen);
	setdesc(0, dmapa+Oreq, rqlen, Dnext, 1);
	setdesc(1, dmapa+Oresp, rslen, Dwrite, 0);
	avail = dma + Oavail;
	idx = r16(avail, 2);
	w16(avail, 4 + 2*(idx % qsize), 0);
	w16(avail, 2, idx+1);
	w16(notifyp, 0, 0);	/* queue 0 */
	waitused();
	memmove(rs, dma+Oresp, rslen);
	t = GBIT32(rs);
	if(t != Oknodata)
		DBG(2, "cmd %#ux: response %#ux\n", GBIT32(rq), t);
	return t;
}

static uchar*
hdr(uchar *p, int type)
{
	memset(p, 0, Hdrsz);
	PBIT32(p, type);
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
	int n;

	s = ctlline("fb", buf, sizeof buf);
	if(s != nil && strncmp(s, "none", 4) == 0){
		if(need < Fbmin)
			need = Fbmin;
		if(fprint(gctlfd, "fb %lud", need) < 0)
			sysfatal("allocate framebuffer: %r");
		s = ctlline("fb", buf, sizeof buf);
	}
	if(s == nil || (n = tokenize(s, f, nelem(f))) < 2)
		sysfatal("%s: no framebuffer", gdir);
	USED(n);
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

static void
flushrect(int x0, int y0, int x1, int y1)
{
	uchar rq[Hdrsz+32], *p;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 > cur.w) x1 = cur.w;
	if(y1 > cur.h) y1 = cur.h;
	if(x0 >= x1 || y0 >= y1 || resid == 0)
		return;
	p = rect(hdr(rq, Transfer2d), x0, y0, x1-x0, y1-y0);
	PBIT64(p, (uvlong)y0*cur.w*4 + x0*4);
	PBIT32(p+8, resid);
	PBIT32(p+12, 0);
	simplecmd(rq, Hdrsz+32);
	p = rect(hdr(rq, Resflush), x0, y0, x1-x0, y1-y0);
	PBIT32(p, resid);
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
	id = resid+1;
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
	if(resid != 0){
		p = hdr(rq, Resunref);
		PBIT32(p, resid);
		PBIT32(p+4, 0);
		simplecmd(rq, Hdrsz+8);
	}
	resid = id;
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
 * file server
 */
enum {
	Xctl,
	Xstatus,
	Xmodes,
	Xmode,
	Xedid,
};

static char*
modestr(void)
{
	return smprint("%dx%d\n", cur.w, cur.h);
}

static char*
statusstr(void)
{
	char buf[64];

	return smprint("device virtio-gpu\npci %s\nfeatures %#lux\nqueue %d\nirq %s\n"
		"display %dx%d\nmode %dx%d\nresource %d\nfb %#llux %lud\n",
		ctlline("pci", buf, sizeof buf), features, qsize,
		irqfd >= 0 ? "on" : "poll", disp.w, disp.h, cur.w, cur.h, resid, fbpa, fbsize);
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
fsread(Req *r)
{
	char *s;

	switch((int)(uintptr)r->fid->file->aux){
	case Xctl:
	case Xmode:
		qlock(&cmdlock);
		s = modestr();
		qunlock(&cmdlock);
		break;
	case Xstatus:
		qlock(&cmdlock);
		s = statusstr();
		qunlock(&cmdlock);
		break;
	case Xmodes:
		s = modesstr();
		break;
	case Xedid:
		readbuf(r, edid, edidlen);
		respond(r, nil);
		return;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s);
	free(s);
	respond(r, nil);
}

static void
fswrite(Req *r)
{
	char buf[64], *f[2], *e;
	int n, w, h;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	n = tokenize(buf, f, nelem(f));
	switch((int)(uintptr)r->fid->file->aux){
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
		if(e != nil){
			respond(r, e);
			return;
		}
		r->ofcall.count = r->ifcall.count;
		respond(r, nil);
		return;
	}
	respond(r, "permission denied");
}

static Srv fs;

static void
mktree(void)
{
	File *gpu, *n, *outs, *o;
	char *user;

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
	getdisplayinfo();
	getedid();
	if(w == 0){
		w = disp.w;
		h = disp.h;
	}
	fbinit((ulong)w*h*4);
	if((initerr = setmode(w, h)) != nil)
		sysfatal("mode %dx%d: %s", w, h, initerr);
	fprint(2, "%s: %dx%d, fb %#llux %lud, irq %s\n", argv0, cur.w, cur.h,
		fbpa, fbsize, irqfd >= 0 ? "on" : "poll");

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		dirtyproc();
		exits(nil);
	}

	fs.read = fsread;
	fs.write = fswrite;
	mktree();
	postmountsrv(&fs, srvname, "/dev", MAFTER);
	exits(nil);
}
