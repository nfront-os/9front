/*
 * SPDX-License-Identifier: MIT
 *
 * #G: minimal kernel stub for user-space GPU drivers.
 *
 *	#G/N/ctl	r: pci, id, bars, segments, framebuffer
 *			w: bme on|off, fb <bytes>, irq ack, bar <n> cached
 *	#G/N/irq	read blocks until the device interrupts
 *	#G/N/dirty	read blocks until /dev/draw changed the
 *			framebuffer; returns "x0 y0 x1 y1": what changed
 *			since this open file's last read (each reader its
 *			own: the driver's copy to the card, a compositor's)
 *	#G/N/cursor	read blocks until the cursor (vga hwgc gpu)
 *			changed; returns "x y on gen ox oy clrset size":
 *			the point it is at, shown or not, its image's
 *			generation, offset and 64 bytes clr and set
 *			(cursor.h), hex, and how many pixels big it is
 *			drawn (16: as it is; more: scaled, the offset too)
 *
 * The memory BARs are physical segments gpuNbarM, the
 * framebuffer is gpuNfb (see segattach(2)).  Interrupts are MSI
 * where the device has it (intrenable chooses it), else legacy
 * INTx: then the handler masks them in the PCI command register
 * (interrupt disable), the driver unmasks them with "irq ack"
 * after it has served the device; an MSI is an edge and needs no
 * masking.  There is no mode setting, memory management or
 * firmware here.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"io.h"
#include	"../port/pci.h"
#include	"../port/error.h"

enum {
	Qtopdir,
	Qgpudir,
	Qctl,
	Qirq,
	Qdirty,
	Qcursor,

	Maxgpu	= 4,
	Maxdirty	= 4,	/* readers of #G/N/dirty at once */

	Intdis	= 1<<10,	/* PciPCR: interrupt disable */
	Intsts	= 1<<3,		/* PciPSR: interrupt status */
};

#define TYPE(q)		((ulong)(q).path & 0xF)
#define GPU(q)		(((ulong)(q).path >> 4) & 0xF)
#define QID(g, t)	(((g)<<4)|(t))

typedef struct Gpu Gpu;
typedef struct Dirty Dirty;
struct Gpu
{
	QLock;
	Pcidev	*pci;
	int	id;
	int	segs;		/* bar segments registered */
	char	barseg[nelem(((Pcidev*)0)->mem)][KNAMELEN];
	char	cachedseg[nelem(((Pcidev*)0)->mem)][KNAMELEN];	/* bar N cached: write-back */
	char	wcseg[nelem(((Pcidev*)0)->mem)][KNAMELEN];	/* and write-combining */

	Lock	irqlock;
	Rendez	irqr;
	int	intrenabled;
	int	msi;		/* intrenable gave the device MSI */
	int	irqpending;
	ulong	nirq;

	Lock	dirtylock;
	Dirty	*dirty[Maxdirty];	/* the open dirty files */

	Lock	curlock;	/* ilock: the mouse's interrupt moves it */
	Rendez	curr;
	int	curchanged;
	int	curgen;		/* one more each image */
	int	cursize;	/* drawn this many pixels big (0: 16; vgactl cursorsize) */
	int	curon;
	int	curx, cury;
	int	curox, curoy;	/* the image's offset from the point */
	uchar	curclr[32], curset[32];	/* cursor.h's */

	uchar	*fb;
	ulong	fbsize;
	char	fbseg[KNAMELEN];

	uvlong	oppa;		/* Intel: the ACPI OpRegion (ASLS), with the VBT: page */
	ulong	opsize;		/* and size of its segment */
	char	opseg[KNAMELEN];
};

enum {
	Asls	= 0xFC,		/* Intel graphics: ASL storage, the OpRegion's address */
	Opsize	= 8*1024,	/* the OpRegion (Linux's OPREGION_SIZE) */
};

/* an open #G/N/dirty: what changed since its last read */
struct Dirty {
	Rendez	rend;
	int	isdirty;
	int	r[4];
};

static Gpu gpus[Maxgpu];
static int ngpu;
static Gpu *screengpu;	/* the one whose framebuffer is the screen (vgagpu.c): the first with one */

static Dirtab gpudir[] = {
	"ctl",		{Qctl},		0,	0660,
	"irq",		{Qirq},		0,	0440,
	"dirty",	{Qdirty},	0,	0440,
	"cursor",	{Qcursor},	0,	0440,
};

