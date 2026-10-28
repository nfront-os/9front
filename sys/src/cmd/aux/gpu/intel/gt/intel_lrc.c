/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the logical ring context: its image, its descriptor.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_lrc.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * a context's image for an engine, as i915's populate_lr_context: the
 * per-process status page, then the register state the engine loads
 * (MI_LOAD_REGISTER_IMM lists); the first load restores nothing else
 * (restore inhibit), the engine writes the rest at the switches
 */
void
ecimage(Ectx *ec)
{
	Engine *e;
	u32int *r;
	ulong b;
	int n;

	e = ec->e;
	memset(ec->image, 0, (1+e->statepages)*4096);
	r = ec->image + 1024;
	b = e->base;
	r[0x01] = MI_LOAD_REGISTER_IMM(e->idx == 0 ? 14 : 11) | MI_LRI_FORCE_POSTED;
	r[0x02] = RING_CONTEXT_CONTROL(b);
	r[0x03] = masked(CTX_CTRL_INHIBIT_SYN_CTX_SWITCH | CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT | CTX_CTRL_RS_CTX_ENABLE, 1);
	r[0x04] = RING_HEAD(b);	r[0x05] = 0;
	r[0x06] = RING_TAIL(b);	r[0x07] = 0;
	r[0x08] = RING_START(b);	r[0x09] = ec->ringgtt;
	r[0x0a] = RING_CTL(b);	r[0x0b] = (Ringpages-1)<<12 | RING_VALID;
	r[0x0c] = RING_BBADDR_UDW(b);	r[0x0d] = 0;
	r[0x0e] = RING_BBADDR(b);	r[0x0f] = 0;
	r[0x10] = RING_BBSTATE(b);	r[0x11] = 1<<5;
	r[0x12] = RING_SBBADDR_UDW(b);	r[0x13] = 0;
	r[0x14] = RING_SBBADDR(b);	r[0x15] = 0;
	r[0x16] = RING_SBBSTATE(b);	r[0x17] = 0;
	if(e->idx == 0){
		r[0x18] = RING_BB_PER_CTX_PTR(b);	r[0x19] = 0;
		r[0x1a] = RING_INDIRECT_CTX(b);	r[0x1b] = 0;
		r[0x1c] = RING_INDIRECT_CTX_OFFSET(b);	r[0x1d] = 0;
	}
	r[0x21] = MI_LOAD_REGISTER_IMM(9) | MI_LRI_FORCE_POSTED;
	r[0x22] = RING_CTX_TIMESTAMP(b);	r[0x23] = 0;
	for(n = 3; n >= 0; n--){
		r[0x24 + (3-n)*4] = GEN8_RING_PDP_UDW(b, n);
		r[0x26 + (3-n)*4] = GEN8_RING_PDP_LDW(b, n);
	}
	if(e->idx == 0){
		r[0x41] = MI_LOAD_REGISTER_IMM(1);
		r[0x42] = GEN8_R_PWR_CLK_STATE(RENDER_RING_BASE);	r[0x43] = 0;	/* gen 8: no explicit RPCS request (make_rpcs) */
	}
	memset(ec->ring, 0, Ringpages*4096);
	flushcache(ec->image, (1+e->statepages)*4096);
	flushcache(ec->ring, Ringpages*4096);
	ec->tail = ec->subtail = 0;
	memset(ec->endpos, 0, sizeof ec->endpos);
}

void
ecinit(Ctx *c, Ectx *ec, Engine *e, ulong off)
{
	ec->ctx = c;
	ec->e = e;
	ec->image = c->mem + off/4;
	ec->lrca = c->gtt + off;
	ec->ring = c->mem + (off + (1+e->statepages)*4096)/4;
	ec->ringgtt = c->gtt + off + (1+e->statepages)*4096;
	ec->seqno = c->mem + e->idx*16;
	ec->seqgtt = c->gtt + e->idx*64;
	*ec->seqno = 0;
	ec->nseq = 0;
	ec->serial = 0;
	ec->fresh = 1;
	ec->queued = 0;
	ec->next = nil;
	ecimage(ec);
}

/* the ring's tail and the address space into the image, before the engine loads it */
void
ecupdate(Ectx *ec)
{
	u32int *r;
	uvlong pa;
	int n;

	r = ec->image + 1024;
	r[0x07] = ec->tail;
	r[0x09] = ec->ringgtt;
	for(n = 3; n >= 0; n--){
		pa = pdpval(ec->ctx, n);
		r[0x25 + (3-n)*4] = pa>>32;
		r[0x27 + (3-n)*4] = pa;
	}
	flushcache(r, 0x50*4);
}

uvlong
descriptor(Ectx *ec)
{
	return (uvlong)(ec->lrca>>12)<<32 | ec->lrca | GEN8_CTX_VALID | GEN8_CTX_L3LLC_COHERENT | GEN8_CTX_PRIVILEGE |
		(global || legacy32 ? (INTEL_LEGACY_32B_CONTEXT << GEN8_CTX_ADDRESSING_MODE_SHIFT) : (INTEL_LEGACY_64B_CONTEXT << GEN8_CTX_ADDRESSING_MODE_SHIFT));
}
