/*
 * Cadence Gigabit Ethernet MAC (GEM, Linux's cdns,macb): PolarFire
 * SoC (qemu's microchip-icicle-kit) and SiFive FU540/FU740. Ported from
 * Geoff Collyer's ethergem.c (9k, riscv): 9front's Ether and Bpool, the
 * controllers where the machine has them (soc.ether, socpf.c).
 * 32-bit descriptors, so the buffers must be below 4GB (low memory).
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

#define MASK(w)		((1u<<(w))-1)
#define HOWMANY(x, y)	(((x)+((y)-1))/(y))
#define NEXT(x, l)	(((x)+1)%(l))

typedef struct Ctlr Ctlr;
typedef struct Rd Rd;
typedef uvlong Macaddr;

enum {
	Rbsz	= (ETHERMAXTU+32+63) & ~63,	/* manual recommends 1600 */
	Descalign= 8,

	Ntd	= 128,		/* power of 2, >= 64 */
	Nrd	= 128,		/* power of 2, >= 64 */
	Nrb	= 1024,		/* receive buffers in the pool */
};

enum {				/* registers, by u32int* index */
	Ctl	= 0,
	Cfg	= 4/4,
	Sts	= 8/4,
	Dmacfg	= 0x10/4,
	Txsts	= 0x14/4,
	Rxdescq	= 0x18/4,	/* write: desc list base; read: current desc */
	Txdescq	= 0x1c/4,	/* write: desc list base; read: current desc */
	Rxsts	= 0x20/4,
	Intsts	= 0x24/4,
	Intena	= 0x28/4,	/* write only; same bits as Intsts */
	Intdis	= 0x2c/4,	/* write only; same bits as Intsts */
	Phymgmt	= 0x34/4,
	Hashbot	= 0x80/4,	/* low bits of multicast hash filter */
	Hashtop	= 0x84/4,
	Specaddr1bot	= 0x88/4,	/* our mac address */
	Specaddr1top	= 0x8c/4,
	Laddr2l		= 0x90/4,	/* zero all ... */
	Match4off	= 0xb4/4,	/* ... of these */
	Pcsctl	= 0x200/4,
	Pcssts	= 0x204/4,
	Txdescqhi = 0x4c8/4,
	Rxdescqhi = 0x4d4/4,
};

enum {				/* Ctl bits */
	Txhalt	= 1<<10,	/* stop output dma (wo) */
	Txstart	= 1<<9,		/* resume output dma (wo) */
	Mden	= 1<<4,		/* mgmt port enable */
	Txallow	= 1<<3,		/* also sets tx q ptr to start of tx desc list */
	Rxallow	= 1<<2,
};

enum {				/* Cfg bits */
	Sgmii		= 1<<27,	/* enable gb mii, thus gb speed */
	Rxckoffl	= 1<<24,	/* rx checksum offload: drop bad pkts */
	Mdcclkdivshft	= 18,
	Mdcclkdiv	= 7<<18,	/* default 2; 4 for Gb/s */
	Rxomitfcs	= 1<<17,	/* rx omit crc in memory */
	Pcsmand		= 1<<11,	/* not TBI */
	Gbmode		= 1<<10,	/* gigabit mode enable */
	Rxunihashfilt	= 1<<7,
	Rxmultihashfilt	= 1<<6,
	Rxnobcast	= 1<<5,		/* discard broadcasts */
	Caf		= 1<<4,		/* copy all frames (promiscuous mode) */
	Fd		= 1<<1,		/* full duplex */
};

enum {				/* Dmacfg bits */
	Dmaaddr64	= 1<<30,	/* use 64-bit dma addresses */
	Txextbd		= 1<<29,	/* extended tx buffer descriptors */
	Rxextbd		= 1<<28,	/* extended rx buffer descriptors */
	Rxbsshft	= 16,
	Rxbufsize	= MASK(8)<<Rxbsshft,	/* rx buffer size / 64 */
	Blen		= MASK(5),	/* dma burst len; def 4 (incr4) */
};

