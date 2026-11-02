/* Copyright (C) 2020 Google, Inc.
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
 */


#ifndef _RADV_TRACEPOINTS_H
#define _RADV_TRACEPOINTS_H


#include "util/perf/u_trace.h"

#ifdef __cplusplus
extern "C" {
#endif


enum radv_gpu_tracepoint {
   RADV_GPU_TRACEPOINT_CMD_BUFFER = 1ull << 0,
   RADV_GPU_TRACEPOINT_RENDERING = 1ull << 1,
   RADV_GPU_TRACEPOINT_SECONDARY = 1ull << 2,
   RADV_GPU_TRACEPOINT_INTERNAL = 1ull << 3,
   RADV_GPU_TRACEPOINT_DRAW = 1ull << 4,
   RADV_GPU_TRACEPOINT_DISPATCH = 1ull << 5,
   RADV_GPU_TRACEPOINT_BUFFER = 1ull << 6,
   RADV_GPU_TRACEPOINT_IMAGE = 1ull << 7,
   RADV_GPU_TRACEPOINT_QUERY = 1ull << 8,
   RADV_GPU_TRACEPOINT_DGC = 1ull << 9,
   RADV_GPU_TRACEPOINT_ACCEL_STRUCT = 1ull << 10,
   RADV_GPU_TRACEPOINT_TRACE_RAYS = 1ull << 11,
   RADV_GPU_TRACEPOINT_SYNC = 1ull << 12,
};

extern uint64_t radv_gpu_tracepoint;

void radv_gpu_tracepoint_config_variable(void);


/*
 * radv_begin_cmd_buffer
 */
struct trace_radv_begin_cmd_buffer {
   uint8_t dummy;
};
void __trace_radv_begin_cmd_buffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_cmd_buffer(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_CMD_BUFFER)))
      return;
   __trace_radv_begin_cmd_buffer(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_cmd_buffer
 */
struct trace_radv_end_cmd_buffer {
   uint8_t dummy;
};
void __trace_radv_end_cmd_buffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_cmd_buffer(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_CMD_BUFFER)))
      return;
   __trace_radv_end_cmd_buffer(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_rendering
 */
struct trace_radv_begin_rendering {
   uint8_t dummy;
};
void __trace_radv_begin_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_RENDERING)))
      return;
   __trace_radv_begin_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_rendering
 */
struct trace_radv_end_rendering {
   uint8_t dummy;
};
void __trace_radv_end_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_RENDERING)))
      return;
   __trace_radv_end_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_secondary
 */
