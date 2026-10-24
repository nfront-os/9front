/*
 * SPDX-License-Identifier: MIT
 *
 * The display of gen 8 (Broadwell, Haswell's DDI), read from the
 * hardware as the firmware or aux/vga set it up: the outputs, whether
 * something is connected, its EDID and modes, and what each output
 * shows now. Nothing is changed yet.
 *
 * It follows Linux's i915 (v4.4; notices in NOTICE): the outputs as
 * intel_setup_outputs finds them on DDI hardware (intel_display.c),
 * their live status (intel_dp.c, intel_hdmi.c: the PCH's hotplug
 * status), the DP AUX channel (intel_dp.c: intel_dp_aux_ch,
 * intel_dp_aux_transfer, the clock dividers), I2C over AUX for the EDID
 * (drm_dp_helper.c: drm_dp_i2c_xfer), GMBUS for HDMI's DDC
 * (intel_i2c.c: gmbus_xfer), the transcoders' state and timings
 * (intel_ddi.c, intel_display.c: the hardware state readout).
 */
#include <u.h>
#include <libc.h>
#include "intel.h"

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
