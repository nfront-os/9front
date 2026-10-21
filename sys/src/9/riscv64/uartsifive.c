/*
 * SiFive UART (sifive,uart0): FU540, FU740 (qemu's sifive_u, the
 * HiFive Unmatched). Not an 8250: 8-byte receive and transmit fifos
 * with watermark interrupts, no modem lines; the registers from Geoff
 * Collyer's uartsifive.c (9k), the driver after uarti8250.c. The
 * console is soc.uart.
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

enum {					/* registers, by u32int index */
	Txdata	= 0x00/4,
	Rxdata	= 0x04/4,
	Txctrl	= 0x08/4,
	Rxctrl	= 0x0c/4,
	Ie	= 0x10/4,
	Ip	= 0x14/4,
	Div	= 0x18/4,
};

enum {
	Notready	= 1u<<31,	/* Txdata: fifo full; Rxdata: empty */

	Enable		= 1<<0,		/* Txctrl, Rxctrl */
	Stop2		= 1<<1,		/* Txctrl: two stop bits */
	Wmshift		= 16,		/* watermark level */

	Txwm		= 1<<0,		/* Ie, Ip */
	Rxwm		= 1<<1,
};

typedef struct Ctlr {
	u32int	*io;
	int	irq;
	int	iena;
	Lock;
} Ctlr;

extern PhysUart sifivephysuart;

static Ctlr sifivectlr[1];

static Uart sifiveuart[1] = {
{	.regs	= &sifivectlr[0],
	.name	= "eia0",
	.phys	= &sifivephysuart,
	.special= 0,
	.next	= nil, },
};

#define rd(c, r)	(((volatile u32int*)(c)->io)[r])
#define wr(c, r, v)	(((volatile u32int*)(c)->io)[r] = (v))

static char*
sifivestatus(Uart* uart, char *p, char *e)
{
	Ctlr *ctlr;

	ctlr = uart->regs;
	return seprint(p, e,
		"b%d c%d d%d e%d l%d m%d p%c r%d s%d i%d\n"
		"dev(%d) type(%d) framing(%d) overruns(%d) "
		"berr(%d) serr(%d)\n",
		uart->baud, uart->hup_dcd, 1, uart->hup_dsr, 8, 0, 'n', 0,
		(rd(ctlr, Txctrl) & Stop2)? 2: 1, ctlr->iena,
		uart->dev, uart->type, uart->ferr, uart->oerr,
		uart->berr, uart->serr);
}

static void
sifivefifo(Uart*, int)
{
}

static void
sifivedtr(Uart*, int)
{
}

static void
sifiverts(Uart*, int)
{
}

static void
sifivemodemctl(Uart* uart, int on)
{
	ilock(&uart->tlock);
	uart->modem = on;
	uart->cts = 1;		/* no modem lines */
	iunlock(&uart->tlock);
}

/* 8 bits, no parity only */
static int
sifiveparity(Uart*, int parity)
{
	return parity == 'n'? 0: -1;
}

static int
sifivebits(Uart*, int bits)
{
	return bits == 8? 0: -1;
}

static int
sifivestop(Uart* uart, int stop)
{
	Ctlr *ctlr;

	ctlr = uart->regs;
	switch(stop){
	case 1:
		wr(ctlr, Txctrl, rd(ctlr, Txctrl) & ~Stop2);
		break;
	case 2:
		wr(ctlr, Txctrl, rd(ctlr, Txctrl) | Stop2);
		break;
	default:
		return -1;
	}
	uart->stop = stop;
	return 0;
}

/*
 * baud = freq / (div+1); without the clock (soc.uartfreq 0: the
 * board's firmware set the console up) the divisor stays
 */
static int
sifivebaud(Uart* uart, int baud)
{
	Ctlr *ctlr;

	if(baud <= 0)
		return -1;
	if(uart->freq == 0){
		uart->baud = baud;
		return 0;
	}
	ctlr = uart->regs;
	wr(ctlr, Div, (uart->freq + baud/2)/baud - 1);
	uart->baud = baud;
	return 0;
}

static void
sifivebreak(Uart*, int)
{
}

/*
 * fill the fifo. Unlike the 8250's, the transmit interrupt is on only
 * while needed (it is pending whenever the fifo is empty): until the
 * queue is found empty -- uartstageoutput's qconsume marks it starved
 * then, so the next write kicks again. Stopping at a full fifo keeps
 * it on even when nothing seems left: the queue was not seen empty,
 * and a write now would not kick.
 */
