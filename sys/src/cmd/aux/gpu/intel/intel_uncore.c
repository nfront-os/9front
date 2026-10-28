/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: MMIO with forcewake on demand, waiting for register bits.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/intel_uncore.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

/* forcewake taken on demand */
Lock fwlk;
int fwheld;
vlong fwlast;

/*
 * MMIO; the GT's registers (below Fwrange) with forcewake, taken on
 * demand and dropped Fwidle ms after the last access (fwproc), as
 * i915's intel_uncore.c does: held, the GT never sleeps (RC6). The
 * access is made under fwlk, so the release cannot come between.
 */
void
fwon(void)
{
	if(!fwheld){
		if(forcewake() < 0)
			fprint(2, "intelgpu: forcewake: no ack\n");
		fwheld = 1;
	}
	fwlast = nsec();
}

u32int
rd(ulong r)
{
	u32int v;

	if(r >= Fwrange)
		return ((volatile u32int*)mmio)[r/4];
	lock(&fwlk);
	fwon();
	v = ((volatile u32int*)mmio)[r/4];
	unlock(&fwlk);
	return v;
}

void
wr(ulong r, u32int v)
{
	if(r >= Fwrange){
		((volatile u32int*)mmio)[r/4] = v;
		return;
	}
	lock(&fwlk);
	fwon();
	((volatile u32int*)mmio)[r/4] = v;
	unlock(&fwlk);
}

u32int
masked(u32int bits, int on)
{
	return bits<<16 | (on ? bits : 0);
}

/* the kernel's forcewake bit, and its ack (no sleep: under fwlk) */
int
forcewake(void)
{
	vlong t;

	((volatile u32int*)mmio)[FORCEWAKE_MT/4] = masked(1, 1);
	for(t = nsec(); nsec() - t < 50*1000000LL; )
		if(((volatile u32int*)mmio)[FORCEWAKE_ACK_HSW/4] & 1)
			return 0;
	return -1;
}

/* forcewake off once nothing used it for Fwidle ms */
void
fwproc(void)
{
	for(;;){
		sleep(Fwidle);
		lock(&fwlk);
		if(fwheld && !nopm && nsec() - fwlast > Fwidle*1000000LL){
			((volatile u32int*)mmio)[FORCEWAKE_MT/4] = masked(1, 0);
			fwheld = 0;
		}
		unlock(&fwlk);
	}
}

int
intel_wait_for_register(ulong r, u32int mask, u32int want, int ms)
{
	vlong t;

	for(t = nsec(); nsec() - t < ms*1000000LL; sleep(1))
		if((rd(r) & mask) == want)
			return 0;
	return -1;
}
