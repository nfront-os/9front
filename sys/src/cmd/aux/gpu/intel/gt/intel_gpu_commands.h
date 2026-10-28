/* SPDX-License-Identifier: MIT*/
/*
 * Copyright © 2003-2018 Intel Corporation
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/gt/intel_gpu_commands.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_GT_INTEL_GPU_COMMANDS_H_
#define _INTELGPU_GT_INTEL_GPU_COMMANDS_H_

#define INSTR_CLIENT_SHIFT      29
#define   INSTR_MI_CLIENT       0x0
#define __INSTR(client) ((client) << INSTR_CLIENT_SHIFT)
#define MI_INSTR(opcode, flags) \
	(__INSTR(INSTR_MI_CLIENT) | (opcode) << 23 | (flags))
#define MI_NOOP			MI_INSTR(0, 0)
#define MI_USER_INTERRUPT	MI_INSTR(0x02, 0)
#define MI_STORE_DWORD_IMM_GEN4	MI_INSTR(0x20, 2)
#define   MI_USE_GGTT		(1 << 22) /* g4x+ */
#define MI_LOAD_REGISTER_IMM(x)	MI_INSTR(0x22, 2*(x)-1)
#define   MI_LRI_FORCE_POSTED		(1<<12)
#define MI_FLUSH_DW		MI_INSTR(0x26, 1) /* for GEN6 */
#define   MI_FLUSH_DW_STORE_INDEX	(1<<21)
#define   MI_INVALIDATE_TLB		(1<<18)
#define   MI_FLUSH_DW_OP_STOREDW	(1<<14)
#define   MI_FLUSH_DW_USE_GTT		(1<<2)
#define   MI_BATCH_PPGTT_HSW		(1<<8)
#define MI_BATCH_BUFFER_START_GEN8	MI_INSTR(0x31, 1)
#define GFX_OP_PIPE_CONTROL(len)	((0x3<<29)|(0x3<<27)|(0x2<<24)|((len)-2))
#define   PIPE_CONTROL_GLOBAL_GTT_IVB			(1<<24) /* gen7+ */
#define   PIPE_CONTROL_CS_STALL				(1<<20)
#define   PIPE_CONTROL_TLB_INVALIDATE			(1<<18)
#define   PIPE_CONTROL_QW_WRITE				(1<<14)
#define   PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH	(1<<12) /* gen6+ */
#define   PIPE_CONTROL_DC_FLUSH_ENABLE			(1<<5)
#define   PIPE_CONTROL_DEPTH_CACHE_FLUSH		(1<<0)

#endif
