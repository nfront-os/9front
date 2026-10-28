/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: I2C over DP AUX.
 *
 * Follows Linux 6.6 drivers/gpu/drm/display/drm_dp_helper.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* an I2C-over-AUX message with retries on defer (drm_dp_i2c_do_msg) */
int
auxi2c(int port, int req, uchar *buf, int size)
{
	int try, r;

	for(try = 0; try < 16; try++){
		if((r = auxmsg(port, req, DDC_ADDR, buf, size)) < 0)
			return -1;
		if((r & DP_AUX_NATIVE_REPLY_MASK) == 2 || (r & DP_AUX_I2C_REPLY_MASK) == 2<<2){	/* defer */
			microwait(500);
			continue;
		}
		if(r != 0)
			return -1;
		return 0;
	}
	return -1;
}
