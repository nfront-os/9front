/*
 * DesignWare mobile storage host (dw_mmc, "synopsys dwc sdio"):
 * the VisionFive 2's sd card slot and emmc socket (JH7110 mmc1, mmc0).
 * The controllers where the machine has them (soc.dwmmc, soc.dwmmcirq,
 * the card clock soc.dwmmcclk; plan9.ini *dwmmcclk= overrides); the
 * registers as in Geoff Collyer's sddwc.c (9k), which did not get
 * past CMD8: the first command must send the 80 initialisation
 * clocks (Sendinit). Programmed i/o: the interrupt moves the data
 * through the fifo (the internal dma's descriptors hold 32-bit
 * addresses, the ram goes beyond 4GB).
 * No card detect (the board's is a gpio): a card is assumed.
 */
#include "u.h"
#include "../port/lib.h"
#include "../port/error.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/sd.h"

enum {					/* registers, by u32int index */
	Ctrl	= 0x00/4,
	Pwren	= 0x04/4,
	Clkdiv	= 0x08/4,
	Clksrc	= 0x0C/4,
	Clkena	= 0x10/4,
	Tmout	= 0x14/4,
	Ctype	= 0x18/4,
	Blksiz	= 0x1C/4,
	Bytcnt	= 0x20/4,
	Intmask	= 0x24/4,
	Cmdarg	= 0x28/4,
	Cmd	= 0x2C/4,
	Resp0	= 0x30/4,
	Resp1	= 0x34/4,
	Resp2	= 0x38/4,
	Resp3	= 0x3C/4,
	Mintsts	= 0x40/4,
	Rintsts	= 0x44/4,
	Status	= 0x48/4,
	Fifoth	= 0x4C/4,
	Verid	= 0x6C/4,
	Hcon	= 0x70/4,
	Uhsreg	= 0x74/4,
	Rstn	= 0x78/4,
	Bmod	= 0x80/4,
};

enum {
	/* Ctrl */
	Reset		= 1<<0,
	Fiforeset	= 1<<1,
	Dmareset	= 1<<2,
	Intenable	= 1<<4,
	Dmaenable	= 1<<5,
	Idmacenable	= 1<<25,

	/* Cmd */
	Start		= 1<<31,
	Holdreg		= 1<<29,
	Updclk		= 1<<21,
	Sendinit	= 1<<15,
	Stopabort	= 1<<14,
	Prvdatwait	= 1<<13,
	Wr		= 1<<10,
	Dataexp		= 1<<9,
	Respcrc		= 1<<8,
	Resplong	= 1<<7,
	Respexp		= 1<<6,

	/* Intmask, Mintsts, Rintsts */
	Ebe		= 1<<15,	/* end bit error */
	Sbe		= 1<<13,	/* start bit error */
	Hle		= 1<<12,	/* hardware locked write */
	Frun		= 1<<11,	/* fifo under/overrun */
	Hto		= 1<<10,	/* data starvation by host */
	Drto		= 1<<9,		/* data read timeout */
	Rto		= 1<<8,		/* response timeout */
	Dcrc		= 1<<7,
	Rcrc		= 1<<6,
	Rxdr		= 1<<5,		/* fifo above the receive watermark */
	Txdr		= 1<<4,		/* fifo below the transmit watermark */
	Dto		= 1<<3,		/* data transfer over */
	Cdone		= 1<<2,		/* command done */
	Re		= 1<<1,		/* response error */

	Cmderr		= Re|Rcrc|Rto|Hle,
	Dataerr		= Drto|Dcrc|Hto|Sbe|Ebe|Frun,

	/* Status */
	Busy		= 1<<9,		/* data[0] low: the card is busy */
	Fifoshift	= 17,		/* fifo count, in fifo words */
	Fifomask	= 0x1FFF,

	/* Ctype */
	Width4		= 1<<0,
	Width8		= 1<<16,

	/* Hcon */
	Hdatashift	= 7,		/* fifo width: 0 16, 1 32, 2 64 bits */
};