static void
addseg(char *name, uvlong pa, uvlong size, ulong attr)
{
	Physseg seg;

	memset(&seg, 0, sizeof seg);
	seg.attr = attr;
	seg.name = name;
	seg.pa = pa;
	seg.size = size;
	seg.eveonly = 1;	/* the bars reach all memory (DMA): the driver hands out views (segment(3) phys) */
	if(addphysseg(&seg) == nil)
		print("#G: no room for segment %s\n", name);
}

static void
gpusegs(Gpu *g)
{
	Pcidev *p;
	uvlong pa;
	int i;

	if(g->segs)
		return;
	g->segs = 1;
	p = g->pci;
	for(i = 0; i < nelem(p->mem); i++){
		if(p->mem[i].size == 0 || p->mem[i].bar & 1)
			continue;
		snprint(g->barseg[i], KNAMELEN, "gpu%dbar%d", g->id, i);
		addseg(g->barseg[i], p->mem[i].bar & ~0xFULL, p->mem[i].size,
			SG_PHYSICAL | SG_DEVICE | SG_NOEXEC);
	}
	/*
	 * Intel: the firmware's OpRegion, memory the driver reads the video
	 * BIOS tables (VBT) and talks to ACPI through; its address is in the
	 * device's own configuration space, nothing else is offered
	 */
	if(p->vid == 0x8086 && (pa = (u32int)pcicfgr32(p, Asls)) != 0){
		g->oppa = pa & ~(uvlong)(BY2PG-1);
		g->opsize = ROUND(pa - g->oppa + Opsize, BY2PG);
		snprint(g->opseg, KNAMELEN, "gpu%dopregion", g->id);
		addseg(g->opseg, g->oppa, g->opsize, SG_PHYSICAL | SG_CACHED | SG_NOEXEC);
	}
}

static void
gpureset(void)
{
	Pcidev *p;
	Gpu *g;

	p = nil;
	while(ngpu < Maxgpu && (p = pcimatch(p, 0, 0)) != nil){
		if(p->ccrb != Pcibcdisp)
			continue;
		g = &gpus[ngpu];
		g->pci = p;
		g->id = ngpu++;
	}
}

static int
gpugen(Chan *c, char*, Dirtab*, int, int s, Dir *dp)
{
	Qid q;
	Dirtab *t;
	int n;

	if(s == DEVDOTDOT){
		mkqid(&q, Qtopdir, 0, QTDIR);
		devdir(c, q, "#G", 0, eve, 0555, dp);
		return 1;
	}
	if(TYPE(c->qid) == Qtopdir){
		if(s >= ngpu)
			return -1;
		mkqid(&q, QID(s, Qgpudir), 0, QTDIR);
		snprint(up->genbuf, sizeof up->genbuf, "%d", s);
		devdir(c, q, up->genbuf, 0, eve, 0555, dp);
		return 1;
	}
	if(s >= nelem(gpudir))
		return -1;
	t = &gpudir[s];
	n = GPU(c->qid);
	mkqid(&q, QID(n, t->qid.path), 0, QTFILE);
	devdir(c, q, t->name, 0, eve, t->perm, dp);
	return 1;
}

static Chan*
gpuattach(char *spec)
{
	int i;

	for(i = 0; i < ngpu; i++)
		gpusegs(&gpus[i]);
	return devattach('G', spec);
}

static Walkqid*
gpuwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, nil, 0, gpugen);
}

static int
gpustat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, nil, 0, gpugen);
}

static void
gpuintr(Ureg*, void *a)
{
	Gpu *g;
	Pcidev *p;

	g = a;
	p = g->pci;
	if(!g->msi){
		if((pcicfgr16(p, PciPSR) & Intsts) == 0)
			return;		/* shared line, not ours */
		pcicfgw16(p, PciPCR, pcicfgr16(p, PciPCR) | Intdis);
	}
	ilock(&g->irqlock);
	g->nirq++;
	g->irqpending = 1;
	iunlock(&g->irqlock);
	wakeup(&g->irqr);
}

