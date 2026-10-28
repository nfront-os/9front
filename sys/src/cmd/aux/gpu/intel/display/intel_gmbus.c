/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: GMBUS: HDMI's DDC.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/display/intel_gmbus.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* GMBUS: wait for status bits, -1 on a NAK or after 50 ms (gmbus_wait_hw_status) */
int
gmwait(u32int bits)
{
	u32int st;
	vlong t;

	for(t = nsec(); nsec() - t < 50*1000000LL; sleep(0)){
		st = rd(Gmbus2);
		if(st & Gmsatoer)
			return -1;
		if(st & bits)
			return 0;
	}
	return -1;
}

void
gmreset(void)
{
	wr(Gmbus1, Gmswclrint);
	wr(Gmbus1, 0);
	wr(Gmbus0, 0);
}

/* EDID bytes from offset over GMBUS pin (gmbus_xfer: an index cycle, then the read) */
int
gmedid(int pin, int off, uchar *buf, int n)
{
	u32int v;
	int i, j;
	vlong t;

	wr(Gmbus4, 0);
	wr(Gmbus0, pin);	/* 100 kHz */
	wr(Gmbus1, Gmcycleindex | off<<8 | Gmcyclewait | n<<16 | Ddcaddr<<1 | 1 | Gmswrdy);
	for(i = 0; i < n; ){
		if(gmwait(Gmhwrdy) < 0){
			gmreset();
			return -1;
		}
		v = rd(Gmbus3);
		for(j = 0; j < 4 && i < n; j++, i++)
			buf[i] = v >> 8*j;
	}
	wr(Gmbus1, Gmcyclestop | Gmswrdy);
	for(t = nsec(); (rd(Gmbus2) & Gmactive) && nsec() - t < 10*1000000LL; )
		sleep(0);
	wr(Gmbus0, 0);
	return 0;
}
