/*
 * Synopsys DesignWare ethernet quality of service mac (dwmac 4.x/5.x,
 * "gmac", eqos): the VisionFive 2's two (JH7110, dwmac-5.20) with
 * Motorcomm YT8531 phys on rgmii. The registers and descriptors from
 * Geoff Collyer's etherdwmac4.c (9k), the driver after 9front's
 * imx8/etherimx.c: Bpool, kprocs for receive, transmit and the sent
 * buffers, the phy through ethermii. The controllers where the
 * machine has them (soc.ether, soc.etherirq); the dma is coherent
 * (dmaflush for others). Clocks and resets are the firmware's (u-boot
 * probes both); the phy's rgmii delays and tx clock inversion are the
 * board's (soc.etherphy, from the board's device tree), as is the mac address
 * u-boot left in the registers unless plan9.ini has ea=.
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/error.h"
#include "../port/netif.h"
#include "../port/etherif.h"
#include "../port/ethermii.h"

enum {
	Maxtu		= 1518,
	Rbsz		= 1536,		/* receive buffer, a multiple of 16 */
	Ntd		= 256,
	Nrd		= 256,
};

enum {				/* registers, by u32int index */
	Cfg		= 0x000/4,	/* MAC_Configuration */
	Pktfilt		= 0x008/4,
	Hashlo		= 0x010/4,
	Hashhi		= 0x014/4,
	Rxq0ctl0	= 0x0A0/4,
	Intenab		= 0x0B4/4,
	Version		= 0x110/4,
	Mdioaddr	= 0x200/4,
	Mdiodata	= 0x204/4,
	Addrhi0		= 0x300/4,
	Addrlo0		= 0x304/4,
	Mmcctl		= 0x700/4,

	Txq0op		= 0xD00/4,	/* MTL tx queue 0 operation mode */
	Rxq0op		= 0xD30/4,

	Dmamode		= 0x1000/4,
	Dmasysbus	= 0x1004/4,
	Dmactl		= 0x1100/4,	/* channel 0 */
	Dmatxctl	= 0x1104/4,
	Dmarxctl	= 0x1108/4,
	Txbasehi	= 0x1110/4,
	Txbase		= 0x1114/4,
	Rxbasehi	= 0x1118/4,
	Rxbase		= 0x111C/4,
	Txtail		= 0x1120/4,
	Rxtail		= 0x1128/4,
	Txlen		= 0x112C/4,	/* descriptors - 1 */
	Rxlen		= 0x1130/4,
	Dmainten	= 0x1134/4,
	Dmasts		= 0x1160/4,
};

enum {
	/* Cfg */
	Acs		= 1<<20,	/* strip the crc */
	Cst		= 1<<21,
	Ps		= 1<<15,	/* port select: 10/100 */
	Fes		= 1<<14,	/* 100 (with Ps) */
	Dm		= 1<<13,	/* full duplex */
	Te		= 1<<1,
	Re		= 1<<0,

	/* Pktfilt */
	Pm		= 1<<4,		/* all multicast */
	Pr		= 1<<0,		/* promiscuous */

	/* Rxq0ctl0 */
	Rxq0en		= 2,		/* queue 0 for generic traffic */

	/* Mdioaddr */
	Mdiopa		= 21,		/* phy address */
	Mdiorda		= 16,		/* its register */
	Mdiocr		= 8,		/* csr clock range */
	Mdiocrmask	= 0xF<<8,
	Mdiowrite	= 1<<2,
	Mdioread	= 3<<2,
	Mdiobusy	= 1<<0,

	/* Addrhi0 */
	Ae		= 1u<<31,

	/* Txq0op */
	Tsf		= 1<<1,		/* store and forward */
	Txqen		= 2<<2,
	Tqs		= (2048/256-1)<<16,

	/* Rxq0op */
	Rsf		= 1<<5,
	Rqs		= (2048/256-1)<<20,

	/* Dmamode */
	Swr		= 1<<0,

	/* Dmasysbus */
	Eame		= 1<<11,	/* enhanced (40-bit) addresses */

	/* Dmatxctl, Dmarxctl */
	Pblshift	= 16,
	Pbl		= 16,		/* the device tree's txpbl, rxpbl */
	Rbszshift	= 1,
	Dmastart	= 1<<0,