enum {				/* Txsts bits, cleared by writing 1 */
	Txdone		= 1<<5,
	Txusedbitread	= 1<<0,	/* tx buf desc read with its used bit set */
};

enum {				/* Rxsts bits, cleared by writing 1 */
	Rxoverrun	= 1<<2,
	Rxrcvd		= 1<<1,
	Rxbufnotavail	= 1<<0,
	Rxdonebits	= Rxoverrun | Rxbufnotavail | Rxrcvd,
};

enum {				/* Int* bits, cleared by writing 1 */
	Linkchange	= 1<<9,
	Txdoneint	= 1<<7,
	Rxrcvdint	= 1<<1,
};

enum {				/* Pcsctl bits */
	Pcsenan		= 1<<12,	/* enable auto negotiation */
};

enum {				/* Pcssts bits */
	Andone		= 1<<5,		/* auto. neg. done */
	Linksts		= 1<<2,		/* link status */
};

/* a register layout, which can't tolerate bogus padding */
struct Rd {			/* Receive Descriptor */
	u32int	addr;		/* low physical */
	u32int	size_flags;
};

enum {				/* Rd->addr bits (other than rcv buffer addr) */
	Rdlast	= 1<<1,		/* last Rd */
	Full	= 1<<0,		/* pkt recv'd; setting to 0 allows input */
};
enum {				/* Rd->size_flags bits */
	Rdpktsize= MASK(13),
};

#define Td Rd			/* Transmit Descriptor; Td->addr is a byte address */

enum {				/* Td->size_flags bits */
	Sent	= 1u<<31,	/* tx done; setting to 0 enables output */
	Tdlast	= 1<<30,	/* last Td */
	Tdeofr	= 1<<15,	/* buffer contains end of frame (packet) */
};

struct Ctlr {
	u32int	*regs;		/* memory-mapped device registers */
	uintptr	physreg;	/* their physical address */
	int	irq;
	Ether	*edev;

	Lock	reglock;
	uint	im;		/* interrupt mask (enables) */
	uint	lim;
	uint	rim;
	uint	tim;

	Rendez	rrendez;
	Rd	*rdba;		/* receive descriptor base address */
	uint	rdh;		/* " " head */
	uint	rdt;		/* " " tail */
	Block	**rb;		/* " buffers */
	uint	rintr;
	uint	rsleep;
	int	rdfree;		/* " descriptors awaiting packets */
	Bpool	pool[1];	/* receive buffers */

	Rendez	trendez;
	Td	*tdba;		/* transmit descriptor base address */
	uint	tdh;		/* " " head */
	uint	tdt;		/* " " tail */
	Block	**tb;		/* " buffers */
	uint	tintr;
	uint	tsleep;
	QLock	tlock;
	ulong	txtick;		/* tick at start of last tx start */

	Rendez	lrendez;

	int	active;
	int	procsrunning;	/* kprocs started for this Ctlr? */
	QLock	alock;		/* attach lock */
	int	attached;

	uchar	ra[Eaddrlen];	/* receive address */
	Macaddr	macregs;	/* saved from hardware if set */
};

static	Ctlr	*ctlrtab[nelem(soc.ether)];
static	int	nctlr;

static void	macremember(Ctlr *ctlr);
static void	replenish(Ctlr *ctlr);
static void	setmacs(Ctlr *ctlr);
static void	transmit(Ether *edev);

static char*
ifstat(void *a, char *p, char *e)
{
	Ether *edev;
	Ctlr *ctlr;

	if(p >= e)	/* netif's stats: only to update counters */
		return p;
	edev = a;
	ctlr = edev->ctlr;
	p = seprint(p, e, "ctl %#ux\n",  ctlr->regs[Ctl]);
	p = seprint(p, e, "cfg %#ux\n", ctlr->regs[Cfg]);
	p = seprint(p, e, "dmacfg %#ux\n", ctlr->regs[Dmacfg]);
	p = seprint(p, e, "pcssts %#ux\n", ctlr->regs[Pcssts]);
	p = seprint(p, e, "rintr %d rsleep %d\n", ctlr->rintr, ctlr->rsleep);
	return seprint(p, e, "tintr %d tsleep %d\n", ctlr->tintr, ctlr->tsleep);
}