static Chan*
gpuopen(Chan *c, int omode)
{
	Gpu *g;
	char name[KNAMELEN];
	int msi, i;
	Dirty *d;

	c = devopen(c, omode, nil, 0, gpugen);
	if(TYPE(c->qid) == Qdirty){
		g = &gpus[GPU(c->qid)];
		d = smalloc(sizeof *d);
		lock(&g->dirtylock);
		for(i = 0; i < Maxdirty; i++)
			if(g->dirty[i] == nil){
				g->dirty[i] = d;
				break;
			}
		unlock(&g->dirtylock);
		if(i == Maxdirty){
			free(d);
			c->flag &= ~COPEN;
			error(Einuse);
		}
		c->aux = d;
	}
	if(TYPE(c->qid) == Qirq){
		g = &gpus[GPU(c->qid)];
		qlock(g);
		/*
		 * MSI does not need the INTx line, which firmware often
		 * leaves unassigned for an integrated GPU (0xFF)
		 */
		msi = pcicap(g->pci, PciCapMSI);
		if(!g->intrenabled && (msi >= 0 || g->pci->intl != 0 && g->pci->intl != 0xFF)){
			snprint(name, sizeof name, "gpu%d", g->id);
			intrenable(g->pci->intl, gpuintr, g, g->pci->tbdf, name);
			g->intrenabled = 1;
			g->msi = msi >= 0 && (pcicfgr16(g->pci, msi+2) & 1) != 0;
		}
		qunlock(g);
	}
	return c;
}

static void
gpuclose(Chan *c)
{
	Gpu *g;
	Dirty *d;
	int i;

	if((c->flag & COPEN) == 0 || TYPE(c->qid) != Qdirty || (d = c->aux) == nil)
		return;
	g = &gpus[GPU(c->qid)];
	lock(&g->dirtylock);
	for(i = 0; i < Maxdirty; i++)
		if(g->dirty[i] == d)
			g->dirty[i] = nil;
	unlock(&g->dirtylock);
	c->aux = nil;
	free(d);
}

static int
irqpending(void *a)
{
	return ((Gpu*)a)->irqpending;
}

static int
isdirty(void *a)
{
	return ((Dirty*)a)->isdirty;
}

static int
curchanged(void *a)
{
	return ((Gpu*)a)->curchanged;
}

static long
ctlread(Gpu *g, void *a, long n, vlong off)
{
	Pcidev *p;
	char *s, *e, *buf;
	int i;

	p = g->pci;
	buf = smalloc(READSTR);
	if(waserror()){
		free(buf);
		nexterror();
	}
	s = buf;
	e = buf + READSTR;
	s = seprint(s, e, "pci %d.%d.%d\n", BUSBNO(p->tbdf), BUSDNO(p->tbdf), BUSFNO(p->tbdf));
	s = seprint(s, e, "id %.4ux/%.4ux\n", p->vid, p->did);
	s = seprint(s, e, "irq %d %s %lud %s\n", p->intl, g->intrenabled ? "on" : "off", g->nirq,
		g->msi ? "msi" : "intx");
	for(i = 0; i < nelem(p->mem); i++){
		if(p->mem[i].size == 0)
			continue;
		s = seprint(s, e, "bar%d %#llux %lld %s", i, p->mem[i].bar & ~0xFULL,
			p->mem[i].size, g->barseg[i][0] ? g->barseg[i] : "-");
		if(g->cachedseg[i][0])
			s = seprint(s, e, " %s %s", g->cachedseg[i], g->wcseg[i]);
		s = seprint(s, e, "\n");
	}
	if(g->opseg[0])
		s = seprint(s, e, "mem %#llux %lud %s\n", g->oppa, g->opsize, g->opseg);
	if(g->fb != nil)
		s = seprint(s, e, "fb %#llux %lud %s\n", (uvlong)PADDR(g->fb), g->fbsize, g->fbseg);
	else
		s = seprint(s, e, "fb none\n");
	USED(s);
	n = readstr(off, a, n, buf);
	poperror();
	free(buf);
	return n;
}

