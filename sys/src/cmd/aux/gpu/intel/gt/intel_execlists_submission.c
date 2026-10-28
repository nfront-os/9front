/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: execlists: ELSP, the context status buffer, the rings.
 *
 * Follows Linux 6.6's
 * drivers/gpu/drm/i915/gt/intel_execlists_submission.c (much of it
 * first written after v4.4); the notices are in NOTICE, UPSTREAM says
 * which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/* Linux 6.6, gt/intel_execlists_submission.c: the context status buffer */
#define GEN8_CTX_STATUS_IDLE_ACTIVE	(1 << 0)
#define GEN8_CTX_STATUS_PREEMPTED	(1 << 1)
#define GEN8_CTX_STATUS_ELEMENT_SWITCH	(1 << 2)
#define GEN8_CTX_STATUS_ACTIVE_IDLE	(1 << 3)
#define GEN8_CTX_STATUS_COMPLETE	(1 << 4)
#define GEN8_CTX_STATUS_LITE_RESTORE	(1 << 15)

/* the next context with work into the idle engine's ELSP; with lk */
void
execlists_dequeue(Engine *e)
{
	Ectx *ec;
	uvlong d;

	if(!e->ok || e->running != nil || (ec = e->queue) == nil)
		return;
	e->queue = ec->next;
	ec->next = nil;
	ec->queued = 0;
	lrc_update_regs(ec);
	d = lrc_descriptor(ec);
	/* both elements, the second (none) first; the first's lower dword starts it */
	wr(RING_ELSP(e->base), 0);
	wr(RING_ELSP(e->base), 0);
	wr(RING_ELSP(e->base), d>>32);
	wr(RING_ELSP(e->base), d);
	rd(RING_EXECLIST_STATUS_LO(e->base));
	e->running = ec;
	ec->subtail = ec->tail;
	e->since = nsec();
}

void
enqueue(Ectx *ec)
{
	Ectx **l;

	if(ec->queued || ec->e->running == ec)
		return;
	for(l = &ec->e->queue; *l != nil; l = &(*l)->next)
		;
	*l = ec;
	ec->next = nil;
	ec->queued = 1;
}

/*
 * the context status buffer (i915's intel_lrc_irq_handler): a context
 * that went idle or was switched out leaves the engine, which takes
 * the next; with work added meanwhile it queues again.  With lk.
 */
void
process_csb(Engine *e)
{
	u32int st, id, w;
	Ectx *ec;
	int r;

	w = rd(RING_CONTEXT_STATUS_PTR(e->base)) & GEN8_CSB_PTR_MASK;
	if(w == GEN8_CSB_PTR_MASK || w >= GEN8_CSB_ENTRIES)
		return;
	r = e->csbread;
	while(r != w){
		r = (r + 1) % GEN8_CSB_ENTRIES;
		st = rd(e->base + GEN8_EXECLISTS_STATUS_BUF + r*8);
		id = rd(e->base + GEN8_EXECLISTS_STATUS_BUF + r*8 + 4);
		DBG(2, "%s csb %d: %#ux %#ux\n", e->name, r, st, id);
		if(st & GEN8_CTX_STATUS_PREEMPTED)
			fprint(2, "%s: %s: context %#ux preempted\n", argv0, e->name, id);
		if((st & (GEN8_CTX_STATUS_COMPLETE|GEN8_CTX_STATUS_ACTIVE_IDLE|GEN8_CTX_STATUS_ELEMENT_SWITCH)) == 0)
			continue;
		ec = e->running;
		if(ec == nil || (ec->lrca>>12) != id)
			continue;
		e->running = nil;
		e->nswitch++;
		if(ec->tail != ec->subtail)
			enqueue(ec);
	}
	e->csbread = r;
	wr(RING_CONTEXT_STATUS_PTR(e->base), (GEN8_CSB_PTR_MASK<<8)<<16 |
		(r & GEN8_CSB_PTR_MASK)<<8);
	execlists_dequeue(e);
}

void
ecemit(Ectx *ec, u32int v)
{
	ec->ring[ec->tail/4] = v;
	ec->tail = (ec->tail + 4) % (Ringpages*4096);
}

/* the context's sequence number done on the engine */
u32int
ecdone(Ectx *ec)
{
	flushcache(ec->seqno, 4);
	return *ec->seqno;
}

/*
 * room for n dwords in the ring: what is done is free, from where the
 * oldest unfinished submission starts; waits (lk released) for the
 * engine to get on
 */
int
ecspace(Ectx *ec, int n)
{
	ulong start, size;
	u32int d;
	vlong t0;

	size = Ringpages*4096;
	t0 = nsec();
	for(;;){
		d = ecdone(ec);
		if(d == ec->nseq)
			return 0;
		start = ec->endpos[d % Fwin];
		if((start + size - ec->tail) % size > (ulong)n*4 + 8)
			return 0;
		if(nsec() - t0 > (vlong)Timeout*1000000)
			return -1;
		rsleep(&done);
	}
}

/*
 * the batch at gpu address a into the context's ring on its engine:
 * the TLBs invalidated if its mapping changed, the batch, the caches
 * flushed, the sequence number into the context's status page, the
 * interrupt; then queued for the engine.  With lk.
 */