static void
ienablelcked(Ctlr *ctlr, int ie)
{
	ctlr->im |= ie;
	ctlr->regs[Intena] = ctlr->im;
}

static void
ienable(Ctlr *ctlr, int ie)
{
	ilock(&ctlr->reglock);
	ienablelcked(ctlr, ie);
	iunlock(&ctlr->reglock);
}

static int
lim(void *v)
{
	return ((Ctlr*)v)->lim != 0;
}

static void
lproc(void *v)
{
	Ctlr *ctlr;
	Ether *edev;

	edev = v;
	ctlr = edev->ctlr;
	for (;;) {
		ienable(ctlr, Linkchange);
		sleep(&ctlr->lrendez, lim, ctlr);
		ctlr->lim = 0;
		if (ctlr->regs[Pcssts] & Andone)
			ethersetlink(edev, (ctlr->regs[Pcssts] & Linksts) != 0);
	}
}

/* reclaim sent buffers & descriptors */
static void
cleanup(Ctlr *ctlr)
{
	uint i, n, tdh;
	Block *bp;
	Td *td;

	tdh = ctlr->tdh;
	i = 0;
	n = NEXT(tdh, Ntd);
	td = &ctlr->tdba[n];
	dmaflush(0, td, sizeof *td);
	while (td = &ctlr->tdba[n], td->size_flags & Sent && n != ctlr->tdt &&
	    i++ < Ntd-2) {
		tdh = n;
		bp = ctlr->tb[tdh];
		ctlr->tb[tdh] = nil;
		if (bp)
			freeb(bp);
		/* clearing Sent here would allow this (non-existent) buffer to be sent */
		n = NEXT(tdh, Ntd);
		dmaflush(0, td, sizeof *td);
	}
	ctlr->tdh = tdh;
}

static void
transmit(Ether *edev)
{
	Block *bp;
	Ctlr *ctlr;
	Td *td;
	uint nqd, tdt, tdh;

	ctlr = edev->ctlr;
	if(!canqlock(&ctlr->tlock)){
		ienable(ctlr, Txdoneint);
		return;
	}
	if (!ctlr->attached) {
		qunlock(&ctlr->tlock);
		return;
	}
	cleanup(ctlr);				/* free transmitted buffers */
	tdh = ctlr->tdh;
	tdt = ctlr->tdt;
	for(nqd = 0; NEXT(tdt, Ntd) != tdh; nqd++){	/* ring not full? */
		if((bp = qget(edev->oq)) == nil)
			break;

		dmaflush(1, bp->rp, BLEN(bp));
		td = &ctlr->tdba[tdt];
		td->addr = PCIWADDR(bp->rp);
		/* clears Sent in size_flags, allows xmit */
		td->size_flags = (td->size_flags & Tdlast) | Tdeofr | BLEN(bp);
		dmaflush(1, td, sizeof *td);

		if (ctlr->tb[tdt])
			panic("ethergem: %#p: xmit q full", ctlr->physreg);
		ctlr->tb[tdt] = bp;

		tdt = NEXT(tdt, Ntd);
	}

	ilock(&ctlr->reglock);
	if(nqd) {
		ctlr->tdt = tdt;
		ctlr->regs[Txsts] |= Txdone|Txusedbitread;	/* reset bits */
		coherence();
		ctlr->txtick = MACHP(0)->ticks;
	}
	ctlr->regs[Ctl] |= Txstart;		/* kick transmitter */
	ienablelcked(ctlr, Txdoneint);
	iunlock(&ctlr->reglock);
	qunlock(&ctlr->tlock);
}

