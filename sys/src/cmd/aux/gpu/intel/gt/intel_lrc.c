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
	r[0x01] = Milri | (2*(e->idx == 0 ? 14 : 11) - 1) | Lriposted;
	r[0x02] = b + Rctxctl;
	r[0x03] = masked(Ctxinhibitsyn | Ctxrestoreinhibit | Ctxrsenable, 1);
	r[0x04] = b + Rhead;	r[0x05] = 0;
	r[0x06] = b + Rtail;	r[0x07] = 0;
	r[0x08] = b + Rstart;	r[0x09] = ec->ringgtt;
	r[0x0a] = b + Rctl;	r[0x0b] = (Ringpages-1)<<12 | Ringvalid;
	r[0x0c] = b + Rbbheadu;	r[0x0d] = 0;
	r[0x0e] = b + Rbbheadl;	r[0x0f] = 0;
	r[0x10] = b + Rbbstate;	r[0x11] = 1<<5;
	r[0x12] = b + Rsbbheadu;	r[0x13] = 0;
	r[0x14] = b + Rsbbheadl;	r[0x15] = 0;
	r[0x16] = b + Rsbbstate;	r[0x17] = 0;
	if(e->idx == 0){
		r[0x18] = b + Rbbperctx;	r[0x19] = 0;
		r[0x1a] = b + Rindctx;	r[0x1b] = 0;
		r[0x1c] = b + Rindctxoff;	r[0x1d] = 0;
	}
	r[0x21] = Milri | (2*9 - 1) | Lriposted;
	r[0x22] = b + Rctxtimestamp;	r[0x23] = 0;
	for(n = 3; n >= 0; n--){
		r[0x24 + (3-n)*4] = b + Rpdp + n*8 + 4;
		r[0x26 + (3-n)*4] = b + Rpdp + n*8;
	}
	if(e->idx == 0){
		r[0x41] = Milri | (2*1 - 1);
		r[0x42] = Rpwrclkstate;	r[0x43] = 0;	/* gen 8: no explicit RPCS request (make_rpcs) */
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
	return (uvlong)(ec->lrca>>12)<<32 | ec->lrca | Descvalid | Descl3llc | Descpriv |
		(global || legacy32 ? Desclegacy32 : Desclegacy64);
}