char*
ecsubmit(Ectx *ec, uvlong a, u32int *seq)
{
	Engine *e;
	ulong t0;
	uvlong pa;
	int n;

	e = ec->e;
	if(!e->ok)
		return "engine not running";
	if(ecspace(ec, 72 + 2*nrcswa + 2 + 2*6 + 3) < 0)
		return "ring full (engine hung?)";
	t0 = ec->tail;
	if(ec->fresh && e->idx == 0 && !norcsinit){
		/*
		 * a new render context: the workarounds, flushed before and
		 * after, then the null render state, a privileged batch in
		 * the GTT (i915's gen8_init_rcs_context)
		 */
		ecemit(ec, GFX_OP_PIPE_CONTROL(6));
		ecemit(ec, PIPE_CONTROL_CS_STALL |
			PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
			PIPE_CONTROL_DC_FLUSH_ENABLE | PIPE_CONTROL_DEPTH_CACHE_FLUSH |
			PIPE_CONTROL_QW_WRITE | PIPE_CONTROL_GLOBAL_GTT_IVB);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, MI_LOAD_REGISTER_IMM(nrcswa));
		for(n = 0; n < nrcswa; n++){
			ecemit(ec, rcswa[n].reg);
			ecemit(ec, rcswa[n].val);
		}
		ecemit(ec, MI_NOOP);
		ecemit(ec, GFX_OP_PIPE_CONTROL(6));
		ecemit(ec, PIPE_CONTROL_CS_STALL |
			PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
			PIPE_CONTROL_DC_FLUSH_ENABLE | PIPE_CONTROL_DEPTH_CACHE_FLUSH |
			PIPE_CONTROL_QW_WRITE | PIPE_CONTROL_GLOBAL_GTT_IVB);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, MI_BATCH_BUFFER_START_GEN8);
		ecemit(ec, rsgtt);
		ecemit(ec, 0);
	}
	ec->fresh = 0;
	if(!global && ec->serial != ec->ctx->serial){
		if(e->idx == 0){
			ecemit(ec, GFX_OP_PIPE_CONTROL(6));
			ecemit(ec, PIPE_CONTROL_CS_STALL | PIPE_CONTROL_TLB_INVALIDATE |
				PIPE_CONTROL_QW_WRITE | PIPE_CONTROL_GLOBAL_GTT_IVB);
			ecemit(ec, e->hwsgtt + Hwsscratch*4);
			ecemit(ec, 0);
			ecemit(ec, 0);
			ecemit(ec, 0);
		}else{
			ecemit(ec, (MI_FLUSH_DW+1) | MI_INVALIDATE_TLB |
				MI_FLUSH_DW_OP_STOREDW | MI_FLUSH_DW_STORE_INDEX);
			ecemit(ec, Hwsscratch*4 | MI_FLUSH_DW_USE_GTT);
			ecemit(ec, 0);
			ecemit(ec, 0);
		}
		ec->serial = ec->ctx->serial;
	}
	/*
	 * the PDPs from the ring before every batch, as i915's
	 * gen8_emit_bb_start does before the first (pd_dirty_rings): the
	 * blitter did not take them from the image (on the laptop it
	 * walked wrong page tables), and another context may have run on
	 * the engine between two batches of this one
	 */
	if(!global){
		ecemit(ec, MI_LOAD_REGISTER_IMM(8));
		for(n = 3; n >= 0; n--){
			pa = pdpval(ec->ctx, n);
			ecemit(ec, GEN8_RING_PDP_UDW(e->base, n));
			ecemit(ec, pa>>32);
			ecemit(ec, GEN8_RING_PDP_LDW(e->base, n));
			ecemit(ec, pa);
		}
		ecemit(ec, MI_NOOP);
	}
	ecemit(ec, MI_BATCH_BUFFER_START_GEN8 | (global ? 0 : MI_BATCH_PPGTT_HSW));
	ecemit(ec, a);
	ecemit(ec, a>>32);
	if(e->idx == 0){
		ecemit(ec, GFX_OP_PIPE_CONTROL(6));
		ecemit(ec, PIPE_CONTROL_CS_STALL |
			PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH |
			PIPE_CONTROL_DC_FLUSH_ENABLE | PIPE_CONTROL_DEPTH_CACHE_FLUSH |
			PIPE_CONTROL_QW_WRITE | PIPE_CONTROL_GLOBAL_GTT_IVB);
		ecemit(ec, e->hwsgtt + Hwsscratch*4);
		ecemit(ec, 0);
		ecemit(ec, 0);
		ecemit(ec, 0);
	}else{
		ecemit(ec, (MI_FLUSH_DW+1) | MI_FLUSH_DW_OP_STOREDW | MI_FLUSH_DW_STORE_INDEX);
		ecemit(ec, Hwsscratch*4 | MI_FLUSH_DW_USE_GTT);
		ecemit(ec, 0);
		ecemit(ec, 0);
	}
	*seq = ++ec->nseq;
	ecemit(ec, MI_STORE_DWORD_IMM_GEN4 | MI_USE_GGTT);
	ecemit(ec, ec->seqgtt);
	ecemit(ec, 0);
	ecemit(ec, *seq);
	ecemit(ec, MI_USER_INTERRUPT);
	while(ec->tail & 7)
		ecemit(ec, MI_NOOP);
	ec->endpos[*seq % Fwin] = ec->tail;
	if(ec->tail > t0)
		flushcache(ec->ring + t0/4, ec->tail - t0);
	else{
		flushcache(ec->ring + t0/4, Ringpages*4096 - t0);
		flushcache(ec->ring, ec->tail);
	}
	enqueue(ec);
	execlists_dequeue(e);
	return nil;
}
