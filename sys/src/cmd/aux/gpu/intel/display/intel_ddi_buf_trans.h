/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_ddi_buf_trans.h,
 * for aux/intelgpu: Haswell's and Broadwell's entries only.
 */

struct hsw_ddi_buf_trans {
	u32 trans1;	/* balance leg enable, de-emph level */
	u32 trans2;	/* vref sel, vswing */
	u8 i_boost;	/* SKL: I_boost; valid: 0x0, 0x1, 0x3, 0x7 */
};

union intel_ddi_buf_trans_entry {
	struct hsw_ddi_buf_trans hsw;
};

struct intel_ddi_buf_trans {
	const union intel_ddi_buf_trans_entry *entries;
	u8 num_entries;
	u8 hdmi_default_entry;
};

/* aux/intelgpu: Broadwell's table for DP (hdmi 0) or HDMI */
const struct intel_ddi_buf_trans *bdw_trans(int hdmi);
