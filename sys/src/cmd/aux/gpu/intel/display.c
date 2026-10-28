/*
 * SPDX-License-Identifier: MIT
 *
 * The display of gen 8 (Broadwell, Haswell's DDI), read from the
 * hardware as the firmware or aux/vga set it up: the outputs, whether
 * something is connected, its EDID and modes, and what each output
 * shows now; and a mode of the EDID's set on an HDMI output (DVI mode),
 * on pipe and transcoder B, showing a frame buffer of the driver's.
 *
 * It follows Linux's i915 (v4.4; notices in NOTICE): the outputs as
 * intel_setup_outputs finds them on DDI hardware (intel_display.c),
 * their live status (intel_dp.c, intel_hdmi.c: the PCH's hotplug
 * status), the DP AUX channel (intel_dp.c: intel_dp_aux_ch,
 * intel_dp_aux_transfer, the clock dividers), I2C over AUX for the EDID
 * (drm_dp_helper.c: drm_dp_i2c_xfer), GMBUS for HDMI's DDC
 * (intel_i2c.c: gmbus_xfer), the transcoders' state and timings
 * (intel_ddi.c, intel_display.c: the hardware state readout), and the
 * modeset as haswell_crtc_enable and haswell_crtc_disable do it for a
 * DDI port in HDMI or DVI mode (intel_display.c, intel_ddi.c: the power
 * well, hsw_ddi_calculate_wrpll, intel_prepare_ddi_buffers, the
 * transcoder function; intel_pm.c: the ILK-style watermarks).
 */
#include <u.h>
#include <libc.h>
#include "intel.h"
#include "ddibuf.h"

enum {
	/* DDI ports A-D */
	Ddibufctl	= 0x64000,	/* DDI_BUF_CTL(port): +0x100 a port */
	Ddiinitdetected	= 1<<0,		/* port A: a display (eDP) */
	Sfusestrap	= 0xC2014,	/* ports B, C, D present: bits 2, 1, 0 */
	Sdeisr		= 0xC4000,	/* the PCH's hotplug status: ports B, C, D at 21, 22, 23 */
	Portclksel	= 0x46100,	/* PORT_CLK_SEL(port): +4 a port */
	Lcpllctl	= 0x130040,
	Fusestrap	= 0x42014,
	Rawclkfreq	= 0xC6204,	/* PCH_RAWCLK_FREQ, MHz */

	/* transcoders A, B, C, eDP */
	Transddifuncctl	= 0x400,	/* from the transcoder's base */
	Tddienable	= 1<<31,
	Tddiedpinput	= 7<<12,
	Transhtotal	= 0x00,
	Transvtotal	= 0x0C,
	Translinkm	= 0x40,
	Translinkn	= 0x44,
	Pipeconf	= 0x70008,	/* PIPECONF(cpu_transcoder): A, B, C +0x1000, eDP at 0x7F008 */
	Pipeenable	= 1<<31,

	/* DP AUX */
	Auxsendbusy	= 1<<31,
	Auxdone		= 1<<30,
	Auxtimeouterr	= 1<<28,
	Auxtimeout600	= 1<<26,
	Auxrecverr	= 1<<25,
	Auxsizeshift	= 20,
	Auxprecharge	= 5<<16,

	Auxi2cwrite	= 0x0,
	Auxi2cread	= 0x1,
	Auxi2cmot	= 0x4,
	Auxnativeread	= 0x9,
	Replynative	= 3,		/* ack 0, nack 1, defer 2 */
	Replyi2c	= 3<<2,

	/* GMBUS (the PCH's I2C) */
	Gmbus0		= 0xC5100,	/* pin, rate */
	Gmbus1		= 0xC5104,	/* command */
	Gmbus2		= 0xC5108,	/* status */
	Gmbus3		= 0xC510C,	/* data */
	Gmbus4		= 0xC5110,	/* interrupt mask */
	Gmswclrint	= 1<<31,
	Gmswrdy		= 1<<30,
	Gmcyclewait	= 1<<25,
	Gmcycleindex	= 2<<25,
	Gmcyclestop	= 4<<25,
	Gmactive	= 1<<9,
	Gmsatoer	= 1<<10,
	Gmhwrdy		= 1<<11,

	Ddcaddr		= 0x50,

