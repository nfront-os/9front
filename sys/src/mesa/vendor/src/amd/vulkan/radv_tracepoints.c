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

#include "radv_tracepoints.h"


#define __NEEDS_TRACE_PRIV
#include "util/u_debug.h"
#include "util/perf/u_trace_priv.h"

static const struct debug_control config_control[] = {
   { "cmd_buffer", RADV_GPU_TRACEPOINT_CMD_BUFFER, },
   { "rendering", RADV_GPU_TRACEPOINT_RENDERING, },
   { "secondary", RADV_GPU_TRACEPOINT_SECONDARY, },
   { "internal", RADV_GPU_TRACEPOINT_INTERNAL, },
   { "draw", RADV_GPU_TRACEPOINT_DRAW, },
   { "dispatch", RADV_GPU_TRACEPOINT_DISPATCH, },
   { "buffer", RADV_GPU_TRACEPOINT_BUFFER, },
   { "image", RADV_GPU_TRACEPOINT_IMAGE, },
   { "query", RADV_GPU_TRACEPOINT_QUERY, },
   { "dgc", RADV_GPU_TRACEPOINT_DGC, },
   { "accel_struct", RADV_GPU_TRACEPOINT_ACCEL_STRUCT, },
   { "trace_rays", RADV_GPU_TRACEPOINT_TRACE_RAYS, },
   { "sync", RADV_GPU_TRACEPOINT_SYNC, },
   { NULL, 0, },
};
uint64_t radv_gpu_tracepoint = 0;

static void
radv_gpu_tracepoint_variable_once(void)
{
   uint64_t default_value = 0
     | RADV_GPU_TRACEPOINT_CMD_BUFFER
     | RADV_GPU_TRACEPOINT_RENDERING
     | RADV_GPU_TRACEPOINT_SECONDARY
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_INTERNAL
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DISPATCH
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_QUERY
     | RADV_GPU_TRACEPOINT_QUERY
     | RADV_GPU_TRACEPOINT_DGC
     | RADV_GPU_TRACEPOINT_DGC
     | RADV_GPU_TRACEPOINT_DISPATCH
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_ACCEL_STRUCT
     | RADV_GPU_TRACEPOINT_ACCEL_STRUCT
     | RADV_GPU_TRACEPOINT_ACCEL_STRUCT
     | RADV_GPU_TRACEPOINT_TRACE_RAYS
     | RADV_GPU_TRACEPOINT_TRACE_RAYS
     | RADV_GPU_TRACEPOINT_TRACE_RAYS
     | RADV_GPU_TRACEPOINT_ACCEL_STRUCT
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_SYNC
     | RADV_GPU_TRACEPOINT_SYNC
     | RADV_GPU_TRACEPOINT_SYNC
     | RADV_GPU_TRACEPOINT_SYNC
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_IMAGE
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_BUFFER
     | RADV_GPU_TRACEPOINT_QUERY
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DRAW
     | RADV_GPU_TRACEPOINT_DISPATCH
     ;

   radv_gpu_tracepoint =
      parse_enable_string(os_get_option("RADV_GPU_TRACEPOINT"),
                          default_value,
                          config_control);
}

void
radv_gpu_tracepoint_config_variable(void)
{
   static once_flag process_radv_gpu_tracepoint_variable_flag = ONCE_FLAG_INIT;

   call_once(&process_radv_gpu_tracepoint_variable_flag,
             radv_gpu_tracepoint_variable_once);
}

/*
 * radv_begin_cmd_buffer
 */
  
#define __print_radv_begin_cmd_buffer NULL
  
#define __print_json_radv_begin_cmd_buffer NULL
#define __print_fuzzy_hash_args_radv_begin_cmd_buffer NULL