	/* Dmainten, Dmasts */
	Nie		= 1<<15,
	Aie		= 1<<14,
	Fbe		= 1<<12,	/* fatal bus error */
	Rbu		= 1<<7,		/* receive buffer unavailable */
	Ri		= 1<<6,
	Ti		= 1<<0,
};

enum {				/* descriptor word 3 */
	Own		= 1u<<31,
	Ioc		= 1<<30,	/* rx (read format): interrupt */
	Fd		= 1<<29,	/* first */
	Ld		= 1<<28,	/* last */
	Buf1v		= 1<<24,	/* rx: buffer 1 valid */
	Es		= 1<<15,	/* rx: error summary */
	Lenmask		= 0x7FFF,

	Txioc		= 1u<<31,	/* tx word 2: interrupt */
};

typedef struct Descr Descr;
struct Descr
{
	u32int	addr;		/* the buffer, low and high (Eame) */
	u32int	addrhi;
	u32int	ctl;
	u32int	sts;
};

typedef struct Ctlr Ctlr;
struct Ctlr
{
	u32int	*regs;
	uintptr	physreg;
	int	irq;
	int	ctlrno;		/* which of soc.ether */
	int	active;
	u32int	cr;		/* mdio clock range */

	Bpool	pool;

	struct {
		Block	*b[Nrd];
		Descr	*d;
		Rendez;
	}	rx[1];

	struct {
		Block	*b[Ntd];
		Descr	*d;
		uint	i;		/* next to fill (txproc) */
		uint	f;		/* next to free (frproc) */
		Rendez;
	}	tx[1];

	struct {
		Rendez;
	}	free[1];

	struct {
		Mii;
		Rendez	link;
	}	mii[1];

	int	attached;
	QLock;
	Lock	reglock;
};

static Ctlr *ctlrtab[nelem(soc.ether)];
static int nctlr;

#define rr(c, r)	(((volatile u32int*)(c)->regs)[r])
#define wr(c, r, v)	(((volatile u32int*)(c)->regs)[r] = (v))

static int
mdiowait(Ctlr *ctlr)
{
	int i;

	for(i = 0; i < 1000; i++){
		if((rr(ctlr, Mdioaddr) & Mdiobusy) == 0)
			return 0;
		microdelay(10);
	}
	return -1;
}

static int
mdiow(Mii *mii, int phy, int addr, int data)
{
	Ctlr *ctlr = mii->ctlr;

	if(mdiowait(ctlr) < 0)
		return -1;
	wr(ctlr, Mdiodata, data & 0xFFFF);
	wr(ctlr, Mdioaddr, phy<<Mdiopa | addr<<Mdiorda | ctlr->cr | Mdiowrite | Mdiobusy);
	if(mdiowait(ctlr) < 0)
		return -1;
	return data & 0xFFFF;
}

static int
mdior(Mii *mii, int phy, int addr)
{
	Ctlr *ctlr = mii->ctlr;

	if(mdiowait(ctlr) < 0)
		return -1;
	wr(ctlr, Mdioaddr, phy<<Mdiopa | addr<<Mdiorda | ctlr->cr | Mdioread | Mdiobusy);
	if(mdiowait(ctlr) < 0)
		return -1;
	return rr(ctlr, Mdiodata) & 0xFFFF;
}

/*
 * the Motorcomm YT8531 (its datasheet): extended registers
 * through 0x1E/0x1F; rgmii delays in 150ps steps, the receive clock
 * delay of 2ns off; the drive strength of the receive clock for 3970µA
 * at 1.8V; the tx clock inverted at the speeds the board says.
 */
enum {
	Yt8531id	= 0x4f51e91b,
	Ytextaddr	= 0x1E,
	Ytextdata	= 0x1F,
	Ytchipcfg	= 0xA001,
	 Rxcdlyen	= 1<<8,
	Ytrgmiicfg1	= 0xA003,
	 Txclkinv	= 1<<14,
	 Rxdlyshift	= 10,
	 Rxdlymask	= 0xF<<10,
	 Getxdlymask	= 0xF<<0,
	Ytpaddrive	= 0xA010,
	 Rxcdsshift	= 13,
	 Rxcdsmask	= 7<<13,
};