	/* the modeset: power well, PLLs, buffers, transcoder B, pipe B, plane B */
	Pwrwelldriver	= 0x45404,	/* HSW_PWR_WELL_DRIVER */
	Pwrwellreq	= 1<<31,
	Pwrwellon	= 1<<30,
	Wrpllctl	= 0x46040,	/* WRPLL_CTL1, +0x20: WRPLL_CTL2 */
	Wrpllenable	= 1<<31,
	Wrplllcpll	= 3<<28,	/* reference: the LCPLL */
	Ddibuftransreg	= 0x64E00,	/* DDI_BUF_TRANS(port): +0x60 a port, 8 an entry */
	Ddibufidle	= 1<<7,
	Ddibufenable	= 1<<31,
	Transclksel	= 0x46140,	/* TRANS_CLK_SEL(transcoder): +4 */
	Transhblank	= 0x04,
	Transhsync	= 0x08,
	Transvblank	= 0x10,
	Transvsync	= 0x14,
	Transvsyncshift	= 0x28,
	Transpipemult	= 0x2C,
	Pipesrc		= 0x6001C,	/* +0x1000 a pipe */
	Gammamode	= 0x4A480,	/* +0x800 a pipe */
	Pipemisc	= 0x70030,	/* +0x1000 a pipe */
	Pipestate	= 1<<30,	/* PIPECONF: the pipe runs */
	Tddibpc8	= 0<<20,
	Tddipvsync	= 1<<17,
	Tddiphsync	= 1<<16,
	Tddidvi		= 1<<24,
	Dspcntr		= 0x70180,	/* plane registers: +0x1000 a pipe */
	Dspstride	= 0x70188,
	Dspsurf		= 0x7019C,
	Dspoffset	= 0x701A4,
	Dspenable	= 1<<31,
	Dspbgrx		= 6<<26,
	Mchsskpd	= 0x145D10,	/* MCH_SSKPD: the memory's latencies (64 bits) */
	Wm0pipe		= 0x45100,	/* WM0_PIPE[ABC]_ILK: +4 a pipe */
	Wm1lp		= 0x45108,	/* WM1-3_LP_ILK */
	Wm1slp		= 0x45120,

	Hdmimaxclock	= 300000,	/* kHz: HDMI 1.4 on DDI (hdmi_port_clock_limit) */
	Ourpipe		= 1,		/* B: A drives the panel */
};

Output outputs[Maxout];
int noutputs;

static int cdclk;		/* kHz */
static ulong transbase[] = { 0x60000, 0x61000, 0x62000, 0x6F000 };
static char *transname[] = { "A", "B", "C", "eDP" };

static void
microwait(int us)
{
	vlong t;

	for(t = nsec(); nsec() - t < us*1000LL; )
		sleep(0);
}

/* broadwell_get_display_clock_speed */
static int
getcdclk(void)
{
	u32int l;

	l = rd(Lcpllctl);
	if(l & 1<<21)
		return 800000;
	if(rd(Fusestrap) & 1<<24)
		return 450000;
	switch(l>>26 & 3){
	case 0:	return 450000;
	case 1:	return 540000;
	case 2:	return 337500;
	}
	return 675000;
}

static ulong
auxctl(int port)
{
	return port == 0 ? 0x64010 : 0xE4110 + (port-1)*0x100;
}

/*
 * one AUX transaction: send n bytes, receive up to nr (intel_dp_aux_ch);
 * the bytes received or -1
 */
static int
auxch(int port, uchar *send, int n, uchar *recv, int nr)
{
	ulong ctl;
	u32int st, sendctl, w;
	int i, j, div, try, got;
	vlong t;

	ctl = auxctl(port);
	for(try = 0; try < 3 && (rd(ctl) & Auxsendbusy); try++)
		sleep(1);
	if(try == 3)
		return -1;
	if(port == 0)
		div = (cdclk + 1000) / 2000;
	else
		div = ((rd(Rawclkfreq) & 0x3FF) + 1) / 2;
	sendctl = Auxsendbusy | Auxdone | Auxtimeouterr | (port == 0 ? Auxtimeout600 : 0) |
		Auxrecverr | n<<Auxsizeshift | Auxprecharge | div;
	st = 0;
	for(try = 0; try < 5; try++){
		for(i = 0; i < n; i += 4){
			w = 0;
			for(j = 0; j < 4; j++)
				w |= (i+j < n ? send[i+j] : 0) << (24 - 8*j);
			wr(ctl + 4 + i, w);
		}
		wr(ctl, sendctl);
		for(t = nsec(); ((st = rd(ctl)) & Auxsendbusy) && nsec() - t < 10*1000000LL; )
			sleep(0);
		wr(ctl, st | Auxdone | Auxtimeouterr | Auxrecverr);
		if(st & Auxtimeouterr)
			continue;
		if(st & Auxrecverr){
			microwait(500);
			continue;
		}
		if(st & Auxdone)
			break;
	}
	if((st & Auxdone) == 0 || (st & (Auxtimeouterr|Auxrecverr)))
		return -1;
	got = st>>Auxsizeshift & 0x1F;
	if(got > nr)
		got = nr;
	for(i = 0; i < got; i += 4){
		w = rd(ctl + 4 + i);
		for(j = 0; j < 4 && i+j < got; j++)
			recv[i+j] = w >> (24 - 8*j);
	}
	return got;
}

