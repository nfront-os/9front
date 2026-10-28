/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the render engine's workarounds, in every new context.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_workarounds.c
 * (gen8_ctx_workarounds_init, bdw_ctx_workarounds_init; the list first
 * from v4.4); the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * the render engine's workarounds, written into each new context
 * (i915's gen8_init_workarounds and bdw_init_workarounds, GT2): masked
 * registers, the bits to set in the upper half, their values below
 */
Wa rcswa[11] = {
	0x20C0, 0x00800080,	/* INSTPM: force ordering */
	0x209C, 0x40004000,	/* MI_MODE: async flip perf disable */
	0xE4F0, 0x01000100,	/* GEN8_ROW_CHICKEN: partial instruction shootdown disable */
	0x7300, 0x08100810,	/* HDC_CHICKEN0: don't fetch when masked, force non-coherent */
	0x7000, 0x00040000,	/* CACHE_MODE_0: HiZ RAW stall optimisation on */
	0x7004, 0x00400040,	/* CACHE_MODE_1: 4x4 STC optimisation disable */
	0x7008, 0x02800200,	/* GT_MODE: WIZ hashing 16x4 (field bits 9, 7) */
	0xE4F0, 0x00200020,	/* GEN8_ROW_CHICKEN: stall DOP gating disable */
	0xE4F4, 0x00010001,	/* GEN7_ROW_CHICKEN2: DOP clock gating disable */
	0xE184, 0x00020002,	/* HALF_SLICE_CHICKEN3: sampler power bypass disable */
	0x7300, 0x00200020,	/* HDC_CHICKEN0: context save/restore non-coherent */
};