static int
ytextmodify(MiiPhy *phy, int reg, int mask, int set)
{
	int v;

	if(miimiw(phy, Ytextaddr, reg) < 0 || (v = miimir(phy, Ytextdata)) < 0)
		return -1;
	return miimiw(phy, Ytextdata, (v & ~mask) | set);
}

static int
isyt8531(Ctlr *ctlr)
{
	MiiPhy *phy = ctlr->mii->curphy;

	return phy != nil && phy->id == Yt8531id && soc.etherphy[ctlr->ctlrno].rxdly | soc.etherphy[ctlr->ctlrno].txdly;
}

static void
ytconfig(Ctlr *ctlr)
{
	MiiPhy *phy = ctlr->mii->curphy;
	int rx, tx;

	if(!isyt8531(ctlr))
		return;
	rx = soc.etherphy[ctlr->ctlrno].rxdly / 150;
	tx = soc.etherphy[ctlr->ctlrno].txdly / 150;
	ytextmodify(phy, Ytchipcfg, Rxcdlyen, 0);
	ytextmodify(phy, Ytrgmiicfg1, Rxdlymask|Getxdlymask, (rx&0xF)<<Rxdlyshift | (tx&0xF));
	ytextmodify(phy, Ytpaddrive, Rxcdsmask, 6<<Rxcdsshift);
}

static void
ytspeed(Ctlr *ctlr, int mbps)
{
	int bit;

	if(!isyt8531(ctlr))
		return;
	bit = mbps == 1000? 1<<2: mbps == 100? 1<<1: 1<<0;
	ytextmodify(ctlr->mii->curphy, Ytrgmiicfg1, Txclkinv,
		(soc.etherphy[ctlr->ctlrno].txinv & bit)? Txclkinv: 0);
}

static void
interrupt(Ureg*, void *arg)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;
	u32int s;

	s = rr(ctlr, Dmasts);
	wr(ctlr, Dmasts, s);
	if(s & (Ri|Rbu))
		wakeup(ctlr->rx);
	if(s & Ti)
		wakeup(ctlr->tx);
	if(s & Fbe)
		iprint("#l%d: dwmac: fatal bus error, dma status %#ux\n", edev->ctlrno, s);
}

/*
 * a transmit descriptor is the software's when its block is nil (frproc
 * frees the block after the controller cleared Own); one stays free,
 * so the tail never reaches the descriptor the controller is at
 */
static int
txroom(void *arg)
{
	Ctlr *ctlr = arg;
	uint i;

	i = ctlr->tx->i;
	return ctlr->tx->b[i] == nil && ctlr->tx->b[(i+1)%Ntd] == nil;
}

static void
txproc(void *arg)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;
	Block *b;
	Descr *d;
	uvlong pa;
	uint i;

	while(waserror())
		;

	for(;;){
		if((b = qbread(edev->oq, 100000)) == nil)
			break;

		while(!txroom(ctlr))
			sleep(ctlr->free, txroom, ctlr);

		i = ctlr->tx->i;
		d = &ctlr->tx->d[i];
		dmaflush(1, b->rp, BLEN(b));
		pa = PADDR(b->rp);
		d->addr = pa;
		d->addrhi = pa >> 32;
		d->ctl = Txioc | BLEN(b);
		coherence();
		d->sts = Own | Fd | Ld | BLEN(b);
		coherence();
		ctlr->tx->b[i] = b;
		coherence();
		ctlr->tx->i = i = (i+1) % Ntd;
		wr(ctlr, Txtail, PADDR(&ctlr->tx->d[i]));
	}
}

static int
tddone(void *arg)
{
	Ctlr *ctlr = arg;
	uint i;

	i = ctlr->tx->f;
	return ctlr->tx->b[i] != nil && (ctlr->tx->d[i].sts & Own) == 0;
}