/*
 * an AUX message (intel_dp_aux_transfer): request, address, size bytes
 * of data; the reply nibble, or -1; reads fill buf
 */
static int
auxmsg(int port, int req, int addr, uchar *buf, int size)
{
	uchar tx[20], rx[20];
	int n, nt, r;

	tx[0] = req<<4 | (addr>>16 & 0xF);
	tx[1] = addr>>8;
	tx[2] = addr;
	tx[3] = size - 1;
	if(req & 1){	/* reads */
		nt = size ? 4 : 3;
		if((n = auxch(port, tx, nt, rx, size+1)) <= 0)
			return -1;
		r = rx[0]>>4;
		if(n-1 > 0 && buf != nil)
			memmove(buf, rx+1, n-1 < size ? n-1 : size);
		return r;
	}
	nt = size ? 4 + size : 3;
	if(size)
		memmove(tx+4, buf, size);
	if((n = auxch(port, tx, nt, rx, 2)) <= 0)
		return -1;
	return rx[0]>>4;
}

/* an I2C-over-AUX message with retries on defer (drm_dp_i2c_do_msg) */
static int
auxi2c(int port, int req, uchar *buf, int size)
{
	int try, r;

	for(try = 0; try < 16; try++){
		if((r = auxmsg(port, req, Ddcaddr, buf, size)) < 0)
			return -1;
		if((r & Replynative) == 2 || (r & Replyi2c) == 2<<2){	/* defer */
			microwait(500);
			continue;
		}
		if(r != 0)
			return -1;
		return 0;
	}
	return -1;
}

/*
 * EDID bytes from offset over AUX: the offset written, then read, 16
 * bytes a message (drm_dp_i2c_xfer: a bare address to start each
 * message, one without MOT to end)
 */
static int
auxedid(int port, int off, uchar *buf, int n)
{
	uchar o;
	int i, k, r;

	o = off;
	r = -1;
	if(auxi2c(port, Auxi2cwrite|Auxi2cmot, nil, 0) < 0 || auxi2c(port, Auxi2cwrite|Auxi2cmot, &o, 1) < 0)
		goto End;
	if(auxi2c(port, Auxi2cread|Auxi2cmot, nil, 0) < 0)
		goto End;
	for(i = 0; i < n; i += k){
		k = n - i < 16 ? n - i : 16;
		if(auxi2c(port, Auxi2cread|Auxi2cmot, buf+i, k) < 0)
			goto End;
	}
	r = 0;
End:
	auxi2c(port, Auxi2cread, nil, 0);
	return r;
}

/* GMBUS: wait for status bits, -1 on a NAK or after 50 ms (gmbus_wait_hw_status) */
static int
gmwait(u32int bits)
{
	u32int st;
	vlong t;

	for(t = nsec(); nsec() - t < 50*1000000LL; sleep(0)){
		st = rd(Gmbus2);
		if(st & Gmsatoer)
			return -1;
		if(st & bits)
			return 0;
	}
	return -1;
}

static void
gmreset(void)
{
	wr(Gmbus1, Gmswclrint);
	wr(Gmbus1, 0);
	wr(Gmbus0, 0);
}

/* EDID bytes from offset over GMBUS pin (gmbus_xfer: an index cycle, then the read) */
static int
gmedid(int pin, int off, uchar *buf, int n)
{
	u32int v;
	int i, j;
	vlong t;

	wr(Gmbus4, 0);
	wr(Gmbus0, pin);	/* 100 kHz */
	wr(Gmbus1, Gmcycleindex | off<<8 | Gmcyclewait | n<<16 | Ddcaddr<<1 | 1 | Gmswrdy);
	for(i = 0; i < n; ){
		if(gmwait(Gmhwrdy) < 0){
			gmreset();
			return -1;
		}
		v = rd(Gmbus3);
		for(j = 0; j < 4 && i < n; j++, i++)
			buf[i] = v >> 8*j;
	}
	wr(Gmbus1, Gmcyclestop | Gmswrdy);
	for(t = nsec(); (rd(Gmbus2) & Gmactive) && nsec() - t < 10*1000000LL; )
		sleep(0);
	wr(Gmbus0, 0);
	return 0;
}

static int
edidok(uchar *e)
{
	static uchar hdr[8] = { 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0 };
	uchar s;
	int i;

	if(memcmp(e, hdr, 8) != 0)
		return 0;
	for(s = 0, i = 0; i < 128; i++)
		s += e[i];
	return s == 0;
}

