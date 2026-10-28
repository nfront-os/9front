/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the outputs found, the mode read back, a hsw_crtc_enable (HDMI).
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/display/intel_display.c
 * (much of it first written after v4.4); the notices are in NOTICE,
 * UPSTREAM says which functions correspond.
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
/*
 * Broadwell's pipes and transcoders, as Linux's intel_display_device.c
 * has them (HSW_PIPE_OFFSETS, HSW_TRANSCODER_OFFSETS) for _MMIO_PIPE2
 * and _MMIO_TRANS2; indexed by our transcoder numbers (eDP: Tedp)
 */
Displayinfo bdw_display_info = {
	0,
	{ 0x70000, 0x71000, 0x72000, 0x7F000 },
	{ 0x60000, 0x61000, 0x62000, 0x6F000 },
};
char *transname[4] = { "A", "B", "C", "eDP" };

/* the transcoder driving port, its pipe and timings (the hardware state readout) */
void
readmode(Output *o)
{
	u32int f, ht, vt, m, n, clk;
	long dot;
	int t, p;

	o->trans = o->pipe = -1;
	for(t = 0; t < nelem(bdw_display_info.trans_offsets); t++){
		f = rd(TRANS_DDI_FUNC_CTL(t));
		if((f & TRANS_DDI_FUNC_ENABLE) == 0 || (f>>28 & 7) != o->port)
			continue;
		if(t == Tedp)
			switch(f & TRANS_DDI_EDP_INPUT_MASK){
			case 0<<12: case 4<<12:	p = 0; break;
			case 5<<12:	p = 1; break;
			default:	p = 2; break;
			}
		else
			p = t;
		/* on DDI hardware PIPECONF is the transcoder's (i915: PIPECONF(cpu_transcoder)) */
		if((rd(TRANSCONF(t)) & TRANSCONF_ENABLE) == 0)
			continue;
		o->trans = t;
		o->pipe = p;
		ht = rd(TRANS_HTOTAL(t));
		vt = rd(TRANS_VTOTAL(t));
		o->w = (ht & 0xFFFF) + 1;
		o->h = (vt & 0xFFFF) + 1;
		/* DP: the dot clock from the link's rate and M/N (intel_dotclock_calculate) */
		o->refresh = 0;
		if(o->type != Ohdmi){
			switch(rd(PORT_CLK_SEL(o->port)) >> 29){
			case 0:	clk = 270000; break;
			case 1:	clk = 135000; break;
			case 2:	clk = 81000; break;
			default: clk = 0; break;
			}
			m = rd(PIPE_LINK_M1(t)) & 0xFFFFFF;
			n = rd(PIPE_LINK_N1(t));
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
intel_setup_outputs(void)
{
	char name[16];
	u32int straps, hpd;
	Output *o, *d, *h;
	int port;

	cdclk = bdw_get_cdclk();
	noutputs = 0;
	if(rd(DDI_BUF_CTL(PORT_A)) & DDI_INIT_DISPLAY_DETECTED){
		o = newout("eDP-1", 0, Oedp);
		o->connected = 1;	/* the panel */
		readedid(o);
		readmode(o);
	}
	straps = rd(SFUSE_STRAP);
	hpd = rd(SDEISR);
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
hsw_crtc_disable(Output *o)
{
	int p;

	if(!o->ours)
		return;
	p = Ourpipe;
	wr(DSPCNTR(p), 0);
	wr(DSPSURF(p), 0);
	wr(TRANSCONF(p), rd(TRANSCONF(p)) & ~TRANSCONF_ENABLE);
	intel_wait_for_register(TRANSCONF(p), TRANSCONF_STATE_ENABLE, 0, 100);
	wr(TRANS_DDI_FUNC_CTL(p), 0);
	wr(TRANS_CLK_SEL(p), 0);
	wr(DDI_BUF_CTL(o->port), rd(DDI_BUF_CTL(o->port)) & ~DDI_BUF_CTL_ENABLE);
	intel_wait_for_register(DDI_BUF_CTL(o->port), DDI_BUF_IS_IDLE, DDI_BUF_IS_IDLE, 10);
	wr(PORT_CLK_SEL(o->port), PORT_CLK_SEL_NONE);	/* PORT_CLK_SEL_NONE */
	if(o->wrpll >= 0)
		wr(WRPLL_CTL(o->wrpll), rd(WRPLL_CTL(o->wrpll)) & ~WRPLL_PLL_ENABLE);
	wr(WM0_PIPE_ILK(p), 0);
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
hsw_crtc_enable(Output *o, Timing *t)
{
	uvlong sskpd;
	u32int f, wm;
	int p, i, pll, lat;
	const struct intel_ddi_buf_trans *bt;

	if(o->type != Ohdmi)
		return "only HDMI outputs yet";
	hsw_crtc_disable(o);
	p = Ourpipe;
	if((rd(TRANSCONF(p)) & TRANSCONF_ENABLE) != 0)
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
	wr(HSW_PWR_WELL_CTL2, HSW_PWR_WELL_CTL_REQ(HSW_PW_CTL_IDX_GLOBAL));
	if(intel_wait_for_register(HSW_PWR_WELL_CTL2, HSW_PWR_WELL_CTL_STATE(HSW_PW_CTL_IDX_GLOBAL), HSW_PWR_WELL_CTL_STATE(HSW_PW_CTL_IDX_GLOBAL), 20) < 0)
		return "power well does not come on";

	/* a free WRPLL for the pixel clock (8 bits a colour: the port clock) */
	for(pll = 0; pll < 2; pll++)
		if((rd(WRPLL_CTL(pll)) & WRPLL_PLL_ENABLE) == 0)
			break;
	if(pll == 2)
		return "no free WRPLL";
	wr(WRPLL_CTL(pll), hsw_ddi_calculate_wrpll(t->clock*1000));
	rd(WRPLL_CTL(pll));
	sleep(1);
	o->wrpll = pll;

	/* intel_set_pipe_timings */
	wr(TRANS_VSYNCSHIFT(p), 0);
	wr(TRANS_HTOTAL(p), (t->ha-1) | (t->ht-1)<<16);
	wr(TRANS_HBLANK(p), (t->ha-1) | (t->ht-1)<<16);
	wr(TRANS_HSYNC(p), (t->hss-1) | (t->hse-1)<<16);
	wr(TRANS_VTOTAL(p), (t->va-1) | (t->vt-1)<<16);
	wr(TRANS_VBLANK(p), (t->va-1) | (t->vt-1)<<16);
	wr(TRANS_VSYNC(p), (t->vss-1) | (t->vse-1)<<16);
	wr(PIPESRC(p), (t->ha-1)<<16 | (t->va-1));
	wr(TRANS_MULT(p), 0);

	/* haswell_set_pipeconf: progressive, 8-bit gamma mode, 8 bits a colour */
	wr(TRANSCONF(p), 0);
	rd(TRANSCONF(p));
	wr(GAMMA_MODE(p), 0);
	wr(PIPE_MISC(p), 0);

	/* the DDI encoder's pre_enable: the port's clock; its buffers (intel_prepare_ddi_buffers) */
	bt = bdw_trans(0);
	for(i = 0; i < bt->num_entries; i++){
		wr(DDI_BUF_TRANS_LO(o->port, i), bt->entries[i].hsw.trans1);
		wr(DDI_BUF_TRANS_HI(o->port, i), bt->entries[i].hsw.trans2);
	}
	bt = bdw_trans(1);	/* entry 9: HDMI, BDW's default level */
	wr(DDI_BUF_TRANS_LO(o->port, i), bt->entries[bt->hdmi_default_entry].hsw.trans1);
	wr(DDI_BUF_TRANS_HI(o->port, i), bt->entries[bt->hdmi_default_entry].hsw.trans2);
	wr(PORT_CLK_SEL(o->port), PORT_CLK_SEL_WRPLL(pll));	/* PORT_CLK_SEL_WRPLL(pll) */

	/* intel_ddi_enable_pipe_clock, the transcoder's function */
	wr(TRANS_CLK_SEL(p), TRANS_CLK_SEL_PORT(o->port));
	f = TRANS_DDI_FUNC_ENABLE | (u32int)o->port<<28 | TRANS_DDI_BPC_8 |
		TRANS_DDI_MODE_SELECT_DVI;
	if(t->phsync)
		f |= TRANS_DDI_PHSYNC;
	if(t->pvsync)
		f |= TRANS_DDI_PVSYNC;
	wr(TRANS_DDI_FUNC_CTL(p), f);

	/*
	 * watermarks (ilk_wm_method1 with level 0's latency, 0.1 µs units;
	 * two pipes: no LP watermarks)
	 */
	sskpd = rd(MCH_SSKPD) | (uvlong)rd(MCH_SSKPD+4)<<32;
	lat = sskpd>>56 & 0xFF;
	if(lat == 0)
		lat = sskpd & 0xF;
	if(lat == 0)
		lat = 20;
	wm = ((uvlong)t->clock * 4 * lat + 64*10000 - 1) / (64*10000) + 2;
	if(wm > 255)
		wm = 255;
	wr(WM3_LP_ILK, 0);
	wr(WM2_LP_ILK, 0);
	wr(WM1_LP_ILK, 0);
	wr(WM1S_LP_ILK, 0);
	wr(WM0_PIPE_ILK(p), wm<<16);	/* the primary plane; sprite and cursor off */

	/* intel_enable_pipe */
	wr(TRANSCONF(p), rd(TRANSCONF(p)) | TRANSCONF_ENABLE);
	if(intel_wait_for_register(TRANSCONF(p), TRANSCONF_STATE_ENABLE, TRANSCONF_STATE_ENABLE, 100) < 0){
		hsw_crtc_disable(o);
		return "pipe B does not start";
	}

	/* the encoder's enable: the port */
	wr(DDI_BUF_CTL(o->port), (rd(DDI_BUF_CTL(o->port)) & (1<<4)) | DDI_BUF_CTL_ENABLE);

	/* the plane: the frame buffer, linear x8r8g8b8 */
	wr(DSPSTRIDE(p), t->ha*4);
	wr(DSPOFFSET(p), 0);
	wr(DSPCNTR(p), DISP_ENABLE | DISP_FORMAT_BGRX888);
	wr(DSPSURF(p), o->fbgtt);
	o->ours = 1;
	readmode(o);
	o->refresh = (long)t->hz * 1000;
	return nil;
}