static int
tim(void *vc)
{
	Ctlr *ctlr = (void *)vc;

	if(ctlr->tim != 0)
		return 1;
	if(qlen(ctlr->edev->oq) == 0)
		return 0;
	dmaflush(0, &ctlr->tdba[ctlr->tdt], sizeof(Td));
	return ctlr->tdba[ctlr->tdt].size_flags & Sent;
}

static void
tproc(void *v)
{
	ulong txtick;
	Ctlr *ctlr;
	Ether *edev;

	edev = v;
	ctlr = edev->ctlr;
	for (;;) {
		ctlr->tsleep++;
		/* the time-out is only for diagnosis */
		tsleep(&ctlr->trendez, tim, ctlr, 1000);
		ctlr->tim = 0;
		txtick = ctlr->txtick;
		if (txtick && MACHP(0)->ticks - txtick > 2*HZ) {
			print("ethergem %#p: not transmitting\n", ctlr->physreg);
			ctlr->txtick = 0;
		}
		/* sent buffers' descriptors can take more of edev->oq */
		transmit(edev);
	}
}

static void
initrxring(Ctlr *ctlr, Rd *rdbase, int ndescs)
{
	int i;
	Rd *rd;

	ctlr->rdba = rdbase;
	memset(ctlr->rdba, 0, ndescs * sizeof(Rd));
	for (i = 0; i < ndescs; i++) {
		rd = &ctlr->rdba[i];
		rd->addr = Full;	/* prevent rcv filling for now */
		rd->size_flags = Rbsz;
	}
	ctlr->rdba[ndescs - 1].addr |= Rdlast;
	dmaflush(1, ctlr->rdba, ndescs * sizeof(Rd));
}

static void
inittxring(Ctlr *ctlr, Td *tdbase, int ndescs)
{
	int i;
	Td *td;

	ctlr->tdba = tdbase;
	memset(ctlr->tdba, 0, ndescs * sizeof(Td));
	for (i = 0; i < ndescs; i++) {
		td = &ctlr->tdba[i];
		td->size_flags = Sent;		/* available to fill to send */
	}
	ctlr->tdba[ndescs - 1].size_flags |= Tdlast;
	dmaflush(1, ctlr->tdba, ndescs * sizeof(Td));
}

static void
rxon(Ctlr *ctlr)
{
	ilock(&ctlr->reglock);
	ctlr->regs[Ctl] |= Rxallow;
	coherence();
	ienablelcked(ctlr, Rxrcvdint);
	iunlock(&ctlr->reglock);
}

/*
 * Get the receiver into a usable state, with gigabit sgmii and the
 * pcs's auto negotiation (the icicle's vsc8662 phys); no checksum
 * offload. We're called before txinit.
 */
static void
rxinit(Ctlr *ctlr)
{
	u32int *regs;
	ulong cfg;
	uvlong physbda;

	regs = ctlr->regs;
	macremember(ctlr);
	ilock(&ctlr->reglock);
	regs[Ctl] &= ~Rxallow;		/* allow state is unknown: clear it */
	coherence();
	iunlock(&ctlr->reglock);
	delay(10);
	ctlr->rdfree = 0;

	ilock(&ctlr->reglock);
	regs[Pcsctl] |= Pcsenan;
	cfg = regs[Cfg];
	cfg &= ~(Rxnobcast|Rxckoffl|Mdcclkdiv);
	cfg |= Pcsmand | Rxomitfcs | Fd | Gbmode | Sgmii |
		Rxunihashfilt | Rxmultihashfilt | 4<<Mdcclkdivshft;
	regs[Cfg] = cfg;

	cfg = regs[Dmacfg];
	cfg &= ~(Rxbufsize|Blen);
	cfg |= HOWMANY(Rbsz, 64) << Rxbsshft | 4;	/* 4 = incr16 */
	cfg &= ~(Dmaaddr64 | Txextbd | Rxextbd);
	regs[Dmacfg] = cfg;

	regs[Hashtop] = regs[Hashbot] = ~0;	/* accept all for now */
	iunlock(&ctlr->reglock);

	/* set up the rcv ring */
	setmacs(ctlr);			/* in case sw reset cleared it */
	initrxring(ctlr, ctlr->rdba, Nrd);
	physbda = PADDR(ctlr->rdba);
	ilock(&ctlr->reglock);
	regs[Rxdescq] = physbda;
	regs[Rxdescqhi] = physbda >> 32;
	iunlock(&ctlr->reglock);
	replenish(ctlr);
	rxon(ctlr);
}

