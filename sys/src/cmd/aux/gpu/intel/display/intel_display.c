/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the outputs found, the mode read back, a modeset (HDMI).
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/intel_display.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"
#include "intel_ddi_buf_trans.h"

Output outputs[Maxout];
int noutputs;
ulong transbase[4] = { 0x60000, 0x61000, 0x62000, 0x6F000 };
char *transname[4] = { "A", "B", "C", "eDP" };

/* the transcoder driving port, its pipe and timings (the hardware state readout) */
void
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

Output*
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

/* switch off what this driver set (haswell_crtc_disable); the frame buffer stays */
void
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
char*
modeset(Output *o, Timing *t)
{
	uvlong sskpd;
	u32int f, wm;
	ulong tb;
	int p, i, pll, lat;
	const struct intel_ddi_buf_trans *bt;

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
	bt = bdw_trans(0);
	for(i = 0; i < bt->num_entries; i++){
		wr(Ddibuftransreg + 0x60*o->port + 8*i, bt->entries[i].hsw.trans1);
		wr(Ddibuftransreg + 0x60*o->port + 8*i + 4, bt->entries[i].hsw.trans2);
	}
	bt = bdw_trans(1);	/* entry 9: HDMI, BDW's default level */
	wr(Ddibuftransreg + 0x60*o->port + 8*i, bt->entries[bt->hdmi_default_entry].hsw.trans1);
	wr(Ddibuftransreg + 0x60*o->port + 8*i + 4, bt->entries[bt->hdmi_default_entry].hsw.trans2);
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
