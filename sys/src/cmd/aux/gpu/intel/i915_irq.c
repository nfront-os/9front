/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: interrupts: the master control, the GT banks (MSI through #G).
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/i915_irq.c (much of it
 * first written after v4.4); the notices are in NOTICE, UPSTREAM says
 * which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

ulong nirq;

void
gen8_irq_postinstall(void)
{
	u32int mask;
	int i;

	mask = 0;
	for(i = 0; i < nelem(eng); i++)
		if(eng[i].ok)
			mask |= (GT_RENDER_USER_INTERRUPT|GT_CONTEXT_SWITCH_INTERRUPT) << eng[i].irqshift;
	wr(GEN8_GT_IIR(0), ~0);		/* IIR */
	wr(GEN8_GT_IER(0), mask);		/* IER */
	wr(GEN8_GT_IMR(0), ~mask);		/* IMR */
	wr(GEN8_GT_IIR(2), ~0);		/* PM: the RPS events */
	wr(GEN8_GT_IER(2), rps.on ? Pmrpsevents : 0);
	wr(GEN8_GT_IMR(2), rps.on ? rps.pmimr : ~0);
	wr(GEN8_MASTER_IRQ, GEN8_MASTER_IRQ_CONTROL);
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
			wr(GEN8_MASTER_IRQ, 0);
			iir = rd(GEN8_GT_IIR(0));
			wr(GEN8_GT_IIR(0), iir);
			pm = rd(GEN8_GT_IIR(2));
			if(pm != 0)
				wr(GEN8_GT_IIR(2), pm);
			wr(GEN8_MASTER_IRQ, GEN8_MASTER_IRQ_CONTROL);
			fprint(gctlfd, "irq ack");
		}else
			sleep(1);
		qlock(&lk);
		nirq++;
		rps_work(pm);
		for(i = 0; i < nelem(eng); i++)
			process_csb(&eng[i]);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
	}
}