static void
replenish(Ctlr *ctlr)
{
	uint rdt, rdh;
	Block *bp;
	Block **rb;
	Rd *rd;

	rdh = ctlr->rdh;
	for(rdt = ctlr->rdt; NEXT(rdt, Nrd) != rdh; rdt = NEXT(rdt, Nrd)){
		rd = &ctlr->rdba[rdt];
		rb = &ctlr->rb[rdt];
		if(*rb != nil){
			iprint("ethergem %#p: rx overrun\n", ctlr->physreg);
			break;
		}
		*rb = bp = iallocbp(ctlr->pool);
		if(bp == nil)		/* no buffer for this desc */
			break;
		dmaflush(1, bp->rp, Rbsz);
		rd->size_flags = Rbsz;
		/* Full=0 allows reception */
		rd->addr = rd->addr & Rdlast | PCIWADDR(bp->rp);
		dmaflush(1, rd, sizeof *rd);
		ctlr->rdfree++;
	}
	ctlr->rdt = rdt;
}

static int
rim(void *vc)
{
	Ctlr *ctlr = (void *)vc;

	if(ctlr->rim != 0)
		return 1;
	dmaflush(0, &ctlr->rdba[ctlr->rdh], sizeof(Rd));
	return ctlr->rdba[ctlr->rdh].addr & Full;
}

static int
qinpkt(Ctlr *ctlr)
{
	int passed;
	uint rdh, len;
	Block *bp;
	Rd *rd;

	ctlr->rim = 0;
	rdh = ctlr->rdh;
	rd = &ctlr->rdba[rdh];
	dmaflush(0, rd, sizeof *rd);
	if (!(rd->addr & Full))
		return -1;		/* wait for pkts to arrive */

	/* accept packets with no errors */
	passed = 0;
	coherence();
	bp = ctlr->rb[rdh];
	if ((ctlr->regs[Rxsts] & Rxdonebits) == Rxrcvd){
		/* no overrun and buffer available */
		if (bp == nil) {
			print("ethergem %#p: nil Block* from ctlr->rb\n", ctlr->physreg);
			return -1;
		}
		len = rd->size_flags & Rdpktsize;
		if (len == ETHERMINTU)
			len += 4;	/* for 60-byte arps? */
		if (len <= ETHERMAXTU)
			bp->wp += len;
		dmaflush(0, bp->rp, len);
		etheriq(ctlr->edev, bp);	/* pass pkt upstream, freed there */
		passed++;
	} else {
		freeb(bp);			/* toss bad pkt */
		ilock(&ctlr->reglock);
		ctlr->regs[Rxsts] |= Rxdonebits; /* reset done bits */
		iunlock(&ctlr->reglock);
	}

	ctlr->rb[rdh] = nil;
	ctlr->rdh = NEXT(rdh, Nrd);
	ctlr->rdfree--;
	/* if few rds are ready for packets, set up the unready ones */
	if (ctlr->rdfree <= Nrd*3/4)
		replenish(ctlr);
	return passed;
}