static long
gpuread(Chan *c, void *a, long n, vlong off)
{
	Gpu *g;
	char buf[64];
	int r[4];
	ulong cnt;
	Dirty *d;

	if(c->qid.type & QTDIR)
		return devdirread(c, a, n, nil, 0, gpugen);
	g = &gpus[GPU(c->qid)];
	switch(TYPE(c->qid)){
	case Qctl:
		return ctlread(g, a, n, off);
	case Qirq:
		sleep(&g->irqr, irqpending, g);
		ilock(&g->irqlock);
		g->irqpending = 0;
		cnt = g->nirq;
		iunlock(&g->irqlock);
		snprint(buf, sizeof buf, "%lud\n", cnt);
		return readstr(0, a, n, buf);
	case Qdirty:
		d = c->aux;
		sleep(&d->rend, isdirty, d);
		lock(&g->dirtylock);
		memmove(r, d->r, sizeof r);
		d->isdirty = 0;
		unlock(&g->dirtylock);
		snprint(buf, sizeof buf, "%d %d %d %d\n", r[0], r[1], r[2], r[3]);
		return readstr(0, a, n, buf);
	case Qcursor:
		{
			char cbuf[256], *p, *e;
			uchar clr[32], set[32];
			int x, y, ox, oy, on, gen, i, size;

			sleep(&g->curr, curchanged, g);
			ilock(&g->curlock);
			x = g->curx;
			y = g->cury;
			ox = g->curox;
			oy = g->curoy;
			on = g->curon;
			gen = g->curgen;
			size = g->cursize ? g->cursize : 16;
			memmove(clr, g->curclr, sizeof clr);
			memmove(set, g->curset, sizeof set);
			g->curchanged = 0;
			iunlock(&g->curlock);
			e = cbuf + sizeof cbuf;
			p = seprint(cbuf, e, "%d %d %d %d %d %d ", x, y, on, gen, ox, oy);
			for(i = 0; i < sizeof clr; i++)
				p = seprint(p, e, "%.2ux", clr[i]);
			for(i = 0; i < sizeof set; i++)
				p = seprint(p, e, "%.2ux", set[i]);
			seprint(p, e, " %d\n", size);
			return readstr(0, a, n, cbuf);
		}
	}
	error(Egreg);
	return 0;
}

static void
fballoc(Gpu *g, ulong size)
{
	size = PGROUND(size);
	if(g->fb != nil){
		if(size > g->fbsize)
			error("framebuffer already allocated and too small");
		return;
	}
	g->fb = xspanalloc(size, BY2PG, 0);
	if(g->fb == nil)
		error(Enomem);
	memset(g->fb, 0, size);
	g->fbsize = size;
	snprint(g->fbseg, KNAMELEN, "gpu%dfb", g->id);
	/*
	 * cached, as the kernel has it (devdraw draws there): without
	 * SG_CACHED the driver read it uncached, every copy of a change to
	 * the card crawled (the screen slower than VESA's, a window's
	 * frames torn), and one page had two memory types
	 */
	addseg(g->fbseg, PADDR(g->fb), size, SG_PHYSICAL | SG_CACHED | SG_NOEXEC);
	if(screengpu == nil)
		screengpu = g;
}

/*
 * a cached (write-back) and a write-combining segment for a memory
 * bar, besides the uncached one: only for memory that is RAM to the
 * processor, such as virtio-gpu's host visible memory (the driver
 * knows, not #G); the MTRRs make the bar write-back, so that the
 * page attributes decide.  The driver tells its clients which part
 * to map how (virtio-gpu: the host's map_info of each blob).
 */
extern char *gpubarcache(uvlong, uvlong);	/* pc/vgagpu.c: the MTRRs */

static void
barcached(Gpu *g, int i)
{
	Pcidev *p;
	char *err;

	p = g->pci;
	if(i < 0 || i >= nelem(p->mem) || p->mem[i].size == 0 || p->mem[i].bar & 1)
		error("no such memory bar");
	if(g->cachedseg[i][0])
		return;
	if((err = gpubarcache(p->mem[i].bar & ~0xFULL, p->mem[i].size)) != nil)
		print("#G: bar%d write-back: %s\n", i, err);
	snprint(g->cachedseg[i], KNAMELEN, "gpu%dbar%dc", g->id, i);
	addseg(g->cachedseg[i], p->mem[i].bar & ~0xFULL, p->mem[i].size,
		SG_PHYSICAL | SG_CACHED | SG_NOEXEC);
	snprint(g->wcseg[i], KNAMELEN, "gpu%dbar%dw", g->id, i);
	addseg(g->wcseg[i], p->mem[i].bar & ~0xFULL, p->mem[i].size,
		SG_PHYSICAL | SG_WC | SG_NOEXEC);
}

/*
 * write back and invalidate the caches of every processor (Linux's
 * wbinvd_on_all_cpus): the drivers need it for memory the GPU reads
 * past the caches; x86 only (pc, pc64)
 */
static void
wbinvdall(void)
{
	int i, w;

	w = up->wired;
	for(i = 0; i < conf.nmach; i++){
		procwired(up, i);
		sched();
		wbinvd();
	}
	up->wired = w;
	sched();
}

