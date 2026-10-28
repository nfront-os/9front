/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: RC6 and RPS: the GT sleeps, its frequency follows the load.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/gt/intel_rps.c (much of it
 * first written after v4.4); the notices are in NOTICE, UPSTREAM says
 * which functions correspond.
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
rps_limits(int val)
{
	u32int l;

	l = rps.maxsoft << 24;
	if(val <= rps.minsoft)
		l |= rps.minsoft << 16;
	return l;
}

/* the PM interrupts to mask at val (gen6_rps_pm_mask, sanitized for gen 8) */
u32int
rps_pm_mask(int val)
{
	u32int m;

	m = 0;
	if(val > rps.minsoft)
		m |= GEN6_PM_RP_DOWN_EI_EXPIRED | GEN6_PM_RP_DOWN_THRESHOLD |
			GEN6_PM_RP_DOWN_TIMEOUT;
	if(val < rps.maxsoft)
		m |= GEN6_PM_RP_UP_EI_EXPIRED | GEN6_PM_RP_UP_THRESHOLD;
	m &= Pmrpsevents;
	return ~m & ~GEN8_PMINTR_DISABLE_REDIRECT_TO_GUC;
}

void
rps_set_power(int val)
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
	wr(GEN6_RP_UP_EI, gtinterval(eiup));
	wr(GEN6_RP_UP_THRESHOLD, gtinterval(eiup*up/100));
	wr(GEN6_RP_DOWN_EI, gtinterval(eidown));
	wr(GEN6_RP_DOWN_THRESHOLD, gtinterval(eidown*down/100));
	wr(GEN6_RP_CONTROL, (GEN6_RP_MEDIA_TURBO | GEN6_RP_MEDIA_HW_NORMAL_MODE |
		GEN6_RP_MEDIA_IS_GFX | GEN6_RP_ENABLE | GEN6_RP_UP_BUSY_AVG |
		GEN6_RP_DOWN_IDLE_AVG));
	rps.power = power;
	rps.lastadj = 0;
}

void
gen6_rps_set(int val)
{
	if(val != rps.cur){
		rps_set_power(val);
		wr(GEN6_RPNSWREQ, val << 24);	/* HSW_FREQUENCY */
	}
	wr(GEN6_RP_INTERRUPT_LIMITS, rps_limits(val));
	wr(GEN6_PMINTRMSK, rps_pm_mask(val));
	rd(GEN6_RPNSWREQ);
	rps.cur = val;
}

/* the PM interrupts that came (gen6_pm_rps_work); with lk */
void
rps_work(u32int pm)
{
	int val, adj;

	if(!rps.on || (pm & Pmrpsevents) == 0)
		return;
	adj = rps.lastadj;
	val = rps.cur;
	if(pm & GEN6_PM_RP_UP_THRESHOLD){
		adj = adj > 0 ? adj*2 : 1;
		if(val < rps.efficient - adj){	/* below RPe: straight there */
			val = rps.efficient;
			adj = 0;
		}
	}else if(pm & GEN6_PM_RP_DOWN_TIMEOUT){
		val = rps.cur > rps.efficient ? rps.efficient : rps.minsoft;
		adj = 0;
	}else if(pm & GEN6_PM_RP_DOWN_THRESHOLD)
		adj = adj < 0 ? adj*2 : -1;
	else
		adj = 0;
	rps.lastadj = adj;
	val += adj;
	if(val < rps.minsoft)
		val = rps.minsoft;
	if(val > rps.maxsoft)
		val = rps.maxsoft;
	gen6_rps_set(val);
}

/* the GPU went idle: the idle frequency (gen6_rps_idle); with lk */
void
intel_rps_park(void)
{
	if(rps.on && rps.cur > rps.idle)
		gen6_rps_set(rps.idle);
	rps.lastadj = 0;
}

void
intel_rps_enable(void)
{
	u32int cap;

	cap = rd(GEN6_RP_STATE_CAP);
	rps.rp0 = cap & 0xFF;
	rps.rp1 = cap>>8 & 0xFF;
	rps.min = cap>>16 & 0xFF;
	rps.max = rps.rp0;
	rps.efficient = rps.rp1;
	rps.idle = rps.min;
	rps.maxsoft = rps.max;
	rps.minsoft = rps.min;

	wr(GEN6_RPNSWREQ, rps.rp1 << 24);
	wr(GEN6_RC_VIDEO_FREQ, rps.rp1 << 24);
	wr(GEN6_RP_DOWN_TIMEOUT, 100000000/128);	/* 1 s */
	wr(GEN6_RP_INTERRUPT_LIMITS, rps.maxsoft << 24 | rps.minsoft << 16);
	wr(GEN6_RP_UP_THRESHOLD, 7600000/128);
	wr(GEN6_RP_DOWN_THRESHOLD, 31300000/128);
	wr(GEN6_RP_UP_EI, 66000);
	wr(GEN6_RP_DOWN_EI, 350000);
	wr(GEN6_RP_IDLE_HYSTERSIS, 10);
	wr(GEN6_RP_CONTROL, (GEN6_RP_MEDIA_TURBO | GEN6_RP_MEDIA_HW_NORMAL_MODE |
		GEN6_RP_MEDIA_IS_GFX | GEN6_RP_ENABLE | GEN6_RP_UP_BUSY_AVG |
		GEN6_RP_DOWN_IDLE_AVG));

	rps.power = Highpower;		/* forces the thresholds' programming */
	rps.cur = 0;
	rps.on = 1;
	gen6_rps_set(rps.idle);
	rps.pmimr = ~Pmrpsevents;
}