static void
rproc(void *v)
{
	Ctlr *ctlr;
	Ether *edev;

	edev = v;
	ctlr = edev->ctlr;
	for (ctlr->rdh = 0; ; ) {
		replenish(ctlr);
		/*
		 * Prevent an idle or unplugged interface from interrupting.
		 * Allow receiver interrupts initially and again
		 * if the interface (and transmitter) see actual use.
		 */
		if (edev->outpackets > 10 || ctlr->rintr < 2*Nrd)
			ienable(ctlr, Rxrcvdint);
		ctlr->rsleep++;
		sleep(&ctlr->rrendez, rim, ctlr);
		while(qinpkt(ctlr) >= 0)
			;
	}
}

static void
promiscuous(void *a, int on)
{
	Ctlr *ctlr;
	Ether *edev;

	edev = a;
	ctlr = edev->ctlr;
	ilock(&ctlr->reglock);
	if(on)
		ctlr->regs[Cfg] |= Caf;
	else
		ctlr->regs[Cfg] &= ~Caf;
	iunlock(&ctlr->reglock);
}

/* see §16.2.3 of the Xilinx Zynq-7000 SoC Tech. Ref. Man. */
static ulong
mcasthash(uchar *mac)
{
	uint chr, bit, hash;

	hash = 0;
	for (chr = 0; chr < Eaddrlen; chr++)
		for (bit = chr; bit < 8*Eaddrlen; bit += 6)
			if (mac[bit>>3] & (1 << (bit & 7)))
				hash ^= 1 << chr;
	return hash;
}

static void
multicast(void *a, uchar *addr, int on)
{
	ulong hash;
	Ctlr *ctlr;
	Ether *edev;

	edev = a;
	ctlr = edev->ctlr;
	/*
	 * multiple ether addresses can hash to the same filter bit, so it's
	 * never safe to clear one without knowing all the addresses in use.
	 */
	hash = mcasthash(addr);
	ilock(&ctlr->reglock);
	if(on)
		ctlr->regs[Hashbot + (hash/32)%2] |= 1u << hash%32;
	iunlock(&ctlr->reglock);
}

#define GETMACREGS(regs) \
	(((uvlong)(regs)[Specaddr1top] & MASK(16))<<32 | (regs)[Specaddr1bot])

static int
ismacset(Macaddr a)
{
	return a != 0 && a != 0xFFFFFFFFFFFFULL;
}

/* if hw mac addr regs set, remember for later */
static void
macremember(Ctlr *ctlr)
{
	Macaddr addr;

	addr = GETMACREGS(ctlr->regs);
	if (ismacset(addr) && !ismacset(ctlr->macregs))
		ctlr->macregs = addr;
}

/* if remembered mac is set, set hw regs to it */
static void
setmacregs(Ctlr *ctlr)
{
	u32int *regs;
	Macaddr addr;

	addr = ctlr->macregs;
	regs = ctlr->regs;
	if (ismacset(addr) && !ismacset(GETMACREGS(regs))) {
		regs[Specaddr1bot] = addr;
		regs[Specaddr1top] = addr>>32;
	}
}

/* don't discard all state; we may be attached again */
static void
detach(Ctlr *ctlr)
{
	u32int *regs;

	regs = ctlr->regs;
	macremember(ctlr);
	ilock(&ctlr->reglock);
	regs[Intdis] = ~0;
	regs[Ctl] |= Txhalt;
	coherence();
	regs[Ctl] &= ~(Txallow | Rxallow);
	iunlock(&ctlr->reglock);
	delay(10);
	ctlr->attached = 0;

	/* ensure mac addr persists across reboot, in case a reset cleared it */
	setmacregs(ctlr);
}

static void
shutdown(Ether *edev)
{
	detach(edev->ctlr);
}

/*
 * u-boot (or qemu, or the previous kernel) should have left the mac
 * address in the mac address registers. If not, take the one plan9.ini
 * gives (ether0=ea=...), else make a locally administered one.
 */
