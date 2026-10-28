/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: RC6 and RPS: the GT sleeps, its frequency follows the load.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_rps.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

Rps rps;

/*
 * RC6 and RPS, as i915's gen8_enable_rps, gen6_set_rps and
 * gen6_pm_rps_work (intel_pm.c, i915_irq.c, v4.4) have them on
 * Broadwell: the GT sleeps when idle (RC6, timeout mode, 800 µs),
 * and the frequency follows the load: the GT says when it was busy
 * above or below a threshold over an interval (PM interrupts), the
 * driver steps the frequency it asks for, doubling the step while
 * the load stays on one side.
 */
int
gtinterval(int us)	/* GT_INTERVAL_FROM_US, gen 8: 1.28 µs units */
{
	return us*100 >> 7;
}

u32int
rpslimits(int val)
{
	u32int l;

	l = rps.maxsoft << 24;
	if(val <= rps.minsoft)
		l |= rps.minsoft << 16;
	return l;
}

/* the PM interrupts to mask at val (gen6_rps_pm_mask, sanitized for gen 8) */
u32int
rpspmmask(int val)
{
	u32int m;

	m = 0;
	if(val > rps.minsoft)
		m |= Pmdowneiexpired | Pmdownthreshold | Pmdowntimeout;
	if(val < rps.maxsoft)
		m |= Pmupeiexpired | Pmupthreshold;
	m &= Pmrpsevents;
	return ~m & ~Pmredirect;
}

void
rpsthresholds(int val)
{
	int power, up, down, eiup, eidown;

	power = rps.power;
	switch(rps.power){
	case Lowpower:
		if(val > rps.efficient + 1 && val > rps.cur)
			power = Between;
		break;
	case Between:
		if(val <= rps.efficient && val < rps.cur)
			power = Lowpower;
		else if(val >= rps.rp0 && val > rps.cur)
			power = Highpower;
		break;
	case Highpower:
		if(val < (rps.rp1 + rps.rp0) >> 1 && val < rps.cur)
			power = Between;
		break;
	}
	if(val <= rps.minsoft)
		power = Lowpower;
	if(val >= rps.maxsoft)
		power = Highpower;
	if(power == rps.power)
		return;
	switch(power){
	case Lowpower:	eiup = 16000; up = 95; eidown = 32000; down = 85; break;
	case Between:	eiup = 13000; up = 90; eidown = 32000; down = 75; break;
	default:	eiup = 10000; up = 85; eidown = 32000; down = 60; break;
	}
	wr(Rpupei, gtinterval(eiup));
	wr(Rpupthreshold, gtinterval(eiup*up/100));
	wr(Rpdownei, gtinterval(eidown));
	wr(Rpdownthreshold, gtinterval(eidown*down/100));
	wr(Rpcontrol, Rpctlbits);
	rps.power = power;
	rps.lastadj = 0;
}

void
rpsset(int val)
{
	if(val != rps.cur){
		rpsthresholds(val);
		wr(Rpnswreq, val << 24);	/* HSW_FREQUENCY */
	}
	wr(Rpintlimits, rpslimits(val));
	wr(Pmintrmsk, rpspmmask(val));
	rd(Rpnswreq);
	rps.cur = val;
}

/* the PM interrupts that came (gen6_pm_rps_work); with lk */
void
rpswork(u32int pm)
{
	int val, adj;

	if(!rps.on || (pm & Pmrpsevents) == 0)
		return;
	adj = rps.lastadj;
	val = rps.cur;
	if(pm & Pmupthreshold){
		adj = adj > 0 ? adj*2 : 1;
		if(val < rps.efficient - adj){	/* below RPe: straight there */
			val = rps.efficient;
			adj = 0;
		}
	}else if(pm & Pmdowntimeout){
		val = rps.cur > rps.efficient ? rps.efficient : rps.minsoft;
		adj = 0;
	}else if(pm & Pmdownthreshold)
		adj = adj < 0 ? adj*2 : -1;
	else
		adj = 0;
	rps.lastadj = adj;
	val += adj;
	if(val < rps.minsoft)
		val = rps.minsoft;
	if(val > rps.maxsoft)
		val = rps.maxsoft;
	rpsset(val);
}

/* the GPU went idle: the idle frequency (gen6_rps_idle); with lk */
void
rpsidle(void)
{
	if(rps.on && rps.cur > rps.idle)
		rpsset(rps.idle);
	rps.lastadj = 0;
}

void
rpsinit(void)
{
	u32int cap;
	int i;

	wr(Rcstate, 0);			/* software RC state: RC0 */
	wr(Rccontrol, 0);		/* RC states off while programming */

	cap = rd(Rpstatecap);
	rps.rp0 = cap & 0xFF;
	rps.rp1 = cap>>8 & 0xFF;
	rps.min = cap>>16 & 0xFF;
	rps.max = rps.rp0;
	rps.efficient = rps.rp1;
	rps.idle = rps.min;
	rps.maxsoft = rps.max;
	rps.minsoft = rps.min;

	wr(Rc6wakerate, 40<<16);
	wr(Rcevalint, 125000);		/* 12500 * 1280 ns */
	wr(Rcidlehyst, 25);		/* 25 * 1280 ns */
	for(i = 0; i < nelem(eng); i++)
		wr(eng[i].base + Rmaxidle, 10);
	wr(Rcsleep, 0);
	wr(Rc6threshold, 625);		/* 800 µs / 1.28, timeout mode */
	wr(Rccontrol, Rcctlhw | Rcctlto | Rcctlrc6);

	wr(Rpnswreq, rps.rp1 << 24);
	wr(Rcvideofreq, rps.rp1 << 24);
	wr(Rpdowntimeout, 100000000/128);	/* 1 s */
	wr(Rpintlimits, rps.maxsoft << 24 | rps.minsoft << 16);
	wr(Rpupthreshold, 7600000/128);
	wr(Rpdownthreshold, 31300000/128);
	wr(Rpupei, 66000);
	wr(Rpdownei, 350000);
	wr(Rpidlehyst, 10);
	wr(Rpcontrol, Rpctlbits);

	rps.power = Highpower;		/* forces the thresholds' programming */
	rps.cur = 0;
	rps.on = 1;
	rpsset(rps.idle);
	rps.pmimr = ~Pmrpsevents;
}