struct trace_radv_begin_secondary {
   uint8_t dummy;
};
void __trace_radv_begin_secondary(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_secondary(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SECONDARY)))
      return;
   __trace_radv_begin_secondary(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_secondary
 */
struct trace_radv_end_secondary {
   uint8_t dummy;
};
void __trace_radv_end_secondary(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_secondary(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SECONDARY)))
      return;
   __trace_radv_end_secondary(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_image_transition
 */
struct trace_radv_begin_image_transition {
   uint8_t dummy;
};
void __trace_radv_begin_image_transition(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_image_transition(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_image_transition(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_image_transition
 */
struct trace_radv_end_image_transition {
   uint8_t dummy;
};
void __trace_radv_end_image_transition(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_image_transition(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_image_transition(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_copy_vrs_htile
 */
struct trace_radv_begin_copy_vrs_htile {
   uint8_t dummy;
};
void __trace_radv_begin_copy_vrs_htile(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_copy_vrs_htile(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_copy_vrs_htile(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_copy_vrs_htile
 */
struct trace_radv_end_copy_vrs_htile {
   uint8_t dummy;
};
void __trace_radv_end_copy_vrs_htile(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_copy_vrs_htile(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_copy_vrs_htile(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_clear_rendering
 */
struct trace_radv_begin_clear_rendering {
   uint8_t dummy;
};
void __trace_radv_begin_clear_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_clear_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_clear_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_clear_rendering
 */
struct trace_radv_end_clear_rendering {
   uint8_t dummy;
};
void __trace_radv_end_clear_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_clear_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_clear_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_resolve_rendering
 */
struct trace_radv_begin_resolve_rendering {
   uint8_t dummy;
};
void __trace_radv_begin_resolve_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_resolve_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_resolve_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_resolve_rendering
 */
struct trace_radv_end_resolve_rendering {
   uint8_t dummy;
};
void __trace_radv_end_resolve_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_resolve_rendering(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_resolve_rendering(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_compute_copy_memory
 */
struct trace_radv_begin_compute_copy_memory {
   uint32_t size;
};
void __trace_radv_begin_compute_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
);
static ALWAYS_INLINE void trace_radv_begin_compute_copy_memory(
     struct u_trace *ut
   , void *cs
   , uint32_t size
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_compute_copy_memory(
        ut
      , enabled_traces
      , cs
      , size
   );
}

/*
 * radv_end_compute_copy_memory
 */
struct trace_radv_end_compute_copy_memory {
   uint8_t dummy;
};
void __trace_radv_end_compute_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_compute_copy_memory(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_compute_copy_memory(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_cp_dma_copy_memory
 */
struct trace_radv_begin_cp_dma_copy_memory {
   uint32_t size;
};
void __trace_radv_begin_cp_dma_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
);
static ALWAYS_INLINE void trace_radv_begin_cp_dma_copy_memory(
     struct u_trace *ut
   , void *cs
   , uint32_t size
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_cp_dma_copy_memory(
        ut
      , enabled_traces
      , cs
      , size
   );
}

/*
 * radv_end_cp_dma_copy_memory
 */
struct trace_radv_end_cp_dma_copy_memory {
   uint8_t dummy;
};
void __trace_radv_end_cp_dma_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_cp_dma_copy_memory(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_cp_dma_copy_memory(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_leaves
 */
struct trace_radv_begin_leaves {
   uint8_t dummy;
};
void __trace_radv_begin_leaves(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_leaves(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_leaves(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_leaves
 */
struct trace_radv_end_leaves {
   uint8_t dummy;
};
void __trace_radv_end_leaves(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_leaves(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_leaves(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_morton_generate
 */
struct trace_radv_begin_morton_generate {
   uint8_t dummy;
};
void __trace_radv_begin_morton_generate(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_morton_generate(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_morton_generate(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_morton_generate
 */
struct trace_radv_end_morton_generate {
   uint8_t dummy;
};
void __trace_radv_end_morton_generate(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_morton_generate(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_morton_generate(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_morton_sort
 */
struct trace_radv_begin_morton_sort {
   uint8_t dummy;
};
void __trace_radv_begin_morton_sort(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_morton_sort(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_morton_sort(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_morton_sort
 */
struct trace_radv_end_morton_sort {
   uint8_t dummy;
};
void __trace_radv_end_morton_sort(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_morton_sort(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_morton_sort(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_lbvh_main
 */
struct trace_radv_begin_lbvh_main {
   uint8_t dummy;
};
void __trace_radv_begin_lbvh_main(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_lbvh_main(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_lbvh_main(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_lbvh_main
 */
struct trace_radv_end_lbvh_main {
   uint8_t dummy;
};
void __trace_radv_end_lbvh_main(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_lbvh_main(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_lbvh_main(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_lbvh_generate_ir
 */
struct trace_radv_begin_lbvh_generate_ir {
   uint8_t dummy;
};
void __trace_radv_begin_lbvh_generate_ir(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_lbvh_generate_ir(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_lbvh_generate_ir(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_lbvh_generate_ir
 */
struct trace_radv_end_lbvh_generate_ir {
   uint8_t dummy;
};
void __trace_radv_end_lbvh_generate_ir(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_lbvh_generate_ir(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_lbvh_generate_ir(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ploc
 */
struct trace_radv_begin_ploc {
   uint8_t dummy;
};
void __trace_radv_begin_ploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ploc(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_ploc(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ploc
 */
struct trace_radv_end_ploc {
   uint8_t dummy;
};
void __trace_radv_end_ploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ploc(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_ploc(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_hploc
 */
struct trace_radv_begin_hploc {
   uint8_t dummy;
};
void __trace_radv_begin_hploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_hploc(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_hploc(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_hploc
 */
struct trace_radv_end_hploc {
   uint8_t dummy;
};
void __trace_radv_end_hploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_hploc(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_hploc(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_update_as
 */
struct trace_radv_begin_update_as {
   uint32_t build_flags;
};
void __trace_radv_begin_update_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
);
static ALWAYS_INLINE void trace_radv_begin_update_as(
     struct u_trace *ut
   , void *cs
   , uint32_t build_flags
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_update_as(
        ut
      , enabled_traces
      , cs
      , build_flags
   );
}

/*
 * radv_end_update_as
 */
struct trace_radv_end_update_as {
   uint8_t dummy;
};
void __trace_radv_end_update_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_update_as(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_update_as(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_encode_as
 */
struct trace_radv_begin_encode_as {
   uint32_t build_flags;
};
void __trace_radv_begin_encode_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
);
static ALWAYS_INLINE void trace_radv_begin_encode_as(
     struct u_trace *ut
   , void *cs
   , uint32_t build_flags
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_encode_as(
        ut
      , enabled_traces
      , cs
      , build_flags
   );
}

/*
 * radv_end_encode_as
 */
struct trace_radv_end_encode_as {
   uint8_t dummy;
};
void __trace_radv_end_encode_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_encode_as(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_encode_as(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_encode_triangles
 */
struct trace_radv_begin_encode_triangles {
   uint32_t build_flags;
};
void __trace_radv_begin_encode_triangles(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
);
static ALWAYS_INLINE void trace_radv_begin_encode_triangles(
     struct u_trace *ut
   , void *cs
   , uint32_t build_flags
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_encode_triangles(
        ut
      , enabled_traces
      , cs
      , build_flags
   );
}

/*
 * radv_end_encode_triangles
 */
struct trace_radv_end_encode_triangles {
   uint8_t dummy;
};
void __trace_radv_end_encode_triangles(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_encode_triangles(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_encode_triangles(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_encode_triangles_retry
 */
struct trace_radv_begin_encode_triangles_retry {
   uint32_t build_flags;
};
void __trace_radv_begin_encode_triangles_retry(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
);
static ALWAYS_INLINE void trace_radv_begin_encode_triangles_retry(
     struct u_trace *ut
   , void *cs
   , uint32_t build_flags
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_encode_triangles_retry(
        ut
      , enabled_traces
      , cs
      , build_flags
   );
}

/*
 * radv_end_encode_triangles_retry
 */
struct trace_radv_end_encode_triangles_retry {
   uint8_t dummy;
};
void __trace_radv_end_encode_triangles_retry(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_encode_triangles_retry(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_encode_triangles_retry(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_init_header
 */
struct trace_radv_begin_init_header {
   uint32_t build_flags;
};
void __trace_radv_begin_init_header(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
);
static ALWAYS_INLINE void trace_radv_begin_init_header(
     struct u_trace *ut
   , void *cs
   , uint32_t build_flags
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_begin_init_header(
        ut
      , enabled_traces
      , cs
      , build_flags
   );
}

/*
 * radv_end_init_header
 */
struct trace_radv_end_init_header {
   uint8_t dummy;
};
void __trace_radv_end_init_header(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_init_header(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_INTERNAL)))
      return;
   __trace_radv_end_init_header(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_Draw
 */
struct trace_radv_begin_Draw {
   uint32_t vertex_count;
   uint32_t instance_count;
};
void __trace_radv_begin_Draw(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t vertex_count
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_Draw(
     struct u_trace *ut
   , void *cs
   , uint32_t vertex_count
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_Draw(
        ut
      , enabled_traces
      , cs
      , vertex_count
      , instance_count
   );
}

/*
 * radv_end_Draw
 */
struct trace_radv_end_Draw {
   uint8_t dummy;
};
void __trace_radv_end_Draw(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_Draw(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_Draw(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndexed
 */
struct trace_radv_begin_DrawIndexed {
   uint32_t index_count;
   uint32_t instance_count;
};
void __trace_radv_begin_DrawIndexed(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t index_count
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndexed(
     struct u_trace *ut
   , void *cs
   , uint32_t index_count
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndexed(
        ut
      , enabled_traces
      , cs
      , index_count
      , instance_count
   );
}

/*
 * radv_end_DrawIndexed
 */
struct trace_radv_end_DrawIndexed {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndexed(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndexed(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndexed(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMultiEXT
 */
struct trace_radv_begin_DrawMultiEXT {
   uint32_t draw_count;
   uint32_t instance_count;
};
void __trace_radv_begin_DrawMultiEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMultiEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMultiEXT(
        ut
      , enabled_traces
      , cs
      , draw_count
      , instance_count
   );
}

/*
 * radv_end_DrawMultiEXT
 */
struct trace_radv_end_DrawMultiEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMultiEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMultiEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMultiEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMultiIndexedEXT
 */
struct trace_radv_begin_DrawMultiIndexedEXT {
   uint32_t draw_count;
   uint32_t instance_count;
};
void __trace_radv_begin_DrawMultiIndexedEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMultiIndexedEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMultiIndexedEXT(
        ut
      , enabled_traces
      , cs
      , draw_count
      , instance_count
   );
}

/*
 * radv_end_DrawMultiIndexedEXT
 */
struct trace_radv_end_DrawMultiIndexedEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMultiIndexedEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMultiIndexedEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMultiIndexedEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirect
 */
struct trace_radv_begin_DrawIndirect {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirect(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirect(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawIndirect
 */
struct trace_radv_end_DrawIndirect {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirect(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirect(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndexedIndirect
 */
struct trace_radv_begin_DrawIndexedIndirect {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawIndexedIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndexedIndirect(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndexedIndirect(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawIndexedIndirect
 */
struct trace_radv_end_DrawIndexedIndirect {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndexedIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndexedIndirect(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndexedIndirect(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DispatchIndirect
 */
struct trace_radv_begin_DispatchIndirect {
   uint8_t dummy;
};
void __trace_radv_begin_DispatchIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_DispatchIndirect(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_begin_DispatchIndirect(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_DispatchIndirect
 */
struct trace_radv_end_DispatchIndirect {
   uint8_t dummy;
};
void __trace_radv_end_DispatchIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DispatchIndirect(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_end_DispatchIndirect(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyMemoryIndirectKHR
 */
struct trace_radv_begin_CopyMemoryIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_CopyMemoryIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyMemoryIndirectKHR
 */
struct trace_radv_end_CopyMemoryIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_CopyMemoryIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyMemoryToImageIndirectKHR
 */
struct trace_radv_begin_CopyMemoryToImageIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyMemoryToImageIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyMemoryToImageIndirectKHR
 */
struct trace_radv_end_CopyMemoryToImageIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyMemoryToImageIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_UpdateBuffer
 */
struct trace_radv_begin_UpdateBuffer {
   uint32_t size;
};
void __trace_radv_begin_UpdateBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
);
static ALWAYS_INLINE void trace_radv_begin_UpdateBuffer(
     struct u_trace *ut
   , void *cs
   , uint32_t size
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_UpdateBuffer(
        ut
      , enabled_traces
      , cs
      , size
   );
}

/*
 * radv_end_UpdateBuffer
 */
struct trace_radv_end_UpdateBuffer {
   uint8_t dummy;
};
void __trace_radv_end_UpdateBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_UpdateBuffer(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_UpdateBuffer(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_FillBuffer
 */
struct trace_radv_begin_FillBuffer {
   uint32_t size;
};
void __trace_radv_begin_FillBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
);
static ALWAYS_INLINE void trace_radv_begin_FillBuffer(
     struct u_trace *ut
   , void *cs
   , uint32_t size
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_FillBuffer(
        ut
      , enabled_traces
      , cs
      , size
   );
}

/*
 * radv_end_FillBuffer
 */
struct trace_radv_end_FillBuffer {
   uint8_t dummy;
};
void __trace_radv_end_FillBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_FillBuffer(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_FillBuffer(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ClearColorImage
 */
struct trace_radv_begin_ClearColorImage {
   uint8_t dummy;
};
void __trace_radv_begin_ClearColorImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ClearColorImage(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_ClearColorImage(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ClearColorImage
 */
struct trace_radv_end_ClearColorImage {
   uint8_t dummy;
};
void __trace_radv_end_ClearColorImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ClearColorImage(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_ClearColorImage(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ClearDepthStencilImage
 */
struct trace_radv_begin_ClearDepthStencilImage {
   uint8_t dummy;
};
void __trace_radv_begin_ClearDepthStencilImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ClearDepthStencilImage(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_ClearDepthStencilImage(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ClearDepthStencilImage
 */
struct trace_radv_end_ClearDepthStencilImage {
   uint8_t dummy;
};
void __trace_radv_end_ClearDepthStencilImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ClearDepthStencilImage(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_ClearDepthStencilImage(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ClearAttachments
 */
struct trace_radv_begin_ClearAttachments {
   uint8_t dummy;
};
void __trace_radv_begin_ClearAttachments(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ClearAttachments(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_ClearAttachments(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ClearAttachments
 */
struct trace_radv_end_ClearAttachments {
   uint8_t dummy;
};
void __trace_radv_end_ClearAttachments(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ClearAttachments(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_ClearAttachments(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ResetQueryPool
 */
struct trace_radv_begin_ResetQueryPool {
   uint8_t dummy;
};
void __trace_radv_begin_ResetQueryPool(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ResetQueryPool(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_begin_ResetQueryPool(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ResetQueryPool
 */
struct trace_radv_end_ResetQueryPool {
   uint8_t dummy;
};
void __trace_radv_end_ResetQueryPool(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ResetQueryPool(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_end_ResetQueryPool(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyQueryPoolResults
 */
struct trace_radv_begin_CopyQueryPoolResults {
   uint8_t dummy;
};
void __trace_radv_begin_CopyQueryPoolResults(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyQueryPoolResults(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_begin_CopyQueryPoolResults(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyQueryPoolResults
 */
struct trace_radv_end_CopyQueryPoolResults {
   uint8_t dummy;
};
void __trace_radv_end_CopyQueryPoolResults(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyQueryPoolResults(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_end_CopyQueryPoolResults(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ExecuteGeneratedCommandsEXT
 */
struct trace_radv_begin_ExecuteGeneratedCommandsEXT {
   uint8_t dummy;
};
void __trace_radv_begin_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DGC)))
      return;
   __trace_radv_begin_ExecuteGeneratedCommandsEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ExecuteGeneratedCommandsEXT
 */
struct trace_radv_end_ExecuteGeneratedCommandsEXT {
   uint8_t dummy;
};
void __trace_radv_end_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DGC)))
      return;
   __trace_radv_end_ExecuteGeneratedCommandsEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_PreprocessGeneratedCommandsEXT
 */
struct trace_radv_begin_PreprocessGeneratedCommandsEXT {
   uint8_t dummy;
};
void __trace_radv_begin_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DGC)))
      return;
   __trace_radv_begin_PreprocessGeneratedCommandsEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_PreprocessGeneratedCommandsEXT
 */
struct trace_radv_end_PreprocessGeneratedCommandsEXT {
   uint8_t dummy;
};
void __trace_radv_end_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DGC)))
      return;
   __trace_radv_end_PreprocessGeneratedCommandsEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DispatchBase
 */
struct trace_radv_begin_DispatchBase {
   uint32_t base_x;
   uint32_t base_y;
   uint32_t base_z;
   uint32_t x;
   uint32_t y;
   uint32_t z;
};
void __trace_radv_begin_DispatchBase(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t base_x
   , uint32_t base_y
   , uint32_t base_z
   , uint32_t x
   , uint32_t y
   , uint32_t z
);
static ALWAYS_INLINE void trace_radv_begin_DispatchBase(
     struct u_trace *ut
   , void *cs
   , uint32_t base_x
   , uint32_t base_y
   , uint32_t base_z
   , uint32_t x
   , uint32_t y
   , uint32_t z
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_begin_DispatchBase(
        ut
      , enabled_traces
      , cs
      , base_x
      , base_y
      , base_z
      , x
      , y
      , z
   );
}

/*
 * radv_end_DispatchBase
 */
struct trace_radv_end_DispatchBase {
   uint8_t dummy;
};
void __trace_radv_end_DispatchBase(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DispatchBase(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_end_DispatchBase(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirectCount
 */
struct trace_radv_begin_DrawIndirectCount {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirectCount(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirectCount(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawIndirectCount
 */
struct trace_radv_end_DrawIndirectCount {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirectCount(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirectCount(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndexedIndirectCount
 */
struct trace_radv_begin_DrawIndexedIndirectCount {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawIndexedIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndexedIndirectCount(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndexedIndirectCount(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawIndexedIndirectCount
 */
struct trace_radv_end_DrawIndexedIndirectCount {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndexedIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndexedIndirectCount(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndexedIndirectCount(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirectByteCountEXT
 */
struct trace_radv_begin_DrawIndirectByteCountEXT {
   uint32_t instance_count;
};
void __trace_radv_begin_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirectByteCountEXT(
        ut
      , enabled_traces
      , cs
      , instance_count
   );
}

/*
 * radv_end_DrawIndirectByteCountEXT
 */
struct trace_radv_end_DrawIndirectByteCountEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirectByteCountEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMeshTasksEXT
 */
struct trace_radv_begin_DrawMeshTasksEXT {
   uint32_t x;
   uint32_t y;
   uint32_t z;
};
void __trace_radv_begin_DrawMeshTasksEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t x
   , uint32_t y
   , uint32_t z
);
static ALWAYS_INLINE void trace_radv_begin_DrawMeshTasksEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t x
   , uint32_t y
   , uint32_t z
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMeshTasksEXT(
        ut
      , enabled_traces
      , cs
      , x
      , y
      , z
   );
}

/*
 * radv_end_DrawMeshTasksEXT
 */
struct trace_radv_end_DrawMeshTasksEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMeshTasksEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMeshTasksEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMeshTasksEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMeshTasksIndirectEXT
 */
struct trace_radv_begin_DrawMeshTasksIndirectEXT {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMeshTasksIndirectEXT(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawMeshTasksIndirectEXT
 */
struct trace_radv_end_DrawMeshTasksIndirectEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMeshTasksIndirectEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMeshTasksIndirectCountEXT
 */
struct trace_radv_begin_DrawMeshTasksIndirectCountEXT {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMeshTasksIndirectCountEXT(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawMeshTasksIndirectCountEXT
 */
struct trace_radv_end_DrawMeshTasksIndirectCountEXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMeshTasksIndirectCountEXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyAccelerationStructureKHR
 */
struct trace_radv_begin_CopyAccelerationStructureKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_begin_CopyAccelerationStructureKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyAccelerationStructureKHR
 */
struct trace_radv_end_CopyAccelerationStructureKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_end_CopyAccelerationStructureKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyAccelerationStructureToMemoryKHR
 */
struct trace_radv_begin_CopyAccelerationStructureToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_begin_CopyAccelerationStructureToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyAccelerationStructureToMemoryKHR
 */
struct trace_radv_end_CopyAccelerationStructureToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_end_CopyAccelerationStructureToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyMemoryToAccelerationStructureKHR
 */
struct trace_radv_begin_CopyMemoryToAccelerationStructureKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_begin_CopyMemoryToAccelerationStructureKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyMemoryToAccelerationStructureKHR
 */
struct trace_radv_end_CopyMemoryToAccelerationStructureKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_end_CopyMemoryToAccelerationStructureKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_TraceRaysKHR
 */
struct trace_radv_begin_TraceRaysKHR {
   uint32_t width;
   uint32_t height;
   uint32_t depth;
};
void __trace_radv_begin_TraceRaysKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t width
   , uint32_t height
   , uint32_t depth
);
static ALWAYS_INLINE void trace_radv_begin_TraceRaysKHR(
     struct u_trace *ut
   , void *cs
   , uint32_t width
   , uint32_t height
   , uint32_t depth
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_begin_TraceRaysKHR(
        ut
      , enabled_traces
      , cs
      , width
      , height
      , depth
   );
}

/*
 * radv_end_TraceRaysKHR
 */
struct trace_radv_end_TraceRaysKHR {
   uint8_t dummy;
};
void __trace_radv_end_TraceRaysKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_TraceRaysKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_end_TraceRaysKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_TraceRaysIndirectKHR
 */
struct trace_radv_begin_TraceRaysIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_begin_TraceRaysIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_TraceRaysIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_begin_TraceRaysIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_TraceRaysIndirectKHR
 */
struct trace_radv_end_TraceRaysIndirectKHR {
   uint8_t dummy;
};
void __trace_radv_end_TraceRaysIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_TraceRaysIndirectKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_end_TraceRaysIndirectKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_TraceRaysIndirect2KHR
 */
struct trace_radv_begin_TraceRaysIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_begin_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_begin_TraceRaysIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_TraceRaysIndirect2KHR
 */
struct trace_radv_end_TraceRaysIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_end_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_TRACE_RAYS)))
      return;
   __trace_radv_end_TraceRaysIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_BuildAccelerationStructuresKHR
 */
struct trace_radv_begin_BuildAccelerationStructuresKHR {
   uint8_t dummy;
};
void __trace_radv_begin_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_begin_BuildAccelerationStructuresKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_BuildAccelerationStructuresKHR
 */
struct trace_radv_end_BuildAccelerationStructuresKHR {
   uint8_t dummy;
};
void __trace_radv_end_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_ACCEL_STRUCT)))
      return;
   __trace_radv_end_BuildAccelerationStructuresKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyBuffer2
 */
struct trace_radv_begin_CopyBuffer2 {
   uint8_t dummy;
};
void __trace_radv_begin_CopyBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyBuffer2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_CopyBuffer2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyBuffer2
 */
struct trace_radv_end_CopyBuffer2 {
   uint8_t dummy;
};
void __trace_radv_end_CopyBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyBuffer2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_CopyBuffer2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyImage2
 */
struct trace_radv_begin_CopyImage2 {
   uint8_t dummy;
};
void __trace_radv_begin_CopyImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyImage2
 */
struct trace_radv_end_CopyImage2 {
   uint8_t dummy;
};
void __trace_radv_end_CopyImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_BlitImage2
 */
struct trace_radv_begin_BlitImage2 {
   uint8_t dummy;
};
void __trace_radv_begin_BlitImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_BlitImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_BlitImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_BlitImage2
 */
struct trace_radv_end_BlitImage2 {
   uint8_t dummy;
};
void __trace_radv_end_BlitImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_BlitImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_BlitImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyBufferToImage2
 */
struct trace_radv_begin_CopyBufferToImage2 {
   uint8_t dummy;
};
void __trace_radv_begin_CopyBufferToImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyBufferToImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyBufferToImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyBufferToImage2
 */
struct trace_radv_end_CopyBufferToImage2 {
   uint8_t dummy;
};
void __trace_radv_end_CopyBufferToImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyBufferToImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyBufferToImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyImageToBuffer2
 */
struct trace_radv_begin_CopyImageToBuffer2 {
   uint8_t dummy;
};
void __trace_radv_begin_CopyImageToBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyImageToBuffer2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyImageToBuffer2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyImageToBuffer2
 */
struct trace_radv_end_CopyImageToBuffer2 {
   uint8_t dummy;
};
void __trace_radv_end_CopyImageToBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyImageToBuffer2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyImageToBuffer2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ResolveImage2
 */
struct trace_radv_begin_ResolveImage2 {
   uint8_t dummy;
};
void __trace_radv_begin_ResolveImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ResolveImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_ResolveImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ResolveImage2
 */
struct trace_radv_end_ResolveImage2 {
   uint8_t dummy;
};
void __trace_radv_end_ResolveImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ResolveImage2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_ResolveImage2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_SetEvent2
 */
struct trace_radv_begin_SetEvent2 {
   uint8_t dummy;
};
void __trace_radv_begin_SetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_SetEvent2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_begin_SetEvent2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_SetEvent2
 */
struct trace_radv_end_SetEvent2 {
   uint8_t dummy;
};
void __trace_radv_end_SetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_SetEvent2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_end_SetEvent2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_ResetEvent2
 */
struct trace_radv_begin_ResetEvent2 {
   uint8_t dummy;
};
void __trace_radv_begin_ResetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_ResetEvent2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_begin_ResetEvent2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_ResetEvent2
 */
struct trace_radv_end_ResetEvent2 {
   uint8_t dummy;
};
void __trace_radv_end_ResetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_ResetEvent2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_end_ResetEvent2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_WaitEvents2
 */
struct trace_radv_begin_WaitEvents2 {
   uint8_t dummy;
};
void __trace_radv_begin_WaitEvents2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_WaitEvents2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_begin_WaitEvents2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_WaitEvents2
 */
struct trace_radv_end_WaitEvents2 {
   uint8_t dummy;
};
void __trace_radv_end_WaitEvents2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_WaitEvents2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_end_WaitEvents2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_PipelineBarrier2
 */
struct trace_radv_begin_PipelineBarrier2 {
   uint8_t dummy;
};
void __trace_radv_begin_PipelineBarrier2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_PipelineBarrier2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_begin_PipelineBarrier2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_PipelineBarrier2
 */
struct trace_radv_end_PipelineBarrier2 {
   uint8_t dummy;
};
void __trace_radv_end_PipelineBarrier2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_PipelineBarrier2(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_SYNC)))
      return;
   __trace_radv_end_PipelineBarrier2(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyMemoryKHR
 */
struct trace_radv_begin_CopyMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_CopyMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyMemoryKHR
 */
struct trace_radv_end_CopyMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_CopyMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyMemoryToImageKHR
 */
struct trace_radv_begin_CopyMemoryToImageKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyMemoryToImageKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyMemoryToImageKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyMemoryToImageKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyMemoryToImageKHR
 */
struct trace_radv_end_CopyMemoryToImageKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyMemoryToImageKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyMemoryToImageKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyMemoryToImageKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyImageToMemoryKHR
 */
struct trace_radv_begin_CopyImageToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyImageToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyImageToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_begin_CopyImageToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyImageToMemoryKHR
 */
struct trace_radv_end_CopyImageToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyImageToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyImageToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_IMAGE)))
      return;
   __trace_radv_end_CopyImageToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_UpdateMemoryKHR
 */
struct trace_radv_begin_UpdateMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_UpdateMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_UpdateMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_UpdateMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_UpdateMemoryKHR
 */
struct trace_radv_end_UpdateMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_UpdateMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_UpdateMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_UpdateMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_FillMemoryKHR
 */
struct trace_radv_begin_FillMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_FillMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_FillMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_begin_FillMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_FillMemoryKHR
 */
struct trace_radv_end_FillMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_FillMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_FillMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_BUFFER)))
      return;
   __trace_radv_end_FillMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_CopyQueryPoolResultsToMemoryKHR
 */
struct trace_radv_begin_CopyQueryPoolResultsToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_CopyQueryPoolResultsToMemoryKHR
 */
struct trace_radv_end_CopyQueryPoolResultsToMemoryKHR {
   uint8_t dummy;
};
void __trace_radv_end_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_QUERY)))
      return;
   __trace_radv_end_CopyQueryPoolResultsToMemoryKHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirectByteCount2EXT
 */
struct trace_radv_begin_DrawIndirectByteCount2EXT {
   uint32_t instance_count;
};
void __trace_radv_begin_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t instance_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , void *cs
   , uint32_t instance_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirectByteCount2EXT(
        ut
      , enabled_traces
      , cs
      , instance_count
   );
}

/*
 * radv_end_DrawIndirectByteCount2EXT
 */
struct trace_radv_end_DrawIndirectByteCount2EXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirectByteCount2EXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirect2KHR
 */
struct trace_radv_begin_DrawIndirect2KHR {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirect2KHR(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirect2KHR(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawIndirect2KHR
 */
struct trace_radv_end_DrawIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndexedIndirect2KHR
 */
struct trace_radv_begin_DrawIndexedIndirect2KHR {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndexedIndirect2KHR(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawIndexedIndirect2KHR
 */
struct trace_radv_end_DrawIndexedIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndexedIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndirectCount2KHR
 */
struct trace_radv_begin_DrawIndirectCount2KHR {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndirectCount2KHR(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndirectCount2KHR(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawIndirectCount2KHR
 */
struct trace_radv_end_DrawIndirectCount2KHR {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndirectCount2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndirectCount2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawIndexedIndirectCount2KHR
 */
struct trace_radv_begin_DrawIndexedIndirectCount2KHR {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawIndexedIndirectCount2KHR(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawIndexedIndirectCount2KHR
 */
struct trace_radv_end_DrawIndexedIndirectCount2KHR {
   uint8_t dummy;
};
void __trace_radv_end_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawIndexedIndirectCount2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMeshTasksIndirect2EXT
 */
struct trace_radv_begin_DrawMeshTasksIndirect2EXT {
   uint32_t draw_count;
};
void __trace_radv_begin_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , void *cs
   , uint32_t draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMeshTasksIndirect2EXT(
        ut
      , enabled_traces
      , cs
      , draw_count
   );
}

/*
 * radv_end_DrawMeshTasksIndirect2EXT
 */
struct trace_radv_end_DrawMeshTasksIndirect2EXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMeshTasksIndirect2EXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DrawMeshTasksIndirectCount2EXT
 */
struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT {
   uint32_t max_draw_count;
};
void __trace_radv_begin_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
);
static ALWAYS_INLINE void trace_radv_begin_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , void *cs
   , uint32_t max_draw_count
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_begin_DrawMeshTasksIndirectCount2EXT(
        ut
      , enabled_traces
      , cs
      , max_draw_count
   );
}

/*
 * radv_end_DrawMeshTasksIndirectCount2EXT
 */
struct trace_radv_end_DrawMeshTasksIndirectCount2EXT {
   uint8_t dummy;
};
void __trace_radv_end_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DRAW)))
      return;
   __trace_radv_end_DrawMeshTasksIndirectCount2EXT(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_begin_DispatchIndirect2KHR
 */
struct trace_radv_begin_DispatchIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_begin_DispatchIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_begin_DispatchIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_begin_DispatchIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

/*
 * radv_end_DispatchIndirect2KHR
 */
struct trace_radv_end_DispatchIndirect2KHR {
   uint8_t dummy;
};
void __trace_radv_end_DispatchIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
);
static ALWAYS_INLINE void trace_radv_end_DispatchIndirect2KHR(
     struct u_trace *ut
   , void *cs
) {
   enum u_trace_type enabled_traces = p_atomic_read_relaxed(&ut->utctx->enabled_traces);
   if (!unlikely(enabled_traces != 0 &&
                 (radv_gpu_tracepoint & RADV_GPU_TRACEPOINT_DISPATCH)))
      return;
   __trace_radv_end_DispatchIndirect2KHR(
        ut
      , enabled_traces
      , cs
   );
}

#ifdef __cplusplus
}
#endif

#include "radv_cmd_buffer.h"

static inline void
radv_utrace_begin_cmd_buffer(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_cmd_buffer(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_cmd_buffer(...)
#endif

static inline void
radv_utrace_end_cmd_buffer(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_cmd_buffer(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_cmd_buffer(...)
#endif

static inline void
radv_utrace_begin_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_rendering(...)
#endif

static inline void
radv_utrace_end_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_rendering(...)
#endif

static inline void
radv_utrace_begin_secondary(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_secondary(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_secondary(...)
#endif

static inline void
radv_utrace_end_secondary(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_secondary(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_secondary(...)
#endif

static inline void
radv_utrace_begin_image_transition(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_image_transition(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_image_transition(...)
#endif

static inline void
radv_utrace_end_image_transition(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_image_transition(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_image_transition(...)
#endif

static inline void
radv_utrace_begin_copy_vrs_htile(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_copy_vrs_htile(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_copy_vrs_htile(...)
#endif

static inline void
radv_utrace_end_copy_vrs_htile(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_copy_vrs_htile(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_copy_vrs_htile(...)
#endif

static inline void
radv_utrace_begin_clear_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_clear_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_clear_rendering(...)
#endif

static inline void
radv_utrace_end_clear_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_clear_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_clear_rendering(...)
#endif

static inline void
radv_utrace_begin_resolve_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_resolve_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_resolve_rendering(...)
#endif

static inline void
radv_utrace_end_resolve_rendering(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_resolve_rendering(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_resolve_rendering(...)
#endif

static inline void
radv_utrace_begin_compute_copy_memory(struct radv_cmd_buffer *cmd_buffer, uint32_t size)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_compute_copy_memory(cmd_buffer->utrace.trace, cmd_buffer, size);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_compute_copy_memory(...)
#endif

static inline void
radv_utrace_end_compute_copy_memory(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_compute_copy_memory(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_compute_copy_memory(...)
#endif

static inline void
radv_utrace_begin_cp_dma_copy_memory(struct radv_cmd_buffer *cmd_buffer, uint32_t size)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_cp_dma_copy_memory(cmd_buffer->utrace.trace, cmd_buffer, size);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_cp_dma_copy_memory(...)
#endif

static inline void
radv_utrace_end_cp_dma_copy_memory(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_cp_dma_copy_memory(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_cp_dma_copy_memory(...)
#endif

static inline void
radv_utrace_begin_leaves(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_leaves(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_leaves(...)
#endif

static inline void
radv_utrace_end_leaves(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_leaves(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_leaves(...)
#endif

static inline void
radv_utrace_begin_morton_generate(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_morton_generate(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_morton_generate(...)
#endif

static inline void
radv_utrace_end_morton_generate(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_morton_generate(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_morton_generate(...)
#endif

static inline void
radv_utrace_begin_morton_sort(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_morton_sort(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_morton_sort(...)
#endif

static inline void
radv_utrace_end_morton_sort(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_morton_sort(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_morton_sort(...)
#endif

static inline void
radv_utrace_begin_lbvh_main(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_lbvh_main(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_lbvh_main(...)
#endif

static inline void
radv_utrace_end_lbvh_main(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_lbvh_main(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_lbvh_main(...)
#endif

static inline void
radv_utrace_begin_lbvh_generate_ir(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_lbvh_generate_ir(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_lbvh_generate_ir(...)
#endif

static inline void
radv_utrace_end_lbvh_generate_ir(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_lbvh_generate_ir(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_lbvh_generate_ir(...)
#endif

static inline void
radv_utrace_begin_ploc(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ploc(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ploc(...)
#endif

static inline void
radv_utrace_end_ploc(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ploc(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ploc(...)
#endif

static inline void
radv_utrace_begin_hploc(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_hploc(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_hploc(...)
#endif

static inline void
radv_utrace_end_hploc(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_hploc(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_hploc(...)
#endif

static inline void
radv_utrace_begin_update_as(struct radv_cmd_buffer *cmd_buffer, uint32_t build_flags)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_update_as(cmd_buffer->utrace.trace, cmd_buffer, build_flags);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_update_as(...)
#endif

static inline void
radv_utrace_end_update_as(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_update_as(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_update_as(...)
#endif

static inline void
radv_utrace_begin_encode_as(struct radv_cmd_buffer *cmd_buffer, uint32_t build_flags)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_encode_as(cmd_buffer->utrace.trace, cmd_buffer, build_flags);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_encode_as(...)
#endif

static inline void
radv_utrace_end_encode_as(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_encode_as(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_encode_as(...)
#endif

static inline void
radv_utrace_begin_encode_triangles(struct radv_cmd_buffer *cmd_buffer, uint32_t build_flags)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_encode_triangles(cmd_buffer->utrace.trace, cmd_buffer, build_flags);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_encode_triangles(...)
#endif

static inline void
radv_utrace_end_encode_triangles(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_encode_triangles(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_encode_triangles(...)
#endif

static inline void
radv_utrace_begin_encode_triangles_retry(struct radv_cmd_buffer *cmd_buffer, uint32_t build_flags)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_encode_triangles_retry(cmd_buffer->utrace.trace, cmd_buffer, build_flags);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_encode_triangles_retry(...)
#endif

static inline void
radv_utrace_end_encode_triangles_retry(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_encode_triangles_retry(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_encode_triangles_retry(...)
#endif

static inline void
radv_utrace_begin_init_header(struct radv_cmd_buffer *cmd_buffer, uint32_t build_flags)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_init_header(cmd_buffer->utrace.trace, cmd_buffer, build_flags);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_init_header(...)
#endif

static inline void
radv_utrace_end_init_header(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_init_header(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_init_header(...)
#endif

static inline void
radv_utrace_begin_Draw(struct radv_cmd_buffer *cmd_buffer, uint32_t vertex_count, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_Draw(cmd_buffer->utrace.trace, cmd_buffer, vertex_count, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_Draw(...)
#endif

static inline void
radv_utrace_end_Draw(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_Draw(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_Draw(...)
#endif

static inline void
radv_utrace_begin_DrawIndexed(struct radv_cmd_buffer *cmd_buffer, uint32_t index_count, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndexed(cmd_buffer->utrace.trace, cmd_buffer, index_count, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndexed(...)
#endif

static inline void
radv_utrace_end_DrawIndexed(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndexed(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndexed(...)
#endif

static inline void
radv_utrace_begin_DrawMultiEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMultiEXT(cmd_buffer->utrace.trace, cmd_buffer, draw_count, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMultiEXT(...)
#endif

static inline void
radv_utrace_end_DrawMultiEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMultiEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMultiEXT(...)
#endif

static inline void
radv_utrace_begin_DrawMultiIndexedEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMultiIndexedEXT(cmd_buffer->utrace.trace, cmd_buffer, draw_count, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMultiIndexedEXT(...)
#endif

static inline void
radv_utrace_end_DrawMultiIndexedEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMultiIndexedEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMultiIndexedEXT(...)
#endif

static inline void
radv_utrace_begin_DrawIndirect(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirect(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirect(...)
#endif

static inline void
radv_utrace_end_DrawIndirect(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirect(...)
#endif

static inline void
radv_utrace_begin_DrawIndexedIndirect(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndexedIndirect(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndexedIndirect(...)
#endif

static inline void
radv_utrace_end_DrawIndexedIndirect(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndexedIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndexedIndirect(...)
#endif

static inline void
radv_utrace_begin_DispatchIndirect(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DispatchIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DispatchIndirect(...)
#endif

static inline void
radv_utrace_end_DispatchIndirect(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DispatchIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DispatchIndirect(...)
#endif

static inline void
radv_utrace_begin_CopyMemoryIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyMemoryIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyMemoryIndirectKHR(...)
#endif

static inline void
radv_utrace_end_CopyMemoryIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyMemoryIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyMemoryIndirectKHR(...)
#endif

static inline void
radv_utrace_begin_CopyMemoryToImageIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyMemoryToImageIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyMemoryToImageIndirectKHR(...)
#endif

static inline void
radv_utrace_end_CopyMemoryToImageIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyMemoryToImageIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyMemoryToImageIndirectKHR(...)
#endif

static inline void
radv_utrace_begin_UpdateBuffer(struct radv_cmd_buffer *cmd_buffer, uint32_t size)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_UpdateBuffer(cmd_buffer->utrace.trace, cmd_buffer, size);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_UpdateBuffer(...)
#endif

static inline void
radv_utrace_end_UpdateBuffer(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_UpdateBuffer(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_UpdateBuffer(...)
#endif

static inline void
radv_utrace_begin_FillBuffer(struct radv_cmd_buffer *cmd_buffer, uint32_t size)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_FillBuffer(cmd_buffer->utrace.trace, cmd_buffer, size);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_FillBuffer(...)
#endif

static inline void
radv_utrace_end_FillBuffer(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_FillBuffer(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_FillBuffer(...)
#endif

static inline void
radv_utrace_begin_ClearColorImage(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ClearColorImage(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ClearColorImage(...)
#endif

static inline void
radv_utrace_end_ClearColorImage(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ClearColorImage(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ClearColorImage(...)
#endif

static inline void
radv_utrace_begin_ClearDepthStencilImage(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ClearDepthStencilImage(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ClearDepthStencilImage(...)
#endif

static inline void
radv_utrace_end_ClearDepthStencilImage(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ClearDepthStencilImage(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ClearDepthStencilImage(...)
#endif

static inline void
radv_utrace_begin_ClearAttachments(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ClearAttachments(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ClearAttachments(...)
#endif

static inline void
radv_utrace_end_ClearAttachments(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ClearAttachments(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ClearAttachments(...)
#endif

static inline void
radv_utrace_begin_ResetQueryPool(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ResetQueryPool(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ResetQueryPool(...)
#endif

static inline void
radv_utrace_end_ResetQueryPool(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ResetQueryPool(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ResetQueryPool(...)
#endif

static inline void
radv_utrace_begin_CopyQueryPoolResults(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyQueryPoolResults(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyQueryPoolResults(...)
#endif

static inline void
radv_utrace_end_CopyQueryPoolResults(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyQueryPoolResults(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyQueryPoolResults(...)
#endif

static inline void
radv_utrace_begin_ExecuteGeneratedCommandsEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ExecuteGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ExecuteGeneratedCommandsEXT(...)
#endif

static inline void
radv_utrace_end_ExecuteGeneratedCommandsEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ExecuteGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ExecuteGeneratedCommandsEXT(...)
#endif

static inline void
radv_utrace_begin_PreprocessGeneratedCommandsEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_PreprocessGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_PreprocessGeneratedCommandsEXT(...)
#endif

static inline void
radv_utrace_end_PreprocessGeneratedCommandsEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_PreprocessGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_PreprocessGeneratedCommandsEXT(...)
#endif

static inline void
radv_utrace_begin_DispatchBase(struct radv_cmd_buffer *cmd_buffer, uint32_t base_x, uint32_t base_y, uint32_t base_z, uint32_t x, uint32_t y, uint32_t z)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DispatchBase(cmd_buffer->utrace.trace, cmd_buffer, base_x, base_y, base_z, x, y, z);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DispatchBase(...)
#endif

static inline void
radv_utrace_end_DispatchBase(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DispatchBase(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DispatchBase(...)
#endif

static inline void
radv_utrace_begin_DrawIndirectCount(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirectCount(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirectCount(...)
#endif

static inline void
radv_utrace_end_DrawIndirectCount(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirectCount(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirectCount(...)
#endif

static inline void
radv_utrace_begin_DrawIndexedIndirectCount(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndexedIndirectCount(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndexedIndirectCount(...)
#endif

static inline void
radv_utrace_end_DrawIndexedIndirectCount(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndexedIndirectCount(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndexedIndirectCount(...)
#endif

static inline void
radv_utrace_begin_DrawIndirectByteCountEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirectByteCountEXT(cmd_buffer->utrace.trace, cmd_buffer, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirectByteCountEXT(...)
#endif

static inline void
radv_utrace_end_DrawIndirectByteCountEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirectByteCountEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirectByteCountEXT(...)
#endif

static inline void
radv_utrace_begin_DrawMeshTasksEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t x, uint32_t y, uint32_t z)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMeshTasksEXT(cmd_buffer->utrace.trace, cmd_buffer, x, y, z);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMeshTasksEXT(...)
#endif

static inline void
radv_utrace_end_DrawMeshTasksEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMeshTasksEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMeshTasksEXT(...)
#endif

static inline void
radv_utrace_begin_DrawMeshTasksIndirectEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMeshTasksIndirectEXT(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMeshTasksIndirectEXT(...)
#endif

static inline void
radv_utrace_end_DrawMeshTasksIndirectEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMeshTasksIndirectEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMeshTasksIndirectEXT(...)
#endif

static inline void
radv_utrace_begin_DrawMeshTasksIndirectCountEXT(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMeshTasksIndirectCountEXT(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMeshTasksIndirectCountEXT(...)
#endif

static inline void
radv_utrace_end_DrawMeshTasksIndirectCountEXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMeshTasksIndirectCountEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMeshTasksIndirectCountEXT(...)
#endif

static inline void
radv_utrace_begin_CopyAccelerationStructureKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyAccelerationStructureKHR(...)
#endif

static inline void
radv_utrace_end_CopyAccelerationStructureKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyAccelerationStructureKHR(...)
#endif

static inline void
radv_utrace_begin_CopyAccelerationStructureToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyAccelerationStructureToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyAccelerationStructureToMemoryKHR(...)
#endif

static inline void
radv_utrace_end_CopyAccelerationStructureToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyAccelerationStructureToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyAccelerationStructureToMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_CopyMemoryToAccelerationStructureKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyMemoryToAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyMemoryToAccelerationStructureKHR(...)
#endif

static inline void
radv_utrace_end_CopyMemoryToAccelerationStructureKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyMemoryToAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyMemoryToAccelerationStructureKHR(...)
#endif

static inline void
radv_utrace_begin_TraceRaysKHR(struct radv_cmd_buffer *cmd_buffer, uint32_t width, uint32_t height, uint32_t depth)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_TraceRaysKHR(cmd_buffer->utrace.trace, cmd_buffer, width, height, depth);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_TraceRaysKHR(...)
#endif

static inline void
radv_utrace_end_TraceRaysKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_TraceRaysKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_TraceRaysKHR(...)
#endif

static inline void
radv_utrace_begin_TraceRaysIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_TraceRaysIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_TraceRaysIndirectKHR(...)
#endif

static inline void
radv_utrace_end_TraceRaysIndirectKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_TraceRaysIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_TraceRaysIndirectKHR(...)
#endif

static inline void
radv_utrace_begin_TraceRaysIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_TraceRaysIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_TraceRaysIndirect2KHR(...)
#endif

static inline void
radv_utrace_end_TraceRaysIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_TraceRaysIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_TraceRaysIndirect2KHR(...)
#endif

static inline void
radv_utrace_begin_BuildAccelerationStructuresKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_BuildAccelerationStructuresKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_BuildAccelerationStructuresKHR(...)
#endif

static inline void
radv_utrace_end_BuildAccelerationStructuresKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_BuildAccelerationStructuresKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_BuildAccelerationStructuresKHR(...)
#endif

static inline void
radv_utrace_begin_CopyBuffer2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyBuffer2(...)
#endif

static inline void
radv_utrace_end_CopyBuffer2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyBuffer2(...)
#endif

static inline void
radv_utrace_begin_CopyImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyImage2(...)
#endif

static inline void
radv_utrace_end_CopyImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyImage2(...)
#endif

static inline void
radv_utrace_begin_BlitImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_BlitImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_BlitImage2(...)
#endif

static inline void
radv_utrace_end_BlitImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_BlitImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_BlitImage2(...)
#endif

static inline void
radv_utrace_begin_CopyBufferToImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyBufferToImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyBufferToImage2(...)
#endif

static inline void
radv_utrace_end_CopyBufferToImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyBufferToImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyBufferToImage2(...)
#endif

static inline void
radv_utrace_begin_CopyImageToBuffer2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyImageToBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyImageToBuffer2(...)
#endif

static inline void
radv_utrace_end_CopyImageToBuffer2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyImageToBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyImageToBuffer2(...)
#endif

static inline void
radv_utrace_begin_ResolveImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ResolveImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ResolveImage2(...)
#endif

static inline void
radv_utrace_end_ResolveImage2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ResolveImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ResolveImage2(...)
#endif

static inline void
radv_utrace_begin_SetEvent2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_SetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_SetEvent2(...)
#endif

static inline void
radv_utrace_end_SetEvent2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_SetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_SetEvent2(...)
#endif

static inline void
radv_utrace_begin_ResetEvent2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_ResetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_ResetEvent2(...)
#endif

static inline void
radv_utrace_end_ResetEvent2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_ResetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_ResetEvent2(...)
#endif

static inline void
radv_utrace_begin_WaitEvents2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_WaitEvents2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_WaitEvents2(...)
#endif

static inline void
radv_utrace_end_WaitEvents2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_WaitEvents2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_WaitEvents2(...)
#endif

static inline void
radv_utrace_begin_PipelineBarrier2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_PipelineBarrier2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_PipelineBarrier2(...)
#endif

static inline void
radv_utrace_end_PipelineBarrier2(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_PipelineBarrier2(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_PipelineBarrier2(...)
#endif

static inline void
radv_utrace_begin_CopyMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyMemoryKHR(...)
#endif

static inline void
radv_utrace_end_CopyMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_CopyMemoryToImageKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyMemoryToImageKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyMemoryToImageKHR(...)
#endif

static inline void
radv_utrace_end_CopyMemoryToImageKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyMemoryToImageKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyMemoryToImageKHR(...)
#endif

static inline void
radv_utrace_begin_CopyImageToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyImageToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyImageToMemoryKHR(...)
#endif

static inline void
radv_utrace_end_CopyImageToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyImageToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyImageToMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_UpdateMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_UpdateMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_UpdateMemoryKHR(...)
#endif

static inline void
radv_utrace_end_UpdateMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_UpdateMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_UpdateMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_FillMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_FillMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_FillMemoryKHR(...)
#endif

static inline void
radv_utrace_end_FillMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_FillMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_FillMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_CopyQueryPoolResultsToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_CopyQueryPoolResultsToMemoryKHR(...)
#endif

static inline void
radv_utrace_end_CopyQueryPoolResultsToMemoryKHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_CopyQueryPoolResultsToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_CopyQueryPoolResultsToMemoryKHR(...)
#endif

static inline void
radv_utrace_begin_DrawIndirectByteCount2EXT(struct radv_cmd_buffer *cmd_buffer, uint32_t instance_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirectByteCount2EXT(cmd_buffer->utrace.trace, cmd_buffer, instance_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirectByteCount2EXT(...)
#endif

static inline void
radv_utrace_end_DrawIndirectByteCount2EXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirectByteCount2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirectByteCount2EXT(...)
#endif

static inline void
radv_utrace_begin_DrawIndirect2KHR(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirect2KHR(...)
#endif

static inline void
radv_utrace_end_DrawIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirect2KHR(...)
#endif

static inline void
radv_utrace_begin_DrawIndexedIndirect2KHR(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndexedIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndexedIndirect2KHR(...)
#endif

static inline void
radv_utrace_end_DrawIndexedIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndexedIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndexedIndirect2KHR(...)
#endif

static inline void
radv_utrace_begin_DrawIndirectCount2KHR(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndirectCount2KHR(...)
#endif

static inline void
radv_utrace_end_DrawIndirectCount2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndirectCount2KHR(...)
#endif

static inline void
radv_utrace_begin_DrawIndexedIndirectCount2KHR(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawIndexedIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawIndexedIndirectCount2KHR(...)
#endif

static inline void
radv_utrace_end_DrawIndexedIndirectCount2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawIndexedIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawIndexedIndirectCount2KHR(...)
#endif

static inline void
radv_utrace_begin_DrawMeshTasksIndirect2EXT(struct radv_cmd_buffer *cmd_buffer, uint32_t draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMeshTasksIndirect2EXT(cmd_buffer->utrace.trace, cmd_buffer, draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMeshTasksIndirect2EXT(...)
#endif

static inline void
radv_utrace_end_DrawMeshTasksIndirect2EXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMeshTasksIndirect2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMeshTasksIndirect2EXT(...)
#endif

static inline void
radv_utrace_begin_DrawMeshTasksIndirectCount2EXT(struct radv_cmd_buffer *cmd_buffer, uint32_t max_draw_count)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DrawMeshTasksIndirectCount2EXT(cmd_buffer->utrace.trace, cmd_buffer, max_draw_count);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DrawMeshTasksIndirectCount2EXT(...)
#endif

static inline void
radv_utrace_end_DrawMeshTasksIndirectCount2EXT(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DrawMeshTasksIndirectCount2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DrawMeshTasksIndirectCount2EXT(...)
#endif

static inline void
radv_utrace_begin_DispatchIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_begin_DispatchIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_begin_DispatchIndirect2KHR(...)
#endif

static inline void
radv_utrace_end_DispatchIndirect2KHR(struct radv_cmd_buffer *cmd_buffer)
{
   if (unlikely(cmd_buffer->utrace.trace))
      trace_radv_end_DispatchIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

#ifndef RADV_U_TRACE
#define radv_utrace_end_DispatchIndirect2KHR(...)
#endif



#endif /* _RADV_TRACEPOINTS_H */
