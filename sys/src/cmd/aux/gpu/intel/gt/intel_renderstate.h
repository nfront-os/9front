/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2014 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_renderstate.h, for
 * aux/intelgpu: the part gen8_renderstate.c uses.
 */

struct intel_renderstate_rodata {
	const u32 *reloc;
	const u32 *batch;
	const u32 batch_items;
};

#define RO_RENDERSTATE(_g)						\
	const struct intel_renderstate_rodata gen ## _g ## _null_state = { \
		.reloc = gen ## _g ## _null_state_relocs,		\
		.batch = gen ## _g ## _null_state_batch,		\
		.batch_items = sizeof(gen ## _g ## _null_state_batch)/4, \
	}

extern const struct intel_renderstate_rodata gen8_null_state;