/* the EDID, base block and one extension at most */
static void
readedid(Output *o)
{
	static int gmpin[] = { 0, 5, 4, 6 };	/* GMBUS_PIN_DPB, DPC, DPD */
	int (*get)(int, int, uchar*, int);
	int a;

	o->nedid = 0;
	if(o->type == Ohdmi){
		get = gmedid;
		a = gmpin[o->port];
	}else{
		get = auxedid;
		a = o->port;
	}
	if(get(a, 0, o->edid, 128) < 0 || !edidok(o->edid))
		return;
	o->nedid = 128;
	if(o->edid[126] >= 1 && get(a, 128, o->edid+128, 128) == 0)
		o->nedid = 256;
}

/* a DP sink answers on AUX: its DPCD revision */
static int
dpsink(int port)
{
	uchar rev;

	return auxmsg(port, Auxnativeread, 0, &rev, 1) == 0 && rev != 0;
}

/* the transcoder driving port, its pipe and timings (the hardware state readout) */
static void
readmode(Output *o)
{
	u32int f, ht, vt, m, n, clk;
	long dot;
	int t, p;

	o->trans = o->pipe = -1;
	for(t = 0; t < nelem(transbase); t++){
		f = rd(transbase[t] + Transddifuncctl);
		if((f & Tddienable) == 0 || (f>>28 & 7) != o->port)
			continue;
		if(t == Tedp)
			switch(f & Tddiedpinput){
			case 0<<12: case 4<<12:	p = 0; break;
			case 5<<12:	p = 1; break;
			default:	p = 2; break;
			}
		else
			p = t;
		/* on DDI hardware PIPECONF is the transcoder's (i915: PIPECONF(cpu_transcoder)) */
		if((rd(t == Tedp ? 0x7F008 : Pipeconf + t*0x1000) & Pipeenable) == 0)
			continue;
		o->trans = t;
		o->pipe = p;
		ht = rd(transbase[t] + Transhtotal);
		vt = rd(transbase[t] + Transvtotal);
		o->w = (ht & 0xFFFF) + 1;
		o->h = (vt & 0xFFFF) + 1;
		/* DP: the dot clock from the link's rate and M/N (intel_dotclock_calculate) */
		o->refresh = 0;
		if(o->type != Ohdmi){
			switch(rd(Portclksel + 4*o->port) >> 29){
			case 0:	clk = 270000; break;
			case 1:	clk = 135000; break;
			case 2:	clk = 81000; break;
			default: clk = 0; break;
			}
			m = rd(transbase[t] + Translinkm) & 0xFFFFFF;
			n = rd(transbase[t] + Translinkn);
			if(clk && n){
				dot = (vlong)clk * m / n;	/* kHz */
				o->refresh = (vlong)dot * 1000000 / ((vlong)((ht>>16) + 1) * ((vt>>16) + 1));
			}
		}
		return;
	}
}

static Output*
newout(char *name, int port, int type)
{
	Output *o;

	if(noutputs == Maxout)
		return nil;
	o = &outputs[noutputs++];
	memset(o, 0, sizeof *o);
	snprint(o->name, sizeof o->name, "%s", name);
	o->port = port;
	o->type = type;
	o->trans = o->pipe = -1;
	o->wrpll = -1;
	return o;
}

/* the outputs (intel_setup_outputs on DDI), what is connected, their EDIDs and modes */
void
dispinit(void)
{
	char name[16];
	u32int straps, hpd;
	Output *o, *d, *h;
	int port;

	cdclk = getcdclk();
	noutputs = 0;
	if(rd(Ddibufctl) & Ddiinitdetected){
		o = newout("eDP-1", 0, Oedp);
		o->connected = 1;	/* the panel */
		readedid(o);
		readmode(o);
	}
	straps = rd(Sfusestrap);
	hpd = rd(Sdeisr);
	for(port = 1; port <= 3; port++){
		if((straps & 1<<(3-port)) == 0)
			continue;
		snprint(name, sizeof name, "DP-%d", port);
		d = newout(name, port, Odp);
		snprint(name, sizeof name, "HDMI-A-%d", port);
		h = newout(name, port, Ohdmi);
		if(d == nil || h == nil || (hpd & 1<<(20+port)) == 0)
			continue;
		/* a sink there: DP if it answers on AUX, else HDMI (DDC on GMBUS) */
		if(dpsink(port)){
			d->connected = 1;
			readedid(d);
			readmode(d);
		}else{
			readedid(h);
			h->connected = h->nedid > 0;
			readmode(h);
		}
	}
}

/*
 * the EDID's modes, the preferred first: detailed timings (base block,
 * CEA extension), standard and some established timings; "wxh hz"
 */
static int
addmode(char *b, int n, int len, int w, int h, int hz)
{
	char m[32];

	if(w <= 0 || h <= 0)
		return n;
	snprint(m, sizeof m, "%dx%d %d\n", w, h, hz);
	if(strstr(b, m) != nil)
		return n;
	return n + snprint(b+n, len-n, "%s", m);
}

