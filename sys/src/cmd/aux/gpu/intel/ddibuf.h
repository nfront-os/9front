/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_ddi_buf_trans.c:
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2020 Intel Corporation
 */

/*
 * For aux/intelgpu's display.c: Broadwell's DDI buffer translations
 * (balance legs and de-emphasis, voltage swing) for DP and HDMI.
 * Changed: Ddibuftrans for union intel_ddi_buf_trans_entry's hsw member.
 */

typedef struct Ddibuftrans Ddibuftrans;
struct Ddibuftrans {
	u32int	trans1;	/* balance leg enable, de-emph level */
	u32int	trans2;	/* vref sel, vswing */
	uchar	i_boost;	/* SKL_I_BOOST, unused on BDW */
};

static Ddibuftrans bdw_ddi_translations_dp[] = {
	{ 0x00FFFFFF, 0x0007000E, 0x0 },
	{ 0x00D75FFF, 0x000E000A, 0x0 },
	{ 0x00BEFFFF, 0x00140006, 0x0 },
	{ 0x80B2CFFF, 0x001B0002, 0x0 },
	{ 0x00FFFFFF, 0x000E000A, 0x0 },
	{ 0x00DB6FFF, 0x00160005, 0x0 },
	{ 0x80C71FFF, 0x001A0002, 0x0 },
	{ 0x00F7DFFF, 0x00180004, 0x0 },
	{ 0x80D75FFF, 0x001B0002, 0x0 },
};

static Ddibuftrans bdw_ddi_translations_hdmi[] = {
							/* Idx	NT mV d	T mV df	db	*/
	{ 0x00FFFFFF, 0x0007000E, 0x0 },	/* 0:	400	400	0	*/
	{ 0x00D75FFF, 0x000E000A, 0x0 },	/* 1:	400	600	3.5	*/
	{ 0x00BEFFFF, 0x00140006, 0x0 },	/* 2:	400	800	6	*/
	{ 0x00FFFFFF, 0x0009000D, 0x0 },	/* 3:	450	450	0	*/
	{ 0x00FFFFFF, 0x000E000A, 0x0 },	/* 4:	600	600	0	*/
	{ 0x00D7FFFF, 0x00140006, 0x0 },	/* 5:	600	800	2.5	*/
	{ 0x80CB2FFF, 0x001B0002, 0x0 },	/* 6:	600	1000	4.5	*/
	{ 0x00FFFFFF, 0x00140006, 0x0 },	/* 7:	800	800	0	*/
	{ 0x80E79FFF, 0x001B0002, 0x0 },	/* 8:	800	1000	2	*/
	{ 0x80FFFFFF, 0x001B0002, 0x0 },	/* 9:	1000	1000	0	*/
};