typedef struct Ctlr Ctlr;
struct Ctlr {
	Lock;
	u32int	*regs;
	int	irq;
	int	fifo;			/* the data register's index */
	int	fifow;			/* bytes per fifo word, 4 or 8 */
	int	fifodepth;		/* in fifo words */
	ulong	clk;			/* the card clock (ciu) */

	Rendez	r;
	u32int	sts;			/* interrupts seen since the command */
	u32int	waitmsk;

	/* the data of the command */
	int	write;
	uchar	*buf;
	int	len;			/* still to move */
	int	bsize;
	int	bcount;
};

static Ctlr dwmmcctlr[nelem(soc.dwmmc)];

#define rd(c, r)	(((volatile u32int*)(c)->regs)[r])
#define wr(c, r, v)	(((volatile u32int*)(c)->regs)[r] = (v))

/* the fifo: a word to or from the buffer, the last maybe short */
static void
fiforead(Ctlr *c)
{
	int n, k;
	u32int w[2];

	while(c->len > 0){
		n = (rd(c, Status) >> Fifoshift) & Fifomask;
		if(n == 0)
			break;
		for(; n > 0 && c->len > 0; n--){
			if(c->fifow == 8){
				w[0] = rd(c, c->fifo);
				w[1] = rd(c, c->fifo+1);
			}else
				w[0] = rd(c, c->fifo);
			k = c->fifow;
			if(k > c->len)
				k = c->len;
			memmove(c->buf, w, k);
			c->buf += k;
			c->len -= k;
		}
	}
}

static void
fifowrite(Ctlr *c)
{
	int n, k;
	u32int w[2];

	while(c->len > 0){
		n = c->fifodepth - ((rd(c, Status) >> Fifoshift) & Fifomask);
		if(n <= 0)
			break;
		for(; n > 0 && c->len > 0; n--){
			k = c->fifow;
			if(k > c->len)
				k = c->len;
			w[0] = w[1] = 0;
			memmove(w, c->buf, k);
			if(c->fifow == 8){
				wr(c, c->fifo, w[0]);
				wr(c, c->fifo+1, w[1]);
			}else
				wr(c, c->fifo, w[0]);
			c->buf += k;
			c->len -= k;
		}
	}
}

static void
dwmmcinterrupt(Ureg*, void *arg)
{
	Ctlr *c;
	u32int pend;

	c = arg;
	ilock(c);
	pend = rd(c, Mintsts);
	if(pend == 0){
		iunlock(c);
		return;
	}
	wr(c, Rintsts, pend);
	if(c->buf != nil){
		if(c->write){
			if(pend & Txdr)
				fifowrite(c);
		}else if(pend & (Rxdr|Dto))
			fiforead(c);
	}
	c->sts |= pend;
	if(c->sts & c->waitmsk)
		wakeup(&c->r);
	iunlock(c);
}

static int
waitcond(void *arg)
{
	Ctlr *c = arg;

	return (c->sts & c->waitmsk) != 0;
}

/* wait for one of mask (or an error of errs); the interrupts seen */
static u32int
intrwait(Ctlr *c, u32int mask, u32int errs, int tmo)
{
	u32int sts;

	ilock(c);
	c->waitmsk = mask | errs;
	iunlock(c);
	do {
		if(!waserror()){
			tsleep(&c->r, waitcond, c, 100);
			poperror();
		}
		dwmmcinterrupt(nil, c);	/* in case its interrupt is lost */
		if(waitcond(c))
			break;
		tmo -= 100;
	} while(tmo > 0);
	ilock(c);
	c->waitmsk = 0;
	sts = c->sts;
	iunlock(c);
	return sts;
}

static int
waitbits(Ctlr *c, int reg, u32int bits, int ms)
{
	while(rd(c, reg) & bits){
		if(ms-- <= 0)
			return -1;
		if(up != nil)
			tsleep(&up->sleep, return0, nil, 1);
		else
			delay(1);
	}
	return 0;
}

static void
ctlrreset(Ctlr *c, u32int bits)
{
	wr(c, Ctrl, rd(c, Ctrl) | bits);
	if(waitbits(c, Ctrl, bits, 500) < 0)
		print("dwmmc: reset %ux timed out\n", bits);
}