static int
dtd(char *b, int n, int len, uchar *d)
{
	int clk, ha, hb, va, vb;

	clk = (d[0] | d[1]<<8) * 10;	/* kHz */
	if(clk == 0)
		return n;
	ha = d[2] | (d[4]>>4)<<8;
	hb = d[3] | (d[4]&0xF)<<8;
	va = d[5] | (d[7]>>4)<<8;
	vb = d[6] | (d[7]&0xF)<<8;
	if(ha+hb == 0 || va+vb == 0)
		return n;
	return addmode(b, n, len, ha, va, (int)(((vlong)clk*1000 + (ha+hb)*(va+vb)/2) / ((ha+hb)*(va+vb))));
}

char*
outmodes(Output *o)
{
	static struct { int byte, bit, w, h; } est[] = {
		35, 5, 640, 480,  35, 0, 800, 600,  36, 3, 1024, 768,  36, 0, 1280, 1024,
	};
	char *b;
	uchar *e, *x;
	int n, i, w, a, len;

	len = 4096;
	b = mallocz(len, 1);
	if(o->nedid == 0)
		return b;
	e = o->edid;
	n = 0;
	for(i = 54; i < 126; i += 18)
		n = dtd(b, n, len, e+i);
	if(o->nedid >= 256 && e[128] == 2)	/* CEA-861: its detailed timings */
		for(x = e+128, i = x[2]; i >= 4 && i + 18 <= 127; i += 18)
			n = dtd(b, n, len, x+i);
	for(i = 38; i < 54; i += 2){
		if(e[i] == 1 && e[i+1] == 1)
			continue;
		w = (e[i] + 31) * 8;
		a = e[i+1] >> 6;
		n = addmode(b, n, len, w, a == 0 ? w*10/16 : a == 1 ? w*3/4 : a == 2 ? w*4/5 : w*9/16,
			(e[i+1] & 0x3F) + 60);
	}
	for(i = 0; i < nelem(est); i++)
		if(e[est[i].byte] & 1<<est[i].bit)
			n = addmode(b, n, len, est[i].w, est[i].h, 60);
	return b;
}

char*
outmode(Output *o)
{
	if(o->trans < 0)
		return smprint("off\n");
	return smprint("%dx%d %ld\n", o->w, o->h, (o->refresh + 500) / 1000);
}

char*
dispstatus(void)
{
	char *s, *t;
	Output *o;
	int i;

	s = smprint("display cdclk %d kHz\n", cdclk);
	for(i = 0; i < noutputs; i++){
		o = &outputs[i];
		if(o->trans >= 0)
			t = smprint("%soutput %s %s, %dx%d %ld.%03ld Hz, pipe %c transcoder %s, edid %d bytes\n", s,
				o->name, o->connected ? "connected" : "disconnected", o->w, o->h,
				o->refresh / 1000, o->refresh % 1000, 'A' + o->pipe, transname[o->trans], o->nedid);
		else
			t = smprint("%soutput %s %s, off, edid %d bytes\n", s, o->name,
				o->connected ? "connected" : "disconnected", o->nedid);
		free(s);
		s = t;
	}
	return s;
}

/*
 * the EDID's detailed timings, to set (the standard timings carry no
 * timing); the first one is the preferred
 */
typedef struct Timing Timing;
struct Timing {
	int	clock;		/* kHz */
	int	ha, hss, hse, ht;
	int	va, vss, vse, vt;
	int	phsync, pvsync;
	int	hz;
};

static int
dtdtiming(uchar *d, Timing *t)
{
	int hb, vb, hso, hsw, vso, vsw;

	t->clock = (d[0] | d[1]<<8) * 10;
	if(t->clock == 0 || (d[17] & 0x80))	/* not a timing, interlaced */
		return -1;
	t->ha = d[2] | (d[4]>>4)<<8;
	hb = d[3] | (d[4]&0xF)<<8;
	t->va = d[5] | (d[7]>>4)<<8;
	vb = d[6] | (d[7]&0xF)<<8;
	hso = d[8] | (d[11]>>6 & 3)<<8;
	hsw = d[9] | (d[11]>>4 & 3)<<8;
	vso = d[10]>>4 | (d[11]>>2 & 3)<<4;
	vsw = (d[10] & 0xF) | (d[11] & 3)<<4;
	t->ht = t->ha + hb;
	t->vt = t->va + vb;
	t->hss = t->ha + hso;
	t->hse = t->hss + hsw;
	t->vss = t->va + vso;
	t->vse = t->vss + vsw;
	if(t->ht == 0 || t->vt == 0)
		return -1;
	t->phsync = (d[17] & 0x18) == 0x18 && (d[17] & 2);
	t->pvsync = (d[17] & 0x18) == 0x18 && (d[17] & 4);
	t->hz = ((vlong)t->clock*1000 + t->ht*t->vt/2) / (t->ht*t->vt);
	return 0;
}

