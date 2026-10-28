/*
 * Copyright © 2006-2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_display.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_DISPLAY_INTEL_DISPLAY_H_
#define _INTELGPU_DISPLAY_INTEL_DISPLAY_H_

enum aux_ch {
	AUX_CH_NONE = -1,

	AUX_CH_A,
	AUX_CH_B,
	AUX_CH_C,
	AUX_CH_D,
	AUX_CH_E, /* ICL+ */
	AUX_CH_F,
	AUX_CH_G,
	AUX_CH_H,
	AUX_CH_I,

	/* tgl+ */
	AUX_CH_USBC1 = AUX_CH_D,
	AUX_CH_USBC2,
	AUX_CH_USBC3,
	AUX_CH_USBC4,
	AUX_CH_USBC5,
	AUX_CH_USBC6,

	/* XE_LPD repositions D/E offsets and bitfields */
	AUX_CH_D_XELPD = AUX_CH_USBC5,
	AUX_CH_E_XELPD,
};

#endif