/* the card clock's new divisor (or gating) to the card */
static void
updateclock(Ctlr *c)
{
	wr(c, Cmd, Start | Updclk | Prvdatwait);
	if(waitbits(c, Cmd, Start, 500) < 0)
		print("dwmmc: clock update timed out\n");
}

static void
setclock(Ctlr *c, uint hz)
{
	uint div;

	if(hz >= c->clk)
		div = 0;	/* bypass */
	else
		div = (c->clk + 2*hz - 1) / (2*hz);
	waitbits(c, Status, Busy, 500);
	wr(c, Clkena, 0);
	updateclock(c);
	wr(c, Clksrc, 0);
	wr(c, Clkdiv, div);
	updateclock(c);
	wr(c, Clkena, 1);
	updateclock(c);
}

/*
 * the receive interrupt comes above the watermark: one below a block
 * (64 bytes of SWITCH_FUNC) would never come
 */
static void
setwatermark(Ctlr *c, int bsize)
{
	int half, rx;

	half = c->fifodepth/2;
	rx = bsize / c->fifow;
	if(rx > half)
		rx = half;
	if(rx < 1)
		rx = 1;
	wr(c, Fifoth, 2<<28 | (rx-1)<<16 | half);
}

static int
dwmmcinit(SDio *io)
{
	static int n;
	Ctlr *c;
	char *p;
	ulong ver;

	if(n >= nelem(soc.dwmmc) || soc.dwmmc[n] == 0)
		return -1;
	c = &dwmmcctlr[n];
	c->regs = vmap(soc.dwmmc[n], 0x1000);
	if(c->regs == nil)
		return -1;
	c->irq = soc.dwmmcirq[n];
	c->clk = soc.dwmmcclk;
	if((p = getconf("*dwmmcclk")) != nil)
		c->clk = strtoul(p, nil, 0);
	if(c->clk == 0)
		c->clk = 50000000;
	ver = rd(c, Verid) & 0xFFFF;
	c->fifo = (ver >= 0x240A? 0x200: 0x100)/4;
	c->fifow = ((rd(c, Hcon) >> Hdatashift) & 7) == 2? 8: 4;
	c->fifodepth = ((rd(c, Fifoth) >> 16) & 0xFFF) + 1;
	if(c->fifodepth < 8)
		c->fifodepth = 32;	/* the device tree's */
	io->aux = c;
	print("%s%d: %#p version %#lux fifo %dx%d irq %d clock %lud\n",
		io->name, n, soc.dwmmc[n], ver, c->fifodepth, c->fifow, c->irq, c->clk);
	n++;
	return n < nelem(soc.dwmmc) && soc.dwmmc[n] != 0;
}

static int
dwmmcinquiry(SDio*, char *inquiry, int inqlen)
{
	return snprint(inquiry, inqlen, "DesignWare MMC Host Controller");
}

static void
dwmmcenable(SDio *io)
{
	Ctlr *c = io->aux;

	ctlrreset(c, Reset|Fiforeset|Dmareset);
	wr(c, Ctrl, rd(c, Ctrl) & ~(Dmaenable|Idmacenable));
	wr(c, Bmod, 0);
	wr(c, Pwren, 1);
	delay(10);
	wr(c, Rstn, 1);
	wr(c, Tmout, ~0);
	wr(c, Rintsts, ~0);
	setwatermark(c, 512);
	wr(c, Ctype, 0);
	setclock(c, 400000);
	wr(c, Intmask, Cmderr|Dataerr|Cdone|Dto|Txdr|Rxdr);
	intrenable(c->irq, dwmmcinterrupt, c, BUSUNKNOWN, io->name);
	wr(c, Ctrl, rd(c, Ctrl) | Intenable);
}

