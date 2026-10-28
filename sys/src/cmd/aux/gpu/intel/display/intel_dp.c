/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: DP: a sink there.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/display/intel_dp.c (much of
 * it first written after v4.4); the notices are in NOTICE, UPSTREAM
 * says which functions correspond.
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

	return intel_dp_aux_transfer(port, DP_AUX_NATIVE_READ, 0, &rev, 1) == 0 && rev != 0;
}