static void
frproc(void *arg)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;
	Block *b;
	uint i;

	while(waserror())
		;

	for(;;){
		while(!tddone(ctlr))
			sleep(ctlr->tx, tddone, ctlr);

		i = ctlr->tx->f;
		b = ctlr->tx->b[i];
		coherence();
		ctlr->tx->b[i] = nil;
		ctlr->tx->f = (i+1) % Ntd;

		wakeup(ctlr->free);
		freeb(b);
	}
}

static void
rxgive(Descr *d, Block *b)
{
	uvlong pa;

	dmaflush(1, b->rp, Rbsz);
	pa = PADDR(b->rp);
	d->addr = pa;
	d->addrhi = pa >> 32;
	d->ctl = 0;
	coherence();
	d->sts = Own | Ioc | Buf1v;
	coherence();
}

static int
rdfull(void *arg)
{
	Descr *d = arg;

	return (d->sts & Own) == 0;
}

/*
 * one receive descriptor stays the software's, the one before the
 * next to fill (the tail): a ring all the controller's would put the
 * tail on its current descriptor, and it would stop. A descriptor
 * filled is kept back, the one kept before handed over.
 */
static void
rxproc(void *arg)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;
	Block *b;
	Descr *d;
	uint s, i, prev;

	while(waserror())
		;

	for(i = 0;;){
		d = &ctlr->rx->d[i];
		s = d->sts;
		if(s & Own){
			sleep(ctlr->rx, rdfull, d);
			continue;
		}
		if((s & (Fd|Ld|Es)) == (Fd|Ld) && (s & Lenmask) <= Maxtu){
			b = ctlr->rx->b[i];
			b->wp = b->rp + (s & Lenmask);
			dmaflush(0, b->rp, BLEN(b));
			etheriq(edev, b);

			while((b = iallocbp(&ctlr->pool)) == nil)
				resrcwait("out of dwmac rx buffers");
			ctlr->rx->b[i] = b;
		}else
			edev->buffs++;
		d->sts = 0;		/* kept back */
		prev = (i+Nrd-1) % Nrd;
		rxgive(&ctlr->rx->d[prev], ctlr->rx->b[prev]);
		wr(ctlr, Rxtail, PADDR(d));
		i = (i+1) % Nrd;
	}
}

static void
linkproc(void *arg)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;
	MiiPhy *phy;
	u32int cfg;
	int link = 0;

	while(waserror())
		;
	phy = ctlr->mii->curphy;
	miiane(phy, ~0, ~0, ~0);
	for(;;){
		miistatus(phy);
		if(phy->link == link){
			tsleep(&ctlr->mii->link, return0, nil, 1000);
			continue;
		}
		link = phy->link;
		if(link){
			ytspeed(ctlr, phy->speed);
			ilock(&ctlr->reglock);
			cfg = rr(ctlr, Cfg) & ~(Ps|Fes|Dm);
			switch(phy->speed){
			case 100:
				cfg |= Ps|Fes;
				break;
			case 10:
				cfg |= Ps;
				break;
			}
			if(phy->fd)
				cfg |= Dm;
			wr(ctlr, Cfg, cfg);
			iunlock(&ctlr->reglock);
			ethersetspeed(edev, phy->speed);
		}
		ethersetlink(edev, link);
	}
}

static void
setea(Ctlr *ctlr, uchar *ea)
{
	wr(ctlr, Addrhi0, ea[5]<<8 | ea[4]);
	wr(ctlr, Addrlo0, ea[3]<<24 | ea[2]<<16 | ea[1]<<8 | ea[0]);
	wr(ctlr, Addrhi0, Ae | ea[5]<<8 | ea[4]);
}

static void
shutdown(Ether *edev)
{
	Ctlr *ctlr = edev->ctlr;
	int i;

	wr(ctlr, Dmainten, 0);
	wr(ctlr, Dmatxctl, rr(ctlr, Dmatxctl) & ~Dmastart);
	wr(ctlr, Dmarxctl, rr(ctlr, Dmarxctl) & ~Dmastart);
	wr(ctlr, Cfg, rr(ctlr, Cfg) & ~(Te|Re));
	delay(10);
	wr(ctlr, Dmamode, Swr);
	for(i = 0; i < 1000 && (rr(ctlr, Dmamode) & Swr) != 0; i++)
		delay(1);
	if(i == 1000)
		print("#l%d: dwmac: dma reset timed out (no phy clock?)\n", edev->ctlrno);
	wr(ctlr, Dmasts, ~0);
	setea(ctlr, edev->ea);	/* for a rebooted kernel */
}

