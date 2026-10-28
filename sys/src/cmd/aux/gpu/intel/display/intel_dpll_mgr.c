/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: WRPLL: dividers for a pixel clock.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/intel_dpll_mgr.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

enum { Lcfreq = 2700, Pmin = 2, Pmax = 64, Pinc = 2, Refmin = 48, Refmax = 400, Vcomin = 2400, Vcomax = 4800 };

/* hsw_wrpll_get_budget_for_freq: ppm the WRPLL may be off for a clock (Hz) */
uint
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

uvlong
absdiff(uvlong a, uvlong b)
{
	return a > b ? a - b : b - a;
}

/* hsw_wrpll_update_rnp */
void
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
u32int
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
