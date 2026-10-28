/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the CD clock.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/intel_cdclk.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

int cdclk;		/* kHz */

/* broadwell_get_display_clock_speed */
int
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
