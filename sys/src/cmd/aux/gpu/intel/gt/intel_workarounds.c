/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the render engine's workarounds, in every new context.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_workarounds.c
 * (gen8_ctx_workarounds_init and bdw_ctx_workarounds_init as they are
 * there); the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * the render engine's context workarounds, which the first batch of each
 * new context loads (ecsubmit): as Linux's i915_wa_list, in the order
 * they are added (masked registers: the writes to one register add up)
 */
struct i915_wa_list {
	Wa	*list;
	uint	count;
};

Wa rcswa[Nwa];
int nrcswa;

static void
wa_add(struct i915_wa_list *wal, u32 reg, u32 set)
{
	if(wal->count >= Nwa)
		sysfatal("intel_workarounds: more than %d", Nwa);
	wal->list[wal->count].reg = reg;
	wal->list[wal->count].val = set;
	wal->count++;
}

static void
wa_masked_en(struct i915_wa_list *wal, u32 reg, u32 val)
{
	wa_add(wal, reg, _MASKED_BIT_ENABLE(val));
}

static void
wa_masked_dis(struct i915_wa_list *wal, u32 reg, u32 val)
{
	wa_add(wal, reg, _MASKED_BIT_DISABLE(val));
}

static void
wa_masked_field_set(struct i915_wa_list *wal, u32 reg, u32 mask, u32 val)
{
	wa_add(wal, reg, _MASKED_FIELD(mask, val));
}

/* multicast registers: one write reaches every slice here (gen 8) */
#define wa_mcr_masked_en	wa_masked_en

/* Linux 6.6, gt/intel_workarounds.c: */
static void gen8_ctx_workarounds_init(Engine *engine, struct i915_wa_list *wal)
{
	wa_masked_en(wal, INSTPM, INSTPM_FORCE_ORDERING);

	/* WaDisableAsyncFlipPerfMode:bdw,chv */
	wa_masked_en(wal, RING_MI_MODE(RENDER_RING_BASE), ASYNC_FLIP_PERF_DISABLE);

	/* WaDisablePartialInstShootdown:bdw,chv */
	wa_mcr_masked_en(wal, GEN8_ROW_CHICKEN,
			 PARTIAL_INSTRUCTION_SHOOTDOWN_DISABLE);

	/* Use Force Non-Coherent whenever executing a 3D context. This is a
	 * workaround for a possible hang in the unlikely event a TLB
	 * invalidation occurs during a PSD flush.
	 */
	/* WaForceEnableNonCoherent:bdw,chv */
	/* WaHdcDisableFetchWhenMasked:bdw,chv */
	wa_masked_en(wal, HDC_CHICKEN0,
		     HDC_DONOT_FETCH_MEM_WHEN_MASKED |
		     HDC_FORCE_NON_COHERENT);

	/* From the Haswell PRM, Command Reference: Registers, CACHE_MODE_0:
	 * "The Hierarchical Z RAW Stall Optimization allows non-overlapping
	 *  polygons in the same 8x4 pixel/sample area to be processed without
	 *  stalling waiting for the earlier ones to write to Hierarchical Z
	 *  buffer."
	 *
	 * This optimization is off by default for BDW and CHV; turn it on.
	 */
	wa_masked_dis(wal, CACHE_MODE_0_GEN7, HIZ_RAW_STALL_OPT_DISABLE);

	/* Wa4x4STCOptimizationDisable:bdw,chv */
	wa_masked_en(wal, CACHE_MODE_1, GEN8_4x4_STC_OPTIMIZATION_DISABLE);

	/*
	 * BSpec recommends 8x4 when MSAA is used,
	 * however in practice 16x4 seems fastest.
	 *
	 * Note that PS/WM thread counts depend on the WIZ hashing
	 * disable bit, which we don't touch here, but it's good
	 * to keep in mind (see 3DSTATE_PS and 3DSTATE_WM).
	 */
	wa_masked_field_set(wal, GEN7_GT_MODE,
			    GEN6_WIZ_HASHING_MASK,
			    GEN6_WIZ_HASHING_16x4);
}

static void bdw_ctx_workarounds_init(Engine *engine, struct i915_wa_list *wal)
{
	gen8_ctx_workarounds_init(engine, wal);

	/* WaDisableThreadStallDopClockGating:bdw (pre-production) */
	wa_mcr_masked_en(wal, GEN8_ROW_CHICKEN, STALL_DOP_GATING_DISABLE);

	/* WaDisableDopClockGating:bdw
	 *
	 * Also see the related UCGTCL1 write in bdw_init_clock_gating()
	 * to disable EUTC clock gating.
	 */
	wa_mcr_masked_en(wal, GEN8_ROW_CHICKEN2,
			 DOP_CLOCK_GATING_DISABLE);

	wa_mcr_masked_en(wal, GEN8_HALF_SLICE_CHICKEN3,
			 GEN8_SAMPLER_POWER_BYPASS_DIS);

	wa_masked_en(wal, HDC_CHICKEN0,
		     /* WaForceContextSaveRestoreNonCoherent:bdw */
		     HDC_FORCE_CONTEXT_SAVE_RESTORE_NON_COHERENT |
		     /* WaDisableFenceDestinationToSLM:bdw (pre-prod) */
		     (IS_BROADWELL_GT3(i915) ? HDC_FENCE_DEST_SLM_DISABLE : 0));
}

/* as Linux's intel_engine_init_ctx_wa, for the render engine of Broadwell */
void
intel_engine_init_ctx_wa(Engine *engine)
{
	struct i915_wa_list wal;

	wal.list = rcswa;
	wal.count = 0;
	bdw_ctx_workarounds_init(engine, &wal);
	nrcswa = wal.count;
}
