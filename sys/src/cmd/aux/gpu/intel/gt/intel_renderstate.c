/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the null render state in the GTT and every context.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_renderstate.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"
#include "intel_renderstate.h"

ulong rsgtt;		/* the null render state (gen8_renderstate.c), at this address in every context too */
uvlong rspa;

/*
 * the null render state (renderstate.h) in the driver's page after
 * the status pages, relocated to its GTT address (i915's
 * render_state_setup)
 */
void
intel_renderstate_init(void)
{
	u32int *d, v;
	int i, r;

	if(gen8_null_state.batch_items*4 > 4096)
		sysfatal("render state larger than a page");
	d = drv + nelem(eng)*1024;
	rsgtt = drvgtt + nelem(eng)*4096;
	rspa = drvpa + nelem(eng)*4096;
	memset(d, 0, 4096);
	r = 0;
	for(i = 0; i < gen8_null_state.batch_items; i++){
		v = gen8_null_state.batch[i];
		if(i*4 == gen8_null_state.reloc[r]){
			if(i+1 >= gen8_null_state.batch_items || gen8_null_state.batch[i+1] != 0)
				sysfatal("render state: bad relocation at %#x", i*4);
			v += global ? rsgtt : Rsva;	/* the state is read through the context's PPGTT */
			r++;
		}
		d[i] = v;
	}
	if(gen8_null_state.reloc[r] != (u32int)-1)
		sysfatal("render state: only %d relocations done", r);
	flushcache(d, 4096);
}
