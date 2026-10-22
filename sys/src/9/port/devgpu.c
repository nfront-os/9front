/*
 * SPDX-License-Identifier: MIT
 *
 * #G: minimal kernel stub for user-space GPU drivers.
 *
 *	#G/N/ctl	r: pci, id, bars, segments, framebuffer
 *			w: bme on|off, fb <bytes>, irq ack
 *	#G/N/irq	read blocks until the device interrupts
 *	#G/N/dirty	read blocks until /dev/draw changed the
 *			framebuffer; returns "x0 y0 x1 y1"
 *
 * The memory BARs are physical segments gpuNbarM, the
 * framebuffer is gpuNfb (see segattach(2)).  Interrupts are
 * legacy INTx: the handler masks them in the PCI command
 * register (interrupt disable), the driver unmasks them with
 * "irq ack" after it has served the device.  There is no
 * mode setting, memory management or firmware here.
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

	Maxgpu	= 4,

	Intdis	= 1<<10,	/* PciPCR: interrupt disable */
	Intsts	= 1<<3,		/* PciPSR: interrupt status */
};

#define TYPE(q)		((ulong)(q).path & 0xF)
#define GPU(q)		(((ulong)(q).path >> 4) & 0xF)
#define QID(g, t)	(((g)<<4)|(t))

typedef struct Gpu Gpu;
struct Gpu
{
	QLock;
	Pcidev	*pci;
	int	id;
	int	segs;		/* bar segments registered */
	char	barseg[nelem(((Pcidev*)0)->mem)][KNAMELEN];

	Lock	irqlock;
	Rendez	irqr;
	int	intrenabled;
	int	irqpending;
	ulong	nirq;

	Lock	dirtylock;
	Rendez	dirtyr;
	int	isdirty;
	int	dirty[4];

	uchar	*fb;
	ulong	fbsize;
	char	fbseg[KNAMELEN];
};

static Gpu gpus[Maxgpu];
static int ngpu;

static Dirtab gpudir[] = {
	"ctl",		{Qctl},		0,	0660,
	"irq",		{Qirq},		0,	0440,
	"dirty",	{Qdirty},	0,	0440,
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
	if(addphysseg(&seg) == nil)
		print("#G: no room for segment %s\n", name);
}

static void
gpusegs(Gpu *g)
{
	Pcidev *p;
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
	if((pcicfgr16(p, PciPSR) & Intsts) == 0)
		return;		/* shared line, not ours */
	pcicfgw16(p, PciPCR, pcicfgr16(p, PciPCR) | Intdis);
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

	c = devopen(c, omode, nil, 0, gpugen);
	if(TYPE(c->qid) == Qirq){
		g = &gpus[GPU(c->qid)];
		qlock(g);
		if(!g->intrenabled && g->pci->intl != 0 && g->pci->intl != 0xFF){
			snprint(name, sizeof name, "gpu%d", g->id);
			intrenable(g->pci->intl, gpuintr, g, g->pci->tbdf, name);
			g->intrenabled = 1;
		}
		qunlock(g);
	}
	return c;
}

static void
gpuclose(Chan*)
{
}

static int
irqpending(void *a)
{
	return ((Gpu*)a)->irqpending;
}

static int
isdirty(void *a)
{
	return ((Gpu*)a)->isdirty;
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
	s = seprint(s, e, "irq %d %s %lud\n", p->intl, g->intrenabled ? "on" : "off", g->nirq);
	for(i = 0; i < nelem(p->mem); i++){
		if(p->mem[i].size == 0)
			continue;
		s = seprint(s, e, "bar%d %#llux %lld %s\n", i, p->mem[i].bar & ~0xFULL,
			p->mem[i].size, g->barseg[i][0] ? g->barseg[i] : "-");
	}
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
		sleep(&g->dirtyr, isdirty, g);
		lock(&g->dirtylock);
		memmove(r, g->dirty, sizeof r);
		g->isdirty = 0;
		unlock(&g->dirtylock);
		snprint(buf, sizeof buf, "%d %d %d %d\n", r[0], r[1], r[2], r[3]);
		return readstr(0, a, n, buf);
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
	addseg(g->fbseg, PADDR(g->fb), size, SG_PHYSICAL | SG_NOEXEC);
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
	else if(cb->nf == 2 && strcmp(cb->f[0], "irq") == 0 && strcmp(cb->f[1], "ack") == 0)
		pcicfgw16(g->pci, PciPCR, pcicfgr16(g->pci, PciPCR) & ~Intdis);
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

	if(ngpu == 0 || gpus[0].fb == nil)
		return -1;
	g = &gpus[0];
	*pa = PADDR(g->fb);
	*va = g->fb;
	*size = g->fbsize;
	return 0;
}

void
gpudirty(int x0, int y0, int x1, int y1)
{
	Gpu *g;

	if(ngpu == 0)
		return;
	g = &gpus[0];
	lock(&g->dirtylock);
	if(!g->isdirty){
		g->dirty[0] = x0;
		g->dirty[1] = y0;
		g->dirty[2] = x1;
		g->dirty[3] = y1;
		g->isdirty = 1;
	} else {
		if(x0 < g->dirty[0]) g->dirty[0] = x0;
		if(y0 < g->dirty[1]) g->dirty[1] = y0;
		if(x1 > g->dirty[2]) g->dirty[2] = x1;
		if(y1 > g->dirty[3]) g->dirty[3] = y1;
	}
	unlock(&g->dirtylock);
	wakeup(&g->dirtyr);
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
