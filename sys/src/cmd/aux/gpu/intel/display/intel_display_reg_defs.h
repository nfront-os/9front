/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2022 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/display/intel_display_reg_defs.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_DISPLAY_INTEL_DISPLAY_REG_DEFS_H_
#define _INTELGPU_DISPLAY_INTEL_DISPLAY_REG_DEFS_H_

#define _PIPE(pipe, a, b)		_PICK_EVEN(pipe, a, b)
#define _TRANS(tran, a, b)		_PICK_EVEN(tran, a, b)
#define _PORT(port, a, b)		_PICK_EVEN(port, a, b)
#define _MMIO_PIPE(pipe, a, b)		_MMIO(_PIPE(pipe, a, b))
#define _MMIO_TRANS(tran, a, b)		_MMIO(_TRANS(tran, a, b))
#define _MMIO_PORT(port, a, b)		_MMIO(_PORT(port, a, b))
#define _MMIO_PIPE3(pipe, a, b, c)	_MMIO(_PICK_EVEN_2RANGES(pipe, 1, a, a, b, c))
#define _MMIO_PIPE2(pipe, reg)		_MMIO(DISPLAY_INFO(dev_priv)->pipe_offsets[(pipe)] - \
					      DISPLAY_INFO(dev_priv)->pipe_offsets[PIPE_A] + \
					      DISPLAY_MMIO_BASE(dev_priv) + (reg))
#define _MMIO_TRANS2(tran, reg)		_MMIO(DISPLAY_INFO(dev_priv)->trans_offsets[(tran)] - \
					      DISPLAY_INFO(dev_priv)->trans_offsets[TRANSCODER_A] + \
					      DISPLAY_MMIO_BASE(dev_priv) + (reg))

#endif