static void
setmacs(Ctlr *ctlr)
{
	int i;
	Macaddr a;
	u32int *regs;
	uchar *ra;

	regs = ctlr->regs;
	ra = ctlr->ra;
	ilock(&ctlr->reglock);
	a = GETMACREGS(regs);
	if (!ismacset(a)) {
		setmacregs(ctlr);
		a = GETMACREGS(regs);
	}
	if (ismacset(a))
		for(i = 0; i < Eaddrlen; i++)
			ra[i] = a >> 8*i;
	else {
		if(ctlr->edev != nil && memcmp(ctlr->edev->ea, ra, Eaddrlen) != 0)
			memmove(ra, ctlr->edev->ea, Eaddrlen);
		if(memcmp(ra, "\0\0\0\0\0\0", Eaddrlen) == 0){
			ra[0] = 0x02;	/* locally administered */
			ra[5] = ctlr->physreg>>12;
			ra[4] = ctlr->physreg>>20;
		}
		regs[Specaddr1top] = ra[5]<<8  | ra[4];
		regs[Specaddr1bot] = ra[3]<<24 | ra[2]<<16 | ra[1]<<8 | ra[0];
	}
	macremember(ctlr);
	for (i = Laddr2l; i <= Match4off; i++)
		regs[i] = 0;
	iunlock(&ctlr->reglock);
	if (ctlr->edev)
		multicast(ctlr->edev, ra, 1);
}

static void
reset(Ctlr *ctlr)
{
	detach(ctlr);
	setmacs(ctlr);
}

/* get the transmitter into a usable state */
static void
txinit(Ctlr *ctlr)
{
	u32int *regs;
	uvlong physbda;

	regs = ctlr->regs;
	ilock(&ctlr->reglock);
	regs[Ctl] |= Txhalt;
	coherence();
	regs[Ctl] &= ~Txallow;
	iunlock(&ctlr->reglock);
	delay(10);

	/* set up tx queue 0 ring */
	inittxring(ctlr, ctlr->tdba, Ntd);
	ctlr->tdh = Ntd - 1;
	physbda = PADDR(ctlr->tdba);
	ilock(&ctlr->reglock);
	regs[Txdescq] = physbda;
	regs[Txdescqhi] = physbda >> 32;
	coherence();
	regs[Ctl] |= Txallow; /* don't set Txstart until there's a pkt to send */
	iunlock(&ctlr->reglock);
}

static void
allocall(Ctlr *ctlr)
{
	if (ctlr->rdba == nil)
		ctlr->rdba = mallocalign(Nrd * sizeof(Rd), Descalign, 0, 0);
	if (ctlr->tdba == nil)
		ctlr->tdba = mallocalign(Ntd * sizeof(Td), Descalign, 0, 0);
	if (ctlr->rb == nil)
		ctlr->rb = malloc(Nrd * sizeof(Block *));
	if (ctlr->tb == nil)
		ctlr->tb = malloc(Ntd * sizeof(Block *));
	if (ctlr->rdba == nil || ctlr->tdba == nil || ctlr->rb == nil || ctlr->tb == nil)
		error(Enomem);
	if (ctlr->pool->size == 0) {
		ctlr->pool->size = Rbsz;
		ctlr->pool->align = BLOCKALIGN;
		growbp(ctlr->pool, Nrb);
	}
}

static void
etherkproc(Ether *edev, void (*kp)(void *), char *sfx)
{
	char buf[KNAMELEN];

	snprint(buf, sizeof buf, "#l%d%s", edev->ctlrno, sfx);
	kproc(buf, kp, edev);
}

static void
startkprocs(Ctlr *ctlr)
{
	Ether *edev;

	if (ctlr->procsrunning)
		return;
	edev = ctlr->edev;
	etherkproc(edev, lproc, "link");
	ienable(ctlr, Linkchange);
	etherkproc(edev, tproc, "xmit");
	etherkproc(edev, rproc, "rcv");
	ctlr->procsrunning = 1;
}