static int
dwmmccmd(SDio *io, SDiocmd *iocmd, u32int arg, u32int *resp)
{
	Ctlr *c = io->aux;
	u32int cmd, sts;

	cmd = Start | Holdreg | iocmd->index;
	switch(iocmd->resp){
	case 0:
		break;
	case 2:
		cmd |= Respexp | Resplong | Respcrc;
		break;
	case 3:
		cmd |= Respexp;		/* R3 (ocr) has no crc */
		break;
	default:
		cmd |= Respexp | Respcrc;
		break;
	}
	if(iocmd->index == 0)
		cmd |= Sendinit;	/* 80 clocks first, the card's power up */
	if(iocmd->index == 12 && iocmd->data == 0)
		cmd |= Stopabort;
	else
		cmd |= Prvdatwait;
	if(iocmd->data){
		cmd |= Dataexp;
		if((iocmd->data & 1) == 0)
			cmd |= Wr;
	}

	if(waitbits(c, Cmd, Start, 500) < 0)
		error(Eio);
	ilock(c);
	c->sts = 0;
	if((cmd & Dataexp) == 0)
		c->buf = nil;
	wr(c, Cmdarg, arg);
	wr(c, Cmd, cmd);
	iunlock(c);

	sts = intrwait(c, Cdone, Cmderr, 1000);
	if((sts & Cdone) == 0 || (sts & Cmderr) != 0){
		if(sts & Hle)
			print("%s: cmd %d: hardware locked\n", io->name, iocmd->index);
		if(cmd & Dataexp){
			ctlrreset(c, Fiforeset);
			c->buf = nil;
		}
		error(Eio);
	}
	switch(iocmd->resp){
	case 0:
		resp[0] = 0;
		break;
	case 2:
		resp[0] = rd(c, Resp0);
		resp[1] = rd(c, Resp1);
		resp[2] = rd(c, Resp2);
		resp[3] = rd(c, Resp3);
		break;
	default:
		resp[0] = rd(c, Resp0);
		break;
	}
	if(iocmd->busy && waitbits(c, Status, Busy, 3000) < 0)
		error(Eio);
	return 0;
}

static void
dwmmciosetup(SDio *io, int write, void *buf, int bsize, int bcount)
{
	Ctlr *c = io->aux;

	if(bsize == 0 || (bsize & 3) != 0)
		error(Egreg);
	ctlrreset(c, Fiforeset);
	setwatermark(c, bsize);
	wr(c, Blksiz, bsize);
	wr(c, Bytcnt, bsize*bcount);
	ilock(c);
	c->write = write;
	c->buf = buf;
	c->len = bsize*bcount;
	c->bsize = bsize;
	c->bcount = bcount;
	iunlock(c);
}

static void
dwmmcio(SDio *io, int write, uchar*, int len)
{
	Ctlr *c = io->aux;
	u32int sts;

	if(len != c->bsize*c->bcount)
		error(Egreg);
	if(write){
		ilock(c);
		fifowrite(c);	/* the first words, Txdr the rest */
		iunlock(c);
	}
	sts = intrwait(c, Dto, Dataerr, 3000 + len/1024);
	ilock(c);
	if(!write)
		fiforead(c);	/* the words below the watermark */
	len = c->len;
	c->buf = nil;
	iunlock(c);
	if((sts & Dto) == 0 || (sts & Dataerr) != 0 || len != 0){
		print("%s: data %s: sts %ux left %d\n", io->name,
			write? "write": "read", sts, len);
		ctlrreset(c, Fiforeset);
		error(Eio);
	}
	if(write && waitbits(c, Status, Busy, 3000) < 0)
		error(Eio);
}

static void
dwmmcbus(SDio *io, int width, int speed)
{
	Ctlr *c = io->aux;

	switch(width){
	case 1:
		wr(c, Ctype, 0);
		break;
	case 4:
		wr(c, Ctype, Width4);
		break;
	case 8:
		wr(c, Ctype, Width8);
		break;
	}
	if(speed)
		setclock(c, speed);
}

void
sddwmmclink(void)
{
	static SDio io = {
		"dwmmc",
		dwmmcinit,
		dwmmcenable,
		dwmmcinquiry,
		dwmmccmd,
		dwmmciosetup,
		dwmmcio,
		dwmmcbus,
	};
	addmmcio(&io);
}
