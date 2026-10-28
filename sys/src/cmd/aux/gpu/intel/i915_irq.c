/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: interrupts: the master control, the GT banks (MSI through #G).
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/i915_irq.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

ulong nirq;

void
irqinit(void)
{
	u32int mask;
	int i;

	mask = 0;
	for(i = 0; i < nelem(eng); i++)
		if(eng[i].ok)
			mask |= (Userintr|Ctxswitchintr) << eng[i].irqshift;
	wr(Gtisr0+8, ~0);		/* IIR */
	wr(Gtisr0+0xC, mask);		/* IER */
	wr(Gtisr0+4, ~mask);		/* IMR */
	wr(Gtisr2+8, ~0);		/* PM: the RPS events */
	wr(Gtisr2+0xC, rps.on ? Pmrpsevents : 0);
	wr(Gtisr2+4, rps.on ? rps.pmimr : ~0);
	wr(Masterirq, Mastercontrol);
}

void
irqproc(void)
{
	char buf[32];
	u32int iir, pm;
	int i;

	for(;;){
		pm = 0;
		if(irqfd >= 0){
			if(read(irqfd, buf, sizeof buf) <= 0)
				sysfatal("read irq: %r");
			wr(Masterirq, 0);
			iir = rd(Gtisr0+8);
			wr(Gtisr0+8, iir);
			pm = rd(Gtisr2+8);
			if(pm != 0)
				wr(Gtisr2+8, pm);
			wr(Masterirq, Mastercontrol);
			fprint(gctlfd, "irq ack");
		}else
			sleep(1);
		qlock(&lk);
		nirq++;
		rpswork(pm);
		for(i = 0; i < nelem(eng); i++)
			csb(&eng[i]);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}