/* the timing for w x h [hz] among the EDID's detailed timings that the port can drive */
static int
findtiming(Output *o, int w, int h, int hz, Timing *t)
{
	uchar *e, *x;
	int i;

	e = o->edid;
	if(o->nedid < 128)
		return -1;
	for(i = 54; i < 126; i += 18)
		if(dtdtiming(e+i, t) == 0 && t->ha == w && t->va == h && (hz == 0 || t->hz == hz)
		&& t->clock <= Hdmimaxclock)
			return 0;
	if(o->nedid >= 256 && e[128] == 2)
		for(x = e+128, i = x[2]; i >= 4 && i + 18 <= 127; i += 18)
			if(dtdtiming(x+i, t) == 0 && t->ha == w && t->va == h && (hz == 0 || t->hz == hz)
			&& t->clock <= Hdmimaxclock)
				return 0;
	return -1;
}

/* hsw_wrpll_get_budget_for_freq: ppm the WRPLL may be off for a clock (Hz) */
static uint
wrpllbudget(int clock)
{
	switch(clock){
	case 25175000: case 25200000: case 27000000: case 27027000: case 37762500:
	case 37800000: case 40500000: case 40541000: case 54000000: case 54054000:
	case 59341000: case 59400000: case 72000000: case 74176000: case 74250000:
	case 81000000: case 81081000: case 89012000: case 89100000: case 108000000:
	case 108108000: case 111264000: case 111375000: case 148352000: case 148500000:
	case 162000000: case 162162000: case 222525000: case 222750000: case 296703000:
	case 297000000:
		return 0;
	case 233500000: case 245250000: case 247750000: case 253250000: case 298000000:
		return 1500;
	case 169128000: case 169500000: case 179500000: case 202000000:
		return 2000;
	case 256250000: case 262500000: case 270000000: case 272500000: case 273750000:
	case 280750000: case 281250000: case 286000000: case 291750000:
		return 4000;
	case 267250000: case 268500000:
		return 5000;
	}
	return 1000;
}

enum { Lcfreq = 2700, Pmin = 2, Pmax = 64, Pinc = 2, Refmin = 48, Refmax = 400, Vcomin = 2400, Vcomax = 4800 };

static uvlong
absdiff(uvlong a, uvlong b)
{
	return a > b ? a - b : b - a;
}

/* hsw_wrpll_update_rnp */
static void
wrpllupdate(uvlong freq2k, uint budget, uint r2, uint n2, uint p, uint *bp, uint *bn2, uint *br2)
{
	uvlong a, b, c, d, diff, diffbest;

	if(*bp == 0){
		*bp = p;
		*bn2 = n2;
		*br2 = r2;
		return;
	}
	a = freq2k * budget * p * r2;
	b = freq2k * budget * *bp * *br2;
	diff = absdiff(freq2k * p * r2, (uvlong)Lcfreq*2000 * n2);
	diffbest = absdiff(freq2k * *bp * *br2, (uvlong)Lcfreq*2000 * *bn2);
	c = 1000000 * diff;
	d = 1000000 * diffbest;
	if(a < c && b < d){
		if(*bp * *br2 * diff < p * r2 * diffbest){
			*bp = p; *bn2 = n2; *br2 = r2;
		}
	}else if(a >= c && b < d){
		*bp = p; *bn2 = n2; *br2 = r2;
	}else if(a >= c && b >= d){
		if(n2 * *br2 * *br2 > *bn2 * r2 * r2){
			*bp = p; *bn2 = n2; *br2 = r2;
		}
	}
}

/* hsw_ddi_calculate_wrpll: the WRPLL's dividers for a clock (Hz) */
static u32int
wrpllval(int clock)
{
	uvlong freq2k;
	uint budget, r2, n2, p, bp, bn2, br2;

	freq2k = clock / 100;
	budget = wrpllbudget(clock);
	bp = bn2 = br2 = 0;
	if(freq2k == 5400000){
		bn2 = 2; bp = 1; br2 = 2;
	}else
		for(r2 = Lcfreq*2/Refmax + 1; r2 <= Lcfreq*2/Refmin; r2++)
			for(n2 = Vcomin*r2/Lcfreq + 1; n2 <= Vcomax*r2/Lcfreq; n2++)
				for(p = Pmin; p <= Pmax; p += Pinc)
					wrpllupdate(freq2k, budget, r2, n2, p, &bp, &bn2, &br2);
	return Wrpllenable | Wrplllcpll | br2 | bp<<8 | bn2<<16;
}

