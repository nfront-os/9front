/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: GMBUS: HDMI's DDC.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/display/intel_gmbus.c (much
 * of it first written after v4.4); the notices are in NOTICE, UPSTREAM
 * says which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* GMBUS: wait for status bits, -1 on a NAK or after 50 ms (gmbus_wait_hw_status) */
int
gmbus_wait(u32int bits)
{
	u32int st;
	vlong t;

	for(t = nsec(); nsec() - t < 50*1000000LL; sleep(0)){
		st = rd(GMBUS2(i915));
		if(st & GMBUS_SATOER)
			return -1;
		if(st & bits)
			return 0;
	}
	return -1;
}

void
intel_gmbus_reset(void)
{
	wr(GMBUS1(i915), GMBUS_SW_CLR_INT);
	wr(GMBUS1(i915), 0);
	wr(GMBUS0(i915), 0);
}

/* EDID bytes from offset over GMBUS pin (gmbus_xfer: an index cycle, then the read) */
int
gmedid(int pin, int off, uchar *buf, int n)
{
	u32int v;
	int i, j;
	vlong t;

	wr(GMBUS4(i915), 0);
	wr(GMBUS0(i915), pin);	/* 100 kHz */
	wr(GMBUS1(i915), GMBUS_CYCLE_INDEX | off<<GMBUS_SLAVE_INDEX_SHIFT |
		GMBUS_CYCLE_WAIT | n<<GMBUS_BYTE_COUNT_SHIFT |
		DDC_ADDR<<GMBUS_SLAVE_ADDR_SHIFT | GMBUS_SLAVE_READ | GMBUS_SW_RDY);
	for(i = 0; i < n; ){
		if(gmbus_wait(GMBUS_HW_RDY) < 0){
			intel_gmbus_reset();
			return -1;
		}
		v = rd(GMBUS3(i915));
		for(j = 0; j < 4 && i < n; j++, i++)
			buf[i] = v >> 8*j;
	}
	wr(GMBUS1(i915), GMBUS_CYCLE_STOP | GMBUS_SW_RDY);
	for(t = nsec(); (rd(GMBUS2(i915)) & GMBUS_ACTIVE) && nsec() - t < 10*1000000LL; )
		sleep(0);
	wr(GMBUS0(i915), 0);
	return 0;
}