static void
attach(Ether *edev)
{
	Ctlr *ctlr = edev->ctlr;
	Block *b;
	Descr *d;
	int i;

	eqlock(ctlr);
	if(ctlr->attached){
		qunlock(ctlr);
		return;
	}
	if(waserror()){
		qunlock(ctlr);
		nexterror();
	}

	shutdown(edev);

	if(mii(ctlr->mii, ~0) == 0 || ctlr->mii->curphy == nil)
		error("no phy");
	addmiibus(ctlr->mii);
	print("#l%d: phy%d id %.8ux oui %x\n",
		edev->ctlrno, ctlr->mii->curphy->phyno,
		ctlr->mii->curphy->id, ctlr->mii->curphy->oui);
	ytconfig(ctlr);

	wr(ctlr, Dmasysbus, Eame);
	wr(ctlr, Dmactl, 0);		/* descriptors contiguous, no pbl×8 */

	if(ctlr->pool.size == 0){
		ctlr->pool.size = Rbsz;
		ctlr->pool.align = BLOCKALIGN;
		growbp(&ctlr->pool, 4*Nrd);
	}
	if(ctlr->rx->d == nil)
		ctlr->rx->d = mallocalign(sizeof(Descr) * Nrd, 64, 0, 0);
	if(ctlr->tx->d == nil)
		ctlr->tx->d = mallocalign(sizeof(Descr) * Ntd, 64, 0, 0);
	if(ctlr->rx->d == nil || ctlr->tx->d == nil)
		error(Enomem);

	for(i = 0; i < Nrd; i++){
		b = ctlr->rx->b[i];
		if(b == nil){
			if((b = iallocbp(&ctlr->pool)) == nil)
				error("out of rx buffers");
			ctlr->rx->b[i] = b;
		}
		if(i < Nrd-1)
			rxgive(&ctlr->rx->d[i], b);
		else
			ctlr->rx->d[i].sts = 0;	/* kept back, rxproc */
	}
	ctlr->tx->i = ctlr->tx->f = 0;
	for(i = 0; i < Ntd; i++){
		ctlr->tx->b[i] = nil;
		d = &ctlr->tx->d[i];
		d->addr = d->addrhi = d->ctl = d->sts = 0;
	}
	dmaflush(1, ctlr->rx->d, sizeof(Descr) * Nrd);
	dmaflush(1, ctlr->tx->d, sizeof(Descr) * Ntd);

	wr(ctlr, Txbasehi, (uvlong)PADDR(ctlr->tx->d) >> 32);
	wr(ctlr, Txbase, PADDR(ctlr->tx->d));
	wr(ctlr, Txlen, Ntd-1);
	wr(ctlr, Txtail, PADDR(ctlr->tx->d));
	wr(ctlr, Rxbasehi, (uvlong)PADDR(ctlr->rx->d) >> 32);
	wr(ctlr, Rxbase, PADDR(ctlr->rx->d));
	wr(ctlr, Rxlen, Nrd-1);
	wr(ctlr, Rxtail, PADDR(&ctlr->rx->d[Nrd-1]));

	wr(ctlr, Txq0op, Tqs | Txqen | Tsf);
	wr(ctlr, Rxq0op, Rqs | Rsf);
	wr(ctlr, Rxq0ctl0, Rxq0en);

	setea(ctlr, edev->ea);
	wr(ctlr, Pktfilt, Pm);
	wr(ctlr, Hashlo, 0);
	wr(ctlr, Hashhi, 0);
	wr(ctlr, Intenab, 0);		/* no mac interrupts */
	wr(ctlr, Mmcctl, 1<<0);		/* reset the counters */

	wr(ctlr, Dmatxctl, Pbl<<Pblshift | Dmastart);
	wr(ctlr, Dmarxctl, Pbl<<Pblshift | Rbsz<<Rbszshift | Dmastart);
	wr(ctlr, Dmainten, Nie | Aie | Fbe | Rbu | Ri | Ti);
	wr(ctlr, Cfg, Cst | Acs | Dm | Te | Re);

	ctlr->attached = 1;

	kproc("ether-rx", rxproc, edev);
	kproc("ether-tx", txproc, edev);
	kproc("ether-fr", frproc, edev);
	kproc("ether-link", linkproc, edev);

	qunlock(ctlr);
	poperror();
}

