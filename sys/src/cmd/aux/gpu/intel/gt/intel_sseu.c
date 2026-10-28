/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: slices, subslices, EUs from the fuses.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/gt/intel_sseu.c (much of it
 * first written after v4.4); the notices are in NOTICE, UPSTREAM says
 * which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

int sslices, ssubslices, seus;	/* sseu: masks of the enabled ones, EUs */

/*
 * the enabled slices, subslices and EUs from the fuses, as i915's
 * broadwell_sseu_info_init (Mesa's getparams SLICE_MASK, SUBSLICE_MASK,
 * EU_TOTAL)
 */
void
bdw_sseu_info_init(void)
{
	u32int fuse2, eud[3], d0, d1, d2;
	int s, ss, n, b;

	fuse2 = rd(GEN8_FUSE2);
	sslices = fuse2>>25 & 7;
	ssubslices = ~(fuse2>>21) & 7;
	d0 = rd(GEN8_EU_DISABLE0);
	d1 = rd(GEN8_EU_DISABLE1);
	d2 = rd(GEN8_EU_DISABLE2);
	eud[0] = d0 & 0xFFFFFF;
	eud[1] = d0>>24 | (d1 & 0xFFFF)<<8;
	eud[2] = d1>>16 | (d2 & 0xFF)<<16;
	seus = 0;
	for(s = 0; s < 3; s++){
		if((sslices & 1<<s) == 0)
			continue;
		for(ss = 0; ss < 3; ss++){
			if((ssubslices & 1<<ss) == 0)
				continue;
			n = 0;
			for(b = 0; b < 8; b++)
				if(eud[s]>>(ss*8+b) & 1)
					n++;
			seus += 8 - n;
		}
	}
}