/* could be attaching again after a detach */
static void
attach(Ether *edev)
{
	Ctlr *ctlr;

	ctlr = edev->ctlr;
	qlock(&ctlr->alock);
	if (ctlr->attached) {
		qunlock(&ctlr->alock);
		return;
	}
	if(waserror()){
		reset(ctlr);
		qunlock(&ctlr->alock);
		nexterror();
	}
	allocall(ctlr);
	rxinit(ctlr);
	txinit(ctlr);
	startkprocs(ctlr);
	ctlr->attached = 1;
	poperror();
	qunlock(&ctlr->alock);
}

static void
interrupt(Ureg*, void *v)
{
	int icr, im, waker, waket, wakel;
	u32int *regs;
	Ctlr *ctlr;

	ctlr = ((Ether *)v)->ctlr;
	regs = ctlr->regs;
	ilock(&ctlr->reglock);
	icr = regs[Intsts];
	regs[Intdis] = ~0;		/* disable all my intrs */
	coherence();
	im = ctlr->im;
	icr &= im;

	waker = waket = wakel = 0;
	if(icr & Rxrcvdint){
		ctlr->rim = Rxrcvdint;
		regs[Intsts] |= Rxrcvdint;	/* reset */
		waker = 1;
		im &= ~Rxrcvdint;
		ctlr->rintr++;
	}
	if(icr & Txdoneint){
		ctlr->tim = Txdoneint;
		regs[Intsts] |= Txdoneint;	/* reset */
		ctlr->txtick = 0;
		waket = 1;
		im &= ~Txdoneint;
		ctlr->tintr++;
	}
	if(icr & Linkchange){
		ctlr->lim = Linkchange;
		regs[Intsts] |= Linkchange;	/* reset */
		wakel = 1;
		/* we always want to see link changes */
	}

	/* enable only intr sources we didn't service and are interested in */
	regs[Intena] = ctlr->im = im;
	iunlock(&ctlr->reglock);

	if(waker)
		wakeup(&ctlr->rrendez);
	if(waket)
		wakeup(&ctlr->trendez);
	if(wakel)
		wakeup(&ctlr->lrendez);
}

/* map the controller at physical address pa */
static Ctlr*
newctlr(uintptr pa, int irq)
{
	Ctlr *ctlr;

	ctlr = malloc(sizeof *ctlr);
	if(ctlr == nil)
		return nil;
	ctlr->regs = vmap(pa, 2*BY2PG);
	if(ctlr->regs == nil){
		print("ethergem: can't map regs %#p\n", pa);
		free(ctlr);
		return nil;
	}
	ctlr->physreg = pa;
	ctlr->irq = irq;
	reset(ctlr);
	ctlrtab[nctlr++] = ctlr;
	return ctlr;
}

static int
pnp(Ether *edev)
{
	int i;
	Ctlr *ctlr;

	if(nctlr == 0)
		for(i = 0; i < nelem(soc.ether) && soc.ether[i] != 0; i++)
			newctlr(soc.ether[i], soc.etherirq[i]);
	ctlr = nil;
	for(i = 0; i < nctlr; i++){
		ctlr = ctlrtab[i];
		if(ctlr->active)
			continue;
		if(edev->port == 0 || edev->port == ctlr->physreg)
			break;
	}
	if (i >= nctlr)
		return -1;
	ctlr->active = 1;

	ctlr->edev = edev;
	edev->ctlr = ctlr;
	edev->port = ctlr->physreg;
	edev->irq = ctlr->irq;
	edev->tbdf = BUSUNKNOWN;
	edev->mbps = 1000;
	edev->link = 1;
	memmove(edev->ea, ctlr->ra, Eaddrlen);

	edev->arg = edev;
	edev->attach = attach;
	edev->transmit = transmit;
	edev->shutdown = shutdown;
	edev->ifstat = ifstat;
	edev->multicast = multicast;
	edev->promiscuous = promiscuous;
	intrenable(edev->irq, interrupt, edev, edev->tbdf, edev->name);
	return 0;
}

void
ethergemlink(void)
{
	addethercard("gem", pnp);
}