#define __fuzzy_hash_radv_begin_cmd_buffer NULL
#define __fuzzy_equals_radv_begin_cmd_buffer NULL
#define __print_fuzzy_hash_args_radv_begin_cmd_buffer NULL
static const struct u_tracepoint __tp_radv_begin_cmd_buffer = {
    "radv_begin_cmd_buffer",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    0,
    u_tracepoint_type_begin_range,
    __print_radv_begin_cmd_buffer,
    __print_json_radv_begin_cmd_buffer,
    __fuzzy_hash_radv_begin_cmd_buffer,
    __fuzzy_equals_radv_begin_cmd_buffer,
    __print_fuzzy_hash_args_radv_begin_cmd_buffer,
};
void __trace_radv_begin_cmd_buffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_cmd_buffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_cmd_buffer *__entry =
      queueing ?
      (struct trace_radv_begin_cmd_buffer *)u_trace_appendv(ut, cs, &__tp_radv_begin_cmd_buffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_cmd_buffer
 */
  
#define __print_radv_end_cmd_buffer NULL
  
#define __print_json_radv_end_cmd_buffer NULL
#define __print_fuzzy_hash_args_radv_end_cmd_buffer NULL

#define __fuzzy_hash_radv_end_cmd_buffer NULL
#define __fuzzy_equals_radv_end_cmd_buffer NULL
#define __print_fuzzy_hash_args_radv_end_cmd_buffer NULL
static const struct u_tracepoint __tp_radv_end_cmd_buffer = {
    "radv_end_cmd_buffer",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    1,
    u_tracepoint_type_end_range,
    __print_radv_end_cmd_buffer,
    __print_json_radv_end_cmd_buffer,
    __fuzzy_hash_radv_end_cmd_buffer,
    __fuzzy_equals_radv_end_cmd_buffer,
    __print_fuzzy_hash_args_radv_end_cmd_buffer,
};
void __trace_radv_end_cmd_buffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_cmd_buffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_cmd_buffer *__entry =
      queueing ?
      (struct trace_radv_end_cmd_buffer *)u_trace_appendv(ut, cs, &__tp_radv_end_cmd_buffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_rendering
 */
  
#define __print_radv_begin_rendering NULL
  
#define __print_json_radv_begin_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_rendering NULL

#define __fuzzy_hash_radv_begin_rendering NULL
#define __fuzzy_equals_radv_begin_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_rendering NULL
static const struct u_tracepoint __tp_radv_begin_rendering = {
    "radv_begin_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    2,
    u_tracepoint_type_begin_range,
    __print_radv_begin_rendering,
    __print_json_radv_begin_rendering,
    __fuzzy_hash_radv_begin_rendering,
    __fuzzy_equals_radv_begin_rendering,
    __print_fuzzy_hash_args_radv_begin_rendering,
};
void __trace_radv_begin_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_rendering *__entry =
      queueing ?
      (struct trace_radv_begin_rendering *)u_trace_appendv(ut, cs, &__tp_radv_begin_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_rendering
 */
  
#define __print_radv_end_rendering NULL
  
#define __print_json_radv_end_rendering NULL
#define __print_fuzzy_hash_args_radv_end_rendering NULL

#define __fuzzy_hash_radv_end_rendering NULL
#define __fuzzy_equals_radv_end_rendering NULL
#define __print_fuzzy_hash_args_radv_end_rendering NULL
static const struct u_tracepoint __tp_radv_end_rendering = {
    "radv_end_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    3,
    u_tracepoint_type_end_range,
    __print_radv_end_rendering,
    __print_json_radv_end_rendering,
    __fuzzy_hash_radv_end_rendering,
    __fuzzy_equals_radv_end_rendering,
    __print_fuzzy_hash_args_radv_end_rendering,
};
void __trace_radv_end_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_rendering *__entry =
      queueing ?
      (struct trace_radv_end_rendering *)u_trace_appendv(ut, cs, &__tp_radv_end_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_secondary
 */
  
#define __print_radv_begin_secondary NULL
  
#define __print_json_radv_begin_secondary NULL
#define __print_fuzzy_hash_args_radv_begin_secondary NULL

#define __fuzzy_hash_radv_begin_secondary NULL
#define __fuzzy_equals_radv_begin_secondary NULL
#define __print_fuzzy_hash_args_radv_begin_secondary NULL
static const struct u_tracepoint __tp_radv_begin_secondary = {
    "radv_begin_secondary",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    4,
    u_tracepoint_type_begin_range,
    __print_radv_begin_secondary,
    __print_json_radv_begin_secondary,
    __fuzzy_hash_radv_begin_secondary,
    __fuzzy_equals_radv_begin_secondary,
    __print_fuzzy_hash_args_radv_begin_secondary,
};
void __trace_radv_begin_secondary(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_secondary entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_secondary *__entry =
      queueing ?
      (struct trace_radv_begin_secondary *)u_trace_appendv(ut, cs, &__tp_radv_begin_secondary,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_secondary
 */
  
#define __print_radv_end_secondary NULL
  
#define __print_json_radv_end_secondary NULL
#define __print_fuzzy_hash_args_radv_end_secondary NULL

#define __fuzzy_hash_radv_end_secondary NULL
#define __fuzzy_equals_radv_end_secondary NULL
#define __print_fuzzy_hash_args_radv_end_secondary NULL
static const struct u_tracepoint __tp_radv_end_secondary = {
    "radv_end_secondary",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    5,
    u_tracepoint_type_end_range,
    __print_radv_end_secondary,
    __print_json_radv_end_secondary,
    __fuzzy_hash_radv_end_secondary,
    __fuzzy_equals_radv_end_secondary,
    __print_fuzzy_hash_args_radv_end_secondary,
};
void __trace_radv_end_secondary(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_secondary entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_secondary *__entry =
      queueing ?
      (struct trace_radv_end_secondary *)u_trace_appendv(ut, cs, &__tp_radv_end_secondary,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_image_transition
 */
  
#define __print_radv_begin_image_transition NULL
  
#define __print_json_radv_begin_image_transition NULL
#define __print_fuzzy_hash_args_radv_begin_image_transition NULL

#define __fuzzy_hash_radv_begin_image_transition NULL
#define __fuzzy_equals_radv_begin_image_transition NULL
#define __print_fuzzy_hash_args_radv_begin_image_transition NULL
static const struct u_tracepoint __tp_radv_begin_image_transition = {
    "radv_begin_image_transition",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    6,
    u_tracepoint_type_begin_range,
    __print_radv_begin_image_transition,
    __print_json_radv_begin_image_transition,
    __fuzzy_hash_radv_begin_image_transition,
    __fuzzy_equals_radv_begin_image_transition,
    __print_fuzzy_hash_args_radv_begin_image_transition,
};
void __trace_radv_begin_image_transition(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_image_transition entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_image_transition *__entry =
      queueing ?
      (struct trace_radv_begin_image_transition *)u_trace_appendv(ut, cs, &__tp_radv_begin_image_transition,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_image_transition
 */
  
#define __print_radv_end_image_transition NULL
  
#define __print_json_radv_end_image_transition NULL
#define __print_fuzzy_hash_args_radv_end_image_transition NULL

#define __fuzzy_hash_radv_end_image_transition NULL
#define __fuzzy_equals_radv_end_image_transition NULL
#define __print_fuzzy_hash_args_radv_end_image_transition NULL
static const struct u_tracepoint __tp_radv_end_image_transition = {
    "radv_end_image_transition",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    7,
    u_tracepoint_type_end_range,
    __print_radv_end_image_transition,
    __print_json_radv_end_image_transition,
    __fuzzy_hash_radv_end_image_transition,
    __fuzzy_equals_radv_end_image_transition,
    __print_fuzzy_hash_args_radv_end_image_transition,
};
void __trace_radv_end_image_transition(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_image_transition entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_image_transition *__entry =
      queueing ?
      (struct trace_radv_end_image_transition *)u_trace_appendv(ut, cs, &__tp_radv_end_image_transition,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_copy_vrs_htile
 */
  
#define __print_radv_begin_copy_vrs_htile NULL
  
#define __print_json_radv_begin_copy_vrs_htile NULL
#define __print_fuzzy_hash_args_radv_begin_copy_vrs_htile NULL

#define __fuzzy_hash_radv_begin_copy_vrs_htile NULL
#define __fuzzy_equals_radv_begin_copy_vrs_htile NULL
#define __print_fuzzy_hash_args_radv_begin_copy_vrs_htile NULL
static const struct u_tracepoint __tp_radv_begin_copy_vrs_htile = {
    "radv_begin_copy_vrs_htile",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    8,
    u_tracepoint_type_begin_range,
    __print_radv_begin_copy_vrs_htile,
    __print_json_radv_begin_copy_vrs_htile,
    __fuzzy_hash_radv_begin_copy_vrs_htile,
    __fuzzy_equals_radv_begin_copy_vrs_htile,
    __print_fuzzy_hash_args_radv_begin_copy_vrs_htile,
};
void __trace_radv_begin_copy_vrs_htile(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_copy_vrs_htile entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_copy_vrs_htile *__entry =
      queueing ?
      (struct trace_radv_begin_copy_vrs_htile *)u_trace_appendv(ut, cs, &__tp_radv_begin_copy_vrs_htile,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_copy_vrs_htile
 */
  
#define __print_radv_end_copy_vrs_htile NULL
  
#define __print_json_radv_end_copy_vrs_htile NULL
#define __print_fuzzy_hash_args_radv_end_copy_vrs_htile NULL

#define __fuzzy_hash_radv_end_copy_vrs_htile NULL
#define __fuzzy_equals_radv_end_copy_vrs_htile NULL
#define __print_fuzzy_hash_args_radv_end_copy_vrs_htile NULL
static const struct u_tracepoint __tp_radv_end_copy_vrs_htile = {
    "radv_end_copy_vrs_htile",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    9,
    u_tracepoint_type_end_range,
    __print_radv_end_copy_vrs_htile,
    __print_json_radv_end_copy_vrs_htile,
    __fuzzy_hash_radv_end_copy_vrs_htile,
    __fuzzy_equals_radv_end_copy_vrs_htile,
    __print_fuzzy_hash_args_radv_end_copy_vrs_htile,
};
void __trace_radv_end_copy_vrs_htile(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_copy_vrs_htile entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_copy_vrs_htile *__entry =
      queueing ?
      (struct trace_radv_end_copy_vrs_htile *)u_trace_appendv(ut, cs, &__tp_radv_end_copy_vrs_htile,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_clear_rendering
 */
  
#define __print_radv_begin_clear_rendering NULL
  
#define __print_json_radv_begin_clear_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_clear_rendering NULL

#define __fuzzy_hash_radv_begin_clear_rendering NULL
#define __fuzzy_equals_radv_begin_clear_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_clear_rendering NULL
static const struct u_tracepoint __tp_radv_begin_clear_rendering = {
    "radv_begin_clear_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    10,
    u_tracepoint_type_begin_range,
    __print_radv_begin_clear_rendering,
    __print_json_radv_begin_clear_rendering,
    __fuzzy_hash_radv_begin_clear_rendering,
    __fuzzy_equals_radv_begin_clear_rendering,
    __print_fuzzy_hash_args_radv_begin_clear_rendering,
};
void __trace_radv_begin_clear_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_clear_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_clear_rendering *__entry =
      queueing ?
      (struct trace_radv_begin_clear_rendering *)u_trace_appendv(ut, cs, &__tp_radv_begin_clear_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_clear_rendering
 */
  
#define __print_radv_end_clear_rendering NULL
  
#define __print_json_radv_end_clear_rendering NULL
#define __print_fuzzy_hash_args_radv_end_clear_rendering NULL

#define __fuzzy_hash_radv_end_clear_rendering NULL
#define __fuzzy_equals_radv_end_clear_rendering NULL
#define __print_fuzzy_hash_args_radv_end_clear_rendering NULL
static const struct u_tracepoint __tp_radv_end_clear_rendering = {
    "radv_end_clear_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    11,
    u_tracepoint_type_end_range,
    __print_radv_end_clear_rendering,
    __print_json_radv_end_clear_rendering,
    __fuzzy_hash_radv_end_clear_rendering,
    __fuzzy_equals_radv_end_clear_rendering,
    __print_fuzzy_hash_args_radv_end_clear_rendering,
};
void __trace_radv_end_clear_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_clear_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_clear_rendering *__entry =
      queueing ?
      (struct trace_radv_end_clear_rendering *)u_trace_appendv(ut, cs, &__tp_radv_end_clear_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_resolve_rendering
 */
  
#define __print_radv_begin_resolve_rendering NULL
  
#define __print_json_radv_begin_resolve_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_resolve_rendering NULL

#define __fuzzy_hash_radv_begin_resolve_rendering NULL
#define __fuzzy_equals_radv_begin_resolve_rendering NULL
#define __print_fuzzy_hash_args_radv_begin_resolve_rendering NULL
static const struct u_tracepoint __tp_radv_begin_resolve_rendering = {
    "radv_begin_resolve_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    12,
    u_tracepoint_type_begin_range,
    __print_radv_begin_resolve_rendering,
    __print_json_radv_begin_resolve_rendering,
    __fuzzy_hash_radv_begin_resolve_rendering,
    __fuzzy_equals_radv_begin_resolve_rendering,
    __print_fuzzy_hash_args_radv_begin_resolve_rendering,
};
void __trace_radv_begin_resolve_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_resolve_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_resolve_rendering *__entry =
      queueing ?
      (struct trace_radv_begin_resolve_rendering *)u_trace_appendv(ut, cs, &__tp_radv_begin_resolve_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_resolve_rendering
 */
  
#define __print_radv_end_resolve_rendering NULL
  
#define __print_json_radv_end_resolve_rendering NULL
#define __print_fuzzy_hash_args_radv_end_resolve_rendering NULL

#define __fuzzy_hash_radv_end_resolve_rendering NULL
#define __fuzzy_equals_radv_end_resolve_rendering NULL
#define __print_fuzzy_hash_args_radv_end_resolve_rendering NULL
static const struct u_tracepoint __tp_radv_end_resolve_rendering = {
    "radv_end_resolve_rendering",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    13,
    u_tracepoint_type_end_range,
    __print_radv_end_resolve_rendering,
    __print_json_radv_end_resolve_rendering,
    __fuzzy_hash_radv_end_resolve_rendering,
    __fuzzy_equals_radv_end_resolve_rendering,
    __print_fuzzy_hash_args_radv_end_resolve_rendering,
};
void __trace_radv_end_resolve_rendering(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_resolve_rendering entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_resolve_rendering *__entry =
      queueing ?
      (struct trace_radv_end_resolve_rendering *)u_trace_appendv(ut, cs, &__tp_radv_end_resolve_rendering,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_compute_copy_memory
 */
  
static void __print_radv_begin_compute_copy_memory(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_compute_copy_memory *__entry =
      (const struct trace_radv_begin_compute_copy_memory *)arg;
   fprintf(out, ""
      "size=%u, "
         "\n"
   ,__entry->size
   );
}

  
static void __print_json_radv_begin_compute_copy_memory(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_compute_copy_memory *__entry =
      (const struct trace_radv_begin_compute_copy_memory *)arg;
   fprintf(out, ""
      "\"size\": \"%u\""
   ,__entry->size
   );
}

#define __print_fuzzy_hash_args_radv_begin_compute_copy_memory NULL

static uint32_t __fuzzy_hash_radv_begin_compute_copy_memory(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_compute_copy_memory *__entry =
      (const struct trace_radv_begin_compute_copy_memory *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_compute_copy_memory(const void *_a, const void *_b) {
   const struct trace_radv_begin_compute_copy_memory *__entry_a =
      (const struct trace_radv_begin_compute_copy_memory *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_compute_copy_memory *__entry_b =
      (const struct trace_radv_begin_compute_copy_memory *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_compute_copy_memory = {
    "radv_begin_compute_copy_memory",
    ALIGN_POT(sizeof(struct trace_radv_begin_compute_copy_memory), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    14,
    u_tracepoint_type_begin_range,
    __print_radv_begin_compute_copy_memory,
    __print_json_radv_begin_compute_copy_memory,
    __fuzzy_hash_radv_begin_compute_copy_memory,
    __fuzzy_equals_radv_begin_compute_copy_memory,
    __print_fuzzy_hash_args_radv_begin_compute_copy_memory,
};
void __trace_radv_begin_compute_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
) {
   struct trace_radv_begin_compute_copy_memory entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_compute_copy_memory *__entry =
      queueing ?
      (struct trace_radv_begin_compute_copy_memory *)u_trace_appendv(ut, cs, &__tp_radv_begin_compute_copy_memory,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->size = size;
}

/*
 * radv_end_compute_copy_memory
 */
  
#define __print_radv_end_compute_copy_memory NULL
  
#define __print_json_radv_end_compute_copy_memory NULL
#define __print_fuzzy_hash_args_radv_end_compute_copy_memory NULL

#define __fuzzy_hash_radv_end_compute_copy_memory NULL
#define __fuzzy_equals_radv_end_compute_copy_memory NULL
#define __print_fuzzy_hash_args_radv_end_compute_copy_memory NULL
static const struct u_tracepoint __tp_radv_end_compute_copy_memory = {
    "radv_end_compute_copy_memory",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    15,
    u_tracepoint_type_end_range,
    __print_radv_end_compute_copy_memory,
    __print_json_radv_end_compute_copy_memory,
    __fuzzy_hash_radv_end_compute_copy_memory,
    __fuzzy_equals_radv_end_compute_copy_memory,
    __print_fuzzy_hash_args_radv_end_compute_copy_memory,
};
void __trace_radv_end_compute_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_compute_copy_memory entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_compute_copy_memory *__entry =
      queueing ?
      (struct trace_radv_end_compute_copy_memory *)u_trace_appendv(ut, cs, &__tp_radv_end_compute_copy_memory,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_cp_dma_copy_memory
 */
  
static void __print_radv_begin_cp_dma_copy_memory(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_cp_dma_copy_memory *__entry =
      (const struct trace_radv_begin_cp_dma_copy_memory *)arg;
   fprintf(out, ""
      "size=%u, "
         "\n"
   ,__entry->size
   );
}

  
static void __print_json_radv_begin_cp_dma_copy_memory(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_cp_dma_copy_memory *__entry =
      (const struct trace_radv_begin_cp_dma_copy_memory *)arg;
   fprintf(out, ""
      "\"size\": \"%u\""
   ,__entry->size
   );
}

#define __print_fuzzy_hash_args_radv_begin_cp_dma_copy_memory NULL

static uint32_t __fuzzy_hash_radv_begin_cp_dma_copy_memory(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_cp_dma_copy_memory *__entry =
      (const struct trace_radv_begin_cp_dma_copy_memory *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_cp_dma_copy_memory(const void *_a, const void *_b) {
   const struct trace_radv_begin_cp_dma_copy_memory *__entry_a =
      (const struct trace_radv_begin_cp_dma_copy_memory *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_cp_dma_copy_memory *__entry_b =
      (const struct trace_radv_begin_cp_dma_copy_memory *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_cp_dma_copy_memory = {
    "radv_begin_cp_dma_copy_memory",
    ALIGN_POT(sizeof(struct trace_radv_begin_cp_dma_copy_memory), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    16,
    u_tracepoint_type_begin_range,
    __print_radv_begin_cp_dma_copy_memory,
    __print_json_radv_begin_cp_dma_copy_memory,
    __fuzzy_hash_radv_begin_cp_dma_copy_memory,
    __fuzzy_equals_radv_begin_cp_dma_copy_memory,
    __print_fuzzy_hash_args_radv_begin_cp_dma_copy_memory,
};
void __trace_radv_begin_cp_dma_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
) {
   struct trace_radv_begin_cp_dma_copy_memory entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_cp_dma_copy_memory *__entry =
      queueing ?
      (struct trace_radv_begin_cp_dma_copy_memory *)u_trace_appendv(ut, cs, &__tp_radv_begin_cp_dma_copy_memory,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->size = size;
}

/*
 * radv_end_cp_dma_copy_memory
 */
  
#define __print_radv_end_cp_dma_copy_memory NULL
  
#define __print_json_radv_end_cp_dma_copy_memory NULL
#define __print_fuzzy_hash_args_radv_end_cp_dma_copy_memory NULL

#define __fuzzy_hash_radv_end_cp_dma_copy_memory NULL
#define __fuzzy_equals_radv_end_cp_dma_copy_memory NULL
#define __print_fuzzy_hash_args_radv_end_cp_dma_copy_memory NULL
static const struct u_tracepoint __tp_radv_end_cp_dma_copy_memory = {
    "radv_end_cp_dma_copy_memory",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    17,
    u_tracepoint_type_end_range,
    __print_radv_end_cp_dma_copy_memory,
    __print_json_radv_end_cp_dma_copy_memory,
    __fuzzy_hash_radv_end_cp_dma_copy_memory,
    __fuzzy_equals_radv_end_cp_dma_copy_memory,
    __print_fuzzy_hash_args_radv_end_cp_dma_copy_memory,
};
void __trace_radv_end_cp_dma_copy_memory(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_cp_dma_copy_memory entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_cp_dma_copy_memory *__entry =
      queueing ?
      (struct trace_radv_end_cp_dma_copy_memory *)u_trace_appendv(ut, cs, &__tp_radv_end_cp_dma_copy_memory,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_leaves
 */
  
#define __print_radv_begin_leaves NULL
  
#define __print_json_radv_begin_leaves NULL
#define __print_fuzzy_hash_args_radv_begin_leaves NULL

#define __fuzzy_hash_radv_begin_leaves NULL
#define __fuzzy_equals_radv_begin_leaves NULL
#define __print_fuzzy_hash_args_radv_begin_leaves NULL
static const struct u_tracepoint __tp_radv_begin_leaves = {
    "radv_begin_leaves",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    18,
    u_tracepoint_type_begin_range,
    __print_radv_begin_leaves,
    __print_json_radv_begin_leaves,
    __fuzzy_hash_radv_begin_leaves,
    __fuzzy_equals_radv_begin_leaves,
    __print_fuzzy_hash_args_radv_begin_leaves,
};
void __trace_radv_begin_leaves(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_leaves entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_leaves *__entry =
      queueing ?
      (struct trace_radv_begin_leaves *)u_trace_appendv(ut, cs, &__tp_radv_begin_leaves,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_leaves
 */
  
#define __print_radv_end_leaves NULL
  
#define __print_json_radv_end_leaves NULL
#define __print_fuzzy_hash_args_radv_end_leaves NULL

#define __fuzzy_hash_radv_end_leaves NULL
#define __fuzzy_equals_radv_end_leaves NULL
#define __print_fuzzy_hash_args_radv_end_leaves NULL
static const struct u_tracepoint __tp_radv_end_leaves = {
    "radv_end_leaves",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    19,
    u_tracepoint_type_end_range,
    __print_radv_end_leaves,
    __print_json_radv_end_leaves,
    __fuzzy_hash_radv_end_leaves,
    __fuzzy_equals_radv_end_leaves,
    __print_fuzzy_hash_args_radv_end_leaves,
};
void __trace_radv_end_leaves(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_leaves entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_leaves *__entry =
      queueing ?
      (struct trace_radv_end_leaves *)u_trace_appendv(ut, cs, &__tp_radv_end_leaves,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_morton_generate
 */
  
#define __print_radv_begin_morton_generate NULL
  
#define __print_json_radv_begin_morton_generate NULL
#define __print_fuzzy_hash_args_radv_begin_morton_generate NULL

#define __fuzzy_hash_radv_begin_morton_generate NULL
#define __fuzzy_equals_radv_begin_morton_generate NULL
#define __print_fuzzy_hash_args_radv_begin_morton_generate NULL
static const struct u_tracepoint __tp_radv_begin_morton_generate = {
    "radv_begin_morton_generate",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    20,
    u_tracepoint_type_begin_range,
    __print_radv_begin_morton_generate,
    __print_json_radv_begin_morton_generate,
    __fuzzy_hash_radv_begin_morton_generate,
    __fuzzy_equals_radv_begin_morton_generate,
    __print_fuzzy_hash_args_radv_begin_morton_generate,
};
void __trace_radv_begin_morton_generate(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_morton_generate entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_morton_generate *__entry =
      queueing ?
      (struct trace_radv_begin_morton_generate *)u_trace_appendv(ut, cs, &__tp_radv_begin_morton_generate,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_morton_generate
 */
  
#define __print_radv_end_morton_generate NULL
  
#define __print_json_radv_end_morton_generate NULL
#define __print_fuzzy_hash_args_radv_end_morton_generate NULL

#define __fuzzy_hash_radv_end_morton_generate NULL
#define __fuzzy_equals_radv_end_morton_generate NULL
#define __print_fuzzy_hash_args_radv_end_morton_generate NULL
static const struct u_tracepoint __tp_radv_end_morton_generate = {
    "radv_end_morton_generate",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    21,
    u_tracepoint_type_end_range,
    __print_radv_end_morton_generate,
    __print_json_radv_end_morton_generate,
    __fuzzy_hash_radv_end_morton_generate,
    __fuzzy_equals_radv_end_morton_generate,
    __print_fuzzy_hash_args_radv_end_morton_generate,
};
void __trace_radv_end_morton_generate(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_morton_generate entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_morton_generate *__entry =
      queueing ?
      (struct trace_radv_end_morton_generate *)u_trace_appendv(ut, cs, &__tp_radv_end_morton_generate,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_morton_sort
 */
  
#define __print_radv_begin_morton_sort NULL
  
#define __print_json_radv_begin_morton_sort NULL
#define __print_fuzzy_hash_args_radv_begin_morton_sort NULL

#define __fuzzy_hash_radv_begin_morton_sort NULL
#define __fuzzy_equals_radv_begin_morton_sort NULL
#define __print_fuzzy_hash_args_radv_begin_morton_sort NULL
static const struct u_tracepoint __tp_radv_begin_morton_sort = {
    "radv_begin_morton_sort",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    22,
    u_tracepoint_type_begin_range,
    __print_radv_begin_morton_sort,
    __print_json_radv_begin_morton_sort,
    __fuzzy_hash_radv_begin_morton_sort,
    __fuzzy_equals_radv_begin_morton_sort,
    __print_fuzzy_hash_args_radv_begin_morton_sort,
};
void __trace_radv_begin_morton_sort(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_morton_sort entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_morton_sort *__entry =
      queueing ?
      (struct trace_radv_begin_morton_sort *)u_trace_appendv(ut, cs, &__tp_radv_begin_morton_sort,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_morton_sort
 */
  
#define __print_radv_end_morton_sort NULL
  
#define __print_json_radv_end_morton_sort NULL
#define __print_fuzzy_hash_args_radv_end_morton_sort NULL

#define __fuzzy_hash_radv_end_morton_sort NULL
#define __fuzzy_equals_radv_end_morton_sort NULL
#define __print_fuzzy_hash_args_radv_end_morton_sort NULL
static const struct u_tracepoint __tp_radv_end_morton_sort = {
    "radv_end_morton_sort",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    23,
    u_tracepoint_type_end_range,
    __print_radv_end_morton_sort,
    __print_json_radv_end_morton_sort,
    __fuzzy_hash_radv_end_morton_sort,
    __fuzzy_equals_radv_end_morton_sort,
    __print_fuzzy_hash_args_radv_end_morton_sort,
};
void __trace_radv_end_morton_sort(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_morton_sort entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_morton_sort *__entry =
      queueing ?
      (struct trace_radv_end_morton_sort *)u_trace_appendv(ut, cs, &__tp_radv_end_morton_sort,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_lbvh_main
 */
  
#define __print_radv_begin_lbvh_main NULL
  
#define __print_json_radv_begin_lbvh_main NULL
#define __print_fuzzy_hash_args_radv_begin_lbvh_main NULL

#define __fuzzy_hash_radv_begin_lbvh_main NULL
#define __fuzzy_equals_radv_begin_lbvh_main NULL
#define __print_fuzzy_hash_args_radv_begin_lbvh_main NULL
static const struct u_tracepoint __tp_radv_begin_lbvh_main = {
    "radv_begin_lbvh_main",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    24,
    u_tracepoint_type_begin_range,
    __print_radv_begin_lbvh_main,
    __print_json_radv_begin_lbvh_main,
    __fuzzy_hash_radv_begin_lbvh_main,
    __fuzzy_equals_radv_begin_lbvh_main,
    __print_fuzzy_hash_args_radv_begin_lbvh_main,
};
void __trace_radv_begin_lbvh_main(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_lbvh_main entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_lbvh_main *__entry =
      queueing ?
      (struct trace_radv_begin_lbvh_main *)u_trace_appendv(ut, cs, &__tp_radv_begin_lbvh_main,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_lbvh_main
 */
  
#define __print_radv_end_lbvh_main NULL
  
#define __print_json_radv_end_lbvh_main NULL
#define __print_fuzzy_hash_args_radv_end_lbvh_main NULL

#define __fuzzy_hash_radv_end_lbvh_main NULL
#define __fuzzy_equals_radv_end_lbvh_main NULL
#define __print_fuzzy_hash_args_radv_end_lbvh_main NULL
static const struct u_tracepoint __tp_radv_end_lbvh_main = {
    "radv_end_lbvh_main",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    25,
    u_tracepoint_type_end_range,
    __print_radv_end_lbvh_main,
    __print_json_radv_end_lbvh_main,
    __fuzzy_hash_radv_end_lbvh_main,
    __fuzzy_equals_radv_end_lbvh_main,
    __print_fuzzy_hash_args_radv_end_lbvh_main,
};
void __trace_radv_end_lbvh_main(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_lbvh_main entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_lbvh_main *__entry =
      queueing ?
      (struct trace_radv_end_lbvh_main *)u_trace_appendv(ut, cs, &__tp_radv_end_lbvh_main,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_lbvh_generate_ir
 */
  
#define __print_radv_begin_lbvh_generate_ir NULL
  
#define __print_json_radv_begin_lbvh_generate_ir NULL
#define __print_fuzzy_hash_args_radv_begin_lbvh_generate_ir NULL

#define __fuzzy_hash_radv_begin_lbvh_generate_ir NULL
#define __fuzzy_equals_radv_begin_lbvh_generate_ir NULL
#define __print_fuzzy_hash_args_radv_begin_lbvh_generate_ir NULL
static const struct u_tracepoint __tp_radv_begin_lbvh_generate_ir = {
    "radv_begin_lbvh_generate_ir",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    26,
    u_tracepoint_type_begin_range,
    __print_radv_begin_lbvh_generate_ir,
    __print_json_radv_begin_lbvh_generate_ir,
    __fuzzy_hash_radv_begin_lbvh_generate_ir,
    __fuzzy_equals_radv_begin_lbvh_generate_ir,
    __print_fuzzy_hash_args_radv_begin_lbvh_generate_ir,
};
void __trace_radv_begin_lbvh_generate_ir(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_lbvh_generate_ir entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_lbvh_generate_ir *__entry =
      queueing ?
      (struct trace_radv_begin_lbvh_generate_ir *)u_trace_appendv(ut, cs, &__tp_radv_begin_lbvh_generate_ir,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_lbvh_generate_ir
 */
  
#define __print_radv_end_lbvh_generate_ir NULL
  
#define __print_json_radv_end_lbvh_generate_ir NULL
#define __print_fuzzy_hash_args_radv_end_lbvh_generate_ir NULL

#define __fuzzy_hash_radv_end_lbvh_generate_ir NULL
#define __fuzzy_equals_radv_end_lbvh_generate_ir NULL
#define __print_fuzzy_hash_args_radv_end_lbvh_generate_ir NULL
static const struct u_tracepoint __tp_radv_end_lbvh_generate_ir = {
    "radv_end_lbvh_generate_ir",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    27,
    u_tracepoint_type_end_range,
    __print_radv_end_lbvh_generate_ir,
    __print_json_radv_end_lbvh_generate_ir,
    __fuzzy_hash_radv_end_lbvh_generate_ir,
    __fuzzy_equals_radv_end_lbvh_generate_ir,
    __print_fuzzy_hash_args_radv_end_lbvh_generate_ir,
};
void __trace_radv_end_lbvh_generate_ir(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_lbvh_generate_ir entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_lbvh_generate_ir *__entry =
      queueing ?
      (struct trace_radv_end_lbvh_generate_ir *)u_trace_appendv(ut, cs, &__tp_radv_end_lbvh_generate_ir,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ploc
 */
  
#define __print_radv_begin_ploc NULL
  
#define __print_json_radv_begin_ploc NULL
#define __print_fuzzy_hash_args_radv_begin_ploc NULL

#define __fuzzy_hash_radv_begin_ploc NULL
#define __fuzzy_equals_radv_begin_ploc NULL
#define __print_fuzzy_hash_args_radv_begin_ploc NULL
static const struct u_tracepoint __tp_radv_begin_ploc = {
    "radv_begin_ploc",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    28,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ploc,
    __print_json_radv_begin_ploc,
    __fuzzy_hash_radv_begin_ploc,
    __fuzzy_equals_radv_begin_ploc,
    __print_fuzzy_hash_args_radv_begin_ploc,
};
void __trace_radv_begin_ploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ploc entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ploc *__entry =
      queueing ?
      (struct trace_radv_begin_ploc *)u_trace_appendv(ut, cs, &__tp_radv_begin_ploc,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ploc
 */
  
#define __print_radv_end_ploc NULL
  
#define __print_json_radv_end_ploc NULL
#define __print_fuzzy_hash_args_radv_end_ploc NULL

#define __fuzzy_hash_radv_end_ploc NULL
#define __fuzzy_equals_radv_end_ploc NULL
#define __print_fuzzy_hash_args_radv_end_ploc NULL
static const struct u_tracepoint __tp_radv_end_ploc = {
    "radv_end_ploc",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    29,
    u_tracepoint_type_end_range,
    __print_radv_end_ploc,
    __print_json_radv_end_ploc,
    __fuzzy_hash_radv_end_ploc,
    __fuzzy_equals_radv_end_ploc,
    __print_fuzzy_hash_args_radv_end_ploc,
};
void __trace_radv_end_ploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ploc entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ploc *__entry =
      queueing ?
      (struct trace_radv_end_ploc *)u_trace_appendv(ut, cs, &__tp_radv_end_ploc,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_hploc
 */
  
#define __print_radv_begin_hploc NULL
  
#define __print_json_radv_begin_hploc NULL
#define __print_fuzzy_hash_args_radv_begin_hploc NULL

#define __fuzzy_hash_radv_begin_hploc NULL
#define __fuzzy_equals_radv_begin_hploc NULL
#define __print_fuzzy_hash_args_radv_begin_hploc NULL
static const struct u_tracepoint __tp_radv_begin_hploc = {
    "radv_begin_hploc",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    30,
    u_tracepoint_type_begin_range,
    __print_radv_begin_hploc,
    __print_json_radv_begin_hploc,
    __fuzzy_hash_radv_begin_hploc,
    __fuzzy_equals_radv_begin_hploc,
    __print_fuzzy_hash_args_radv_begin_hploc,
};
void __trace_radv_begin_hploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_hploc entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_hploc *__entry =
      queueing ?
      (struct trace_radv_begin_hploc *)u_trace_appendv(ut, cs, &__tp_radv_begin_hploc,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_hploc
 */
  
#define __print_radv_end_hploc NULL
  
#define __print_json_radv_end_hploc NULL
#define __print_fuzzy_hash_args_radv_end_hploc NULL

#define __fuzzy_hash_radv_end_hploc NULL
#define __fuzzy_equals_radv_end_hploc NULL
#define __print_fuzzy_hash_args_radv_end_hploc NULL
static const struct u_tracepoint __tp_radv_end_hploc = {
    "radv_end_hploc",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    31,
    u_tracepoint_type_end_range,
    __print_radv_end_hploc,
    __print_json_radv_end_hploc,
    __fuzzy_hash_radv_end_hploc,
    __fuzzy_equals_radv_end_hploc,
    __print_fuzzy_hash_args_radv_end_hploc,
};
void __trace_radv_end_hploc(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_hploc entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_hploc *__entry =
      queueing ?
      (struct trace_radv_end_hploc *)u_trace_appendv(ut, cs, &__tp_radv_end_hploc,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_update_as
 */
  
static void __print_radv_begin_update_as(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_update_as *__entry =
      (const struct trace_radv_begin_update_as *)arg;
   fprintf(out, ""
      "build_flags=0x%x, "
         "\n"
   ,__entry->build_flags
   );
}

  
static void __print_json_radv_begin_update_as(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_update_as *__entry =
      (const struct trace_radv_begin_update_as *)arg;
   fprintf(out, ""
      "\"build_flags\": \"0x%x\""
   ,__entry->build_flags
   );
}

static void __print_fuzzy_hash_args_radv_begin_update_as(FILE *out, const void *arg) {
   const struct trace_radv_begin_update_as *__entry =
      (const struct trace_radv_begin_update_as *)arg;
   (void)__entry;
   fprintf(out, ""
      "build_flags=0x%x, "
      , __entry->build_flags
   );
}

static uint32_t __fuzzy_hash_radv_begin_update_as(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_update_as *__entry =
      (const struct trace_radv_begin_update_as *)_event;
   (void)__entry;
   hash *= (__entry->build_flags);
   return hash;
}

static bool __fuzzy_equals_radv_begin_update_as(const void *_a, const void *_b) {
   const struct trace_radv_begin_update_as *__entry_a =
      (const struct trace_radv_begin_update_as *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_update_as *__entry_b =
      (const struct trace_radv_begin_update_as *)_b;
   (void)__entry_b;
   if (__entry_a->build_flags != __entry_b->build_flags) return false;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_update_as = {
    "radv_begin_update_as",
    ALIGN_POT(sizeof(struct trace_radv_begin_update_as), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    32,
    u_tracepoint_type_begin_range,
    __print_radv_begin_update_as,
    __print_json_radv_begin_update_as,
    __fuzzy_hash_radv_begin_update_as,
    __fuzzy_equals_radv_begin_update_as,
    __print_fuzzy_hash_args_radv_begin_update_as,
};
void __trace_radv_begin_update_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
) {
   struct trace_radv_begin_update_as entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_update_as *__entry =
      queueing ?
      (struct trace_radv_begin_update_as *)u_trace_appendv(ut, cs, &__tp_radv_begin_update_as,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->build_flags = build_flags;
}

/*
 * radv_end_update_as
 */
  
#define __print_radv_end_update_as NULL
  
#define __print_json_radv_end_update_as NULL
#define __print_fuzzy_hash_args_radv_end_update_as NULL

#define __fuzzy_hash_radv_end_update_as NULL
#define __fuzzy_equals_radv_end_update_as NULL
#define __print_fuzzy_hash_args_radv_end_update_as NULL
static const struct u_tracepoint __tp_radv_end_update_as = {
    "radv_end_update_as",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    33,
    u_tracepoint_type_end_range,
    __print_radv_end_update_as,
    __print_json_radv_end_update_as,
    __fuzzy_hash_radv_end_update_as,
    __fuzzy_equals_radv_end_update_as,
    __print_fuzzy_hash_args_radv_end_update_as,
};
void __trace_radv_end_update_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_update_as entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_update_as *__entry =
      queueing ?
      (struct trace_radv_end_update_as *)u_trace_appendv(ut, cs, &__tp_radv_end_update_as,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_encode_as
 */
  
static void __print_radv_begin_encode_as(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_as *__entry =
      (const struct trace_radv_begin_encode_as *)arg;
   fprintf(out, ""
      "build_flags=0x%x, "
         "\n"
   ,__entry->build_flags
   );
}

  
static void __print_json_radv_begin_encode_as(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_as *__entry =
      (const struct trace_radv_begin_encode_as *)arg;
   fprintf(out, ""
      "\"build_flags\": \"0x%x\""
   ,__entry->build_flags
   );
}

static void __print_fuzzy_hash_args_radv_begin_encode_as(FILE *out, const void *arg) {
   const struct trace_radv_begin_encode_as *__entry =
      (const struct trace_radv_begin_encode_as *)arg;
   (void)__entry;
   fprintf(out, ""
      "build_flags=0x%x, "
      , __entry->build_flags
   );
}

static uint32_t __fuzzy_hash_radv_begin_encode_as(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_encode_as *__entry =
      (const struct trace_radv_begin_encode_as *)_event;
   (void)__entry;
   hash *= (__entry->build_flags);
   return hash;
}

static bool __fuzzy_equals_radv_begin_encode_as(const void *_a, const void *_b) {
   const struct trace_radv_begin_encode_as *__entry_a =
      (const struct trace_radv_begin_encode_as *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_encode_as *__entry_b =
      (const struct trace_radv_begin_encode_as *)_b;
   (void)__entry_b;
   if (__entry_a->build_flags != __entry_b->build_flags) return false;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_encode_as = {
    "radv_begin_encode_as",
    ALIGN_POT(sizeof(struct trace_radv_begin_encode_as), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    34,
    u_tracepoint_type_begin_range,
    __print_radv_begin_encode_as,
    __print_json_radv_begin_encode_as,
    __fuzzy_hash_radv_begin_encode_as,
    __fuzzy_equals_radv_begin_encode_as,
    __print_fuzzy_hash_args_radv_begin_encode_as,
};
void __trace_radv_begin_encode_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
) {
   struct trace_radv_begin_encode_as entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_encode_as *__entry =
      queueing ?
      (struct trace_radv_begin_encode_as *)u_trace_appendv(ut, cs, &__tp_radv_begin_encode_as,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->build_flags = build_flags;
}

/*
 * radv_end_encode_as
 */
  
#define __print_radv_end_encode_as NULL
  
#define __print_json_radv_end_encode_as NULL
#define __print_fuzzy_hash_args_radv_end_encode_as NULL

#define __fuzzy_hash_radv_end_encode_as NULL
#define __fuzzy_equals_radv_end_encode_as NULL
#define __print_fuzzy_hash_args_radv_end_encode_as NULL
static const struct u_tracepoint __tp_radv_end_encode_as = {
    "radv_end_encode_as",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    35,
    u_tracepoint_type_end_range,
    __print_radv_end_encode_as,
    __print_json_radv_end_encode_as,
    __fuzzy_hash_radv_end_encode_as,
    __fuzzy_equals_radv_end_encode_as,
    __print_fuzzy_hash_args_radv_end_encode_as,
};
void __trace_radv_end_encode_as(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_encode_as entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_encode_as *__entry =
      queueing ?
      (struct trace_radv_end_encode_as *)u_trace_appendv(ut, cs, &__tp_radv_end_encode_as,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_encode_triangles
 */
  
static void __print_radv_begin_encode_triangles(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_triangles *__entry =
      (const struct trace_radv_begin_encode_triangles *)arg;
   fprintf(out, ""
      "build_flags=0x%x, "
         "\n"
   ,__entry->build_flags
   );
}

  
static void __print_json_radv_begin_encode_triangles(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_triangles *__entry =
      (const struct trace_radv_begin_encode_triangles *)arg;
   fprintf(out, ""
      "\"build_flags\": \"0x%x\""
   ,__entry->build_flags
   );
}

static void __print_fuzzy_hash_args_radv_begin_encode_triangles(FILE *out, const void *arg) {
   const struct trace_radv_begin_encode_triangles *__entry =
      (const struct trace_radv_begin_encode_triangles *)arg;
   (void)__entry;
   fprintf(out, ""
      "build_flags=0x%x, "
      , __entry->build_flags
   );
}

static uint32_t __fuzzy_hash_radv_begin_encode_triangles(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_encode_triangles *__entry =
      (const struct trace_radv_begin_encode_triangles *)_event;
   (void)__entry;
   hash *= (__entry->build_flags);
   return hash;
}

static bool __fuzzy_equals_radv_begin_encode_triangles(const void *_a, const void *_b) {
   const struct trace_radv_begin_encode_triangles *__entry_a =
      (const struct trace_radv_begin_encode_triangles *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_encode_triangles *__entry_b =
      (const struct trace_radv_begin_encode_triangles *)_b;
   (void)__entry_b;
   if (__entry_a->build_flags != __entry_b->build_flags) return false;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_encode_triangles = {
    "radv_begin_encode_triangles",
    ALIGN_POT(sizeof(struct trace_radv_begin_encode_triangles), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    36,
    u_tracepoint_type_begin_range,
    __print_radv_begin_encode_triangles,
    __print_json_radv_begin_encode_triangles,
    __fuzzy_hash_radv_begin_encode_triangles,
    __fuzzy_equals_radv_begin_encode_triangles,
    __print_fuzzy_hash_args_radv_begin_encode_triangles,
};
void __trace_radv_begin_encode_triangles(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
) {
   struct trace_radv_begin_encode_triangles entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_encode_triangles *__entry =
      queueing ?
      (struct trace_radv_begin_encode_triangles *)u_trace_appendv(ut, cs, &__tp_radv_begin_encode_triangles,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->build_flags = build_flags;
}

/*
 * radv_end_encode_triangles
 */
  
#define __print_radv_end_encode_triangles NULL
  
#define __print_json_radv_end_encode_triangles NULL
#define __print_fuzzy_hash_args_radv_end_encode_triangles NULL

#define __fuzzy_hash_radv_end_encode_triangles NULL
#define __fuzzy_equals_radv_end_encode_triangles NULL
#define __print_fuzzy_hash_args_radv_end_encode_triangles NULL
static const struct u_tracepoint __tp_radv_end_encode_triangles = {
    "radv_end_encode_triangles",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    37,
    u_tracepoint_type_end_range,
    __print_radv_end_encode_triangles,
    __print_json_radv_end_encode_triangles,
    __fuzzy_hash_radv_end_encode_triangles,
    __fuzzy_equals_radv_end_encode_triangles,
    __print_fuzzy_hash_args_radv_end_encode_triangles,
};
void __trace_radv_end_encode_triangles(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_encode_triangles entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_encode_triangles *__entry =
      queueing ?
      (struct trace_radv_end_encode_triangles *)u_trace_appendv(ut, cs, &__tp_radv_end_encode_triangles,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_encode_triangles_retry
 */
  
static void __print_radv_begin_encode_triangles_retry(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_triangles_retry *__entry =
      (const struct trace_radv_begin_encode_triangles_retry *)arg;
   fprintf(out, ""
      "build_flags=0x%x, "
         "\n"
   ,__entry->build_flags
   );
}

  
static void __print_json_radv_begin_encode_triangles_retry(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_encode_triangles_retry *__entry =
      (const struct trace_radv_begin_encode_triangles_retry *)arg;
   fprintf(out, ""
      "\"build_flags\": \"0x%x\""
   ,__entry->build_flags
   );
}

static void __print_fuzzy_hash_args_radv_begin_encode_triangles_retry(FILE *out, const void *arg) {
   const struct trace_radv_begin_encode_triangles_retry *__entry =
      (const struct trace_radv_begin_encode_triangles_retry *)arg;
   (void)__entry;
   fprintf(out, ""
      "build_flags=0x%x, "
      , __entry->build_flags
   );
}

static uint32_t __fuzzy_hash_radv_begin_encode_triangles_retry(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_encode_triangles_retry *__entry =
      (const struct trace_radv_begin_encode_triangles_retry *)_event;
   (void)__entry;
   hash *= (__entry->build_flags);
   return hash;
}

static bool __fuzzy_equals_radv_begin_encode_triangles_retry(const void *_a, const void *_b) {
   const struct trace_radv_begin_encode_triangles_retry *__entry_a =
      (const struct trace_radv_begin_encode_triangles_retry *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_encode_triangles_retry *__entry_b =
      (const struct trace_radv_begin_encode_triangles_retry *)_b;
   (void)__entry_b;
   if (__entry_a->build_flags != __entry_b->build_flags) return false;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_encode_triangles_retry = {
    "radv_begin_encode_triangles_retry",
    ALIGN_POT(sizeof(struct trace_radv_begin_encode_triangles_retry), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    38,
    u_tracepoint_type_begin_range,
    __print_radv_begin_encode_triangles_retry,
    __print_json_radv_begin_encode_triangles_retry,
    __fuzzy_hash_radv_begin_encode_triangles_retry,
    __fuzzy_equals_radv_begin_encode_triangles_retry,
    __print_fuzzy_hash_args_radv_begin_encode_triangles_retry,
};
void __trace_radv_begin_encode_triangles_retry(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
) {
   struct trace_radv_begin_encode_triangles_retry entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_encode_triangles_retry *__entry =
      queueing ?
      (struct trace_radv_begin_encode_triangles_retry *)u_trace_appendv(ut, cs, &__tp_radv_begin_encode_triangles_retry,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->build_flags = build_flags;
}

/*
 * radv_end_encode_triangles_retry
 */
  
#define __print_radv_end_encode_triangles_retry NULL
  
#define __print_json_radv_end_encode_triangles_retry NULL
#define __print_fuzzy_hash_args_radv_end_encode_triangles_retry NULL

#define __fuzzy_hash_radv_end_encode_triangles_retry NULL
#define __fuzzy_equals_radv_end_encode_triangles_retry NULL
#define __print_fuzzy_hash_args_radv_end_encode_triangles_retry NULL
static const struct u_tracepoint __tp_radv_end_encode_triangles_retry = {
    "radv_end_encode_triangles_retry",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    39,
    u_tracepoint_type_end_range,
    __print_radv_end_encode_triangles_retry,
    __print_json_radv_end_encode_triangles_retry,
    __fuzzy_hash_radv_end_encode_triangles_retry,
    __fuzzy_equals_radv_end_encode_triangles_retry,
    __print_fuzzy_hash_args_radv_end_encode_triangles_retry,
};
void __trace_radv_end_encode_triangles_retry(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_encode_triangles_retry entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_encode_triangles_retry *__entry =
      queueing ?
      (struct trace_radv_end_encode_triangles_retry *)u_trace_appendv(ut, cs, &__tp_radv_end_encode_triangles_retry,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_init_header
 */
  
static void __print_radv_begin_init_header(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_init_header *__entry =
      (const struct trace_radv_begin_init_header *)arg;
   fprintf(out, ""
      "build_flags=0x%x, "
         "\n"
   ,__entry->build_flags
   );
}

  
static void __print_json_radv_begin_init_header(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_init_header *__entry =
      (const struct trace_radv_begin_init_header *)arg;
   fprintf(out, ""
      "\"build_flags\": \"0x%x\""
   ,__entry->build_flags
   );
}

static void __print_fuzzy_hash_args_radv_begin_init_header(FILE *out, const void *arg) {
   const struct trace_radv_begin_init_header *__entry =
      (const struct trace_radv_begin_init_header *)arg;
   (void)__entry;
   fprintf(out, ""
      "build_flags=0x%x, "
      , __entry->build_flags
   );
}

static uint32_t __fuzzy_hash_radv_begin_init_header(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_init_header *__entry =
      (const struct trace_radv_begin_init_header *)_event;
   (void)__entry;
   hash *= (__entry->build_flags);
   return hash;
}

static bool __fuzzy_equals_radv_begin_init_header(const void *_a, const void *_b) {
   const struct trace_radv_begin_init_header *__entry_a =
      (const struct trace_radv_begin_init_header *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_init_header *__entry_b =
      (const struct trace_radv_begin_init_header *)_b;
   (void)__entry_b;
   if (__entry_a->build_flags != __entry_b->build_flags) return false;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_init_header = {
    "radv_begin_init_header",
    ALIGN_POT(sizeof(struct trace_radv_begin_init_header), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    40,
    u_tracepoint_type_begin_range,
    __print_radv_begin_init_header,
    __print_json_radv_begin_init_header,
    __fuzzy_hash_radv_begin_init_header,
    __fuzzy_equals_radv_begin_init_header,
    __print_fuzzy_hash_args_radv_begin_init_header,
};
void __trace_radv_begin_init_header(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t build_flags
) {
   struct trace_radv_begin_init_header entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_init_header *__entry =
      queueing ?
      (struct trace_radv_begin_init_header *)u_trace_appendv(ut, cs, &__tp_radv_begin_init_header,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->build_flags = build_flags;
}

/*
 * radv_end_init_header
 */
  
#define __print_radv_end_init_header NULL
  
#define __print_json_radv_end_init_header NULL
#define __print_fuzzy_hash_args_radv_end_init_header NULL

#define __fuzzy_hash_radv_end_init_header NULL
#define __fuzzy_equals_radv_end_init_header NULL
#define __print_fuzzy_hash_args_radv_end_init_header NULL
static const struct u_tracepoint __tp_radv_end_init_header = {
    "radv_end_init_header",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    41,
    u_tracepoint_type_end_range,
    __print_radv_end_init_header,
    __print_json_radv_end_init_header,
    __fuzzy_hash_radv_end_init_header,
    __fuzzy_equals_radv_end_init_header,
    __print_fuzzy_hash_args_radv_end_init_header,
};
void __trace_radv_end_init_header(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_init_header entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_init_header *__entry =
      queueing ?
      (struct trace_radv_end_init_header *)u_trace_appendv(ut, cs, &__tp_radv_end_init_header,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_Draw
 */
  
static void __print_radv_begin_Draw(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_Draw *__entry =
      (const struct trace_radv_begin_Draw *)arg;
   fprintf(out, ""
      "vertex_count=%u, "
      "instance_count=%u, "
         "\n"
   ,__entry->vertex_count
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_Draw(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_Draw *__entry =
      (const struct trace_radv_begin_Draw *)arg;
   fprintf(out, ""
      "\"vertex_count\": \"%u\""
         ", "
      "\"instance_count\": \"%u\""
   ,__entry->vertex_count
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_Draw NULL

static uint32_t __fuzzy_hash_radv_begin_Draw(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_Draw *__entry =
      (const struct trace_radv_begin_Draw *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_Draw(const void *_a, const void *_b) {
   const struct trace_radv_begin_Draw *__entry_a =
      (const struct trace_radv_begin_Draw *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_Draw *__entry_b =
      (const struct trace_radv_begin_Draw *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_Draw = {
    "radv_begin_Draw",
    ALIGN_POT(sizeof(struct trace_radv_begin_Draw), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    42,
    u_tracepoint_type_begin_range,
    __print_radv_begin_Draw,
    __print_json_radv_begin_Draw,
    __fuzzy_hash_radv_begin_Draw,
    __fuzzy_equals_radv_begin_Draw,
    __print_fuzzy_hash_args_radv_begin_Draw,
};
void __trace_radv_begin_Draw(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t vertex_count
   , uint32_t instance_count
) {
   struct trace_radv_begin_Draw entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_Draw *__entry =
      queueing ?
      (struct trace_radv_begin_Draw *)u_trace_appendv(ut, cs, &__tp_radv_begin_Draw,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->vertex_count = vertex_count;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_Draw
 */
  
#define __print_radv_end_Draw NULL
  
#define __print_json_radv_end_Draw NULL
#define __print_fuzzy_hash_args_radv_end_Draw NULL

#define __fuzzy_hash_radv_end_Draw NULL
#define __fuzzy_equals_radv_end_Draw NULL
#define __print_fuzzy_hash_args_radv_end_Draw NULL
static const struct u_tracepoint __tp_radv_end_Draw = {
    "radv_end_Draw",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    43,
    u_tracepoint_type_end_range,
    __print_radv_end_Draw,
    __print_json_radv_end_Draw,
    __fuzzy_hash_radv_end_Draw,
    __fuzzy_equals_radv_end_Draw,
    __print_fuzzy_hash_args_radv_end_Draw,
};
void __trace_radv_end_Draw(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_Draw entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_Draw *__entry =
      queueing ?
      (struct trace_radv_end_Draw *)u_trace_appendv(ut, cs, &__tp_radv_end_Draw,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndexed
 */
  
static void __print_radv_begin_DrawIndexed(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexed *__entry =
      (const struct trace_radv_begin_DrawIndexed *)arg;
   fprintf(out, ""
      "index_count=%u, "
      "instance_count=%u, "
         "\n"
   ,__entry->index_count
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_DrawIndexed(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexed *__entry =
      (const struct trace_radv_begin_DrawIndexed *)arg;
   fprintf(out, ""
      "\"index_count\": \"%u\""
         ", "
      "\"instance_count\": \"%u\""
   ,__entry->index_count
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndexed NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndexed(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndexed *__entry =
      (const struct trace_radv_begin_DrawIndexed *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndexed(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndexed *__entry_a =
      (const struct trace_radv_begin_DrawIndexed *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndexed *__entry_b =
      (const struct trace_radv_begin_DrawIndexed *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndexed = {
    "radv_begin_DrawIndexed",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndexed), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    44,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndexed,
    __print_json_radv_begin_DrawIndexed,
    __fuzzy_hash_radv_begin_DrawIndexed,
    __fuzzy_equals_radv_begin_DrawIndexed,
    __print_fuzzy_hash_args_radv_begin_DrawIndexed,
};
void __trace_radv_begin_DrawIndexed(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t index_count
   , uint32_t instance_count
) {
   struct trace_radv_begin_DrawIndexed entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndexed *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndexed *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndexed,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->index_count = index_count;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_DrawIndexed
 */
  
#define __print_radv_end_DrawIndexed NULL
  
#define __print_json_radv_end_DrawIndexed NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexed NULL

#define __fuzzy_hash_radv_end_DrawIndexed NULL
#define __fuzzy_equals_radv_end_DrawIndexed NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexed NULL
static const struct u_tracepoint __tp_radv_end_DrawIndexed = {
    "radv_end_DrawIndexed",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    45,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndexed,
    __print_json_radv_end_DrawIndexed,
    __fuzzy_hash_radv_end_DrawIndexed,
    __fuzzy_equals_radv_end_DrawIndexed,
    __print_fuzzy_hash_args_radv_end_DrawIndexed,
};
void __trace_radv_end_DrawIndexed(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndexed entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndexed *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndexed *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndexed,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMultiEXT
 */
  
static void __print_radv_begin_DrawMultiEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMultiEXT *__entry =
      (const struct trace_radv_begin_DrawMultiEXT *)arg;
   fprintf(out, ""
      "draw_count=%u, "
      "instance_count=%u, "
         "\n"
   ,__entry->draw_count
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_DrawMultiEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMultiEXT *__entry =
      (const struct trace_radv_begin_DrawMultiEXT *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
         ", "
      "\"instance_count\": \"%u\""
   ,__entry->draw_count
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMultiEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMultiEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMultiEXT *__entry =
      (const struct trace_radv_begin_DrawMultiEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMultiEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMultiEXT *__entry_a =
      (const struct trace_radv_begin_DrawMultiEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMultiEXT *__entry_b =
      (const struct trace_radv_begin_DrawMultiEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMultiEXT = {
    "radv_begin_DrawMultiEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMultiEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    46,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMultiEXT,
    __print_json_radv_begin_DrawMultiEXT,
    __fuzzy_hash_radv_begin_DrawMultiEXT,
    __fuzzy_equals_radv_begin_DrawMultiEXT,
    __print_fuzzy_hash_args_radv_begin_DrawMultiEXT,
};
void __trace_radv_begin_DrawMultiEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
) {
   struct trace_radv_begin_DrawMultiEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMultiEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMultiEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMultiEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_DrawMultiEXT
 */
  
#define __print_radv_end_DrawMultiEXT NULL
  
#define __print_json_radv_end_DrawMultiEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMultiEXT NULL

#define __fuzzy_hash_radv_end_DrawMultiEXT NULL
#define __fuzzy_equals_radv_end_DrawMultiEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMultiEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMultiEXT = {
    "radv_end_DrawMultiEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    47,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMultiEXT,
    __print_json_radv_end_DrawMultiEXT,
    __fuzzy_hash_radv_end_DrawMultiEXT,
    __fuzzy_equals_radv_end_DrawMultiEXT,
    __print_fuzzy_hash_args_radv_end_DrawMultiEXT,
};
void __trace_radv_end_DrawMultiEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMultiEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMultiEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMultiEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMultiEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMultiIndexedEXT
 */
  
static void __print_radv_begin_DrawMultiIndexedEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMultiIndexedEXT *__entry =
      (const struct trace_radv_begin_DrawMultiIndexedEXT *)arg;
   fprintf(out, ""
      "draw_count=%u, "
      "instance_count=%u, "
         "\n"
   ,__entry->draw_count
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_DrawMultiIndexedEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMultiIndexedEXT *__entry =
      (const struct trace_radv_begin_DrawMultiIndexedEXT *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
         ", "
      "\"instance_count\": \"%u\""
   ,__entry->draw_count
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMultiIndexedEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMultiIndexedEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMultiIndexedEXT *__entry =
      (const struct trace_radv_begin_DrawMultiIndexedEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMultiIndexedEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMultiIndexedEXT *__entry_a =
      (const struct trace_radv_begin_DrawMultiIndexedEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMultiIndexedEXT *__entry_b =
      (const struct trace_radv_begin_DrawMultiIndexedEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMultiIndexedEXT = {
    "radv_begin_DrawMultiIndexedEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMultiIndexedEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    48,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMultiIndexedEXT,
    __print_json_radv_begin_DrawMultiIndexedEXT,
    __fuzzy_hash_radv_begin_DrawMultiIndexedEXT,
    __fuzzy_equals_radv_begin_DrawMultiIndexedEXT,
    __print_fuzzy_hash_args_radv_begin_DrawMultiIndexedEXT,
};
void __trace_radv_begin_DrawMultiIndexedEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
   , uint32_t instance_count
) {
   struct trace_radv_begin_DrawMultiIndexedEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMultiIndexedEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMultiIndexedEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMultiIndexedEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_DrawMultiIndexedEXT
 */
  
#define __print_radv_end_DrawMultiIndexedEXT NULL
  
#define __print_json_radv_end_DrawMultiIndexedEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMultiIndexedEXT NULL

#define __fuzzy_hash_radv_end_DrawMultiIndexedEXT NULL
#define __fuzzy_equals_radv_end_DrawMultiIndexedEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMultiIndexedEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMultiIndexedEXT = {
    "radv_end_DrawMultiIndexedEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    49,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMultiIndexedEXT,
    __print_json_radv_end_DrawMultiIndexedEXT,
    __fuzzy_hash_radv_end_DrawMultiIndexedEXT,
    __fuzzy_equals_radv_end_DrawMultiIndexedEXT,
    __print_fuzzy_hash_args_radv_end_DrawMultiIndexedEXT,
};
void __trace_radv_end_DrawMultiIndexedEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMultiIndexedEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMultiIndexedEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMultiIndexedEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMultiIndexedEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirect
 */
  
static void __print_radv_begin_DrawIndirect(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirect *__entry =
      (const struct trace_radv_begin_DrawIndirect *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndirect(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirect *__entry =
      (const struct trace_radv_begin_DrawIndirect *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirect NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirect(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirect *__entry =
      (const struct trace_radv_begin_DrawIndirect *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirect(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirect *__entry_a =
      (const struct trace_radv_begin_DrawIndirect *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirect *__entry_b =
      (const struct trace_radv_begin_DrawIndirect *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirect = {
    "radv_begin_DrawIndirect",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirect), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    50,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirect,
    __print_json_radv_begin_DrawIndirect,
    __fuzzy_hash_radv_begin_DrawIndirect,
    __fuzzy_equals_radv_begin_DrawIndirect,
    __print_fuzzy_hash_args_radv_begin_DrawIndirect,
};
void __trace_radv_begin_DrawIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirect *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirect *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawIndirect
 */
  
#define __print_radv_end_DrawIndirect NULL
  
#define __print_json_radv_end_DrawIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirect NULL

#define __fuzzy_hash_radv_end_DrawIndirect NULL
#define __fuzzy_equals_radv_end_DrawIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirect NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirect = {
    "radv_end_DrawIndirect",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    51,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirect,
    __print_json_radv_end_DrawIndirect,
    __fuzzy_hash_radv_end_DrawIndirect,
    __fuzzy_equals_radv_end_DrawIndirect,
    __print_fuzzy_hash_args_radv_end_DrawIndirect,
};
void __trace_radv_end_DrawIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirect *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirect *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndexedIndirect
 */
  
static void __print_radv_begin_DrawIndexedIndirect(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirect *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndexedIndirect(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirect *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirect NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndexedIndirect(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndexedIndirect *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndexedIndirect(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndexedIndirect *__entry_a =
      (const struct trace_radv_begin_DrawIndexedIndirect *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndexedIndirect *__entry_b =
      (const struct trace_radv_begin_DrawIndexedIndirect *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndexedIndirect = {
    "radv_begin_DrawIndexedIndirect",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndexedIndirect), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    52,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndexedIndirect,
    __print_json_radv_begin_DrawIndexedIndirect,
    __fuzzy_hash_radv_begin_DrawIndexedIndirect,
    __fuzzy_equals_radv_begin_DrawIndexedIndirect,
    __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirect,
};
void __trace_radv_begin_DrawIndexedIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawIndexedIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndexedIndirect *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndexedIndirect *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndexedIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawIndexedIndirect
 */
  
#define __print_radv_end_DrawIndexedIndirect NULL
  
#define __print_json_radv_end_DrawIndexedIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect NULL

#define __fuzzy_hash_radv_end_DrawIndexedIndirect NULL
#define __fuzzy_equals_radv_end_DrawIndexedIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect NULL
static const struct u_tracepoint __tp_radv_end_DrawIndexedIndirect = {
    "radv_end_DrawIndexedIndirect",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    53,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndexedIndirect,
    __print_json_radv_end_DrawIndexedIndirect,
    __fuzzy_hash_radv_end_DrawIndexedIndirect,
    __fuzzy_equals_radv_end_DrawIndexedIndirect,
    __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect,
};
void __trace_radv_end_DrawIndexedIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndexedIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndexedIndirect *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndexedIndirect *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndexedIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DispatchIndirect
 */
  
#define __print_radv_begin_DispatchIndirect NULL
  
#define __print_json_radv_begin_DispatchIndirect NULL
#define __print_fuzzy_hash_args_radv_begin_DispatchIndirect NULL

#define __fuzzy_hash_radv_begin_DispatchIndirect NULL
#define __fuzzy_equals_radv_begin_DispatchIndirect NULL
#define __print_fuzzy_hash_args_radv_begin_DispatchIndirect NULL
static const struct u_tracepoint __tp_radv_begin_DispatchIndirect = {
    "radv_begin_DispatchIndirect",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    54,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DispatchIndirect,
    __print_json_radv_begin_DispatchIndirect,
    __fuzzy_hash_radv_begin_DispatchIndirect,
    __fuzzy_equals_radv_begin_DispatchIndirect,
    __print_fuzzy_hash_args_radv_begin_DispatchIndirect,
};
void __trace_radv_begin_DispatchIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_DispatchIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DispatchIndirect *__entry =
      queueing ?
      (struct trace_radv_begin_DispatchIndirect *)u_trace_appendv(ut, cs, &__tp_radv_begin_DispatchIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_DispatchIndirect
 */
  
#define __print_radv_end_DispatchIndirect NULL
  
#define __print_json_radv_end_DispatchIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DispatchIndirect NULL

#define __fuzzy_hash_radv_end_DispatchIndirect NULL
#define __fuzzy_equals_radv_end_DispatchIndirect NULL
#define __print_fuzzy_hash_args_radv_end_DispatchIndirect NULL
static const struct u_tracepoint __tp_radv_end_DispatchIndirect = {
    "radv_end_DispatchIndirect",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    55,
    u_tracepoint_type_end_range,
    __print_radv_end_DispatchIndirect,
    __print_json_radv_end_DispatchIndirect,
    __fuzzy_hash_radv_end_DispatchIndirect,
    __fuzzy_equals_radv_end_DispatchIndirect,
    __print_fuzzy_hash_args_radv_end_DispatchIndirect,
};
void __trace_radv_end_DispatchIndirect(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DispatchIndirect entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DispatchIndirect *__entry =
      queueing ?
      (struct trace_radv_end_DispatchIndirect *)u_trace_appendv(ut, cs, &__tp_radv_end_DispatchIndirect,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyMemoryIndirectKHR
 */
  
#define __print_radv_begin_CopyMemoryIndirectKHR NULL
  
#define __print_json_radv_begin_CopyMemoryIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryIndirectKHR NULL

#define __fuzzy_hash_radv_begin_CopyMemoryIndirectKHR NULL
#define __fuzzy_equals_radv_begin_CopyMemoryIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryIndirectKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyMemoryIndirectKHR = {
    "radv_begin_CopyMemoryIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    56,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyMemoryIndirectKHR,
    __print_json_radv_begin_CopyMemoryIndirectKHR,
    __fuzzy_hash_radv_begin_CopyMemoryIndirectKHR,
    __fuzzy_equals_radv_begin_CopyMemoryIndirectKHR,
    __print_fuzzy_hash_args_radv_begin_CopyMemoryIndirectKHR,
};
void __trace_radv_begin_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyMemoryIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyMemoryIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyMemoryIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyMemoryIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyMemoryIndirectKHR
 */
  
#define __print_radv_end_CopyMemoryIndirectKHR NULL
  
#define __print_json_radv_end_CopyMemoryIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryIndirectKHR NULL

#define __fuzzy_hash_radv_end_CopyMemoryIndirectKHR NULL
#define __fuzzy_equals_radv_end_CopyMemoryIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryIndirectKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyMemoryIndirectKHR = {
    "radv_end_CopyMemoryIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    57,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyMemoryIndirectKHR,
    __print_json_radv_end_CopyMemoryIndirectKHR,
    __fuzzy_hash_radv_end_CopyMemoryIndirectKHR,
    __fuzzy_equals_radv_end_CopyMemoryIndirectKHR,
    __print_fuzzy_hash_args_radv_end_CopyMemoryIndirectKHR,
};
void __trace_radv_end_CopyMemoryIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyMemoryIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyMemoryIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyMemoryIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyMemoryIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyMemoryToImageIndirectKHR
 */
  
#define __print_radv_begin_CopyMemoryToImageIndirectKHR NULL
  
#define __print_json_radv_begin_CopyMemoryToImageIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageIndirectKHR NULL

#define __fuzzy_hash_radv_begin_CopyMemoryToImageIndirectKHR NULL
#define __fuzzy_equals_radv_begin_CopyMemoryToImageIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageIndirectKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyMemoryToImageIndirectKHR = {
    "radv_begin_CopyMemoryToImageIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    58,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyMemoryToImageIndirectKHR,
    __print_json_radv_begin_CopyMemoryToImageIndirectKHR,
    __fuzzy_hash_radv_begin_CopyMemoryToImageIndirectKHR,
    __fuzzy_equals_radv_begin_CopyMemoryToImageIndirectKHR,
    __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageIndirectKHR,
};
void __trace_radv_begin_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyMemoryToImageIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyMemoryToImageIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyMemoryToImageIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyMemoryToImageIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyMemoryToImageIndirectKHR
 */
  
#define __print_radv_end_CopyMemoryToImageIndirectKHR NULL
  
#define __print_json_radv_end_CopyMemoryToImageIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToImageIndirectKHR NULL

#define __fuzzy_hash_radv_end_CopyMemoryToImageIndirectKHR NULL
#define __fuzzy_equals_radv_end_CopyMemoryToImageIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToImageIndirectKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyMemoryToImageIndirectKHR = {
    "radv_end_CopyMemoryToImageIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    59,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyMemoryToImageIndirectKHR,
    __print_json_radv_end_CopyMemoryToImageIndirectKHR,
    __fuzzy_hash_radv_end_CopyMemoryToImageIndirectKHR,
    __fuzzy_equals_radv_end_CopyMemoryToImageIndirectKHR,
    __print_fuzzy_hash_args_radv_end_CopyMemoryToImageIndirectKHR,
};
void __trace_radv_end_CopyMemoryToImageIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyMemoryToImageIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyMemoryToImageIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyMemoryToImageIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyMemoryToImageIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_UpdateBuffer
 */
  
static void __print_radv_begin_UpdateBuffer(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_UpdateBuffer *__entry =
      (const struct trace_radv_begin_UpdateBuffer *)arg;
   fprintf(out, ""
      "size=%u, "
         "\n"
   ,__entry->size
   );
}

  
static void __print_json_radv_begin_UpdateBuffer(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_UpdateBuffer *__entry =
      (const struct trace_radv_begin_UpdateBuffer *)arg;
   fprintf(out, ""
      "\"size\": \"%u\""
   ,__entry->size
   );
}

#define __print_fuzzy_hash_args_radv_begin_UpdateBuffer NULL

static uint32_t __fuzzy_hash_radv_begin_UpdateBuffer(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_UpdateBuffer *__entry =
      (const struct trace_radv_begin_UpdateBuffer *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_UpdateBuffer(const void *_a, const void *_b) {
   const struct trace_radv_begin_UpdateBuffer *__entry_a =
      (const struct trace_radv_begin_UpdateBuffer *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_UpdateBuffer *__entry_b =
      (const struct trace_radv_begin_UpdateBuffer *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_UpdateBuffer = {
    "radv_begin_UpdateBuffer",
    ALIGN_POT(sizeof(struct trace_radv_begin_UpdateBuffer), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    60,
    u_tracepoint_type_begin_range,
    __print_radv_begin_UpdateBuffer,
    __print_json_radv_begin_UpdateBuffer,
    __fuzzy_hash_radv_begin_UpdateBuffer,
    __fuzzy_equals_radv_begin_UpdateBuffer,
    __print_fuzzy_hash_args_radv_begin_UpdateBuffer,
};
void __trace_radv_begin_UpdateBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
) {
   struct trace_radv_begin_UpdateBuffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_UpdateBuffer *__entry =
      queueing ?
      (struct trace_radv_begin_UpdateBuffer *)u_trace_appendv(ut, cs, &__tp_radv_begin_UpdateBuffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->size = size;
}

/*
 * radv_end_UpdateBuffer
 */
  
#define __print_radv_end_UpdateBuffer NULL
  
#define __print_json_radv_end_UpdateBuffer NULL
#define __print_fuzzy_hash_args_radv_end_UpdateBuffer NULL

#define __fuzzy_hash_radv_end_UpdateBuffer NULL
#define __fuzzy_equals_radv_end_UpdateBuffer NULL
#define __print_fuzzy_hash_args_radv_end_UpdateBuffer NULL
static const struct u_tracepoint __tp_radv_end_UpdateBuffer = {
    "radv_end_UpdateBuffer",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    61,
    u_tracepoint_type_end_range,
    __print_radv_end_UpdateBuffer,
    __print_json_radv_end_UpdateBuffer,
    __fuzzy_hash_radv_end_UpdateBuffer,
    __fuzzy_equals_radv_end_UpdateBuffer,
    __print_fuzzy_hash_args_radv_end_UpdateBuffer,
};
void __trace_radv_end_UpdateBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_UpdateBuffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_UpdateBuffer *__entry =
      queueing ?
      (struct trace_radv_end_UpdateBuffer *)u_trace_appendv(ut, cs, &__tp_radv_end_UpdateBuffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_FillBuffer
 */
  
static void __print_radv_begin_FillBuffer(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_FillBuffer *__entry =
      (const struct trace_radv_begin_FillBuffer *)arg;
   fprintf(out, ""
      "size=%u, "
         "\n"
   ,__entry->size
   );
}

  
static void __print_json_radv_begin_FillBuffer(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_FillBuffer *__entry =
      (const struct trace_radv_begin_FillBuffer *)arg;
   fprintf(out, ""
      "\"size\": \"%u\""
   ,__entry->size
   );
}

#define __print_fuzzy_hash_args_radv_begin_FillBuffer NULL

static uint32_t __fuzzy_hash_radv_begin_FillBuffer(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_FillBuffer *__entry =
      (const struct trace_radv_begin_FillBuffer *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_FillBuffer(const void *_a, const void *_b) {
   const struct trace_radv_begin_FillBuffer *__entry_a =
      (const struct trace_radv_begin_FillBuffer *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_FillBuffer *__entry_b =
      (const struct trace_radv_begin_FillBuffer *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_FillBuffer = {
    "radv_begin_FillBuffer",
    ALIGN_POT(sizeof(struct trace_radv_begin_FillBuffer), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    62,
    u_tracepoint_type_begin_range,
    __print_radv_begin_FillBuffer,
    __print_json_radv_begin_FillBuffer,
    __fuzzy_hash_radv_begin_FillBuffer,
    __fuzzy_equals_radv_begin_FillBuffer,
    __print_fuzzy_hash_args_radv_begin_FillBuffer,
};
void __trace_radv_begin_FillBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t size
) {
   struct trace_radv_begin_FillBuffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_FillBuffer *__entry =
      queueing ?
      (struct trace_radv_begin_FillBuffer *)u_trace_appendv(ut, cs, &__tp_radv_begin_FillBuffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->size = size;
}

/*
 * radv_end_FillBuffer
 */
  
#define __print_radv_end_FillBuffer NULL
  
#define __print_json_radv_end_FillBuffer NULL
#define __print_fuzzy_hash_args_radv_end_FillBuffer NULL

#define __fuzzy_hash_radv_end_FillBuffer NULL
#define __fuzzy_equals_radv_end_FillBuffer NULL
#define __print_fuzzy_hash_args_radv_end_FillBuffer NULL
static const struct u_tracepoint __tp_radv_end_FillBuffer = {
    "radv_end_FillBuffer",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    63,
    u_tracepoint_type_end_range,
    __print_radv_end_FillBuffer,
    __print_json_radv_end_FillBuffer,
    __fuzzy_hash_radv_end_FillBuffer,
    __fuzzy_equals_radv_end_FillBuffer,
    __print_fuzzy_hash_args_radv_end_FillBuffer,
};
void __trace_radv_end_FillBuffer(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_FillBuffer entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_FillBuffer *__entry =
      queueing ?
      (struct trace_radv_end_FillBuffer *)u_trace_appendv(ut, cs, &__tp_radv_end_FillBuffer,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ClearColorImage
 */
  
#define __print_radv_begin_ClearColorImage NULL
  
#define __print_json_radv_begin_ClearColorImage NULL
#define __print_fuzzy_hash_args_radv_begin_ClearColorImage NULL

#define __fuzzy_hash_radv_begin_ClearColorImage NULL
#define __fuzzy_equals_radv_begin_ClearColorImage NULL
#define __print_fuzzy_hash_args_radv_begin_ClearColorImage NULL
static const struct u_tracepoint __tp_radv_begin_ClearColorImage = {
    "radv_begin_ClearColorImage",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    64,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ClearColorImage,
    __print_json_radv_begin_ClearColorImage,
    __fuzzy_hash_radv_begin_ClearColorImage,
    __fuzzy_equals_radv_begin_ClearColorImage,
    __print_fuzzy_hash_args_radv_begin_ClearColorImage,
};
void __trace_radv_begin_ClearColorImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ClearColorImage entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ClearColorImage *__entry =
      queueing ?
      (struct trace_radv_begin_ClearColorImage *)u_trace_appendv(ut, cs, &__tp_radv_begin_ClearColorImage,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ClearColorImage
 */
  
#define __print_radv_end_ClearColorImage NULL
  
#define __print_json_radv_end_ClearColorImage NULL
#define __print_fuzzy_hash_args_radv_end_ClearColorImage NULL

#define __fuzzy_hash_radv_end_ClearColorImage NULL
#define __fuzzy_equals_radv_end_ClearColorImage NULL
#define __print_fuzzy_hash_args_radv_end_ClearColorImage NULL
static const struct u_tracepoint __tp_radv_end_ClearColorImage = {
    "radv_end_ClearColorImage",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    65,
    u_tracepoint_type_end_range,
    __print_radv_end_ClearColorImage,
    __print_json_radv_end_ClearColorImage,
    __fuzzy_hash_radv_end_ClearColorImage,
    __fuzzy_equals_radv_end_ClearColorImage,
    __print_fuzzy_hash_args_radv_end_ClearColorImage,
};
void __trace_radv_end_ClearColorImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ClearColorImage entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ClearColorImage *__entry =
      queueing ?
      (struct trace_radv_end_ClearColorImage *)u_trace_appendv(ut, cs, &__tp_radv_end_ClearColorImage,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ClearDepthStencilImage
 */
  
#define __print_radv_begin_ClearDepthStencilImage NULL
  
#define __print_json_radv_begin_ClearDepthStencilImage NULL
#define __print_fuzzy_hash_args_radv_begin_ClearDepthStencilImage NULL

#define __fuzzy_hash_radv_begin_ClearDepthStencilImage NULL
#define __fuzzy_equals_radv_begin_ClearDepthStencilImage NULL
#define __print_fuzzy_hash_args_radv_begin_ClearDepthStencilImage NULL
static const struct u_tracepoint __tp_radv_begin_ClearDepthStencilImage = {
    "radv_begin_ClearDepthStencilImage",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    66,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ClearDepthStencilImage,
    __print_json_radv_begin_ClearDepthStencilImage,
    __fuzzy_hash_radv_begin_ClearDepthStencilImage,
    __fuzzy_equals_radv_begin_ClearDepthStencilImage,
    __print_fuzzy_hash_args_radv_begin_ClearDepthStencilImage,
};
void __trace_radv_begin_ClearDepthStencilImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ClearDepthStencilImage entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ClearDepthStencilImage *__entry =
      queueing ?
      (struct trace_radv_begin_ClearDepthStencilImage *)u_trace_appendv(ut, cs, &__tp_radv_begin_ClearDepthStencilImage,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ClearDepthStencilImage
 */
  
#define __print_radv_end_ClearDepthStencilImage NULL
  
#define __print_json_radv_end_ClearDepthStencilImage NULL
#define __print_fuzzy_hash_args_radv_end_ClearDepthStencilImage NULL

#define __fuzzy_hash_radv_end_ClearDepthStencilImage NULL
#define __fuzzy_equals_radv_end_ClearDepthStencilImage NULL
#define __print_fuzzy_hash_args_radv_end_ClearDepthStencilImage NULL
static const struct u_tracepoint __tp_radv_end_ClearDepthStencilImage = {
    "radv_end_ClearDepthStencilImage",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    67,
    u_tracepoint_type_end_range,
    __print_radv_end_ClearDepthStencilImage,
    __print_json_radv_end_ClearDepthStencilImage,
    __fuzzy_hash_radv_end_ClearDepthStencilImage,
    __fuzzy_equals_radv_end_ClearDepthStencilImage,
    __print_fuzzy_hash_args_radv_end_ClearDepthStencilImage,
};
void __trace_radv_end_ClearDepthStencilImage(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ClearDepthStencilImage entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ClearDepthStencilImage *__entry =
      queueing ?
      (struct trace_radv_end_ClearDepthStencilImage *)u_trace_appendv(ut, cs, &__tp_radv_end_ClearDepthStencilImage,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ClearAttachments
 */
  
#define __print_radv_begin_ClearAttachments NULL
  
#define __print_json_radv_begin_ClearAttachments NULL
#define __print_fuzzy_hash_args_radv_begin_ClearAttachments NULL

#define __fuzzy_hash_radv_begin_ClearAttachments NULL
#define __fuzzy_equals_radv_begin_ClearAttachments NULL
#define __print_fuzzy_hash_args_radv_begin_ClearAttachments NULL
static const struct u_tracepoint __tp_radv_begin_ClearAttachments = {
    "radv_begin_ClearAttachments",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    68,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ClearAttachments,
    __print_json_radv_begin_ClearAttachments,
    __fuzzy_hash_radv_begin_ClearAttachments,
    __fuzzy_equals_radv_begin_ClearAttachments,
    __print_fuzzy_hash_args_radv_begin_ClearAttachments,
};
void __trace_radv_begin_ClearAttachments(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ClearAttachments entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ClearAttachments *__entry =
      queueing ?
      (struct trace_radv_begin_ClearAttachments *)u_trace_appendv(ut, cs, &__tp_radv_begin_ClearAttachments,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ClearAttachments
 */
  
#define __print_radv_end_ClearAttachments NULL
  
#define __print_json_radv_end_ClearAttachments NULL
#define __print_fuzzy_hash_args_radv_end_ClearAttachments NULL

#define __fuzzy_hash_radv_end_ClearAttachments NULL
#define __fuzzy_equals_radv_end_ClearAttachments NULL
#define __print_fuzzy_hash_args_radv_end_ClearAttachments NULL
static const struct u_tracepoint __tp_radv_end_ClearAttachments = {
    "radv_end_ClearAttachments",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    69,
    u_tracepoint_type_end_range,
    __print_radv_end_ClearAttachments,
    __print_json_radv_end_ClearAttachments,
    __fuzzy_hash_radv_end_ClearAttachments,
    __fuzzy_equals_radv_end_ClearAttachments,
    __print_fuzzy_hash_args_radv_end_ClearAttachments,
};
void __trace_radv_end_ClearAttachments(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ClearAttachments entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ClearAttachments *__entry =
      queueing ?
      (struct trace_radv_end_ClearAttachments *)u_trace_appendv(ut, cs, &__tp_radv_end_ClearAttachments,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ResetQueryPool
 */
  
#define __print_radv_begin_ResetQueryPool NULL
  
#define __print_json_radv_begin_ResetQueryPool NULL
#define __print_fuzzy_hash_args_radv_begin_ResetQueryPool NULL

#define __fuzzy_hash_radv_begin_ResetQueryPool NULL
#define __fuzzy_equals_radv_begin_ResetQueryPool NULL
#define __print_fuzzy_hash_args_radv_begin_ResetQueryPool NULL
static const struct u_tracepoint __tp_radv_begin_ResetQueryPool = {
    "radv_begin_ResetQueryPool",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    70,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ResetQueryPool,
    __print_json_radv_begin_ResetQueryPool,
    __fuzzy_hash_radv_begin_ResetQueryPool,
    __fuzzy_equals_radv_begin_ResetQueryPool,
    __print_fuzzy_hash_args_radv_begin_ResetQueryPool,
};
void __trace_radv_begin_ResetQueryPool(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ResetQueryPool entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ResetQueryPool *__entry =
      queueing ?
      (struct trace_radv_begin_ResetQueryPool *)u_trace_appendv(ut, cs, &__tp_radv_begin_ResetQueryPool,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ResetQueryPool
 */
  
#define __print_radv_end_ResetQueryPool NULL
  
#define __print_json_radv_end_ResetQueryPool NULL
#define __print_fuzzy_hash_args_radv_end_ResetQueryPool NULL

#define __fuzzy_hash_radv_end_ResetQueryPool NULL
#define __fuzzy_equals_radv_end_ResetQueryPool NULL
#define __print_fuzzy_hash_args_radv_end_ResetQueryPool NULL
static const struct u_tracepoint __tp_radv_end_ResetQueryPool = {
    "radv_end_ResetQueryPool",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    71,
    u_tracepoint_type_end_range,
    __print_radv_end_ResetQueryPool,
    __print_json_radv_end_ResetQueryPool,
    __fuzzy_hash_radv_end_ResetQueryPool,
    __fuzzy_equals_radv_end_ResetQueryPool,
    __print_fuzzy_hash_args_radv_end_ResetQueryPool,
};
void __trace_radv_end_ResetQueryPool(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ResetQueryPool entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ResetQueryPool *__entry =
      queueing ?
      (struct trace_radv_end_ResetQueryPool *)u_trace_appendv(ut, cs, &__tp_radv_end_ResetQueryPool,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyQueryPoolResults
 */
  
#define __print_radv_begin_CopyQueryPoolResults NULL
  
#define __print_json_radv_begin_CopyQueryPoolResults NULL
#define __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResults NULL

#define __fuzzy_hash_radv_begin_CopyQueryPoolResults NULL
#define __fuzzy_equals_radv_begin_CopyQueryPoolResults NULL
#define __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResults NULL
static const struct u_tracepoint __tp_radv_begin_CopyQueryPoolResults = {
    "radv_begin_CopyQueryPoolResults",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    72,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyQueryPoolResults,
    __print_json_radv_begin_CopyQueryPoolResults,
    __fuzzy_hash_radv_begin_CopyQueryPoolResults,
    __fuzzy_equals_radv_begin_CopyQueryPoolResults,
    __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResults,
};
void __trace_radv_begin_CopyQueryPoolResults(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyQueryPoolResults entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyQueryPoolResults *__entry =
      queueing ?
      (struct trace_radv_begin_CopyQueryPoolResults *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyQueryPoolResults,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyQueryPoolResults
 */
  
#define __print_radv_end_CopyQueryPoolResults NULL
  
#define __print_json_radv_end_CopyQueryPoolResults NULL
#define __print_fuzzy_hash_args_radv_end_CopyQueryPoolResults NULL

#define __fuzzy_hash_radv_end_CopyQueryPoolResults NULL
#define __fuzzy_equals_radv_end_CopyQueryPoolResults NULL
#define __print_fuzzy_hash_args_radv_end_CopyQueryPoolResults NULL
static const struct u_tracepoint __tp_radv_end_CopyQueryPoolResults = {
    "radv_end_CopyQueryPoolResults",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    73,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyQueryPoolResults,
    __print_json_radv_end_CopyQueryPoolResults,
    __fuzzy_hash_radv_end_CopyQueryPoolResults,
    __fuzzy_equals_radv_end_CopyQueryPoolResults,
    __print_fuzzy_hash_args_radv_end_CopyQueryPoolResults,
};
void __trace_radv_end_CopyQueryPoolResults(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyQueryPoolResults entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyQueryPoolResults *__entry =
      queueing ?
      (struct trace_radv_end_CopyQueryPoolResults *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyQueryPoolResults,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ExecuteGeneratedCommandsEXT
 */
  
#define __print_radv_begin_ExecuteGeneratedCommandsEXT NULL
  
#define __print_json_radv_begin_ExecuteGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_begin_ExecuteGeneratedCommandsEXT NULL

#define __fuzzy_hash_radv_begin_ExecuteGeneratedCommandsEXT NULL
#define __fuzzy_equals_radv_begin_ExecuteGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_begin_ExecuteGeneratedCommandsEXT NULL
static const struct u_tracepoint __tp_radv_begin_ExecuteGeneratedCommandsEXT = {
    "radv_begin_ExecuteGeneratedCommandsEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    74,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ExecuteGeneratedCommandsEXT,
    __print_json_radv_begin_ExecuteGeneratedCommandsEXT,
    __fuzzy_hash_radv_begin_ExecuteGeneratedCommandsEXT,
    __fuzzy_equals_radv_begin_ExecuteGeneratedCommandsEXT,
    __print_fuzzy_hash_args_radv_begin_ExecuteGeneratedCommandsEXT,
};
void __trace_radv_begin_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ExecuteGeneratedCommandsEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ExecuteGeneratedCommandsEXT *__entry =
      queueing ?
      (struct trace_radv_begin_ExecuteGeneratedCommandsEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_ExecuteGeneratedCommandsEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ExecuteGeneratedCommandsEXT
 */
  
#define __print_radv_end_ExecuteGeneratedCommandsEXT NULL
  
#define __print_json_radv_end_ExecuteGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_end_ExecuteGeneratedCommandsEXT NULL

#define __fuzzy_hash_radv_end_ExecuteGeneratedCommandsEXT NULL
#define __fuzzy_equals_radv_end_ExecuteGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_end_ExecuteGeneratedCommandsEXT NULL
static const struct u_tracepoint __tp_radv_end_ExecuteGeneratedCommandsEXT = {
    "radv_end_ExecuteGeneratedCommandsEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    75,
    u_tracepoint_type_end_range,
    __print_radv_end_ExecuteGeneratedCommandsEXT,
    __print_json_radv_end_ExecuteGeneratedCommandsEXT,
    __fuzzy_hash_radv_end_ExecuteGeneratedCommandsEXT,
    __fuzzy_equals_radv_end_ExecuteGeneratedCommandsEXT,
    __print_fuzzy_hash_args_radv_end_ExecuteGeneratedCommandsEXT,
};
void __trace_radv_end_ExecuteGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ExecuteGeneratedCommandsEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ExecuteGeneratedCommandsEXT *__entry =
      queueing ?
      (struct trace_radv_end_ExecuteGeneratedCommandsEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_ExecuteGeneratedCommandsEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_PreprocessGeneratedCommandsEXT
 */
  
#define __print_radv_begin_PreprocessGeneratedCommandsEXT NULL
  
#define __print_json_radv_begin_PreprocessGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_begin_PreprocessGeneratedCommandsEXT NULL

#define __fuzzy_hash_radv_begin_PreprocessGeneratedCommandsEXT NULL
#define __fuzzy_equals_radv_begin_PreprocessGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_begin_PreprocessGeneratedCommandsEXT NULL
static const struct u_tracepoint __tp_radv_begin_PreprocessGeneratedCommandsEXT = {
    "radv_begin_PreprocessGeneratedCommandsEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    76,
    u_tracepoint_type_begin_range,
    __print_radv_begin_PreprocessGeneratedCommandsEXT,
    __print_json_radv_begin_PreprocessGeneratedCommandsEXT,
    __fuzzy_hash_radv_begin_PreprocessGeneratedCommandsEXT,
    __fuzzy_equals_radv_begin_PreprocessGeneratedCommandsEXT,
    __print_fuzzy_hash_args_radv_begin_PreprocessGeneratedCommandsEXT,
};
void __trace_radv_begin_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_PreprocessGeneratedCommandsEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_PreprocessGeneratedCommandsEXT *__entry =
      queueing ?
      (struct trace_radv_begin_PreprocessGeneratedCommandsEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_PreprocessGeneratedCommandsEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_PreprocessGeneratedCommandsEXT
 */
  
#define __print_radv_end_PreprocessGeneratedCommandsEXT NULL
  
#define __print_json_radv_end_PreprocessGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_end_PreprocessGeneratedCommandsEXT NULL

#define __fuzzy_hash_radv_end_PreprocessGeneratedCommandsEXT NULL
#define __fuzzy_equals_radv_end_PreprocessGeneratedCommandsEXT NULL
#define __print_fuzzy_hash_args_radv_end_PreprocessGeneratedCommandsEXT NULL
static const struct u_tracepoint __tp_radv_end_PreprocessGeneratedCommandsEXT = {
    "radv_end_PreprocessGeneratedCommandsEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    77,
    u_tracepoint_type_end_range,
    __print_radv_end_PreprocessGeneratedCommandsEXT,
    __print_json_radv_end_PreprocessGeneratedCommandsEXT,
    __fuzzy_hash_radv_end_PreprocessGeneratedCommandsEXT,
    __fuzzy_equals_radv_end_PreprocessGeneratedCommandsEXT,
    __print_fuzzy_hash_args_radv_end_PreprocessGeneratedCommandsEXT,
};
void __trace_radv_end_PreprocessGeneratedCommandsEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_PreprocessGeneratedCommandsEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_PreprocessGeneratedCommandsEXT *__entry =
      queueing ?
      (struct trace_radv_end_PreprocessGeneratedCommandsEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_PreprocessGeneratedCommandsEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DispatchBase
 */
  
static void __print_radv_begin_DispatchBase(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DispatchBase *__entry =
      (const struct trace_radv_begin_DispatchBase *)arg;
   fprintf(out, ""
      "base_x=%u, "
      "base_y=%u, "
      "base_z=%u, "
      "x=%u, "
      "y=%u, "
      "z=%u, "
         "\n"
   ,__entry->base_x
   ,__entry->base_y
   ,__entry->base_z
   ,__entry->x
   ,__entry->y
   ,__entry->z
   );
}

  
static void __print_json_radv_begin_DispatchBase(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DispatchBase *__entry =
      (const struct trace_radv_begin_DispatchBase *)arg;
   fprintf(out, ""
      "\"base_x\": \"%u\""
         ", "
      "\"base_y\": \"%u\""
         ", "
      "\"base_z\": \"%u\""
         ", "
      "\"x\": \"%u\""
         ", "
      "\"y\": \"%u\""
         ", "
      "\"z\": \"%u\""
   ,__entry->base_x
   ,__entry->base_y
   ,__entry->base_z
   ,__entry->x
   ,__entry->y
   ,__entry->z
   );
}

#define __print_fuzzy_hash_args_radv_begin_DispatchBase NULL

static uint32_t __fuzzy_hash_radv_begin_DispatchBase(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DispatchBase *__entry =
      (const struct trace_radv_begin_DispatchBase *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DispatchBase(const void *_a, const void *_b) {
   const struct trace_radv_begin_DispatchBase *__entry_a =
      (const struct trace_radv_begin_DispatchBase *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DispatchBase *__entry_b =
      (const struct trace_radv_begin_DispatchBase *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DispatchBase = {
    "radv_begin_DispatchBase",
    ALIGN_POT(sizeof(struct trace_radv_begin_DispatchBase), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    78,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DispatchBase,
    __print_json_radv_begin_DispatchBase,
    __fuzzy_hash_radv_begin_DispatchBase,
    __fuzzy_equals_radv_begin_DispatchBase,
    __print_fuzzy_hash_args_radv_begin_DispatchBase,
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
) {
   struct trace_radv_begin_DispatchBase entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DispatchBase *__entry =
      queueing ?
      (struct trace_radv_begin_DispatchBase *)u_trace_appendv(ut, cs, &__tp_radv_begin_DispatchBase,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->base_x = base_x;
   __entry->base_y = base_y;
   __entry->base_z = base_z;
   __entry->x = x;
   __entry->y = y;
   __entry->z = z;
}

/*
 * radv_end_DispatchBase
 */
  
#define __print_radv_end_DispatchBase NULL
  
#define __print_json_radv_end_DispatchBase NULL
#define __print_fuzzy_hash_args_radv_end_DispatchBase NULL

#define __fuzzy_hash_radv_end_DispatchBase NULL
#define __fuzzy_equals_radv_end_DispatchBase NULL
#define __print_fuzzy_hash_args_radv_end_DispatchBase NULL
static const struct u_tracepoint __tp_radv_end_DispatchBase = {
    "radv_end_DispatchBase",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    79,
    u_tracepoint_type_end_range,
    __print_radv_end_DispatchBase,
    __print_json_radv_end_DispatchBase,
    __fuzzy_hash_radv_end_DispatchBase,
    __fuzzy_equals_radv_end_DispatchBase,
    __print_fuzzy_hash_args_radv_end_DispatchBase,
};
void __trace_radv_end_DispatchBase(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DispatchBase entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DispatchBase *__entry =
      queueing ?
      (struct trace_radv_end_DispatchBase *)u_trace_appendv(ut, cs, &__tp_radv_end_DispatchBase,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirectCount
 */
  
static void __print_radv_begin_DrawIndirectCount(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndirectCount *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndirectCount(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndirectCount *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirectCount NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirectCount(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndirectCount *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirectCount(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirectCount *__entry_a =
      (const struct trace_radv_begin_DrawIndirectCount *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirectCount *__entry_b =
      (const struct trace_radv_begin_DrawIndirectCount *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirectCount = {
    "radv_begin_DrawIndirectCount",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirectCount), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    80,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirectCount,
    __print_json_radv_begin_DrawIndirectCount,
    __fuzzy_hash_radv_begin_DrawIndirectCount,
    __fuzzy_equals_radv_begin_DrawIndirectCount,
    __print_fuzzy_hash_args_radv_begin_DrawIndirectCount,
};
void __trace_radv_begin_DrawIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawIndirectCount entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirectCount *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirectCount *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirectCount,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawIndirectCount
 */
  
#define __print_radv_end_DrawIndirectCount NULL
  
#define __print_json_radv_end_DrawIndirectCount NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectCount NULL

#define __fuzzy_hash_radv_end_DrawIndirectCount NULL
#define __fuzzy_equals_radv_end_DrawIndirectCount NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectCount NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirectCount = {
    "radv_end_DrawIndirectCount",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    81,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirectCount,
    __print_json_radv_end_DrawIndirectCount,
    __fuzzy_hash_radv_end_DrawIndirectCount,
    __fuzzy_equals_radv_end_DrawIndirectCount,
    __print_fuzzy_hash_args_radv_end_DrawIndirectCount,
};
void __trace_radv_end_DrawIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirectCount entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirectCount *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirectCount *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirectCount,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndexedIndirectCount
 */
  
static void __print_radv_begin_DrawIndexedIndirectCount(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndexedIndirectCount(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirectCount NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndexedIndirectCount(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndexedIndirectCount *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndexedIndirectCount(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndexedIndirectCount *__entry_a =
      (const struct trace_radv_begin_DrawIndexedIndirectCount *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndexedIndirectCount *__entry_b =
      (const struct trace_radv_begin_DrawIndexedIndirectCount *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndexedIndirectCount = {
    "radv_begin_DrawIndexedIndirectCount",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndexedIndirectCount), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    82,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndexedIndirectCount,
    __print_json_radv_begin_DrawIndexedIndirectCount,
    __fuzzy_hash_radv_begin_DrawIndexedIndirectCount,
    __fuzzy_equals_radv_begin_DrawIndexedIndirectCount,
    __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirectCount,
};
void __trace_radv_begin_DrawIndexedIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawIndexedIndirectCount entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndexedIndirectCount *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndexedIndirectCount *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndexedIndirectCount,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawIndexedIndirectCount
 */
  
#define __print_radv_end_DrawIndexedIndirectCount NULL
  
#define __print_json_radv_end_DrawIndexedIndirectCount NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount NULL

#define __fuzzy_hash_radv_end_DrawIndexedIndirectCount NULL
#define __fuzzy_equals_radv_end_DrawIndexedIndirectCount NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount NULL
static const struct u_tracepoint __tp_radv_end_DrawIndexedIndirectCount = {
    "radv_end_DrawIndexedIndirectCount",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    83,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndexedIndirectCount,
    __print_json_radv_end_DrawIndexedIndirectCount,
    __fuzzy_hash_radv_end_DrawIndexedIndirectCount,
    __fuzzy_equals_radv_end_DrawIndexedIndirectCount,
    __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount,
};
void __trace_radv_end_DrawIndexedIndirectCount(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndexedIndirectCount entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndexedIndirectCount *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndexedIndirectCount *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndexedIndirectCount,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirectByteCountEXT
 */
  
static void __print_radv_begin_DrawIndirectByteCountEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectByteCountEXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCountEXT *)arg;
   fprintf(out, ""
      "instance_count=%u, "
         "\n"
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_DrawIndirectByteCountEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectByteCountEXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCountEXT *)arg;
   fprintf(out, ""
      "\"instance_count\": \"%u\""
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirectByteCountEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirectByteCountEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirectByteCountEXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCountEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirectByteCountEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirectByteCountEXT *__entry_a =
      (const struct trace_radv_begin_DrawIndirectByteCountEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirectByteCountEXT *__entry_b =
      (const struct trace_radv_begin_DrawIndirectByteCountEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirectByteCountEXT = {
    "radv_begin_DrawIndirectByteCountEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirectByteCountEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    84,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirectByteCountEXT,
    __print_json_radv_begin_DrawIndirectByteCountEXT,
    __fuzzy_hash_radv_begin_DrawIndirectByteCountEXT,
    __fuzzy_equals_radv_begin_DrawIndirectByteCountEXT,
    __print_fuzzy_hash_args_radv_begin_DrawIndirectByteCountEXT,
};
void __trace_radv_begin_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t instance_count
) {
   struct trace_radv_begin_DrawIndirectByteCountEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirectByteCountEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirectByteCountEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirectByteCountEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_DrawIndirectByteCountEXT
 */
  
#define __print_radv_end_DrawIndirectByteCountEXT NULL
  
#define __print_json_radv_end_DrawIndirectByteCountEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectByteCountEXT NULL

#define __fuzzy_hash_radv_end_DrawIndirectByteCountEXT NULL
#define __fuzzy_equals_radv_end_DrawIndirectByteCountEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectByteCountEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirectByteCountEXT = {
    "radv_end_DrawIndirectByteCountEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    85,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirectByteCountEXT,
    __print_json_radv_end_DrawIndirectByteCountEXT,
    __fuzzy_hash_radv_end_DrawIndirectByteCountEXT,
    __fuzzy_equals_radv_end_DrawIndirectByteCountEXT,
    __print_fuzzy_hash_args_radv_end_DrawIndirectByteCountEXT,
};
void __trace_radv_end_DrawIndirectByteCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirectByteCountEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirectByteCountEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirectByteCountEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirectByteCountEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMeshTasksEXT
 */
  
static void __print_radv_begin_DrawMeshTasksEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksEXT *)arg;
   fprintf(out, ""
      "x=%u, "
      "y=%u, "
      "z=%u, "
         "\n"
   ,__entry->x
   ,__entry->y
   ,__entry->z
   );
}

  
static void __print_json_radv_begin_DrawMeshTasksEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksEXT *)arg;
   fprintf(out, ""
      "\"x\": \"%u\""
         ", "
      "\"y\": \"%u\""
         ", "
      "\"z\": \"%u\""
   ,__entry->x
   ,__entry->y
   ,__entry->z
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMeshTasksEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMeshTasksEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMeshTasksEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMeshTasksEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMeshTasksEXT *__entry_a =
      (const struct trace_radv_begin_DrawMeshTasksEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMeshTasksEXT *__entry_b =
      (const struct trace_radv_begin_DrawMeshTasksEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMeshTasksEXT = {
    "radv_begin_DrawMeshTasksEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMeshTasksEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    86,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMeshTasksEXT,
    __print_json_radv_begin_DrawMeshTasksEXT,
    __fuzzy_hash_radv_begin_DrawMeshTasksEXT,
    __fuzzy_equals_radv_begin_DrawMeshTasksEXT,
    __print_fuzzy_hash_args_radv_begin_DrawMeshTasksEXT,
};
void __trace_radv_begin_DrawMeshTasksEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t x
   , uint32_t y
   , uint32_t z
) {
   struct trace_radv_begin_DrawMeshTasksEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMeshTasksEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMeshTasksEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMeshTasksEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->x = x;
   __entry->y = y;
   __entry->z = z;
}

/*
 * radv_end_DrawMeshTasksEXT
 */
  
#define __print_radv_end_DrawMeshTasksEXT NULL
  
#define __print_json_radv_end_DrawMeshTasksEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksEXT NULL

#define __fuzzy_hash_radv_end_DrawMeshTasksEXT NULL
#define __fuzzy_equals_radv_end_DrawMeshTasksEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMeshTasksEXT = {
    "radv_end_DrawMeshTasksEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    87,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMeshTasksEXT,
    __print_json_radv_end_DrawMeshTasksEXT,
    __fuzzy_hash_radv_end_DrawMeshTasksEXT,
    __fuzzy_equals_radv_end_DrawMeshTasksEXT,
    __print_fuzzy_hash_args_radv_end_DrawMeshTasksEXT,
};
void __trace_radv_end_DrawMeshTasksEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMeshTasksEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMeshTasksEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMeshTasksEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMeshTasksEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMeshTasksIndirectEXT
 */
  
static void __print_radv_begin_DrawMeshTasksIndirectEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectEXT *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawMeshTasksIndirectEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectEXT *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMeshTasksIndirectEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMeshTasksIndirectEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry_a =
      (const struct trace_radv_begin_DrawMeshTasksIndirectEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry_b =
      (const struct trace_radv_begin_DrawMeshTasksIndirectEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMeshTasksIndirectEXT = {
    "radv_begin_DrawMeshTasksIndirectEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMeshTasksIndirectEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    88,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMeshTasksIndirectEXT,
    __print_json_radv_begin_DrawMeshTasksIndirectEXT,
    __fuzzy_hash_radv_begin_DrawMeshTasksIndirectEXT,
    __fuzzy_equals_radv_begin_DrawMeshTasksIndirectEXT,
    __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectEXT,
};
void __trace_radv_begin_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawMeshTasksIndirectEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMeshTasksIndirectEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMeshTasksIndirectEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMeshTasksIndirectEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawMeshTasksIndirectEXT
 */
  
#define __print_radv_end_DrawMeshTasksIndirectEXT NULL
  
#define __print_json_radv_end_DrawMeshTasksIndirectEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectEXT NULL

#define __fuzzy_hash_radv_end_DrawMeshTasksIndirectEXT NULL
#define __fuzzy_equals_radv_end_DrawMeshTasksIndirectEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMeshTasksIndirectEXT = {
    "radv_end_DrawMeshTasksIndirectEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    89,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMeshTasksIndirectEXT,
    __print_json_radv_end_DrawMeshTasksIndirectEXT,
    __fuzzy_hash_radv_end_DrawMeshTasksIndirectEXT,
    __fuzzy_equals_radv_end_DrawMeshTasksIndirectEXT,
    __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectEXT,
};
void __trace_radv_end_DrawMeshTasksIndirectEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMeshTasksIndirectEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMeshTasksIndirectEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMeshTasksIndirectEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMeshTasksIndirectEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMeshTasksIndirectCountEXT
 */
  
static void __print_radv_begin_DrawMeshTasksIndirectCountEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawMeshTasksIndirectCountEXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectCountEXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMeshTasksIndirectCountEXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMeshTasksIndirectCountEXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry_a =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry_b =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMeshTasksIndirectCountEXT = {
    "radv_begin_DrawMeshTasksIndirectCountEXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMeshTasksIndirectCountEXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    90,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMeshTasksIndirectCountEXT,
    __print_json_radv_begin_DrawMeshTasksIndirectCountEXT,
    __fuzzy_hash_radv_begin_DrawMeshTasksIndirectCountEXT,
    __fuzzy_equals_radv_begin_DrawMeshTasksIndirectCountEXT,
    __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectCountEXT,
};
void __trace_radv_begin_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawMeshTasksIndirectCountEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMeshTasksIndirectCountEXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMeshTasksIndirectCountEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawMeshTasksIndirectCountEXT
 */
  
#define __print_radv_end_DrawMeshTasksIndirectCountEXT NULL
  
#define __print_json_radv_end_DrawMeshTasksIndirectCountEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCountEXT NULL

#define __fuzzy_hash_radv_end_DrawMeshTasksIndirectCountEXT NULL
#define __fuzzy_equals_radv_end_DrawMeshTasksIndirectCountEXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCountEXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMeshTasksIndirectCountEXT = {
    "radv_end_DrawMeshTasksIndirectCountEXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    91,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMeshTasksIndirectCountEXT,
    __print_json_radv_end_DrawMeshTasksIndirectCountEXT,
    __fuzzy_hash_radv_end_DrawMeshTasksIndirectCountEXT,
    __fuzzy_equals_radv_end_DrawMeshTasksIndirectCountEXT,
    __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCountEXT,
};
void __trace_radv_end_DrawMeshTasksIndirectCountEXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMeshTasksIndirectCountEXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMeshTasksIndirectCountEXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMeshTasksIndirectCountEXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMeshTasksIndirectCountEXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyAccelerationStructureKHR
 */
  
#define __print_radv_begin_CopyAccelerationStructureKHR NULL
  
#define __print_json_radv_begin_CopyAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureKHR NULL

#define __fuzzy_hash_radv_begin_CopyAccelerationStructureKHR NULL
#define __fuzzy_equals_radv_begin_CopyAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyAccelerationStructureKHR = {
    "radv_begin_CopyAccelerationStructureKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    92,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyAccelerationStructureKHR,
    __print_json_radv_begin_CopyAccelerationStructureKHR,
    __fuzzy_hash_radv_begin_CopyAccelerationStructureKHR,
    __fuzzy_equals_radv_begin_CopyAccelerationStructureKHR,
    __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureKHR,
};
void __trace_radv_begin_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyAccelerationStructureKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyAccelerationStructureKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyAccelerationStructureKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyAccelerationStructureKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyAccelerationStructureKHR
 */
  
#define __print_radv_end_CopyAccelerationStructureKHR NULL
  
#define __print_json_radv_end_CopyAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureKHR NULL

#define __fuzzy_hash_radv_end_CopyAccelerationStructureKHR NULL
#define __fuzzy_equals_radv_end_CopyAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyAccelerationStructureKHR = {
    "radv_end_CopyAccelerationStructureKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    93,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyAccelerationStructureKHR,
    __print_json_radv_end_CopyAccelerationStructureKHR,
    __fuzzy_hash_radv_end_CopyAccelerationStructureKHR,
    __fuzzy_equals_radv_end_CopyAccelerationStructureKHR,
    __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureKHR,
};
void __trace_radv_end_CopyAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyAccelerationStructureKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyAccelerationStructureKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyAccelerationStructureKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyAccelerationStructureKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyAccelerationStructureToMemoryKHR
 */
  
#define __print_radv_begin_CopyAccelerationStructureToMemoryKHR NULL
  
#define __print_json_radv_begin_CopyAccelerationStructureToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureToMemoryKHR NULL

#define __fuzzy_hash_radv_begin_CopyAccelerationStructureToMemoryKHR NULL
#define __fuzzy_equals_radv_begin_CopyAccelerationStructureToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyAccelerationStructureToMemoryKHR = {
    "radv_begin_CopyAccelerationStructureToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    94,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyAccelerationStructureToMemoryKHR,
    __print_json_radv_begin_CopyAccelerationStructureToMemoryKHR,
    __fuzzy_hash_radv_begin_CopyAccelerationStructureToMemoryKHR,
    __fuzzy_equals_radv_begin_CopyAccelerationStructureToMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_CopyAccelerationStructureToMemoryKHR,
};
void __trace_radv_begin_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyAccelerationStructureToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyAccelerationStructureToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyAccelerationStructureToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyAccelerationStructureToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyAccelerationStructureToMemoryKHR
 */
  
#define __print_radv_end_CopyAccelerationStructureToMemoryKHR NULL
  
#define __print_json_radv_end_CopyAccelerationStructureToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureToMemoryKHR NULL

#define __fuzzy_hash_radv_end_CopyAccelerationStructureToMemoryKHR NULL
#define __fuzzy_equals_radv_end_CopyAccelerationStructureToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyAccelerationStructureToMemoryKHR = {
    "radv_end_CopyAccelerationStructureToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    95,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyAccelerationStructureToMemoryKHR,
    __print_json_radv_end_CopyAccelerationStructureToMemoryKHR,
    __fuzzy_hash_radv_end_CopyAccelerationStructureToMemoryKHR,
    __fuzzy_equals_radv_end_CopyAccelerationStructureToMemoryKHR,
    __print_fuzzy_hash_args_radv_end_CopyAccelerationStructureToMemoryKHR,
};
void __trace_radv_end_CopyAccelerationStructureToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyAccelerationStructureToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyAccelerationStructureToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyAccelerationStructureToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyAccelerationStructureToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyMemoryToAccelerationStructureKHR
 */
  
#define __print_radv_begin_CopyMemoryToAccelerationStructureKHR NULL
  
#define __print_json_radv_begin_CopyMemoryToAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToAccelerationStructureKHR NULL

#define __fuzzy_hash_radv_begin_CopyMemoryToAccelerationStructureKHR NULL
#define __fuzzy_equals_radv_begin_CopyMemoryToAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToAccelerationStructureKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyMemoryToAccelerationStructureKHR = {
    "radv_begin_CopyMemoryToAccelerationStructureKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    96,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyMemoryToAccelerationStructureKHR,
    __print_json_radv_begin_CopyMemoryToAccelerationStructureKHR,
    __fuzzy_hash_radv_begin_CopyMemoryToAccelerationStructureKHR,
    __fuzzy_equals_radv_begin_CopyMemoryToAccelerationStructureKHR,
    __print_fuzzy_hash_args_radv_begin_CopyMemoryToAccelerationStructureKHR,
};
void __trace_radv_begin_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyMemoryToAccelerationStructureKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyMemoryToAccelerationStructureKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyMemoryToAccelerationStructureKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyMemoryToAccelerationStructureKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyMemoryToAccelerationStructureKHR
 */
  
#define __print_radv_end_CopyMemoryToAccelerationStructureKHR NULL
  
#define __print_json_radv_end_CopyMemoryToAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToAccelerationStructureKHR NULL

#define __fuzzy_hash_radv_end_CopyMemoryToAccelerationStructureKHR NULL
#define __fuzzy_equals_radv_end_CopyMemoryToAccelerationStructureKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToAccelerationStructureKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyMemoryToAccelerationStructureKHR = {
    "radv_end_CopyMemoryToAccelerationStructureKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    97,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyMemoryToAccelerationStructureKHR,
    __print_json_radv_end_CopyMemoryToAccelerationStructureKHR,
    __fuzzy_hash_radv_end_CopyMemoryToAccelerationStructureKHR,
    __fuzzy_equals_radv_end_CopyMemoryToAccelerationStructureKHR,
    __print_fuzzy_hash_args_radv_end_CopyMemoryToAccelerationStructureKHR,
};
void __trace_radv_end_CopyMemoryToAccelerationStructureKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyMemoryToAccelerationStructureKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyMemoryToAccelerationStructureKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyMemoryToAccelerationStructureKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyMemoryToAccelerationStructureKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_TraceRaysKHR
 */
  
static void __print_radv_begin_TraceRaysKHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_TraceRaysKHR *__entry =
      (const struct trace_radv_begin_TraceRaysKHR *)arg;
   fprintf(out, ""
      "width=%u, "
      "height=%u, "
      "depth=%u, "
         "\n"
   ,__entry->width
   ,__entry->height
   ,__entry->depth
   );
}

  
static void __print_json_radv_begin_TraceRaysKHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_TraceRaysKHR *__entry =
      (const struct trace_radv_begin_TraceRaysKHR *)arg;
   fprintf(out, ""
      "\"width\": \"%u\""
         ", "
      "\"height\": \"%u\""
         ", "
      "\"depth\": \"%u\""
   ,__entry->width
   ,__entry->height
   ,__entry->depth
   );
}

#define __print_fuzzy_hash_args_radv_begin_TraceRaysKHR NULL

static uint32_t __fuzzy_hash_radv_begin_TraceRaysKHR(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_TraceRaysKHR *__entry =
      (const struct trace_radv_begin_TraceRaysKHR *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_TraceRaysKHR(const void *_a, const void *_b) {
   const struct trace_radv_begin_TraceRaysKHR *__entry_a =
      (const struct trace_radv_begin_TraceRaysKHR *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_TraceRaysKHR *__entry_b =
      (const struct trace_radv_begin_TraceRaysKHR *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_TraceRaysKHR = {
    "radv_begin_TraceRaysKHR",
    ALIGN_POT(sizeof(struct trace_radv_begin_TraceRaysKHR), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    98,
    u_tracepoint_type_begin_range,
    __print_radv_begin_TraceRaysKHR,
    __print_json_radv_begin_TraceRaysKHR,
    __fuzzy_hash_radv_begin_TraceRaysKHR,
    __fuzzy_equals_radv_begin_TraceRaysKHR,
    __print_fuzzy_hash_args_radv_begin_TraceRaysKHR,
};
void __trace_radv_begin_TraceRaysKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t width
   , uint32_t height
   , uint32_t depth
) {
   struct trace_radv_begin_TraceRaysKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_TraceRaysKHR *__entry =
      queueing ?
      (struct trace_radv_begin_TraceRaysKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_TraceRaysKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->width = width;
   __entry->height = height;
   __entry->depth = depth;
}

/*
 * radv_end_TraceRaysKHR
 */
  
#define __print_radv_end_TraceRaysKHR NULL
  
#define __print_json_radv_end_TraceRaysKHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysKHR NULL

#define __fuzzy_hash_radv_end_TraceRaysKHR NULL
#define __fuzzy_equals_radv_end_TraceRaysKHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysKHR NULL
static const struct u_tracepoint __tp_radv_end_TraceRaysKHR = {
    "radv_end_TraceRaysKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    99,
    u_tracepoint_type_end_range,
    __print_radv_end_TraceRaysKHR,
    __print_json_radv_end_TraceRaysKHR,
    __fuzzy_hash_radv_end_TraceRaysKHR,
    __fuzzy_equals_radv_end_TraceRaysKHR,
    __print_fuzzy_hash_args_radv_end_TraceRaysKHR,
};
void __trace_radv_end_TraceRaysKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_TraceRaysKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_TraceRaysKHR *__entry =
      queueing ?
      (struct trace_radv_end_TraceRaysKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_TraceRaysKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_TraceRaysIndirectKHR
 */
  
#define __print_radv_begin_TraceRaysIndirectKHR NULL
  
#define __print_json_radv_begin_TraceRaysIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_TraceRaysIndirectKHR NULL

#define __fuzzy_hash_radv_begin_TraceRaysIndirectKHR NULL
#define __fuzzy_equals_radv_begin_TraceRaysIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_begin_TraceRaysIndirectKHR NULL
static const struct u_tracepoint __tp_radv_begin_TraceRaysIndirectKHR = {
    "radv_begin_TraceRaysIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    100,
    u_tracepoint_type_begin_range,
    __print_radv_begin_TraceRaysIndirectKHR,
    __print_json_radv_begin_TraceRaysIndirectKHR,
    __fuzzy_hash_radv_begin_TraceRaysIndirectKHR,
    __fuzzy_equals_radv_begin_TraceRaysIndirectKHR,
    __print_fuzzy_hash_args_radv_begin_TraceRaysIndirectKHR,
};
void __trace_radv_begin_TraceRaysIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_TraceRaysIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_TraceRaysIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_begin_TraceRaysIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_TraceRaysIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_TraceRaysIndirectKHR
 */
  
#define __print_radv_end_TraceRaysIndirectKHR NULL
  
#define __print_json_radv_end_TraceRaysIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysIndirectKHR NULL

#define __fuzzy_hash_radv_end_TraceRaysIndirectKHR NULL
#define __fuzzy_equals_radv_end_TraceRaysIndirectKHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysIndirectKHR NULL
static const struct u_tracepoint __tp_radv_end_TraceRaysIndirectKHR = {
    "radv_end_TraceRaysIndirectKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    101,
    u_tracepoint_type_end_range,
    __print_radv_end_TraceRaysIndirectKHR,
    __print_json_radv_end_TraceRaysIndirectKHR,
    __fuzzy_hash_radv_end_TraceRaysIndirectKHR,
    __fuzzy_equals_radv_end_TraceRaysIndirectKHR,
    __print_fuzzy_hash_args_radv_end_TraceRaysIndirectKHR,
};
void __trace_radv_end_TraceRaysIndirectKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_TraceRaysIndirectKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_TraceRaysIndirectKHR *__entry =
      queueing ?
      (struct trace_radv_end_TraceRaysIndirectKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_TraceRaysIndirectKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_TraceRaysIndirect2KHR
 */
  
#define __print_radv_begin_TraceRaysIndirect2KHR NULL
  
#define __print_json_radv_begin_TraceRaysIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_begin_TraceRaysIndirect2KHR NULL

#define __fuzzy_hash_radv_begin_TraceRaysIndirect2KHR NULL
#define __fuzzy_equals_radv_begin_TraceRaysIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_begin_TraceRaysIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_begin_TraceRaysIndirect2KHR = {
    "radv_begin_TraceRaysIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    102,
    u_tracepoint_type_begin_range,
    __print_radv_begin_TraceRaysIndirect2KHR,
    __print_json_radv_begin_TraceRaysIndirect2KHR,
    __fuzzy_hash_radv_begin_TraceRaysIndirect2KHR,
    __fuzzy_equals_radv_begin_TraceRaysIndirect2KHR,
    __print_fuzzy_hash_args_radv_begin_TraceRaysIndirect2KHR,
};
void __trace_radv_begin_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_TraceRaysIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_TraceRaysIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_TraceRaysIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_TraceRaysIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_TraceRaysIndirect2KHR
 */
  
#define __print_radv_end_TraceRaysIndirect2KHR NULL
  
#define __print_json_radv_end_TraceRaysIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysIndirect2KHR NULL

#define __fuzzy_hash_radv_end_TraceRaysIndirect2KHR NULL
#define __fuzzy_equals_radv_end_TraceRaysIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_TraceRaysIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_end_TraceRaysIndirect2KHR = {
    "radv_end_TraceRaysIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    103,
    u_tracepoint_type_end_range,
    __print_radv_end_TraceRaysIndirect2KHR,
    __print_json_radv_end_TraceRaysIndirect2KHR,
    __fuzzy_hash_radv_end_TraceRaysIndirect2KHR,
    __fuzzy_equals_radv_end_TraceRaysIndirect2KHR,
    __print_fuzzy_hash_args_radv_end_TraceRaysIndirect2KHR,
};
void __trace_radv_end_TraceRaysIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_TraceRaysIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_TraceRaysIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_end_TraceRaysIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_TraceRaysIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_BuildAccelerationStructuresKHR
 */
  
#define __print_radv_begin_BuildAccelerationStructuresKHR NULL
  
#define __print_json_radv_begin_BuildAccelerationStructuresKHR NULL
#define __print_fuzzy_hash_args_radv_begin_BuildAccelerationStructuresKHR NULL

#define __fuzzy_hash_radv_begin_BuildAccelerationStructuresKHR NULL
#define __fuzzy_equals_radv_begin_BuildAccelerationStructuresKHR NULL
#define __print_fuzzy_hash_args_radv_begin_BuildAccelerationStructuresKHR NULL
static const struct u_tracepoint __tp_radv_begin_BuildAccelerationStructuresKHR = {
    "radv_begin_BuildAccelerationStructuresKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    104,
    u_tracepoint_type_begin_range,
    __print_radv_begin_BuildAccelerationStructuresKHR,
    __print_json_radv_begin_BuildAccelerationStructuresKHR,
    __fuzzy_hash_radv_begin_BuildAccelerationStructuresKHR,
    __fuzzy_equals_radv_begin_BuildAccelerationStructuresKHR,
    __print_fuzzy_hash_args_radv_begin_BuildAccelerationStructuresKHR,
};
void __trace_radv_begin_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_BuildAccelerationStructuresKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_BuildAccelerationStructuresKHR *__entry =
      queueing ?
      (struct trace_radv_begin_BuildAccelerationStructuresKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_BuildAccelerationStructuresKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_BuildAccelerationStructuresKHR
 */
  
#define __print_radv_end_BuildAccelerationStructuresKHR NULL
  
#define __print_json_radv_end_BuildAccelerationStructuresKHR NULL
#define __print_fuzzy_hash_args_radv_end_BuildAccelerationStructuresKHR NULL

#define __fuzzy_hash_radv_end_BuildAccelerationStructuresKHR NULL
#define __fuzzy_equals_radv_end_BuildAccelerationStructuresKHR NULL
#define __print_fuzzy_hash_args_radv_end_BuildAccelerationStructuresKHR NULL
static const struct u_tracepoint __tp_radv_end_BuildAccelerationStructuresKHR = {
    "radv_end_BuildAccelerationStructuresKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    105,
    u_tracepoint_type_end_range,
    __print_radv_end_BuildAccelerationStructuresKHR,
    __print_json_radv_end_BuildAccelerationStructuresKHR,
    __fuzzy_hash_radv_end_BuildAccelerationStructuresKHR,
    __fuzzy_equals_radv_end_BuildAccelerationStructuresKHR,
    __print_fuzzy_hash_args_radv_end_BuildAccelerationStructuresKHR,
};
void __trace_radv_end_BuildAccelerationStructuresKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_BuildAccelerationStructuresKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_BuildAccelerationStructuresKHR *__entry =
      queueing ?
      (struct trace_radv_end_BuildAccelerationStructuresKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_BuildAccelerationStructuresKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyBuffer2
 */
  
#define __print_radv_begin_CopyBuffer2 NULL
  
#define __print_json_radv_begin_CopyBuffer2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyBuffer2 NULL

#define __fuzzy_hash_radv_begin_CopyBuffer2 NULL
#define __fuzzy_equals_radv_begin_CopyBuffer2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyBuffer2 NULL
static const struct u_tracepoint __tp_radv_begin_CopyBuffer2 = {
    "radv_begin_CopyBuffer2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    106,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyBuffer2,
    __print_json_radv_begin_CopyBuffer2,
    __fuzzy_hash_radv_begin_CopyBuffer2,
    __fuzzy_equals_radv_begin_CopyBuffer2,
    __print_fuzzy_hash_args_radv_begin_CopyBuffer2,
};
void __trace_radv_begin_CopyBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyBuffer2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyBuffer2 *__entry =
      queueing ?
      (struct trace_radv_begin_CopyBuffer2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyBuffer2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyBuffer2
 */
  
#define __print_radv_end_CopyBuffer2 NULL
  
#define __print_json_radv_end_CopyBuffer2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyBuffer2 NULL

#define __fuzzy_hash_radv_end_CopyBuffer2 NULL
#define __fuzzy_equals_radv_end_CopyBuffer2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyBuffer2 NULL
static const struct u_tracepoint __tp_radv_end_CopyBuffer2 = {
    "radv_end_CopyBuffer2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    107,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyBuffer2,
    __print_json_radv_end_CopyBuffer2,
    __fuzzy_hash_radv_end_CopyBuffer2,
    __fuzzy_equals_radv_end_CopyBuffer2,
    __print_fuzzy_hash_args_radv_end_CopyBuffer2,
};
void __trace_radv_end_CopyBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyBuffer2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyBuffer2 *__entry =
      queueing ?
      (struct trace_radv_end_CopyBuffer2 *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyBuffer2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyImage2
 */
  
#define __print_radv_begin_CopyImage2 NULL
  
#define __print_json_radv_begin_CopyImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImage2 NULL

#define __fuzzy_hash_radv_begin_CopyImage2 NULL
#define __fuzzy_equals_radv_begin_CopyImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImage2 NULL
static const struct u_tracepoint __tp_radv_begin_CopyImage2 = {
    "radv_begin_CopyImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    108,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyImage2,
    __print_json_radv_begin_CopyImage2,
    __fuzzy_hash_radv_begin_CopyImage2,
    __fuzzy_equals_radv_begin_CopyImage2,
    __print_fuzzy_hash_args_radv_begin_CopyImage2,
};
void __trace_radv_begin_CopyImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyImage2 *__entry =
      queueing ?
      (struct trace_radv_begin_CopyImage2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyImage2
 */
  
#define __print_radv_end_CopyImage2 NULL
  
#define __print_json_radv_end_CopyImage2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyImage2 NULL

#define __fuzzy_hash_radv_end_CopyImage2 NULL
#define __fuzzy_equals_radv_end_CopyImage2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyImage2 NULL
static const struct u_tracepoint __tp_radv_end_CopyImage2 = {
    "radv_end_CopyImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    109,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyImage2,
    __print_json_radv_end_CopyImage2,
    __fuzzy_hash_radv_end_CopyImage2,
    __fuzzy_equals_radv_end_CopyImage2,
    __print_fuzzy_hash_args_radv_end_CopyImage2,
};
void __trace_radv_end_CopyImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyImage2 *__entry =
      queueing ?
      (struct trace_radv_end_CopyImage2 *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_BlitImage2
 */
  
#define __print_radv_begin_BlitImage2 NULL
  
#define __print_json_radv_begin_BlitImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_BlitImage2 NULL

#define __fuzzy_hash_radv_begin_BlitImage2 NULL
#define __fuzzy_equals_radv_begin_BlitImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_BlitImage2 NULL
static const struct u_tracepoint __tp_radv_begin_BlitImage2 = {
    "radv_begin_BlitImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    110,
    u_tracepoint_type_begin_range,
    __print_radv_begin_BlitImage2,
    __print_json_radv_begin_BlitImage2,
    __fuzzy_hash_radv_begin_BlitImage2,
    __fuzzy_equals_radv_begin_BlitImage2,
    __print_fuzzy_hash_args_radv_begin_BlitImage2,
};
void __trace_radv_begin_BlitImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_BlitImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_BlitImage2 *__entry =
      queueing ?
      (struct trace_radv_begin_BlitImage2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_BlitImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_BlitImage2
 */
  
#define __print_radv_end_BlitImage2 NULL
  
#define __print_json_radv_end_BlitImage2 NULL
#define __print_fuzzy_hash_args_radv_end_BlitImage2 NULL

#define __fuzzy_hash_radv_end_BlitImage2 NULL
#define __fuzzy_equals_radv_end_BlitImage2 NULL
#define __print_fuzzy_hash_args_radv_end_BlitImage2 NULL
static const struct u_tracepoint __tp_radv_end_BlitImage2 = {
    "radv_end_BlitImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    111,
    u_tracepoint_type_end_range,
    __print_radv_end_BlitImage2,
    __print_json_radv_end_BlitImage2,
    __fuzzy_hash_radv_end_BlitImage2,
    __fuzzy_equals_radv_end_BlitImage2,
    __print_fuzzy_hash_args_radv_end_BlitImage2,
};
void __trace_radv_end_BlitImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_BlitImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_BlitImage2 *__entry =
      queueing ?
      (struct trace_radv_end_BlitImage2 *)u_trace_appendv(ut, cs, &__tp_radv_end_BlitImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyBufferToImage2
 */
  
#define __print_radv_begin_CopyBufferToImage2 NULL
  
#define __print_json_radv_begin_CopyBufferToImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyBufferToImage2 NULL

#define __fuzzy_hash_radv_begin_CopyBufferToImage2 NULL
#define __fuzzy_equals_radv_begin_CopyBufferToImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyBufferToImage2 NULL
static const struct u_tracepoint __tp_radv_begin_CopyBufferToImage2 = {
    "radv_begin_CopyBufferToImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    112,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyBufferToImage2,
    __print_json_radv_begin_CopyBufferToImage2,
    __fuzzy_hash_radv_begin_CopyBufferToImage2,
    __fuzzy_equals_radv_begin_CopyBufferToImage2,
    __print_fuzzy_hash_args_radv_begin_CopyBufferToImage2,
};
void __trace_radv_begin_CopyBufferToImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyBufferToImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyBufferToImage2 *__entry =
      queueing ?
      (struct trace_radv_begin_CopyBufferToImage2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyBufferToImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyBufferToImage2
 */
  
#define __print_radv_end_CopyBufferToImage2 NULL
  
#define __print_json_radv_end_CopyBufferToImage2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyBufferToImage2 NULL

#define __fuzzy_hash_radv_end_CopyBufferToImage2 NULL
#define __fuzzy_equals_radv_end_CopyBufferToImage2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyBufferToImage2 NULL
static const struct u_tracepoint __tp_radv_end_CopyBufferToImage2 = {
    "radv_end_CopyBufferToImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    113,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyBufferToImage2,
    __print_json_radv_end_CopyBufferToImage2,
    __fuzzy_hash_radv_end_CopyBufferToImage2,
    __fuzzy_equals_radv_end_CopyBufferToImage2,
    __print_fuzzy_hash_args_radv_end_CopyBufferToImage2,
};
void __trace_radv_end_CopyBufferToImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyBufferToImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyBufferToImage2 *__entry =
      queueing ?
      (struct trace_radv_end_CopyBufferToImage2 *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyBufferToImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyImageToBuffer2
 */
  
#define __print_radv_begin_CopyImageToBuffer2 NULL
  
#define __print_json_radv_begin_CopyImageToBuffer2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImageToBuffer2 NULL

#define __fuzzy_hash_radv_begin_CopyImageToBuffer2 NULL
#define __fuzzy_equals_radv_begin_CopyImageToBuffer2 NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImageToBuffer2 NULL
static const struct u_tracepoint __tp_radv_begin_CopyImageToBuffer2 = {
    "radv_begin_CopyImageToBuffer2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    114,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyImageToBuffer2,
    __print_json_radv_begin_CopyImageToBuffer2,
    __fuzzy_hash_radv_begin_CopyImageToBuffer2,
    __fuzzy_equals_radv_begin_CopyImageToBuffer2,
    __print_fuzzy_hash_args_radv_begin_CopyImageToBuffer2,
};
void __trace_radv_begin_CopyImageToBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyImageToBuffer2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyImageToBuffer2 *__entry =
      queueing ?
      (struct trace_radv_begin_CopyImageToBuffer2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyImageToBuffer2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyImageToBuffer2
 */
  
#define __print_radv_end_CopyImageToBuffer2 NULL
  
#define __print_json_radv_end_CopyImageToBuffer2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyImageToBuffer2 NULL

#define __fuzzy_hash_radv_end_CopyImageToBuffer2 NULL
#define __fuzzy_equals_radv_end_CopyImageToBuffer2 NULL
#define __print_fuzzy_hash_args_radv_end_CopyImageToBuffer2 NULL
static const struct u_tracepoint __tp_radv_end_CopyImageToBuffer2 = {
    "radv_end_CopyImageToBuffer2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    115,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyImageToBuffer2,
    __print_json_radv_end_CopyImageToBuffer2,
    __fuzzy_hash_radv_end_CopyImageToBuffer2,
    __fuzzy_equals_radv_end_CopyImageToBuffer2,
    __print_fuzzy_hash_args_radv_end_CopyImageToBuffer2,
};
void __trace_radv_end_CopyImageToBuffer2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyImageToBuffer2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyImageToBuffer2 *__entry =
      queueing ?
      (struct trace_radv_end_CopyImageToBuffer2 *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyImageToBuffer2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ResolveImage2
 */
  
#define __print_radv_begin_ResolveImage2 NULL
  
#define __print_json_radv_begin_ResolveImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_ResolveImage2 NULL

#define __fuzzy_hash_radv_begin_ResolveImage2 NULL
#define __fuzzy_equals_radv_begin_ResolveImage2 NULL
#define __print_fuzzy_hash_args_radv_begin_ResolveImage2 NULL
static const struct u_tracepoint __tp_radv_begin_ResolveImage2 = {
    "radv_begin_ResolveImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    116,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ResolveImage2,
    __print_json_radv_begin_ResolveImage2,
    __fuzzy_hash_radv_begin_ResolveImage2,
    __fuzzy_equals_radv_begin_ResolveImage2,
    __print_fuzzy_hash_args_radv_begin_ResolveImage2,
};
void __trace_radv_begin_ResolveImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ResolveImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ResolveImage2 *__entry =
      queueing ?
      (struct trace_radv_begin_ResolveImage2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_ResolveImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ResolveImage2
 */
  
#define __print_radv_end_ResolveImage2 NULL
  
#define __print_json_radv_end_ResolveImage2 NULL
#define __print_fuzzy_hash_args_radv_end_ResolveImage2 NULL

#define __fuzzy_hash_radv_end_ResolveImage2 NULL
#define __fuzzy_equals_radv_end_ResolveImage2 NULL
#define __print_fuzzy_hash_args_radv_end_ResolveImage2 NULL
static const struct u_tracepoint __tp_radv_end_ResolveImage2 = {
    "radv_end_ResolveImage2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    117,
    u_tracepoint_type_end_range,
    __print_radv_end_ResolveImage2,
    __print_json_radv_end_ResolveImage2,
    __fuzzy_hash_radv_end_ResolveImage2,
    __fuzzy_equals_radv_end_ResolveImage2,
    __print_fuzzy_hash_args_radv_end_ResolveImage2,
};
void __trace_radv_end_ResolveImage2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ResolveImage2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ResolveImage2 *__entry =
      queueing ?
      (struct trace_radv_end_ResolveImage2 *)u_trace_appendv(ut, cs, &__tp_radv_end_ResolveImage2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_SetEvent2
 */
  
#define __print_radv_begin_SetEvent2 NULL
  
#define __print_json_radv_begin_SetEvent2 NULL
#define __print_fuzzy_hash_args_radv_begin_SetEvent2 NULL

#define __fuzzy_hash_radv_begin_SetEvent2 NULL
#define __fuzzy_equals_radv_begin_SetEvent2 NULL
#define __print_fuzzy_hash_args_radv_begin_SetEvent2 NULL
static const struct u_tracepoint __tp_radv_begin_SetEvent2 = {
    "radv_begin_SetEvent2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    118,
    u_tracepoint_type_begin_range,
    __print_radv_begin_SetEvent2,
    __print_json_radv_begin_SetEvent2,
    __fuzzy_hash_radv_begin_SetEvent2,
    __fuzzy_equals_radv_begin_SetEvent2,
    __print_fuzzy_hash_args_radv_begin_SetEvent2,
};
void __trace_radv_begin_SetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_SetEvent2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_SetEvent2 *__entry =
      queueing ?
      (struct trace_radv_begin_SetEvent2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_SetEvent2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_SetEvent2
 */
  
#define __print_radv_end_SetEvent2 NULL
  
#define __print_json_radv_end_SetEvent2 NULL
#define __print_fuzzy_hash_args_radv_end_SetEvent2 NULL

#define __fuzzy_hash_radv_end_SetEvent2 NULL
#define __fuzzy_equals_radv_end_SetEvent2 NULL
#define __print_fuzzy_hash_args_radv_end_SetEvent2 NULL
static const struct u_tracepoint __tp_radv_end_SetEvent2 = {
    "radv_end_SetEvent2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    119,
    u_tracepoint_type_end_range,
    __print_radv_end_SetEvent2,
    __print_json_radv_end_SetEvent2,
    __fuzzy_hash_radv_end_SetEvent2,
    __fuzzy_equals_radv_end_SetEvent2,
    __print_fuzzy_hash_args_radv_end_SetEvent2,
};
void __trace_radv_end_SetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_SetEvent2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_SetEvent2 *__entry =
      queueing ?
      (struct trace_radv_end_SetEvent2 *)u_trace_appendv(ut, cs, &__tp_radv_end_SetEvent2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_ResetEvent2
 */
  
#define __print_radv_begin_ResetEvent2 NULL
  
#define __print_json_radv_begin_ResetEvent2 NULL
#define __print_fuzzy_hash_args_radv_begin_ResetEvent2 NULL

#define __fuzzy_hash_radv_begin_ResetEvent2 NULL
#define __fuzzy_equals_radv_begin_ResetEvent2 NULL
#define __print_fuzzy_hash_args_radv_begin_ResetEvent2 NULL
static const struct u_tracepoint __tp_radv_begin_ResetEvent2 = {
    "radv_begin_ResetEvent2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    120,
    u_tracepoint_type_begin_range,
    __print_radv_begin_ResetEvent2,
    __print_json_radv_begin_ResetEvent2,
    __fuzzy_hash_radv_begin_ResetEvent2,
    __fuzzy_equals_radv_begin_ResetEvent2,
    __print_fuzzy_hash_args_radv_begin_ResetEvent2,
};
void __trace_radv_begin_ResetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_ResetEvent2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_ResetEvent2 *__entry =
      queueing ?
      (struct trace_radv_begin_ResetEvent2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_ResetEvent2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_ResetEvent2
 */
  
#define __print_radv_end_ResetEvent2 NULL
  
#define __print_json_radv_end_ResetEvent2 NULL
#define __print_fuzzy_hash_args_radv_end_ResetEvent2 NULL

#define __fuzzy_hash_radv_end_ResetEvent2 NULL
#define __fuzzy_equals_radv_end_ResetEvent2 NULL
#define __print_fuzzy_hash_args_radv_end_ResetEvent2 NULL
static const struct u_tracepoint __tp_radv_end_ResetEvent2 = {
    "radv_end_ResetEvent2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    121,
    u_tracepoint_type_end_range,
    __print_radv_end_ResetEvent2,
    __print_json_radv_end_ResetEvent2,
    __fuzzy_hash_radv_end_ResetEvent2,
    __fuzzy_equals_radv_end_ResetEvent2,
    __print_fuzzy_hash_args_radv_end_ResetEvent2,
};
void __trace_radv_end_ResetEvent2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_ResetEvent2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_ResetEvent2 *__entry =
      queueing ?
      (struct trace_radv_end_ResetEvent2 *)u_trace_appendv(ut, cs, &__tp_radv_end_ResetEvent2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_WaitEvents2
 */
  
#define __print_radv_begin_WaitEvents2 NULL
  
#define __print_json_radv_begin_WaitEvents2 NULL
#define __print_fuzzy_hash_args_radv_begin_WaitEvents2 NULL

#define __fuzzy_hash_radv_begin_WaitEvents2 NULL
#define __fuzzy_equals_radv_begin_WaitEvents2 NULL
#define __print_fuzzy_hash_args_radv_begin_WaitEvents2 NULL
static const struct u_tracepoint __tp_radv_begin_WaitEvents2 = {
    "radv_begin_WaitEvents2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    122,
    u_tracepoint_type_begin_range,
    __print_radv_begin_WaitEvents2,
    __print_json_radv_begin_WaitEvents2,
    __fuzzy_hash_radv_begin_WaitEvents2,
    __fuzzy_equals_radv_begin_WaitEvents2,
    __print_fuzzy_hash_args_radv_begin_WaitEvents2,
};
void __trace_radv_begin_WaitEvents2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_WaitEvents2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_WaitEvents2 *__entry =
      queueing ?
      (struct trace_radv_begin_WaitEvents2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_WaitEvents2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_WaitEvents2
 */
  
#define __print_radv_end_WaitEvents2 NULL
  
#define __print_json_radv_end_WaitEvents2 NULL
#define __print_fuzzy_hash_args_radv_end_WaitEvents2 NULL

#define __fuzzy_hash_radv_end_WaitEvents2 NULL
#define __fuzzy_equals_radv_end_WaitEvents2 NULL
#define __print_fuzzy_hash_args_radv_end_WaitEvents2 NULL
static const struct u_tracepoint __tp_radv_end_WaitEvents2 = {
    "radv_end_WaitEvents2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    123,
    u_tracepoint_type_end_range,
    __print_radv_end_WaitEvents2,
    __print_json_radv_end_WaitEvents2,
    __fuzzy_hash_radv_end_WaitEvents2,
    __fuzzy_equals_radv_end_WaitEvents2,
    __print_fuzzy_hash_args_radv_end_WaitEvents2,
};
void __trace_radv_end_WaitEvents2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_WaitEvents2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_WaitEvents2 *__entry =
      queueing ?
      (struct trace_radv_end_WaitEvents2 *)u_trace_appendv(ut, cs, &__tp_radv_end_WaitEvents2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_PipelineBarrier2
 */
  
#define __print_radv_begin_PipelineBarrier2 NULL
  
#define __print_json_radv_begin_PipelineBarrier2 NULL
#define __print_fuzzy_hash_args_radv_begin_PipelineBarrier2 NULL

#define __fuzzy_hash_radv_begin_PipelineBarrier2 NULL
#define __fuzzy_equals_radv_begin_PipelineBarrier2 NULL
#define __print_fuzzy_hash_args_radv_begin_PipelineBarrier2 NULL
static const struct u_tracepoint __tp_radv_begin_PipelineBarrier2 = {
    "radv_begin_PipelineBarrier2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    124,
    u_tracepoint_type_begin_range,
    __print_radv_begin_PipelineBarrier2,
    __print_json_radv_begin_PipelineBarrier2,
    __fuzzy_hash_radv_begin_PipelineBarrier2,
    __fuzzy_equals_radv_begin_PipelineBarrier2,
    __print_fuzzy_hash_args_radv_begin_PipelineBarrier2,
};
void __trace_radv_begin_PipelineBarrier2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_PipelineBarrier2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_PipelineBarrier2 *__entry =
      queueing ?
      (struct trace_radv_begin_PipelineBarrier2 *)u_trace_appendv(ut, cs, &__tp_radv_begin_PipelineBarrier2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_PipelineBarrier2
 */
  
#define __print_radv_end_PipelineBarrier2 NULL
  
#define __print_json_radv_end_PipelineBarrier2 NULL
#define __print_fuzzy_hash_args_radv_end_PipelineBarrier2 NULL

#define __fuzzy_hash_radv_end_PipelineBarrier2 NULL
#define __fuzzy_equals_radv_end_PipelineBarrier2 NULL
#define __print_fuzzy_hash_args_radv_end_PipelineBarrier2 NULL
static const struct u_tracepoint __tp_radv_end_PipelineBarrier2 = {
    "radv_end_PipelineBarrier2",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    125,
    u_tracepoint_type_end_range,
    __print_radv_end_PipelineBarrier2,
    __print_json_radv_end_PipelineBarrier2,
    __fuzzy_hash_radv_end_PipelineBarrier2,
    __fuzzy_equals_radv_end_PipelineBarrier2,
    __print_fuzzy_hash_args_radv_end_PipelineBarrier2,
};
void __trace_radv_end_PipelineBarrier2(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_PipelineBarrier2 entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_PipelineBarrier2 *__entry =
      queueing ?
      (struct trace_radv_end_PipelineBarrier2 *)u_trace_appendv(ut, cs, &__tp_radv_end_PipelineBarrier2,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyMemoryKHR
 */
  
#define __print_radv_begin_CopyMemoryKHR NULL
  
#define __print_json_radv_begin_CopyMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryKHR NULL

#define __fuzzy_hash_radv_begin_CopyMemoryKHR NULL
#define __fuzzy_equals_radv_begin_CopyMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyMemoryKHR = {
    "radv_begin_CopyMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    126,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyMemoryKHR,
    __print_json_radv_begin_CopyMemoryKHR,
    __fuzzy_hash_radv_begin_CopyMemoryKHR,
    __fuzzy_equals_radv_begin_CopyMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_CopyMemoryKHR,
};
void __trace_radv_begin_CopyMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyMemoryKHR
 */
  
#define __print_radv_end_CopyMemoryKHR NULL
  
#define __print_json_radv_end_CopyMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryKHR NULL

#define __fuzzy_hash_radv_end_CopyMemoryKHR NULL
#define __fuzzy_equals_radv_end_CopyMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyMemoryKHR = {
    "radv_end_CopyMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    127,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyMemoryKHR,
    __print_json_radv_end_CopyMemoryKHR,
    __fuzzy_hash_radv_end_CopyMemoryKHR,
    __fuzzy_equals_radv_end_CopyMemoryKHR,
    __print_fuzzy_hash_args_radv_end_CopyMemoryKHR,
};
void __trace_radv_end_CopyMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyMemoryToImageKHR
 */
  
#define __print_radv_begin_CopyMemoryToImageKHR NULL
  
#define __print_json_radv_begin_CopyMemoryToImageKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageKHR NULL

#define __fuzzy_hash_radv_begin_CopyMemoryToImageKHR NULL
#define __fuzzy_equals_radv_begin_CopyMemoryToImageKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyMemoryToImageKHR = {
    "radv_begin_CopyMemoryToImageKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    128,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyMemoryToImageKHR,
    __print_json_radv_begin_CopyMemoryToImageKHR,
    __fuzzy_hash_radv_begin_CopyMemoryToImageKHR,
    __fuzzy_equals_radv_begin_CopyMemoryToImageKHR,
    __print_fuzzy_hash_args_radv_begin_CopyMemoryToImageKHR,
};
void __trace_radv_begin_CopyMemoryToImageKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyMemoryToImageKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyMemoryToImageKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyMemoryToImageKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyMemoryToImageKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyMemoryToImageKHR
 */
  
#define __print_radv_end_CopyMemoryToImageKHR NULL
  
#define __print_json_radv_end_CopyMemoryToImageKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToImageKHR NULL

#define __fuzzy_hash_radv_end_CopyMemoryToImageKHR NULL
#define __fuzzy_equals_radv_end_CopyMemoryToImageKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyMemoryToImageKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyMemoryToImageKHR = {
    "radv_end_CopyMemoryToImageKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    129,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyMemoryToImageKHR,
    __print_json_radv_end_CopyMemoryToImageKHR,
    __fuzzy_hash_radv_end_CopyMemoryToImageKHR,
    __fuzzy_equals_radv_end_CopyMemoryToImageKHR,
    __print_fuzzy_hash_args_radv_end_CopyMemoryToImageKHR,
};
void __trace_radv_end_CopyMemoryToImageKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyMemoryToImageKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyMemoryToImageKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyMemoryToImageKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyMemoryToImageKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyImageToMemoryKHR
 */
  
#define __print_radv_begin_CopyImageToMemoryKHR NULL
  
#define __print_json_radv_begin_CopyImageToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImageToMemoryKHR NULL

#define __fuzzy_hash_radv_begin_CopyImageToMemoryKHR NULL
#define __fuzzy_equals_radv_begin_CopyImageToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyImageToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyImageToMemoryKHR = {
    "radv_begin_CopyImageToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    130,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyImageToMemoryKHR,
    __print_json_radv_begin_CopyImageToMemoryKHR,
    __fuzzy_hash_radv_begin_CopyImageToMemoryKHR,
    __fuzzy_equals_radv_begin_CopyImageToMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_CopyImageToMemoryKHR,
};
void __trace_radv_begin_CopyImageToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyImageToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyImageToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyImageToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyImageToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyImageToMemoryKHR
 */
  
#define __print_radv_end_CopyImageToMemoryKHR NULL
  
#define __print_json_radv_end_CopyImageToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyImageToMemoryKHR NULL

#define __fuzzy_hash_radv_end_CopyImageToMemoryKHR NULL
#define __fuzzy_equals_radv_end_CopyImageToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyImageToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyImageToMemoryKHR = {
    "radv_end_CopyImageToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    131,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyImageToMemoryKHR,
    __print_json_radv_end_CopyImageToMemoryKHR,
    __fuzzy_hash_radv_end_CopyImageToMemoryKHR,
    __fuzzy_equals_radv_end_CopyImageToMemoryKHR,
    __print_fuzzy_hash_args_radv_end_CopyImageToMemoryKHR,
};
void __trace_radv_end_CopyImageToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyImageToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyImageToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyImageToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyImageToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_UpdateMemoryKHR
 */
  
#define __print_radv_begin_UpdateMemoryKHR NULL
  
#define __print_json_radv_begin_UpdateMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_UpdateMemoryKHR NULL

#define __fuzzy_hash_radv_begin_UpdateMemoryKHR NULL
#define __fuzzy_equals_radv_begin_UpdateMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_UpdateMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_UpdateMemoryKHR = {
    "radv_begin_UpdateMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    132,
    u_tracepoint_type_begin_range,
    __print_radv_begin_UpdateMemoryKHR,
    __print_json_radv_begin_UpdateMemoryKHR,
    __fuzzy_hash_radv_begin_UpdateMemoryKHR,
    __fuzzy_equals_radv_begin_UpdateMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_UpdateMemoryKHR,
};
void __trace_radv_begin_UpdateMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_UpdateMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_UpdateMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_UpdateMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_UpdateMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_UpdateMemoryKHR
 */
  
#define __print_radv_end_UpdateMemoryKHR NULL
  
#define __print_json_radv_end_UpdateMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_UpdateMemoryKHR NULL

#define __fuzzy_hash_radv_end_UpdateMemoryKHR NULL
#define __fuzzy_equals_radv_end_UpdateMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_UpdateMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_UpdateMemoryKHR = {
    "radv_end_UpdateMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    133,
    u_tracepoint_type_end_range,
    __print_radv_end_UpdateMemoryKHR,
    __print_json_radv_end_UpdateMemoryKHR,
    __fuzzy_hash_radv_end_UpdateMemoryKHR,
    __fuzzy_equals_radv_end_UpdateMemoryKHR,
    __print_fuzzy_hash_args_radv_end_UpdateMemoryKHR,
};
void __trace_radv_end_UpdateMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_UpdateMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_UpdateMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_UpdateMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_UpdateMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_FillMemoryKHR
 */
  
#define __print_radv_begin_FillMemoryKHR NULL
  
#define __print_json_radv_begin_FillMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_FillMemoryKHR NULL

#define __fuzzy_hash_radv_begin_FillMemoryKHR NULL
#define __fuzzy_equals_radv_begin_FillMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_FillMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_FillMemoryKHR = {
    "radv_begin_FillMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    134,
    u_tracepoint_type_begin_range,
    __print_radv_begin_FillMemoryKHR,
    __print_json_radv_begin_FillMemoryKHR,
    __fuzzy_hash_radv_begin_FillMemoryKHR,
    __fuzzy_equals_radv_begin_FillMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_FillMemoryKHR,
};
void __trace_radv_begin_FillMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_FillMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_FillMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_FillMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_FillMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_FillMemoryKHR
 */
  
#define __print_radv_end_FillMemoryKHR NULL
  
#define __print_json_radv_end_FillMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_FillMemoryKHR NULL

#define __fuzzy_hash_radv_end_FillMemoryKHR NULL
#define __fuzzy_equals_radv_end_FillMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_FillMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_FillMemoryKHR = {
    "radv_end_FillMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    135,
    u_tracepoint_type_end_range,
    __print_radv_end_FillMemoryKHR,
    __print_json_radv_end_FillMemoryKHR,
    __fuzzy_hash_radv_end_FillMemoryKHR,
    __fuzzy_equals_radv_end_FillMemoryKHR,
    __print_fuzzy_hash_args_radv_end_FillMemoryKHR,
};
void __trace_radv_end_FillMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_FillMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_FillMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_FillMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_FillMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_CopyQueryPoolResultsToMemoryKHR
 */
  
#define __print_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL
  
#define __print_json_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL

#define __fuzzy_hash_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL
#define __fuzzy_equals_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResultsToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_begin_CopyQueryPoolResultsToMemoryKHR = {
    "radv_begin_CopyQueryPoolResultsToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    136,
    u_tracepoint_type_begin_range,
    __print_radv_begin_CopyQueryPoolResultsToMemoryKHR,
    __print_json_radv_begin_CopyQueryPoolResultsToMemoryKHR,
    __fuzzy_hash_radv_begin_CopyQueryPoolResultsToMemoryKHR,
    __fuzzy_equals_radv_begin_CopyQueryPoolResultsToMemoryKHR,
    __print_fuzzy_hash_args_radv_begin_CopyQueryPoolResultsToMemoryKHR,
};
void __trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_CopyQueryPoolResultsToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_CopyQueryPoolResultsToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_begin_CopyQueryPoolResultsToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_CopyQueryPoolResultsToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_CopyQueryPoolResultsToMemoryKHR
 */
  
#define __print_radv_end_CopyQueryPoolResultsToMemoryKHR NULL
  
#define __print_json_radv_end_CopyQueryPoolResultsToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyQueryPoolResultsToMemoryKHR NULL

#define __fuzzy_hash_radv_end_CopyQueryPoolResultsToMemoryKHR NULL
#define __fuzzy_equals_radv_end_CopyQueryPoolResultsToMemoryKHR NULL
#define __print_fuzzy_hash_args_radv_end_CopyQueryPoolResultsToMemoryKHR NULL
static const struct u_tracepoint __tp_radv_end_CopyQueryPoolResultsToMemoryKHR = {
    "radv_end_CopyQueryPoolResultsToMemoryKHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    137,
    u_tracepoint_type_end_range,
    __print_radv_end_CopyQueryPoolResultsToMemoryKHR,
    __print_json_radv_end_CopyQueryPoolResultsToMemoryKHR,
    __fuzzy_hash_radv_end_CopyQueryPoolResultsToMemoryKHR,
    __fuzzy_equals_radv_end_CopyQueryPoolResultsToMemoryKHR,
    __print_fuzzy_hash_args_radv_end_CopyQueryPoolResultsToMemoryKHR,
};
void __trace_radv_end_CopyQueryPoolResultsToMemoryKHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_CopyQueryPoolResultsToMemoryKHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_CopyQueryPoolResultsToMemoryKHR *__entry =
      queueing ?
      (struct trace_radv_end_CopyQueryPoolResultsToMemoryKHR *)u_trace_appendv(ut, cs, &__tp_radv_end_CopyQueryPoolResultsToMemoryKHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirectByteCount2EXT
 */
  
static void __print_radv_begin_DrawIndirectByteCount2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCount2EXT *)arg;
   fprintf(out, ""
      "instance_count=%u, "
         "\n"
   ,__entry->instance_count
   );
}

  
static void __print_json_radv_begin_DrawIndirectByteCount2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCount2EXT *)arg;
   fprintf(out, ""
      "\"instance_count\": \"%u\""
   ,__entry->instance_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirectByteCount2EXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirectByteCount2EXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry =
      (const struct trace_radv_begin_DrawIndirectByteCount2EXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirectByteCount2EXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry_a =
      (const struct trace_radv_begin_DrawIndirectByteCount2EXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry_b =
      (const struct trace_radv_begin_DrawIndirectByteCount2EXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirectByteCount2EXT = {
    "radv_begin_DrawIndirectByteCount2EXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirectByteCount2EXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    138,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirectByteCount2EXT,
    __print_json_radv_begin_DrawIndirectByteCount2EXT,
    __fuzzy_hash_radv_begin_DrawIndirectByteCount2EXT,
    __fuzzy_equals_radv_begin_DrawIndirectByteCount2EXT,
    __print_fuzzy_hash_args_radv_begin_DrawIndirectByteCount2EXT,
};
void __trace_radv_begin_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t instance_count
) {
   struct trace_radv_begin_DrawIndirectByteCount2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirectByteCount2EXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirectByteCount2EXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirectByteCount2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->instance_count = instance_count;
}

/*
 * radv_end_DrawIndirectByteCount2EXT
 */
  
#define __print_radv_end_DrawIndirectByteCount2EXT NULL
  
#define __print_json_radv_end_DrawIndirectByteCount2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectByteCount2EXT NULL

#define __fuzzy_hash_radv_end_DrawIndirectByteCount2EXT NULL
#define __fuzzy_equals_radv_end_DrawIndirectByteCount2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectByteCount2EXT NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirectByteCount2EXT = {
    "radv_end_DrawIndirectByteCount2EXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    139,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirectByteCount2EXT,
    __print_json_radv_end_DrawIndirectByteCount2EXT,
    __fuzzy_hash_radv_end_DrawIndirectByteCount2EXT,
    __fuzzy_equals_radv_end_DrawIndirectByteCount2EXT,
    __print_fuzzy_hash_args_radv_end_DrawIndirectByteCount2EXT,
};
void __trace_radv_end_DrawIndirectByteCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirectByteCount2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirectByteCount2EXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirectByteCount2EXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirectByteCount2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirect2KHR
 */
  
static void __print_radv_begin_DrawIndirect2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirect2KHR *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndirect2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirect2KHR *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirect2KHR NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirect2KHR(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirect2KHR *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirect2KHR(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirect2KHR *__entry_a =
      (const struct trace_radv_begin_DrawIndirect2KHR *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirect2KHR *__entry_b =
      (const struct trace_radv_begin_DrawIndirect2KHR *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirect2KHR = {
    "radv_begin_DrawIndirect2KHR",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirect2KHR), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    140,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirect2KHR,
    __print_json_radv_begin_DrawIndirect2KHR,
    __fuzzy_hash_radv_begin_DrawIndirect2KHR,
    __fuzzy_equals_radv_begin_DrawIndirect2KHR,
    __print_fuzzy_hash_args_radv_begin_DrawIndirect2KHR,
};
void __trace_radv_begin_DrawIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawIndirect2KHR
 */
  
#define __print_radv_end_DrawIndirect2KHR NULL
  
#define __print_json_radv_end_DrawIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirect2KHR NULL

#define __fuzzy_hash_radv_end_DrawIndirect2KHR NULL
#define __fuzzy_equals_radv_end_DrawIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirect2KHR = {
    "radv_end_DrawIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    141,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirect2KHR,
    __print_json_radv_end_DrawIndirect2KHR,
    __fuzzy_hash_radv_end_DrawIndirect2KHR,
    __fuzzy_equals_radv_end_DrawIndirect2KHR,
    __print_fuzzy_hash_args_radv_end_DrawIndirect2KHR,
};
void __trace_radv_end_DrawIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndexedIndirect2KHR
 */
  
static void __print_radv_begin_DrawIndexedIndirect2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect2KHR *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndexedIndirect2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect2KHR *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirect2KHR NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndexedIndirect2KHR(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirect2KHR *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndexedIndirect2KHR(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry_a =
      (const struct trace_radv_begin_DrawIndexedIndirect2KHR *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry_b =
      (const struct trace_radv_begin_DrawIndexedIndirect2KHR *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndexedIndirect2KHR = {
    "radv_begin_DrawIndexedIndirect2KHR",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndexedIndirect2KHR), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    142,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndexedIndirect2KHR,
    __print_json_radv_begin_DrawIndexedIndirect2KHR,
    __fuzzy_hash_radv_begin_DrawIndexedIndirect2KHR,
    __fuzzy_equals_radv_begin_DrawIndexedIndirect2KHR,
    __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirect2KHR,
};
void __trace_radv_begin_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawIndexedIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndexedIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndexedIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndexedIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawIndexedIndirect2KHR
 */
  
#define __print_radv_end_DrawIndexedIndirect2KHR NULL
  
#define __print_json_radv_end_DrawIndexedIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect2KHR NULL

#define __fuzzy_hash_radv_end_DrawIndexedIndirect2KHR NULL
#define __fuzzy_equals_radv_end_DrawIndexedIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_end_DrawIndexedIndirect2KHR = {
    "radv_end_DrawIndexedIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    143,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndexedIndirect2KHR,
    __print_json_radv_end_DrawIndexedIndirect2KHR,
    __fuzzy_hash_radv_end_DrawIndexedIndirect2KHR,
    __fuzzy_equals_radv_end_DrawIndexedIndirect2KHR,
    __print_fuzzy_hash_args_radv_end_DrawIndexedIndirect2KHR,
};
void __trace_radv_end_DrawIndexedIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndexedIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndexedIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndexedIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndexedIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndirectCount2KHR
 */
  
static void __print_radv_begin_DrawIndirectCount2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirectCount2KHR *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndirectCount2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirectCount2KHR *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndirectCount2KHR NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndirectCount2KHR(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndirectCount2KHR *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndirectCount2KHR(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndirectCount2KHR *__entry_a =
      (const struct trace_radv_begin_DrawIndirectCount2KHR *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndirectCount2KHR *__entry_b =
      (const struct trace_radv_begin_DrawIndirectCount2KHR *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndirectCount2KHR = {
    "radv_begin_DrawIndirectCount2KHR",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndirectCount2KHR), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    144,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndirectCount2KHR,
    __print_json_radv_begin_DrawIndirectCount2KHR,
    __fuzzy_hash_radv_begin_DrawIndirectCount2KHR,
    __fuzzy_equals_radv_begin_DrawIndirectCount2KHR,
    __print_fuzzy_hash_args_radv_begin_DrawIndirectCount2KHR,
};
void __trace_radv_begin_DrawIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawIndirectCount2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndirectCount2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndirectCount2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndirectCount2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawIndirectCount2KHR
 */
  
#define __print_radv_end_DrawIndirectCount2KHR NULL
  
#define __print_json_radv_end_DrawIndirectCount2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectCount2KHR NULL

#define __fuzzy_hash_radv_end_DrawIndirectCount2KHR NULL
#define __fuzzy_equals_radv_end_DrawIndirectCount2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndirectCount2KHR NULL
static const struct u_tracepoint __tp_radv_end_DrawIndirectCount2KHR = {
    "radv_end_DrawIndirectCount2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    145,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndirectCount2KHR,
    __print_json_radv_end_DrawIndirectCount2KHR,
    __fuzzy_hash_radv_end_DrawIndirectCount2KHR,
    __fuzzy_equals_radv_end_DrawIndirectCount2KHR,
    __print_fuzzy_hash_args_radv_end_DrawIndirectCount2KHR,
};
void __trace_radv_end_DrawIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndirectCount2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndirectCount2KHR *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndirectCount2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndirectCount2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawIndexedIndirectCount2KHR
 */
  
static void __print_radv_begin_DrawIndexedIndirectCount2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawIndexedIndirectCount2KHR(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirectCount2KHR NULL

static uint32_t __fuzzy_hash_radv_begin_DrawIndexedIndirectCount2KHR(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry =
      (const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawIndexedIndirectCount2KHR(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry_a =
      (const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry_b =
      (const struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawIndexedIndirectCount2KHR = {
    "radv_begin_DrawIndexedIndirectCount2KHR",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawIndexedIndirectCount2KHR), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    146,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawIndexedIndirectCount2KHR,
    __print_json_radv_begin_DrawIndexedIndirectCount2KHR,
    __fuzzy_hash_radv_begin_DrawIndexedIndirectCount2KHR,
    __fuzzy_equals_radv_begin_DrawIndexedIndirectCount2KHR,
    __print_fuzzy_hash_args_radv_begin_DrawIndexedIndirectCount2KHR,
};
void __trace_radv_begin_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawIndexedIndirectCount2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawIndexedIndirectCount2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_DrawIndexedIndirectCount2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawIndexedIndirectCount2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawIndexedIndirectCount2KHR
 */
  
#define __print_radv_end_DrawIndexedIndirectCount2KHR NULL
  
#define __print_json_radv_end_DrawIndexedIndirectCount2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount2KHR NULL

#define __fuzzy_hash_radv_end_DrawIndexedIndirectCount2KHR NULL
#define __fuzzy_equals_radv_end_DrawIndexedIndirectCount2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount2KHR NULL
static const struct u_tracepoint __tp_radv_end_DrawIndexedIndirectCount2KHR = {
    "radv_end_DrawIndexedIndirectCount2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    147,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawIndexedIndirectCount2KHR,
    __print_json_radv_end_DrawIndexedIndirectCount2KHR,
    __fuzzy_hash_radv_end_DrawIndexedIndirectCount2KHR,
    __fuzzy_equals_radv_end_DrawIndexedIndirectCount2KHR,
    __print_fuzzy_hash_args_radv_end_DrawIndexedIndirectCount2KHR,
};
void __trace_radv_end_DrawIndexedIndirectCount2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawIndexedIndirectCount2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawIndexedIndirectCount2KHR *__entry =
      queueing ?
      (struct trace_radv_end_DrawIndexedIndirectCount2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawIndexedIndirectCount2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMeshTasksIndirect2EXT
 */
  
static void __print_radv_begin_DrawMeshTasksIndirect2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)arg;
   fprintf(out, ""
      "draw_count=%u, "
         "\n"
   ,__entry->draw_count
   );
}

  
static void __print_json_radv_begin_DrawMeshTasksIndirect2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)arg;
   fprintf(out, ""
      "\"draw_count\": \"%u\""
   ,__entry->draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirect2EXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMeshTasksIndirect2EXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMeshTasksIndirect2EXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry_a =
      (const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry_b =
      (const struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMeshTasksIndirect2EXT = {
    "radv_begin_DrawMeshTasksIndirect2EXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMeshTasksIndirect2EXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    148,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMeshTasksIndirect2EXT,
    __print_json_radv_begin_DrawMeshTasksIndirect2EXT,
    __fuzzy_hash_radv_begin_DrawMeshTasksIndirect2EXT,
    __fuzzy_equals_radv_begin_DrawMeshTasksIndirect2EXT,
    __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirect2EXT,
};
void __trace_radv_begin_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t draw_count
) {
   struct trace_radv_begin_DrawMeshTasksIndirect2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMeshTasksIndirect2EXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMeshTasksIndirect2EXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMeshTasksIndirect2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->draw_count = draw_count;
}

/*
 * radv_end_DrawMeshTasksIndirect2EXT
 */
  
#define __print_radv_end_DrawMeshTasksIndirect2EXT NULL
  
#define __print_json_radv_end_DrawMeshTasksIndirect2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirect2EXT NULL

#define __fuzzy_hash_radv_end_DrawMeshTasksIndirect2EXT NULL
#define __fuzzy_equals_radv_end_DrawMeshTasksIndirect2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirect2EXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMeshTasksIndirect2EXT = {
    "radv_end_DrawMeshTasksIndirect2EXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    149,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMeshTasksIndirect2EXT,
    __print_json_radv_end_DrawMeshTasksIndirect2EXT,
    __fuzzy_hash_radv_end_DrawMeshTasksIndirect2EXT,
    __fuzzy_equals_radv_end_DrawMeshTasksIndirect2EXT,
    __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirect2EXT,
};
void __trace_radv_end_DrawMeshTasksIndirect2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMeshTasksIndirect2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMeshTasksIndirect2EXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMeshTasksIndirect2EXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMeshTasksIndirect2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DrawMeshTasksIndirectCount2EXT
 */
  
static void __print_radv_begin_DrawMeshTasksIndirectCount2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)arg;
   fprintf(out, ""
      "max_draw_count=%u, "
         "\n"
   ,__entry->max_draw_count
   );
}

  
static void __print_json_radv_begin_DrawMeshTasksIndirectCount2EXT(FILE *out, const void *arg, const void *indirect) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)arg;
   fprintf(out, ""
      "\"max_draw_count\": \"%u\""
   ,__entry->max_draw_count
   );
}

#define __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectCount2EXT NULL

static uint32_t __fuzzy_hash_radv_begin_DrawMeshTasksIndirectCount2EXT(const void *_event) {
   uint32_t hash = 1;
   const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)_event;
   (void)__entry;
   return hash;
}

static bool __fuzzy_equals_radv_begin_DrawMeshTasksIndirectCount2EXT(const void *_a, const void *_b) {
   const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry_a =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)_a;
   (void)__entry_a;
   const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry_b =
      (const struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)_b;
   (void)__entry_b;
   return true;
}

static const struct u_tracepoint __tp_radv_begin_DrawMeshTasksIndirectCount2EXT = {
    "radv_begin_DrawMeshTasksIndirectCount2EXT",
    ALIGN_POT(sizeof(struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT), 8),   /* keep size 64b aligned */
    0
    ,
    0,
    150,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DrawMeshTasksIndirectCount2EXT,
    __print_json_radv_begin_DrawMeshTasksIndirectCount2EXT,
    __fuzzy_hash_radv_begin_DrawMeshTasksIndirectCount2EXT,
    __fuzzy_equals_radv_begin_DrawMeshTasksIndirectCount2EXT,
    __print_fuzzy_hash_args_radv_begin_DrawMeshTasksIndirectCount2EXT,
};
void __trace_radv_begin_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
   , uint32_t max_draw_count
) {
   struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *__entry =
      queueing ?
      (struct trace_radv_begin_DrawMeshTasksIndirectCount2EXT *)u_trace_appendv(ut, cs, &__tp_radv_begin_DrawMeshTasksIndirectCount2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
   __entry->max_draw_count = max_draw_count;
}

/*
 * radv_end_DrawMeshTasksIndirectCount2EXT
 */
  
#define __print_radv_end_DrawMeshTasksIndirectCount2EXT NULL
  
#define __print_json_radv_end_DrawMeshTasksIndirectCount2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCount2EXT NULL

#define __fuzzy_hash_radv_end_DrawMeshTasksIndirectCount2EXT NULL
#define __fuzzy_equals_radv_end_DrawMeshTasksIndirectCount2EXT NULL
#define __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCount2EXT NULL
static const struct u_tracepoint __tp_radv_end_DrawMeshTasksIndirectCount2EXT = {
    "radv_end_DrawMeshTasksIndirectCount2EXT",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    151,
    u_tracepoint_type_end_range,
    __print_radv_end_DrawMeshTasksIndirectCount2EXT,
    __print_json_radv_end_DrawMeshTasksIndirectCount2EXT,
    __fuzzy_hash_radv_end_DrawMeshTasksIndirectCount2EXT,
    __fuzzy_equals_radv_end_DrawMeshTasksIndirectCount2EXT,
    __print_fuzzy_hash_args_radv_end_DrawMeshTasksIndirectCount2EXT,
};
void __trace_radv_end_DrawMeshTasksIndirectCount2EXT(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DrawMeshTasksIndirectCount2EXT entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DrawMeshTasksIndirectCount2EXT *__entry =
      queueing ?
      (struct trace_radv_end_DrawMeshTasksIndirectCount2EXT *)u_trace_appendv(ut, cs, &__tp_radv_end_DrawMeshTasksIndirectCount2EXT,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_begin_DispatchIndirect2KHR
 */
  
#define __print_radv_begin_DispatchIndirect2KHR NULL
  
#define __print_json_radv_begin_DispatchIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_begin_DispatchIndirect2KHR NULL

#define __fuzzy_hash_radv_begin_DispatchIndirect2KHR NULL
#define __fuzzy_equals_radv_begin_DispatchIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_begin_DispatchIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_begin_DispatchIndirect2KHR = {
    "radv_begin_DispatchIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    152,
    u_tracepoint_type_begin_range,
    __print_radv_begin_DispatchIndirect2KHR,
    __print_json_radv_begin_DispatchIndirect2KHR,
    __fuzzy_hash_radv_begin_DispatchIndirect2KHR,
    __fuzzy_equals_radv_begin_DispatchIndirect2KHR,
    __print_fuzzy_hash_args_radv_begin_DispatchIndirect2KHR,
};
void __trace_radv_begin_DispatchIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_begin_DispatchIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_begin_DispatchIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_begin_DispatchIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_begin_DispatchIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

/*
 * radv_end_DispatchIndirect2KHR
 */
  
#define __print_radv_end_DispatchIndirect2KHR NULL
  
#define __print_json_radv_end_DispatchIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DispatchIndirect2KHR NULL

#define __fuzzy_hash_radv_end_DispatchIndirect2KHR NULL
#define __fuzzy_equals_radv_end_DispatchIndirect2KHR NULL
#define __print_fuzzy_hash_args_radv_end_DispatchIndirect2KHR NULL
static const struct u_tracepoint __tp_radv_end_DispatchIndirect2KHR = {
    "radv_end_DispatchIndirect2KHR",
    ALIGN_POT(0, 8),   /* keep size 64b aligned */
    0
    ,
    0,
    153,
    u_tracepoint_type_end_range,
    __print_radv_end_DispatchIndirect2KHR,
    __print_json_radv_end_DispatchIndirect2KHR,
    __fuzzy_hash_radv_end_DispatchIndirect2KHR,
    __fuzzy_equals_radv_end_DispatchIndirect2KHR,
    __print_fuzzy_hash_args_radv_end_DispatchIndirect2KHR,
};
void __trace_radv_end_DispatchIndirect2KHR(
     struct u_trace *ut
   , enum u_trace_type enabled_traces
   , void *cs
) {
   struct trace_radv_end_DispatchIndirect2KHR entry;
   const bool queueing = enabled_traces & U_TRACE_TYPE_REQUIRE_QUEUING;
   UNUSED struct trace_radv_end_DispatchIndirect2KHR *__entry =
      queueing ?
      (struct trace_radv_end_DispatchIndirect2KHR *)u_trace_appendv(ut, cs, &__tp_radv_end_DispatchIndirect2KHR,
                                                    0
                                                    ,
                                                    0, NULL, NULL
                                                    ) :
      &entry;
}