static long
gpuwrite(Chan *c, void *a, long n, vlong)
{
	Gpu *g;
	Cmdbuf *cb;

	if(TYPE(c->qid) != Qctl)
		error(Eperm);
	g = &gpus[GPU(c->qid)];
	cb = parsecmd(a, n);
	qlock(g);
	if(waserror()){
		qunlock(g);
		free(cb);
		nexterror();
	}
	if(cb->nf == 2 && strcmp(cb->f[0], "bme") == 0){
		if(strcmp(cb->f[1], "on") == 0)
			pcisetbme(g->pci);
		else
			pciclrbme(g->pci);
	} else if(cb->nf == 2 && strcmp(cb->f[0], "fb") == 0)
		fballoc(g, strtoul(cb->f[1], 0, 0));
	else if(cb->nf == 3 && strcmp(cb->f[0], "bar") == 0 && strcmp(cb->f[2], "cached") == 0)
		barcached(g, atoi(cb->f[1]));
	else if(cb->nf == 2 && strcmp(cb->f[0], "irq") == 0 && strcmp(cb->f[1], "ack") == 0)
		pcicfgw16(g->pci, PciPCR, pcicfgr16(g->pci, PciPCR) & ~Intdis);
	else if(cb->nf == 1 && strcmp(cb->f[0], "wbinvd") == 0)
		wbinvdall();
	else
		error(Ebadctl);
	qunlock(g);
	poperror();
	free(cb);
	return n;
}

/*
 * framebuffer for vgagpu.c: devdraw draws into it,
 * gpudirty collects what changed for the driver.
 */
int
gpufb(uvlong *pa, void **va, ulong *size)
{
	Gpu *g;

	/* not always #G/0: a VM's own VGA may come first, a passed-through card after */
	if((g = screengpu) == nil || g->fb == nil)
		return -1;
	*pa = PADDR(g->fb);
	*va = g->fb;
	*size = g->fbsize;
	return 0;
}

void
gpudirty(int x0, int y0, int x1, int y1)
{
	Gpu *g;
	Dirty *d;
	int i;

	if((g = screengpu) == nil)
		return;
	lock(&g->dirtylock);
	for(i = 0; i < Maxdirty; i++){
		if((d = g->dirty[i]) == nil)
			continue;
		if(!d->isdirty){
			d->r[0] = x0;
			d->r[1] = y0;
			d->r[2] = x1;
			d->r[3] = y1;
			d->isdirty = 1;
		} else {
			if(x0 < d->r[0]) d->r[0] = x0;
			if(y0 < d->r[1]) d->r[1] = y0;
			if(x1 > d->r[2]) d->r[2] = x1;
			if(y1 > d->r[3]) d->r[3] = y1;
		}
		wakeup(&d->rend);
	}
	unlock(&g->dirtylock);
}

/*
 * the cursor for vgagpu.c (hwgc gpu): its image, where it is, shown or
 * not, for the driver (#G/N/cursor); from the mouse's interrupt too
 */
static Gpu*
curlock(void)
{
	Gpu *g;

	if((g = screengpu) == nil)
		return nil;
	ilock(&g->curlock);
	return g;
}

static void
curunlock(Gpu *g)
{
	g->curchanged = 1;
	iunlock(&g->curlock);
	wakeup(&g->curr);
}

/* the image: cursor.h's clr and set (32 bytes each), its offset */
void
gpucurload(uchar *clr, uchar *set, int ox, int oy)
{
	Gpu *g;

	if((g = curlock()) == nil)
		return;
	memmove(g->curclr, clr, sizeof g->curclr);
	memmove(g->curset, set, sizeof g->curset);
	g->curox = ox;
	g->curoy = oy;
	g->curgen++;
	curunlock(g);
}

/* drawn n pixels big (16 to 64): a window system's scale */
void
gpucursize(int n)
{
	Gpu *g;

	if((g = curlock()) == nil)
		return;
	g->cursize = n;
	g->curgen++;
	curunlock(g);
}

void
gpucurmove(int x, int y)
{
	Gpu *g;

	if((g = curlock()) == nil)
		return;
	g->curx = x;
	g->cury = y;
	curunlock(g);
}

void
gpucuron(int on)
{
	Gpu *g;

	if((g = curlock()) == nil)
		return;
	g->curon = on;
	curunlock(g);
}

Dev gpudevtab = {
	'G',
	"gpu",

	gpureset,
	devinit,
	devshutdown,
	gpuattach,
	gpuwalk,
	gpustat,
	gpuopen,
	devcreate,
	gpuclose,
	gpuread,
	devbread,
	gpuwrite,
	devbwrite,
	devremove,
	devwstat,
};