static int
waitbits(ulong r, u32int mask, u32int want, int ms)
{
	vlong t;

	for(t = nsec(); nsec() - t < ms*1000000LL; sleep(1))
		if((rd(r) & mask) == want)
			return 0;
	return -1;
}

/* colour bars and a frame: what the driver shows until a client draws */
static void
testpicture(Output *o, int w, int h)
{
	static u32int bars[] = { 0xFFFFFF, 0xFFFF00, 0x00FFFF, 0x00FF00, 0xFF00FF, 0xFF0000, 0x0000FF, 0x000000 };
	u32int *p;
	int x, y;

	for(y = 0; y < h; y++){
		p = o->fb + y*w;
		for(x = 0; x < w; x++)
			p[x] = (y < 8 || y >= h-8 || x < 8 || x >= w-8) ? 0x808080 : bars[x*8/w];
	}
	clflushrange(o->fb, (ulong)w*h*4);
}

/* switch off what this driver set (haswell_crtc_disable); the frame buffer stays */
static void
modeoff(Output *o)
{
	ulong tb;
	int p;

	if(!o->ours)
		return;
	p = Ourpipe;
	tb = transbase[p];
	wr(Dspcntr + p*0x1000, 0);
	wr(Dspsurf + p*0x1000, 0);
	wr(Pipeconf + p*0x1000, rd(Pipeconf + p*0x1000) & ~Pipeenable);
	waitbits(Pipeconf + p*0x1000, Pipestate, 0, 100);
	wr(tb + Transddifuncctl, 0);
	wr(Transclksel + 4*p, 0);
	wr(Ddibufctl + 0x100*o->port, rd(Ddibufctl + 0x100*o->port) & ~Ddibufenable);
	waitbits(Ddibufctl + 0x100*o->port, Ddibufidle, Ddibufidle, 10);
	wr(Portclksel + 4*o->port, 7u<<29);	/* PORT_CLK_SEL_NONE */
	if(o->wrpll >= 0)
		wr(Wrpllctl + 0x20*o->wrpll, rd(Wrpllctl + 0x20*o->wrpll) & ~Wrpllenable);
	wr(Wm0pipe + 4*p, 0);
	o->wrpll = -1;
	o->ours = 0;
	o->trans = o->pipe = -1;
}

/*
 * the mode on an HDMI output, DVI mode (no infoframes), 8 bits a
 * colour, on pipe and transcoder B (haswell_crtc_enable, the DDI
 * encoder's pre_enable and enable)
 */