static void
prom(void *arg, int on)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;

	ilock(&ctlr->reglock);
	if(on)
		wr(ctlr, Pktfilt, rr(ctlr, Pktfilt) | Pr);
	else
		wr(ctlr, Pktfilt, rr(ctlr, Pktfilt) & ~Pr);
	iunlock(&ctlr->reglock);
}

/* all multicast pass (Pm) */
static void
multi(void*, uchar*, int)
{
}

static long
ctl(Ether*, void*, long len)
{
	return len;
}

static char*
ifstat(void *arg, char *p, char *e)
{
	Ether *edev = arg;
	Ctlr *ctlr = edev->ctlr;

	if(p >= e)
		return p;
	return seprint(p, e, "version: %#ux\ncfg: %#ux\ndma status: %#ux\n",
		rr(ctlr, Version), rr(ctlr, Cfg), rr(ctlr, Dmasts));
}

static int
pnp(Ether *edev)
{
	static uchar zea[Eaddrlen];
	Ctlr *ctlr;
	u32int hi, lo;
	int i;

	if(nctlr == 0)
		for(i = 0; i < nelem(soc.ether) && soc.ether[i] != 0; i++){
			ctlr = malloc(sizeof *ctlr);
			if(ctlr == nil)
				break;
			ctlr->regs = vmap(soc.ether[i], 0x2000);
			if(ctlr->regs == nil){
				free(ctlr);
				break;
			}
			ctlr->physreg = soc.ether[i];
			ctlr->irq = soc.etherirq[i];
			ctlr->ctlrno = i;
			ctlrtab[nctlr++] = ctlr;
		}
	ctlr = nil;
	for(i = 0; i < nctlr; i++){
		ctlr = ctlrtab[i];
		if(ctlr->active)
			continue;
		if(edev->port == 0 || edev->port == ctlr->physreg)
			break;
	}
	if(i >= nctlr)
		return -1;
	ctlr->active = 1;

	/* the mac address u-boot left, unless plan9.ini gives one */
	if(memcmp(edev->ea, zea, Eaddrlen) == 0){
		hi = rr(ctlr, Addrhi0);
		lo = rr(ctlr, Addrlo0);
		edev->ea[0] = lo;
		edev->ea[1] = lo>>8;
		edev->ea[2] = lo>>16;
		edev->ea[3] = lo>>24;
		edev->ea[4] = hi;
		edev->ea[5] = hi>>8;
	}
	if(memcmp(edev->ea, zea, Eaddrlen) == 0 || (edev->ea[0] & 1) != 0)
		print("#l%d: dwmac: no mac address (plan9.ini ether%d=ea=)\n",
			edev->ctlrno, edev->ctlrno);

	/* the mdio clock range u-boot chose, else the slowest (csr 250-300MHz) */
	ctlr->cr = rr(ctlr, Mdioaddr) & Mdiocrmask;
	if(ctlr->cr == 0)
		ctlr->cr = 5<<Mdiocr;
	ctlr->mii->name = edev->name;
	ctlr->mii->ctlr = ctlr;
	ctlr->mii->mir = mdior;
	ctlr->mii->miw = mdiow;

	edev->ctlr = ctlr;
	edev->port = ctlr->physreg;
	edev->irq = ctlr->irq;
	edev->tbdf = BUSUNKNOWN;
	edev->mbps = 1000;
	edev->maxmtu = Maxtu;
	edev->arg = edev;
	edev->attach = attach;
	edev->shutdown = shutdown;
	edev->ifstat = ifstat;
	edev->promiscuous = prom;
	edev->multicast = multi;
	edev->ctl = ctl;

	intrenable(edev->irq, interrupt, edev, edev->tbdf, edev->name);
	return 0;
}

void
etherdwmaclink(void)
{
	addethercard("dwmac", pnp);
}