static void
sifivekick(Uart* uart)
{
	int i, empty;
	Ctlr *ctlr;

	if(uart->cts == 0)
		return;
	ctlr = uart->regs;
	empty = 0;
	for(i = 0; i < 128; i++){
		if(rd(ctlr, Txdata) & Notready)
			break;
		if(uart->op >= uart->oe && uartstageoutput(uart) == 0){
			empty = 1;
			break;
		}
		wr(ctlr, Txdata, *(uart->op++));
	}
	if(ctlr->iena)
		wr(ctlr, Ie, empty? Rxwm: Rxwm|Txwm);
}

static void
sifiveinterrupt(Ureg*, void* arg)
{
	Ctlr *ctlr;
	Uart *uart;
	u32int ip, r;

	uart = arg;
	ctlr = uart->regs;
	ip = rd(ctlr, Ip);
	if(ip & Rxwm)
		while(((r = rd(ctlr, Rxdata)) & Notready) == 0)
			uartrecv(uart, r & 0xFF);
	if(ip & Txwm)
		uartkick(uart);
}

static void
sifivedisable(Uart* uart)
{
	Ctlr *ctlr;

	ctlr = uart->regs;
	wr(ctlr, Ie, 0);
	if(ctlr->iena != 0){
		ctlr->iena = 0;
		intrdisable(ctlr->irq, sifiveinterrupt, uart, BUSUNKNOWN, uart->name);
	}
}

static void
sifiveenable(Uart* uart, int ie)
{
	Ctlr *ctlr;

	ctlr = uart->regs;
	/* interrupt when a byte arrives (rx count > 0) and when the tx fifo is below 1 */
	wr(ctlr, Txctrl, (rd(ctlr, Txctrl) & Stop2) | Enable | 1<<Wmshift);
	wr(ctlr, Rxctrl, Enable | 0<<Wmshift);
	uart->cts = 1;
	if(ie){
		if(ctlr->iena == 0){
			ctlr->irq = soc.uartirq;	/* the device tree's, by now (bootargs.c) */
			intrenable(ctlr->irq, sifiveinterrupt, uart, BUSUNKNOWN, uart->name);
			ctlr->iena = 1;
		}
		wr(ctlr, Ie, Rxwm);
		sifiveinterrupt(nil, uart);
	}else
		wr(ctlr, Ie, 0);
}

static Uart*
sifivepnp(void)
{
	return sifiveuart;
}

static int
sifivegetc(Uart *uart)
{
	Ctlr *ctlr;
	u32int r;

	ctlr = uart->regs;
	while((r = rd(ctlr, Rxdata)) & Notready)
		delay(1);
	return r & 0xFF;
}

static void
sifiveputc(Uart *uart, int c)
{
	int i;
	Ctlr *ctlr;

	ctlr = uart->regs;
	for(i = 0; (rd(ctlr, Txdata) & Notready) && i < 128; i++)
		delay(1);
	wr(ctlr, Txdata, c);
}

PhysUart sifivephysuart = {
	.name		= "sifive",
	.pnp		= sifivepnp,
	.enable		= sifiveenable,
	.disable	= sifivedisable,
	.kick		= sifivekick,
	.dobreak	= sifivebreak,
	.baud		= sifivebaud,
	.bits		= sifivebits,
	.stop		= sifivestop,
	.parity		= sifiveparity,
	.modemctl	= sifivemodemctl,
	.rts		= sifiverts,
	.dtr		= sifivedtr,
	.status		= sifivestatus,
	.fifo		= sifivefifo,
	.getc		= sifivegetc,
	.putc		= sifiveputc,
};

void
uartconsinit(void)
{
	Uart *uart;
	Ctlr *ctlr;
	char *cmd, *p;

	uart = &sifiveuart[0];
	ctlr = uart->regs;
	ctlr->irq = soc.uartirq;
	ctlr->io = vmap(soc.uart, 0x1C);
	uart->freq = soc.uartfreq;
	(*uart->phys->enable)(uart, 0);
	uartctl(uart, "b115200 l8 pn s1");
	if((p = getconf("console")) != nil){
		strtoul(p, &cmd, 0);
		if(*cmd != '\0')
			uartctl(uart, cmd);
	}
	consuart = uart;
	uart->console = 1;
}