static char*
modeset(Output *o, Timing *t)
{
	uvlong sskpd;
	u32int f, wm;
	ulong tb;
	int p, i, pll, lat;

	if(o->type != Ohdmi)
		return "only HDMI outputs yet";
	modeoff(o);
	p = Ourpipe;
	tb = transbase[p];
	if((rd(Pipeconf + p*0x1000) & Pipeenable) != 0)
		return "pipe B in use";

	/* the frame buffer: one of the mode's size, the colour bars */
	if(o->fb == nil || o->fbsize < (ulong)t->ha*t->va*4){
		o->fbsize = ((ulong)t->ha*t->va*4 + 4095) & ~4095UL;
		if((o->fbgtt = fbnew(o->fbsize, &o->fb, o->fbseg, sizeof o->fbseg)) == 0){
			o->fb = nil;
			return "no memory for a frame buffer";
		}
	}
	testpicture(o, t->ha, t->va);

	/* the display power well (pipes B, C and their transcoders) */
	wr(Pwrwelldriver, Pwrwellreq);
	if(waitbits(Pwrwelldriver, Pwrwellon, Pwrwellon, 20) < 0)
		return "power well does not come on";

	/* a free WRPLL for the pixel clock (8 bits a colour: the port clock) */
	for(pll = 0; pll < 2; pll++)
		if((rd(Wrpllctl + 0x20*pll) & Wrpllenable) == 0)
			break;
	if(pll == 2)
		return "no free WRPLL";
	wr(Wrpllctl + 0x20*pll, wrpllval(t->clock*1000));
	rd(Wrpllctl + 0x20*pll);
	sleep(1);
	o->wrpll = pll;

	/* intel_set_pipe_timings */
	wr(tb + Transvsyncshift, 0);
	wr(tb + Transhtotal, (t->ha-1) | (t->ht-1)<<16);
	wr(tb + Transhblank, (t->ha-1) | (t->ht-1)<<16);
	wr(tb + Transhsync, (t->hss-1) | (t->hse-1)<<16);
	wr(tb + Transvtotal, (t->va-1) | (t->vt-1)<<16);
	wr(tb + Transvblank, (t->va-1) | (t->vt-1)<<16);
	wr(tb + Transvsync, (t->vss-1) | (t->vse-1)<<16);
	wr(Pipesrc + p*0x1000, (t->ha-1)<<16 | (t->va-1));
	wr(tb + Transpipemult, 0);

	/* haswell_set_pipeconf: progressive, 8-bit gamma mode, 8 bits a colour */
	wr(Pipeconf + p*0x1000, 0);
	rd(Pipeconf + p*0x1000);
	wr(Gammamode + p*0x800, 0);
	wr(Pipemisc + p*0x1000, 0);

	/* the DDI encoder's pre_enable: the port's clock; its buffers (intel_prepare_ddi_buffers) */
	for(i = 0; i < nelem(bdw_ddi_translations_dp); i++){
		wr(Ddibuftransreg + 0x60*o->port + 8*i, bdw_ddi_translations_dp[i].trans1);
		wr(Ddibuftransreg + 0x60*o->port + 8*i + 4, bdw_ddi_translations_dp[i].trans2);
	}
	wr(Ddibuftransreg + 0x60*o->port + 8*i, bdw_ddi_translations_hdmi[7].trans1);	/* entry 9: HDMI, BDW's default level */
	wr(Ddibuftransreg + 0x60*o->port + 8*i + 4, bdw_ddi_translations_hdmi[7].trans2);
	wr(Portclksel + 4*o->port, (u32int)(pll+4)<<29);	/* PORT_CLK_SEL_WRPLL(pll) */

	/* intel_ddi_enable_pipe_clock, the transcoder's function */
	wr(Transclksel + 4*p, (u32int)(o->port+1)<<29);
	f = Tddienable | (u32int)o->port<<28 | Tddibpc8 | Tddidvi;
	if(t->phsync)
		f |= Tddiphsync;
	if(t->pvsync)
		f |= Tddipvsync;
	wr(tb + Transddifuncctl, f);

	/*
	 * watermarks (ilk_wm_method1 with level 0's latency, 0.1 µs units;
	 * two pipes: no LP watermarks)
	 */
	sskpd = rd(Mchsskpd) | (uvlong)rd(Mchsskpd+4)<<32;
	lat = sskpd>>56 & 0xFF;
	if(lat == 0)
		lat = sskpd & 0xF;
	if(lat == 0)
		lat = 20;
	wm = ((uvlong)t->clock * 4 * lat + 64*10000 - 1) / (64*10000) + 2;
	if(wm > 255)
		wm = 255;
	wr(Wm1lp+8, 0);
	wr(Wm1lp+4, 0);
	wr(Wm1lp, 0);
	wr(Wm1slp, 0);
	wr(Wm0pipe + 4*p, wm<<16);	/* the primary plane; sprite and cursor off */

	/* intel_enable_pipe */
	wr(Pipeconf + p*0x1000, rd(Pipeconf + p*0x1000) | Pipeenable);
	if(waitbits(Pipeconf + p*0x1000, Pipestate, Pipestate, 100) < 0){
		modeoff(o);
		return "pipe B does not start";
	}

	/* the encoder's enable: the port */
	wr(Ddibufctl + 0x100*o->port, (rd(Ddibufctl + 0x100*o->port) & (1<<4)) | Ddibufenable);

	/* the plane: the frame buffer, linear x8r8g8b8 */
	wr(Dspstride + p*0x1000, t->ha*4);
	wr(Dspoffset + p*0x1000, 0);
	wr(Dspcntr + p*0x1000, Dspenable | Dspbgrx);
	wr(Dspsurf + p*0x1000, o->fbgtt);
	o->ours = 1;
	readmode(o);
	o->refresh = (long)t->hz * 1000;
	return nil;
}

static int
parsewxh(char *s, int *w, int *h)
{
	char *x;

	*w = strtol(s, &x, 10);
	if(*x != 'x')
		return -1;
	*h = strtol(x+1, &x, 10);
	return *x == 0 && *w > 0 && *h > 0 ? 0 : -1;
}

/* write to outputs/<conn>/mode: wxh [hz] (one of the EDID's detailed timings), or off */
char*
outsetmode(Output *o, char *req)
{
	Timing t;
	char *f[3];
	int n, w, h;

	n = tokenize(req, f, nelem(f));
	if(n == 1 && strcmp(f[0], "off") == 0){
		if(!o->ours)
			return "not set by this driver";
		modeoff(o);
		return nil;
	}
	if(n < 1 || n > 2 || parsewxh(f[0], &w, &h) < 0)
		return "usage: wxh [hz] | off";
	if(!o->connected)
		return "nothing connected";
	if(findtiming(o, w, h, n == 2 ? atoi(f[1]) : 0, &t) < 0)
		return "no such mode among the EDID's detailed timings (up to 300 MHz)";
	return modeset(o, &t);
}
