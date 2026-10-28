/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the CD clock.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/display/intel_cdclk.c (much
 * of it first written after v4.4); the notices are in NOTICE, UPSTREAM
 * says which functions correspond.
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
bdw_get_cdclk(void)
{
	u32int l;

	l = rd(LCPLL_CTL);
	if(l & 1<<21)
		return 800000;
	if(rd(FUSE_STRAP) & 1<<24)
		return 450000;
	switch(l>>26 & 3){
	case 0:	return 450000;
	case 1:	return 540000;
	case 2:	return 337500;
	}
	return 675000;
}
