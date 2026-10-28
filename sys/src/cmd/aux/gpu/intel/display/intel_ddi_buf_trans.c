// SPDX-License-Identifier: MIT
/*
 * Copyright © 2020 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_ddi_buf_trans.c,
 * for aux/intelgpu: Broadwell's DDI buffer translations for DP and
 * HDMI; bdw_trans is ours, for what bdw_get_buf_trans chooses there.
 */

#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"
#include "intel_ddi_buf_trans.h"

static const union intel_ddi_buf_trans_entry _bdw_trans_dp[] = {
	{ .hsw = { 0x00FFFFFF, 0x0007000E, 0x0 } },
	{ .hsw = { 0x00D75FFF, 0x000E000A, 0x0 } },
	{ .hsw = { 0x00BEFFFF, 0x00140006, 0x0 } },
	{ .hsw = { 0x80B2CFFF, 0x001B0002, 0x0 } },
	{ .hsw = { 0x00FFFFFF, 0x000E000A, 0x0 } },
	{ .hsw = { 0x00DB6FFF, 0x00160005, 0x0 } },
	{ .hsw = { 0x80C71FFF, 0x001A0002, 0x0 } },
	{ .hsw = { 0x00F7DFFF, 0x00180004, 0x0 } },
	{ .hsw = { 0x80D75FFF, 0x001B0002, 0x0 } },
};

static const struct intel_ddi_buf_trans bdw_trans_dp = {
	.entries = _bdw_trans_dp,
	.num_entries = ARRAY_SIZE(_bdw_trans_dp),
};

static const union intel_ddi_buf_trans_entry _bdw_trans_hdmi[] = {
							/* Idx	NT mV d	T mV df	db	*/
	{ .hsw = { 0x00FFFFFF, 0x0007000E, 0x0 } },	/* 0:	400	400	0	*/
	{ .hsw = { 0x00D75FFF, 0x000E000A, 0x0 } },	/* 1:	400	600	3.5	*/
	{ .hsw = { 0x00BEFFFF, 0x00140006, 0x0 } },	/* 2:	400	800	6	*/
	{ .hsw = { 0x00FFFFFF, 0x0009000D, 0x0 } },	/* 3:	450	450	0	*/
	{ .hsw = { 0x00FFFFFF, 0x000E000A, 0x0 } },	/* 4:	600	600	0	*/
	{ .hsw = { 0x00D7FFFF, 0x00140006, 0x0 } },	/* 5:	600	800	2.5	*/
	{ .hsw = { 0x80CB2FFF, 0x001B0002, 0x0 } },	/* 6:	600	1000	4.5	*/
	{ .hsw = { 0x00FFFFFF, 0x00140006, 0x0 } },	/* 7:	800	800	0	*/
	{ .hsw = { 0x80E79FFF, 0x001B0002, 0x0 } },	/* 8:	800	1000	2	*/
	{ .hsw = { 0x80FFFFFF, 0x001B0002, 0x0 } },	/* 9:	1000	1000	0	*/
};

static const struct intel_ddi_buf_trans bdw_trans_hdmi = {
	.entries = _bdw_trans_hdmi,
	.num_entries = ARRAY_SIZE(_bdw_trans_hdmi),
	.hdmi_default_entry = 7,
};

const struct intel_ddi_buf_trans *
bdw_trans(int hdmi)
{
	return hdmi ? &bdw_trans_hdmi : &bdw_trans_dp;
}
