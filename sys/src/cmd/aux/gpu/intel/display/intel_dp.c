/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: DP: a sink there.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/intel_dp.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* a DP sink answers on AUX: its DPCD revision */
int
dpsink(int port)
{
	uchar rev;

	return auxmsg(port, Auxnativeread, 0, &rev, 1) == 0 && rev != 0;
}
