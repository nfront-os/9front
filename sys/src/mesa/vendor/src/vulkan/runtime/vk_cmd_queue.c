
/* Copyright © 2015-2021 Intel Corporation
 * Copyright © 2021 Collabora, Ltd.
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

/* This file generated from vk_cmd_queue_gen.py, don't edit directly. */

#include "vk_cmd_queue.h"

#define VK_PROTOTYPES
#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif

#include "vk_cmd_enqueue_entrypoints.h"
#include "vk_command_buffer.h"
#include "vk_dispatch_table.h"
#include "vk_device.h"
#include "vulkan/runtime/vk_pipeline_layout.h"
#include "vulkan/runtime/vk_descriptor_update_template.h"
#include "vulkan/runtime/vk_descriptor_set_layout.h"

const char *vk_cmd_queue_type_names[] = {
   "VK_CMD_BIND_PIPELINE",
   "VK_CMD_SET_PRIMITIVE_RESTART_INDEX_EXT",
   "VK_CMD_SET_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT",
   "VK_CMD_SET_VIEWPORT",
   "VK_CMD_SET_SCISSOR",
   "VK_CMD_SET_LINE_WIDTH",
   "VK_CMD_SET_DEPTH_BIAS",
   "VK_CMD_SET_BLEND_CONSTANTS",
   "VK_CMD_SET_DEPTH_BOUNDS",
   "VK_CMD_SET_STENCIL_COMPARE_MASK",
   "VK_CMD_SET_STENCIL_WRITE_MASK",
   "VK_CMD_SET_STENCIL_REFERENCE",
   "VK_CMD_BIND_DESCRIPTOR_SETS",
   "VK_CMD_BIND_INDEX_BUFFER",
   "VK_CMD_BIND_VERTEX_BUFFERS",
   "VK_CMD_DRAW",
   "VK_CMD_DRAW_INDEXED",
   "VK_CMD_DRAW_MULTI_EXT",
   "VK_CMD_DRAW_MULTI_INDEXED_EXT",
   "VK_CMD_DRAW_INDIRECT",
   "VK_CMD_DRAW_INDEXED_INDIRECT",
   "VK_CMD_DISPATCH",
   "VK_CMD_DISPATCH_INDIRECT",
   "VK_CMD_SUBPASS_SHADING_HUAWEI",
   "VK_CMD_DRAW_CLUSTER_HUAWEI",
   "VK_CMD_DRAW_CLUSTER_INDIRECT_HUAWEI",
   "VK_CMD_UPDATE_PIPELINE_INDIRECT_BUFFER_NV",
   "VK_CMD_COPY_BUFFER",
   "VK_CMD_COPY_IMAGE",
   "VK_CMD_BLIT_IMAGE",
   "VK_CMD_COPY_BUFFER_TO_IMAGE",
   "VK_CMD_COPY_IMAGE_TO_BUFFER",
   "VK_CMD_COPY_MEMORY_INDIRECT_NV",
   "VK_CMD_COPY_MEMORY_INDIRECT_KHR",
   "VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_NV",
   "VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_KHR",
   "VK_CMD_UPDATE_BUFFER",
   "VK_CMD_FILL_BUFFER",
   "VK_CMD_CLEAR_COLOR_IMAGE",
   "VK_CMD_CLEAR_DEPTH_STENCIL_IMAGE",
   "VK_CMD_CLEAR_ATTACHMENTS",
   "VK_CMD_RESOLVE_IMAGE",
   "VK_CMD_SET_EVENT",
   "VK_CMD_RESET_EVENT",
   "VK_CMD_WAIT_EVENTS",
   "VK_CMD_PIPELINE_BARRIER",
   "VK_CMD_BEGIN_QUERY",
   "VK_CMD_END_QUERY",
   "VK_CMD_BEGIN_CONDITIONAL_RENDERING_EXT",
   "VK_CMD_END_CONDITIONAL_RENDERING_EXT",
   "VK_CMD_BEGIN_CUSTOM_RESOLVE_EXT",
   "VK_CMD_RESET_QUERY_POOL",
   "VK_CMD_WRITE_TIMESTAMP",
   "VK_CMD_COPY_QUERY_POOL_RESULTS",
   "VK_CMD_PUSH_CONSTANTS",
   "VK_CMD_BEGIN_RENDER_PASS",
   "VK_CMD_NEXT_SUBPASS",
   "VK_CMD_END_RENDER_PASS",
   "VK_CMD_EXECUTE_COMMANDS",
   "VK_CMD_DEBUG_MARKER_BEGIN_EXT",
   "VK_CMD_DEBUG_MARKER_END_EXT",
   "VK_CMD_DEBUG_MARKER_INSERT_EXT",
   "VK_CMD_EXECUTE_GENERATED_COMMANDS_NV",
   "VK_CMD_PREPROCESS_GENERATED_COMMANDS_NV",
   "VK_CMD_BIND_PIPELINE_SHADER_GROUP_NV",
   "VK_CMD_EXECUTE_GENERATED_COMMANDS_EXT",
   "VK_CMD_PREPROCESS_GENERATED_COMMANDS_EXT",
   "VK_CMD_PUSH_DESCRIPTOR_SET",
   "VK_CMD_SET_DEVICE_MASK",
   "VK_CMD_DISPATCH_BASE",
   "VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE",
   "VK_CMD_SET_VIEWPORT_WSCALING_NV",
   "VK_CMD_SET_DISCARD_RECTANGLE_EXT",
   "VK_CMD_SET_DISCARD_RECTANGLE_ENABLE_EXT",
   "VK_CMD_SET_DISCARD_RECTANGLE_MODE_EXT",
   "VK_CMD_SET_SAMPLE_LOCATIONS_EXT",
   "VK_CMD_BEGIN_DEBUG_UTILS_LABEL_EXT",
   "VK_CMD_END_DEBUG_UTILS_LABEL_EXT",
   "VK_CMD_INSERT_DEBUG_UTILS_LABEL_EXT",
   "VK_CMD_WRITE_BUFFER_MARKER_AMD",
   "VK_CMD_BEGIN_RENDER_PASS2",
   "VK_CMD_NEXT_SUBPASS2",
   "VK_CMD_END_RENDER_PASS2",
   "VK_CMD_DRAW_INDIRECT_COUNT",
   "VK_CMD_DRAW_INDEXED_INDIRECT_COUNT",
   "VK_CMD_SET_CHECKPOINT_NV",
   "VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS_EXT",
   "VK_CMD_BEGIN_TRANSFORM_FEEDBACK_EXT",
   "VK_CMD_END_TRANSFORM_FEEDBACK_EXT",
   "VK_CMD_BEGIN_QUERY_INDEXED_EXT",
   "VK_CMD_END_QUERY_INDEXED_EXT",
   "VK_CMD_DRAW_INDIRECT_BYTE_COUNT_EXT",
   "VK_CMD_SET_EXCLUSIVE_SCISSOR_NV",
   "VK_CMD_SET_EXCLUSIVE_SCISSOR_ENABLE_NV",
   "VK_CMD_BIND_SHADING_RATE_IMAGE_NV",
   "VK_CMD_SET_VIEWPORT_SHADING_RATE_PALETTE_NV",
   "VK_CMD_SET_COARSE_SAMPLE_ORDER_NV",
   "VK_CMD_DRAW_MESH_TASKS_NV",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT_NV",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_NV",
   "VK_CMD_DRAW_MESH_TASKS_EXT",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT_EXT",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_EXT",
   "VK_CMD_BIND_INVOCATION_MASK_HUAWEI",
   "VK_CMD_COPY_ACCELERATION_STRUCTURE_NV",
   "VK_CMD_COPY_ACCELERATION_STRUCTURE_KHR",
   "VK_CMD_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_KHR",
   "VK_CMD_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_KHR",
   "VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_KHR",
   "VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_NV",
   "VK_CMD_BUILD_ACCELERATION_STRUCTURE_NV",
   "VK_CMD_TRACE_RAYS_KHR",
   "VK_CMD_TRACE_RAYS_NV",
   "VK_CMD_TRACE_RAYS_INDIRECT_KHR",
   "VK_CMD_TRACE_RAYS_INDIRECT2_KHR",
   "VK_CMD_BUILD_CLUSTER_ACCELERATION_STRUCTURE_INDIRECT_NV",
   "VK_CMD_SET_RAY_TRACING_PIPELINE_STACK_SIZE_KHR",
   "VK_CMD_SET_PERFORMANCE_MARKER_INTEL",
   "VK_CMD_SET_PERFORMANCE_STREAM_MARKER_INTEL",
   "VK_CMD_SET_PERFORMANCE_OVERRIDE_INTEL",
   "VK_CMD_SET_LINE_STIPPLE",
   "VK_CMD_BUILD_ACCELERATION_STRUCTURES_KHR",
   "VK_CMD_BUILD_ACCELERATION_STRUCTURES_INDIRECT_KHR",
   "VK_CMD_SET_CULL_MODE",
   "VK_CMD_SET_FRONT_FACE",
   "VK_CMD_SET_PRIMITIVE_TOPOLOGY",
   "VK_CMD_SET_VIEWPORT_WITH_COUNT",
   "VK_CMD_SET_SCISSOR_WITH_COUNT",
   "VK_CMD_BIND_INDEX_BUFFER2",
   "VK_CMD_BIND_VERTEX_BUFFERS2",
   "VK_CMD_SET_DEPTH_TEST_ENABLE",
   "VK_CMD_SET_DEPTH_WRITE_ENABLE",
   "VK_CMD_SET_DEPTH_COMPARE_OP",
   "VK_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE",
   "VK_CMD_SET_STENCIL_TEST_ENABLE",
   "VK_CMD_SET_STENCIL_OP",
   "VK_CMD_SET_PATCH_CONTROL_POINTS_EXT",
   "VK_CMD_SET_RASTERIZER_DISCARD_ENABLE",
   "VK_CMD_SET_DEPTH_BIAS_ENABLE",
   "VK_CMD_SET_LOGIC_OP_EXT",
   "VK_CMD_SET_PRIMITIVE_RESTART_ENABLE",
   "VK_CMD_SET_TESSELLATION_DOMAIN_ORIGIN_EXT",
   "VK_CMD_SET_DEPTH_CLAMP_ENABLE_EXT",
   "VK_CMD_SET_POLYGON_MODE_EXT",
   "VK_CMD_SET_RASTERIZATION_SAMPLES_EXT",
   "VK_CMD_SET_SAMPLE_MASK_EXT",
   "VK_CMD_SET_ALPHA_TO_COVERAGE_ENABLE_EXT",
   "VK_CMD_SET_ALPHA_TO_ONE_ENABLE_EXT",
   "VK_CMD_SET_LOGIC_OP_ENABLE_EXT",
   "VK_CMD_SET_COLOR_BLEND_ENABLE_EXT",
   "VK_CMD_SET_COLOR_BLEND_EQUATION_EXT",
   "VK_CMD_SET_COLOR_WRITE_MASK_EXT",
   "VK_CMD_SET_RASTERIZATION_STREAM_EXT",
   "VK_CMD_SET_CONSERVATIVE_RASTERIZATION_MODE_EXT",
   "VK_CMD_SET_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT",
   "VK_CMD_SET_DEPTH_CLIP_ENABLE_EXT",
   "VK_CMD_SET_SAMPLE_LOCATIONS_ENABLE_EXT",
   "VK_CMD_SET_COLOR_BLEND_ADVANCED_EXT",
   "VK_CMD_SET_PROVOKING_VERTEX_MODE_EXT",
   "VK_CMD_SET_LINE_RASTERIZATION_MODE_EXT",
   "VK_CMD_SET_LINE_STIPPLE_ENABLE_EXT",
   "VK_CMD_SET_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT",
   "VK_CMD_SET_VIEWPORT_WSCALING_ENABLE_NV",
   "VK_CMD_SET_VIEWPORT_SWIZZLE_NV",
   "VK_CMD_SET_COVERAGE_TO_COLOR_ENABLE_NV",
   "VK_CMD_SET_COVERAGE_TO_COLOR_LOCATION_NV",
   "VK_CMD_SET_COVERAGE_MODULATION_MODE_NV",
   "VK_CMD_SET_COVERAGE_MODULATION_TABLE_ENABLE_NV",
   "VK_CMD_SET_COVERAGE_MODULATION_TABLE_NV",
   "VK_CMD_SET_SHADING_RATE_IMAGE_ENABLE_NV",
   "VK_CMD_SET_COVERAGE_REDUCTION_MODE_NV",
   "VK_CMD_SET_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV",
   "VK_CMD_COPY_BUFFER2",
   "VK_CMD_COPY_IMAGE2",
   "VK_CMD_BLIT_IMAGE2",
   "VK_CMD_COPY_BUFFER_TO_IMAGE2",
   "VK_CMD_COPY_IMAGE_TO_BUFFER2",
   "VK_CMD_RESOLVE_IMAGE2",
   "VK_CMD_SET_FRAGMENT_SHADING_RATE_KHR",
   "VK_CMD_SET_FRAGMENT_SHADING_RATE_ENUM_NV",
   "VK_CMD_SET_VERTEX_INPUT_EXT",
   "VK_CMD_SET_COLOR_WRITE_ENABLE_EXT",
   "VK_CMD_SET_EVENT2",
   "VK_CMD_RESET_EVENT2",
   "VK_CMD_WAIT_EVENTS2",
   "VK_CMD_PIPELINE_BARRIER2",
   "VK_CMD_WRITE_TIMESTAMP2",
   "VK_CMD_WRITE_BUFFER_MARKER2_AMD",
   "VK_CMD_DECODE_VIDEO_KHR",
   "VK_CMD_BEGIN_VIDEO_CODING_KHR",
   "VK_CMD_CONTROL_VIDEO_CODING_KHR",
   "VK_CMD_END_VIDEO_CODING_KHR",
   "VK_CMD_ENCODE_VIDEO_KHR",
   "VK_CMD_DECOMPRESS_MEMORY_NV",
   "VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_NV",
   "VK_CMD_BUILD_PARTITIONED_ACCELERATION_STRUCTURES_NV",
   "VK_CMD_DECOMPRESS_MEMORY_EXT",
   "VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_EXT",
   "VK_CMD_CU_LAUNCH_KERNEL_NVX",
   "VK_CMD_BIND_DESCRIPTOR_BUFFERS_EXT",
   "VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS_EXT",
   "VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS_EXT",
   "VK_CMD_BEGIN_RENDERING",
   "VK_CMD_END_RENDERING",
   "VK_CMD_END_RENDERING2_KHR",
   "VK_CMD_BUILD_MICROMAPS_EXT",
   "VK_CMD_COPY_MICROMAP_EXT",
   "VK_CMD_COPY_MICROMAP_TO_MEMORY_EXT",
   "VK_CMD_COPY_MEMORY_TO_MICROMAP_EXT",
   "VK_CMD_WRITE_MICROMAPS_PROPERTIES_EXT",
   "VK_CMD_BIND_TILE_MEMORY_QCOM",
   "VK_CMD_OPTICAL_FLOW_EXECUTE_NV",
   "VK_CMD_SET_DEPTH_BIAS2_EXT",
   "VK_CMD_BIND_SHADERS_EXT",
   "VK_CMD_BEGIN_GPA_SESSION_AMD",
   "VK_CMD_END_GPA_SESSION_AMD",
   "VK_CMD_BEGIN_GPA_SAMPLE_AMD",
   "VK_CMD_END_GPA_SAMPLE_AMD",
   "VK_CMD_COPY_GPA_SESSION_RESULTS_AMD",
   "VK_CMD_BIND_DESCRIPTOR_SETS2",
   "VK_CMD_PUSH_CONSTANTS2",
   "VK_CMD_PUSH_DESCRIPTOR_SET2",
   "VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE2",
   "VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS2_EXT",
   "VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS2_EXT",
   "VK_CMD_SET_RENDERING_ATTACHMENT_LOCATIONS",
   "VK_CMD_SET_RENDERING_INPUT_ATTACHMENT_INDICES",
   "VK_CMD_SET_DEPTH_CLAMP_RANGE_EXT",
   "VK_CMD_CONVERT_COOPERATIVE_VECTOR_MATRIX_NV",
   "VK_CMD_DISPATCH_TILE_QCOM",
   "VK_CMD_BEGIN_PER_TILE_EXECUTION_QCOM",
   "VK_CMD_END_PER_TILE_EXECUTION_QCOM",
   "VK_CMD_BEGIN_SHADER_INSTRUMENTATION_ARM",
   "VK_CMD_END_SHADER_INSTRUMENTATION_ARM",
   "VK_CMD_COPY_TENSOR_ARM",
   "VK_CMD_DISPATCH_DATA_GRAPH_ARM",
   "VK_CMD_SET_COMPUTE_OCCUPANCY_PRIORITY_NV",
   "VK_CMD_BIND_SAMPLER_HEAP_EXT",
   "VK_CMD_BIND_RESOURCE_HEAP_EXT",
   "VK_CMD_PUSH_DATA_EXT",
   "VK_CMD_COPY_MEMORY_KHR",
   "VK_CMD_COPY_MEMORY_TO_IMAGE_KHR",
   "VK_CMD_COPY_IMAGE_TO_MEMORY_KHR",
   "VK_CMD_UPDATE_MEMORY_KHR",
   "VK_CMD_FILL_MEMORY_KHR",
   "VK_CMD_COPY_QUERY_POOL_RESULTS_TO_MEMORY_KHR",
   "VK_CMD_BEGIN_CONDITIONAL_RENDERING2_EXT",
   "VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS2_EXT",
   "VK_CMD_BEGIN_TRANSFORM_FEEDBACK2_EXT",
   "VK_CMD_END_TRANSFORM_FEEDBACK2_EXT",
   "VK_CMD_DRAW_INDIRECT_BYTE_COUNT2_EXT",
   "VK_CMD_WRITE_MARKER_TO_MEMORY_AMD",
   "VK_CMD_BIND_INDEX_BUFFER3_KHR",
   "VK_CMD_BIND_VERTEX_BUFFERS3_KHR",
   "VK_CMD_DRAW_INDIRECT2_KHR",
   "VK_CMD_DRAW_INDEXED_INDIRECT2_KHR",
   "VK_CMD_DRAW_INDIRECT_COUNT2_KHR",
   "VK_CMD_DRAW_INDEXED_INDIRECT_COUNT2_KHR",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT2_EXT",
   "VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT2_EXT",
   "VK_CMD_DISPATCH_INDIRECT2_KHR",
   "VK_CMD_SET_DISPATCH_PARAMETERS_ARM",
};

size_t vk_cmd_queue_type_sizes[] = {
   sizeof(struct vk_cmd_bind_pipeline) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_primitive_restart_index_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_attachment_feedback_loop_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_scissor) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_line_width) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_bias) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_blend_constants) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_bounds) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_stencil_compare_mask) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_stencil_write_mask) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_stencil_reference) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_descriptor_sets) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_index_buffer) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_vertex_buffers) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indexed) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_multi_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_multi_indexed_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indexed_indirect) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch_indirect) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_cluster_huawei) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_cluster_indirect_huawei) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_update_pipeline_indirect_buffer_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_buffer) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_blit_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_buffer_to_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_image_to_buffer) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_indirect_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_indirect_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_to_image_indirect_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_to_image_indirect_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_update_buffer) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_fill_buffer) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_clear_color_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_clear_depth_stencil_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_clear_attachments) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_resolve_image) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_event) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_reset_event) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_wait_events) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_pipeline_barrier) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_query) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_query) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_conditional_rendering_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_custom_resolve_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_reset_query_pool) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_timestamp) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_query_pool_results) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_constants) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_render_pass) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_next_subpass) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_execute_commands) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_debug_marker_begin_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_debug_marker_insert_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_execute_generated_commands_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_preprocess_generated_commands_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_pipeline_shader_group_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_execute_generated_commands_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_preprocess_generated_commands_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_descriptor_set) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_device_mask) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch_base) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_descriptor_set_with_template) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport_wscaling_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_discard_rectangle_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_discard_rectangle_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_discard_rectangle_mode_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_sample_locations_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_debug_utils_label_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_insert_debug_utils_label_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_buffer_marker_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_render_pass2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_next_subpass2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_render_pass2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect_count) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indexed_indirect_count) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_checkpoint_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_transform_feedback_buffers_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_transform_feedback_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_transform_feedback_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_query_indexed_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_query_indexed_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect_byte_count_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_exclusive_scissor_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_exclusive_scissor_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_shading_rate_image_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport_shading_rate_palette_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coarse_sample_order_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect_count_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect_count_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_invocation_mask_huawei) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_acceleration_structure_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_acceleration_structure_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_acceleration_structure_to_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_to_acceleration_structure_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_acceleration_structures_properties_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_acceleration_structures_properties_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_acceleration_structure_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_trace_rays_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_trace_rays_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_trace_rays_indirect_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_trace_rays_indirect2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_cluster_acceleration_structure_indirect_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_ray_tracing_pipeline_stack_size_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_performance_marker_intel) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_performance_stream_marker_intel) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_performance_override_intel) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_line_stipple) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_acceleration_structures_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_acceleration_structures_indirect_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_cull_mode) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_front_face) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_primitive_topology) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport_with_count) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_scissor_with_count) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_index_buffer2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_vertex_buffers2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_test_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_write_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_compare_op) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_bounds_test_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_stencil_test_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_stencil_op) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_patch_control_points_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_rasterizer_discard_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_bias_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_logic_op_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_primitive_restart_enable) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_tessellation_domain_origin_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_clamp_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_polygon_mode_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_rasterization_samples_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_sample_mask_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_alpha_to_coverage_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_alpha_to_one_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_logic_op_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_color_blend_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_color_blend_equation_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_color_write_mask_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_rasterization_stream_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_conservative_rasterization_mode_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_extra_primitive_overestimation_size_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_clip_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_sample_locations_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_color_blend_advanced_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_provoking_vertex_mode_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_line_rasterization_mode_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_line_stipple_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_clip_negative_one_to_one_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport_wscaling_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_viewport_swizzle_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_to_color_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_to_color_location_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_modulation_mode_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_modulation_table_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_modulation_table_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_shading_rate_image_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_coverage_reduction_mode_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_representative_fragment_test_enable_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_buffer2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_image2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_blit_image2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_buffer_to_image2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_image_to_buffer2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_resolve_image2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_fragment_shading_rate_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_fragment_shading_rate_enum_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_vertex_input_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_color_write_enable_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_event2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_reset_event2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_wait_events2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_pipeline_barrier2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_timestamp2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_buffer_marker2_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_decode_video_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_video_coding_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_control_video_coding_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_video_coding_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_encode_video_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_decompress_memory_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_decompress_memory_indirect_count_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_partitioned_acceleration_structures_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_decompress_memory_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_decompress_memory_indirect_count_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_cu_launch_kernel_nvx) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_descriptor_buffers_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_descriptor_buffer_offsets_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_descriptor_buffer_embedded_samplers_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_rendering) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_rendering2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_build_micromaps_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_micromap_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_micromap_to_memory_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_to_micromap_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_micromaps_properties_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_tile_memory_qcom) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_optical_flow_execute_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_bias2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_shaders_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_gpa_session_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_gpa_session_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_gpa_sample_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_gpa_sample_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_gpa_session_results_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_descriptor_sets2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_constants2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_descriptor_set2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_descriptor_set_with_template2) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_descriptor_buffer_offsets2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_descriptor_buffer_embedded_samplers2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_rendering_attachment_locations) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_rendering_input_attachment_indices) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_depth_clamp_range_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_convert_cooperative_vector_matrix_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch_tile_qcom) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_per_tile_execution_qcom) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_per_tile_execution_qcom) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_shader_instrumentation_arm) +
   offsetof(struct vk_cmd_queue_entry, u),
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_tensor_arm) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch_data_graph_arm) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_compute_occupancy_priority_nv) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_sampler_heap_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_resource_heap_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_push_data_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_memory_to_image_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_image_to_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_update_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_fill_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_copy_query_pool_results_to_memory_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_conditional_rendering2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_transform_feedback_buffers2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_begin_transform_feedback2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_end_transform_feedback2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect_byte_count2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_write_marker_to_memory_amd) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_index_buffer3_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_bind_vertex_buffers3_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indexed_indirect2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indirect_count2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_indexed_indirect_count2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_draw_mesh_tasks_indirect_count2_ext) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_dispatch_indirect2_khr) +
   offsetof(struct vk_cmd_queue_entry, u),
   sizeof(struct vk_cmd_set_dispatch_parameters_arm) +
   offsetof(struct vk_cmd_queue_entry, u),
};

/* From the application's perspective, the vk_cmd_queue_entry can outlive the
 * layout. Take a reference.
 */
static inline void
enqueue_pipeline_layout(struct vk_cmd_queue *queue, VkPipelineLayout layout)
{
   VK_FROM_HANDLE(vk_pipeline_layout, vklayout, layout);
   vk_pipeline_layout_ref(vklayout);
   util_dynarray_append(&queue->pipeline_layouts, vklayout);
}

static void
enqueue_descriptor_layout(struct vk_cmd_queue *queue, VkDescriptorSetLayout layout)
{
   VK_FROM_HANDLE(vk_descriptor_set_layout, vklayout, layout);
   vk_descriptor_set_layout_ref(vklayout);
   util_dynarray_append(&queue->set_layouts, vklayout);
}

static void
enqueue_descriptor_template(struct vk_cmd_queue *queue, VkDescriptorUpdateTemplate templ)
{
   VK_FROM_HANDLE(vk_descriptor_update_template, vktempl, templ);
   vk_descriptor_update_template_ref(vktempl);
   util_dynarray_append(&queue->update_templates, vktempl);
}

static void
enqueue_VkWriteDescriptorSet(struct vk_cmd_queue *queue, VkWriteDescriptorSet *dst, const VkWriteDescriptorSet *src)
{
   switch (dst->descriptorType) {
   case VK_DESCRIPTOR_TYPE_SAMPLER:
   case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
   case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
   case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
   case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      dst->pImageInfo = linear_alloc_child(queue->ctx, sizeof(VkDescriptorImageInfo) * dst->descriptorCount);
      memcpy((VkDescriptorImageInfo *)dst->pImageInfo,
             src->pImageInfo,
             sizeof(VkDescriptorImageInfo) * dst->descriptorCount);
      break;
   case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
   case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      dst->pTexelBufferView = linear_alloc_child(queue->ctx, sizeof(VkBufferView) * dst->descriptorCount);
      memcpy((VkBufferView *)dst->pTexelBufferView,
             src->pTexelBufferView,
             sizeof(VkBufferView) * dst->descriptorCount);
      break;
   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
      dst->pBufferInfo = linear_zalloc_child(queue->ctx, sizeof(VkDescriptorBufferInfo) * dst->descriptorCount);
      memcpy((VkDescriptorBufferInfo *)dst->pBufferInfo,
             src->pBufferInfo,
             sizeof(VkDescriptorBufferInfo) * dst->descriptorCount);
      break;
   default:
      break;
   }

}

static unsigned
vk_descriptor_type_update_size(VkDescriptorType type)
{
   switch (type) {
   case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
      UNREACHABLE("handled in caller");

   case VK_DESCRIPTOR_TYPE_SAMPLER:
   case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
   case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
   case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
   case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
      return sizeof(VkDescriptorImageInfo);

   case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
   case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      return sizeof(VkBufferView);

   case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
      return sizeof(VkAccelerationStructureKHR);

   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
   default:
      return sizeof(VkDescriptorBufferInfo);
   }
}

static void *
enqueue_push_descriptor_template_data(struct vk_cmd_queue *queue, VkDescriptorUpdateTemplate vktempl, const uint8_t *pData)
{

   /* What makes this tricky is that the size of pData is implicit. We determine
    * it by walking the template and determining the ranges read by the driver.
    */
   size_t data_size = 0;
   VK_FROM_HANDLE(vk_descriptor_update_template, templ,
                  vktempl);
   for (unsigned i = 0; i < templ->entry_count; ++i) {
      struct vk_descriptor_template_entry entry = templ->entries[i];
      unsigned end = 0;

      /* From the spec:
       *
       *    If descriptorType is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK then
       *    the value of stride is ignored and the stride is assumed to be 1,
       *    i.e. the descriptor update information for them is always specified
       *    as a contiguous range.
       */
      if (entry.type == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK) {
         end = entry.offset + entry.array_count;
      } else if (entry.array_count > 0) {
         end = entry.offset + ((entry.array_count - 1) * entry.stride) +
               vk_descriptor_type_update_size(entry.type);
      }

      data_size = MAX2(data_size, end);
   }

   uint8_t *out_pData = linear_alloc_child(queue->ctx, data_size);

   /* Now walk the template again, copying what we actually need */
   for (unsigned i = 0; i < templ->entry_count; ++i) {
      struct vk_descriptor_template_entry entry = templ->entries[i];
      unsigned size = 0;

      if (entry.type == VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK) {
         size = entry.array_count;
      } else if (entry.array_count > 0) {
         size = ((entry.array_count - 1) * entry.stride) +
                vk_descriptor_type_update_size(entry.type);
      }

      memcpy(out_pData + entry.offset, pData + entry.offset, size);
   }

   return out_pData;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_pipeline(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipeline pipeline
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_PIPELINE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_PIPELINE;
   cmd->u.bind_pipeline.pipeline_bind_point = pipelineBindPoint;
   cmd->u.bind_pipeline.pipeline = pipeline;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_restart_index_ext(struct vk_cmd_queue *queue
, uint32_t primitiveRestartIndex
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_PRIMITIVE_RESTART_INDEX_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_PRIMITIVE_RESTART_INDEX_EXT;
   cmd->u.set_primitive_restart_index_ext.primitive_restart_index = primitiveRestartIndex;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_attachment_feedback_loop_enable_ext(struct vk_cmd_queue *queue
, VkImageAspectFlags aspectMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT;
   cmd->u.set_attachment_feedback_loop_enable_ext.aspect_mask = aspectMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport(struct vk_cmd_queue *queue
, uint32_t firstViewport
, uint32_t viewportCount
, const VkViewport* pViewports
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT;
   cmd->u.set_viewport.first_viewport = firstViewport;
   cmd->u.set_viewport.viewport_count = viewportCount;
   if (pViewports) {
      cmd->u.set_viewport.viewports = linear_alloc_child(queue->ctx, sizeof(VkViewport) * ceil(viewportCount));
      if (cmd->u.set_viewport.viewports == NULL) return NULL;
      memcpy((void *)cmd->u.set_viewport.viewports, pViewports, sizeof(VkViewport) * ceil(viewportCount));
   } else {
      cmd->u.set_viewport.viewports = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_scissor(struct vk_cmd_queue *queue
, uint32_t firstScissor
, uint32_t scissorCount
, const VkRect2D* pScissors
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SCISSOR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SCISSOR;
   cmd->u.set_scissor.first_scissor = firstScissor;
   cmd->u.set_scissor.scissor_count = scissorCount;
   if (pScissors) {
      cmd->u.set_scissor.scissors = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(scissorCount));
      if (cmd->u.set_scissor.scissors == NULL) return NULL;
      memcpy((void *)cmd->u.set_scissor.scissors, pScissors, sizeof(VkRect2D) * ceil(scissorCount));
   } else {
      cmd->u.set_scissor.scissors = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_width(struct vk_cmd_queue *queue
, float lineWidth
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LINE_WIDTH]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LINE_WIDTH;
   cmd->u.set_line_width.line_width = lineWidth;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias(struct vk_cmd_queue *queue
, float depthBiasConstantFactor
, float depthBiasClamp
, float depthBiasSlopeFactor
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_BIAS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_BIAS;
   cmd->u.set_depth_bias.depth_bias_constant_factor = depthBiasConstantFactor;
   cmd->u.set_depth_bias.depth_bias_clamp = depthBiasClamp;
   cmd->u.set_depth_bias.depth_bias_slope_factor = depthBiasSlopeFactor;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_blend_constants(struct vk_cmd_queue *queue
, const float blendConstants[4]
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_BLEND_CONSTANTS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_BLEND_CONSTANTS;
   memcpy(cmd->u.set_blend_constants.blend_constants, blendConstants, sizeof(*blendConstants) * 4);

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bounds(struct vk_cmd_queue *queue
, float minDepthBounds
, float maxDepthBounds
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_BOUNDS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_BOUNDS;
   cmd->u.set_depth_bounds.min_depth_bounds = minDepthBounds;
   cmd->u.set_depth_bounds.max_depth_bounds = maxDepthBounds;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_compare_mask(struct vk_cmd_queue *queue
, VkStencilFaceFlags faceMask
, uint32_t compareMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_STENCIL_COMPARE_MASK]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_STENCIL_COMPARE_MASK;
   cmd->u.set_stencil_compare_mask.face_mask = faceMask;
   cmd->u.set_stencil_compare_mask.compare_mask = compareMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_write_mask(struct vk_cmd_queue *queue
, VkStencilFaceFlags faceMask
, uint32_t writeMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_STENCIL_WRITE_MASK]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_STENCIL_WRITE_MASK;
   cmd->u.set_stencil_write_mask.face_mask = faceMask;
   cmd->u.set_stencil_write_mask.write_mask = writeMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_reference(struct vk_cmd_queue *queue
, VkStencilFaceFlags faceMask
, uint32_t reference
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_STENCIL_REFERENCE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_STENCIL_REFERENCE;
   cmd->u.set_stencil_reference.face_mask = faceMask;
   cmd->u.set_stencil_reference.reference = reference;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_sets(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipelineLayout layout
, uint32_t firstSet
, uint32_t descriptorSetCount
, const VkDescriptorSet* pDescriptorSets
, uint32_t dynamicOffsetCount
, const uint32_t* pDynamicOffsets
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_DESCRIPTOR_SETS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_DESCRIPTOR_SETS;
   cmd->u.bind_descriptor_sets.pipeline_bind_point = pipelineBindPoint;
   cmd->u.bind_descriptor_sets.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.bind_descriptor_sets.first_set = firstSet;
   cmd->u.bind_descriptor_sets.descriptor_set_count = descriptorSetCount;
   if (pDescriptorSets) {
      cmd->u.bind_descriptor_sets.descriptor_sets = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSet) * ceil(descriptorSetCount));
      if (cmd->u.bind_descriptor_sets.descriptor_sets == NULL) return NULL;
      memcpy((void *)cmd->u.bind_descriptor_sets.descriptor_sets, pDescriptorSets, sizeof(VkDescriptorSet) * ceil(descriptorSetCount));
   } else {
      cmd->u.bind_descriptor_sets.descriptor_sets = NULL;
   }
   cmd->u.bind_descriptor_sets.dynamic_offset_count = dynamicOffsetCount;
   if (pDynamicOffsets) {
      cmd->u.bind_descriptor_sets.dynamic_offsets = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(dynamicOffsetCount));
      if (cmd->u.bind_descriptor_sets.dynamic_offsets == NULL) return NULL;
      memcpy((void *)cmd->u.bind_descriptor_sets.dynamic_offsets, pDynamicOffsets, sizeof(uint32_t) * ceil(dynamicOffsetCount));
   } else {
      cmd->u.bind_descriptor_sets.dynamic_offsets = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkIndexType indexType
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_INDEX_BUFFER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_INDEX_BUFFER;
   cmd->u.bind_index_buffer.buffer = buffer;
   cmd->u.bind_index_buffer.offset = offset;
   cmd->u.bind_index_buffer.index_type = indexType;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers(struct vk_cmd_queue *queue
, uint32_t firstBinding
, uint32_t bindingCount
, const VkBuffer* pBuffers
, const VkDeviceSize* pOffsets
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_VERTEX_BUFFERS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_VERTEX_BUFFERS;
   cmd->u.bind_vertex_buffers.first_binding = firstBinding;
   cmd->u.bind_vertex_buffers.binding_count = bindingCount;
   if (pBuffers) {
      cmd->u.bind_vertex_buffers.buffers = linear_alloc_child(queue->ctx, sizeof(VkBuffer) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers.buffers == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers.buffers, pBuffers, sizeof(VkBuffer) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers.buffers = NULL;
   }
   if (pOffsets) {
      cmd->u.bind_vertex_buffers.offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers.offsets == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers.offsets, pOffsets, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers.offsets = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw(struct vk_cmd_queue *queue
, uint32_t vertexCount
, uint32_t instanceCount
, uint32_t firstVertex
, uint32_t firstInstance
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW;
   cmd->u.draw.vertex_count = vertexCount;
   cmd->u.draw.instance_count = instanceCount;
   cmd->u.draw.first_vertex = firstVertex;
   cmd->u.draw.first_instance = firstInstance;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed(struct vk_cmd_queue *queue
, uint32_t indexCount
, uint32_t instanceCount
, uint32_t firstIndex
, int32_t vertexOffset
, uint32_t firstInstance
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDEXED]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDEXED;
   cmd->u.draw_indexed.index_count = indexCount;
   cmd->u.draw_indexed.instance_count = instanceCount;
   cmd->u.draw_indexed.first_index = firstIndex;
   cmd->u.draw_indexed.vertex_offset = vertexOffset;
   cmd->u.draw_indexed.first_instance = firstInstance;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}



struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, uint32_t drawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT;
   cmd->u.draw_indirect.buffer = buffer;
   cmd->u.draw_indirect.offset = offset;
   cmd->u.draw_indirect.draw_count = drawCount;
   cmd->u.draw_indirect.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, uint32_t drawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDEXED_INDIRECT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDEXED_INDIRECT;
   cmd->u.draw_indexed_indirect.buffer = buffer;
   cmd->u.draw_indexed_indirect.offset = offset;
   cmd->u.draw_indexed_indirect.draw_count = drawCount;
   cmd->u.draw_indexed_indirect.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch(struct vk_cmd_queue *queue
, uint32_t groupCountX
, uint32_t groupCountY
, uint32_t groupCountZ
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH;
   cmd->u.dispatch.group_count_x = groupCountX;
   cmd->u.dispatch.group_count_y = groupCountY;
   cmd->u.dispatch.group_count_z = groupCountZ;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_indirect(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH_INDIRECT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH_INDIRECT;
   cmd->u.dispatch_indirect.buffer = buffer;
   cmd->u.dispatch_indirect.offset = offset;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_subpass_shading_huawei(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SUBPASS_SHADING_HUAWEI]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SUBPASS_SHADING_HUAWEI;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_cluster_huawei(struct vk_cmd_queue *queue
, uint32_t groupCountX
, uint32_t groupCountY
, uint32_t groupCountZ
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_CLUSTER_HUAWEI]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_CLUSTER_HUAWEI;
   cmd->u.draw_cluster_huawei.group_count_x = groupCountX;
   cmd->u.draw_cluster_huawei.group_count_y = groupCountY;
   cmd->u.draw_cluster_huawei.group_count_z = groupCountZ;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_cluster_indirect_huawei(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_CLUSTER_INDIRECT_HUAWEI]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_CLUSTER_INDIRECT_HUAWEI;
   cmd->u.draw_cluster_indirect_huawei.buffer = buffer;
   cmd->u.draw_cluster_indirect_huawei.offset = offset;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_update_pipeline_indirect_buffer_nv(struct vk_cmd_queue *queue
, VkPipelineBindPoint           pipelineBindPoint
, VkPipeline                    pipeline
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_UPDATE_PIPELINE_INDIRECT_BUFFER_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_UPDATE_PIPELINE_INDIRECT_BUFFER_NV;
   cmd->u.update_pipeline_indirect_buffer_nv.pipeline_bind_point = pipelineBindPoint;
   cmd->u.update_pipeline_indirect_buffer_nv.pipeline = pipeline;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer(struct vk_cmd_queue *queue
, VkBuffer srcBuffer
, VkBuffer dstBuffer
, uint32_t regionCount
, const VkBufferCopy* pRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_BUFFER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_BUFFER;
   cmd->u.copy_buffer.src_buffer = srcBuffer;
   cmd->u.copy_buffer.dst_buffer = dstBuffer;
   cmd->u.copy_buffer.region_count = regionCount;
   if (pRegions) {
      cmd->u.copy_buffer.regions = linear_alloc_child(queue->ctx, sizeof(VkBufferCopy) * ceil(regionCount));
      if (cmd->u.copy_buffer.regions == NULL) return NULL;
      memcpy((void *)cmd->u.copy_buffer.regions, pRegions, sizeof(VkBufferCopy) * ceil(regionCount));
   } else {
      cmd->u.copy_buffer.regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image(struct vk_cmd_queue *queue
, VkImage srcImage
, VkImageLayout srcImageLayout
, VkImage dstImage
, VkImageLayout dstImageLayout
, uint32_t regionCount
, const VkImageCopy* pRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_IMAGE;
   cmd->u.copy_image.src_image = srcImage;
   cmd->u.copy_image.src_image_layout = srcImageLayout;
   cmd->u.copy_image.dst_image = dstImage;
   cmd->u.copy_image.dst_image_layout = dstImageLayout;
   cmd->u.copy_image.region_count = regionCount;
   if (pRegions) {
      cmd->u.copy_image.regions = linear_alloc_child(queue->ctx, sizeof(VkImageCopy) * ceil(regionCount));
      if (cmd->u.copy_image.regions == NULL) return NULL;
      memcpy((void *)cmd->u.copy_image.regions, pRegions, sizeof(VkImageCopy) * ceil(regionCount));
   } else {
      cmd->u.copy_image.regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_blit_image(struct vk_cmd_queue *queue
, VkImage srcImage
, VkImageLayout srcImageLayout
, VkImage dstImage
, VkImageLayout dstImageLayout
, uint32_t regionCount
, const VkImageBlit* pRegions
, VkFilter filter
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BLIT_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BLIT_IMAGE;
   cmd->u.blit_image.src_image = srcImage;
   cmd->u.blit_image.src_image_layout = srcImageLayout;
   cmd->u.blit_image.dst_image = dstImage;
   cmd->u.blit_image.dst_image_layout = dstImageLayout;
   cmd->u.blit_image.region_count = regionCount;
   if (pRegions) {
      cmd->u.blit_image.regions = linear_alloc_child(queue->ctx, sizeof(VkImageBlit) * ceil(regionCount));
      if (cmd->u.blit_image.regions == NULL) return NULL;
      memcpy((void *)cmd->u.blit_image.regions, pRegions, sizeof(VkImageBlit) * ceil(regionCount));
   } else {
      cmd->u.blit_image.regions = NULL;
   }
   cmd->u.blit_image.filter = filter;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer_to_image(struct vk_cmd_queue *queue
, VkBuffer srcBuffer
, VkImage dstImage
, VkImageLayout dstImageLayout
, uint32_t regionCount
, const VkBufferImageCopy* pRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_BUFFER_TO_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_BUFFER_TO_IMAGE;
   cmd->u.copy_buffer_to_image.src_buffer = srcBuffer;
   cmd->u.copy_buffer_to_image.dst_image = dstImage;
   cmd->u.copy_buffer_to_image.dst_image_layout = dstImageLayout;
   cmd->u.copy_buffer_to_image.region_count = regionCount;
   if (pRegions) {
      cmd->u.copy_buffer_to_image.regions = linear_alloc_child(queue->ctx, sizeof(VkBufferImageCopy) * ceil(regionCount));
      if (cmd->u.copy_buffer_to_image.regions == NULL) return NULL;
      memcpy((void *)cmd->u.copy_buffer_to_image.regions, pRegions, sizeof(VkBufferImageCopy) * ceil(regionCount));
   } else {
      cmd->u.copy_buffer_to_image.regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_buffer(struct vk_cmd_queue *queue
, VkImage srcImage
, VkImageLayout srcImageLayout
, VkBuffer dstBuffer
, uint32_t regionCount
, const VkBufferImageCopy* pRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_IMAGE_TO_BUFFER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_IMAGE_TO_BUFFER;
   cmd->u.copy_image_to_buffer.src_image = srcImage;
   cmd->u.copy_image_to_buffer.src_image_layout = srcImageLayout;
   cmd->u.copy_image_to_buffer.dst_buffer = dstBuffer;
   cmd->u.copy_image_to_buffer.region_count = regionCount;
   if (pRegions) {
      cmd->u.copy_image_to_buffer.regions = linear_alloc_child(queue->ctx, sizeof(VkBufferImageCopy) * ceil(regionCount));
      if (cmd->u.copy_image_to_buffer.regions == NULL) return NULL;
      memcpy((void *)cmd->u.copy_image_to_buffer.regions, pRegions, sizeof(VkBufferImageCopy) * ceil(regionCount));
   } else {
      cmd->u.copy_image_to_buffer.regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_indirect_nv(struct vk_cmd_queue *queue
, VkDeviceAddress copyBufferAddress
, uint32_t copyCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_INDIRECT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_INDIRECT_NV;
   cmd->u.copy_memory_indirect_nv.copy_buffer_address = copyBufferAddress;
   cmd->u.copy_memory_indirect_nv.copy_count = copyCount;
   cmd->u.copy_memory_indirect_nv.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_indirect_khr(struct vk_cmd_queue *queue
, const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_INDIRECT_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_INDIRECT_KHR;
   if (pCopyMemoryIndirectInfo) {
      cmd->u.copy_memory_indirect_khr.copy_memory_indirect_info = linear_alloc_child(queue->ctx, sizeof(VkCopyMemoryIndirectInfoKHR));
      if (cmd->u.copy_memory_indirect_khr.copy_memory_indirect_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_indirect_khr.copy_memory_indirect_info, pCopyMemoryIndirectInfo, sizeof(VkCopyMemoryIndirectInfoKHR));
   } else {
      cmd->u.copy_memory_indirect_khr.copy_memory_indirect_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_indirect_nv(struct vk_cmd_queue *queue
, VkDeviceAddress copyBufferAddress
, uint32_t copyCount
, uint32_t stride
, VkImage dstImage
, VkImageLayout dstImageLayout
, const VkImageSubresourceLayers* pImageSubresources
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_NV;
   cmd->u.copy_memory_to_image_indirect_nv.copy_buffer_address = copyBufferAddress;
   cmd->u.copy_memory_to_image_indirect_nv.copy_count = copyCount;
   cmd->u.copy_memory_to_image_indirect_nv.stride = stride;
   cmd->u.copy_memory_to_image_indirect_nv.dst_image = dstImage;
   cmd->u.copy_memory_to_image_indirect_nv.dst_image_layout = dstImageLayout;
   if (pImageSubresources) {
      cmd->u.copy_memory_to_image_indirect_nv.image_subresources = linear_alloc_child(queue->ctx, sizeof(VkImageSubresourceLayers) * ceil(copyCount));
      if (cmd->u.copy_memory_to_image_indirect_nv.image_subresources == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_to_image_indirect_nv.image_subresources, pImageSubresources, sizeof(VkImageSubresourceLayers) * ceil(copyCount));
   } else {
      cmd->u.copy_memory_to_image_indirect_nv.image_subresources = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_indirect_khr(struct vk_cmd_queue *queue
, const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_KHR;
   if (pCopyMemoryToImageIndirectInfo) {
      cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info = linear_alloc_child(queue->ctx, sizeof(VkCopyMemoryToImageIndirectInfoKHR));
      if (cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info, pCopyMemoryToImageIndirectInfo, sizeof(VkCopyMemoryToImageIndirectInfoKHR));
      VkCopyMemoryToImageIndirectInfoKHR *tmp_dst1 = (void *)cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info;
      VkCopyMemoryToImageIndirectInfoKHR *tmp_src2 = (void *)pCopyMemoryToImageIndirectInfo;
      if (tmp_src2->pImageSubresources) {
         tmp_dst1->pImageSubresources = linear_alloc_child(queue->ctx, sizeof(VkImageSubresourceLayers) * ceil(tmp_src2->copyCount));
         if (tmp_dst1->pImageSubresources == NULL) return NULL;
         memcpy((void *)tmp_dst1->pImageSubresources, tmp_src2->pImageSubresources, sizeof(VkImageSubresourceLayers) * ceil(tmp_src2->copyCount));
      }
   } else {
      cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_update_buffer(struct vk_cmd_queue *queue
, VkBuffer dstBuffer
, VkDeviceSize dstOffset
, VkDeviceSize dataSize
, const void* pData
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_UPDATE_BUFFER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_UPDATE_BUFFER;
   cmd->u.update_buffer.dst_buffer = dstBuffer;
   cmd->u.update_buffer.dst_offset = dstOffset;
   cmd->u.update_buffer.data_size = dataSize;
   if (pData) {
      cmd->u.update_buffer.data = linear_alloc_child(queue->ctx, 1 * ceil(dataSize));
      if (cmd->u.update_buffer.data == NULL) return NULL;
      memcpy((void *)cmd->u.update_buffer.data, pData, 1 * ceil(dataSize));
   } else {
      cmd->u.update_buffer.data = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_fill_buffer(struct vk_cmd_queue *queue
, VkBuffer dstBuffer
, VkDeviceSize dstOffset
, VkDeviceSize size
, uint32_t data
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_FILL_BUFFER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_FILL_BUFFER;
   cmd->u.fill_buffer.dst_buffer = dstBuffer;
   cmd->u.fill_buffer.dst_offset = dstOffset;
   cmd->u.fill_buffer.size = size;
   cmd->u.fill_buffer.data = data;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_color_image(struct vk_cmd_queue *queue
, VkImage image
, VkImageLayout imageLayout
, const VkClearColorValue* pColor
, uint32_t rangeCount
, const VkImageSubresourceRange* pRanges
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CLEAR_COLOR_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CLEAR_COLOR_IMAGE;
   cmd->u.clear_color_image.image = image;
   cmd->u.clear_color_image.image_layout = imageLayout;
   if (pColor) {
      cmd->u.clear_color_image.color = linear_alloc_child(queue->ctx, sizeof(VkClearColorValue));
      if (cmd->u.clear_color_image.color == NULL) return NULL;
      memcpy((void *)cmd->u.clear_color_image.color, pColor, sizeof(VkClearColorValue));
   } else {
      cmd->u.clear_color_image.color = NULL;
   }
   cmd->u.clear_color_image.range_count = rangeCount;
   if (pRanges) {
      cmd->u.clear_color_image.ranges = linear_alloc_child(queue->ctx, sizeof(VkImageSubresourceRange) * ceil(rangeCount));
      if (cmd->u.clear_color_image.ranges == NULL) return NULL;
      memcpy((void *)cmd->u.clear_color_image.ranges, pRanges, sizeof(VkImageSubresourceRange) * ceil(rangeCount));
   } else {
      cmd->u.clear_color_image.ranges = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_depth_stencil_image(struct vk_cmd_queue *queue
, VkImage image
, VkImageLayout imageLayout
, const VkClearDepthStencilValue* pDepthStencil
, uint32_t rangeCount
, const VkImageSubresourceRange* pRanges
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CLEAR_DEPTH_STENCIL_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CLEAR_DEPTH_STENCIL_IMAGE;
   cmd->u.clear_depth_stencil_image.image = image;
   cmd->u.clear_depth_stencil_image.image_layout = imageLayout;
   if (pDepthStencil) {
      cmd->u.clear_depth_stencil_image.depth_stencil = linear_alloc_child(queue->ctx, sizeof(VkClearDepthStencilValue));
      if (cmd->u.clear_depth_stencil_image.depth_stencil == NULL) return NULL;
      memcpy((void *)cmd->u.clear_depth_stencil_image.depth_stencil, pDepthStencil, sizeof(VkClearDepthStencilValue));
   } else {
      cmd->u.clear_depth_stencil_image.depth_stencil = NULL;
   }
   cmd->u.clear_depth_stencil_image.range_count = rangeCount;
   if (pRanges) {
      cmd->u.clear_depth_stencil_image.ranges = linear_alloc_child(queue->ctx, sizeof(VkImageSubresourceRange) * ceil(rangeCount));
      if (cmd->u.clear_depth_stencil_image.ranges == NULL) return NULL;
      memcpy((void *)cmd->u.clear_depth_stencil_image.ranges, pRanges, sizeof(VkImageSubresourceRange) * ceil(rangeCount));
   } else {
      cmd->u.clear_depth_stencil_image.ranges = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_attachments(struct vk_cmd_queue *queue
, uint32_t attachmentCount
, const VkClearAttachment* pAttachments
, uint32_t rectCount
, const VkClearRect* pRects
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CLEAR_ATTACHMENTS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CLEAR_ATTACHMENTS;
   cmd->u.clear_attachments.attachment_count = attachmentCount;
   if (pAttachments) {
      cmd->u.clear_attachments.attachments = linear_alloc_child(queue->ctx, sizeof(VkClearAttachment) * ceil(attachmentCount));
      if (cmd->u.clear_attachments.attachments == NULL) return NULL;
      memcpy((void *)cmd->u.clear_attachments.attachments, pAttachments, sizeof(VkClearAttachment) * ceil(attachmentCount));
   } else {
      cmd->u.clear_attachments.attachments = NULL;
   }
   cmd->u.clear_attachments.rect_count = rectCount;
   if (pRects) {
      cmd->u.clear_attachments.rects = linear_alloc_child(queue->ctx, sizeof(VkClearRect) * ceil(rectCount));
      if (cmd->u.clear_attachments.rects == NULL) return NULL;
      memcpy((void *)cmd->u.clear_attachments.rects, pRects, sizeof(VkClearRect) * ceil(rectCount));
   } else {
      cmd->u.clear_attachments.rects = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_resolve_image(struct vk_cmd_queue *queue
, VkImage srcImage
, VkImageLayout srcImageLayout
, VkImage dstImage
, VkImageLayout dstImageLayout
, uint32_t regionCount
, const VkImageResolve* pRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_RESOLVE_IMAGE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_RESOLVE_IMAGE;
   cmd->u.resolve_image.src_image = srcImage;
   cmd->u.resolve_image.src_image_layout = srcImageLayout;
   cmd->u.resolve_image.dst_image = dstImage;
   cmd->u.resolve_image.dst_image_layout = dstImageLayout;
   cmd->u.resolve_image.region_count = regionCount;
   if (pRegions) {
      cmd->u.resolve_image.regions = linear_alloc_child(queue->ctx, sizeof(VkImageResolve) * ceil(regionCount));
      if (cmd->u.resolve_image.regions == NULL) return NULL;
      memcpy((void *)cmd->u.resolve_image.regions, pRegions, sizeof(VkImageResolve) * ceil(regionCount));
   } else {
      cmd->u.resolve_image.regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_event(struct vk_cmd_queue *queue
, VkEvent event
, VkPipelineStageFlags stageMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_EVENT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_EVENT;
   cmd->u.set_event.event = event;
   cmd->u.set_event.stage_mask = stageMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_event(struct vk_cmd_queue *queue
, VkEvent event
, VkPipelineStageFlags stageMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_RESET_EVENT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_RESET_EVENT;
   cmd->u.reset_event.event = event;
   cmd->u.reset_event.stage_mask = stageMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_wait_events(struct vk_cmd_queue *queue
, uint32_t eventCount
, const VkEvent* pEvents
, VkPipelineStageFlags srcStageMask
, VkPipelineStageFlags dstStageMask
, uint32_t memoryBarrierCount
, const VkMemoryBarrier* pMemoryBarriers
, uint32_t bufferMemoryBarrierCount
, const VkBufferMemoryBarrier* pBufferMemoryBarriers
, uint32_t imageMemoryBarrierCount
, const VkImageMemoryBarrier* pImageMemoryBarriers
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WAIT_EVENTS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WAIT_EVENTS;
   cmd->u.wait_events.event_count = eventCount;
   if (pEvents) {
      cmd->u.wait_events.events = linear_alloc_child(queue->ctx, sizeof(VkEvent) * ceil(eventCount));
      if (cmd->u.wait_events.events == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events.events, pEvents, sizeof(VkEvent) * ceil(eventCount));
   } else {
      cmd->u.wait_events.events = NULL;
   }
   cmd->u.wait_events.src_stage_mask = srcStageMask;
   cmd->u.wait_events.dst_stage_mask = dstStageMask;
   cmd->u.wait_events.memory_barrier_count = memoryBarrierCount;
   if (pMemoryBarriers) {
      cmd->u.wait_events.memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrier) * ceil(memoryBarrierCount));
      if (cmd->u.wait_events.memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events.memory_barriers, pMemoryBarriers, sizeof(VkMemoryBarrier) * ceil(memoryBarrierCount));
   } else {
      cmd->u.wait_events.memory_barriers = NULL;
   }
   cmd->u.wait_events.buffer_memory_barrier_count = bufferMemoryBarrierCount;
   if (pBufferMemoryBarriers) {
      cmd->u.wait_events.buffer_memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkBufferMemoryBarrier) * ceil(bufferMemoryBarrierCount));
      if (cmd->u.wait_events.buffer_memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events.buffer_memory_barriers, pBufferMemoryBarriers, sizeof(VkBufferMemoryBarrier) * ceil(bufferMemoryBarrierCount));
      VkBufferMemoryBarrier *tmp_dst1 = (void *)cmd->u.wait_events.buffer_memory_barriers;
      VkBufferMemoryBarrier *tmp_src2 = (void *)pBufferMemoryBarriers;
      for (uint32_t i3 = 0; i3 < bufferMemoryBarrierCount; i3++) {
         VkBufferMemoryBarrier *tmp_dst4 = tmp_dst1 + i3;
         VkBufferMemoryBarrier *tmp_src5 = tmp_src2 + i3;
         const VkBaseInStructure *pnext = tmp_src5->pNext;
         void **dst_pnext_link = (void **)&tmp_dst4->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.wait_events.buffer_memory_barriers = NULL;
   }
   cmd->u.wait_events.image_memory_barrier_count = imageMemoryBarrierCount;
   if (pImageMemoryBarriers) {
      cmd->u.wait_events.image_memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkImageMemoryBarrier) * ceil(imageMemoryBarrierCount));
      if (cmd->u.wait_events.image_memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events.image_memory_barriers, pImageMemoryBarriers, sizeof(VkImageMemoryBarrier) * ceil(imageMemoryBarrierCount));
      VkImageMemoryBarrier *tmp_dst6 = (void *)cmd->u.wait_events.image_memory_barriers;
      VkImageMemoryBarrier *tmp_src7 = (void *)pImageMemoryBarriers;
      for (uint32_t i8 = 0; i8 < imageMemoryBarrierCount; i8++) {
         VkImageMemoryBarrier *tmp_dst9 = tmp_dst6 + i8;
         VkImageMemoryBarrier *tmp_src10 = tmp_src7 + i8;
         const VkBaseInStructure *pnext = tmp_src10->pNext;
         void **dst_pnext_link = (void **)&tmp_dst9->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
               VkSampleLocationsInfoEXT *tmp_dst11 = (void *)(*dst_pnext_link);
               VkSampleLocationsInfoEXT *tmp_src12 = (void *)pnext;
               if (tmp_src12->pSampleLocations) {
                  tmp_dst11->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
                  if (tmp_dst11->pSampleLocations == NULL) return NULL;
                  memcpy((void *)tmp_dst11->pSampleLocations, tmp_src12->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
               }
               break;
            case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.wait_events.image_memory_barriers = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_pipeline_barrier(struct vk_cmd_queue *queue
, VkPipelineStageFlags srcStageMask
, VkPipelineStageFlags dstStageMask
, VkDependencyFlags dependencyFlags
, uint32_t memoryBarrierCount
, const VkMemoryBarrier* pMemoryBarriers
, uint32_t bufferMemoryBarrierCount
, const VkBufferMemoryBarrier* pBufferMemoryBarriers
, uint32_t imageMemoryBarrierCount
, const VkImageMemoryBarrier* pImageMemoryBarriers
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PIPELINE_BARRIER]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PIPELINE_BARRIER;
   cmd->u.pipeline_barrier.src_stage_mask = srcStageMask;
   cmd->u.pipeline_barrier.dst_stage_mask = dstStageMask;
   cmd->u.pipeline_barrier.dependency_flags = dependencyFlags;
   cmd->u.pipeline_barrier.memory_barrier_count = memoryBarrierCount;
   if (pMemoryBarriers) {
      cmd->u.pipeline_barrier.memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrier) * ceil(memoryBarrierCount));
      if (cmd->u.pipeline_barrier.memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.pipeline_barrier.memory_barriers, pMemoryBarriers, sizeof(VkMemoryBarrier) * ceil(memoryBarrierCount));
   } else {
      cmd->u.pipeline_barrier.memory_barriers = NULL;
   }
   cmd->u.pipeline_barrier.buffer_memory_barrier_count = bufferMemoryBarrierCount;
   if (pBufferMemoryBarriers) {
      cmd->u.pipeline_barrier.buffer_memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkBufferMemoryBarrier) * ceil(bufferMemoryBarrierCount));
      if (cmd->u.pipeline_barrier.buffer_memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.pipeline_barrier.buffer_memory_barriers, pBufferMemoryBarriers, sizeof(VkBufferMemoryBarrier) * ceil(bufferMemoryBarrierCount));
      VkBufferMemoryBarrier *tmp_dst1 = (void *)cmd->u.pipeline_barrier.buffer_memory_barriers;
      VkBufferMemoryBarrier *tmp_src2 = (void *)pBufferMemoryBarriers;
      for (uint32_t i3 = 0; i3 < bufferMemoryBarrierCount; i3++) {
         VkBufferMemoryBarrier *tmp_dst4 = tmp_dst1 + i3;
         VkBufferMemoryBarrier *tmp_src5 = tmp_src2 + i3;
         const VkBaseInStructure *pnext = tmp_src5->pNext;
         void **dst_pnext_link = (void **)&tmp_dst4->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.pipeline_barrier.buffer_memory_barriers = NULL;
   }
   cmd->u.pipeline_barrier.image_memory_barrier_count = imageMemoryBarrierCount;
   if (pImageMemoryBarriers) {
      cmd->u.pipeline_barrier.image_memory_barriers = linear_alloc_child(queue->ctx, sizeof(VkImageMemoryBarrier) * ceil(imageMemoryBarrierCount));
      if (cmd->u.pipeline_barrier.image_memory_barriers == NULL) return NULL;
      memcpy((void *)cmd->u.pipeline_barrier.image_memory_barriers, pImageMemoryBarriers, sizeof(VkImageMemoryBarrier) * ceil(imageMemoryBarrierCount));
      VkImageMemoryBarrier *tmp_dst6 = (void *)cmd->u.pipeline_barrier.image_memory_barriers;
      VkImageMemoryBarrier *tmp_src7 = (void *)pImageMemoryBarriers;
      for (uint32_t i8 = 0; i8 < imageMemoryBarrierCount; i8++) {
         VkImageMemoryBarrier *tmp_dst9 = tmp_dst6 + i8;
         VkImageMemoryBarrier *tmp_src10 = tmp_src7 + i8;
         const VkBaseInStructure *pnext = tmp_src10->pNext;
         void **dst_pnext_link = (void **)&tmp_dst9->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
               VkSampleLocationsInfoEXT *tmp_dst11 = (void *)(*dst_pnext_link);
               VkSampleLocationsInfoEXT *tmp_src12 = (void *)pnext;
               if (tmp_src12->pSampleLocations) {
                  tmp_dst11->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
                  if (tmp_dst11->pSampleLocations == NULL) return NULL;
                  memcpy((void *)tmp_dst11->pSampleLocations, tmp_src12->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
               }
               break;
            case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.pipeline_barrier.image_memory_barriers = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_query(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t query
, VkQueryControlFlags flags
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_QUERY]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_QUERY;
   cmd->u.begin_query.query_pool = queryPool;
   cmd->u.begin_query.query = query;
   cmd->u.begin_query.flags = flags;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_query(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t query
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_QUERY]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_QUERY;
   cmd->u.end_query.query_pool = queryPool;
   cmd->u.end_query.query = query;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_conditional_rendering_ext(struct vk_cmd_queue *queue
, const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_CONDITIONAL_RENDERING_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_CONDITIONAL_RENDERING_EXT;
   if (pConditionalRenderingBegin) {
      cmd->u.begin_conditional_rendering_ext.conditional_rendering_begin = linear_alloc_child(queue->ctx, sizeof(VkConditionalRenderingBeginInfoEXT));
      if (cmd->u.begin_conditional_rendering_ext.conditional_rendering_begin == NULL) return NULL;
      memcpy((void *)cmd->u.begin_conditional_rendering_ext.conditional_rendering_begin, pConditionalRenderingBegin, sizeof(VkConditionalRenderingBeginInfoEXT));
   } else {
      cmd->u.begin_conditional_rendering_ext.conditional_rendering_begin = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_conditional_rendering_ext(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_CONDITIONAL_RENDERING_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_CONDITIONAL_RENDERING_EXT;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_custom_resolve_ext(struct vk_cmd_queue *queue
, const VkBeginCustomResolveInfoEXT* pBeginCustomResolveInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_CUSTOM_RESOLVE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_CUSTOM_RESOLVE_EXT;
   if (pBeginCustomResolveInfo) {
      cmd->u.begin_custom_resolve_ext.begin_custom_resolve_info = linear_alloc_child(queue->ctx, sizeof(VkBeginCustomResolveInfoEXT));
      if (cmd->u.begin_custom_resolve_ext.begin_custom_resolve_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_custom_resolve_ext.begin_custom_resolve_info, pBeginCustomResolveInfo, sizeof(VkBeginCustomResolveInfoEXT));
   } else {
      cmd->u.begin_custom_resolve_ext.begin_custom_resolve_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_query_pool(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t firstQuery
, uint32_t queryCount
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_RESET_QUERY_POOL]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_RESET_QUERY_POOL;
   cmd->u.reset_query_pool.query_pool = queryPool;
   cmd->u.reset_query_pool.first_query = firstQuery;
   cmd->u.reset_query_pool.query_count = queryCount;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_timestamp(struct vk_cmd_queue *queue
, VkPipelineStageFlagBits pipelineStage
, VkQueryPool queryPool
, uint32_t query
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_TIMESTAMP]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_TIMESTAMP;
   cmd->u.write_timestamp.pipeline_stage = pipelineStage;
   cmd->u.write_timestamp.query_pool = queryPool;
   cmd->u.write_timestamp.query = query;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_query_pool_results(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t firstQuery
, uint32_t queryCount
, VkBuffer dstBuffer
, VkDeviceSize dstOffset
, VkDeviceSize stride
, VkQueryResultFlags flags
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_QUERY_POOL_RESULTS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_QUERY_POOL_RESULTS;
   cmd->u.copy_query_pool_results.query_pool = queryPool;
   cmd->u.copy_query_pool_results.first_query = firstQuery;
   cmd->u.copy_query_pool_results.query_count = queryCount;
   cmd->u.copy_query_pool_results.dst_buffer = dstBuffer;
   cmd->u.copy_query_pool_results.dst_offset = dstOffset;
   cmd->u.copy_query_pool_results.stride = stride;
   cmd->u.copy_query_pool_results.flags = flags;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_constants(struct vk_cmd_queue *queue
, VkPipelineLayout layout
, VkShaderStageFlags stageFlags
, uint32_t offset
, uint32_t size
, const void* pValues
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_CONSTANTS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_CONSTANTS;
   cmd->u.push_constants.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.push_constants.stage_flags = stageFlags;
   cmd->u.push_constants.offset = offset;
   cmd->u.push_constants.size = size;
   if (pValues) {
      cmd->u.push_constants.values = linear_alloc_child(queue->ctx, 1 * ceil(size));
      if (cmd->u.push_constants.values == NULL) return NULL;
      memcpy((void *)cmd->u.push_constants.values, pValues, 1 * ceil(size));
   } else {
      cmd->u.push_constants.values = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_render_pass(struct vk_cmd_queue *queue
, const VkRenderPassBeginInfo* pRenderPassBegin
, VkSubpassContents contents
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_RENDER_PASS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_RENDER_PASS;
   if (pRenderPassBegin) {
      cmd->u.begin_render_pass.render_pass_begin = linear_alloc_child(queue->ctx, sizeof(VkRenderPassBeginInfo));
      if (cmd->u.begin_render_pass.render_pass_begin == NULL) return NULL;
      memcpy((void *)cmd->u.begin_render_pass.render_pass_begin, pRenderPassBegin, sizeof(VkRenderPassBeginInfo));
      VkRenderPassBeginInfo *tmp_dst1 = (void *)cmd->u.begin_render_pass.render_pass_begin;
      VkRenderPassBeginInfo *tmp_src2 = (void *)pRenderPassBegin;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDeviceGroupRenderPassBeginInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDeviceGroupRenderPassBeginInfo));
            VkDeviceGroupRenderPassBeginInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkDeviceGroupRenderPassBeginInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pDeviceRenderAreas) {
               tmp_dst3->pDeviceRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
               if (tmp_dst3->pDeviceRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst3->pDeviceRenderAreas, tmp_src4->pDeviceRenderAreas, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_SAMPLE_LOCATIONS_BEGIN_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassSampleLocationsBeginInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassSampleLocationsBeginInfoEXT));
            VkRenderPassSampleLocationsBeginInfoEXT *tmp_dst5 = (void *)(*dst_pnext_link);
            VkRenderPassSampleLocationsBeginInfoEXT *tmp_src6 = (void *)pnext;
            if (tmp_src6->pAttachmentInitialSampleLocations) {
               tmp_dst5->pAttachmentInitialSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkAttachmentSampleLocationsEXT) * ceil(tmp_src6->attachmentInitialSampleLocationsCount));
               if (tmp_dst5->pAttachmentInitialSampleLocations == NULL) return NULL;
               memcpy((void *)tmp_dst5->pAttachmentInitialSampleLocations, tmp_src6->pAttachmentInitialSampleLocations, sizeof(VkAttachmentSampleLocationsEXT) * ceil(tmp_src6->attachmentInitialSampleLocationsCount));
               VkAttachmentSampleLocationsEXT *tmp_dst7 = (void *)tmp_dst5->pAttachmentInitialSampleLocations;
               VkAttachmentSampleLocationsEXT *tmp_src8 = (void *)tmp_src6->pAttachmentInitialSampleLocations;
               for (uint32_t i9 = 0; i9 < tmp_src6->attachmentInitialSampleLocationsCount; i9++) {
                  VkAttachmentSampleLocationsEXT *tmp_dst10 = tmp_dst7 + i9;
                  VkAttachmentSampleLocationsEXT *tmp_src11 = tmp_src8 + i9;
                  VkSampleLocationsInfoEXT *tmp_dst12 = (void *)&tmp_dst10->sampleLocationsInfo;
                  VkSampleLocationsInfoEXT *tmp_src13 = (void *)&tmp_src11->sampleLocationsInfo;
                  if (tmp_src13->pSampleLocations) {
                     tmp_dst12->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src13->sampleLocationsCount));
                     if (tmp_dst12->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst12->pSampleLocations, tmp_src13->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src13->sampleLocationsCount));
                  }
               }
            }
            if (tmp_src6->pPostSubpassSampleLocations) {
               tmp_dst5->pPostSubpassSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSubpassSampleLocationsEXT) * ceil(tmp_src6->postSubpassSampleLocationsCount));
               if (tmp_dst5->pPostSubpassSampleLocations == NULL) return NULL;
               memcpy((void *)tmp_dst5->pPostSubpassSampleLocations, tmp_src6->pPostSubpassSampleLocations, sizeof(VkSubpassSampleLocationsEXT) * ceil(tmp_src6->postSubpassSampleLocationsCount));
               VkSubpassSampleLocationsEXT *tmp_dst14 = (void *)tmp_dst5->pPostSubpassSampleLocations;
               VkSubpassSampleLocationsEXT *tmp_src15 = (void *)tmp_src6->pPostSubpassSampleLocations;
               for (uint32_t i16 = 0; i16 < tmp_src6->postSubpassSampleLocationsCount; i16++) {
                  VkSubpassSampleLocationsEXT *tmp_dst17 = tmp_dst14 + i16;
                  VkSubpassSampleLocationsEXT *tmp_src18 = tmp_src15 + i16;
                  VkSampleLocationsInfoEXT *tmp_dst19 = (void *)&tmp_dst17->sampleLocationsInfo;
                  VkSampleLocationsInfoEXT *tmp_src20 = (void *)&tmp_src18->sampleLocationsInfo;
                  if (tmp_src20->pSampleLocations) {
                     tmp_dst19->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src20->sampleLocationsCount));
                     if (tmp_dst19->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst19->pSampleLocations, tmp_src20->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src20->sampleLocationsCount));
                  }
               }
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassAttachmentBeginInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassAttachmentBeginInfo));
            VkRenderPassAttachmentBeginInfo *tmp_dst21 = (void *)(*dst_pnext_link);
            VkRenderPassAttachmentBeginInfo *tmp_src22 = (void *)pnext;
            if (tmp_src22->pAttachments) {
               tmp_dst21->pAttachments = linear_alloc_child(queue->ctx, sizeof(VkImageView) * ceil(tmp_src22->attachmentCount));
               if (tmp_dst21->pAttachments == NULL) return NULL;
               memcpy((void *)tmp_dst21->pAttachments, tmp_src22->pAttachments, sizeof(VkImageView) * ceil(tmp_src22->attachmentCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_TRANSFORM_BEGIN_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassTransformBeginInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassTransformBeginInfoQCOM));
            break;
         case VK_STRUCTURE_TYPE_MULTIVIEW_PER_VIEW_RENDER_AREAS_RENDER_PASS_BEGIN_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_dst23 = (void *)(*dst_pnext_link);
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_src24 = (void *)pnext;
            if (tmp_src24->pPerViewRenderAreas) {
               tmp_dst23->pPerViewRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src24->perViewRenderAreaCount));
               if (tmp_dst23->pPerViewRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst23->pPerViewRenderAreas, tmp_src24->pPerViewRenderAreas, sizeof(VkRect2D) * ceil(tmp_src24->perViewRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_STRIPE_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassStripeBeginInfoARM));
            VkRenderPassStripeBeginInfoARM *tmp_dst25 = (void *)(*dst_pnext_link);
            VkRenderPassStripeBeginInfoARM *tmp_src26 = (void *)pnext;
            if (tmp_src26->pStripeInfos) {
               tmp_dst25->pStripeInfos = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src26->stripeInfoCount));
               if (tmp_dst25->pStripeInfos == NULL) return NULL;
               memcpy((void *)tmp_dst25->pStripeInfos, tmp_src26->pStripeInfos, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src26->stripeInfoCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_PERFORMANCE_COUNTERS_BY_REGION_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_dst27 = (void *)(*dst_pnext_link);
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_src28 = (void *)pnext;
            if (tmp_src28->pCounterAddresses) {
               tmp_dst27->pCounterAddresses = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddress) * ceil(tmp_src28->counterAddressCount));
               if (tmp_dst27->pCounterAddresses == NULL) return NULL;
               memcpy((void *)tmp_dst27->pCounterAddresses, tmp_src28->pCounterAddresses, sizeof(VkDeviceAddress) * ceil(tmp_src28->counterAddressCount));
            }
            if (tmp_src28->pCounterIndices) {
               tmp_dst27->pCounterIndices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src28->counterIndexCount));
               if (tmp_dst27->pCounterIndices == NULL) return NULL;
               memcpy((void *)tmp_dst27->pCounterIndices, tmp_src28->pCounterIndices, sizeof(uint32_t) * ceil(tmp_src28->counterIndexCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pClearValues) {
         tmp_dst1->pClearValues = linear_alloc_child(queue->ctx, sizeof(VkClearValue) * ceil(tmp_src2->clearValueCount));
         if (tmp_dst1->pClearValues == NULL) return NULL;
         memcpy((void *)tmp_dst1->pClearValues, tmp_src2->pClearValues, sizeof(VkClearValue) * ceil(tmp_src2->clearValueCount));
      }
   } else {
      cmd->u.begin_render_pass.render_pass_begin = NULL;
   }
   cmd->u.begin_render_pass.contents = contents;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_next_subpass(struct vk_cmd_queue *queue
, VkSubpassContents contents
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_NEXT_SUBPASS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_NEXT_SUBPASS;
   cmd->u.next_subpass.contents = contents;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_render_pass(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_RENDER_PASS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_RENDER_PASS;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_commands(struct vk_cmd_queue *queue
, uint32_t commandBufferCount
, const VkCommandBuffer* pCommandBuffers
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_EXECUTE_COMMANDS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_EXECUTE_COMMANDS;
   cmd->u.execute_commands.command_buffer_count = commandBufferCount;
   if (pCommandBuffers) {
      cmd->u.execute_commands.command_buffers = linear_alloc_child(queue->ctx, sizeof(VkCommandBuffer) * ceil(commandBufferCount));
      if (cmd->u.execute_commands.command_buffers == NULL) return NULL;
      memcpy((void *)cmd->u.execute_commands.command_buffers, pCommandBuffers, sizeof(VkCommandBuffer) * ceil(commandBufferCount));
   } else {
      cmd->u.execute_commands.command_buffers = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_begin_ext(struct vk_cmd_queue *queue
, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DEBUG_MARKER_BEGIN_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DEBUG_MARKER_BEGIN_EXT;
   if (pMarkerInfo) {
      cmd->u.debug_marker_begin_ext.marker_info = linear_alloc_child(queue->ctx, sizeof(VkDebugMarkerMarkerInfoEXT));
      if (cmd->u.debug_marker_begin_ext.marker_info == NULL) return NULL;
      memcpy((void *)cmd->u.debug_marker_begin_ext.marker_info, pMarkerInfo, sizeof(VkDebugMarkerMarkerInfoEXT));
      VkDebugMarkerMarkerInfoEXT *tmp_dst1 = (void *)cmd->u.debug_marker_begin_ext.marker_info;
      VkDebugMarkerMarkerInfoEXT *tmp_src2 = (void *)pMarkerInfo;
      tmp_dst1->pMarkerName = linear_strdup(queue->ctx, tmp_src2->pMarkerName);
   } else {
      cmd->u.debug_marker_begin_ext.marker_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_end_ext(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DEBUG_MARKER_END_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DEBUG_MARKER_END_EXT;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_insert_ext(struct vk_cmd_queue *queue
, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DEBUG_MARKER_INSERT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DEBUG_MARKER_INSERT_EXT;
   if (pMarkerInfo) {
      cmd->u.debug_marker_insert_ext.marker_info = linear_alloc_child(queue->ctx, sizeof(VkDebugMarkerMarkerInfoEXT));
      if (cmd->u.debug_marker_insert_ext.marker_info == NULL) return NULL;
      memcpy((void *)cmd->u.debug_marker_insert_ext.marker_info, pMarkerInfo, sizeof(VkDebugMarkerMarkerInfoEXT));
      VkDebugMarkerMarkerInfoEXT *tmp_dst1 = (void *)cmd->u.debug_marker_insert_ext.marker_info;
      VkDebugMarkerMarkerInfoEXT *tmp_src2 = (void *)pMarkerInfo;
      tmp_dst1->pMarkerName = linear_strdup(queue->ctx, tmp_src2->pMarkerName);
   } else {
      cmd->u.debug_marker_insert_ext.marker_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_generated_commands_nv(struct vk_cmd_queue *queue
, VkBool32 isPreprocessed
, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_EXECUTE_GENERATED_COMMANDS_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_EXECUTE_GENERATED_COMMANDS_NV;
   cmd->u.execute_generated_commands_nv.is_preprocessed = isPreprocessed;
   if (pGeneratedCommandsInfo) {
      cmd->u.execute_generated_commands_nv.generated_commands_info = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsInfoNV));
      if (cmd->u.execute_generated_commands_nv.generated_commands_info == NULL) return NULL;
      memcpy((void *)cmd->u.execute_generated_commands_nv.generated_commands_info, pGeneratedCommandsInfo, sizeof(VkGeneratedCommandsInfoNV));
      VkGeneratedCommandsInfoNV *tmp_dst1 = (void *)cmd->u.execute_generated_commands_nv.generated_commands_info;
      VkGeneratedCommandsInfoNV *tmp_src2 = (void *)pGeneratedCommandsInfo;
      if (tmp_src2->pStreams) {
         tmp_dst1->pStreams = linear_alloc_child(queue->ctx, sizeof(VkIndirectCommandsStreamNV) * ceil(tmp_src2->streamCount));
         if (tmp_dst1->pStreams == NULL) return NULL;
         memcpy((void *)tmp_dst1->pStreams, tmp_src2->pStreams, sizeof(VkIndirectCommandsStreamNV) * ceil(tmp_src2->streamCount));
      }
   } else {
      cmd->u.execute_generated_commands_nv.generated_commands_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_preprocess_generated_commands_nv(struct vk_cmd_queue *queue
, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PREPROCESS_GENERATED_COMMANDS_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PREPROCESS_GENERATED_COMMANDS_NV;
   if (pGeneratedCommandsInfo) {
      cmd->u.preprocess_generated_commands_nv.generated_commands_info = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsInfoNV));
      if (cmd->u.preprocess_generated_commands_nv.generated_commands_info == NULL) return NULL;
      memcpy((void *)cmd->u.preprocess_generated_commands_nv.generated_commands_info, pGeneratedCommandsInfo, sizeof(VkGeneratedCommandsInfoNV));
      VkGeneratedCommandsInfoNV *tmp_dst1 = (void *)cmd->u.preprocess_generated_commands_nv.generated_commands_info;
      VkGeneratedCommandsInfoNV *tmp_src2 = (void *)pGeneratedCommandsInfo;
      if (tmp_src2->pStreams) {
         tmp_dst1->pStreams = linear_alloc_child(queue->ctx, sizeof(VkIndirectCommandsStreamNV) * ceil(tmp_src2->streamCount));
         if (tmp_dst1->pStreams == NULL) return NULL;
         memcpy((void *)tmp_dst1->pStreams, tmp_src2->pStreams, sizeof(VkIndirectCommandsStreamNV) * ceil(tmp_src2->streamCount));
      }
   } else {
      cmd->u.preprocess_generated_commands_nv.generated_commands_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_pipeline_shader_group_nv(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipeline pipeline
, uint32_t groupIndex
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_PIPELINE_SHADER_GROUP_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_PIPELINE_SHADER_GROUP_NV;
   cmd->u.bind_pipeline_shader_group_nv.pipeline_bind_point = pipelineBindPoint;
   cmd->u.bind_pipeline_shader_group_nv.pipeline = pipeline;
   cmd->u.bind_pipeline_shader_group_nv.group_index = groupIndex;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_generated_commands_ext(struct vk_cmd_queue *queue
, VkBool32 isPreprocessed
, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_EXECUTE_GENERATED_COMMANDS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_EXECUTE_GENERATED_COMMANDS_EXT;
   cmd->u.execute_generated_commands_ext.is_preprocessed = isPreprocessed;
   if (pGeneratedCommandsInfo) {
      cmd->u.execute_generated_commands_ext.generated_commands_info = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsInfoEXT));
      if (cmd->u.execute_generated_commands_ext.generated_commands_info == NULL) return NULL;
      memcpy((void *)cmd->u.execute_generated_commands_ext.generated_commands_info, pGeneratedCommandsInfo, sizeof(VkGeneratedCommandsInfoEXT));
      VkGeneratedCommandsInfoEXT *tmp_dst1 = (void *)cmd->u.execute_generated_commands_ext.generated_commands_info;
      VkGeneratedCommandsInfoEXT *tmp_src2 = (void *)pGeneratedCommandsInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_GENERATED_COMMANDS_PIPELINE_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsPipelineInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkGeneratedCommandsPipelineInfoEXT));
            break;
         case VK_STRUCTURE_TYPE_GENERATED_COMMANDS_SHADER_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsShaderInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkGeneratedCommandsShaderInfoEXT));
            VkGeneratedCommandsShaderInfoEXT *tmp_dst3 = (void *)(*dst_pnext_link);
            VkGeneratedCommandsShaderInfoEXT *tmp_src4 = (void *)pnext;
            if (tmp_src4->pShaders) {
               tmp_dst3->pShaders = linear_alloc_child(queue->ctx, sizeof(VkShaderEXT) * ceil(tmp_src4->shaderCount));
               if (tmp_dst3->pShaders == NULL) return NULL;
               memcpy((void *)tmp_dst3->pShaders, tmp_src4->pShaders, sizeof(VkShaderEXT) * ceil(tmp_src4->shaderCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.execute_generated_commands_ext.generated_commands_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_preprocess_generated_commands_ext(struct vk_cmd_queue *queue
, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo
, VkCommandBuffer stateCommandBuffer
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PREPROCESS_GENERATED_COMMANDS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PREPROCESS_GENERATED_COMMANDS_EXT;
   if (pGeneratedCommandsInfo) {
      cmd->u.preprocess_generated_commands_ext.generated_commands_info = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsInfoEXT));
      if (cmd->u.preprocess_generated_commands_ext.generated_commands_info == NULL) return NULL;
      memcpy((void *)cmd->u.preprocess_generated_commands_ext.generated_commands_info, pGeneratedCommandsInfo, sizeof(VkGeneratedCommandsInfoEXT));
      VkGeneratedCommandsInfoEXT *tmp_dst1 = (void *)cmd->u.preprocess_generated_commands_ext.generated_commands_info;
      VkGeneratedCommandsInfoEXT *tmp_src2 = (void *)pGeneratedCommandsInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_GENERATED_COMMANDS_PIPELINE_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsPipelineInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkGeneratedCommandsPipelineInfoEXT));
            break;
         case VK_STRUCTURE_TYPE_GENERATED_COMMANDS_SHADER_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkGeneratedCommandsShaderInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkGeneratedCommandsShaderInfoEXT));
            VkGeneratedCommandsShaderInfoEXT *tmp_dst3 = (void *)(*dst_pnext_link);
            VkGeneratedCommandsShaderInfoEXT *tmp_src4 = (void *)pnext;
            if (tmp_src4->pShaders) {
               tmp_dst3->pShaders = linear_alloc_child(queue->ctx, sizeof(VkShaderEXT) * ceil(tmp_src4->shaderCount));
               if (tmp_dst3->pShaders == NULL) return NULL;
               memcpy((void *)tmp_dst3->pShaders, tmp_src4->pShaders, sizeof(VkShaderEXT) * ceil(tmp_src4->shaderCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.preprocess_generated_commands_ext.generated_commands_info = NULL;
   }
   cmd->u.preprocess_generated_commands_ext.state_command_buffer = stateCommandBuffer;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipelineLayout layout
, uint32_t set
, uint32_t descriptorWriteCount
, const VkWriteDescriptorSet* pDescriptorWrites
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_DESCRIPTOR_SET]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_DESCRIPTOR_SET;
   cmd->u.push_descriptor_set.pipeline_bind_point = pipelineBindPoint;
   cmd->u.push_descriptor_set.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.push_descriptor_set.set = set;
   cmd->u.push_descriptor_set.descriptor_write_count = descriptorWriteCount;
   if (pDescriptorWrites) {
      cmd->u.push_descriptor_set.descriptor_writes = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSet) * ceil(descriptorWriteCount));
      if (cmd->u.push_descriptor_set.descriptor_writes == NULL) return NULL;
      memcpy((void *)cmd->u.push_descriptor_set.descriptor_writes, pDescriptorWrites, sizeof(VkWriteDescriptorSet) * ceil(descriptorWriteCount));
      VkWriteDescriptorSet *tmp_dst1 = (void *)cmd->u.push_descriptor_set.descriptor_writes;
      VkWriteDescriptorSet *tmp_src2 = (void *)pDescriptorWrites;
      for (uint32_t i3 = 0; i3 < descriptorWriteCount; i3++) {
         VkWriteDescriptorSet *tmp_dst4 = tmp_dst1 + i3;
         VkWriteDescriptorSet *tmp_src5 = tmp_src2 + i3;
         const VkBaseInStructure *pnext = tmp_src5->pNext;
         void **dst_pnext_link = (void **)&tmp_dst4->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetInlineUniformBlock));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetInlineUniformBlock));
               VkWriteDescriptorSetInlineUniformBlock *tmp_dst6 = (void *)(*dst_pnext_link);
               VkWriteDescriptorSetInlineUniformBlock *tmp_src7 = (void *)pnext;
               if (tmp_src7->pData) {
                  tmp_dst6->pData = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src7->dataSize));
                  if (tmp_dst6->pData == NULL) return NULL;
                  memcpy((void *)tmp_dst6->pData, tmp_src7->pData, 1 * ceil(tmp_src7->dataSize));
               }
               break;
            case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetAccelerationStructureKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetAccelerationStructureKHR));
               VkWriteDescriptorSetAccelerationStructureKHR *tmp_dst8 = (void *)(*dst_pnext_link);
               VkWriteDescriptorSetAccelerationStructureKHR *tmp_src9 = (void *)pnext;
               if (tmp_src9->pAccelerationStructures) {
                  tmp_dst8->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureKHR) * ceil(tmp_src9->accelerationStructureCount));
                  if (tmp_dst8->pAccelerationStructures == NULL) return NULL;
                  memcpy((void *)tmp_dst8->pAccelerationStructures, tmp_src9->pAccelerationStructures, sizeof(VkAccelerationStructureKHR) * ceil(tmp_src9->accelerationStructureCount));
               }
               break;
            case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetAccelerationStructureNV));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetAccelerationStructureNV));
               VkWriteDescriptorSetAccelerationStructureNV *tmp_dst10 = (void *)(*dst_pnext_link);
               VkWriteDescriptorSetAccelerationStructureNV *tmp_src11 = (void *)pnext;
               if (tmp_src11->pAccelerationStructures) {
                  tmp_dst10->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureNV) * ceil(tmp_src11->accelerationStructureCount));
                  if (tmp_dst10->pAccelerationStructures == NULL) return NULL;
                  memcpy((void *)tmp_dst10->pAccelerationStructures, tmp_src11->pAccelerationStructures, sizeof(VkAccelerationStructureNV) * ceil(tmp_src11->accelerationStructureCount));
               }
               break;
            case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_PARTITIONED_ACCELERATION_STRUCTURE_NV:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetPartitionedAccelerationStructureNV));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetPartitionedAccelerationStructureNV));
               VkWriteDescriptorSetPartitionedAccelerationStructureNV *tmp_dst12 = (void *)(*dst_pnext_link);
               VkWriteDescriptorSetPartitionedAccelerationStructureNV *tmp_src13 = (void *)pnext;
               if (tmp_src13->pAccelerationStructures) {
                  tmp_dst12->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddress) * ceil(tmp_src13->accelerationStructureCount));
                  if (tmp_dst12->pAccelerationStructures == NULL) return NULL;
                  memcpy((void *)tmp_dst12->pAccelerationStructures, tmp_src13->pAccelerationStructures, sizeof(VkDeviceAddress) * ceil(tmp_src13->accelerationStructureCount));
               }
               break;
            case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_TENSOR_ARM:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetTensorARM));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetTensorARM));
               VkWriteDescriptorSetTensorARM *tmp_dst14 = (void *)(*dst_pnext_link);
               VkWriteDescriptorSetTensorARM *tmp_src15 = (void *)pnext;
               if (tmp_src15->pTensorViews) {
                  tmp_dst14->pTensorViews = linear_alloc_child(queue->ctx, sizeof(VkTensorViewARM) * ceil(tmp_src15->tensorViewCount));
                  if (tmp_dst14->pTensorViews == NULL) return NULL;
                  memcpy((void *)tmp_dst14->pTensorViews, tmp_src15->pTensorViews, sizeof(VkTensorViewARM) * ceil(tmp_src15->tensorViewCount));
               }
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
         enqueue_VkWriteDescriptorSet(queue, tmp_dst4, tmp_src5);
      }
   } else {
      cmd->u.push_descriptor_set.descriptor_writes = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_device_mask(struct vk_cmd_queue *queue
, uint32_t deviceMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEVICE_MASK]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEVICE_MASK;
   cmd->u.set_device_mask.device_mask = deviceMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_base(struct vk_cmd_queue *queue
, uint32_t baseGroupX
, uint32_t baseGroupY
, uint32_t baseGroupZ
, uint32_t groupCountX
, uint32_t groupCountY
, uint32_t groupCountZ
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH_BASE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH_BASE;
   cmd->u.dispatch_base.base_group_x = baseGroupX;
   cmd->u.dispatch_base.base_group_y = baseGroupY;
   cmd->u.dispatch_base.base_group_z = baseGroupZ;
   cmd->u.dispatch_base.group_count_x = groupCountX;
   cmd->u.dispatch_base.group_count_y = groupCountY;
   cmd->u.dispatch_base.group_count_z = groupCountZ;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set_with_template(struct vk_cmd_queue *queue
, VkDescriptorUpdateTemplate descriptorUpdateTemplate
, VkPipelineLayout layout
, uint32_t set
, const void* pData
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE;
   cmd->u.push_descriptor_set_with_template.descriptor_update_template = descriptorUpdateTemplate;
   enqueue_descriptor_template(queue, descriptorUpdateTemplate);
   cmd->u.push_descriptor_set_with_template.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.push_descriptor_set_with_template.set = set;
   cmd->u.push_descriptor_set_with_template.data = enqueue_push_descriptor_template_data(queue, descriptorUpdateTemplate, pData);

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_wscaling_nv(struct vk_cmd_queue *queue
, uint32_t firstViewport
, uint32_t viewportCount
, const VkViewportWScalingNV* pViewportWScalings
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT_WSCALING_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT_WSCALING_NV;
   cmd->u.set_viewport_wscaling_nv.first_viewport = firstViewport;
   cmd->u.set_viewport_wscaling_nv.viewport_count = viewportCount;
   if (pViewportWScalings) {
      cmd->u.set_viewport_wscaling_nv.viewport_wscalings = linear_alloc_child(queue->ctx, sizeof(VkViewportWScalingNV) * ceil(viewportCount));
      if (cmd->u.set_viewport_wscaling_nv.viewport_wscalings == NULL) return NULL;
      memcpy((void *)cmd->u.set_viewport_wscaling_nv.viewport_wscalings, pViewportWScalings, sizeof(VkViewportWScalingNV) * ceil(viewportCount));
   } else {
      cmd->u.set_viewport_wscaling_nv.viewport_wscalings = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_ext(struct vk_cmd_queue *queue
, uint32_t firstDiscardRectangle
, uint32_t discardRectangleCount
, const VkRect2D* pDiscardRectangles
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DISCARD_RECTANGLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DISCARD_RECTANGLE_EXT;
   cmd->u.set_discard_rectangle_ext.first_discard_rectangle = firstDiscardRectangle;
   cmd->u.set_discard_rectangle_ext.discard_rectangle_count = discardRectangleCount;
   if (pDiscardRectangles) {
      cmd->u.set_discard_rectangle_ext.discard_rectangles = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(discardRectangleCount));
      if (cmd->u.set_discard_rectangle_ext.discard_rectangles == NULL) return NULL;
      memcpy((void *)cmd->u.set_discard_rectangle_ext.discard_rectangles, pDiscardRectangles, sizeof(VkRect2D) * ceil(discardRectangleCount));
   } else {
      cmd->u.set_discard_rectangle_ext.discard_rectangles = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_enable_ext(struct vk_cmd_queue *queue
, VkBool32 discardRectangleEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DISCARD_RECTANGLE_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DISCARD_RECTANGLE_ENABLE_EXT;
   cmd->u.set_discard_rectangle_enable_ext.discard_rectangle_enable = discardRectangleEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_mode_ext(struct vk_cmd_queue *queue
, VkDiscardRectangleModeEXT discardRectangleMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DISCARD_RECTANGLE_MODE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DISCARD_RECTANGLE_MODE_EXT;
   cmd->u.set_discard_rectangle_mode_ext.discard_rectangle_mode = discardRectangleMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_locations_ext(struct vk_cmd_queue *queue
, const VkSampleLocationsInfoEXT* pSampleLocationsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SAMPLE_LOCATIONS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SAMPLE_LOCATIONS_EXT;
   if (pSampleLocationsInfo) {
      cmd->u.set_sample_locations_ext.sample_locations_info = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
      if (cmd->u.set_sample_locations_ext.sample_locations_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_sample_locations_ext.sample_locations_info, pSampleLocationsInfo, sizeof(VkSampleLocationsInfoEXT));
      VkSampleLocationsInfoEXT *tmp_dst1 = (void *)cmd->u.set_sample_locations_ext.sample_locations_info;
      VkSampleLocationsInfoEXT *tmp_src2 = (void *)pSampleLocationsInfo;
      if (tmp_src2->pSampleLocations) {
         tmp_dst1->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src2->sampleLocationsCount));
         if (tmp_dst1->pSampleLocations == NULL) return NULL;
         memcpy((void *)tmp_dst1->pSampleLocations, tmp_src2->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src2->sampleLocationsCount));
      }
   } else {
      cmd->u.set_sample_locations_ext.sample_locations_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_debug_utils_label_ext(struct vk_cmd_queue *queue
, const VkDebugUtilsLabelEXT* pLabelInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_DEBUG_UTILS_LABEL_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_DEBUG_UTILS_LABEL_EXT;
   if (pLabelInfo) {
      cmd->u.begin_debug_utils_label_ext.label_info = linear_alloc_child(queue->ctx, sizeof(VkDebugUtilsLabelEXT));
      if (cmd->u.begin_debug_utils_label_ext.label_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_debug_utils_label_ext.label_info, pLabelInfo, sizeof(VkDebugUtilsLabelEXT));
      VkDebugUtilsLabelEXT *tmp_dst1 = (void *)cmd->u.begin_debug_utils_label_ext.label_info;
      VkDebugUtilsLabelEXT *tmp_src2 = (void *)pLabelInfo;
      tmp_dst1->pLabelName = linear_strdup(queue->ctx, tmp_src2->pLabelName);
   } else {
      cmd->u.begin_debug_utils_label_ext.label_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_debug_utils_label_ext(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_DEBUG_UTILS_LABEL_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_DEBUG_UTILS_LABEL_EXT;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_insert_debug_utils_label_ext(struct vk_cmd_queue *queue
, const VkDebugUtilsLabelEXT* pLabelInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_INSERT_DEBUG_UTILS_LABEL_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_INSERT_DEBUG_UTILS_LABEL_EXT;
   if (pLabelInfo) {
      cmd->u.insert_debug_utils_label_ext.label_info = linear_alloc_child(queue->ctx, sizeof(VkDebugUtilsLabelEXT));
      if (cmd->u.insert_debug_utils_label_ext.label_info == NULL) return NULL;
      memcpy((void *)cmd->u.insert_debug_utils_label_ext.label_info, pLabelInfo, sizeof(VkDebugUtilsLabelEXT));
      VkDebugUtilsLabelEXT *tmp_dst1 = (void *)cmd->u.insert_debug_utils_label_ext.label_info;
      VkDebugUtilsLabelEXT *tmp_src2 = (void *)pLabelInfo;
      tmp_dst1->pLabelName = linear_strdup(queue->ctx, tmp_src2->pLabelName);
   } else {
      cmd->u.insert_debug_utils_label_ext.label_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_buffer_marker_amd(struct vk_cmd_queue *queue
, VkPipelineStageFlagBits pipelineStage
, VkBuffer dstBuffer
, VkDeviceSize dstOffset
, uint32_t marker
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_BUFFER_MARKER_AMD]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_BUFFER_MARKER_AMD;
   cmd->u.write_buffer_marker_amd.pipeline_stage = pipelineStage;
   cmd->u.write_buffer_marker_amd.dst_buffer = dstBuffer;
   cmd->u.write_buffer_marker_amd.dst_offset = dstOffset;
   cmd->u.write_buffer_marker_amd.marker = marker;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_render_pass2(struct vk_cmd_queue *queue
, const VkRenderPassBeginInfo*      pRenderPassBegin
, const VkSubpassBeginInfo*      pSubpassBeginInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_RENDER_PASS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_RENDER_PASS2;
   if (pRenderPassBegin) {
      cmd->u.begin_render_pass2.render_pass_begin = linear_alloc_child(queue->ctx, sizeof(VkRenderPassBeginInfo));
      if (cmd->u.begin_render_pass2.render_pass_begin == NULL) return NULL;
      memcpy((void *)cmd->u.begin_render_pass2.render_pass_begin, pRenderPassBegin, sizeof(VkRenderPassBeginInfo));
      VkRenderPassBeginInfo *tmp_dst1 = (void *)cmd->u.begin_render_pass2.render_pass_begin;
      VkRenderPassBeginInfo *tmp_src2 = (void *)pRenderPassBegin;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDeviceGroupRenderPassBeginInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDeviceGroupRenderPassBeginInfo));
            VkDeviceGroupRenderPassBeginInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkDeviceGroupRenderPassBeginInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pDeviceRenderAreas) {
               tmp_dst3->pDeviceRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
               if (tmp_dst3->pDeviceRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst3->pDeviceRenderAreas, tmp_src4->pDeviceRenderAreas, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_SAMPLE_LOCATIONS_BEGIN_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassSampleLocationsBeginInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassSampleLocationsBeginInfoEXT));
            VkRenderPassSampleLocationsBeginInfoEXT *tmp_dst5 = (void *)(*dst_pnext_link);
            VkRenderPassSampleLocationsBeginInfoEXT *tmp_src6 = (void *)pnext;
            if (tmp_src6->pAttachmentInitialSampleLocations) {
               tmp_dst5->pAttachmentInitialSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkAttachmentSampleLocationsEXT) * ceil(tmp_src6->attachmentInitialSampleLocationsCount));
               if (tmp_dst5->pAttachmentInitialSampleLocations == NULL) return NULL;
               memcpy((void *)tmp_dst5->pAttachmentInitialSampleLocations, tmp_src6->pAttachmentInitialSampleLocations, sizeof(VkAttachmentSampleLocationsEXT) * ceil(tmp_src6->attachmentInitialSampleLocationsCount));
               VkAttachmentSampleLocationsEXT *tmp_dst7 = (void *)tmp_dst5->pAttachmentInitialSampleLocations;
               VkAttachmentSampleLocationsEXT *tmp_src8 = (void *)tmp_src6->pAttachmentInitialSampleLocations;
               for (uint32_t i9 = 0; i9 < tmp_src6->attachmentInitialSampleLocationsCount; i9++) {
                  VkAttachmentSampleLocationsEXT *tmp_dst10 = tmp_dst7 + i9;
                  VkAttachmentSampleLocationsEXT *tmp_src11 = tmp_src8 + i9;
                  VkSampleLocationsInfoEXT *tmp_dst12 = (void *)&tmp_dst10->sampleLocationsInfo;
                  VkSampleLocationsInfoEXT *tmp_src13 = (void *)&tmp_src11->sampleLocationsInfo;
                  if (tmp_src13->pSampleLocations) {
                     tmp_dst12->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src13->sampleLocationsCount));
                     if (tmp_dst12->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst12->pSampleLocations, tmp_src13->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src13->sampleLocationsCount));
                  }
               }
            }
            if (tmp_src6->pPostSubpassSampleLocations) {
               tmp_dst5->pPostSubpassSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSubpassSampleLocationsEXT) * ceil(tmp_src6->postSubpassSampleLocationsCount));
               if (tmp_dst5->pPostSubpassSampleLocations == NULL) return NULL;
               memcpy((void *)tmp_dst5->pPostSubpassSampleLocations, tmp_src6->pPostSubpassSampleLocations, sizeof(VkSubpassSampleLocationsEXT) * ceil(tmp_src6->postSubpassSampleLocationsCount));
               VkSubpassSampleLocationsEXT *tmp_dst14 = (void *)tmp_dst5->pPostSubpassSampleLocations;
               VkSubpassSampleLocationsEXT *tmp_src15 = (void *)tmp_src6->pPostSubpassSampleLocations;
               for (uint32_t i16 = 0; i16 < tmp_src6->postSubpassSampleLocationsCount; i16++) {
                  VkSubpassSampleLocationsEXT *tmp_dst17 = tmp_dst14 + i16;
                  VkSubpassSampleLocationsEXT *tmp_src18 = tmp_src15 + i16;
                  VkSampleLocationsInfoEXT *tmp_dst19 = (void *)&tmp_dst17->sampleLocationsInfo;
                  VkSampleLocationsInfoEXT *tmp_src20 = (void *)&tmp_src18->sampleLocationsInfo;
                  if (tmp_src20->pSampleLocations) {
                     tmp_dst19->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src20->sampleLocationsCount));
                     if (tmp_dst19->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst19->pSampleLocations, tmp_src20->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src20->sampleLocationsCount));
                  }
               }
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassAttachmentBeginInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassAttachmentBeginInfo));
            VkRenderPassAttachmentBeginInfo *tmp_dst21 = (void *)(*dst_pnext_link);
            VkRenderPassAttachmentBeginInfo *tmp_src22 = (void *)pnext;
            if (tmp_src22->pAttachments) {
               tmp_dst21->pAttachments = linear_alloc_child(queue->ctx, sizeof(VkImageView) * ceil(tmp_src22->attachmentCount));
               if (tmp_dst21->pAttachments == NULL) return NULL;
               memcpy((void *)tmp_dst21->pAttachments, tmp_src22->pAttachments, sizeof(VkImageView) * ceil(tmp_src22->attachmentCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_TRANSFORM_BEGIN_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassTransformBeginInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassTransformBeginInfoQCOM));
            break;
         case VK_STRUCTURE_TYPE_MULTIVIEW_PER_VIEW_RENDER_AREAS_RENDER_PASS_BEGIN_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_dst23 = (void *)(*dst_pnext_link);
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_src24 = (void *)pnext;
            if (tmp_src24->pPerViewRenderAreas) {
               tmp_dst23->pPerViewRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src24->perViewRenderAreaCount));
               if (tmp_dst23->pPerViewRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst23->pPerViewRenderAreas, tmp_src24->pPerViewRenderAreas, sizeof(VkRect2D) * ceil(tmp_src24->perViewRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_STRIPE_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassStripeBeginInfoARM));
            VkRenderPassStripeBeginInfoARM *tmp_dst25 = (void *)(*dst_pnext_link);
            VkRenderPassStripeBeginInfoARM *tmp_src26 = (void *)pnext;
            if (tmp_src26->pStripeInfos) {
               tmp_dst25->pStripeInfos = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src26->stripeInfoCount));
               if (tmp_dst25->pStripeInfos == NULL) return NULL;
               memcpy((void *)tmp_dst25->pStripeInfos, tmp_src26->pStripeInfos, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src26->stripeInfoCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_PERFORMANCE_COUNTERS_BY_REGION_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_dst27 = (void *)(*dst_pnext_link);
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_src28 = (void *)pnext;
            if (tmp_src28->pCounterAddresses) {
               tmp_dst27->pCounterAddresses = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddress) * ceil(tmp_src28->counterAddressCount));
               if (tmp_dst27->pCounterAddresses == NULL) return NULL;
               memcpy((void *)tmp_dst27->pCounterAddresses, tmp_src28->pCounterAddresses, sizeof(VkDeviceAddress) * ceil(tmp_src28->counterAddressCount));
            }
            if (tmp_src28->pCounterIndices) {
               tmp_dst27->pCounterIndices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src28->counterIndexCount));
               if (tmp_dst27->pCounterIndices == NULL) return NULL;
               memcpy((void *)tmp_dst27->pCounterIndices, tmp_src28->pCounterIndices, sizeof(uint32_t) * ceil(tmp_src28->counterIndexCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pClearValues) {
         tmp_dst1->pClearValues = linear_alloc_child(queue->ctx, sizeof(VkClearValue) * ceil(tmp_src2->clearValueCount));
         if (tmp_dst1->pClearValues == NULL) return NULL;
         memcpy((void *)tmp_dst1->pClearValues, tmp_src2->pClearValues, sizeof(VkClearValue) * ceil(tmp_src2->clearValueCount));
      }
   } else {
      cmd->u.begin_render_pass2.render_pass_begin = NULL;
   }
   if (pSubpassBeginInfo) {
      cmd->u.begin_render_pass2.subpass_begin_info = linear_alloc_child(queue->ctx, sizeof(VkSubpassBeginInfo));
      if (cmd->u.begin_render_pass2.subpass_begin_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_render_pass2.subpass_begin_info, pSubpassBeginInfo, sizeof(VkSubpassBeginInfo));
   } else {
      cmd->u.begin_render_pass2.subpass_begin_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_next_subpass2(struct vk_cmd_queue *queue
, const VkSubpassBeginInfo*      pSubpassBeginInfo
, const VkSubpassEndInfo*        pSubpassEndInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_NEXT_SUBPASS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_NEXT_SUBPASS2;
   if (pSubpassBeginInfo) {
      cmd->u.next_subpass2.subpass_begin_info = linear_alloc_child(queue->ctx, sizeof(VkSubpassBeginInfo));
      if (cmd->u.next_subpass2.subpass_begin_info == NULL) return NULL;
      memcpy((void *)cmd->u.next_subpass2.subpass_begin_info, pSubpassBeginInfo, sizeof(VkSubpassBeginInfo));
   } else {
      cmd->u.next_subpass2.subpass_begin_info = NULL;
   }
   if (pSubpassEndInfo) {
      cmd->u.next_subpass2.subpass_end_info = linear_alloc_child(queue->ctx, sizeof(VkSubpassEndInfo));
      if (cmd->u.next_subpass2.subpass_end_info == NULL) return NULL;
      memcpy((void *)cmd->u.next_subpass2.subpass_end_info, pSubpassEndInfo, sizeof(VkSubpassEndInfo));
      VkSubpassEndInfo *tmp_dst1 = (void *)cmd->u.next_subpass2.subpass_end_info;
      VkSubpassEndInfo *tmp_src2 = (void *)pSubpassEndInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_OFFSET_END_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_dst3 = (void *)(*dst_pnext_link);
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_src4 = (void *)pnext;
            if (tmp_src4->pFragmentDensityOffsets) {
               tmp_dst3->pFragmentDensityOffsets = linear_alloc_child(queue->ctx, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
               if (tmp_dst3->pFragmentDensityOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst3->pFragmentDensityOffsets, tmp_src4->pFragmentDensityOffsets, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.next_subpass2.subpass_end_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_render_pass2(struct vk_cmd_queue *queue
, const VkSubpassEndInfo*        pSubpassEndInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_RENDER_PASS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_RENDER_PASS2;
   if (pSubpassEndInfo) {
      cmd->u.end_render_pass2.subpass_end_info = linear_alloc_child(queue->ctx, sizeof(VkSubpassEndInfo));
      if (cmd->u.end_render_pass2.subpass_end_info == NULL) return NULL;
      memcpy((void *)cmd->u.end_render_pass2.subpass_end_info, pSubpassEndInfo, sizeof(VkSubpassEndInfo));
      VkSubpassEndInfo *tmp_dst1 = (void *)cmd->u.end_render_pass2.subpass_end_info;
      VkSubpassEndInfo *tmp_src2 = (void *)pSubpassEndInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_OFFSET_END_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_dst3 = (void *)(*dst_pnext_link);
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_src4 = (void *)pnext;
            if (tmp_src4->pFragmentDensityOffsets) {
               tmp_dst3->pFragmentDensityOffsets = linear_alloc_child(queue->ctx, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
               if (tmp_dst3->pFragmentDensityOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst3->pFragmentDensityOffsets, tmp_src4->pFragmentDensityOffsets, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.end_render_pass2.subpass_end_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_count(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkBuffer countBuffer
, VkDeviceSize countBufferOffset
, uint32_t maxDrawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT_COUNT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT_COUNT;
   cmd->u.draw_indirect_count.buffer = buffer;
   cmd->u.draw_indirect_count.offset = offset;
   cmd->u.draw_indirect_count.count_buffer = countBuffer;
   cmd->u.draw_indirect_count.count_buffer_offset = countBufferOffset;
   cmd->u.draw_indirect_count.max_draw_count = maxDrawCount;
   cmd->u.draw_indirect_count.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect_count(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkBuffer countBuffer
, VkDeviceSize countBufferOffset
, uint32_t maxDrawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDEXED_INDIRECT_COUNT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDEXED_INDIRECT_COUNT;
   cmd->u.draw_indexed_indirect_count.buffer = buffer;
   cmd->u.draw_indexed_indirect_count.offset = offset;
   cmd->u.draw_indexed_indirect_count.count_buffer = countBuffer;
   cmd->u.draw_indexed_indirect_count.count_buffer_offset = countBufferOffset;
   cmd->u.draw_indexed_indirect_count.max_draw_count = maxDrawCount;
   cmd->u.draw_indexed_indirect_count.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_checkpoint_nv(struct vk_cmd_queue *queue
, const void* pCheckpointMarker
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_CHECKPOINT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_CHECKPOINT_NV;
   cmd->u.set_checkpoint_nv.checkpoint_marker = ( void* )pCheckpointMarker;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_transform_feedback_buffers_ext(struct vk_cmd_queue *queue
, uint32_t firstBinding
, uint32_t bindingCount
, const VkBuffer* pBuffers
, const VkDeviceSize* pOffsets
, const VkDeviceSize* pSizes
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS_EXT;
   cmd->u.bind_transform_feedback_buffers_ext.first_binding = firstBinding;
   cmd->u.bind_transform_feedback_buffers_ext.binding_count = bindingCount;
   if (pBuffers) {
      cmd->u.bind_transform_feedback_buffers_ext.buffers = linear_alloc_child(queue->ctx, sizeof(VkBuffer) * ceil(bindingCount));
      if (cmd->u.bind_transform_feedback_buffers_ext.buffers == NULL) return NULL;
      memcpy((void *)cmd->u.bind_transform_feedback_buffers_ext.buffers, pBuffers, sizeof(VkBuffer) * ceil(bindingCount));
   } else {
      cmd->u.bind_transform_feedback_buffers_ext.buffers = NULL;
   }
   if (pOffsets) {
      cmd->u.bind_transform_feedback_buffers_ext.offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_transform_feedback_buffers_ext.offsets == NULL) return NULL;
      memcpy((void *)cmd->u.bind_transform_feedback_buffers_ext.offsets, pOffsets, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_transform_feedback_buffers_ext.offsets = NULL;
   }
   if (pSizes) {
      cmd->u.bind_transform_feedback_buffers_ext.sizes = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_transform_feedback_buffers_ext.sizes == NULL) return NULL;
      memcpy((void *)cmd->u.bind_transform_feedback_buffers_ext.sizes, pSizes, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_transform_feedback_buffers_ext.sizes = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_transform_feedback_ext(struct vk_cmd_queue *queue
, uint32_t firstCounterBuffer
, uint32_t counterBufferCount
, const VkBuffer* pCounterBuffers
, const VkDeviceSize* pCounterBufferOffsets
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_TRANSFORM_FEEDBACK_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_TRANSFORM_FEEDBACK_EXT;
   cmd->u.begin_transform_feedback_ext.first_counter_buffer = firstCounterBuffer;
   cmd->u.begin_transform_feedback_ext.counter_buffer_count = counterBufferCount;
   if (pCounterBuffers) {
      cmd->u.begin_transform_feedback_ext.counter_buffers = linear_alloc_child(queue->ctx, sizeof(VkBuffer) * ceil(counterBufferCount));
      if (cmd->u.begin_transform_feedback_ext.counter_buffers == NULL) return NULL;
      memcpy((void *)cmd->u.begin_transform_feedback_ext.counter_buffers, pCounterBuffers, sizeof(VkBuffer) * ceil(counterBufferCount));
   } else {
      cmd->u.begin_transform_feedback_ext.counter_buffers = NULL;
   }
   if (pCounterBufferOffsets) {
      cmd->u.begin_transform_feedback_ext.counter_buffer_offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(counterBufferCount));
      if (cmd->u.begin_transform_feedback_ext.counter_buffer_offsets == NULL) return NULL;
      memcpy((void *)cmd->u.begin_transform_feedback_ext.counter_buffer_offsets, pCounterBufferOffsets, sizeof(VkDeviceSize) * ceil(counterBufferCount));
   } else {
      cmd->u.begin_transform_feedback_ext.counter_buffer_offsets = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_transform_feedback_ext(struct vk_cmd_queue *queue
, uint32_t firstCounterBuffer
, uint32_t counterBufferCount
, const VkBuffer* pCounterBuffers
, const VkDeviceSize* pCounterBufferOffsets
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_TRANSFORM_FEEDBACK_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_TRANSFORM_FEEDBACK_EXT;
   cmd->u.end_transform_feedback_ext.first_counter_buffer = firstCounterBuffer;
   cmd->u.end_transform_feedback_ext.counter_buffer_count = counterBufferCount;
   if (pCounterBuffers) {
      cmd->u.end_transform_feedback_ext.counter_buffers = linear_alloc_child(queue->ctx, sizeof(VkBuffer) * ceil(counterBufferCount));
      if (cmd->u.end_transform_feedback_ext.counter_buffers == NULL) return NULL;
      memcpy((void *)cmd->u.end_transform_feedback_ext.counter_buffers, pCounterBuffers, sizeof(VkBuffer) * ceil(counterBufferCount));
   } else {
      cmd->u.end_transform_feedback_ext.counter_buffers = NULL;
   }
   if (pCounterBufferOffsets) {
      cmd->u.end_transform_feedback_ext.counter_buffer_offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(counterBufferCount));
      if (cmd->u.end_transform_feedback_ext.counter_buffer_offsets == NULL) return NULL;
      memcpy((void *)cmd->u.end_transform_feedback_ext.counter_buffer_offsets, pCounterBufferOffsets, sizeof(VkDeviceSize) * ceil(counterBufferCount));
   } else {
      cmd->u.end_transform_feedback_ext.counter_buffer_offsets = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_query_indexed_ext(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t query
, VkQueryControlFlags flags
, uint32_t index
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_QUERY_INDEXED_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_QUERY_INDEXED_EXT;
   cmd->u.begin_query_indexed_ext.query_pool = queryPool;
   cmd->u.begin_query_indexed_ext.query = query;
   cmd->u.begin_query_indexed_ext.flags = flags;
   cmd->u.begin_query_indexed_ext.index = index;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_query_indexed_ext(struct vk_cmd_queue *queue
, VkQueryPool queryPool
, uint32_t query
, uint32_t index
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_QUERY_INDEXED_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_QUERY_INDEXED_EXT;
   cmd->u.end_query_indexed_ext.query_pool = queryPool;
   cmd->u.end_query_indexed_ext.query = query;
   cmd->u.end_query_indexed_ext.index = index;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_byte_count_ext(struct vk_cmd_queue *queue
, uint32_t instanceCount
, uint32_t firstInstance
, VkBuffer counterBuffer
, VkDeviceSize counterBufferOffset
, uint32_t counterOffset
, uint32_t vertexStride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT_BYTE_COUNT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT_BYTE_COUNT_EXT;
   cmd->u.draw_indirect_byte_count_ext.instance_count = instanceCount;
   cmd->u.draw_indirect_byte_count_ext.first_instance = firstInstance;
   cmd->u.draw_indirect_byte_count_ext.counter_buffer = counterBuffer;
   cmd->u.draw_indirect_byte_count_ext.counter_buffer_offset = counterBufferOffset;
   cmd->u.draw_indirect_byte_count_ext.counter_offset = counterOffset;
   cmd->u.draw_indirect_byte_count_ext.vertex_stride = vertexStride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_exclusive_scissor_nv(struct vk_cmd_queue *queue
, uint32_t firstExclusiveScissor
, uint32_t exclusiveScissorCount
, const VkRect2D* pExclusiveScissors
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_EXCLUSIVE_SCISSOR_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_EXCLUSIVE_SCISSOR_NV;
   cmd->u.set_exclusive_scissor_nv.first_exclusive_scissor = firstExclusiveScissor;
   cmd->u.set_exclusive_scissor_nv.exclusive_scissor_count = exclusiveScissorCount;
   if (pExclusiveScissors) {
      cmd->u.set_exclusive_scissor_nv.exclusive_scissors = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(exclusiveScissorCount));
      if (cmd->u.set_exclusive_scissor_nv.exclusive_scissors == NULL) return NULL;
      memcpy((void *)cmd->u.set_exclusive_scissor_nv.exclusive_scissors, pExclusiveScissors, sizeof(VkRect2D) * ceil(exclusiveScissorCount));
   } else {
      cmd->u.set_exclusive_scissor_nv.exclusive_scissors = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_exclusive_scissor_enable_nv(struct vk_cmd_queue *queue
, uint32_t firstExclusiveScissor
, uint32_t exclusiveScissorCount
, const VkBool32* pExclusiveScissorEnables
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_EXCLUSIVE_SCISSOR_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_EXCLUSIVE_SCISSOR_ENABLE_NV;
   cmd->u.set_exclusive_scissor_enable_nv.first_exclusive_scissor = firstExclusiveScissor;
   cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_count = exclusiveScissorCount;
   if (pExclusiveScissorEnables) {
      cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_enables = linear_alloc_child(queue->ctx, sizeof(VkBool32) * ceil(exclusiveScissorCount));
      if (cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_enables == NULL) return NULL;
      memcpy((void *)cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_enables, pExclusiveScissorEnables, sizeof(VkBool32) * ceil(exclusiveScissorCount));
   } else {
      cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_enables = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_shading_rate_image_nv(struct vk_cmd_queue *queue
, VkImageView imageView
, VkImageLayout imageLayout
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_SHADING_RATE_IMAGE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_SHADING_RATE_IMAGE_NV;
   cmd->u.bind_shading_rate_image_nv.image_view = imageView;
   cmd->u.bind_shading_rate_image_nv.image_layout = imageLayout;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_shading_rate_palette_nv(struct vk_cmd_queue *queue
, uint32_t firstViewport
, uint32_t viewportCount
, const VkShadingRatePaletteNV* pShadingRatePalettes
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT_SHADING_RATE_PALETTE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT_SHADING_RATE_PALETTE_NV;
   cmd->u.set_viewport_shading_rate_palette_nv.first_viewport = firstViewport;
   cmd->u.set_viewport_shading_rate_palette_nv.viewport_count = viewportCount;
   if (pShadingRatePalettes) {
      cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes = linear_alloc_child(queue->ctx, sizeof(VkShadingRatePaletteNV) * ceil(viewportCount));
      if (cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes == NULL) return NULL;
      memcpy((void *)cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes, pShadingRatePalettes, sizeof(VkShadingRatePaletteNV) * ceil(viewportCount));
      VkShadingRatePaletteNV *tmp_dst1 = (void *)cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes;
      VkShadingRatePaletteNV *tmp_src2 = (void *)pShadingRatePalettes;
      for (uint32_t i3 = 0; i3 < viewportCount; i3++) {
         VkShadingRatePaletteNV *tmp_dst4 = tmp_dst1 + i3;
         VkShadingRatePaletteNV *tmp_src5 = tmp_src2 + i3;
         if (tmp_src5->pShadingRatePaletteEntries) {
            tmp_dst4->pShadingRatePaletteEntries = linear_alloc_child(queue->ctx, sizeof(VkShadingRatePaletteEntryNV) * ceil(tmp_src5->shadingRatePaletteEntryCount));
            if (tmp_dst4->pShadingRatePaletteEntries == NULL) return NULL;
            memcpy((void *)tmp_dst4->pShadingRatePaletteEntries, tmp_src5->pShadingRatePaletteEntries, sizeof(VkShadingRatePaletteEntryNV) * ceil(tmp_src5->shadingRatePaletteEntryCount));
         }
      }
   } else {
      cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coarse_sample_order_nv(struct vk_cmd_queue *queue
, VkCoarseSampleOrderTypeNV sampleOrderType
, uint32_t customSampleOrderCount
, const VkCoarseSampleOrderCustomNV* pCustomSampleOrders
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COARSE_SAMPLE_ORDER_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COARSE_SAMPLE_ORDER_NV;
   cmd->u.set_coarse_sample_order_nv.sample_order_type = sampleOrderType;
   cmd->u.set_coarse_sample_order_nv.custom_sample_order_count = customSampleOrderCount;
   if (pCustomSampleOrders) {
      cmd->u.set_coarse_sample_order_nv.custom_sample_orders = linear_alloc_child(queue->ctx, sizeof(VkCoarseSampleOrderCustomNV) * ceil(customSampleOrderCount));
      if (cmd->u.set_coarse_sample_order_nv.custom_sample_orders == NULL) return NULL;
      memcpy((void *)cmd->u.set_coarse_sample_order_nv.custom_sample_orders, pCustomSampleOrders, sizeof(VkCoarseSampleOrderCustomNV) * ceil(customSampleOrderCount));
      VkCoarseSampleOrderCustomNV *tmp_dst1 = (void *)cmd->u.set_coarse_sample_order_nv.custom_sample_orders;
      VkCoarseSampleOrderCustomNV *tmp_src2 = (void *)pCustomSampleOrders;
      for (uint32_t i3 = 0; i3 < customSampleOrderCount; i3++) {
         VkCoarseSampleOrderCustomNV *tmp_dst4 = tmp_dst1 + i3;
         VkCoarseSampleOrderCustomNV *tmp_src5 = tmp_src2 + i3;
         if (tmp_src5->pSampleLocations) {
            tmp_dst4->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkCoarseSampleLocationNV) * ceil(tmp_src5->sampleLocationCount));
            if (tmp_dst4->pSampleLocations == NULL) return NULL;
            memcpy((void *)tmp_dst4->pSampleLocations, tmp_src5->pSampleLocations, sizeof(VkCoarseSampleLocationNV) * ceil(tmp_src5->sampleLocationCount));
         }
      }
   } else {
      cmd->u.set_coarse_sample_order_nv.custom_sample_orders = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_nv(struct vk_cmd_queue *queue
, uint32_t taskCount
, uint32_t firstTask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_NV;
   cmd->u.draw_mesh_tasks_nv.task_count = taskCount;
   cmd->u.draw_mesh_tasks_nv.first_task = firstTask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_nv(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, uint32_t drawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT_NV;
   cmd->u.draw_mesh_tasks_indirect_nv.buffer = buffer;
   cmd->u.draw_mesh_tasks_indirect_nv.offset = offset;
   cmd->u.draw_mesh_tasks_indirect_nv.draw_count = drawCount;
   cmd->u.draw_mesh_tasks_indirect_nv.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count_nv(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkBuffer countBuffer
, VkDeviceSize countBufferOffset
, uint32_t maxDrawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_NV;
   cmd->u.draw_mesh_tasks_indirect_count_nv.buffer = buffer;
   cmd->u.draw_mesh_tasks_indirect_count_nv.offset = offset;
   cmd->u.draw_mesh_tasks_indirect_count_nv.count_buffer = countBuffer;
   cmd->u.draw_mesh_tasks_indirect_count_nv.count_buffer_offset = countBufferOffset;
   cmd->u.draw_mesh_tasks_indirect_count_nv.max_draw_count = maxDrawCount;
   cmd->u.draw_mesh_tasks_indirect_count_nv.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_ext(struct vk_cmd_queue *queue
, uint32_t groupCountX
, uint32_t groupCountY
, uint32_t groupCountZ
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_EXT;
   cmd->u.draw_mesh_tasks_ext.group_count_x = groupCountX;
   cmd->u.draw_mesh_tasks_ext.group_count_y = groupCountY;
   cmd->u.draw_mesh_tasks_ext.group_count_z = groupCountZ;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_ext(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, uint32_t drawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT_EXT;
   cmd->u.draw_mesh_tasks_indirect_ext.buffer = buffer;
   cmd->u.draw_mesh_tasks_indirect_ext.offset = offset;
   cmd->u.draw_mesh_tasks_indirect_ext.draw_count = drawCount;
   cmd->u.draw_mesh_tasks_indirect_ext.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count_ext(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkBuffer countBuffer
, VkDeviceSize countBufferOffset
, uint32_t maxDrawCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_EXT;
   cmd->u.draw_mesh_tasks_indirect_count_ext.buffer = buffer;
   cmd->u.draw_mesh_tasks_indirect_count_ext.offset = offset;
   cmd->u.draw_mesh_tasks_indirect_count_ext.count_buffer = countBuffer;
   cmd->u.draw_mesh_tasks_indirect_count_ext.count_buffer_offset = countBufferOffset;
   cmd->u.draw_mesh_tasks_indirect_count_ext.max_draw_count = maxDrawCount;
   cmd->u.draw_mesh_tasks_indirect_count_ext.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_invocation_mask_huawei(struct vk_cmd_queue *queue
, VkImageView imageView
, VkImageLayout imageLayout
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_INVOCATION_MASK_HUAWEI]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_INVOCATION_MASK_HUAWEI;
   cmd->u.bind_invocation_mask_huawei.image_view = imageView;
   cmd->u.bind_invocation_mask_huawei.image_layout = imageLayout;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_nv(struct vk_cmd_queue *queue
, VkAccelerationStructureNV dst
, VkAccelerationStructureNV src
, VkCopyAccelerationStructureModeKHR mode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_ACCELERATION_STRUCTURE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_ACCELERATION_STRUCTURE_NV;
   cmd->u.copy_acceleration_structure_nv.dst = dst;
   cmd->u.copy_acceleration_structure_nv.src = src;
   cmd->u.copy_acceleration_structure_nv.mode = mode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_khr(struct vk_cmd_queue *queue
, const VkCopyAccelerationStructureInfoKHR* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_ACCELERATION_STRUCTURE_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_ACCELERATION_STRUCTURE_KHR;
   if (pInfo) {
      cmd->u.copy_acceleration_structure_khr.info = linear_alloc_child(queue->ctx, sizeof(VkCopyAccelerationStructureInfoKHR));
      if (cmd->u.copy_acceleration_structure_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_acceleration_structure_khr.info, pInfo, sizeof(VkCopyAccelerationStructureInfoKHR));
   } else {
      cmd->u.copy_acceleration_structure_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_to_memory_khr(struct vk_cmd_queue *queue
, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_KHR;
   if (pInfo) {
      cmd->u.copy_acceleration_structure_to_memory_khr.info = linear_alloc_child(queue->ctx, sizeof(VkCopyAccelerationStructureToMemoryInfoKHR));
      if (cmd->u.copy_acceleration_structure_to_memory_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_acceleration_structure_to_memory_khr.info, pInfo, sizeof(VkCopyAccelerationStructureToMemoryInfoKHR));
   } else {
      cmd->u.copy_acceleration_structure_to_memory_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_acceleration_structure_khr(struct vk_cmd_queue *queue
, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_KHR;
   if (pInfo) {
      cmd->u.copy_memory_to_acceleration_structure_khr.info = linear_alloc_child(queue->ctx, sizeof(VkCopyMemoryToAccelerationStructureInfoKHR));
      if (cmd->u.copy_memory_to_acceleration_structure_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_to_acceleration_structure_khr.info, pInfo, sizeof(VkCopyMemoryToAccelerationStructureInfoKHR));
   } else {
      cmd->u.copy_memory_to_acceleration_structure_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_acceleration_structures_properties_khr(struct vk_cmd_queue *queue
, uint32_t accelerationStructureCount
, const VkAccelerationStructureKHR* pAccelerationStructures
, VkQueryType queryType
, VkQueryPool queryPool
, uint32_t firstQuery
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_KHR;
   cmd->u.write_acceleration_structures_properties_khr.acceleration_structure_count = accelerationStructureCount;
   if (pAccelerationStructures) {
      cmd->u.write_acceleration_structures_properties_khr.acceleration_structures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureKHR) * ceil(accelerationStructureCount));
      if (cmd->u.write_acceleration_structures_properties_khr.acceleration_structures == NULL) return NULL;
      memcpy((void *)cmd->u.write_acceleration_structures_properties_khr.acceleration_structures, pAccelerationStructures, sizeof(VkAccelerationStructureKHR) * ceil(accelerationStructureCount));
   } else {
      cmd->u.write_acceleration_structures_properties_khr.acceleration_structures = NULL;
   }
   cmd->u.write_acceleration_structures_properties_khr.query_type = queryType;
   cmd->u.write_acceleration_structures_properties_khr.query_pool = queryPool;
   cmd->u.write_acceleration_structures_properties_khr.first_query = firstQuery;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_acceleration_structures_properties_nv(struct vk_cmd_queue *queue
, uint32_t accelerationStructureCount
, const VkAccelerationStructureNV* pAccelerationStructures
, VkQueryType queryType
, VkQueryPool queryPool
, uint32_t firstQuery
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_NV;
   cmd->u.write_acceleration_structures_properties_nv.acceleration_structure_count = accelerationStructureCount;
   if (pAccelerationStructures) {
      cmd->u.write_acceleration_structures_properties_nv.acceleration_structures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureNV) * ceil(accelerationStructureCount));
      if (cmd->u.write_acceleration_structures_properties_nv.acceleration_structures == NULL) return NULL;
      memcpy((void *)cmd->u.write_acceleration_structures_properties_nv.acceleration_structures, pAccelerationStructures, sizeof(VkAccelerationStructureNV) * ceil(accelerationStructureCount));
   } else {
      cmd->u.write_acceleration_structures_properties_nv.acceleration_structures = NULL;
   }
   cmd->u.write_acceleration_structures_properties_nv.query_type = queryType;
   cmd->u.write_acceleration_structures_properties_nv.query_pool = queryPool;
   cmd->u.write_acceleration_structures_properties_nv.first_query = firstQuery;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_build_acceleration_structure_nv(struct vk_cmd_queue *queue
, const VkAccelerationStructureInfoNV* pInfo
, VkBuffer instanceData
, VkDeviceSize instanceOffset
, VkBool32 update
, VkAccelerationStructureNV dst
, VkAccelerationStructureNV src
, VkBuffer scratch
, VkDeviceSize scratchOffset
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BUILD_ACCELERATION_STRUCTURE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BUILD_ACCELERATION_STRUCTURE_NV;
   if (pInfo) {
      cmd->u.build_acceleration_structure_nv.info = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureInfoNV));
      if (cmd->u.build_acceleration_structure_nv.info == NULL) return NULL;
      memcpy((void *)cmd->u.build_acceleration_structure_nv.info, pInfo, sizeof(VkAccelerationStructureInfoNV));
      VkAccelerationStructureInfoNV *tmp_dst1 = (void *)cmd->u.build_acceleration_structure_nv.info;
      VkAccelerationStructureInfoNV *tmp_src2 = (void *)pInfo;
      if (tmp_src2->pGeometries) {
         tmp_dst1->pGeometries = linear_alloc_child(queue->ctx, sizeof(VkGeometryNV) * ceil(tmp_src2->geometryCount));
         if (tmp_dst1->pGeometries == NULL) return NULL;
         memcpy((void *)tmp_dst1->pGeometries, tmp_src2->pGeometries, sizeof(VkGeometryNV) * ceil(tmp_src2->geometryCount));
      }
   } else {
      cmd->u.build_acceleration_structure_nv.info = NULL;
   }
   cmd->u.build_acceleration_structure_nv.instance_data = instanceData;
   cmd->u.build_acceleration_structure_nv.instance_offset = instanceOffset;
   cmd->u.build_acceleration_structure_nv.update = update;
   cmd->u.build_acceleration_structure_nv.dst = dst;
   cmd->u.build_acceleration_structure_nv.src = src;
   cmd->u.build_acceleration_structure_nv.scratch = scratch;
   cmd->u.build_acceleration_structure_nv.scratch_offset = scratchOffset;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_khr(struct vk_cmd_queue *queue
, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable
, uint32_t width
, uint32_t height
, uint32_t depth
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_TRACE_RAYS_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_TRACE_RAYS_KHR;
   if (pRaygenShaderBindingTable) {
      cmd->u.trace_rays_khr.raygen_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_khr.raygen_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_khr.raygen_shader_binding_table, pRaygenShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_khr.raygen_shader_binding_table = NULL;
   }
   if (pMissShaderBindingTable) {
      cmd->u.trace_rays_khr.miss_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_khr.miss_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_khr.miss_shader_binding_table, pMissShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_khr.miss_shader_binding_table = NULL;
   }
   if (pHitShaderBindingTable) {
      cmd->u.trace_rays_khr.hit_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_khr.hit_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_khr.hit_shader_binding_table, pHitShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_khr.hit_shader_binding_table = NULL;
   }
   if (pCallableShaderBindingTable) {
      cmd->u.trace_rays_khr.callable_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_khr.callable_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_khr.callable_shader_binding_table, pCallableShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_khr.callable_shader_binding_table = NULL;
   }
   cmd->u.trace_rays_khr.width = width;
   cmd->u.trace_rays_khr.height = height;
   cmd->u.trace_rays_khr.depth = depth;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_nv(struct vk_cmd_queue *queue
, VkBuffer raygenShaderBindingTableBuffer
, VkDeviceSize raygenShaderBindingOffset
, VkBuffer missShaderBindingTableBuffer
, VkDeviceSize missShaderBindingOffset
, VkDeviceSize missShaderBindingStride
, VkBuffer hitShaderBindingTableBuffer
, VkDeviceSize hitShaderBindingOffset
, VkDeviceSize hitShaderBindingStride
, VkBuffer callableShaderBindingTableBuffer
, VkDeviceSize callableShaderBindingOffset
, VkDeviceSize callableShaderBindingStride
, uint32_t width
, uint32_t height
, uint32_t depth
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_TRACE_RAYS_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_TRACE_RAYS_NV;
   cmd->u.trace_rays_nv.raygen_shader_binding_table_buffer = raygenShaderBindingTableBuffer;
   cmd->u.trace_rays_nv.raygen_shader_binding_offset = raygenShaderBindingOffset;
   cmd->u.trace_rays_nv.miss_shader_binding_table_buffer = missShaderBindingTableBuffer;
   cmd->u.trace_rays_nv.miss_shader_binding_offset = missShaderBindingOffset;
   cmd->u.trace_rays_nv.miss_shader_binding_stride = missShaderBindingStride;
   cmd->u.trace_rays_nv.hit_shader_binding_table_buffer = hitShaderBindingTableBuffer;
   cmd->u.trace_rays_nv.hit_shader_binding_offset = hitShaderBindingOffset;
   cmd->u.trace_rays_nv.hit_shader_binding_stride = hitShaderBindingStride;
   cmd->u.trace_rays_nv.callable_shader_binding_table_buffer = callableShaderBindingTableBuffer;
   cmd->u.trace_rays_nv.callable_shader_binding_offset = callableShaderBindingOffset;
   cmd->u.trace_rays_nv.callable_shader_binding_stride = callableShaderBindingStride;
   cmd->u.trace_rays_nv.width = width;
   cmd->u.trace_rays_nv.height = height;
   cmd->u.trace_rays_nv.depth = depth;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_indirect_khr(struct vk_cmd_queue *queue
, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable
, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable
, VkDeviceAddress indirectDeviceAddress
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_TRACE_RAYS_INDIRECT_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_TRACE_RAYS_INDIRECT_KHR;
   if (pRaygenShaderBindingTable) {
      cmd->u.trace_rays_indirect_khr.raygen_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_indirect_khr.raygen_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_indirect_khr.raygen_shader_binding_table, pRaygenShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_indirect_khr.raygen_shader_binding_table = NULL;
   }
   if (pMissShaderBindingTable) {
      cmd->u.trace_rays_indirect_khr.miss_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_indirect_khr.miss_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_indirect_khr.miss_shader_binding_table, pMissShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_indirect_khr.miss_shader_binding_table = NULL;
   }
   if (pHitShaderBindingTable) {
      cmd->u.trace_rays_indirect_khr.hit_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_indirect_khr.hit_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_indirect_khr.hit_shader_binding_table, pHitShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_indirect_khr.hit_shader_binding_table = NULL;
   }
   if (pCallableShaderBindingTable) {
      cmd->u.trace_rays_indirect_khr.callable_shader_binding_table = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRegionKHR));
      if (cmd->u.trace_rays_indirect_khr.callable_shader_binding_table == NULL) return NULL;
      memcpy((void *)cmd->u.trace_rays_indirect_khr.callable_shader_binding_table, pCallableShaderBindingTable, sizeof(VkStridedDeviceAddressRegionKHR));
   } else {
      cmd->u.trace_rays_indirect_khr.callable_shader_binding_table = NULL;
   }
   cmd->u.trace_rays_indirect_khr.indirect_device_address = indirectDeviceAddress;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_indirect2_khr(struct vk_cmd_queue *queue
, VkDeviceAddress indirectDeviceAddress
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_TRACE_RAYS_INDIRECT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_TRACE_RAYS_INDIRECT2_KHR;
   cmd->u.trace_rays_indirect2_khr.indirect_device_address = indirectDeviceAddress;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_build_cluster_acceleration_structure_indirect_nv(struct vk_cmd_queue *queue
, const VkClusterAccelerationStructureCommandsInfoNV*  pCommandInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BUILD_CLUSTER_ACCELERATION_STRUCTURE_INDIRECT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BUILD_CLUSTER_ACCELERATION_STRUCTURE_INDIRECT_NV;
   if (pCommandInfos) {
      cmd->u.build_cluster_acceleration_structure_indirect_nv.command_infos = linear_alloc_child(queue->ctx, sizeof(VkClusterAccelerationStructureCommandsInfoNV));
      if (cmd->u.build_cluster_acceleration_structure_indirect_nv.command_infos == NULL) return NULL;
      memcpy((void *)cmd->u.build_cluster_acceleration_structure_indirect_nv.command_infos, pCommandInfos, sizeof(VkClusterAccelerationStructureCommandsInfoNV));
   } else {
      cmd->u.build_cluster_acceleration_structure_indirect_nv.command_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_ray_tracing_pipeline_stack_size_khr(struct vk_cmd_queue *queue
, uint32_t pipelineStackSize
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RAY_TRACING_PIPELINE_STACK_SIZE_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RAY_TRACING_PIPELINE_STACK_SIZE_KHR;
   cmd->u.set_ray_tracing_pipeline_stack_size_khr.pipeline_stack_size = pipelineStackSize;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}




struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_stipple(struct vk_cmd_queue *queue
, uint32_t lineStippleFactor
, uint16_t lineStipplePattern
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LINE_STIPPLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LINE_STIPPLE;
   cmd->u.set_line_stipple.line_stipple_factor = lineStippleFactor;
   cmd->u.set_line_stipple.line_stipple_pattern = lineStipplePattern;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}



struct vk_cmd_queue_entry *vk_enqueue_cmd_set_cull_mode(struct vk_cmd_queue *queue
, VkCullModeFlags cullMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_CULL_MODE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_CULL_MODE;
   cmd->u.set_cull_mode.cull_mode = cullMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_front_face(struct vk_cmd_queue *queue
, VkFrontFace frontFace
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_FRONT_FACE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_FRONT_FACE;
   cmd->u.set_front_face.front_face = frontFace;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_topology(struct vk_cmd_queue *queue
, VkPrimitiveTopology primitiveTopology
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_PRIMITIVE_TOPOLOGY]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_PRIMITIVE_TOPOLOGY;
   cmd->u.set_primitive_topology.primitive_topology = primitiveTopology;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_with_count(struct vk_cmd_queue *queue
, uint32_t viewportCount
, const VkViewport* pViewports
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT_WITH_COUNT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT_WITH_COUNT;
   cmd->u.set_viewport_with_count.viewport_count = viewportCount;
   if (pViewports) {
      cmd->u.set_viewport_with_count.viewports = linear_alloc_child(queue->ctx, sizeof(VkViewport) * ceil(viewportCount));
      if (cmd->u.set_viewport_with_count.viewports == NULL) return NULL;
      memcpy((void *)cmd->u.set_viewport_with_count.viewports, pViewports, sizeof(VkViewport) * ceil(viewportCount));
   } else {
      cmd->u.set_viewport_with_count.viewports = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_scissor_with_count(struct vk_cmd_queue *queue
, uint32_t scissorCount
, const VkRect2D* pScissors
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SCISSOR_WITH_COUNT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SCISSOR_WITH_COUNT;
   cmd->u.set_scissor_with_count.scissor_count = scissorCount;
   if (pScissors) {
      cmd->u.set_scissor_with_count.scissors = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(scissorCount));
      if (cmd->u.set_scissor_with_count.scissors == NULL) return NULL;
      memcpy((void *)cmd->u.set_scissor_with_count.scissors, pScissors, sizeof(VkRect2D) * ceil(scissorCount));
   } else {
      cmd->u.set_scissor_with_count.scissors = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer2(struct vk_cmd_queue *queue
, VkBuffer buffer
, VkDeviceSize offset
, VkDeviceSize size
, VkIndexType indexType
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_INDEX_BUFFER2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_INDEX_BUFFER2;
   cmd->u.bind_index_buffer2.buffer = buffer;
   cmd->u.bind_index_buffer2.offset = offset;
   cmd->u.bind_index_buffer2.size = size;
   cmd->u.bind_index_buffer2.index_type = indexType;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers2(struct vk_cmd_queue *queue
, uint32_t firstBinding
, uint32_t bindingCount
, const VkBuffer* pBuffers
, const VkDeviceSize* pOffsets
, const VkDeviceSize* pSizes
, const VkDeviceSize* pStrides
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_VERTEX_BUFFERS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_VERTEX_BUFFERS2;
   cmd->u.bind_vertex_buffers2.first_binding = firstBinding;
   cmd->u.bind_vertex_buffers2.binding_count = bindingCount;
   if (pBuffers) {
      cmd->u.bind_vertex_buffers2.buffers = linear_alloc_child(queue->ctx, sizeof(VkBuffer) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers2.buffers == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers2.buffers, pBuffers, sizeof(VkBuffer) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers2.buffers = NULL;
   }
   if (pOffsets) {
      cmd->u.bind_vertex_buffers2.offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers2.offsets == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers2.offsets, pOffsets, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers2.offsets = NULL;
   }
   if (pSizes) {
      cmd->u.bind_vertex_buffers2.sizes = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers2.sizes == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers2.sizes, pSizes, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers2.sizes = NULL;
   }
   if (pStrides) {
      cmd->u.bind_vertex_buffers2.strides = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers2.strides == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers2.strides, pStrides, sizeof(VkDeviceSize) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers2.strides = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_test_enable(struct vk_cmd_queue *queue
, VkBool32 depthTestEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_TEST_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_TEST_ENABLE;
   cmd->u.set_depth_test_enable.depth_test_enable = depthTestEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_write_enable(struct vk_cmd_queue *queue
, VkBool32 depthWriteEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_WRITE_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_WRITE_ENABLE;
   cmd->u.set_depth_write_enable.depth_write_enable = depthWriteEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_compare_op(struct vk_cmd_queue *queue
, VkCompareOp depthCompareOp
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_COMPARE_OP]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_COMPARE_OP;
   cmd->u.set_depth_compare_op.depth_compare_op = depthCompareOp;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bounds_test_enable(struct vk_cmd_queue *queue
, VkBool32 depthBoundsTestEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE;
   cmd->u.set_depth_bounds_test_enable.depth_bounds_test_enable = depthBoundsTestEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_test_enable(struct vk_cmd_queue *queue
, VkBool32 stencilTestEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_STENCIL_TEST_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_STENCIL_TEST_ENABLE;
   cmd->u.set_stencil_test_enable.stencil_test_enable = stencilTestEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_op(struct vk_cmd_queue *queue
, VkStencilFaceFlags faceMask
, VkStencilOp failOp
, VkStencilOp passOp
, VkStencilOp depthFailOp
, VkCompareOp compareOp
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_STENCIL_OP]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_STENCIL_OP;
   cmd->u.set_stencil_op.face_mask = faceMask;
   cmd->u.set_stencil_op.fail_op = failOp;
   cmd->u.set_stencil_op.pass_op = passOp;
   cmd->u.set_stencil_op.depth_fail_op = depthFailOp;
   cmd->u.set_stencil_op.compare_op = compareOp;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_patch_control_points_ext(struct vk_cmd_queue *queue
, uint32_t patchControlPoints
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_PATCH_CONTROL_POINTS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_PATCH_CONTROL_POINTS_EXT;
   cmd->u.set_patch_control_points_ext.patch_control_points = patchControlPoints;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterizer_discard_enable(struct vk_cmd_queue *queue
, VkBool32 rasterizerDiscardEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RASTERIZER_DISCARD_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RASTERIZER_DISCARD_ENABLE;
   cmd->u.set_rasterizer_discard_enable.rasterizer_discard_enable = rasterizerDiscardEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias_enable(struct vk_cmd_queue *queue
, VkBool32 depthBiasEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_BIAS_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_BIAS_ENABLE;
   cmd->u.set_depth_bias_enable.depth_bias_enable = depthBiasEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_logic_op_ext(struct vk_cmd_queue *queue
, VkLogicOp logicOp
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LOGIC_OP_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LOGIC_OP_EXT;
   cmd->u.set_logic_op_ext.logic_op = logicOp;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_restart_enable(struct vk_cmd_queue *queue
, VkBool32 primitiveRestartEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_PRIMITIVE_RESTART_ENABLE]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_PRIMITIVE_RESTART_ENABLE;
   cmd->u.set_primitive_restart_enable.primitive_restart_enable = primitiveRestartEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_tessellation_domain_origin_ext(struct vk_cmd_queue *queue
, VkTessellationDomainOrigin domainOrigin
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_TESSELLATION_DOMAIN_ORIGIN_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_TESSELLATION_DOMAIN_ORIGIN_EXT;
   cmd->u.set_tessellation_domain_origin_ext.domain_origin = domainOrigin;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clamp_enable_ext(struct vk_cmd_queue *queue
, VkBool32 depthClampEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_CLAMP_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_CLAMP_ENABLE_EXT;
   cmd->u.set_depth_clamp_enable_ext.depth_clamp_enable = depthClampEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_polygon_mode_ext(struct vk_cmd_queue *queue
, VkPolygonMode polygonMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_POLYGON_MODE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_POLYGON_MODE_EXT;
   cmd->u.set_polygon_mode_ext.polygon_mode = polygonMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterization_samples_ext(struct vk_cmd_queue *queue
, VkSampleCountFlagBits  rasterizationSamples
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RASTERIZATION_SAMPLES_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RASTERIZATION_SAMPLES_EXT;
   cmd->u.set_rasterization_samples_ext.rasterization_samples = rasterizationSamples;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_mask_ext(struct vk_cmd_queue *queue
, VkSampleCountFlagBits  samples
, const VkSampleMask*    pSampleMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SAMPLE_MASK_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SAMPLE_MASK_EXT;
   cmd->u.set_sample_mask_ext.samples = samples;
   if (pSampleMask) {
      cmd->u.set_sample_mask_ext.sample_mask = linear_alloc_child(queue->ctx, sizeof(VkSampleMask) * ceil((samples + 31) / 32));
      if (cmd->u.set_sample_mask_ext.sample_mask == NULL) return NULL;
      memcpy((void *)cmd->u.set_sample_mask_ext.sample_mask, pSampleMask, sizeof(VkSampleMask) * ceil((samples + 31) / 32));
   } else {
      cmd->u.set_sample_mask_ext.sample_mask = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_alpha_to_coverage_enable_ext(struct vk_cmd_queue *queue
, VkBool32 alphaToCoverageEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_ALPHA_TO_COVERAGE_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_ALPHA_TO_COVERAGE_ENABLE_EXT;
   cmd->u.set_alpha_to_coverage_enable_ext.alpha_to_coverage_enable = alphaToCoverageEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_alpha_to_one_enable_ext(struct vk_cmd_queue *queue
, VkBool32 alphaToOneEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_ALPHA_TO_ONE_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_ALPHA_TO_ONE_ENABLE_EXT;
   cmd->u.set_alpha_to_one_enable_ext.alpha_to_one_enable = alphaToOneEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_logic_op_enable_ext(struct vk_cmd_queue *queue
, VkBool32 logicOpEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LOGIC_OP_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LOGIC_OP_ENABLE_EXT;
   cmd->u.set_logic_op_enable_ext.logic_op_enable = logicOpEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_enable_ext(struct vk_cmd_queue *queue
, uint32_t firstAttachment
, uint32_t attachmentCount
, const VkBool32* pColorBlendEnables
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COLOR_BLEND_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COLOR_BLEND_ENABLE_EXT;
   cmd->u.set_color_blend_enable_ext.first_attachment = firstAttachment;
   cmd->u.set_color_blend_enable_ext.attachment_count = attachmentCount;
   if (pColorBlendEnables) {
      cmd->u.set_color_blend_enable_ext.color_blend_enables = linear_alloc_child(queue->ctx, sizeof(VkBool32) * ceil(attachmentCount));
      if (cmd->u.set_color_blend_enable_ext.color_blend_enables == NULL) return NULL;
      memcpy((void *)cmd->u.set_color_blend_enable_ext.color_blend_enables, pColorBlendEnables, sizeof(VkBool32) * ceil(attachmentCount));
   } else {
      cmd->u.set_color_blend_enable_ext.color_blend_enables = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_equation_ext(struct vk_cmd_queue *queue
, uint32_t firstAttachment
, uint32_t attachmentCount
, const VkColorBlendEquationEXT* pColorBlendEquations
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COLOR_BLEND_EQUATION_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COLOR_BLEND_EQUATION_EXT;
   cmd->u.set_color_blend_equation_ext.first_attachment = firstAttachment;
   cmd->u.set_color_blend_equation_ext.attachment_count = attachmentCount;
   if (pColorBlendEquations) {
      cmd->u.set_color_blend_equation_ext.color_blend_equations = linear_alloc_child(queue->ctx, sizeof(VkColorBlendEquationEXT) * ceil(attachmentCount));
      if (cmd->u.set_color_blend_equation_ext.color_blend_equations == NULL) return NULL;
      memcpy((void *)cmd->u.set_color_blend_equation_ext.color_blend_equations, pColorBlendEquations, sizeof(VkColorBlendEquationEXT) * ceil(attachmentCount));
   } else {
      cmd->u.set_color_blend_equation_ext.color_blend_equations = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_write_mask_ext(struct vk_cmd_queue *queue
, uint32_t firstAttachment
, uint32_t attachmentCount
, const VkColorComponentFlags* pColorWriteMasks
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COLOR_WRITE_MASK_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COLOR_WRITE_MASK_EXT;
   cmd->u.set_color_write_mask_ext.first_attachment = firstAttachment;
   cmd->u.set_color_write_mask_ext.attachment_count = attachmentCount;
   if (pColorWriteMasks) {
      cmd->u.set_color_write_mask_ext.color_write_masks = linear_alloc_child(queue->ctx, sizeof(VkColorComponentFlags) * ceil(attachmentCount));
      if (cmd->u.set_color_write_mask_ext.color_write_masks == NULL) return NULL;
      memcpy((void *)cmd->u.set_color_write_mask_ext.color_write_masks, pColorWriteMasks, sizeof(VkColorComponentFlags) * ceil(attachmentCount));
   } else {
      cmd->u.set_color_write_mask_ext.color_write_masks = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterization_stream_ext(struct vk_cmd_queue *queue
, uint32_t rasterizationStream
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RASTERIZATION_STREAM_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RASTERIZATION_STREAM_EXT;
   cmd->u.set_rasterization_stream_ext.rasterization_stream = rasterizationStream;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_conservative_rasterization_mode_ext(struct vk_cmd_queue *queue
, VkConservativeRasterizationModeEXT conservativeRasterizationMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_CONSERVATIVE_RASTERIZATION_MODE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_CONSERVATIVE_RASTERIZATION_MODE_EXT;
   cmd->u.set_conservative_rasterization_mode_ext.conservative_rasterization_mode = conservativeRasterizationMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_extra_primitive_overestimation_size_ext(struct vk_cmd_queue *queue
, float extraPrimitiveOverestimationSize
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT;
   cmd->u.set_extra_primitive_overestimation_size_ext.extra_primitive_overestimation_size = extraPrimitiveOverestimationSize;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clip_enable_ext(struct vk_cmd_queue *queue
, VkBool32 depthClipEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_CLIP_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_CLIP_ENABLE_EXT;
   cmd->u.set_depth_clip_enable_ext.depth_clip_enable = depthClipEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_locations_enable_ext(struct vk_cmd_queue *queue
, VkBool32 sampleLocationsEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SAMPLE_LOCATIONS_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SAMPLE_LOCATIONS_ENABLE_EXT;
   cmd->u.set_sample_locations_enable_ext.sample_locations_enable = sampleLocationsEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_advanced_ext(struct vk_cmd_queue *queue
, uint32_t firstAttachment
, uint32_t attachmentCount
, const VkColorBlendAdvancedEXT* pColorBlendAdvanced
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COLOR_BLEND_ADVANCED_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COLOR_BLEND_ADVANCED_EXT;
   cmd->u.set_color_blend_advanced_ext.first_attachment = firstAttachment;
   cmd->u.set_color_blend_advanced_ext.attachment_count = attachmentCount;
   if (pColorBlendAdvanced) {
      cmd->u.set_color_blend_advanced_ext.color_blend_advanced = linear_alloc_child(queue->ctx, sizeof(VkColorBlendAdvancedEXT) * ceil(attachmentCount));
      if (cmd->u.set_color_blend_advanced_ext.color_blend_advanced == NULL) return NULL;
      memcpy((void *)cmd->u.set_color_blend_advanced_ext.color_blend_advanced, pColorBlendAdvanced, sizeof(VkColorBlendAdvancedEXT) * ceil(attachmentCount));
   } else {
      cmd->u.set_color_blend_advanced_ext.color_blend_advanced = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_provoking_vertex_mode_ext(struct vk_cmd_queue *queue
, VkProvokingVertexModeEXT provokingVertexMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_PROVOKING_VERTEX_MODE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_PROVOKING_VERTEX_MODE_EXT;
   cmd->u.set_provoking_vertex_mode_ext.provoking_vertex_mode = provokingVertexMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_rasterization_mode_ext(struct vk_cmd_queue *queue
, VkLineRasterizationModeEXT lineRasterizationMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LINE_RASTERIZATION_MODE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LINE_RASTERIZATION_MODE_EXT;
   cmd->u.set_line_rasterization_mode_ext.line_rasterization_mode = lineRasterizationMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_stipple_enable_ext(struct vk_cmd_queue *queue
, VkBool32 stippledLineEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_LINE_STIPPLE_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_LINE_STIPPLE_ENABLE_EXT;
   cmd->u.set_line_stipple_enable_ext.stippled_line_enable = stippledLineEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clip_negative_one_to_one_ext(struct vk_cmd_queue *queue
, VkBool32 negativeOneToOne
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT;
   cmd->u.set_depth_clip_negative_one_to_one_ext.negative_one_to_one = negativeOneToOne;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_wscaling_enable_nv(struct vk_cmd_queue *queue
, VkBool32 viewportWScalingEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT_WSCALING_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT_WSCALING_ENABLE_NV;
   cmd->u.set_viewport_wscaling_enable_nv.viewport_wscaling_enable = viewportWScalingEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_swizzle_nv(struct vk_cmd_queue *queue
, uint32_t firstViewport
, uint32_t viewportCount
, const VkViewportSwizzleNV* pViewportSwizzles
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VIEWPORT_SWIZZLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VIEWPORT_SWIZZLE_NV;
   cmd->u.set_viewport_swizzle_nv.first_viewport = firstViewport;
   cmd->u.set_viewport_swizzle_nv.viewport_count = viewportCount;
   if (pViewportSwizzles) {
      cmd->u.set_viewport_swizzle_nv.viewport_swizzles = linear_alloc_child(queue->ctx, sizeof(VkViewportSwizzleNV) * ceil(viewportCount));
      if (cmd->u.set_viewport_swizzle_nv.viewport_swizzles == NULL) return NULL;
      memcpy((void *)cmd->u.set_viewport_swizzle_nv.viewport_swizzles, pViewportSwizzles, sizeof(VkViewportSwizzleNV) * ceil(viewportCount));
   } else {
      cmd->u.set_viewport_swizzle_nv.viewport_swizzles = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_to_color_enable_nv(struct vk_cmd_queue *queue
, VkBool32 coverageToColorEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_TO_COLOR_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_TO_COLOR_ENABLE_NV;
   cmd->u.set_coverage_to_color_enable_nv.coverage_to_color_enable = coverageToColorEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_to_color_location_nv(struct vk_cmd_queue *queue
, uint32_t coverageToColorLocation
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_TO_COLOR_LOCATION_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_TO_COLOR_LOCATION_NV;
   cmd->u.set_coverage_to_color_location_nv.coverage_to_color_location = coverageToColorLocation;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_mode_nv(struct vk_cmd_queue *queue
, VkCoverageModulationModeNV coverageModulationMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_MODULATION_MODE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_MODULATION_MODE_NV;
   cmd->u.set_coverage_modulation_mode_nv.coverage_modulation_mode = coverageModulationMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_table_enable_nv(struct vk_cmd_queue *queue
, VkBool32 coverageModulationTableEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_MODULATION_TABLE_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_MODULATION_TABLE_ENABLE_NV;
   cmd->u.set_coverage_modulation_table_enable_nv.coverage_modulation_table_enable = coverageModulationTableEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_table_nv(struct vk_cmd_queue *queue
, uint32_t coverageModulationTableCount
, const float* pCoverageModulationTable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_MODULATION_TABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_MODULATION_TABLE_NV;
   cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table_count = coverageModulationTableCount;
   if (pCoverageModulationTable) {
      cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table = linear_alloc_child(queue->ctx, sizeof(float) * ceil(coverageModulationTableCount));
      if (cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table == NULL) return NULL;
      memcpy((void *)cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table, pCoverageModulationTable, sizeof(float) * ceil(coverageModulationTableCount));
   } else {
      cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_shading_rate_image_enable_nv(struct vk_cmd_queue *queue
, VkBool32 shadingRateImageEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_SHADING_RATE_IMAGE_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_SHADING_RATE_IMAGE_ENABLE_NV;
   cmd->u.set_shading_rate_image_enable_nv.shading_rate_image_enable = shadingRateImageEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_reduction_mode_nv(struct vk_cmd_queue *queue
, VkCoverageReductionModeNV coverageReductionMode
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COVERAGE_REDUCTION_MODE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COVERAGE_REDUCTION_MODE_NV;
   cmd->u.set_coverage_reduction_mode_nv.coverage_reduction_mode = coverageReductionMode;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_representative_fragment_test_enable_nv(struct vk_cmd_queue *queue
, VkBool32 representativeFragmentTestEnable
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV;
   cmd->u.set_representative_fragment_test_enable_nv.representative_fragment_test_enable = representativeFragmentTestEnable;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer2(struct vk_cmd_queue *queue
, const VkCopyBufferInfo2* pCopyBufferInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_BUFFER2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_BUFFER2;
   if (pCopyBufferInfo) {
      cmd->u.copy_buffer2.copy_buffer_info = linear_alloc_child(queue->ctx, sizeof(VkCopyBufferInfo2));
      if (cmd->u.copy_buffer2.copy_buffer_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_buffer2.copy_buffer_info, pCopyBufferInfo, sizeof(VkCopyBufferInfo2));
      VkCopyBufferInfo2 *tmp_dst1 = (void *)cmd->u.copy_buffer2.copy_buffer_info;
      VkCopyBufferInfo2 *tmp_src2 = (void *)pCopyBufferInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkBufferCopy2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkBufferCopy2) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.copy_buffer2.copy_buffer_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image2(struct vk_cmd_queue *queue
, const VkCopyImageInfo2* pCopyImageInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_IMAGE2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_IMAGE2;
   if (pCopyImageInfo) {
      cmd->u.copy_image2.copy_image_info = linear_alloc_child(queue->ctx, sizeof(VkCopyImageInfo2));
      if (cmd->u.copy_image2.copy_image_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_image2.copy_image_info, pCopyImageInfo, sizeof(VkCopyImageInfo2));
      VkCopyImageInfo2 *tmp_dst1 = (void *)cmd->u.copy_image2.copy_image_info;
      VkCopyImageInfo2 *tmp_src2 = (void *)pCopyImageInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkImageCopy2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkImageCopy2) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.copy_image2.copy_image_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_blit_image2(struct vk_cmd_queue *queue
, const VkBlitImageInfo2* pBlitImageInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BLIT_IMAGE2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BLIT_IMAGE2;
   if (pBlitImageInfo) {
      cmd->u.blit_image2.blit_image_info = linear_alloc_child(queue->ctx, sizeof(VkBlitImageInfo2));
      if (cmd->u.blit_image2.blit_image_info == NULL) return NULL;
      memcpy((void *)cmd->u.blit_image2.blit_image_info, pBlitImageInfo, sizeof(VkBlitImageInfo2));
      VkBlitImageInfo2 *tmp_dst1 = (void *)cmd->u.blit_image2.blit_image_info;
      VkBlitImageInfo2 *tmp_src2 = (void *)pBlitImageInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_BLIT_IMAGE_CUBIC_WEIGHTS_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkBlitImageCubicWeightsInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkBlitImageCubicWeightsInfoQCOM));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkImageBlit2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkImageBlit2) * ceil(tmp_src2->regionCount));
         VkImageBlit2 *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkImageBlit2 *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkImageBlit2 *tmp_dst6 = tmp_dst3 + i5;
            VkImageBlit2 *tmp_src7 = tmp_src4 + i5;
            const VkBaseInStructure *pnext = tmp_src7->pNext;
            void **dst_pnext_link = (void **)&tmp_dst6->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkCopyCommandTransformInfoQCOM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkCopyCommandTransformInfoQCOM));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.blit_image2.blit_image_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer_to_image2(struct vk_cmd_queue *queue
, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_BUFFER_TO_IMAGE2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_BUFFER_TO_IMAGE2;
   if (pCopyBufferToImageInfo) {
      cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info = linear_alloc_child(queue->ctx, sizeof(VkCopyBufferToImageInfo2));
      if (cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info, pCopyBufferToImageInfo, sizeof(VkCopyBufferToImageInfo2));
      VkCopyBufferToImageInfo2 *tmp_dst1 = (void *)cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info;
      VkCopyBufferToImageInfo2 *tmp_src2 = (void *)pCopyBufferToImageInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkBufferImageCopy2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkBufferImageCopy2) * ceil(tmp_src2->regionCount));
         VkBufferImageCopy2 *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkBufferImageCopy2 *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkBufferImageCopy2 *tmp_dst6 = tmp_dst3 + i5;
            VkBufferImageCopy2 *tmp_src7 = tmp_src4 + i5;
            const VkBaseInStructure *pnext = tmp_src7->pNext;
            void **dst_pnext_link = (void **)&tmp_dst6->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkCopyCommandTransformInfoQCOM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkCopyCommandTransformInfoQCOM));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_buffer2(struct vk_cmd_queue *queue
, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_IMAGE_TO_BUFFER2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_IMAGE_TO_BUFFER2;
   if (pCopyImageToBufferInfo) {
      cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info = linear_alloc_child(queue->ctx, sizeof(VkCopyImageToBufferInfo2));
      if (cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info, pCopyImageToBufferInfo, sizeof(VkCopyImageToBufferInfo2));
      VkCopyImageToBufferInfo2 *tmp_dst1 = (void *)cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info;
      VkCopyImageToBufferInfo2 *tmp_src2 = (void *)pCopyImageToBufferInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkBufferImageCopy2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkBufferImageCopy2) * ceil(tmp_src2->regionCount));
         VkBufferImageCopy2 *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkBufferImageCopy2 *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkBufferImageCopy2 *tmp_dst6 = tmp_dst3 + i5;
            VkBufferImageCopy2 *tmp_src7 = tmp_src4 + i5;
            const VkBaseInStructure *pnext = tmp_src7->pNext;
            void **dst_pnext_link = (void **)&tmp_dst6->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkCopyCommandTransformInfoQCOM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkCopyCommandTransformInfoQCOM));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_resolve_image2(struct vk_cmd_queue *queue
, const VkResolveImageInfo2* pResolveImageInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_RESOLVE_IMAGE2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_RESOLVE_IMAGE2;
   if (pResolveImageInfo) {
      cmd->u.resolve_image2.resolve_image_info = linear_alloc_child(queue->ctx, sizeof(VkResolveImageInfo2));
      if (cmd->u.resolve_image2.resolve_image_info == NULL) return NULL;
      memcpy((void *)cmd->u.resolve_image2.resolve_image_info, pResolveImageInfo, sizeof(VkResolveImageInfo2));
      VkResolveImageInfo2 *tmp_dst1 = (void *)cmd->u.resolve_image2.resolve_image_info;
      VkResolveImageInfo2 *tmp_src2 = (void *)pResolveImageInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_RESOLVE_IMAGE_MODE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkResolveImageModeInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkResolveImageModeInfoKHR));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkImageResolve2) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkImageResolve2) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.resolve_image2.resolve_image_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_fragment_shading_rate_khr(struct vk_cmd_queue *queue
, const VkExtent2D*                           pFragmentSize
, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_FRAGMENT_SHADING_RATE_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_FRAGMENT_SHADING_RATE_KHR;
   if (pFragmentSize) {
      cmd->u.set_fragment_shading_rate_khr.fragment_size = linear_alloc_child(queue->ctx, sizeof(VkExtent2D));
      if (cmd->u.set_fragment_shading_rate_khr.fragment_size == NULL) return NULL;
      memcpy((void *)cmd->u.set_fragment_shading_rate_khr.fragment_size, pFragmentSize, sizeof(VkExtent2D));
   } else {
      cmd->u.set_fragment_shading_rate_khr.fragment_size = NULL;
   }
   memcpy(cmd->u.set_fragment_shading_rate_khr.combiner_ops, combinerOps, sizeof(*combinerOps) * 2);

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_fragment_shading_rate_enum_nv(struct vk_cmd_queue *queue
, VkFragmentShadingRateNV                     shadingRate
, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_FRAGMENT_SHADING_RATE_ENUM_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_FRAGMENT_SHADING_RATE_ENUM_NV;
   cmd->u.set_fragment_shading_rate_enum_nv.shading_rate = shadingRate;
   memcpy(cmd->u.set_fragment_shading_rate_enum_nv.combiner_ops, combinerOps, sizeof(*combinerOps) * 2);

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_vertex_input_ext(struct vk_cmd_queue *queue
, uint32_t vertexBindingDescriptionCount
, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions
, uint32_t vertexAttributeDescriptionCount
, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_VERTEX_INPUT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_VERTEX_INPUT_EXT;
   cmd->u.set_vertex_input_ext.vertex_binding_description_count = vertexBindingDescriptionCount;
   if (pVertexBindingDescriptions) {
      cmd->u.set_vertex_input_ext.vertex_binding_descriptions = linear_alloc_child(queue->ctx, sizeof(VkVertexInputBindingDescription2EXT) * ceil(vertexBindingDescriptionCount));
      if (cmd->u.set_vertex_input_ext.vertex_binding_descriptions == NULL) return NULL;
      memcpy((void *)cmd->u.set_vertex_input_ext.vertex_binding_descriptions, pVertexBindingDescriptions, sizeof(VkVertexInputBindingDescription2EXT) * ceil(vertexBindingDescriptionCount));
   } else {
      cmd->u.set_vertex_input_ext.vertex_binding_descriptions = NULL;
   }
   cmd->u.set_vertex_input_ext.vertex_attribute_description_count = vertexAttributeDescriptionCount;
   if (pVertexAttributeDescriptions) {
      cmd->u.set_vertex_input_ext.vertex_attribute_descriptions = linear_alloc_child(queue->ctx, sizeof(VkVertexInputAttributeDescription2EXT) * ceil(vertexAttributeDescriptionCount));
      if (cmd->u.set_vertex_input_ext.vertex_attribute_descriptions == NULL) return NULL;
      memcpy((void *)cmd->u.set_vertex_input_ext.vertex_attribute_descriptions, pVertexAttributeDescriptions, sizeof(VkVertexInputAttributeDescription2EXT) * ceil(vertexAttributeDescriptionCount));
   } else {
      cmd->u.set_vertex_input_ext.vertex_attribute_descriptions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_write_enable_ext(struct vk_cmd_queue *queue
, uint32_t                                attachmentCount
, const VkBool32*   pColorWriteEnables
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COLOR_WRITE_ENABLE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COLOR_WRITE_ENABLE_EXT;
   cmd->u.set_color_write_enable_ext.attachment_count = attachmentCount;
   if (pColorWriteEnables) {
      cmd->u.set_color_write_enable_ext.color_write_enables = linear_alloc_child(queue->ctx, sizeof(VkBool32) * ceil(attachmentCount));
      if (cmd->u.set_color_write_enable_ext.color_write_enables == NULL) return NULL;
      memcpy((void *)cmd->u.set_color_write_enable_ext.color_write_enables, pColorWriteEnables, sizeof(VkBool32) * ceil(attachmentCount));
   } else {
      cmd->u.set_color_write_enable_ext.color_write_enables = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_event2(struct vk_cmd_queue *queue
, VkEvent                                             event
, const VkDependencyInfo*                             pDependencyInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_EVENT2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_EVENT2;
   cmd->u.set_event2.event = event;
   if (pDependencyInfo) {
      cmd->u.set_event2.dependency_info = linear_alloc_child(queue->ctx, sizeof(VkDependencyInfo));
      if (cmd->u.set_event2.dependency_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_event2.dependency_info, pDependencyInfo, sizeof(VkDependencyInfo));
      VkDependencyInfo *tmp_dst1 = (void *)cmd->u.set_event2.dependency_info;
      VkDependencyInfo *tmp_src2 = (void *)pDependencyInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_TENSOR_MEMORY_BARRIER_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorMemoryBarrierARM));
            break;
         case VK_STRUCTURE_TYPE_TENSOR_DEPENDENCY_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorDependencyInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorDependencyInfoARM));
            VkTensorDependencyInfoARM *tmp_dst3 = (void *)(*dst_pnext_link);
            VkTensorDependencyInfoARM *tmp_src4 = (void *)pnext;
            if (tmp_src4->pTensorMemoryBarriers) {
               tmp_dst3->pTensorMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src4->tensorMemoryBarrierCount));
               if (tmp_dst3->pTensorMemoryBarriers == NULL) return NULL;
               memcpy((void *)tmp_dst3->pTensorMemoryBarriers, tmp_src4->pTensorMemoryBarriers, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src4->tensorMemoryBarrierCount));
            }
            break;
         case VK_STRUCTURE_TYPE_MEMORY_RANGE_BARRIERS_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarriersInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryRangeBarriersInfoKHR));
            VkMemoryRangeBarriersInfoKHR *tmp_dst5 = (void *)(*dst_pnext_link);
            VkMemoryRangeBarriersInfoKHR *tmp_src6 = (void *)pnext;
            const VkBaseInStructure *pnext = tmp_src6->pNext;
            void **dst_pnext_link = (void **)&tmp_dst5->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            if (tmp_src6->pMemoryRangeBarriers) {
               tmp_dst5->pMemoryRangeBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src6->memoryRangeBarrierCount));
               if (tmp_dst5->pMemoryRangeBarriers == NULL) return NULL;
               memcpy((void *)tmp_dst5->pMemoryRangeBarriers, tmp_src6->pMemoryRangeBarriers, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src6->memoryRangeBarrierCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pMemoryBarriers) {
         tmp_dst1->pMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrier2) * ceil(tmp_src2->memoryBarrierCount));
         if (tmp_dst1->pMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pMemoryBarriers, tmp_src2->pMemoryBarriers, sizeof(VkMemoryBarrier2) * ceil(tmp_src2->memoryBarrierCount));
      }
      if (tmp_src2->pBufferMemoryBarriers) {
         tmp_dst1->pBufferMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src2->bufferMemoryBarrierCount));
         if (tmp_dst1->pBufferMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pBufferMemoryBarriers, tmp_src2->pBufferMemoryBarriers, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src2->bufferMemoryBarrierCount));
         VkBufferMemoryBarrier2 *tmp_dst7 = (void *)tmp_dst1->pBufferMemoryBarriers;
         VkBufferMemoryBarrier2 *tmp_src8 = (void *)tmp_src2->pBufferMemoryBarriers;
         for (uint32_t i9 = 0; i9 < tmp_src2->bufferMemoryBarrierCount; i9++) {
            VkBufferMemoryBarrier2 *tmp_dst10 = tmp_dst7 + i9;
            VkBufferMemoryBarrier2 *tmp_src11 = tmp_src8 + i9;
            const VkBaseInStructure *pnext = tmp_src11->pNext;
            void **dst_pnext_link = (void **)&tmp_dst10->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
      if (tmp_src2->pImageMemoryBarriers) {
         tmp_dst1->pImageMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src2->imageMemoryBarrierCount));
         if (tmp_dst1->pImageMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pImageMemoryBarriers, tmp_src2->pImageMemoryBarriers, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src2->imageMemoryBarrierCount));
         VkImageMemoryBarrier2 *tmp_dst12 = (void *)tmp_dst1->pImageMemoryBarriers;
         VkImageMemoryBarrier2 *tmp_src13 = (void *)tmp_src2->pImageMemoryBarriers;
         for (uint32_t i14 = 0; i14 < tmp_src2->imageMemoryBarrierCount; i14++) {
            VkImageMemoryBarrier2 *tmp_dst15 = tmp_dst12 + i14;
            VkImageMemoryBarrier2 *tmp_src16 = tmp_src13 + i14;
            const VkBaseInStructure *pnext = tmp_src16->pNext;
            void **dst_pnext_link = (void **)&tmp_dst15->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
                  VkSampleLocationsInfoEXT *tmp_dst17 = (void *)(*dst_pnext_link);
                  VkSampleLocationsInfoEXT *tmp_src18 = (void *)pnext;
                  if (tmp_src18->pSampleLocations) {
                     tmp_dst17->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src18->sampleLocationsCount));
                     if (tmp_dst17->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst17->pSampleLocations, tmp_src18->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src18->sampleLocationsCount));
                  }
                  break;
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.set_event2.dependency_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_event2(struct vk_cmd_queue *queue
, VkEvent                                             event
, VkPipelineStageFlags2               stageMask
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_RESET_EVENT2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_RESET_EVENT2;
   cmd->u.reset_event2.event = event;
   cmd->u.reset_event2.stage_mask = stageMask;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_wait_events2(struct vk_cmd_queue *queue
, uint32_t                                            eventCount
, const VkEvent*                     pEvents
, const VkDependencyInfo*            pDependencyInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WAIT_EVENTS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WAIT_EVENTS2;
   cmd->u.wait_events2.event_count = eventCount;
   if (pEvents) {
      cmd->u.wait_events2.events = linear_alloc_child(queue->ctx, sizeof(VkEvent) * ceil(eventCount));
      if (cmd->u.wait_events2.events == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events2.events, pEvents, sizeof(VkEvent) * ceil(eventCount));
   } else {
      cmd->u.wait_events2.events = NULL;
   }
   if (pDependencyInfos) {
      cmd->u.wait_events2.dependency_infos = linear_alloc_child(queue->ctx, sizeof(VkDependencyInfo) * ceil(eventCount));
      if (cmd->u.wait_events2.dependency_infos == NULL) return NULL;
      memcpy((void *)cmd->u.wait_events2.dependency_infos, pDependencyInfos, sizeof(VkDependencyInfo) * ceil(eventCount));
      VkDependencyInfo *tmp_dst1 = (void *)cmd->u.wait_events2.dependency_infos;
      VkDependencyInfo *tmp_src2 = (void *)pDependencyInfos;
      for (uint32_t i3 = 0; i3 < eventCount; i3++) {
         VkDependencyInfo *tmp_dst4 = tmp_dst1 + i3;
         VkDependencyInfo *tmp_src5 = tmp_src2 + i3;
         const VkBaseInStructure *pnext = tmp_src5->pNext;
         void **dst_pnext_link = (void **)&tmp_dst4->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_TENSOR_MEMORY_BARRIER_ARM:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorMemoryBarrierARM));
               break;
            case VK_STRUCTURE_TYPE_TENSOR_DEPENDENCY_INFO_ARM:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorDependencyInfoARM));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorDependencyInfoARM));
               VkTensorDependencyInfoARM *tmp_dst6 = (void *)(*dst_pnext_link);
               VkTensorDependencyInfoARM *tmp_src7 = (void *)pnext;
               if (tmp_src7->pTensorMemoryBarriers) {
                  tmp_dst6->pTensorMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src7->tensorMemoryBarrierCount));
                  if (tmp_dst6->pTensorMemoryBarriers == NULL) return NULL;
                  memcpy((void *)tmp_dst6->pTensorMemoryBarriers, tmp_src7->pTensorMemoryBarriers, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src7->tensorMemoryBarrierCount));
               }
               break;
            case VK_STRUCTURE_TYPE_MEMORY_RANGE_BARRIERS_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarriersInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryRangeBarriersInfoKHR));
               VkMemoryRangeBarriersInfoKHR *tmp_dst8 = (void *)(*dst_pnext_link);
               VkMemoryRangeBarriersInfoKHR *tmp_src9 = (void *)pnext;
               const VkBaseInStructure *pnext = tmp_src9->pNext;
               void **dst_pnext_link = (void **)&tmp_dst8->pNext;
               while (pnext) {
                  switch ((int32_t)pnext->sType) {
                  case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     break;
                  }
                  pnext = pnext->pNext;
                  dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
               }
               if (tmp_src9->pMemoryRangeBarriers) {
                  tmp_dst8->pMemoryRangeBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src9->memoryRangeBarrierCount));
                  if (tmp_dst8->pMemoryRangeBarriers == NULL) return NULL;
                  memcpy((void *)tmp_dst8->pMemoryRangeBarriers, tmp_src9->pMemoryRangeBarriers, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src9->memoryRangeBarrierCount));
               }
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
         if (tmp_src5->pMemoryBarriers) {
            tmp_dst4->pMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrier2) * ceil(tmp_src5->memoryBarrierCount));
            if (tmp_dst4->pMemoryBarriers == NULL) return NULL;
            memcpy((void *)tmp_dst4->pMemoryBarriers, tmp_src5->pMemoryBarriers, sizeof(VkMemoryBarrier2) * ceil(tmp_src5->memoryBarrierCount));
         }
         if (tmp_src5->pBufferMemoryBarriers) {
            tmp_dst4->pBufferMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src5->bufferMemoryBarrierCount));
            if (tmp_dst4->pBufferMemoryBarriers == NULL) return NULL;
            memcpy((void *)tmp_dst4->pBufferMemoryBarriers, tmp_src5->pBufferMemoryBarriers, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src5->bufferMemoryBarrierCount));
            VkBufferMemoryBarrier2 *tmp_dst10 = (void *)tmp_dst4->pBufferMemoryBarriers;
            VkBufferMemoryBarrier2 *tmp_src11 = (void *)tmp_src5->pBufferMemoryBarriers;
            for (uint32_t i12 = 0; i12 < tmp_src5->bufferMemoryBarrierCount; i12++) {
               VkBufferMemoryBarrier2 *tmp_dst13 = tmp_dst10 + i12;
               VkBufferMemoryBarrier2 *tmp_src14 = tmp_src11 + i12;
               const VkBaseInStructure *pnext = tmp_src14->pNext;
               void **dst_pnext_link = (void **)&tmp_dst13->pNext;
               while (pnext) {
                  switch ((int32_t)pnext->sType) {
                  case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     break;
                  case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                     break;
                  }
                  pnext = pnext->pNext;
                  dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
               }
            }
         }
         if (tmp_src5->pImageMemoryBarriers) {
            tmp_dst4->pImageMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src5->imageMemoryBarrierCount));
            if (tmp_dst4->pImageMemoryBarriers == NULL) return NULL;
            memcpy((void *)tmp_dst4->pImageMemoryBarriers, tmp_src5->pImageMemoryBarriers, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src5->imageMemoryBarrierCount));
            VkImageMemoryBarrier2 *tmp_dst15 = (void *)tmp_dst4->pImageMemoryBarriers;
            VkImageMemoryBarrier2 *tmp_src16 = (void *)tmp_src5->pImageMemoryBarriers;
            for (uint32_t i17 = 0; i17 < tmp_src5->imageMemoryBarrierCount; i17++) {
               VkImageMemoryBarrier2 *tmp_dst18 = tmp_dst15 + i17;
               VkImageMemoryBarrier2 *tmp_src19 = tmp_src16 + i17;
               const VkBaseInStructure *pnext = tmp_src19->pNext;
               void **dst_pnext_link = (void **)&tmp_dst18->pNext;
               while (pnext) {
                  switch ((int32_t)pnext->sType) {
                  case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
                     VkSampleLocationsInfoEXT *tmp_dst20 = (void *)(*dst_pnext_link);
                     VkSampleLocationsInfoEXT *tmp_src21 = (void *)pnext;
                     if (tmp_src21->pSampleLocations) {
                        tmp_dst20->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src21->sampleLocationsCount));
                        if (tmp_dst20->pSampleLocations == NULL) return NULL;
                        memcpy((void *)tmp_dst20->pSampleLocations, tmp_src21->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src21->sampleLocationsCount));
                     }
                     break;
                  case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                     break;
                  case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                     (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                     if ((*dst_pnext_link) == NULL) return NULL;
                     memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                     break;
                  }
                  pnext = pnext->pNext;
                  dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
               }
            }
         }
      }
   } else {
      cmd->u.wait_events2.dependency_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_pipeline_barrier2(struct vk_cmd_queue *queue
, const VkDependencyInfo*                             pDependencyInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PIPELINE_BARRIER2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PIPELINE_BARRIER2;
   if (pDependencyInfo) {
      cmd->u.pipeline_barrier2.dependency_info = linear_alloc_child(queue->ctx, sizeof(VkDependencyInfo));
      if (cmd->u.pipeline_barrier2.dependency_info == NULL) return NULL;
      memcpy((void *)cmd->u.pipeline_barrier2.dependency_info, pDependencyInfo, sizeof(VkDependencyInfo));
      VkDependencyInfo *tmp_dst1 = (void *)cmd->u.pipeline_barrier2.dependency_info;
      VkDependencyInfo *tmp_src2 = (void *)pDependencyInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_TENSOR_MEMORY_BARRIER_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorMemoryBarrierARM));
            break;
         case VK_STRUCTURE_TYPE_TENSOR_DEPENDENCY_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTensorDependencyInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTensorDependencyInfoARM));
            VkTensorDependencyInfoARM *tmp_dst3 = (void *)(*dst_pnext_link);
            VkTensorDependencyInfoARM *tmp_src4 = (void *)pnext;
            if (tmp_src4->pTensorMemoryBarriers) {
               tmp_dst3->pTensorMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src4->tensorMemoryBarrierCount));
               if (tmp_dst3->pTensorMemoryBarriers == NULL) return NULL;
               memcpy((void *)tmp_dst3->pTensorMemoryBarriers, tmp_src4->pTensorMemoryBarriers, sizeof(VkTensorMemoryBarrierARM) * ceil(tmp_src4->tensorMemoryBarrierCount));
            }
            break;
         case VK_STRUCTURE_TYPE_MEMORY_RANGE_BARRIERS_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarriersInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryRangeBarriersInfoKHR));
            VkMemoryRangeBarriersInfoKHR *tmp_dst5 = (void *)(*dst_pnext_link);
            VkMemoryRangeBarriersInfoKHR *tmp_src6 = (void *)pnext;
            const VkBaseInStructure *pnext = tmp_src6->pNext;
            void **dst_pnext_link = (void **)&tmp_dst5->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            if (tmp_src6->pMemoryRangeBarriers) {
               tmp_dst5->pMemoryRangeBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src6->memoryRangeBarrierCount));
               if (tmp_dst5->pMemoryRangeBarriers == NULL) return NULL;
               memcpy((void *)tmp_dst5->pMemoryRangeBarriers, tmp_src6->pMemoryRangeBarriers, sizeof(VkMemoryRangeBarrierKHR) * ceil(tmp_src6->memoryRangeBarrierCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pMemoryBarriers) {
         tmp_dst1->pMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrier2) * ceil(tmp_src2->memoryBarrierCount));
         if (tmp_dst1->pMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pMemoryBarriers, tmp_src2->pMemoryBarriers, sizeof(VkMemoryBarrier2) * ceil(tmp_src2->memoryBarrierCount));
      }
      if (tmp_src2->pBufferMemoryBarriers) {
         tmp_dst1->pBufferMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src2->bufferMemoryBarrierCount));
         if (tmp_dst1->pBufferMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pBufferMemoryBarriers, tmp_src2->pBufferMemoryBarriers, sizeof(VkBufferMemoryBarrier2) * ceil(tmp_src2->bufferMemoryBarrierCount));
         VkBufferMemoryBarrier2 *tmp_dst7 = (void *)tmp_dst1->pBufferMemoryBarriers;
         VkBufferMemoryBarrier2 *tmp_src8 = (void *)tmp_src2->pBufferMemoryBarriers;
         for (uint32_t i9 = 0; i9 < tmp_src2->bufferMemoryBarrierCount; i9++) {
            VkBufferMemoryBarrier2 *tmp_dst10 = tmp_dst7 + i9;
            VkBufferMemoryBarrier2 *tmp_src11 = tmp_src8 + i9;
            const VkBaseInStructure *pnext = tmp_src11->pNext;
            void **dst_pnext_link = (void **)&tmp_dst10->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
      if (tmp_src2->pImageMemoryBarriers) {
         tmp_dst1->pImageMemoryBarriers = linear_alloc_child(queue->ctx, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src2->imageMemoryBarrierCount));
         if (tmp_dst1->pImageMemoryBarriers == NULL) return NULL;
         memcpy((void *)tmp_dst1->pImageMemoryBarriers, tmp_src2->pImageMemoryBarriers, sizeof(VkImageMemoryBarrier2) * ceil(tmp_src2->imageMemoryBarrierCount));
         VkImageMemoryBarrier2 *tmp_dst12 = (void *)tmp_dst1->pImageMemoryBarriers;
         VkImageMemoryBarrier2 *tmp_src13 = (void *)tmp_src2->pImageMemoryBarriers;
         for (uint32_t i14 = 0; i14 < tmp_src2->imageMemoryBarrierCount; i14++) {
            VkImageMemoryBarrier2 *tmp_dst15 = tmp_dst12 + i14;
            VkImageMemoryBarrier2 *tmp_src16 = tmp_src13 + i14;
            const VkBaseInStructure *pnext = tmp_src16->pNext;
            void **dst_pnext_link = (void **)&tmp_dst15->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
                  VkSampleLocationsInfoEXT *tmp_dst17 = (void *)(*dst_pnext_link);
                  VkSampleLocationsInfoEXT *tmp_src18 = (void *)pnext;
                  if (tmp_src18->pSampleLocations) {
                     tmp_dst17->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src18->sampleLocationsCount));
                     if (tmp_dst17->pSampleLocations == NULL) return NULL;
                     memcpy((void *)tmp_dst17->pSampleLocations, tmp_src18->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src18->sampleLocationsCount));
                  }
                  break;
               case VK_STRUCTURE_TYPE_MEMORY_BARRIER_ACCESS_FLAGS_3_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMemoryBarrierAccessFlags3KHR));
                  break;
               case VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_ACQUIRE_UNMODIFIED_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkExternalMemoryAcquireUnmodifiedEXT));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.pipeline_barrier2.dependency_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_timestamp2(struct vk_cmd_queue *queue
, VkPipelineStageFlags2               stage
, VkQueryPool                                         queryPool
, uint32_t                                            query
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_TIMESTAMP2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_TIMESTAMP2;
   cmd->u.write_timestamp2.stage = stage;
   cmd->u.write_timestamp2.query_pool = queryPool;
   cmd->u.write_timestamp2.query = query;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_buffer_marker2_amd(struct vk_cmd_queue *queue
, VkPipelineStageFlags2               stage
, VkBuffer                                            dstBuffer
, VkDeviceSize                                        dstOffset
, uint32_t                                            marker
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_BUFFER_MARKER2_AMD]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_BUFFER_MARKER2_AMD;
   cmd->u.write_buffer_marker2_amd.stage = stage;
   cmd->u.write_buffer_marker2_amd.dst_buffer = dstBuffer;
   cmd->u.write_buffer_marker2_amd.dst_offset = dstOffset;
   cmd->u.write_buffer_marker2_amd.marker = marker;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_decode_video_khr(struct vk_cmd_queue *queue
, const VkVideoDecodeInfoKHR* pDecodeInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DECODE_VIDEO_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DECODE_VIDEO_KHR;
   if (pDecodeInfo) {
      cmd->u.decode_video_khr.decode_info = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeInfoKHR));
      if (cmd->u.decode_video_khr.decode_info == NULL) return NULL;
      memcpy((void *)cmd->u.decode_video_khr.decode_info, pDecodeInfo, sizeof(VkVideoDecodeInfoKHR));
      VkVideoDecodeInfoKHR *tmp_dst1 = (void *)cmd->u.decode_video_khr.decode_info;
      VkVideoDecodeInfoKHR *tmp_src2 = (void *)pDecodeInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_VIDEO_INLINE_QUERY_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoInlineQueryInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoInlineQueryInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_INLINE_SESSION_PARAMETERS_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264InlineSessionParametersInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264InlineSessionParametersInfoKHR));
            VkVideoDecodeH264InlineSessionParametersInfoKHR *tmp_dst3 = (void *)(*dst_pnext_link);
            VkVideoDecodeH264InlineSessionParametersInfoKHR *tmp_src4 = (void *)pnext;
            if (tmp_src4->pStdSPS) {
               tmp_dst3->pStdSPS = linear_alloc_child(queue->ctx, sizeof(StdVideoH264SequenceParameterSet));
               if (tmp_dst3->pStdSPS == NULL) return NULL;
               memcpy((void *)tmp_dst3->pStdSPS, tmp_src4->pStdSPS, sizeof(StdVideoH264SequenceParameterSet));
            }
            if (tmp_src4->pStdPPS) {
               tmp_dst3->pStdPPS = linear_alloc_child(queue->ctx, sizeof(StdVideoH264PictureParameterSet));
               if (tmp_dst3->pStdPPS == NULL) return NULL;
               memcpy((void *)tmp_dst3->pStdPPS, tmp_src4->pStdPPS, sizeof(StdVideoH264PictureParameterSet));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264PictureInfoKHR));
            VkVideoDecodeH264PictureInfoKHR *tmp_dst5 = (void *)(*dst_pnext_link);
            VkVideoDecodeH264PictureInfoKHR *tmp_src6 = (void *)pnext;
            if (tmp_src6->pStdPictureInfo) {
               tmp_dst5->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264PictureInfo));
               if (tmp_dst5->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst5->pStdPictureInfo, tmp_src6->pStdPictureInfo, sizeof(StdVideoDecodeH264PictureInfo));
            }
            if (tmp_src6->pSliceOffsets) {
               tmp_dst5->pSliceOffsets = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src6->sliceCount));
               if (tmp_dst5->pSliceOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst5->pSliceOffsets, tmp_src6->pSliceOffsets, sizeof(uint32_t) * ceil(tmp_src6->sliceCount));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_INLINE_SESSION_PARAMETERS_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265InlineSessionParametersInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265InlineSessionParametersInfoKHR));
            VkVideoDecodeH265InlineSessionParametersInfoKHR *tmp_dst7 = (void *)(*dst_pnext_link);
            VkVideoDecodeH265InlineSessionParametersInfoKHR *tmp_src8 = (void *)pnext;
            if (tmp_src8->pStdVPS) {
               tmp_dst7->pStdVPS = linear_alloc_child(queue->ctx, sizeof(StdVideoH265VideoParameterSet));
               if (tmp_dst7->pStdVPS == NULL) return NULL;
               memcpy((void *)tmp_dst7->pStdVPS, tmp_src8->pStdVPS, sizeof(StdVideoH265VideoParameterSet));
            }
            if (tmp_src8->pStdSPS) {
               tmp_dst7->pStdSPS = linear_alloc_child(queue->ctx, sizeof(StdVideoH265SequenceParameterSet));
               if (tmp_dst7->pStdSPS == NULL) return NULL;
               memcpy((void *)tmp_dst7->pStdSPS, tmp_src8->pStdSPS, sizeof(StdVideoH265SequenceParameterSet));
            }
            if (tmp_src8->pStdPPS) {
               tmp_dst7->pStdPPS = linear_alloc_child(queue->ctx, sizeof(StdVideoH265PictureParameterSet));
               if (tmp_dst7->pStdPPS == NULL) return NULL;
               memcpy((void *)tmp_dst7->pStdPPS, tmp_src8->pStdPPS, sizeof(StdVideoH265PictureParameterSet));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265PictureInfoKHR));
            VkVideoDecodeH265PictureInfoKHR *tmp_dst9 = (void *)(*dst_pnext_link);
            VkVideoDecodeH265PictureInfoKHR *tmp_src10 = (void *)pnext;
            if (tmp_src10->pStdPictureInfo) {
               tmp_dst9->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265PictureInfo));
               if (tmp_dst9->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst9->pStdPictureInfo, tmp_src10->pStdPictureInfo, sizeof(StdVideoDecodeH265PictureInfo));
            }
            if (tmp_src10->pSliceSegmentOffsets) {
               tmp_dst9->pSliceSegmentOffsets = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src10->sliceSegmentCount));
               if (tmp_dst9->pSliceSegmentOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst9->pSliceSegmentOffsets, tmp_src10->pSliceSegmentOffsets, sizeof(uint32_t) * ceil(tmp_src10->sliceSegmentCount));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_VP9_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeVP9PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeVP9PictureInfoKHR));
            VkVideoDecodeVP9PictureInfoKHR *tmp_dst11 = (void *)(*dst_pnext_link);
            VkVideoDecodeVP9PictureInfoKHR *tmp_src12 = (void *)pnext;
            if (tmp_src12->pStdPictureInfo) {
               tmp_dst11->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeVP9PictureInfo));
               if (tmp_dst11->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst11->pStdPictureInfo, tmp_src12->pStdPictureInfo, sizeof(StdVideoDecodeVP9PictureInfo));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_INLINE_SESSION_PARAMETERS_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1InlineSessionParametersInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1InlineSessionParametersInfoKHR));
            VkVideoDecodeAV1InlineSessionParametersInfoKHR *tmp_dst13 = (void *)(*dst_pnext_link);
            VkVideoDecodeAV1InlineSessionParametersInfoKHR *tmp_src14 = (void *)pnext;
            if (tmp_src14->pStdSequenceHeader) {
               tmp_dst13->pStdSequenceHeader = linear_alloc_child(queue->ctx, sizeof(StdVideoAV1SequenceHeader));
               if (tmp_dst13->pStdSequenceHeader == NULL) return NULL;
               memcpy((void *)tmp_dst13->pStdSequenceHeader, tmp_src14->pStdSequenceHeader, sizeof(StdVideoAV1SequenceHeader));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1PictureInfoKHR));
            VkVideoDecodeAV1PictureInfoKHR *tmp_dst15 = (void *)(*dst_pnext_link);
            VkVideoDecodeAV1PictureInfoKHR *tmp_src16 = (void *)pnext;
            if (tmp_src16->pStdPictureInfo) {
               tmp_dst15->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1PictureInfo));
               if (tmp_dst15->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst15->pStdPictureInfo, tmp_src16->pStdPictureInfo, sizeof(StdVideoDecodeAV1PictureInfo));
            }
            if (tmp_src16->pTileOffsets) {
               tmp_dst15->pTileOffsets = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src16->tileCount));
               if (tmp_dst15->pTileOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst15->pTileOffsets, tmp_src16->pTileOffsets, sizeof(uint32_t) * ceil(tmp_src16->tileCount));
            }
            if (tmp_src16->pTileSizes) {
               tmp_dst15->pTileSizes = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src16->tileCount));
               if (tmp_dst15->pTileSizes == NULL) return NULL;
               memcpy((void *)tmp_dst15->pTileSizes, tmp_src16->pTileSizes, sizeof(uint32_t) * ceil(tmp_src16->tileCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pSetupReferenceSlot) {
         tmp_dst1->pSetupReferenceSlot = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceSlotInfoKHR));
         if (tmp_dst1->pSetupReferenceSlot == NULL) return NULL;
         memcpy((void *)tmp_dst1->pSetupReferenceSlot, tmp_src2->pSetupReferenceSlot, sizeof(VkVideoReferenceSlotInfoKHR));
         VkVideoReferenceSlotInfoKHR *tmp_dst17 = (void *)tmp_dst1->pSetupReferenceSlot;
         VkVideoReferenceSlotInfoKHR *tmp_src18 = (void *)tmp_src2->pSetupReferenceSlot;
         const VkBaseInStructure *pnext = tmp_src18->pNext;
         void **dst_pnext_link = (void **)&tmp_dst17->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
               VkVideoDecodeH264DpbSlotInfoKHR *tmp_dst19 = (void *)(*dst_pnext_link);
               VkVideoDecodeH264DpbSlotInfoKHR *tmp_src20 = (void *)pnext;
               if (tmp_src20->pStdReferenceInfo) {
                  tmp_dst19->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264ReferenceInfo));
                  if (tmp_dst19->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst19->pStdReferenceInfo, tmp_src20->pStdReferenceInfo, sizeof(StdVideoDecodeH264ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
               VkVideoDecodeH265DpbSlotInfoKHR *tmp_dst21 = (void *)(*dst_pnext_link);
               VkVideoDecodeH265DpbSlotInfoKHR *tmp_src22 = (void *)pnext;
               if (tmp_src22->pStdReferenceInfo) {
                  tmp_dst21->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265ReferenceInfo));
                  if (tmp_dst21->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst21->pStdReferenceInfo, tmp_src22->pStdReferenceInfo, sizeof(StdVideoDecodeH265ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
               VkVideoDecodeAV1DpbSlotInfoKHR *tmp_dst23 = (void *)(*dst_pnext_link);
               VkVideoDecodeAV1DpbSlotInfoKHR *tmp_src24 = (void *)pnext;
               if (tmp_src24->pStdReferenceInfo) {
                  tmp_dst23->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1ReferenceInfo));
                  if (tmp_dst23->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst23->pStdReferenceInfo, tmp_src24->pStdReferenceInfo, sizeof(StdVideoDecodeAV1ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
               VkVideoEncodeH264DpbSlotInfoKHR *tmp_dst25 = (void *)(*dst_pnext_link);
               VkVideoEncodeH264DpbSlotInfoKHR *tmp_src26 = (void *)pnext;
               if (tmp_src26->pStdReferenceInfo) {
                  tmp_dst25->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264ReferenceInfo));
                  if (tmp_dst25->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst25->pStdReferenceInfo, tmp_src26->pStdReferenceInfo, sizeof(StdVideoEncodeH264ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
               VkVideoEncodeH265DpbSlotInfoKHR *tmp_dst27 = (void *)(*dst_pnext_link);
               VkVideoEncodeH265DpbSlotInfoKHR *tmp_src28 = (void *)pnext;
               if (tmp_src28->pStdReferenceInfo) {
                  tmp_dst27->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265ReferenceInfo));
                  if (tmp_dst27->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst27->pStdReferenceInfo, tmp_src28->pStdReferenceInfo, sizeof(StdVideoEncodeH265ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
               VkVideoEncodeAV1DpbSlotInfoKHR *tmp_dst29 = (void *)(*dst_pnext_link);
               VkVideoEncodeAV1DpbSlotInfoKHR *tmp_src30 = (void *)pnext;
               if (tmp_src30->pStdReferenceInfo) {
                  tmp_dst29->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1ReferenceInfo));
                  if (tmp_dst29->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst29->pStdReferenceInfo, tmp_src30->pStdReferenceInfo, sizeof(StdVideoEncodeAV1ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_REFERENCE_INTRA_REFRESH_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
         if (tmp_src18->pPictureResource) {
            tmp_dst17->pPictureResource = linear_alloc_child(queue->ctx, sizeof(VkVideoPictureResourceInfoKHR));
            if (tmp_dst17->pPictureResource == NULL) return NULL;
            memcpy((void *)tmp_dst17->pPictureResource, tmp_src18->pPictureResource, sizeof(VkVideoPictureResourceInfoKHR));
         }
      }
      if (tmp_src2->pReferenceSlots) {
         tmp_dst1->pReferenceSlots = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         if (tmp_dst1->pReferenceSlots == NULL) return NULL;
         memcpy((void *)tmp_dst1->pReferenceSlots, tmp_src2->pReferenceSlots, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         VkVideoReferenceSlotInfoKHR *tmp_dst31 = (void *)tmp_dst1->pReferenceSlots;
         VkVideoReferenceSlotInfoKHR *tmp_src32 = (void *)tmp_src2->pReferenceSlots;
         for (uint32_t i33 = 0; i33 < tmp_src2->referenceSlotCount; i33++) {
            VkVideoReferenceSlotInfoKHR *tmp_dst34 = tmp_dst31 + i33;
            VkVideoReferenceSlotInfoKHR *tmp_src35 = tmp_src32 + i33;
            const VkBaseInStructure *pnext = tmp_src35->pNext;
            void **dst_pnext_link = (void **)&tmp_dst34->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_dst36 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_src37 = (void *)pnext;
                  if (tmp_src37->pStdReferenceInfo) {
                     tmp_dst36->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264ReferenceInfo));
                     if (tmp_dst36->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst36->pStdReferenceInfo, tmp_src37->pStdReferenceInfo, sizeof(StdVideoDecodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_dst38 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_src39 = (void *)pnext;
                  if (tmp_src39->pStdReferenceInfo) {
                     tmp_dst38->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265ReferenceInfo));
                     if (tmp_dst38->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst38->pStdReferenceInfo, tmp_src39->pStdReferenceInfo, sizeof(StdVideoDecodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_dst40 = (void *)(*dst_pnext_link);
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_src41 = (void *)pnext;
                  if (tmp_src41->pStdReferenceInfo) {
                     tmp_dst40->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1ReferenceInfo));
                     if (tmp_dst40->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst40->pStdReferenceInfo, tmp_src41->pStdReferenceInfo, sizeof(StdVideoDecodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_dst42 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_src43 = (void *)pnext;
                  if (tmp_src43->pStdReferenceInfo) {
                     tmp_dst42->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264ReferenceInfo));
                     if (tmp_dst42->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst42->pStdReferenceInfo, tmp_src43->pStdReferenceInfo, sizeof(StdVideoEncodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_dst44 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_src45 = (void *)pnext;
                  if (tmp_src45->pStdReferenceInfo) {
                     tmp_dst44->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265ReferenceInfo));
                     if (tmp_dst44->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst44->pStdReferenceInfo, tmp_src45->pStdReferenceInfo, sizeof(StdVideoEncodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_dst46 = (void *)(*dst_pnext_link);
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_src47 = (void *)pnext;
                  if (tmp_src47->pStdReferenceInfo) {
                     tmp_dst46->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1ReferenceInfo));
                     if (tmp_dst46->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst46->pStdReferenceInfo, tmp_src47->pStdReferenceInfo, sizeof(StdVideoEncodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_REFERENCE_INTRA_REFRESH_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            if (tmp_src35->pPictureResource) {
               tmp_dst34->pPictureResource = linear_alloc_child(queue->ctx, sizeof(VkVideoPictureResourceInfoKHR));
               if (tmp_dst34->pPictureResource == NULL) return NULL;
               memcpy((void *)tmp_dst34->pPictureResource, tmp_src35->pPictureResource, sizeof(VkVideoPictureResourceInfoKHR));
            }
         }
      }
   } else {
      cmd->u.decode_video_khr.decode_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_video_coding_khr(struct vk_cmd_queue *queue
, const VkVideoBeginCodingInfoKHR* pBeginInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_VIDEO_CODING_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_VIDEO_CODING_KHR;
   if (pBeginInfo) {
      cmd->u.begin_video_coding_khr.begin_info = linear_alloc_child(queue->ctx, sizeof(VkVideoBeginCodingInfoKHR));
      if (cmd->u.begin_video_coding_khr.begin_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_video_coding_khr.begin_info, pBeginInfo, sizeof(VkVideoBeginCodingInfoKHR));
      VkVideoBeginCodingInfoKHR *tmp_dst1 = (void *)cmd->u.begin_video_coding_khr.begin_info;
      VkVideoBeginCodingInfoKHR *tmp_src2 = (void *)pBeginInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeRateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeRateControlInfoKHR));
            VkVideoEncodeRateControlInfoKHR *tmp_dst3 = (void *)(*dst_pnext_link);
            VkVideoEncodeRateControlInfoKHR *tmp_src4 = (void *)pnext;
            if (tmp_src4->pLayers) {
               tmp_dst3->pLayers = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeRateControlLayerInfoKHR) * ceil(tmp_src4->layerCount));
               if (tmp_dst3->pLayers == NULL) return NULL;
               memcpy((void *)tmp_dst3->pLayers, tmp_src4->pLayers, sizeof(VkVideoEncodeRateControlLayerInfoKHR) * ceil(tmp_src4->layerCount));
               VkVideoEncodeRateControlLayerInfoKHR *tmp_dst5 = (void *)tmp_dst3->pLayers;
               VkVideoEncodeRateControlLayerInfoKHR *tmp_src6 = (void *)tmp_src4->pLayers;
               for (uint32_t i7 = 0; i7 < tmp_src4->layerCount; i7++) {
                  VkVideoEncodeRateControlLayerInfoKHR *tmp_dst8 = tmp_dst5 + i7;
                  VkVideoEncodeRateControlLayerInfoKHR *tmp_src9 = tmp_src6 + i7;
                  const VkBaseInStructure *pnext = tmp_src9->pNext;
                  void **dst_pnext_link = (void **)&tmp_dst8->pNext;
                  while (pnext) {
                     switch ((int32_t)pnext->sType) {
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264RateControlLayerInfoKHR));
                        break;
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265RateControlLayerInfoKHR));
                        break;
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1RateControlLayerInfoKHR));
                        break;
                     }
                     pnext = pnext->pNext;
                     dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
                  }
               }
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264RateControlInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_GOP_REMAINING_FRAME_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264GopRemainingFrameInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264GopRemainingFrameInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265RateControlInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_GOP_REMAINING_FRAME_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265GopRemainingFrameInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265GopRemainingFrameInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1RateControlInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_GOP_REMAINING_FRAME_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1GopRemainingFrameInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1GopRemainingFrameInfoKHR));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pReferenceSlots) {
         tmp_dst1->pReferenceSlots = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         if (tmp_dst1->pReferenceSlots == NULL) return NULL;
         memcpy((void *)tmp_dst1->pReferenceSlots, tmp_src2->pReferenceSlots, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         VkVideoReferenceSlotInfoKHR *tmp_dst10 = (void *)tmp_dst1->pReferenceSlots;
         VkVideoReferenceSlotInfoKHR *tmp_src11 = (void *)tmp_src2->pReferenceSlots;
         for (uint32_t i12 = 0; i12 < tmp_src2->referenceSlotCount; i12++) {
            VkVideoReferenceSlotInfoKHR *tmp_dst13 = tmp_dst10 + i12;
            VkVideoReferenceSlotInfoKHR *tmp_src14 = tmp_src11 + i12;
            const VkBaseInStructure *pnext = tmp_src14->pNext;
            void **dst_pnext_link = (void **)&tmp_dst13->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_dst15 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_src16 = (void *)pnext;
                  if (tmp_src16->pStdReferenceInfo) {
                     tmp_dst15->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264ReferenceInfo));
                     if (tmp_dst15->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst15->pStdReferenceInfo, tmp_src16->pStdReferenceInfo, sizeof(StdVideoDecodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_dst17 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_src18 = (void *)pnext;
                  if (tmp_src18->pStdReferenceInfo) {
                     tmp_dst17->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265ReferenceInfo));
                     if (tmp_dst17->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst17->pStdReferenceInfo, tmp_src18->pStdReferenceInfo, sizeof(StdVideoDecodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_dst19 = (void *)(*dst_pnext_link);
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_src20 = (void *)pnext;
                  if (tmp_src20->pStdReferenceInfo) {
                     tmp_dst19->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1ReferenceInfo));
                     if (tmp_dst19->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst19->pStdReferenceInfo, tmp_src20->pStdReferenceInfo, sizeof(StdVideoDecodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_dst21 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_src22 = (void *)pnext;
                  if (tmp_src22->pStdReferenceInfo) {
                     tmp_dst21->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264ReferenceInfo));
                     if (tmp_dst21->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst21->pStdReferenceInfo, tmp_src22->pStdReferenceInfo, sizeof(StdVideoEncodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_dst23 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_src24 = (void *)pnext;
                  if (tmp_src24->pStdReferenceInfo) {
                     tmp_dst23->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265ReferenceInfo));
                     if (tmp_dst23->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst23->pStdReferenceInfo, tmp_src24->pStdReferenceInfo, sizeof(StdVideoEncodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_dst25 = (void *)(*dst_pnext_link);
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_src26 = (void *)pnext;
                  if (tmp_src26->pStdReferenceInfo) {
                     tmp_dst25->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1ReferenceInfo));
                     if (tmp_dst25->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst25->pStdReferenceInfo, tmp_src26->pStdReferenceInfo, sizeof(StdVideoEncodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_REFERENCE_INTRA_REFRESH_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            if (tmp_src14->pPictureResource) {
               tmp_dst13->pPictureResource = linear_alloc_child(queue->ctx, sizeof(VkVideoPictureResourceInfoKHR));
               if (tmp_dst13->pPictureResource == NULL) return NULL;
               memcpy((void *)tmp_dst13->pPictureResource, tmp_src14->pPictureResource, sizeof(VkVideoPictureResourceInfoKHR));
            }
         }
      }
   } else {
      cmd->u.begin_video_coding_khr.begin_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_control_video_coding_khr(struct vk_cmd_queue *queue
, const VkVideoCodingControlInfoKHR* pCodingControlInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CONTROL_VIDEO_CODING_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CONTROL_VIDEO_CODING_KHR;
   if (pCodingControlInfo) {
      cmd->u.control_video_coding_khr.coding_control_info = linear_alloc_child(queue->ctx, sizeof(VkVideoCodingControlInfoKHR));
      if (cmd->u.control_video_coding_khr.coding_control_info == NULL) return NULL;
      memcpy((void *)cmd->u.control_video_coding_khr.coding_control_info, pCodingControlInfo, sizeof(VkVideoCodingControlInfoKHR));
      VkVideoCodingControlInfoKHR *tmp_dst1 = (void *)cmd->u.control_video_coding_khr.coding_control_info;
      VkVideoCodingControlInfoKHR *tmp_src2 = (void *)pCodingControlInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUALITY_LEVEL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeQualityLevelInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeQualityLevelInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeRateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeRateControlInfoKHR));
            VkVideoEncodeRateControlInfoKHR *tmp_dst3 = (void *)(*dst_pnext_link);
            VkVideoEncodeRateControlInfoKHR *tmp_src4 = (void *)pnext;
            if (tmp_src4->pLayers) {
               tmp_dst3->pLayers = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeRateControlLayerInfoKHR) * ceil(tmp_src4->layerCount));
               if (tmp_dst3->pLayers == NULL) return NULL;
               memcpy((void *)tmp_dst3->pLayers, tmp_src4->pLayers, sizeof(VkVideoEncodeRateControlLayerInfoKHR) * ceil(tmp_src4->layerCount));
               VkVideoEncodeRateControlLayerInfoKHR *tmp_dst5 = (void *)tmp_dst3->pLayers;
               VkVideoEncodeRateControlLayerInfoKHR *tmp_src6 = (void *)tmp_src4->pLayers;
               for (uint32_t i7 = 0; i7 < tmp_src4->layerCount; i7++) {
                  VkVideoEncodeRateControlLayerInfoKHR *tmp_dst8 = tmp_dst5 + i7;
                  VkVideoEncodeRateControlLayerInfoKHR *tmp_src9 = tmp_src6 + i7;
                  const VkBaseInStructure *pnext = tmp_src9->pNext;
                  void **dst_pnext_link = (void **)&tmp_dst8->pNext;
                  while (pnext) {
                     switch ((int32_t)pnext->sType) {
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264RateControlLayerInfoKHR));
                        break;
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265RateControlLayerInfoKHR));
                        break;
                     case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_RATE_CONTROL_LAYER_INFO_KHR:
                        (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1RateControlLayerInfoKHR));
                        if ((*dst_pnext_link) == NULL) return NULL;
                        memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1RateControlLayerInfoKHR));
                        break;
                     }
                     pnext = pnext->pNext;
                     dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
                  }
               }
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264RateControlInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265RateControlInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_RATE_CONTROL_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1RateControlInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1RateControlInfoKHR));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.control_video_coding_khr.coding_control_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_video_coding_khr(struct vk_cmd_queue *queue
, const VkVideoEndCodingInfoKHR* pEndCodingInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_VIDEO_CODING_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_VIDEO_CODING_KHR;
   if (pEndCodingInfo) {
      cmd->u.end_video_coding_khr.end_coding_info = linear_alloc_child(queue->ctx, sizeof(VkVideoEndCodingInfoKHR));
      if (cmd->u.end_video_coding_khr.end_coding_info == NULL) return NULL;
      memcpy((void *)cmd->u.end_video_coding_khr.end_coding_info, pEndCodingInfo, sizeof(VkVideoEndCodingInfoKHR));
   } else {
      cmd->u.end_video_coding_khr.end_coding_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_encode_video_khr(struct vk_cmd_queue *queue
, const VkVideoEncodeInfoKHR* pEncodeInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_ENCODE_VIDEO_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_ENCODE_VIDEO_KHR;
   if (pEncodeInfo) {
      cmd->u.encode_video_khr.encode_info = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeInfoKHR));
      if (cmd->u.encode_video_khr.encode_info == NULL) return NULL;
      memcpy((void *)cmd->u.encode_video_khr.encode_info, pEncodeInfo, sizeof(VkVideoEncodeInfoKHR));
      VkVideoEncodeInfoKHR *tmp_dst1 = (void *)cmd->u.encode_video_khr.encode_info;
      VkVideoEncodeInfoKHR *tmp_src2 = (void *)pEncodeInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_VIDEO_INLINE_QUERY_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoInlineQueryInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoInlineQueryInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUANTIZATION_MAP_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeQuantizationMapInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeQuantizationMapInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264PictureInfoKHR));
            VkVideoEncodeH264PictureInfoKHR *tmp_dst3 = (void *)(*dst_pnext_link);
            VkVideoEncodeH264PictureInfoKHR *tmp_src4 = (void *)pnext;
            if (tmp_src4->pNaluSliceEntries) {
               tmp_dst3->pNaluSliceEntries = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264NaluSliceInfoKHR) * ceil(tmp_src4->naluSliceEntryCount));
               if (tmp_dst3->pNaluSliceEntries == NULL) return NULL;
               memcpy((void *)tmp_dst3->pNaluSliceEntries, tmp_src4->pNaluSliceEntries, sizeof(VkVideoEncodeH264NaluSliceInfoKHR) * ceil(tmp_src4->naluSliceEntryCount));
               VkVideoEncodeH264NaluSliceInfoKHR *tmp_dst5 = (void *)tmp_dst3->pNaluSliceEntries;
               VkVideoEncodeH264NaluSliceInfoKHR *tmp_src6 = (void *)tmp_src4->pNaluSliceEntries;
               for (uint32_t i7 = 0; i7 < tmp_src4->naluSliceEntryCount; i7++) {
                  VkVideoEncodeH264NaluSliceInfoKHR *tmp_dst8 = tmp_dst5 + i7;
                  VkVideoEncodeH264NaluSliceInfoKHR *tmp_src9 = tmp_src6 + i7;
                  if (tmp_src9->pStdSliceHeader) {
                     tmp_dst8->pStdSliceHeader = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264SliceHeader));
                     if (tmp_dst8->pStdSliceHeader == NULL) return NULL;
                     memcpy((void *)tmp_dst8->pStdSliceHeader, tmp_src9->pStdSliceHeader, sizeof(StdVideoEncodeH264SliceHeader));
                  }
               }
            }
            if (tmp_src4->pStdPictureInfo) {
               tmp_dst3->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264PictureInfo));
               if (tmp_dst3->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst3->pStdPictureInfo, tmp_src4->pStdPictureInfo, sizeof(StdVideoEncodeH264PictureInfo));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265PictureInfoKHR));
            VkVideoEncodeH265PictureInfoKHR *tmp_dst10 = (void *)(*dst_pnext_link);
            VkVideoEncodeH265PictureInfoKHR *tmp_src11 = (void *)pnext;
            if (tmp_src11->pNaluSliceSegmentEntries) {
               tmp_dst10->pNaluSliceSegmentEntries = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265NaluSliceSegmentInfoKHR) * ceil(tmp_src11->naluSliceSegmentEntryCount));
               if (tmp_dst10->pNaluSliceSegmentEntries == NULL) return NULL;
               memcpy((void *)tmp_dst10->pNaluSliceSegmentEntries, tmp_src11->pNaluSliceSegmentEntries, sizeof(VkVideoEncodeH265NaluSliceSegmentInfoKHR) * ceil(tmp_src11->naluSliceSegmentEntryCount));
               VkVideoEncodeH265NaluSliceSegmentInfoKHR *tmp_dst12 = (void *)tmp_dst10->pNaluSliceSegmentEntries;
               VkVideoEncodeH265NaluSliceSegmentInfoKHR *tmp_src13 = (void *)tmp_src11->pNaluSliceSegmentEntries;
               for (uint32_t i14 = 0; i14 < tmp_src11->naluSliceSegmentEntryCount; i14++) {
                  VkVideoEncodeH265NaluSliceSegmentInfoKHR *tmp_dst15 = tmp_dst12 + i14;
                  VkVideoEncodeH265NaluSliceSegmentInfoKHR *tmp_src16 = tmp_src13 + i14;
                  if (tmp_src16->pStdSliceSegmentHeader) {
                     tmp_dst15->pStdSliceSegmentHeader = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265SliceSegmentHeader));
                     if (tmp_dst15->pStdSliceSegmentHeader == NULL) return NULL;
                     memcpy((void *)tmp_dst15->pStdSliceSegmentHeader, tmp_src16->pStdSliceSegmentHeader, sizeof(StdVideoEncodeH265SliceSegmentHeader));
                  }
               }
            }
            if (tmp_src11->pStdPictureInfo) {
               tmp_dst10->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265PictureInfo));
               if (tmp_dst10->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst10->pStdPictureInfo, tmp_src11->pStdPictureInfo, sizeof(StdVideoEncodeH265PictureInfo));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_PICTURE_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1PictureInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1PictureInfoKHR));
            VkVideoEncodeAV1PictureInfoKHR *tmp_dst17 = (void *)(*dst_pnext_link);
            VkVideoEncodeAV1PictureInfoKHR *tmp_src18 = (void *)pnext;
            if (tmp_src18->pStdPictureInfo) {
               tmp_dst17->pStdPictureInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1PictureInfo));
               if (tmp_dst17->pStdPictureInfo == NULL) return NULL;
               memcpy((void *)tmp_dst17->pStdPictureInfo, tmp_src18->pStdPictureInfo, sizeof(StdVideoEncodeAV1PictureInfo));
            }
            break;
         case VK_STRUCTURE_TYPE_VIDEO_ENCODE_INTRA_REFRESH_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeIntraRefreshInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeIntraRefreshInfoKHR));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pSetupReferenceSlot) {
         tmp_dst1->pSetupReferenceSlot = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceSlotInfoKHR));
         if (tmp_dst1->pSetupReferenceSlot == NULL) return NULL;
         memcpy((void *)tmp_dst1->pSetupReferenceSlot, tmp_src2->pSetupReferenceSlot, sizeof(VkVideoReferenceSlotInfoKHR));
         VkVideoReferenceSlotInfoKHR *tmp_dst19 = (void *)tmp_dst1->pSetupReferenceSlot;
         VkVideoReferenceSlotInfoKHR *tmp_src20 = (void *)tmp_src2->pSetupReferenceSlot;
         const VkBaseInStructure *pnext = tmp_src20->pNext;
         void **dst_pnext_link = (void **)&tmp_dst19->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
               VkVideoDecodeH264DpbSlotInfoKHR *tmp_dst21 = (void *)(*dst_pnext_link);
               VkVideoDecodeH264DpbSlotInfoKHR *tmp_src22 = (void *)pnext;
               if (tmp_src22->pStdReferenceInfo) {
                  tmp_dst21->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264ReferenceInfo));
                  if (tmp_dst21->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst21->pStdReferenceInfo, tmp_src22->pStdReferenceInfo, sizeof(StdVideoDecodeH264ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
               VkVideoDecodeH265DpbSlotInfoKHR *tmp_dst23 = (void *)(*dst_pnext_link);
               VkVideoDecodeH265DpbSlotInfoKHR *tmp_src24 = (void *)pnext;
               if (tmp_src24->pStdReferenceInfo) {
                  tmp_dst23->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265ReferenceInfo));
                  if (tmp_dst23->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst23->pStdReferenceInfo, tmp_src24->pStdReferenceInfo, sizeof(StdVideoDecodeH265ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
               VkVideoDecodeAV1DpbSlotInfoKHR *tmp_dst25 = (void *)(*dst_pnext_link);
               VkVideoDecodeAV1DpbSlotInfoKHR *tmp_src26 = (void *)pnext;
               if (tmp_src26->pStdReferenceInfo) {
                  tmp_dst25->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1ReferenceInfo));
                  if (tmp_dst25->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst25->pStdReferenceInfo, tmp_src26->pStdReferenceInfo, sizeof(StdVideoDecodeAV1ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
               VkVideoEncodeH264DpbSlotInfoKHR *tmp_dst27 = (void *)(*dst_pnext_link);
               VkVideoEncodeH264DpbSlotInfoKHR *tmp_src28 = (void *)pnext;
               if (tmp_src28->pStdReferenceInfo) {
                  tmp_dst27->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264ReferenceInfo));
                  if (tmp_dst27->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst27->pStdReferenceInfo, tmp_src28->pStdReferenceInfo, sizeof(StdVideoEncodeH264ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
               VkVideoEncodeH265DpbSlotInfoKHR *tmp_dst29 = (void *)(*dst_pnext_link);
               VkVideoEncodeH265DpbSlotInfoKHR *tmp_src30 = (void *)pnext;
               if (tmp_src30->pStdReferenceInfo) {
                  tmp_dst29->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265ReferenceInfo));
                  if (tmp_dst29->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst29->pStdReferenceInfo, tmp_src30->pStdReferenceInfo, sizeof(StdVideoEncodeH265ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_DPB_SLOT_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
               VkVideoEncodeAV1DpbSlotInfoKHR *tmp_dst31 = (void *)(*dst_pnext_link);
               VkVideoEncodeAV1DpbSlotInfoKHR *tmp_src32 = (void *)pnext;
               if (tmp_src32->pStdReferenceInfo) {
                  tmp_dst31->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1ReferenceInfo));
                  if (tmp_dst31->pStdReferenceInfo == NULL) return NULL;
                  memcpy((void *)tmp_dst31->pStdReferenceInfo, tmp_src32->pStdReferenceInfo, sizeof(StdVideoEncodeAV1ReferenceInfo));
               }
               break;
            case VK_STRUCTURE_TYPE_VIDEO_REFERENCE_INTRA_REFRESH_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
         if (tmp_src20->pPictureResource) {
            tmp_dst19->pPictureResource = linear_alloc_child(queue->ctx, sizeof(VkVideoPictureResourceInfoKHR));
            if (tmp_dst19->pPictureResource == NULL) return NULL;
            memcpy((void *)tmp_dst19->pPictureResource, tmp_src20->pPictureResource, sizeof(VkVideoPictureResourceInfoKHR));
         }
      }
      if (tmp_src2->pReferenceSlots) {
         tmp_dst1->pReferenceSlots = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         if (tmp_dst1->pReferenceSlots == NULL) return NULL;
         memcpy((void *)tmp_dst1->pReferenceSlots, tmp_src2->pReferenceSlots, sizeof(VkVideoReferenceSlotInfoKHR) * ceil(tmp_src2->referenceSlotCount));
         VkVideoReferenceSlotInfoKHR *tmp_dst33 = (void *)tmp_dst1->pReferenceSlots;
         VkVideoReferenceSlotInfoKHR *tmp_src34 = (void *)tmp_src2->pReferenceSlots;
         for (uint32_t i35 = 0; i35 < tmp_src2->referenceSlotCount; i35++) {
            VkVideoReferenceSlotInfoKHR *tmp_dst36 = tmp_dst33 + i35;
            VkVideoReferenceSlotInfoKHR *tmp_src37 = tmp_src34 + i35;
            const VkBaseInStructure *pnext = tmp_src37->pNext;
            void **dst_pnext_link = (void **)&tmp_dst36->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH264DpbSlotInfoKHR));
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_dst38 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH264DpbSlotInfoKHR *tmp_src39 = (void *)pnext;
                  if (tmp_src39->pStdReferenceInfo) {
                     tmp_dst38->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH264ReferenceInfo));
                     if (tmp_dst38->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst38->pStdReferenceInfo, tmp_src39->pStdReferenceInfo, sizeof(StdVideoDecodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeH265DpbSlotInfoKHR));
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_dst40 = (void *)(*dst_pnext_link);
                  VkVideoDecodeH265DpbSlotInfoKHR *tmp_src41 = (void *)pnext;
                  if (tmp_src41->pStdReferenceInfo) {
                     tmp_dst40->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeH265ReferenceInfo));
                     if (tmp_dst40->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst40->pStdReferenceInfo, tmp_src41->pStdReferenceInfo, sizeof(StdVideoDecodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_DECODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoDecodeAV1DpbSlotInfoKHR));
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_dst42 = (void *)(*dst_pnext_link);
                  VkVideoDecodeAV1DpbSlotInfoKHR *tmp_src43 = (void *)pnext;
                  if (tmp_src43->pStdReferenceInfo) {
                     tmp_dst42->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoDecodeAV1ReferenceInfo));
                     if (tmp_dst42->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst42->pStdReferenceInfo, tmp_src43->pStdReferenceInfo, sizeof(StdVideoDecodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH264DpbSlotInfoKHR));
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_dst44 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH264DpbSlotInfoKHR *tmp_src45 = (void *)pnext;
                  if (tmp_src45->pStdReferenceInfo) {
                     tmp_dst44->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH264ReferenceInfo));
                     if (tmp_dst44->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst44->pStdReferenceInfo, tmp_src45->pStdReferenceInfo, sizeof(StdVideoEncodeH264ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeH265DpbSlotInfoKHR));
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_dst46 = (void *)(*dst_pnext_link);
                  VkVideoEncodeH265DpbSlotInfoKHR *tmp_src47 = (void *)pnext;
                  if (tmp_src47->pStdReferenceInfo) {
                     tmp_dst46->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeH265ReferenceInfo));
                     if (tmp_dst46->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst46->pStdReferenceInfo, tmp_src47->pStdReferenceInfo, sizeof(StdVideoEncodeH265ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_DPB_SLOT_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoEncodeAV1DpbSlotInfoKHR));
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_dst48 = (void *)(*dst_pnext_link);
                  VkVideoEncodeAV1DpbSlotInfoKHR *tmp_src49 = (void *)pnext;
                  if (tmp_src49->pStdReferenceInfo) {
                     tmp_dst48->pStdReferenceInfo = linear_alloc_child(queue->ctx, sizeof(StdVideoEncodeAV1ReferenceInfo));
                     if (tmp_dst48->pStdReferenceInfo == NULL) return NULL;
                     memcpy((void *)tmp_dst48->pStdReferenceInfo, tmp_src49->pStdReferenceInfo, sizeof(StdVideoEncodeAV1ReferenceInfo));
                  }
                  break;
               case VK_STRUCTURE_TYPE_VIDEO_REFERENCE_INTRA_REFRESH_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkVideoReferenceIntraRefreshInfoKHR));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            if (tmp_src37->pPictureResource) {
               tmp_dst36->pPictureResource = linear_alloc_child(queue->ctx, sizeof(VkVideoPictureResourceInfoKHR));
               if (tmp_dst36->pPictureResource == NULL) return NULL;
               memcpy((void *)tmp_dst36->pPictureResource, tmp_src37->pPictureResource, sizeof(VkVideoPictureResourceInfoKHR));
            }
         }
      }
   } else {
      cmd->u.encode_video_khr.encode_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_nv(struct vk_cmd_queue *queue
, uint32_t decompressRegionCount
, const VkDecompressMemoryRegionNV* pDecompressMemoryRegions
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DECOMPRESS_MEMORY_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DECOMPRESS_MEMORY_NV;
   cmd->u.decompress_memory_nv.decompress_region_count = decompressRegionCount;
   if (pDecompressMemoryRegions) {
      cmd->u.decompress_memory_nv.decompress_memory_regions = linear_alloc_child(queue->ctx, sizeof(VkDecompressMemoryRegionNV) * ceil(decompressRegionCount));
      if (cmd->u.decompress_memory_nv.decompress_memory_regions == NULL) return NULL;
      memcpy((void *)cmd->u.decompress_memory_nv.decompress_memory_regions, pDecompressMemoryRegions, sizeof(VkDecompressMemoryRegionNV) * ceil(decompressRegionCount));
   } else {
      cmd->u.decompress_memory_nv.decompress_memory_regions = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_indirect_count_nv(struct vk_cmd_queue *queue
, VkDeviceAddress indirectCommandsAddress
, VkDeviceAddress indirectCommandsCountAddress
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_NV;
   cmd->u.decompress_memory_indirect_count_nv.indirect_commands_address = indirectCommandsAddress;
   cmd->u.decompress_memory_indirect_count_nv.indirect_commands_count_address = indirectCommandsCountAddress;
   cmd->u.decompress_memory_indirect_count_nv.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_build_partitioned_acceleration_structures_nv(struct vk_cmd_queue *queue
, const VkBuildPartitionedAccelerationStructureInfoNV*  pBuildInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BUILD_PARTITIONED_ACCELERATION_STRUCTURES_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BUILD_PARTITIONED_ACCELERATION_STRUCTURES_NV;
   if (pBuildInfo) {
      cmd->u.build_partitioned_acceleration_structures_nv.build_info = linear_alloc_child(queue->ctx, sizeof(VkBuildPartitionedAccelerationStructureInfoNV));
      if (cmd->u.build_partitioned_acceleration_structures_nv.build_info == NULL) return NULL;
      memcpy((void *)cmd->u.build_partitioned_acceleration_structures_nv.build_info, pBuildInfo, sizeof(VkBuildPartitionedAccelerationStructureInfoNV));
      VkBuildPartitionedAccelerationStructureInfoNV *tmp_dst1 = (void *)cmd->u.build_partitioned_acceleration_structures_nv.build_info;
      VkBuildPartitionedAccelerationStructureInfoNV *tmp_src2 = (void *)pBuildInfo;
      VkPartitionedAccelerationStructureInstancesInputNV *tmp_dst3 = (void *)&tmp_dst1->input;
      VkPartitionedAccelerationStructureInstancesInputNV *tmp_src4 = (void *)&tmp_src2->input;
      const VkBaseInStructure *pnext = tmp_src4->pNext;
      void **dst_pnext_link = (void **)&tmp_dst3->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_FLAGS_NV:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPartitionedAccelerationStructureFlagsNV));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPartitionedAccelerationStructureFlagsNV));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.build_partitioned_acceleration_structures_nv.build_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_ext(struct vk_cmd_queue *queue
, const VkDecompressMemoryInfoEXT* pDecompressMemoryInfoEXT
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DECOMPRESS_MEMORY_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DECOMPRESS_MEMORY_EXT;
   if (pDecompressMemoryInfoEXT) {
      cmd->u.decompress_memory_ext.decompress_memory_info_ext = linear_alloc_child(queue->ctx, sizeof(VkDecompressMemoryInfoEXT));
      if (cmd->u.decompress_memory_ext.decompress_memory_info_ext == NULL) return NULL;
      memcpy((void *)cmd->u.decompress_memory_ext.decompress_memory_info_ext, pDecompressMemoryInfoEXT, sizeof(VkDecompressMemoryInfoEXT));
      VkDecompressMemoryInfoEXT *tmp_dst1 = (void *)cmd->u.decompress_memory_ext.decompress_memory_info_ext;
      VkDecompressMemoryInfoEXT *tmp_src2 = (void *)pDecompressMemoryInfoEXT;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkDecompressMemoryRegionEXT) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkDecompressMemoryRegionEXT) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.decompress_memory_ext.decompress_memory_info_ext = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_indirect_count_ext(struct vk_cmd_queue *queue
, VkMemoryDecompressionMethodFlagsEXT decompressionMethod
, VkDeviceAddress indirectCommandsAddress
, VkDeviceAddress indirectCommandsCountAddress
, uint32_t maxDecompressionCount
, uint32_t stride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_EXT;
   cmd->u.decompress_memory_indirect_count_ext.decompression_method = decompressionMethod;
   cmd->u.decompress_memory_indirect_count_ext.indirect_commands_address = indirectCommandsAddress;
   cmd->u.decompress_memory_indirect_count_ext.indirect_commands_count_address = indirectCommandsCountAddress;
   cmd->u.decompress_memory_indirect_count_ext.max_decompression_count = maxDecompressionCount;
   cmd->u.decompress_memory_indirect_count_ext.stride = stride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_cu_launch_kernel_nvx(struct vk_cmd_queue *queue
, const VkCuLaunchInfoNVX* pLaunchInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CU_LAUNCH_KERNEL_NVX]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CU_LAUNCH_KERNEL_NVX;
   if (pLaunchInfo) {
      cmd->u.cu_launch_kernel_nvx.launch_info = linear_alloc_child(queue->ctx, sizeof(VkCuLaunchInfoNVX));
      if (cmd->u.cu_launch_kernel_nvx.launch_info == NULL) return NULL;
      memcpy((void *)cmd->u.cu_launch_kernel_nvx.launch_info, pLaunchInfo, sizeof(VkCuLaunchInfoNVX));
      VkCuLaunchInfoNVX *tmp_dst1 = (void *)cmd->u.cu_launch_kernel_nvx.launch_info;
      VkCuLaunchInfoNVX *tmp_src2 = (void *)pLaunchInfo;
      if (tmp_src2->pParams) {
         tmp_dst1->pParams = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src2->paramCount));
         if (tmp_dst1->pParams == NULL) return NULL;
         memcpy((void *)tmp_dst1->pParams, tmp_src2->pParams, 1 * ceil(tmp_src2->paramCount));
      }
      if (tmp_src2->pExtras) {
         tmp_dst1->pExtras = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src2->extraCount));
         if (tmp_dst1->pExtras == NULL) return NULL;
         memcpy((void *)tmp_dst1->pExtras, tmp_src2->pExtras, 1 * ceil(tmp_src2->extraCount));
      }
   } else {
      cmd->u.cu_launch_kernel_nvx.launch_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffers_ext(struct vk_cmd_queue *queue
, uint32_t bufferCount
, const VkDescriptorBufferBindingInfoEXT* pBindingInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_DESCRIPTOR_BUFFERS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_DESCRIPTOR_BUFFERS_EXT;
   cmd->u.bind_descriptor_buffers_ext.buffer_count = bufferCount;
   if (pBindingInfos) {
      cmd->u.bind_descriptor_buffers_ext.binding_infos = linear_alloc_child(queue->ctx, sizeof(VkDescriptorBufferBindingInfoEXT) * ceil(bufferCount));
      if (cmd->u.bind_descriptor_buffers_ext.binding_infos == NULL) return NULL;
      memcpy((void *)cmd->u.bind_descriptor_buffers_ext.binding_infos, pBindingInfos, sizeof(VkDescriptorBufferBindingInfoEXT) * ceil(bufferCount));
      VkDescriptorBufferBindingInfoEXT *tmp_dst1 = (void *)cmd->u.bind_descriptor_buffers_ext.binding_infos;
      VkDescriptorBufferBindingInfoEXT *tmp_src2 = (void *)pBindingInfos;
      for (uint32_t i3 = 0; i3 < bufferCount; i3++) {
         VkDescriptorBufferBindingInfoEXT *tmp_dst4 = tmp_dst1 + i3;
         VkDescriptorBufferBindingInfoEXT *tmp_src5 = tmp_src2 + i3;
         const VkBaseInStructure *pnext = tmp_src5->pNext;
         void **dst_pnext_link = (void **)&tmp_dst4->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_BUFFER_USAGE_FLAGS_2_CREATE_INFO:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkBufferUsageFlags2CreateInfo));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkBufferUsageFlags2CreateInfo));
               break;
            case VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_PUSH_DESCRIPTOR_BUFFER_HANDLE_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDescriptorBufferBindingPushDescriptorBufferHandleEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDescriptorBufferBindingPushDescriptorBufferHandleEXT));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.bind_descriptor_buffers_ext.binding_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_descriptor_buffer_offsets_ext(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipelineLayout layout
, uint32_t firstSet
, uint32_t setCount
, const uint32_t* pBufferIndices
, const VkDeviceSize* pOffsets
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS_EXT;
   cmd->u.set_descriptor_buffer_offsets_ext.pipeline_bind_point = pipelineBindPoint;
   cmd->u.set_descriptor_buffer_offsets_ext.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.set_descriptor_buffer_offsets_ext.first_set = firstSet;
   cmd->u.set_descriptor_buffer_offsets_ext.set_count = setCount;
   if (pBufferIndices) {
      cmd->u.set_descriptor_buffer_offsets_ext.buffer_indices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(setCount));
      if (cmd->u.set_descriptor_buffer_offsets_ext.buffer_indices == NULL) return NULL;
      memcpy((void *)cmd->u.set_descriptor_buffer_offsets_ext.buffer_indices, pBufferIndices, sizeof(uint32_t) * ceil(setCount));
   } else {
      cmd->u.set_descriptor_buffer_offsets_ext.buffer_indices = NULL;
   }
   if (pOffsets) {
      cmd->u.set_descriptor_buffer_offsets_ext.offsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(setCount));
      if (cmd->u.set_descriptor_buffer_offsets_ext.offsets == NULL) return NULL;
      memcpy((void *)cmd->u.set_descriptor_buffer_offsets_ext.offsets, pOffsets, sizeof(VkDeviceSize) * ceil(setCount));
   } else {
      cmd->u.set_descriptor_buffer_offsets_ext.offsets = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers_ext(struct vk_cmd_queue *queue
, VkPipelineBindPoint pipelineBindPoint
, VkPipelineLayout layout
, uint32_t set
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS_EXT;
   cmd->u.bind_descriptor_buffer_embedded_samplers_ext.pipeline_bind_point = pipelineBindPoint;
   cmd->u.bind_descriptor_buffer_embedded_samplers_ext.layout = layout;
   enqueue_pipeline_layout(queue, layout);
   cmd->u.bind_descriptor_buffer_embedded_samplers_ext.set = set;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_rendering(struct vk_cmd_queue *queue
, const VkRenderingInfo*                              pRenderingInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_RENDERING]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_RENDERING;
   if (pRenderingInfo) {
      cmd->u.begin_rendering.rendering_info = linear_alloc_child(queue->ctx, sizeof(VkRenderingInfo));
      if (cmd->u.begin_rendering.rendering_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_rendering.rendering_info, pRenderingInfo, sizeof(VkRenderingInfo));
      VkRenderingInfo *tmp_dst1 = (void *)cmd->u.begin_rendering.rendering_info;
      VkRenderingInfo *tmp_src2 = (void *)pRenderingInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDeviceGroupRenderPassBeginInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDeviceGroupRenderPassBeginInfo));
            VkDeviceGroupRenderPassBeginInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkDeviceGroupRenderPassBeginInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pDeviceRenderAreas) {
               tmp_dst3->pDeviceRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
               if (tmp_dst3->pDeviceRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst3->pDeviceRenderAreas, tmp_src4->pDeviceRenderAreas, sizeof(VkRect2D) * ceil(tmp_src4->deviceRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMultisampledRenderToSingleSampledInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMultisampledRenderToSingleSampledInfoEXT));
            break;
         case VK_STRUCTURE_TYPE_RENDERING_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingFragmentShadingRateAttachmentInfoKHR));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingFragmentShadingRateAttachmentInfoKHR));
            break;
         case VK_STRUCTURE_TYPE_RENDERING_FRAGMENT_DENSITY_MAP_ATTACHMENT_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingFragmentDensityMapAttachmentInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingFragmentDensityMapAttachmentInfoEXT));
            break;
         case VK_STRUCTURE_TYPE_MULTIVIEW_PER_VIEW_ATTRIBUTES_INFO_NVX:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMultiviewPerViewAttributesInfoNVX));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMultiviewPerViewAttributesInfoNVX));
            break;
         case VK_STRUCTURE_TYPE_MULTIVIEW_PER_VIEW_RENDER_AREAS_RENDER_PASS_BEGIN_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM));
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_dst5 = (void *)(*dst_pnext_link);
            VkMultiviewPerViewRenderAreasRenderPassBeginInfoQCOM *tmp_src6 = (void *)pnext;
            if (tmp_src6->pPerViewRenderAreas) {
               tmp_dst5->pPerViewRenderAreas = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src6->perViewRenderAreaCount));
               if (tmp_dst5->pPerViewRenderAreas == NULL) return NULL;
               memcpy((void *)tmp_dst5->pPerViewRenderAreas, tmp_src6->pPerViewRenderAreas, sizeof(VkRect2D) * ceil(tmp_src6->perViewRenderAreaCount));
            }
            break;
         case VK_STRUCTURE_TYPE_TILE_MEMORY_SIZE_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkTileMemorySizeInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkTileMemorySizeInfoQCOM));
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_STRIPE_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassStripeBeginInfoARM));
            VkRenderPassStripeBeginInfoARM *tmp_dst7 = (void *)(*dst_pnext_link);
            VkRenderPassStripeBeginInfoARM *tmp_src8 = (void *)pnext;
            if (tmp_src8->pStripeInfos) {
               tmp_dst7->pStripeInfos = linear_alloc_child(queue->ctx, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src8->stripeInfoCount));
               if (tmp_dst7->pStripeInfos == NULL) return NULL;
               memcpy((void *)tmp_dst7->pStripeInfos, tmp_src8->pStripeInfos, sizeof(VkRenderPassStripeInfoARM) * ceil(tmp_src8->stripeInfoCount));
            }
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_TILE_SHADING_CREATE_INFO_QCOM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassTileShadingCreateInfoQCOM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassTileShadingCreateInfoQCOM));
            break;
         case VK_STRUCTURE_TYPE_RENDER_PASS_PERFORMANCE_COUNTERS_BY_REGION_BEGIN_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassPerformanceCountersByRegionBeginInfoARM));
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_dst9 = (void *)(*dst_pnext_link);
            VkRenderPassPerformanceCountersByRegionBeginInfoARM *tmp_src10 = (void *)pnext;
            if (tmp_src10->pCounterAddresses) {
               tmp_dst9->pCounterAddresses = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddress) * ceil(tmp_src10->counterAddressCount));
               if (tmp_dst9->pCounterAddresses == NULL) return NULL;
               memcpy((void *)tmp_dst9->pCounterAddresses, tmp_src10->pCounterAddresses, sizeof(VkDeviceAddress) * ceil(tmp_src10->counterAddressCount));
            }
            if (tmp_src10->pCounterIndices) {
               tmp_dst9->pCounterIndices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src10->counterIndexCount));
               if (tmp_dst9->pCounterIndices == NULL) return NULL;
               memcpy((void *)tmp_dst9->pCounterIndices, tmp_src10->pCounterIndices, sizeof(uint32_t) * ceil(tmp_src10->counterIndexCount));
            }
            break;
         case VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationsInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkSampleLocationsInfoEXT));
            VkSampleLocationsInfoEXT *tmp_dst11 = (void *)(*dst_pnext_link);
            VkSampleLocationsInfoEXT *tmp_src12 = (void *)pnext;
            if (tmp_src12->pSampleLocations) {
               tmp_dst11->pSampleLocations = linear_alloc_child(queue->ctx, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
               if (tmp_dst11->pSampleLocations == NULL) return NULL;
               memcpy((void *)tmp_dst11->pSampleLocations, tmp_src12->pSampleLocations, sizeof(VkSampleLocationEXT) * ceil(tmp_src12->sampleLocationsCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pColorAttachments) {
         tmp_dst1->pColorAttachments = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInfo) * ceil(tmp_src2->colorAttachmentCount));
         if (tmp_dst1->pColorAttachments == NULL) return NULL;
         memcpy((void *)tmp_dst1->pColorAttachments, tmp_src2->pColorAttachments, sizeof(VkRenderingAttachmentInfo) * ceil(tmp_src2->colorAttachmentCount));
         VkRenderingAttachmentInfo *tmp_dst13 = (void *)tmp_dst1->pColorAttachments;
         VkRenderingAttachmentInfo *tmp_src14 = (void *)tmp_src2->pColorAttachments;
         for (uint32_t i15 = 0; i15 < tmp_src2->colorAttachmentCount; i15++) {
            VkRenderingAttachmentInfo *tmp_dst16 = tmp_dst13 + i15;
            VkRenderingAttachmentInfo *tmp_src17 = tmp_src14 + i15;
            const VkBaseInStructure *pnext = tmp_src17->pNext;
            void **dst_pnext_link = (void **)&tmp_dst16->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_ATTACHMENT_FEEDBACK_LOOP_INFO_EXT:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkAttachmentFeedbackLoopInfoEXT));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkAttachmentFeedbackLoopInfoEXT));
                  break;
               case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_FLAGS_INFO_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentFlagsInfoKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentFlagsInfoKHR));
                  break;
               case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INITIAL_LAYOUT_INFO_MESA:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
      if (tmp_src2->pDepthAttachment) {
         tmp_dst1->pDepthAttachment = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInfo));
         if (tmp_dst1->pDepthAttachment == NULL) return NULL;
         memcpy((void *)tmp_dst1->pDepthAttachment, tmp_src2->pDepthAttachment, sizeof(VkRenderingAttachmentInfo));
         VkRenderingAttachmentInfo *tmp_dst18 = (void *)tmp_dst1->pDepthAttachment;
         VkRenderingAttachmentInfo *tmp_src19 = (void *)tmp_src2->pDepthAttachment;
         const VkBaseInStructure *pnext = tmp_src19->pNext;
         void **dst_pnext_link = (void **)&tmp_dst18->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_ATTACHMENT_FEEDBACK_LOOP_INFO_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkAttachmentFeedbackLoopInfoEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkAttachmentFeedbackLoopInfoEXT));
               break;
            case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_FLAGS_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentFlagsInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentFlagsInfoKHR));
               break;
            case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INITIAL_LAYOUT_INFO_MESA:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
      if (tmp_src2->pStencilAttachment) {
         tmp_dst1->pStencilAttachment = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInfo));
         if (tmp_dst1->pStencilAttachment == NULL) return NULL;
         memcpy((void *)tmp_dst1->pStencilAttachment, tmp_src2->pStencilAttachment, sizeof(VkRenderingAttachmentInfo));
         VkRenderingAttachmentInfo *tmp_dst20 = (void *)tmp_dst1->pStencilAttachment;
         VkRenderingAttachmentInfo *tmp_src21 = (void *)tmp_src2->pStencilAttachment;
         const VkBaseInStructure *pnext = tmp_src21->pNext;
         void **dst_pnext_link = (void **)&tmp_dst20->pNext;
         while (pnext) {
            switch ((int32_t)pnext->sType) {
            case VK_STRUCTURE_TYPE_ATTACHMENT_FEEDBACK_LOOP_INFO_EXT:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkAttachmentFeedbackLoopInfoEXT));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkAttachmentFeedbackLoopInfoEXT));
               break;
            case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_FLAGS_INFO_KHR:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentFlagsInfoKHR));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentFlagsInfoKHR));
               break;
            case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INITIAL_LAYOUT_INFO_MESA:
               (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
               if ((*dst_pnext_link) == NULL) return NULL;
               memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderingAttachmentInitialLayoutInfoMESA));
               break;
            }
            pnext = pnext->pNext;
            dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
         }
      }
   } else {
      cmd->u.begin_rendering.rendering_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_rendering(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_RENDERING]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_RENDERING;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_rendering2_khr(struct vk_cmd_queue *queue
, const VkRenderingEndInfoKHR*        pRenderingEndInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_RENDERING2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_RENDERING2_KHR;
   if (pRenderingEndInfo) {
      cmd->u.end_rendering2_khr.rendering_end_info = linear_alloc_child(queue->ctx, sizeof(VkRenderingEndInfoKHR));
      if (cmd->u.end_rendering2_khr.rendering_end_info == NULL) return NULL;
      memcpy((void *)cmd->u.end_rendering2_khr.rendering_end_info, pRenderingEndInfo, sizeof(VkRenderingEndInfoKHR));
      VkRenderingEndInfoKHR *tmp_dst1 = (void *)cmd->u.end_rendering2_khr.rendering_end_info;
      VkRenderingEndInfoKHR *tmp_src2 = (void *)pRenderingEndInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_OFFSET_END_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkRenderPassFragmentDensityMapOffsetEndInfoEXT));
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_dst3 = (void *)(*dst_pnext_link);
            VkRenderPassFragmentDensityMapOffsetEndInfoEXT *tmp_src4 = (void *)pnext;
            if (tmp_src4->pFragmentDensityOffsets) {
               tmp_dst3->pFragmentDensityOffsets = linear_alloc_child(queue->ctx, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
               if (tmp_dst3->pFragmentDensityOffsets == NULL) return NULL;
               memcpy((void *)tmp_dst3->pFragmentDensityOffsets, tmp_src4->pFragmentDensityOffsets, sizeof(VkOffset2D) * ceil(tmp_src4->fragmentDensityOffsetCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.end_rendering2_khr.rendering_end_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_build_micromaps_ext(struct vk_cmd_queue *queue
, uint32_t                                      infoCount
, const VkMicromapBuildInfoEXT* pInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BUILD_MICROMAPS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BUILD_MICROMAPS_EXT;
   cmd->u.build_micromaps_ext.info_count = infoCount;
   if (pInfos) {
      cmd->u.build_micromaps_ext.infos = linear_alloc_child(queue->ctx, sizeof(VkMicromapBuildInfoEXT) * ceil(infoCount));
      if (cmd->u.build_micromaps_ext.infos == NULL) return NULL;
      memcpy((void *)cmd->u.build_micromaps_ext.infos, pInfos, sizeof(VkMicromapBuildInfoEXT) * ceil(infoCount));
      VkMicromapBuildInfoEXT *tmp_dst1 = (void *)cmd->u.build_micromaps_ext.infos;
      VkMicromapBuildInfoEXT *tmp_src2 = (void *)pInfos;
      for (uint32_t i3 = 0; i3 < infoCount; i3++) {
         VkMicromapBuildInfoEXT *tmp_dst4 = tmp_dst1 + i3;
         VkMicromapBuildInfoEXT *tmp_src5 = tmp_src2 + i3;
         if (tmp_src5->pUsageCounts) {
            tmp_dst4->pUsageCounts = linear_alloc_child(queue->ctx, sizeof(VkMicromapUsageEXT) * ceil(tmp_src5->usageCountsCount));
            if (tmp_dst4->pUsageCounts == NULL) return NULL;
            memcpy((void *)tmp_dst4->pUsageCounts, tmp_src5->pUsageCounts, sizeof(VkMicromapUsageEXT) * ceil(tmp_src5->usageCountsCount));
         }
         if (tmp_src5->ppUsageCounts) {
            tmp_dst4->ppUsageCounts = linear_alloc_child(queue->ctx, sizeof(VkMicromapUsageEXT));
            if (tmp_dst4->ppUsageCounts == NULL) return NULL;
            memcpy((void *)tmp_dst4->ppUsageCounts, tmp_src5->ppUsageCounts, sizeof(VkMicromapUsageEXT));
         }
      }
   } else {
      cmd->u.build_micromaps_ext.infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_micromap_ext(struct vk_cmd_queue *queue
, const VkCopyMicromapInfoEXT*      pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MICROMAP_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MICROMAP_EXT;
   if (pInfo) {
      cmd->u.copy_micromap_ext.info = linear_alloc_child(queue->ctx, sizeof(VkCopyMicromapInfoEXT));
      if (cmd->u.copy_micromap_ext.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_micromap_ext.info, pInfo, sizeof(VkCopyMicromapInfoEXT));
   } else {
      cmd->u.copy_micromap_ext.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_micromap_to_memory_ext(struct vk_cmd_queue *queue
, const VkCopyMicromapToMemoryInfoEXT* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MICROMAP_TO_MEMORY_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MICROMAP_TO_MEMORY_EXT;
   if (pInfo) {
      cmd->u.copy_micromap_to_memory_ext.info = linear_alloc_child(queue->ctx, sizeof(VkCopyMicromapToMemoryInfoEXT));
      if (cmd->u.copy_micromap_to_memory_ext.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_micromap_to_memory_ext.info, pInfo, sizeof(VkCopyMicromapToMemoryInfoEXT));
   } else {
      cmd->u.copy_micromap_to_memory_ext.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_micromap_ext(struct vk_cmd_queue *queue
, const VkCopyMemoryToMicromapInfoEXT* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_TO_MICROMAP_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_TO_MICROMAP_EXT;
   if (pInfo) {
      cmd->u.copy_memory_to_micromap_ext.info = linear_alloc_child(queue->ctx, sizeof(VkCopyMemoryToMicromapInfoEXT));
      if (cmd->u.copy_memory_to_micromap_ext.info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_to_micromap_ext.info, pInfo, sizeof(VkCopyMemoryToMicromapInfoEXT));
   } else {
      cmd->u.copy_memory_to_micromap_ext.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_micromaps_properties_ext(struct vk_cmd_queue *queue
, uint32_t                                 micromapCount
, const VkMicromapEXT* pMicromaps
, VkQueryType        queryType
, VkQueryPool                              queryPool
, uint32_t                                 firstQuery
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_MICROMAPS_PROPERTIES_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_MICROMAPS_PROPERTIES_EXT;
   cmd->u.write_micromaps_properties_ext.micromap_count = micromapCount;
   if (pMicromaps) {
      cmd->u.write_micromaps_properties_ext.micromaps = linear_alloc_child(queue->ctx, sizeof(VkMicromapEXT) * ceil(micromapCount));
      if (cmd->u.write_micromaps_properties_ext.micromaps == NULL) return NULL;
      memcpy((void *)cmd->u.write_micromaps_properties_ext.micromaps, pMicromaps, sizeof(VkMicromapEXT) * ceil(micromapCount));
   } else {
      cmd->u.write_micromaps_properties_ext.micromaps = NULL;
   }
   cmd->u.write_micromaps_properties_ext.query_type = queryType;
   cmd->u.write_micromaps_properties_ext.query_pool = queryPool;
   cmd->u.write_micromaps_properties_ext.first_query = firstQuery;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_tile_memory_qcom(struct vk_cmd_queue *queue
, const VkTileMemoryBindInfoQCOM* pTileMemoryBindInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_TILE_MEMORY_QCOM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_TILE_MEMORY_QCOM;
   if (pTileMemoryBindInfo) {
      cmd->u.bind_tile_memory_qcom.tile_memory_bind_info = linear_alloc_child(queue->ctx, sizeof(VkTileMemoryBindInfoQCOM));
      if (cmd->u.bind_tile_memory_qcom.tile_memory_bind_info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_tile_memory_qcom.tile_memory_bind_info, pTileMemoryBindInfo, sizeof(VkTileMemoryBindInfoQCOM));
   } else {
      cmd->u.bind_tile_memory_qcom.tile_memory_bind_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_optical_flow_execute_nv(struct vk_cmd_queue *queue
, VkOpticalFlowSessionNV session
, const VkOpticalFlowExecuteInfoNV* pExecuteInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_OPTICAL_FLOW_EXECUTE_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_OPTICAL_FLOW_EXECUTE_NV;
   cmd->u.optical_flow_execute_nv.session = session;
   if (pExecuteInfo) {
      cmd->u.optical_flow_execute_nv.execute_info = linear_alloc_child(queue->ctx, sizeof(VkOpticalFlowExecuteInfoNV));
      if (cmd->u.optical_flow_execute_nv.execute_info == NULL) return NULL;
      memcpy((void *)cmd->u.optical_flow_execute_nv.execute_info, pExecuteInfo, sizeof(VkOpticalFlowExecuteInfoNV));
      VkOpticalFlowExecuteInfoNV *tmp_dst1 = (void *)cmd->u.optical_flow_execute_nv.execute_info;
      VkOpticalFlowExecuteInfoNV *tmp_src2 = (void *)pExecuteInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkRect2D) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkRect2D) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.optical_flow_execute_nv.execute_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias2_ext(struct vk_cmd_queue *queue
, const VkDepthBiasInfoEXT*         pDepthBiasInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_BIAS2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_BIAS2_EXT;
   if (pDepthBiasInfo) {
      cmd->u.set_depth_bias2_ext.depth_bias_info = linear_alloc_child(queue->ctx, sizeof(VkDepthBiasInfoEXT));
      if (cmd->u.set_depth_bias2_ext.depth_bias_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_depth_bias2_ext.depth_bias_info, pDepthBiasInfo, sizeof(VkDepthBiasInfoEXT));
      VkDepthBiasInfoEXT *tmp_dst1 = (void *)cmd->u.set_depth_bias2_ext.depth_bias_info;
      VkDepthBiasInfoEXT *tmp_src2 = (void *)pDepthBiasInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_DEPTH_BIAS_REPRESENTATION_INFO_EXT:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDepthBiasRepresentationInfoEXT));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDepthBiasRepresentationInfoEXT));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.set_depth_bias2_ext.depth_bias_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_shaders_ext(struct vk_cmd_queue *queue
, uint32_t stageCount
, const VkShaderStageFlagBits* pStages
, const VkShaderEXT* pShaders
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_SHADERS_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_SHADERS_EXT;
   cmd->u.bind_shaders_ext.stage_count = stageCount;
   if (pStages) {
      cmd->u.bind_shaders_ext.stages = linear_alloc_child(queue->ctx, sizeof(VkShaderStageFlagBits) * ceil(stageCount));
      if (cmd->u.bind_shaders_ext.stages == NULL) return NULL;
      memcpy((void *)cmd->u.bind_shaders_ext.stages, pStages, sizeof(VkShaderStageFlagBits) * ceil(stageCount));
   } else {
      cmd->u.bind_shaders_ext.stages = NULL;
   }
   if (pShaders) {
      cmd->u.bind_shaders_ext.shaders = linear_alloc_child(queue->ctx, sizeof(VkShaderEXT) * ceil(stageCount));
      if (cmd->u.bind_shaders_ext.shaders == NULL) return NULL;
      memcpy((void *)cmd->u.bind_shaders_ext.shaders, pShaders, sizeof(VkShaderEXT) * ceil(stageCount));
   } else {
      cmd->u.bind_shaders_ext.shaders = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}




struct vk_cmd_queue_entry *vk_enqueue_cmd_end_gpa_sample_amd(struct vk_cmd_queue *queue
, VkGpaSessionAMD                   gpaSession
, uint32_t                          sampleID
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_GPA_SAMPLE_AMD]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_GPA_SAMPLE_AMD;
   cmd->u.end_gpa_sample_amd.gpa_session = gpaSession;
   cmd->u.end_gpa_sample_amd.sample_id = sampleID;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_gpa_session_results_amd(struct vk_cmd_queue *queue
, VkGpaSessionAMD                   gpaSession
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_GPA_SESSION_RESULTS_AMD]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_GPA_SESSION_RESULTS_AMD;
   cmd->u.copy_gpa_session_results_amd.gpa_session = gpaSession;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_sets2(struct vk_cmd_queue *queue
, const VkBindDescriptorSetsInfo*   pBindDescriptorSetsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_DESCRIPTOR_SETS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_DESCRIPTOR_SETS2;
   if (pBindDescriptorSetsInfo) {
      cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info = linear_alloc_child(queue->ctx, sizeof(VkBindDescriptorSetsInfo));
      if (cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info, pBindDescriptorSetsInfo, sizeof(VkBindDescriptorSetsInfo));
      enqueue_pipeline_layout(queue, pBindDescriptorSetsInfo->layout);
      VkBindDescriptorSetsInfo *tmp_dst1 = (void *)cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info;
      VkBindDescriptorSetsInfo *tmp_src2 = (void *)pBindDescriptorSetsInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pDescriptorSets) {
         tmp_dst1->pDescriptorSets = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSet) * ceil(tmp_src2->descriptorSetCount));
         if (tmp_dst1->pDescriptorSets == NULL) return NULL;
         memcpy((void *)tmp_dst1->pDescriptorSets, tmp_src2->pDescriptorSets, sizeof(VkDescriptorSet) * ceil(tmp_src2->descriptorSetCount));
      }
      if (tmp_src2->pDynamicOffsets) {
         tmp_dst1->pDynamicOffsets = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src2->dynamicOffsetCount));
         if (tmp_dst1->pDynamicOffsets == NULL) return NULL;
         memcpy((void *)tmp_dst1->pDynamicOffsets, tmp_src2->pDynamicOffsets, sizeof(uint32_t) * ceil(tmp_src2->dynamicOffsetCount));
      }
   } else {
      cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_constants2(struct vk_cmd_queue *queue
, const VkPushConstantsInfo*        pPushConstantsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_CONSTANTS2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_CONSTANTS2;
   if (pPushConstantsInfo) {
      cmd->u.push_constants2.push_constants_info = linear_alloc_child(queue->ctx, sizeof(VkPushConstantsInfo));
      if (cmd->u.push_constants2.push_constants_info == NULL) return NULL;
      memcpy((void *)cmd->u.push_constants2.push_constants_info, pPushConstantsInfo, sizeof(VkPushConstantsInfo));
      enqueue_pipeline_layout(queue, pPushConstantsInfo->layout);
      VkPushConstantsInfo *tmp_dst1 = (void *)cmd->u.push_constants2.push_constants_info;
      VkPushConstantsInfo *tmp_src2 = (void *)pPushConstantsInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         case VK_STRUCTURE_TYPE_PUSH_CONSTANT_BANK_INFO_NV:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPushConstantBankInfoNV));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPushConstantBankInfoNV));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pValues) {
         tmp_dst1->pValues = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src2->size));
         if (tmp_dst1->pValues == NULL) return NULL;
         memcpy((void *)tmp_dst1->pValues, tmp_src2->pValues, 1 * ceil(tmp_src2->size));
      }
   } else {
      cmd->u.push_constants2.push_constants_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set2(struct vk_cmd_queue *queue
, const VkPushDescriptorSetInfo*    pPushDescriptorSetInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_DESCRIPTOR_SET2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_DESCRIPTOR_SET2;
   if (pPushDescriptorSetInfo) {
      cmd->u.push_descriptor_set2.push_descriptor_set_info = linear_alloc_child(queue->ctx, sizeof(VkPushDescriptorSetInfo));
      if (cmd->u.push_descriptor_set2.push_descriptor_set_info == NULL) return NULL;
      memcpy((void *)cmd->u.push_descriptor_set2.push_descriptor_set_info, pPushDescriptorSetInfo, sizeof(VkPushDescriptorSetInfo));
      enqueue_pipeline_layout(queue, pPushDescriptorSetInfo->layout);
      VkPushDescriptorSetInfo *tmp_dst1 = (void *)cmd->u.push_descriptor_set2.push_descriptor_set_info;
      VkPushDescriptorSetInfo *tmp_src2 = (void *)pPushDescriptorSetInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pDescriptorWrites) {
         tmp_dst1->pDescriptorWrites = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSet) * ceil(tmp_src2->descriptorWriteCount));
         if (tmp_dst1->pDescriptorWrites == NULL) return NULL;
         memcpy((void *)tmp_dst1->pDescriptorWrites, tmp_src2->pDescriptorWrites, sizeof(VkWriteDescriptorSet) * ceil(tmp_src2->descriptorWriteCount));
         VkWriteDescriptorSet *tmp_dst6 = (void *)tmp_dst1->pDescriptorWrites;
         VkWriteDescriptorSet *tmp_src7 = (void *)tmp_src2->pDescriptorWrites;
         for (uint32_t i8 = 0; i8 < tmp_src2->descriptorWriteCount; i8++) {
            VkWriteDescriptorSet *tmp_dst9 = tmp_dst6 + i8;
            VkWriteDescriptorSet *tmp_src10 = tmp_src7 + i8;
            const VkBaseInStructure *pnext = tmp_src10->pNext;
            void **dst_pnext_link = (void **)&tmp_dst9->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetInlineUniformBlock));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetInlineUniformBlock));
                  VkWriteDescriptorSetInlineUniformBlock *tmp_dst11 = (void *)(*dst_pnext_link);
                  VkWriteDescriptorSetInlineUniformBlock *tmp_src12 = (void *)pnext;
                  if (tmp_src12->pData) {
                     tmp_dst11->pData = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src12->dataSize));
                     if (tmp_dst11->pData == NULL) return NULL;
                     memcpy((void *)tmp_dst11->pData, tmp_src12->pData, 1 * ceil(tmp_src12->dataSize));
                  }
                  break;
               case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetAccelerationStructureKHR));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetAccelerationStructureKHR));
                  VkWriteDescriptorSetAccelerationStructureKHR *tmp_dst13 = (void *)(*dst_pnext_link);
                  VkWriteDescriptorSetAccelerationStructureKHR *tmp_src14 = (void *)pnext;
                  if (tmp_src14->pAccelerationStructures) {
                     tmp_dst13->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureKHR) * ceil(tmp_src14->accelerationStructureCount));
                     if (tmp_dst13->pAccelerationStructures == NULL) return NULL;
                     memcpy((void *)tmp_dst13->pAccelerationStructures, tmp_src14->pAccelerationStructures, sizeof(VkAccelerationStructureKHR) * ceil(tmp_src14->accelerationStructureCount));
                  }
                  break;
               case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetAccelerationStructureNV));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetAccelerationStructureNV));
                  VkWriteDescriptorSetAccelerationStructureNV *tmp_dst15 = (void *)(*dst_pnext_link);
                  VkWriteDescriptorSetAccelerationStructureNV *tmp_src16 = (void *)pnext;
                  if (tmp_src16->pAccelerationStructures) {
                     tmp_dst15->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkAccelerationStructureNV) * ceil(tmp_src16->accelerationStructureCount));
                     if (tmp_dst15->pAccelerationStructures == NULL) return NULL;
                     memcpy((void *)tmp_dst15->pAccelerationStructures, tmp_src16->pAccelerationStructures, sizeof(VkAccelerationStructureNV) * ceil(tmp_src16->accelerationStructureCount));
                  }
                  break;
               case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_PARTITIONED_ACCELERATION_STRUCTURE_NV:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetPartitionedAccelerationStructureNV));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetPartitionedAccelerationStructureNV));
                  VkWriteDescriptorSetPartitionedAccelerationStructureNV *tmp_dst17 = (void *)(*dst_pnext_link);
                  VkWriteDescriptorSetPartitionedAccelerationStructureNV *tmp_src18 = (void *)pnext;
                  if (tmp_src18->pAccelerationStructures) {
                     tmp_dst17->pAccelerationStructures = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddress) * ceil(tmp_src18->accelerationStructureCount));
                     if (tmp_dst17->pAccelerationStructures == NULL) return NULL;
                     memcpy((void *)tmp_dst17->pAccelerationStructures, tmp_src18->pAccelerationStructures, sizeof(VkDeviceAddress) * ceil(tmp_src18->accelerationStructureCount));
                  }
                  break;
               case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_TENSOR_ARM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkWriteDescriptorSetTensorARM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkWriteDescriptorSetTensorARM));
                  VkWriteDescriptorSetTensorARM *tmp_dst19 = (void *)(*dst_pnext_link);
                  VkWriteDescriptorSetTensorARM *tmp_src20 = (void *)pnext;
                  if (tmp_src20->pTensorViews) {
                     tmp_dst19->pTensorViews = linear_alloc_child(queue->ctx, sizeof(VkTensorViewARM) * ceil(tmp_src20->tensorViewCount));
                     if (tmp_dst19->pTensorViews == NULL) return NULL;
                     memcpy((void *)tmp_dst19->pTensorViews, tmp_src20->pTensorViews, sizeof(VkTensorViewARM) * ceil(tmp_src20->tensorViewCount));
                  }
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
            enqueue_VkWriteDescriptorSet(queue, tmp_dst9, tmp_src10);
         }
      }
   } else {
      cmd->u.push_descriptor_set2.push_descriptor_set_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set_with_template2(struct vk_cmd_queue *queue
, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE2]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE2;
   if (pPushDescriptorSetWithTemplateInfo) {
      cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info = linear_alloc_child(queue->ctx, sizeof(VkPushDescriptorSetWithTemplateInfo));
      if (cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info == NULL) return NULL;
      memcpy((void *)cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info, pPushDescriptorSetWithTemplateInfo, sizeof(VkPushDescriptorSetWithTemplateInfo));
      enqueue_descriptor_template(queue, pPushDescriptorSetWithTemplateInfo->descriptorUpdateTemplate);
      enqueue_pipeline_layout(queue, pPushDescriptorSetWithTemplateInfo->layout);
      VkPushDescriptorSetWithTemplateInfo *tmp_dst1 = (void *)cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info;
      VkPushDescriptorSetWithTemplateInfo *tmp_src2 = (void *)pPushDescriptorSetWithTemplateInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      tmp_dst1->pData = enqueue_push_descriptor_template_data(queue, tmp_src2->descriptorUpdateTemplate, tmp_src2->pData);
   } else {
      cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_descriptor_buffer_offsets2_ext(struct vk_cmd_queue *queue
, const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS2_EXT;
   if (pSetDescriptorBufferOffsetsInfo) {
      cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info = linear_alloc_child(queue->ctx, sizeof(VkSetDescriptorBufferOffsetsInfoEXT));
      if (cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info, pSetDescriptorBufferOffsetsInfo, sizeof(VkSetDescriptorBufferOffsetsInfoEXT));
      enqueue_pipeline_layout(queue, pSetDescriptorBufferOffsetsInfo->layout);
      VkSetDescriptorBufferOffsetsInfoEXT *tmp_dst1 = (void *)cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info;
      VkSetDescriptorBufferOffsetsInfoEXT *tmp_src2 = (void *)pSetDescriptorBufferOffsetsInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      if (tmp_src2->pBufferIndices) {
         tmp_dst1->pBufferIndices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src2->setCount));
         if (tmp_dst1->pBufferIndices == NULL) return NULL;
         memcpy((void *)tmp_dst1->pBufferIndices, tmp_src2->pBufferIndices, sizeof(uint32_t) * ceil(tmp_src2->setCount));
      }
      if (tmp_src2->pOffsets) {
         tmp_dst1->pOffsets = linear_alloc_child(queue->ctx, sizeof(VkDeviceSize) * ceil(tmp_src2->setCount));
         if (tmp_dst1->pOffsets == NULL) return NULL;
         memcpy((void *)tmp_dst1->pOffsets, tmp_src2->pOffsets, sizeof(VkDeviceSize) * ceil(tmp_src2->setCount));
      }
   } else {
      cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers2_ext(struct vk_cmd_queue *queue
, const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS2_EXT;
   if (pBindDescriptorBufferEmbeddedSamplersInfo) {
      cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info = linear_alloc_child(queue->ctx, sizeof(VkBindDescriptorBufferEmbeddedSamplersInfoEXT));
      if (cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info, pBindDescriptorBufferEmbeddedSamplersInfo, sizeof(VkBindDescriptorBufferEmbeddedSamplersInfoEXT));
      enqueue_pipeline_layout(queue, pBindDescriptorBufferEmbeddedSamplersInfo->layout);
      VkBindDescriptorBufferEmbeddedSamplersInfoEXT *tmp_dst1 = (void *)cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info;
      VkBindDescriptorBufferEmbeddedSamplersInfoEXT *tmp_src2 = (void *)pBindDescriptorBufferEmbeddedSamplersInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPipelineLayoutCreateInfo));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPipelineLayoutCreateInfo));
            VkPipelineLayoutCreateInfo *tmp_dst3 = (void *)(*dst_pnext_link);
            VkPipelineLayoutCreateInfo *tmp_src4 = (void *)pnext;
            if (tmp_src4->pSetLayouts) {
               tmp_dst3->pSetLayouts = linear_alloc_child(queue->ctx, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               if (tmp_dst3->pSetLayouts == NULL) return NULL;
               memcpy((void *)tmp_dst3->pSetLayouts, tmp_src4->pSetLayouts, sizeof(VkDescriptorSetLayout) * ceil(tmp_src4->setLayoutCount));
               for (unsigned i5 = 0; i5 < tmp_src4->setLayoutCount; i5++) {
                  enqueue_descriptor_layout(queue, tmp_src4->pSetLayouts[i5]);
               }
            }
            if (tmp_src4->pPushConstantRanges) {
               tmp_dst3->pPushConstantRanges = linear_alloc_child(queue->ctx, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
               if (tmp_dst3->pPushConstantRanges == NULL) return NULL;
               memcpy((void *)tmp_dst3->pPushConstantRanges, tmp_src4->pPushConstantRanges, sizeof(VkPushConstantRange) * ceil(tmp_src4->pushConstantRangeCount));
            }
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rendering_attachment_locations(struct vk_cmd_queue *queue
, const VkRenderingAttachmentLocationInfo* pLocationInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RENDERING_ATTACHMENT_LOCATIONS]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RENDERING_ATTACHMENT_LOCATIONS;
   if (pLocationInfo) {
      cmd->u.set_rendering_attachment_locations.location_info = linear_alloc_child(queue->ctx, sizeof(VkRenderingAttachmentLocationInfo));
      if (cmd->u.set_rendering_attachment_locations.location_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_rendering_attachment_locations.location_info, pLocationInfo, sizeof(VkRenderingAttachmentLocationInfo));
      VkRenderingAttachmentLocationInfo *tmp_dst1 = (void *)cmd->u.set_rendering_attachment_locations.location_info;
      VkRenderingAttachmentLocationInfo *tmp_src2 = (void *)pLocationInfo;
      if (tmp_src2->pColorAttachmentLocations) {
         tmp_dst1->pColorAttachmentLocations = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src2->colorAttachmentCount));
         if (tmp_dst1->pColorAttachmentLocations == NULL) return NULL;
         memcpy((void *)tmp_dst1->pColorAttachmentLocations, tmp_src2->pColorAttachmentLocations, sizeof(uint32_t) * ceil(tmp_src2->colorAttachmentCount));
      }
   } else {
      cmd->u.set_rendering_attachment_locations.location_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rendering_input_attachment_indices(struct vk_cmd_queue *queue
, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_RENDERING_INPUT_ATTACHMENT_INDICES]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_RENDERING_INPUT_ATTACHMENT_INDICES;
   if (pInputAttachmentIndexInfo) {
      cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info = linear_alloc_child(queue->ctx, sizeof(VkRenderingInputAttachmentIndexInfo));
      if (cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info == NULL) return NULL;
      memcpy((void *)cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info, pInputAttachmentIndexInfo, sizeof(VkRenderingInputAttachmentIndexInfo));
      VkRenderingInputAttachmentIndexInfo *tmp_dst1 = (void *)cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info;
      VkRenderingInputAttachmentIndexInfo *tmp_src2 = (void *)pInputAttachmentIndexInfo;
      if (tmp_src2->pColorAttachmentInputIndices) {
         tmp_dst1->pColorAttachmentInputIndices = linear_alloc_child(queue->ctx, sizeof(uint32_t) * ceil(tmp_src2->colorAttachmentCount));
         if (tmp_dst1->pColorAttachmentInputIndices == NULL) return NULL;
         memcpy((void *)tmp_dst1->pColorAttachmentInputIndices, tmp_src2->pColorAttachmentInputIndices, sizeof(uint32_t) * ceil(tmp_src2->colorAttachmentCount));
      }
      if (tmp_src2->pDepthInputAttachmentIndex) {
         tmp_dst1->pDepthInputAttachmentIndex = linear_alloc_child(queue->ctx, sizeof(uint32_t));
         if (tmp_dst1->pDepthInputAttachmentIndex == NULL) return NULL;
         memcpy((void *)tmp_dst1->pDepthInputAttachmentIndex, tmp_src2->pDepthInputAttachmentIndex, sizeof(uint32_t));
      }
      if (tmp_src2->pStencilInputAttachmentIndex) {
         tmp_dst1->pStencilInputAttachmentIndex = linear_alloc_child(queue->ctx, sizeof(uint32_t));
         if (tmp_dst1->pStencilInputAttachmentIndex == NULL) return NULL;
         memcpy((void *)tmp_dst1->pStencilInputAttachmentIndex, tmp_src2->pStencilInputAttachmentIndex, sizeof(uint32_t));
      }
   } else {
      cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clamp_range_ext(struct vk_cmd_queue *queue
, VkDepthClampModeEXT depthClampMode
, const VkDepthClampRangeEXT* pDepthClampRange
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DEPTH_CLAMP_RANGE_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DEPTH_CLAMP_RANGE_EXT;
   cmd->u.set_depth_clamp_range_ext.depth_clamp_mode = depthClampMode;
   if (pDepthClampRange) {
      cmd->u.set_depth_clamp_range_ext.depth_clamp_range = linear_alloc_child(queue->ctx, sizeof(VkDepthClampRangeEXT));
      if (cmd->u.set_depth_clamp_range_ext.depth_clamp_range == NULL) return NULL;
      memcpy((void *)cmd->u.set_depth_clamp_range_ext.depth_clamp_range, pDepthClampRange, sizeof(VkDepthClampRangeEXT));
   } else {
      cmd->u.set_depth_clamp_range_ext.depth_clamp_range = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_convert_cooperative_vector_matrix_nv(struct vk_cmd_queue *queue
, uint32_t infoCount
, const VkConvertCooperativeVectorMatrixInfoNV* pInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_CONVERT_COOPERATIVE_VECTOR_MATRIX_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_CONVERT_COOPERATIVE_VECTOR_MATRIX_NV;
   cmd->u.convert_cooperative_vector_matrix_nv.info_count = infoCount;
   if (pInfos) {
      cmd->u.convert_cooperative_vector_matrix_nv.infos = linear_alloc_child(queue->ctx, sizeof(VkConvertCooperativeVectorMatrixInfoNV) * ceil(infoCount));
      if (cmd->u.convert_cooperative_vector_matrix_nv.infos == NULL) return NULL;
      memcpy((void *)cmd->u.convert_cooperative_vector_matrix_nv.infos, pInfos, sizeof(VkConvertCooperativeVectorMatrixInfoNV) * ceil(infoCount));
      VkConvertCooperativeVectorMatrixInfoNV *tmp_dst1 = (void *)cmd->u.convert_cooperative_vector_matrix_nv.infos;
      VkConvertCooperativeVectorMatrixInfoNV *tmp_src2 = (void *)pInfos;
      for (uint32_t i3 = 0; i3 < infoCount; i3++) {
         VkConvertCooperativeVectorMatrixInfoNV *tmp_dst4 = tmp_dst1 + i3;
         VkConvertCooperativeVectorMatrixInfoNV *tmp_src5 = tmp_src2 + i3;
         if (tmp_src5->pDstSize) {
            tmp_dst4->pDstSize = linear_alloc_child(queue->ctx, sizeof(size_t));
            if (tmp_dst4->pDstSize == NULL) return NULL;
            memcpy((void *)tmp_dst4->pDstSize, tmp_src5->pDstSize, sizeof(size_t));
         }
      }
   } else {
      cmd->u.convert_cooperative_vector_matrix_nv.infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_tile_qcom(struct vk_cmd_queue *queue
, const VkDispatchTileInfoQCOM* pDispatchTileInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH_TILE_QCOM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH_TILE_QCOM;
   if (pDispatchTileInfo) {
      cmd->u.dispatch_tile_qcom.dispatch_tile_info = linear_alloc_child(queue->ctx, sizeof(VkDispatchTileInfoQCOM));
      if (cmd->u.dispatch_tile_qcom.dispatch_tile_info == NULL) return NULL;
      memcpy((void *)cmd->u.dispatch_tile_qcom.dispatch_tile_info, pDispatchTileInfo, sizeof(VkDispatchTileInfoQCOM));
   } else {
      cmd->u.dispatch_tile_qcom.dispatch_tile_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_per_tile_execution_qcom(struct vk_cmd_queue *queue
, const VkPerTileBeginInfoQCOM* pPerTileBeginInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_PER_TILE_EXECUTION_QCOM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_PER_TILE_EXECUTION_QCOM;
   if (pPerTileBeginInfo) {
      cmd->u.begin_per_tile_execution_qcom.per_tile_begin_info = linear_alloc_child(queue->ctx, sizeof(VkPerTileBeginInfoQCOM));
      if (cmd->u.begin_per_tile_execution_qcom.per_tile_begin_info == NULL) return NULL;
      memcpy((void *)cmd->u.begin_per_tile_execution_qcom.per_tile_begin_info, pPerTileBeginInfo, sizeof(VkPerTileBeginInfoQCOM));
   } else {
      cmd->u.begin_per_tile_execution_qcom.per_tile_begin_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_per_tile_execution_qcom(struct vk_cmd_queue *queue
, const VkPerTileEndInfoQCOM* pPerTileEndInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_PER_TILE_EXECUTION_QCOM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_PER_TILE_EXECUTION_QCOM;
   if (pPerTileEndInfo) {
      cmd->u.end_per_tile_execution_qcom.per_tile_end_info = linear_alloc_child(queue->ctx, sizeof(VkPerTileEndInfoQCOM));
      if (cmd->u.end_per_tile_execution_qcom.per_tile_end_info == NULL) return NULL;
      memcpy((void *)cmd->u.end_per_tile_execution_qcom.per_tile_end_info, pPerTileEndInfo, sizeof(VkPerTileEndInfoQCOM));
   } else {
      cmd->u.end_per_tile_execution_qcom.per_tile_end_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_shader_instrumentation_arm(struct vk_cmd_queue *queue
, VkShaderInstrumentationARM instrumentation
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_SHADER_INSTRUMENTATION_ARM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_SHADER_INSTRUMENTATION_ARM;
   cmd->u.begin_shader_instrumentation_arm.instrumentation = instrumentation;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_shader_instrumentation_arm(struct vk_cmd_queue *queue
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_SHADER_INSTRUMENTATION_ARM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_SHADER_INSTRUMENTATION_ARM;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_tensor_arm(struct vk_cmd_queue *queue
, const VkCopyTensorInfoARM* pCopyTensorInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_TENSOR_ARM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_TENSOR_ARM;
   if (pCopyTensorInfo) {
      cmd->u.copy_tensor_arm.copy_tensor_info = linear_alloc_child(queue->ctx, sizeof(VkCopyTensorInfoARM));
      if (cmd->u.copy_tensor_arm.copy_tensor_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_tensor_arm.copy_tensor_info, pCopyTensorInfo, sizeof(VkCopyTensorInfoARM));
      VkCopyTensorInfoARM *tmp_dst1 = (void *)cmd->u.copy_tensor_arm.copy_tensor_info;
      VkCopyTensorInfoARM *tmp_src2 = (void *)pCopyTensorInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkTensorCopyARM) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkTensorCopyARM) * ceil(tmp_src2->regionCount));
         VkTensorCopyARM *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkTensorCopyARM *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkTensorCopyARM *tmp_dst6 = tmp_dst3 + i5;
            VkTensorCopyARM *tmp_src7 = tmp_src4 + i5;
            if (tmp_src7->pSrcOffset) {
               tmp_dst6->pSrcOffset = linear_alloc_child(queue->ctx, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
               if (tmp_dst6->pSrcOffset == NULL) return NULL;
               memcpy((void *)tmp_dst6->pSrcOffset, tmp_src7->pSrcOffset, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
            }
            if (tmp_src7->pDstOffset) {
               tmp_dst6->pDstOffset = linear_alloc_child(queue->ctx, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
               if (tmp_dst6->pDstOffset == NULL) return NULL;
               memcpy((void *)tmp_dst6->pDstOffset, tmp_src7->pDstOffset, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
            }
            if (tmp_src7->pExtent) {
               tmp_dst6->pExtent = linear_alloc_child(queue->ctx, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
               if (tmp_dst6->pExtent == NULL) return NULL;
               memcpy((void *)tmp_dst6->pExtent, tmp_src7->pExtent, sizeof(uint64_t) * ceil(tmp_src7->dimensionCount));
            }
         }
      }
   } else {
      cmd->u.copy_tensor_arm.copy_tensor_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_data_graph_arm(struct vk_cmd_queue *queue
, VkDataGraphPipelineSessionARM session
, const VkDataGraphPipelineDispatchInfoARM* pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH_DATA_GRAPH_ARM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH_DATA_GRAPH_ARM;
   cmd->u.dispatch_data_graph_arm.session = session;
   if (pInfo) {
      cmd->u.dispatch_data_graph_arm.info = linear_alloc_child(queue->ctx, sizeof(VkDataGraphPipelineDispatchInfoARM));
      if (cmd->u.dispatch_data_graph_arm.info == NULL) return NULL;
      memcpy((void *)cmd->u.dispatch_data_graph_arm.info, pInfo, sizeof(VkDataGraphPipelineDispatchInfoARM));
      VkDataGraphPipelineDispatchInfoARM *tmp_dst1 = (void *)cmd->u.dispatch_data_graph_arm.info;
      VkDataGraphPipelineDispatchInfoARM *tmp_src2 = (void *)pInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_DATA_GRAPH_PIPELINE_OPTICAL_FLOW_DISPATCH_INFO_ARM:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkDataGraphPipelineOpticalFlowDispatchInfoARM));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkDataGraphPipelineOpticalFlowDispatchInfoARM));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
   } else {
      cmd->u.dispatch_data_graph_arm.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_compute_occupancy_priority_nv(struct vk_cmd_queue *queue
, const VkComputeOccupancyPriorityParametersNV* pParameters
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_COMPUTE_OCCUPANCY_PRIORITY_NV]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_COMPUTE_OCCUPANCY_PRIORITY_NV;
   if (pParameters) {
      cmd->u.set_compute_occupancy_priority_nv.parameters = linear_alloc_child(queue->ctx, sizeof(VkComputeOccupancyPriorityParametersNV));
      if (cmd->u.set_compute_occupancy_priority_nv.parameters == NULL) return NULL;
      memcpy((void *)cmd->u.set_compute_occupancy_priority_nv.parameters, pParameters, sizeof(VkComputeOccupancyPriorityParametersNV));
   } else {
      cmd->u.set_compute_occupancy_priority_nv.parameters = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_sampler_heap_ext(struct vk_cmd_queue *queue
, const VkBindHeapInfoEXT*                            pBindInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_SAMPLER_HEAP_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_SAMPLER_HEAP_EXT;
   if (pBindInfo) {
      cmd->u.bind_sampler_heap_ext.bind_info = linear_alloc_child(queue->ctx, sizeof(VkBindHeapInfoEXT));
      if (cmd->u.bind_sampler_heap_ext.bind_info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_sampler_heap_ext.bind_info, pBindInfo, sizeof(VkBindHeapInfoEXT));
   } else {
      cmd->u.bind_sampler_heap_ext.bind_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_resource_heap_ext(struct vk_cmd_queue *queue
, const VkBindHeapInfoEXT*                            pBindInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_RESOURCE_HEAP_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_RESOURCE_HEAP_EXT;
   if (pBindInfo) {
      cmd->u.bind_resource_heap_ext.bind_info = linear_alloc_child(queue->ctx, sizeof(VkBindHeapInfoEXT));
      if (cmd->u.bind_resource_heap_ext.bind_info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_resource_heap_ext.bind_info, pBindInfo, sizeof(VkBindHeapInfoEXT));
   } else {
      cmd->u.bind_resource_heap_ext.bind_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_push_data_ext(struct vk_cmd_queue *queue
, const VkPushDataInfoEXT*                            pPushDataInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_PUSH_DATA_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_PUSH_DATA_EXT;
   if (pPushDataInfo) {
      cmd->u.push_data_ext.push_data_info = linear_alloc_child(queue->ctx, sizeof(VkPushDataInfoEXT));
      if (cmd->u.push_data_ext.push_data_info == NULL) return NULL;
      memcpy((void *)cmd->u.push_data_ext.push_data_info, pPushDataInfo, sizeof(VkPushDataInfoEXT));
      VkPushDataInfoEXT *tmp_dst1 = (void *)cmd->u.push_data_ext.push_data_info;
      VkPushDataInfoEXT *tmp_src2 = (void *)pPushDataInfo;
      const VkBaseInStructure *pnext = tmp_src2->pNext;
      void **dst_pnext_link = (void **)&tmp_dst1->pNext;
      while (pnext) {
         switch ((int32_t)pnext->sType) {
         case VK_STRUCTURE_TYPE_PUSH_CONSTANT_BANK_INFO_NV:
            (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkPushConstantBankInfoNV));
            if ((*dst_pnext_link) == NULL) return NULL;
            memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkPushConstantBankInfoNV));
            break;
         }
         pnext = pnext->pNext;
         dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
      }
      VkHostAddressRangeConstEXT *tmp_dst3 = (void *)&tmp_dst1->data;
      VkHostAddressRangeConstEXT *tmp_src4 = (void *)&tmp_src2->data;
      if (tmp_src4->address) {
         tmp_dst3->address = linear_alloc_child(queue->ctx, 1 * ceil(tmp_src4->size));
         if (tmp_dst3->address == NULL) return NULL;
         memcpy((void *)tmp_dst3->address, tmp_src4->address, 1 * ceil(tmp_src4->size));
      }
   } else {
      cmd->u.push_data_ext.push_data_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_khr(struct vk_cmd_queue *queue
, const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_KHR;
   if (pCopyMemoryInfo) {
      cmd->u.copy_memory_khr.copy_memory_info = linear_alloc_child(queue->ctx, sizeof(VkCopyDeviceMemoryInfoKHR));
      if (cmd->u.copy_memory_khr.copy_memory_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_khr.copy_memory_info, pCopyMemoryInfo, sizeof(VkCopyDeviceMemoryInfoKHR));
      VkCopyDeviceMemoryInfoKHR *tmp_dst1 = (void *)cmd->u.copy_memory_khr.copy_memory_info;
      VkCopyDeviceMemoryInfoKHR *tmp_src2 = (void *)pCopyMemoryInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkDeviceMemoryCopyKHR) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkDeviceMemoryCopyKHR) * ceil(tmp_src2->regionCount));
      }
   } else {
      cmd->u.copy_memory_khr.copy_memory_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_khr(struct vk_cmd_queue *queue
, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_MEMORY_TO_IMAGE_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_MEMORY_TO_IMAGE_KHR;
   if (pCopyMemoryInfo) {
      cmd->u.copy_memory_to_image_khr.copy_memory_info = linear_alloc_child(queue->ctx, sizeof(VkCopyDeviceMemoryImageInfoKHR));
      if (cmd->u.copy_memory_to_image_khr.copy_memory_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_memory_to_image_khr.copy_memory_info, pCopyMemoryInfo, sizeof(VkCopyDeviceMemoryImageInfoKHR));
      VkCopyDeviceMemoryImageInfoKHR *tmp_dst1 = (void *)cmd->u.copy_memory_to_image_khr.copy_memory_info;
      VkCopyDeviceMemoryImageInfoKHR *tmp_src2 = (void *)pCopyMemoryInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkDeviceMemoryImageCopyKHR) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkDeviceMemoryImageCopyKHR) * ceil(tmp_src2->regionCount));
         VkDeviceMemoryImageCopyKHR *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkDeviceMemoryImageCopyKHR *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkDeviceMemoryImageCopyKHR *tmp_dst6 = tmp_dst3 + i5;
            VkDeviceMemoryImageCopyKHR *tmp_src7 = tmp_src4 + i5;
            const VkBaseInStructure *pnext = tmp_src7->pNext;
            void **dst_pnext_link = (void **)&tmp_dst6->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkCopyCommandTransformInfoQCOM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkCopyCommandTransformInfoQCOM));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.copy_memory_to_image_khr.copy_memory_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_memory_khr(struct vk_cmd_queue *queue
, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_IMAGE_TO_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_IMAGE_TO_MEMORY_KHR;
   if (pCopyMemoryInfo) {
      cmd->u.copy_image_to_memory_khr.copy_memory_info = linear_alloc_child(queue->ctx, sizeof(VkCopyDeviceMemoryImageInfoKHR));
      if (cmd->u.copy_image_to_memory_khr.copy_memory_info == NULL) return NULL;
      memcpy((void *)cmd->u.copy_image_to_memory_khr.copy_memory_info, pCopyMemoryInfo, sizeof(VkCopyDeviceMemoryImageInfoKHR));
      VkCopyDeviceMemoryImageInfoKHR *tmp_dst1 = (void *)cmd->u.copy_image_to_memory_khr.copy_memory_info;
      VkCopyDeviceMemoryImageInfoKHR *tmp_src2 = (void *)pCopyMemoryInfo;
      if (tmp_src2->pRegions) {
         tmp_dst1->pRegions = linear_alloc_child(queue->ctx, sizeof(VkDeviceMemoryImageCopyKHR) * ceil(tmp_src2->regionCount));
         if (tmp_dst1->pRegions == NULL) return NULL;
         memcpy((void *)tmp_dst1->pRegions, tmp_src2->pRegions, sizeof(VkDeviceMemoryImageCopyKHR) * ceil(tmp_src2->regionCount));
         VkDeviceMemoryImageCopyKHR *tmp_dst3 = (void *)tmp_dst1->pRegions;
         VkDeviceMemoryImageCopyKHR *tmp_src4 = (void *)tmp_src2->pRegions;
         for (uint32_t i5 = 0; i5 < tmp_src2->regionCount; i5++) {
            VkDeviceMemoryImageCopyKHR *tmp_dst6 = tmp_dst3 + i5;
            VkDeviceMemoryImageCopyKHR *tmp_src7 = tmp_src4 + i5;
            const VkBaseInStructure *pnext = tmp_src7->pNext;
            void **dst_pnext_link = (void **)&tmp_dst6->pNext;
            while (pnext) {
               switch ((int32_t)pnext->sType) {
               case VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM:
                  (*dst_pnext_link) = linear_alloc_child(queue->ctx, sizeof(VkCopyCommandTransformInfoQCOM));
                  if ((*dst_pnext_link) == NULL) return NULL;
                  memcpy((void *)(*dst_pnext_link), pnext, sizeof(VkCopyCommandTransformInfoQCOM));
                  break;
               }
               pnext = pnext->pNext;
               dst_pnext_link = (void **)&((VkBaseOutStructure *)*dst_pnext_link)->pNext;
            }
         }
      }
   } else {
      cmd->u.copy_image_to_memory_khr.copy_memory_info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_update_memory_khr(struct vk_cmd_queue *queue
, const VkDeviceAddressRangeKHR*      pDstRange
, VkAddressCommandFlagsKHR dstFlags
, VkDeviceSize                        dataSize
, const void*          pData
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_UPDATE_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_UPDATE_MEMORY_KHR;
   if (pDstRange) {
      cmd->u.update_memory_khr.dst_range = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddressRangeKHR));
      if (cmd->u.update_memory_khr.dst_range == NULL) return NULL;
      memcpy((void *)cmd->u.update_memory_khr.dst_range, pDstRange, sizeof(VkDeviceAddressRangeKHR));
   } else {
      cmd->u.update_memory_khr.dst_range = NULL;
   }
   cmd->u.update_memory_khr.dst_flags = dstFlags;
   cmd->u.update_memory_khr.data_size = dataSize;
   if (pData) {
      cmd->u.update_memory_khr.data = linear_alloc_child(queue->ctx, 1 * ceil(dataSize));
      if (cmd->u.update_memory_khr.data == NULL) return NULL;
      memcpy((void *)cmd->u.update_memory_khr.data, pData, 1 * ceil(dataSize));
   } else {
      cmd->u.update_memory_khr.data = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_fill_memory_khr(struct vk_cmd_queue *queue
, const VkDeviceAddressRangeKHR*      pDstRange
, VkAddressCommandFlagsKHR dstFlags
, uint32_t                            data
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_FILL_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_FILL_MEMORY_KHR;
   if (pDstRange) {
      cmd->u.fill_memory_khr.dst_range = linear_alloc_child(queue->ctx, sizeof(VkDeviceAddressRangeKHR));
      if (cmd->u.fill_memory_khr.dst_range == NULL) return NULL;
      memcpy((void *)cmd->u.fill_memory_khr.dst_range, pDstRange, sizeof(VkDeviceAddressRangeKHR));
   } else {
      cmd->u.fill_memory_khr.dst_range = NULL;
   }
   cmd->u.fill_memory_khr.dst_flags = dstFlags;
   cmd->u.fill_memory_khr.data = data;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_query_pool_results_to_memory_khr(struct vk_cmd_queue *queue
, VkQueryPool                         queryPool
, uint32_t                            firstQuery
, uint32_t                            queryCount
, const VkStridedDeviceAddressRangeKHR* pDstRange
, VkAddressCommandFlagsKHR    dstFlags
, VkQueryResultFlags  queryResultFlags
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_COPY_QUERY_POOL_RESULTS_TO_MEMORY_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_COPY_QUERY_POOL_RESULTS_TO_MEMORY_KHR;
   cmd->u.copy_query_pool_results_to_memory_khr.query_pool = queryPool;
   cmd->u.copy_query_pool_results_to_memory_khr.first_query = firstQuery;
   cmd->u.copy_query_pool_results_to_memory_khr.query_count = queryCount;
   if (pDstRange) {
      cmd->u.copy_query_pool_results_to_memory_khr.dst_range = linear_alloc_child(queue->ctx, sizeof(VkStridedDeviceAddressRangeKHR));
      if (cmd->u.copy_query_pool_results_to_memory_khr.dst_range == NULL) return NULL;
      memcpy((void *)cmd->u.copy_query_pool_results_to_memory_khr.dst_range, pDstRange, sizeof(VkStridedDeviceAddressRangeKHR));
   } else {
      cmd->u.copy_query_pool_results_to_memory_khr.dst_range = NULL;
   }
   cmd->u.copy_query_pool_results_to_memory_khr.dst_flags = dstFlags;
   cmd->u.copy_query_pool_results_to_memory_khr.query_result_flags = queryResultFlags;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_conditional_rendering2_ext(struct vk_cmd_queue *queue
, const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_CONDITIONAL_RENDERING2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_CONDITIONAL_RENDERING2_EXT;
   if (pConditionalRenderingBegin) {
      cmd->u.begin_conditional_rendering2_ext.conditional_rendering_begin = linear_alloc_child(queue->ctx, sizeof(VkConditionalRenderingBeginInfo2EXT));
      if (cmd->u.begin_conditional_rendering2_ext.conditional_rendering_begin == NULL) return NULL;
      memcpy((void *)cmd->u.begin_conditional_rendering2_ext.conditional_rendering_begin, pConditionalRenderingBegin, sizeof(VkConditionalRenderingBeginInfo2EXT));
   } else {
      cmd->u.begin_conditional_rendering2_ext.conditional_rendering_begin = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_transform_feedback_buffers2_ext(struct vk_cmd_queue *queue
, uint32_t                            firstBinding
, uint32_t                            bindingCount
, const VkBindTransformFeedbackBuffer2InfoEXT* pBindingInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS2_EXT;
   cmd->u.bind_transform_feedback_buffers2_ext.first_binding = firstBinding;
   cmd->u.bind_transform_feedback_buffers2_ext.binding_count = bindingCount;
   if (pBindingInfos) {
      cmd->u.bind_transform_feedback_buffers2_ext.binding_infos = linear_alloc_child(queue->ctx, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(bindingCount));
      if (cmd->u.bind_transform_feedback_buffers2_ext.binding_infos == NULL) return NULL;
      memcpy((void *)cmd->u.bind_transform_feedback_buffers2_ext.binding_infos, pBindingInfos, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(bindingCount));
   } else {
      cmd->u.bind_transform_feedback_buffers2_ext.binding_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_transform_feedback2_ext(struct vk_cmd_queue *queue
, uint32_t                            firstCounterRange
, uint32_t            counterRangeCount
, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BEGIN_TRANSFORM_FEEDBACK2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BEGIN_TRANSFORM_FEEDBACK2_EXT;
   cmd->u.begin_transform_feedback2_ext.first_counter_range = firstCounterRange;
   cmd->u.begin_transform_feedback2_ext.counter_range_count = counterRangeCount;
   if (pCounterInfos) {
      cmd->u.begin_transform_feedback2_ext.counter_infos = linear_alloc_child(queue->ctx, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(counterRangeCount));
      if (cmd->u.begin_transform_feedback2_ext.counter_infos == NULL) return NULL;
      memcpy((void *)cmd->u.begin_transform_feedback2_ext.counter_infos, pCounterInfos, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(counterRangeCount));
   } else {
      cmd->u.begin_transform_feedback2_ext.counter_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_end_transform_feedback2_ext(struct vk_cmd_queue *queue
, uint32_t                            firstCounterRange
, uint32_t            counterRangeCount
, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_END_TRANSFORM_FEEDBACK2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_END_TRANSFORM_FEEDBACK2_EXT;
   cmd->u.end_transform_feedback2_ext.first_counter_range = firstCounterRange;
   cmd->u.end_transform_feedback2_ext.counter_range_count = counterRangeCount;
   if (pCounterInfos) {
      cmd->u.end_transform_feedback2_ext.counter_infos = linear_alloc_child(queue->ctx, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(counterRangeCount));
      if (cmd->u.end_transform_feedback2_ext.counter_infos == NULL) return NULL;
      memcpy((void *)cmd->u.end_transform_feedback2_ext.counter_infos, pCounterInfos, sizeof(VkBindTransformFeedbackBuffer2InfoEXT) * ceil(counterRangeCount));
   } else {
      cmd->u.end_transform_feedback2_ext.counter_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_byte_count2_ext(struct vk_cmd_queue *queue
, uint32_t instanceCount
, uint32_t firstInstance
, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo
, uint32_t counterOffset
, uint32_t vertexStride
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT_BYTE_COUNT2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT_BYTE_COUNT2_EXT;
   cmd->u.draw_indirect_byte_count2_ext.instance_count = instanceCount;
   cmd->u.draw_indirect_byte_count2_ext.first_instance = firstInstance;
   if (pCounterInfo) {
      cmd->u.draw_indirect_byte_count2_ext.counter_info = linear_alloc_child(queue->ctx, sizeof(VkBindTransformFeedbackBuffer2InfoEXT));
      if (cmd->u.draw_indirect_byte_count2_ext.counter_info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_indirect_byte_count2_ext.counter_info, pCounterInfo, sizeof(VkBindTransformFeedbackBuffer2InfoEXT));
   } else {
      cmd->u.draw_indirect_byte_count2_ext.counter_info = NULL;
   }
   cmd->u.draw_indirect_byte_count2_ext.counter_offset = counterOffset;
   cmd->u.draw_indirect_byte_count2_ext.vertex_stride = vertexStride;

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_write_marker_to_memory_amd(struct vk_cmd_queue *queue
, const VkMemoryMarkerInfoAMD*         pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_WRITE_MARKER_TO_MEMORY_AMD]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_WRITE_MARKER_TO_MEMORY_AMD;
   if (pInfo) {
      cmd->u.write_marker_to_memory_amd.info = linear_alloc_child(queue->ctx, sizeof(VkMemoryMarkerInfoAMD));
      if (cmd->u.write_marker_to_memory_amd.info == NULL) return NULL;
      memcpy((void *)cmd->u.write_marker_to_memory_amd.info, pInfo, sizeof(VkMemoryMarkerInfoAMD));
   } else {
      cmd->u.write_marker_to_memory_amd.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer3_khr(struct vk_cmd_queue *queue
, const VkBindIndexBuffer3InfoKHR*    pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_INDEX_BUFFER3_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_INDEX_BUFFER3_KHR;
   if (pInfo) {
      cmd->u.bind_index_buffer3_khr.info = linear_alloc_child(queue->ctx, sizeof(VkBindIndexBuffer3InfoKHR));
      if (cmd->u.bind_index_buffer3_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.bind_index_buffer3_khr.info, pInfo, sizeof(VkBindIndexBuffer3InfoKHR));
   } else {
      cmd->u.bind_index_buffer3_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers3_khr(struct vk_cmd_queue *queue
, uint32_t                            firstBinding
, uint32_t                            bindingCount
, const VkBindVertexBuffer3InfoKHR* pBindingInfos
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_BIND_VERTEX_BUFFERS3_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_BIND_VERTEX_BUFFERS3_KHR;
   cmd->u.bind_vertex_buffers3_khr.first_binding = firstBinding;
   cmd->u.bind_vertex_buffers3_khr.binding_count = bindingCount;
   if (pBindingInfos) {
      cmd->u.bind_vertex_buffers3_khr.binding_infos = linear_alloc_child(queue->ctx, sizeof(VkBindVertexBuffer3InfoKHR) * ceil(bindingCount));
      if (cmd->u.bind_vertex_buffers3_khr.binding_infos == NULL) return NULL;
      memcpy((void *)cmd->u.bind_vertex_buffers3_khr.binding_infos, pBindingInfos, sizeof(VkBindVertexBuffer3InfoKHR) * ceil(bindingCount));
   } else {
      cmd->u.bind_vertex_buffers3_khr.binding_infos = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect2_khr(struct vk_cmd_queue *queue
, const VkDrawIndirect2InfoKHR*       pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT2_KHR;
   if (pInfo) {
      cmd->u.draw_indirect2_khr.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirect2InfoKHR));
      if (cmd->u.draw_indirect2_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_indirect2_khr.info, pInfo, sizeof(VkDrawIndirect2InfoKHR));
   } else {
      cmd->u.draw_indirect2_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect2_khr(struct vk_cmd_queue *queue
, const VkDrawIndirect2InfoKHR*       pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDEXED_INDIRECT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDEXED_INDIRECT2_KHR;
   if (pInfo) {
      cmd->u.draw_indexed_indirect2_khr.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirect2InfoKHR));
      if (cmd->u.draw_indexed_indirect2_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_indexed_indirect2_khr.info, pInfo, sizeof(VkDrawIndirect2InfoKHR));
   } else {
      cmd->u.draw_indexed_indirect2_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_count2_khr(struct vk_cmd_queue *queue
, const VkDrawIndirectCount2InfoKHR*  pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDIRECT_COUNT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDIRECT_COUNT2_KHR;
   if (pInfo) {
      cmd->u.draw_indirect_count2_khr.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirectCount2InfoKHR));
      if (cmd->u.draw_indirect_count2_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_indirect_count2_khr.info, pInfo, sizeof(VkDrawIndirectCount2InfoKHR));
   } else {
      cmd->u.draw_indirect_count2_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect_count2_khr(struct vk_cmd_queue *queue
, const VkDrawIndirectCount2InfoKHR*  pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_INDEXED_INDIRECT_COUNT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_INDEXED_INDIRECT_COUNT2_KHR;
   if (pInfo) {
      cmd->u.draw_indexed_indirect_count2_khr.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirectCount2InfoKHR));
      if (cmd->u.draw_indexed_indirect_count2_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_indexed_indirect_count2_khr.info, pInfo, sizeof(VkDrawIndirectCount2InfoKHR));
   } else {
      cmd->u.draw_indexed_indirect_count2_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect2_ext(struct vk_cmd_queue *queue
, const VkDrawIndirect2InfoKHR*       pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT2_EXT;
   if (pInfo) {
      cmd->u.draw_mesh_tasks_indirect2_ext.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirect2InfoKHR));
      if (cmd->u.draw_mesh_tasks_indirect2_ext.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_mesh_tasks_indirect2_ext.info, pInfo, sizeof(VkDrawIndirect2InfoKHR));
   } else {
      cmd->u.draw_mesh_tasks_indirect2_ext.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count2_ext(struct vk_cmd_queue *queue
, const VkDrawIndirectCount2InfoKHR*  pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT2_EXT]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT2_EXT;
   if (pInfo) {
      cmd->u.draw_mesh_tasks_indirect_count2_ext.info = linear_alloc_child(queue->ctx, sizeof(VkDrawIndirectCount2InfoKHR));
      if (cmd->u.draw_mesh_tasks_indirect_count2_ext.info == NULL) return NULL;
      memcpy((void *)cmd->u.draw_mesh_tasks_indirect_count2_ext.info, pInfo, sizeof(VkDrawIndirectCount2InfoKHR));
   } else {
      cmd->u.draw_mesh_tasks_indirect_count2_ext.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_indirect2_khr(struct vk_cmd_queue *queue
, const VkDispatchIndirect2InfoKHR*   pInfo
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_DISPATCH_INDIRECT2_KHR]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_DISPATCH_INDIRECT2_KHR;
   if (pInfo) {
      cmd->u.dispatch_indirect2_khr.info = linear_alloc_child(queue->ctx, sizeof(VkDispatchIndirect2InfoKHR));
      if (cmd->u.dispatch_indirect2_khr.info == NULL) return NULL;
      memcpy((void *)cmd->u.dispatch_indirect2_khr.info, pInfo, sizeof(VkDispatchIndirect2InfoKHR));
   } else {
      cmd->u.dispatch_indirect2_khr.info = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}

struct vk_cmd_queue_entry *vk_enqueue_cmd_set_dispatch_parameters_arm(struct vk_cmd_queue *queue
, const VkDispatchParametersARM*    pDispatchParameters
)
{
   struct vk_cmd_queue_entry *cmd = linear_alloc_child(queue->ctx, vk_cmd_queue_type_sizes[VK_CMD_SET_DISPATCH_PARAMETERS_ARM]);
   if (!cmd) return NULL;

   cmd->type = VK_CMD_SET_DISPATCH_PARAMETERS_ARM;
   if (pDispatchParameters) {
      cmd->u.set_dispatch_parameters_arm.dispatch_parameters = linear_alloc_child(queue->ctx, sizeof(VkDispatchParametersARM));
      if (cmd->u.set_dispatch_parameters_arm.dispatch_parameters == NULL) return NULL;
      memcpy((void *)cmd->u.set_dispatch_parameters_arm.dispatch_parameters, pDispatchParameters, sizeof(VkDispatchParametersARM));
   } else {
      cmd->u.set_dispatch_parameters_arm.dispatch_parameters = NULL;
   }

   list_addtail(&cmd->cmd_link, &queue->cmds);
   return cmd;
}


void
vk_free_queue(struct vk_cmd_queue *queue)
{
   struct vk_command_buffer *cmd_buffer =
      container_of(queue, struct vk_command_buffer, cmd_queue);

   util_dynarray_foreach(&queue->pipeline_layouts, void*, layout)
      vk_pipeline_layout_unref(cmd_buffer->base.device, *layout);
   util_dynarray_fini(&queue->pipeline_layouts);
   util_dynarray_foreach(&queue->update_templates, void*, templ)
      vk_descriptor_update_template_unref(cmd_buffer->base.device, *templ);
   util_dynarray_fini(&queue->update_templates);
   util_dynarray_foreach(&queue->set_layouts, void*, layout)
      vk_descriptor_set_layout_unref(cmd_buffer->base.device, *layout);
   util_dynarray_fini(&queue->set_layouts);
   linear_free_context(queue->ctx);
}

void
vk_cmd_queue_execute(struct vk_cmd_queue *queue,
                     VkCommandBuffer commandBuffer,
                     const struct vk_device_dispatch_table *disp)
{
   list_for_each_entry(struct vk_cmd_queue_entry, cmd, &queue->cmds, cmd_link) {
      switch (cmd->type) {
      case VK_CMD_BIND_PIPELINE:
          disp->CmdBindPipeline(commandBuffer
             , cmd->u.bind_pipeline.pipeline_bind_point             , cmd->u.bind_pipeline.pipeline          );
          break;
      case VK_CMD_SET_PRIMITIVE_RESTART_INDEX_EXT:
          disp->CmdSetPrimitiveRestartIndexEXT(commandBuffer
             , cmd->u.set_primitive_restart_index_ext.primitive_restart_index          );
          break;
      case VK_CMD_SET_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT:
          disp->CmdSetAttachmentFeedbackLoopEnableEXT(commandBuffer
             , cmd->u.set_attachment_feedback_loop_enable_ext.aspect_mask          );
          break;
      case VK_CMD_SET_VIEWPORT:
          disp->CmdSetViewport(commandBuffer
             , cmd->u.set_viewport.first_viewport             , cmd->u.set_viewport.viewport_count             , cmd->u.set_viewport.viewports          );
          break;
      case VK_CMD_SET_SCISSOR:
          disp->CmdSetScissor(commandBuffer
             , cmd->u.set_scissor.first_scissor             , cmd->u.set_scissor.scissor_count             , cmd->u.set_scissor.scissors          );
          break;
      case VK_CMD_SET_LINE_WIDTH:
          disp->CmdSetLineWidth(commandBuffer
             , cmd->u.set_line_width.line_width          );
          break;
      case VK_CMD_SET_DEPTH_BIAS:
          disp->CmdSetDepthBias(commandBuffer
             , cmd->u.set_depth_bias.depth_bias_constant_factor             , cmd->u.set_depth_bias.depth_bias_clamp             , cmd->u.set_depth_bias.depth_bias_slope_factor          );
          break;
      case VK_CMD_SET_BLEND_CONSTANTS:
          disp->CmdSetBlendConstants(commandBuffer
             , cmd->u.set_blend_constants.blend_constants          );
          break;
      case VK_CMD_SET_DEPTH_BOUNDS:
          disp->CmdSetDepthBounds(commandBuffer
             , cmd->u.set_depth_bounds.min_depth_bounds             , cmd->u.set_depth_bounds.max_depth_bounds          );
          break;
      case VK_CMD_SET_STENCIL_COMPARE_MASK:
          disp->CmdSetStencilCompareMask(commandBuffer
             , cmd->u.set_stencil_compare_mask.face_mask             , cmd->u.set_stencil_compare_mask.compare_mask          );
          break;
      case VK_CMD_SET_STENCIL_WRITE_MASK:
          disp->CmdSetStencilWriteMask(commandBuffer
             , cmd->u.set_stencil_write_mask.face_mask             , cmd->u.set_stencil_write_mask.write_mask          );
          break;
      case VK_CMD_SET_STENCIL_REFERENCE:
          disp->CmdSetStencilReference(commandBuffer
             , cmd->u.set_stencil_reference.face_mask             , cmd->u.set_stencil_reference.reference          );
          break;
      case VK_CMD_BIND_DESCRIPTOR_SETS:
          disp->CmdBindDescriptorSets(commandBuffer
             , cmd->u.bind_descriptor_sets.pipeline_bind_point             , cmd->u.bind_descriptor_sets.layout             , cmd->u.bind_descriptor_sets.first_set             , cmd->u.bind_descriptor_sets.descriptor_set_count             , cmd->u.bind_descriptor_sets.descriptor_sets             , cmd->u.bind_descriptor_sets.dynamic_offset_count             , cmd->u.bind_descriptor_sets.dynamic_offsets          );
          break;
      case VK_CMD_BIND_INDEX_BUFFER:
          disp->CmdBindIndexBuffer(commandBuffer
             , cmd->u.bind_index_buffer.buffer             , cmd->u.bind_index_buffer.offset             , cmd->u.bind_index_buffer.index_type          );
          break;
      case VK_CMD_BIND_VERTEX_BUFFERS:
          disp->CmdBindVertexBuffers(commandBuffer
             , cmd->u.bind_vertex_buffers.first_binding             , cmd->u.bind_vertex_buffers.binding_count             , cmd->u.bind_vertex_buffers.buffers             , cmd->u.bind_vertex_buffers.offsets          );
          break;
      case VK_CMD_DRAW:
          disp->CmdDraw(commandBuffer
             , cmd->u.draw.vertex_count             , cmd->u.draw.instance_count             , cmd->u.draw.first_vertex             , cmd->u.draw.first_instance          );
          break;
      case VK_CMD_DRAW_INDEXED:
          disp->CmdDrawIndexed(commandBuffer
             , cmd->u.draw_indexed.index_count             , cmd->u.draw_indexed.instance_count             , cmd->u.draw_indexed.first_index             , cmd->u.draw_indexed.vertex_offset             , cmd->u.draw_indexed.first_instance          );
          break;
      case VK_CMD_DRAW_MULTI_EXT:
          disp->CmdDrawMultiEXT(commandBuffer
             , cmd->u.draw_multi_ext.draw_count             , cmd->u.draw_multi_ext.vertex_info             , cmd->u.draw_multi_ext.instance_count             , cmd->u.draw_multi_ext.first_instance             , cmd->u.draw_multi_ext.stride          );
          break;
      case VK_CMD_DRAW_MULTI_INDEXED_EXT:
          disp->CmdDrawMultiIndexedEXT(commandBuffer
             , cmd->u.draw_multi_indexed_ext.draw_count             , cmd->u.draw_multi_indexed_ext.index_info             , cmd->u.draw_multi_indexed_ext.instance_count             , cmd->u.draw_multi_indexed_ext.first_instance             , cmd->u.draw_multi_indexed_ext.stride             , cmd->u.draw_multi_indexed_ext.vertex_offset          );
          break;
      case VK_CMD_DRAW_INDIRECT:
          disp->CmdDrawIndirect(commandBuffer
             , cmd->u.draw_indirect.buffer             , cmd->u.draw_indirect.offset             , cmd->u.draw_indirect.draw_count             , cmd->u.draw_indirect.stride          );
          break;
      case VK_CMD_DRAW_INDEXED_INDIRECT:
          disp->CmdDrawIndexedIndirect(commandBuffer
             , cmd->u.draw_indexed_indirect.buffer             , cmd->u.draw_indexed_indirect.offset             , cmd->u.draw_indexed_indirect.draw_count             , cmd->u.draw_indexed_indirect.stride          );
          break;
      case VK_CMD_DISPATCH:
          disp->CmdDispatch(commandBuffer
             , cmd->u.dispatch.group_count_x             , cmd->u.dispatch.group_count_y             , cmd->u.dispatch.group_count_z          );
          break;
      case VK_CMD_DISPATCH_INDIRECT:
          disp->CmdDispatchIndirect(commandBuffer
             , cmd->u.dispatch_indirect.buffer             , cmd->u.dispatch_indirect.offset          );
          break;
      case VK_CMD_SUBPASS_SHADING_HUAWEI:
          disp->CmdSubpassShadingHUAWEI(commandBuffer
          );
          break;
      case VK_CMD_DRAW_CLUSTER_HUAWEI:
          disp->CmdDrawClusterHUAWEI(commandBuffer
             , cmd->u.draw_cluster_huawei.group_count_x             , cmd->u.draw_cluster_huawei.group_count_y             , cmd->u.draw_cluster_huawei.group_count_z          );
          break;
      case VK_CMD_DRAW_CLUSTER_INDIRECT_HUAWEI:
          disp->CmdDrawClusterIndirectHUAWEI(commandBuffer
             , cmd->u.draw_cluster_indirect_huawei.buffer             , cmd->u.draw_cluster_indirect_huawei.offset          );
          break;
      case VK_CMD_UPDATE_PIPELINE_INDIRECT_BUFFER_NV:
          disp->CmdUpdatePipelineIndirectBufferNV(commandBuffer
             , cmd->u.update_pipeline_indirect_buffer_nv.pipeline_bind_point             , cmd->u.update_pipeline_indirect_buffer_nv.pipeline          );
          break;
      case VK_CMD_COPY_BUFFER:
          disp->CmdCopyBuffer(commandBuffer
             , cmd->u.copy_buffer.src_buffer             , cmd->u.copy_buffer.dst_buffer             , cmd->u.copy_buffer.region_count             , cmd->u.copy_buffer.regions          );
          break;
      case VK_CMD_COPY_IMAGE:
          disp->CmdCopyImage(commandBuffer
             , cmd->u.copy_image.src_image             , cmd->u.copy_image.src_image_layout             , cmd->u.copy_image.dst_image             , cmd->u.copy_image.dst_image_layout             , cmd->u.copy_image.region_count             , cmd->u.copy_image.regions          );
          break;
      case VK_CMD_BLIT_IMAGE:
          disp->CmdBlitImage(commandBuffer
             , cmd->u.blit_image.src_image             , cmd->u.blit_image.src_image_layout             , cmd->u.blit_image.dst_image             , cmd->u.blit_image.dst_image_layout             , cmd->u.blit_image.region_count             , cmd->u.blit_image.regions             , cmd->u.blit_image.filter          );
          break;
      case VK_CMD_COPY_BUFFER_TO_IMAGE:
          disp->CmdCopyBufferToImage(commandBuffer
             , cmd->u.copy_buffer_to_image.src_buffer             , cmd->u.copy_buffer_to_image.dst_image             , cmd->u.copy_buffer_to_image.dst_image_layout             , cmd->u.copy_buffer_to_image.region_count             , cmd->u.copy_buffer_to_image.regions          );
          break;
      case VK_CMD_COPY_IMAGE_TO_BUFFER:
          disp->CmdCopyImageToBuffer(commandBuffer
             , cmd->u.copy_image_to_buffer.src_image             , cmd->u.copy_image_to_buffer.src_image_layout             , cmd->u.copy_image_to_buffer.dst_buffer             , cmd->u.copy_image_to_buffer.region_count             , cmd->u.copy_image_to_buffer.regions          );
          break;
      case VK_CMD_COPY_MEMORY_INDIRECT_NV:
          disp->CmdCopyMemoryIndirectNV(commandBuffer
             , cmd->u.copy_memory_indirect_nv.copy_buffer_address             , cmd->u.copy_memory_indirect_nv.copy_count             , cmd->u.copy_memory_indirect_nv.stride          );
          break;
      case VK_CMD_COPY_MEMORY_INDIRECT_KHR:
          disp->CmdCopyMemoryIndirectKHR(commandBuffer
             , cmd->u.copy_memory_indirect_khr.copy_memory_indirect_info          );
          break;
      case VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_NV:
          disp->CmdCopyMemoryToImageIndirectNV(commandBuffer
             , cmd->u.copy_memory_to_image_indirect_nv.copy_buffer_address             , cmd->u.copy_memory_to_image_indirect_nv.copy_count             , cmd->u.copy_memory_to_image_indirect_nv.stride             , cmd->u.copy_memory_to_image_indirect_nv.dst_image             , cmd->u.copy_memory_to_image_indirect_nv.dst_image_layout             , cmd->u.copy_memory_to_image_indirect_nv.image_subresources          );
          break;
      case VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_KHR:
          disp->CmdCopyMemoryToImageIndirectKHR(commandBuffer
             , cmd->u.copy_memory_to_image_indirect_khr.copy_memory_to_image_indirect_info          );
          break;
      case VK_CMD_UPDATE_BUFFER:
          disp->CmdUpdateBuffer(commandBuffer
             , cmd->u.update_buffer.dst_buffer             , cmd->u.update_buffer.dst_offset             , cmd->u.update_buffer.data_size             , cmd->u.update_buffer.data          );
          break;
      case VK_CMD_FILL_BUFFER:
          disp->CmdFillBuffer(commandBuffer
             , cmd->u.fill_buffer.dst_buffer             , cmd->u.fill_buffer.dst_offset             , cmd->u.fill_buffer.size             , cmd->u.fill_buffer.data          );
          break;
      case VK_CMD_CLEAR_COLOR_IMAGE:
          disp->CmdClearColorImage(commandBuffer
             , cmd->u.clear_color_image.image             , cmd->u.clear_color_image.image_layout             , cmd->u.clear_color_image.color             , cmd->u.clear_color_image.range_count             , cmd->u.clear_color_image.ranges          );
          break;
      case VK_CMD_CLEAR_DEPTH_STENCIL_IMAGE:
          disp->CmdClearDepthStencilImage(commandBuffer
             , cmd->u.clear_depth_stencil_image.image             , cmd->u.clear_depth_stencil_image.image_layout             , cmd->u.clear_depth_stencil_image.depth_stencil             , cmd->u.clear_depth_stencil_image.range_count             , cmd->u.clear_depth_stencil_image.ranges          );
          break;
      case VK_CMD_CLEAR_ATTACHMENTS:
          disp->CmdClearAttachments(commandBuffer
             , cmd->u.clear_attachments.attachment_count             , cmd->u.clear_attachments.attachments             , cmd->u.clear_attachments.rect_count             , cmd->u.clear_attachments.rects          );
          break;
      case VK_CMD_RESOLVE_IMAGE:
          disp->CmdResolveImage(commandBuffer
             , cmd->u.resolve_image.src_image             , cmd->u.resolve_image.src_image_layout             , cmd->u.resolve_image.dst_image             , cmd->u.resolve_image.dst_image_layout             , cmd->u.resolve_image.region_count             , cmd->u.resolve_image.regions          );
          break;
      case VK_CMD_SET_EVENT:
          disp->CmdSetEvent(commandBuffer
             , cmd->u.set_event.event             , cmd->u.set_event.stage_mask          );
          break;
      case VK_CMD_RESET_EVENT:
          disp->CmdResetEvent(commandBuffer
             , cmd->u.reset_event.event             , cmd->u.reset_event.stage_mask          );
          break;
      case VK_CMD_WAIT_EVENTS:
          disp->CmdWaitEvents(commandBuffer
             , cmd->u.wait_events.event_count             , cmd->u.wait_events.events             , cmd->u.wait_events.src_stage_mask             , cmd->u.wait_events.dst_stage_mask             , cmd->u.wait_events.memory_barrier_count             , cmd->u.wait_events.memory_barriers             , cmd->u.wait_events.buffer_memory_barrier_count             , cmd->u.wait_events.buffer_memory_barriers             , cmd->u.wait_events.image_memory_barrier_count             , cmd->u.wait_events.image_memory_barriers          );
          break;
      case VK_CMD_PIPELINE_BARRIER:
          disp->CmdPipelineBarrier(commandBuffer
             , cmd->u.pipeline_barrier.src_stage_mask             , cmd->u.pipeline_barrier.dst_stage_mask             , cmd->u.pipeline_barrier.dependency_flags             , cmd->u.pipeline_barrier.memory_barrier_count             , cmd->u.pipeline_barrier.memory_barriers             , cmd->u.pipeline_barrier.buffer_memory_barrier_count             , cmd->u.pipeline_barrier.buffer_memory_barriers             , cmd->u.pipeline_barrier.image_memory_barrier_count             , cmd->u.pipeline_barrier.image_memory_barriers          );
          break;
      case VK_CMD_BEGIN_QUERY:
          disp->CmdBeginQuery(commandBuffer
             , cmd->u.begin_query.query_pool             , cmd->u.begin_query.query             , cmd->u.begin_query.flags          );
          break;
      case VK_CMD_END_QUERY:
          disp->CmdEndQuery(commandBuffer
             , cmd->u.end_query.query_pool             , cmd->u.end_query.query          );
          break;
      case VK_CMD_BEGIN_CONDITIONAL_RENDERING_EXT:
          disp->CmdBeginConditionalRenderingEXT(commandBuffer
             , cmd->u.begin_conditional_rendering_ext.conditional_rendering_begin          );
          break;
      case VK_CMD_END_CONDITIONAL_RENDERING_EXT:
          disp->CmdEndConditionalRenderingEXT(commandBuffer
          );
          break;
      case VK_CMD_BEGIN_CUSTOM_RESOLVE_EXT:
          disp->CmdBeginCustomResolveEXT(commandBuffer
             , cmd->u.begin_custom_resolve_ext.begin_custom_resolve_info          );
          break;
      case VK_CMD_RESET_QUERY_POOL:
          disp->CmdResetQueryPool(commandBuffer
             , cmd->u.reset_query_pool.query_pool             , cmd->u.reset_query_pool.first_query             , cmd->u.reset_query_pool.query_count          );
          break;
      case VK_CMD_WRITE_TIMESTAMP:
          disp->CmdWriteTimestamp(commandBuffer
             , cmd->u.write_timestamp.pipeline_stage             , cmd->u.write_timestamp.query_pool             , cmd->u.write_timestamp.query          );
          break;
      case VK_CMD_COPY_QUERY_POOL_RESULTS:
          disp->CmdCopyQueryPoolResults(commandBuffer
             , cmd->u.copy_query_pool_results.query_pool             , cmd->u.copy_query_pool_results.first_query             , cmd->u.copy_query_pool_results.query_count             , cmd->u.copy_query_pool_results.dst_buffer             , cmd->u.copy_query_pool_results.dst_offset             , cmd->u.copy_query_pool_results.stride             , cmd->u.copy_query_pool_results.flags          );
          break;
      case VK_CMD_PUSH_CONSTANTS:
          disp->CmdPushConstants(commandBuffer
             , cmd->u.push_constants.layout             , cmd->u.push_constants.stage_flags             , cmd->u.push_constants.offset             , cmd->u.push_constants.size             , cmd->u.push_constants.values          );
          break;
      case VK_CMD_BEGIN_RENDER_PASS:
          disp->CmdBeginRenderPass(commandBuffer
             , cmd->u.begin_render_pass.render_pass_begin             , cmd->u.begin_render_pass.contents          );
          break;
      case VK_CMD_NEXT_SUBPASS:
          disp->CmdNextSubpass(commandBuffer
             , cmd->u.next_subpass.contents          );
          break;
      case VK_CMD_END_RENDER_PASS:
          disp->CmdEndRenderPass(commandBuffer
          );
          break;
      case VK_CMD_EXECUTE_COMMANDS:
          disp->CmdExecuteCommands(commandBuffer
             , cmd->u.execute_commands.command_buffer_count             , cmd->u.execute_commands.command_buffers          );
          break;
      case VK_CMD_DEBUG_MARKER_BEGIN_EXT:
          disp->CmdDebugMarkerBeginEXT(commandBuffer
             , cmd->u.debug_marker_begin_ext.marker_info          );
          break;
      case VK_CMD_DEBUG_MARKER_END_EXT:
          disp->CmdDebugMarkerEndEXT(commandBuffer
          );
          break;
      case VK_CMD_DEBUG_MARKER_INSERT_EXT:
          disp->CmdDebugMarkerInsertEXT(commandBuffer
             , cmd->u.debug_marker_insert_ext.marker_info          );
          break;
      case VK_CMD_EXECUTE_GENERATED_COMMANDS_NV:
          disp->CmdExecuteGeneratedCommandsNV(commandBuffer
             , cmd->u.execute_generated_commands_nv.is_preprocessed             , cmd->u.execute_generated_commands_nv.generated_commands_info          );
          break;
      case VK_CMD_PREPROCESS_GENERATED_COMMANDS_NV:
          disp->CmdPreprocessGeneratedCommandsNV(commandBuffer
             , cmd->u.preprocess_generated_commands_nv.generated_commands_info          );
          break;
      case VK_CMD_BIND_PIPELINE_SHADER_GROUP_NV:
          disp->CmdBindPipelineShaderGroupNV(commandBuffer
             , cmd->u.bind_pipeline_shader_group_nv.pipeline_bind_point             , cmd->u.bind_pipeline_shader_group_nv.pipeline             , cmd->u.bind_pipeline_shader_group_nv.group_index          );
          break;
      case VK_CMD_EXECUTE_GENERATED_COMMANDS_EXT:
          disp->CmdExecuteGeneratedCommandsEXT(commandBuffer
             , cmd->u.execute_generated_commands_ext.is_preprocessed             , cmd->u.execute_generated_commands_ext.generated_commands_info          );
          break;
      case VK_CMD_PREPROCESS_GENERATED_COMMANDS_EXT:
          disp->CmdPreprocessGeneratedCommandsEXT(commandBuffer
             , cmd->u.preprocess_generated_commands_ext.generated_commands_info             , cmd->u.preprocess_generated_commands_ext.state_command_buffer          );
          break;
      case VK_CMD_PUSH_DESCRIPTOR_SET:
          disp->CmdPushDescriptorSet(commandBuffer
             , cmd->u.push_descriptor_set.pipeline_bind_point             , cmd->u.push_descriptor_set.layout             , cmd->u.push_descriptor_set.set             , cmd->u.push_descriptor_set.descriptor_write_count             , cmd->u.push_descriptor_set.descriptor_writes          );
          break;
      case VK_CMD_SET_DEVICE_MASK:
          disp->CmdSetDeviceMask(commandBuffer
             , cmd->u.set_device_mask.device_mask          );
          break;
      case VK_CMD_DISPATCH_BASE:
          disp->CmdDispatchBase(commandBuffer
             , cmd->u.dispatch_base.base_group_x             , cmd->u.dispatch_base.base_group_y             , cmd->u.dispatch_base.base_group_z             , cmd->u.dispatch_base.group_count_x             , cmd->u.dispatch_base.group_count_y             , cmd->u.dispatch_base.group_count_z          );
          break;
      case VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE:
          disp->CmdPushDescriptorSetWithTemplate(commandBuffer
             , cmd->u.push_descriptor_set_with_template.descriptor_update_template             , cmd->u.push_descriptor_set_with_template.layout             , cmd->u.push_descriptor_set_with_template.set             , cmd->u.push_descriptor_set_with_template.data          );
          break;
      case VK_CMD_SET_VIEWPORT_WSCALING_NV:
          disp->CmdSetViewportWScalingNV(commandBuffer
             , cmd->u.set_viewport_wscaling_nv.first_viewport             , cmd->u.set_viewport_wscaling_nv.viewport_count             , cmd->u.set_viewport_wscaling_nv.viewport_wscalings          );
          break;
      case VK_CMD_SET_DISCARD_RECTANGLE_EXT:
          disp->CmdSetDiscardRectangleEXT(commandBuffer
             , cmd->u.set_discard_rectangle_ext.first_discard_rectangle             , cmd->u.set_discard_rectangle_ext.discard_rectangle_count             , cmd->u.set_discard_rectangle_ext.discard_rectangles          );
          break;
      case VK_CMD_SET_DISCARD_RECTANGLE_ENABLE_EXT:
          disp->CmdSetDiscardRectangleEnableEXT(commandBuffer
             , cmd->u.set_discard_rectangle_enable_ext.discard_rectangle_enable          );
          break;
      case VK_CMD_SET_DISCARD_RECTANGLE_MODE_EXT:
          disp->CmdSetDiscardRectangleModeEXT(commandBuffer
             , cmd->u.set_discard_rectangle_mode_ext.discard_rectangle_mode          );
          break;
      case VK_CMD_SET_SAMPLE_LOCATIONS_EXT:
          disp->CmdSetSampleLocationsEXT(commandBuffer
             , cmd->u.set_sample_locations_ext.sample_locations_info          );
          break;
      case VK_CMD_BEGIN_DEBUG_UTILS_LABEL_EXT:
          disp->CmdBeginDebugUtilsLabelEXT(commandBuffer
             , cmd->u.begin_debug_utils_label_ext.label_info          );
          break;
      case VK_CMD_END_DEBUG_UTILS_LABEL_EXT:
          disp->CmdEndDebugUtilsLabelEXT(commandBuffer
          );
          break;
      case VK_CMD_INSERT_DEBUG_UTILS_LABEL_EXT:
          disp->CmdInsertDebugUtilsLabelEXT(commandBuffer
             , cmd->u.insert_debug_utils_label_ext.label_info          );
          break;
      case VK_CMD_WRITE_BUFFER_MARKER_AMD:
          disp->CmdWriteBufferMarkerAMD(commandBuffer
             , cmd->u.write_buffer_marker_amd.pipeline_stage             , cmd->u.write_buffer_marker_amd.dst_buffer             , cmd->u.write_buffer_marker_amd.dst_offset             , cmd->u.write_buffer_marker_amd.marker          );
          break;
      case VK_CMD_BEGIN_RENDER_PASS2:
          disp->CmdBeginRenderPass2(commandBuffer
             , cmd->u.begin_render_pass2.render_pass_begin             , cmd->u.begin_render_pass2.subpass_begin_info          );
          break;
      case VK_CMD_NEXT_SUBPASS2:
          disp->CmdNextSubpass2(commandBuffer
             , cmd->u.next_subpass2.subpass_begin_info             , cmd->u.next_subpass2.subpass_end_info          );
          break;
      case VK_CMD_END_RENDER_PASS2:
          disp->CmdEndRenderPass2(commandBuffer
             , cmd->u.end_render_pass2.subpass_end_info          );
          break;
      case VK_CMD_DRAW_INDIRECT_COUNT:
          disp->CmdDrawIndirectCount(commandBuffer
             , cmd->u.draw_indirect_count.buffer             , cmd->u.draw_indirect_count.offset             , cmd->u.draw_indirect_count.count_buffer             , cmd->u.draw_indirect_count.count_buffer_offset             , cmd->u.draw_indirect_count.max_draw_count             , cmd->u.draw_indirect_count.stride          );
          break;
      case VK_CMD_DRAW_INDEXED_INDIRECT_COUNT:
          disp->CmdDrawIndexedIndirectCount(commandBuffer
             , cmd->u.draw_indexed_indirect_count.buffer             , cmd->u.draw_indexed_indirect_count.offset             , cmd->u.draw_indexed_indirect_count.count_buffer             , cmd->u.draw_indexed_indirect_count.count_buffer_offset             , cmd->u.draw_indexed_indirect_count.max_draw_count             , cmd->u.draw_indexed_indirect_count.stride          );
          break;
      case VK_CMD_SET_CHECKPOINT_NV:
          disp->CmdSetCheckpointNV(commandBuffer
             , cmd->u.set_checkpoint_nv.checkpoint_marker          );
          break;
      case VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS_EXT:
          disp->CmdBindTransformFeedbackBuffersEXT(commandBuffer
             , cmd->u.bind_transform_feedback_buffers_ext.first_binding             , cmd->u.bind_transform_feedback_buffers_ext.binding_count             , cmd->u.bind_transform_feedback_buffers_ext.buffers             , cmd->u.bind_transform_feedback_buffers_ext.offsets             , cmd->u.bind_transform_feedback_buffers_ext.sizes          );
          break;
      case VK_CMD_BEGIN_TRANSFORM_FEEDBACK_EXT:
          disp->CmdBeginTransformFeedbackEXT(commandBuffer
             , cmd->u.begin_transform_feedback_ext.first_counter_buffer             , cmd->u.begin_transform_feedback_ext.counter_buffer_count             , cmd->u.begin_transform_feedback_ext.counter_buffers             , cmd->u.begin_transform_feedback_ext.counter_buffer_offsets          );
          break;
      case VK_CMD_END_TRANSFORM_FEEDBACK_EXT:
          disp->CmdEndTransformFeedbackEXT(commandBuffer
             , cmd->u.end_transform_feedback_ext.first_counter_buffer             , cmd->u.end_transform_feedback_ext.counter_buffer_count             , cmd->u.end_transform_feedback_ext.counter_buffers             , cmd->u.end_transform_feedback_ext.counter_buffer_offsets          );
          break;
      case VK_CMD_BEGIN_QUERY_INDEXED_EXT:
          disp->CmdBeginQueryIndexedEXT(commandBuffer
             , cmd->u.begin_query_indexed_ext.query_pool             , cmd->u.begin_query_indexed_ext.query             , cmd->u.begin_query_indexed_ext.flags             , cmd->u.begin_query_indexed_ext.index          );
          break;
      case VK_CMD_END_QUERY_INDEXED_EXT:
          disp->CmdEndQueryIndexedEXT(commandBuffer
             , cmd->u.end_query_indexed_ext.query_pool             , cmd->u.end_query_indexed_ext.query             , cmd->u.end_query_indexed_ext.index          );
          break;
      case VK_CMD_DRAW_INDIRECT_BYTE_COUNT_EXT:
          disp->CmdDrawIndirectByteCountEXT(commandBuffer
             , cmd->u.draw_indirect_byte_count_ext.instance_count             , cmd->u.draw_indirect_byte_count_ext.first_instance             , cmd->u.draw_indirect_byte_count_ext.counter_buffer             , cmd->u.draw_indirect_byte_count_ext.counter_buffer_offset             , cmd->u.draw_indirect_byte_count_ext.counter_offset             , cmd->u.draw_indirect_byte_count_ext.vertex_stride          );
          break;
      case VK_CMD_SET_EXCLUSIVE_SCISSOR_NV:
          disp->CmdSetExclusiveScissorNV(commandBuffer
             , cmd->u.set_exclusive_scissor_nv.first_exclusive_scissor             , cmd->u.set_exclusive_scissor_nv.exclusive_scissor_count             , cmd->u.set_exclusive_scissor_nv.exclusive_scissors          );
          break;
      case VK_CMD_SET_EXCLUSIVE_SCISSOR_ENABLE_NV:
          disp->CmdSetExclusiveScissorEnableNV(commandBuffer
             , cmd->u.set_exclusive_scissor_enable_nv.first_exclusive_scissor             , cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_count             , cmd->u.set_exclusive_scissor_enable_nv.exclusive_scissor_enables          );
          break;
      case VK_CMD_BIND_SHADING_RATE_IMAGE_NV:
          disp->CmdBindShadingRateImageNV(commandBuffer
             , cmd->u.bind_shading_rate_image_nv.image_view             , cmd->u.bind_shading_rate_image_nv.image_layout          );
          break;
      case VK_CMD_SET_VIEWPORT_SHADING_RATE_PALETTE_NV:
          disp->CmdSetViewportShadingRatePaletteNV(commandBuffer
             , cmd->u.set_viewport_shading_rate_palette_nv.first_viewport             , cmd->u.set_viewport_shading_rate_palette_nv.viewport_count             , cmd->u.set_viewport_shading_rate_palette_nv.shading_rate_palettes          );
          break;
      case VK_CMD_SET_COARSE_SAMPLE_ORDER_NV:
          disp->CmdSetCoarseSampleOrderNV(commandBuffer
             , cmd->u.set_coarse_sample_order_nv.sample_order_type             , cmd->u.set_coarse_sample_order_nv.custom_sample_order_count             , cmd->u.set_coarse_sample_order_nv.custom_sample_orders          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_NV:
          disp->CmdDrawMeshTasksNV(commandBuffer
             , cmd->u.draw_mesh_tasks_nv.task_count             , cmd->u.draw_mesh_tasks_nv.first_task          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT_NV:
          disp->CmdDrawMeshTasksIndirectNV(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect_nv.buffer             , cmd->u.draw_mesh_tasks_indirect_nv.offset             , cmd->u.draw_mesh_tasks_indirect_nv.draw_count             , cmd->u.draw_mesh_tasks_indirect_nv.stride          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_NV:
          disp->CmdDrawMeshTasksIndirectCountNV(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect_count_nv.buffer             , cmd->u.draw_mesh_tasks_indirect_count_nv.offset             , cmd->u.draw_mesh_tasks_indirect_count_nv.count_buffer             , cmd->u.draw_mesh_tasks_indirect_count_nv.count_buffer_offset             , cmd->u.draw_mesh_tasks_indirect_count_nv.max_draw_count             , cmd->u.draw_mesh_tasks_indirect_count_nv.stride          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_EXT:
          disp->CmdDrawMeshTasksEXT(commandBuffer
             , cmd->u.draw_mesh_tasks_ext.group_count_x             , cmd->u.draw_mesh_tasks_ext.group_count_y             , cmd->u.draw_mesh_tasks_ext.group_count_z          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT_EXT:
          disp->CmdDrawMeshTasksIndirectEXT(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect_ext.buffer             , cmd->u.draw_mesh_tasks_indirect_ext.offset             , cmd->u.draw_mesh_tasks_indirect_ext.draw_count             , cmd->u.draw_mesh_tasks_indirect_ext.stride          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_EXT:
          disp->CmdDrawMeshTasksIndirectCountEXT(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect_count_ext.buffer             , cmd->u.draw_mesh_tasks_indirect_count_ext.offset             , cmd->u.draw_mesh_tasks_indirect_count_ext.count_buffer             , cmd->u.draw_mesh_tasks_indirect_count_ext.count_buffer_offset             , cmd->u.draw_mesh_tasks_indirect_count_ext.max_draw_count             , cmd->u.draw_mesh_tasks_indirect_count_ext.stride          );
          break;
      case VK_CMD_BIND_INVOCATION_MASK_HUAWEI:
          disp->CmdBindInvocationMaskHUAWEI(commandBuffer
             , cmd->u.bind_invocation_mask_huawei.image_view             , cmd->u.bind_invocation_mask_huawei.image_layout          );
          break;
      case VK_CMD_COPY_ACCELERATION_STRUCTURE_NV:
          disp->CmdCopyAccelerationStructureNV(commandBuffer
             , cmd->u.copy_acceleration_structure_nv.dst             , cmd->u.copy_acceleration_structure_nv.src             , cmd->u.copy_acceleration_structure_nv.mode          );
          break;
      case VK_CMD_COPY_ACCELERATION_STRUCTURE_KHR:
          disp->CmdCopyAccelerationStructureKHR(commandBuffer
             , cmd->u.copy_acceleration_structure_khr.info          );
          break;
      case VK_CMD_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_KHR:
          disp->CmdCopyAccelerationStructureToMemoryKHR(commandBuffer
             , cmd->u.copy_acceleration_structure_to_memory_khr.info          );
          break;
      case VK_CMD_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_KHR:
          disp->CmdCopyMemoryToAccelerationStructureKHR(commandBuffer
             , cmd->u.copy_memory_to_acceleration_structure_khr.info          );
          break;
      case VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_KHR:
          disp->CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer
             , cmd->u.write_acceleration_structures_properties_khr.acceleration_structure_count             , cmd->u.write_acceleration_structures_properties_khr.acceleration_structures             , cmd->u.write_acceleration_structures_properties_khr.query_type             , cmd->u.write_acceleration_structures_properties_khr.query_pool             , cmd->u.write_acceleration_structures_properties_khr.first_query          );
          break;
      case VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_NV:
          disp->CmdWriteAccelerationStructuresPropertiesNV(commandBuffer
             , cmd->u.write_acceleration_structures_properties_nv.acceleration_structure_count             , cmd->u.write_acceleration_structures_properties_nv.acceleration_structures             , cmd->u.write_acceleration_structures_properties_nv.query_type             , cmd->u.write_acceleration_structures_properties_nv.query_pool             , cmd->u.write_acceleration_structures_properties_nv.first_query          );
          break;
      case VK_CMD_BUILD_ACCELERATION_STRUCTURE_NV:
          disp->CmdBuildAccelerationStructureNV(commandBuffer
             , cmd->u.build_acceleration_structure_nv.info             , cmd->u.build_acceleration_structure_nv.instance_data             , cmd->u.build_acceleration_structure_nv.instance_offset             , cmd->u.build_acceleration_structure_nv.update             , cmd->u.build_acceleration_structure_nv.dst             , cmd->u.build_acceleration_structure_nv.src             , cmd->u.build_acceleration_structure_nv.scratch             , cmd->u.build_acceleration_structure_nv.scratch_offset          );
          break;
      case VK_CMD_TRACE_RAYS_KHR:
          disp->CmdTraceRaysKHR(commandBuffer
             , cmd->u.trace_rays_khr.raygen_shader_binding_table             , cmd->u.trace_rays_khr.miss_shader_binding_table             , cmd->u.trace_rays_khr.hit_shader_binding_table             , cmd->u.trace_rays_khr.callable_shader_binding_table             , cmd->u.trace_rays_khr.width             , cmd->u.trace_rays_khr.height             , cmd->u.trace_rays_khr.depth          );
          break;
      case VK_CMD_TRACE_RAYS_NV:
          disp->CmdTraceRaysNV(commandBuffer
             , cmd->u.trace_rays_nv.raygen_shader_binding_table_buffer             , cmd->u.trace_rays_nv.raygen_shader_binding_offset             , cmd->u.trace_rays_nv.miss_shader_binding_table_buffer             , cmd->u.trace_rays_nv.miss_shader_binding_offset             , cmd->u.trace_rays_nv.miss_shader_binding_stride             , cmd->u.trace_rays_nv.hit_shader_binding_table_buffer             , cmd->u.trace_rays_nv.hit_shader_binding_offset             , cmd->u.trace_rays_nv.hit_shader_binding_stride             , cmd->u.trace_rays_nv.callable_shader_binding_table_buffer             , cmd->u.trace_rays_nv.callable_shader_binding_offset             , cmd->u.trace_rays_nv.callable_shader_binding_stride             , cmd->u.trace_rays_nv.width             , cmd->u.trace_rays_nv.height             , cmd->u.trace_rays_nv.depth          );
          break;
      case VK_CMD_TRACE_RAYS_INDIRECT_KHR:
          disp->CmdTraceRaysIndirectKHR(commandBuffer
             , cmd->u.trace_rays_indirect_khr.raygen_shader_binding_table             , cmd->u.trace_rays_indirect_khr.miss_shader_binding_table             , cmd->u.trace_rays_indirect_khr.hit_shader_binding_table             , cmd->u.trace_rays_indirect_khr.callable_shader_binding_table             , cmd->u.trace_rays_indirect_khr.indirect_device_address          );
          break;
      case VK_CMD_TRACE_RAYS_INDIRECT2_KHR:
          disp->CmdTraceRaysIndirect2KHR(commandBuffer
             , cmd->u.trace_rays_indirect2_khr.indirect_device_address          );
          break;
      case VK_CMD_BUILD_CLUSTER_ACCELERATION_STRUCTURE_INDIRECT_NV:
          disp->CmdBuildClusterAccelerationStructureIndirectNV(commandBuffer
             , cmd->u.build_cluster_acceleration_structure_indirect_nv.command_infos          );
          break;
      case VK_CMD_SET_RAY_TRACING_PIPELINE_STACK_SIZE_KHR:
          disp->CmdSetRayTracingPipelineStackSizeKHR(commandBuffer
             , cmd->u.set_ray_tracing_pipeline_stack_size_khr.pipeline_stack_size          );
          break;
      case VK_CMD_SET_PERFORMANCE_MARKER_INTEL:
          disp->CmdSetPerformanceMarkerINTEL(commandBuffer
             , cmd->u.set_performance_marker_intel.marker_info          );
          break;
      case VK_CMD_SET_PERFORMANCE_STREAM_MARKER_INTEL:
          disp->CmdSetPerformanceStreamMarkerINTEL(commandBuffer
             , cmd->u.set_performance_stream_marker_intel.marker_info          );
          break;
      case VK_CMD_SET_PERFORMANCE_OVERRIDE_INTEL:
          disp->CmdSetPerformanceOverrideINTEL(commandBuffer
             , cmd->u.set_performance_override_intel.override_info          );
          break;
      case VK_CMD_SET_LINE_STIPPLE:
          disp->CmdSetLineStipple(commandBuffer
             , cmd->u.set_line_stipple.line_stipple_factor             , cmd->u.set_line_stipple.line_stipple_pattern          );
          break;
      case VK_CMD_BUILD_ACCELERATION_STRUCTURES_KHR:
          disp->CmdBuildAccelerationStructuresKHR(commandBuffer
             , cmd->u.build_acceleration_structures_khr.info_count             , cmd->u.build_acceleration_structures_khr.infos             , cmd->u.build_acceleration_structures_khr.pp_build_range_infos          );
          break;
      case VK_CMD_BUILD_ACCELERATION_STRUCTURES_INDIRECT_KHR:
          disp->CmdBuildAccelerationStructuresIndirectKHR(commandBuffer
             , cmd->u.build_acceleration_structures_indirect_khr.info_count             , cmd->u.build_acceleration_structures_indirect_khr.infos             , cmd->u.build_acceleration_structures_indirect_khr.indirect_device_addresses             , cmd->u.build_acceleration_structures_indirect_khr.indirect_strides             , cmd->u.build_acceleration_structures_indirect_khr.pp_max_primitive_counts          );
          break;
      case VK_CMD_SET_CULL_MODE:
          disp->CmdSetCullMode(commandBuffer
             , cmd->u.set_cull_mode.cull_mode          );
          break;
      case VK_CMD_SET_FRONT_FACE:
          disp->CmdSetFrontFace(commandBuffer
             , cmd->u.set_front_face.front_face          );
          break;
      case VK_CMD_SET_PRIMITIVE_TOPOLOGY:
          disp->CmdSetPrimitiveTopology(commandBuffer
             , cmd->u.set_primitive_topology.primitive_topology          );
          break;
      case VK_CMD_SET_VIEWPORT_WITH_COUNT:
          disp->CmdSetViewportWithCount(commandBuffer
             , cmd->u.set_viewport_with_count.viewport_count             , cmd->u.set_viewport_with_count.viewports          );
          break;
      case VK_CMD_SET_SCISSOR_WITH_COUNT:
          disp->CmdSetScissorWithCount(commandBuffer
             , cmd->u.set_scissor_with_count.scissor_count             , cmd->u.set_scissor_with_count.scissors          );
          break;
      case VK_CMD_BIND_INDEX_BUFFER2:
          disp->CmdBindIndexBuffer2(commandBuffer
             , cmd->u.bind_index_buffer2.buffer             , cmd->u.bind_index_buffer2.offset             , cmd->u.bind_index_buffer2.size             , cmd->u.bind_index_buffer2.index_type          );
          break;
      case VK_CMD_BIND_VERTEX_BUFFERS2:
          disp->CmdBindVertexBuffers2(commandBuffer
             , cmd->u.bind_vertex_buffers2.first_binding             , cmd->u.bind_vertex_buffers2.binding_count             , cmd->u.bind_vertex_buffers2.buffers             , cmd->u.bind_vertex_buffers2.offsets             , cmd->u.bind_vertex_buffers2.sizes             , cmd->u.bind_vertex_buffers2.strides          );
          break;
      case VK_CMD_SET_DEPTH_TEST_ENABLE:
          disp->CmdSetDepthTestEnable(commandBuffer
             , cmd->u.set_depth_test_enable.depth_test_enable          );
          break;
      case VK_CMD_SET_DEPTH_WRITE_ENABLE:
          disp->CmdSetDepthWriteEnable(commandBuffer
             , cmd->u.set_depth_write_enable.depth_write_enable          );
          break;
      case VK_CMD_SET_DEPTH_COMPARE_OP:
          disp->CmdSetDepthCompareOp(commandBuffer
             , cmd->u.set_depth_compare_op.depth_compare_op          );
          break;
      case VK_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE:
          disp->CmdSetDepthBoundsTestEnable(commandBuffer
             , cmd->u.set_depth_bounds_test_enable.depth_bounds_test_enable          );
          break;
      case VK_CMD_SET_STENCIL_TEST_ENABLE:
          disp->CmdSetStencilTestEnable(commandBuffer
             , cmd->u.set_stencil_test_enable.stencil_test_enable          );
          break;
      case VK_CMD_SET_STENCIL_OP:
          disp->CmdSetStencilOp(commandBuffer
             , cmd->u.set_stencil_op.face_mask             , cmd->u.set_stencil_op.fail_op             , cmd->u.set_stencil_op.pass_op             , cmd->u.set_stencil_op.depth_fail_op             , cmd->u.set_stencil_op.compare_op          );
          break;
      case VK_CMD_SET_PATCH_CONTROL_POINTS_EXT:
          disp->CmdSetPatchControlPointsEXT(commandBuffer
             , cmd->u.set_patch_control_points_ext.patch_control_points          );
          break;
      case VK_CMD_SET_RASTERIZER_DISCARD_ENABLE:
          disp->CmdSetRasterizerDiscardEnable(commandBuffer
             , cmd->u.set_rasterizer_discard_enable.rasterizer_discard_enable          );
          break;
      case VK_CMD_SET_DEPTH_BIAS_ENABLE:
          disp->CmdSetDepthBiasEnable(commandBuffer
             , cmd->u.set_depth_bias_enable.depth_bias_enable          );
          break;
      case VK_CMD_SET_LOGIC_OP_EXT:
          disp->CmdSetLogicOpEXT(commandBuffer
             , cmd->u.set_logic_op_ext.logic_op          );
          break;
      case VK_CMD_SET_PRIMITIVE_RESTART_ENABLE:
          disp->CmdSetPrimitiveRestartEnable(commandBuffer
             , cmd->u.set_primitive_restart_enable.primitive_restart_enable          );
          break;
      case VK_CMD_SET_TESSELLATION_DOMAIN_ORIGIN_EXT:
          disp->CmdSetTessellationDomainOriginEXT(commandBuffer
             , cmd->u.set_tessellation_domain_origin_ext.domain_origin          );
          break;
      case VK_CMD_SET_DEPTH_CLAMP_ENABLE_EXT:
          disp->CmdSetDepthClampEnableEXT(commandBuffer
             , cmd->u.set_depth_clamp_enable_ext.depth_clamp_enable          );
          break;
      case VK_CMD_SET_POLYGON_MODE_EXT:
          disp->CmdSetPolygonModeEXT(commandBuffer
             , cmd->u.set_polygon_mode_ext.polygon_mode          );
          break;
      case VK_CMD_SET_RASTERIZATION_SAMPLES_EXT:
          disp->CmdSetRasterizationSamplesEXT(commandBuffer
             , cmd->u.set_rasterization_samples_ext.rasterization_samples          );
          break;
      case VK_CMD_SET_SAMPLE_MASK_EXT:
          disp->CmdSetSampleMaskEXT(commandBuffer
             , cmd->u.set_sample_mask_ext.samples             , cmd->u.set_sample_mask_ext.sample_mask          );
          break;
      case VK_CMD_SET_ALPHA_TO_COVERAGE_ENABLE_EXT:
          disp->CmdSetAlphaToCoverageEnableEXT(commandBuffer
             , cmd->u.set_alpha_to_coverage_enable_ext.alpha_to_coverage_enable          );
          break;
      case VK_CMD_SET_ALPHA_TO_ONE_ENABLE_EXT:
          disp->CmdSetAlphaToOneEnableEXT(commandBuffer
             , cmd->u.set_alpha_to_one_enable_ext.alpha_to_one_enable          );
          break;
      case VK_CMD_SET_LOGIC_OP_ENABLE_EXT:
          disp->CmdSetLogicOpEnableEXT(commandBuffer
             , cmd->u.set_logic_op_enable_ext.logic_op_enable          );
          break;
      case VK_CMD_SET_COLOR_BLEND_ENABLE_EXT:
          disp->CmdSetColorBlendEnableEXT(commandBuffer
             , cmd->u.set_color_blend_enable_ext.first_attachment             , cmd->u.set_color_blend_enable_ext.attachment_count             , cmd->u.set_color_blend_enable_ext.color_blend_enables          );
          break;
      case VK_CMD_SET_COLOR_BLEND_EQUATION_EXT:
          disp->CmdSetColorBlendEquationEXT(commandBuffer
             , cmd->u.set_color_blend_equation_ext.first_attachment             , cmd->u.set_color_blend_equation_ext.attachment_count             , cmd->u.set_color_blend_equation_ext.color_blend_equations          );
          break;
      case VK_CMD_SET_COLOR_WRITE_MASK_EXT:
          disp->CmdSetColorWriteMaskEXT(commandBuffer
             , cmd->u.set_color_write_mask_ext.first_attachment             , cmd->u.set_color_write_mask_ext.attachment_count             , cmd->u.set_color_write_mask_ext.color_write_masks          );
          break;
      case VK_CMD_SET_RASTERIZATION_STREAM_EXT:
          disp->CmdSetRasterizationStreamEXT(commandBuffer
             , cmd->u.set_rasterization_stream_ext.rasterization_stream          );
          break;
      case VK_CMD_SET_CONSERVATIVE_RASTERIZATION_MODE_EXT:
          disp->CmdSetConservativeRasterizationModeEXT(commandBuffer
             , cmd->u.set_conservative_rasterization_mode_ext.conservative_rasterization_mode          );
          break;
      case VK_CMD_SET_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT:
          disp->CmdSetExtraPrimitiveOverestimationSizeEXT(commandBuffer
             , cmd->u.set_extra_primitive_overestimation_size_ext.extra_primitive_overestimation_size          );
          break;
      case VK_CMD_SET_DEPTH_CLIP_ENABLE_EXT:
          disp->CmdSetDepthClipEnableEXT(commandBuffer
             , cmd->u.set_depth_clip_enable_ext.depth_clip_enable          );
          break;
      case VK_CMD_SET_SAMPLE_LOCATIONS_ENABLE_EXT:
          disp->CmdSetSampleLocationsEnableEXT(commandBuffer
             , cmd->u.set_sample_locations_enable_ext.sample_locations_enable          );
          break;
      case VK_CMD_SET_COLOR_BLEND_ADVANCED_EXT:
          disp->CmdSetColorBlendAdvancedEXT(commandBuffer
             , cmd->u.set_color_blend_advanced_ext.first_attachment             , cmd->u.set_color_blend_advanced_ext.attachment_count             , cmd->u.set_color_blend_advanced_ext.color_blend_advanced          );
          break;
      case VK_CMD_SET_PROVOKING_VERTEX_MODE_EXT:
          disp->CmdSetProvokingVertexModeEXT(commandBuffer
             , cmd->u.set_provoking_vertex_mode_ext.provoking_vertex_mode          );
          break;
      case VK_CMD_SET_LINE_RASTERIZATION_MODE_EXT:
          disp->CmdSetLineRasterizationModeEXT(commandBuffer
             , cmd->u.set_line_rasterization_mode_ext.line_rasterization_mode          );
          break;
      case VK_CMD_SET_LINE_STIPPLE_ENABLE_EXT:
          disp->CmdSetLineStippleEnableEXT(commandBuffer
             , cmd->u.set_line_stipple_enable_ext.stippled_line_enable          );
          break;
      case VK_CMD_SET_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT:
          disp->CmdSetDepthClipNegativeOneToOneEXT(commandBuffer
             , cmd->u.set_depth_clip_negative_one_to_one_ext.negative_one_to_one          );
          break;
      case VK_CMD_SET_VIEWPORT_WSCALING_ENABLE_NV:
          disp->CmdSetViewportWScalingEnableNV(commandBuffer
             , cmd->u.set_viewport_wscaling_enable_nv.viewport_wscaling_enable          );
          break;
      case VK_CMD_SET_VIEWPORT_SWIZZLE_NV:
          disp->CmdSetViewportSwizzleNV(commandBuffer
             , cmd->u.set_viewport_swizzle_nv.first_viewport             , cmd->u.set_viewport_swizzle_nv.viewport_count             , cmd->u.set_viewport_swizzle_nv.viewport_swizzles          );
          break;
      case VK_CMD_SET_COVERAGE_TO_COLOR_ENABLE_NV:
          disp->CmdSetCoverageToColorEnableNV(commandBuffer
             , cmd->u.set_coverage_to_color_enable_nv.coverage_to_color_enable          );
          break;
      case VK_CMD_SET_COVERAGE_TO_COLOR_LOCATION_NV:
          disp->CmdSetCoverageToColorLocationNV(commandBuffer
             , cmd->u.set_coverage_to_color_location_nv.coverage_to_color_location          );
          break;
      case VK_CMD_SET_COVERAGE_MODULATION_MODE_NV:
          disp->CmdSetCoverageModulationModeNV(commandBuffer
             , cmd->u.set_coverage_modulation_mode_nv.coverage_modulation_mode          );
          break;
      case VK_CMD_SET_COVERAGE_MODULATION_TABLE_ENABLE_NV:
          disp->CmdSetCoverageModulationTableEnableNV(commandBuffer
             , cmd->u.set_coverage_modulation_table_enable_nv.coverage_modulation_table_enable          );
          break;
      case VK_CMD_SET_COVERAGE_MODULATION_TABLE_NV:
          disp->CmdSetCoverageModulationTableNV(commandBuffer
             , cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table_count             , cmd->u.set_coverage_modulation_table_nv.coverage_modulation_table          );
          break;
      case VK_CMD_SET_SHADING_RATE_IMAGE_ENABLE_NV:
          disp->CmdSetShadingRateImageEnableNV(commandBuffer
             , cmd->u.set_shading_rate_image_enable_nv.shading_rate_image_enable          );
          break;
      case VK_CMD_SET_COVERAGE_REDUCTION_MODE_NV:
          disp->CmdSetCoverageReductionModeNV(commandBuffer
             , cmd->u.set_coverage_reduction_mode_nv.coverage_reduction_mode          );
          break;
      case VK_CMD_SET_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV:
          disp->CmdSetRepresentativeFragmentTestEnableNV(commandBuffer
             , cmd->u.set_representative_fragment_test_enable_nv.representative_fragment_test_enable          );
          break;
      case VK_CMD_COPY_BUFFER2:
          disp->CmdCopyBuffer2(commandBuffer
             , cmd->u.copy_buffer2.copy_buffer_info          );
          break;
      case VK_CMD_COPY_IMAGE2:
          disp->CmdCopyImage2(commandBuffer
             , cmd->u.copy_image2.copy_image_info          );
          break;
      case VK_CMD_BLIT_IMAGE2:
          disp->CmdBlitImage2(commandBuffer
             , cmd->u.blit_image2.blit_image_info          );
          break;
      case VK_CMD_COPY_BUFFER_TO_IMAGE2:
          disp->CmdCopyBufferToImage2(commandBuffer
             , cmd->u.copy_buffer_to_image2.copy_buffer_to_image_info          );
          break;
      case VK_CMD_COPY_IMAGE_TO_BUFFER2:
          disp->CmdCopyImageToBuffer2(commandBuffer
             , cmd->u.copy_image_to_buffer2.copy_image_to_buffer_info          );
          break;
      case VK_CMD_RESOLVE_IMAGE2:
          disp->CmdResolveImage2(commandBuffer
             , cmd->u.resolve_image2.resolve_image_info          );
          break;
      case VK_CMD_SET_FRAGMENT_SHADING_RATE_KHR:
          disp->CmdSetFragmentShadingRateKHR(commandBuffer
             , cmd->u.set_fragment_shading_rate_khr.fragment_size             , cmd->u.set_fragment_shading_rate_khr.combiner_ops          );
          break;
      case VK_CMD_SET_FRAGMENT_SHADING_RATE_ENUM_NV:
          disp->CmdSetFragmentShadingRateEnumNV(commandBuffer
             , cmd->u.set_fragment_shading_rate_enum_nv.shading_rate             , cmd->u.set_fragment_shading_rate_enum_nv.combiner_ops          );
          break;
      case VK_CMD_SET_VERTEX_INPUT_EXT:
          disp->CmdSetVertexInputEXT(commandBuffer
             , cmd->u.set_vertex_input_ext.vertex_binding_description_count             , cmd->u.set_vertex_input_ext.vertex_binding_descriptions             , cmd->u.set_vertex_input_ext.vertex_attribute_description_count             , cmd->u.set_vertex_input_ext.vertex_attribute_descriptions          );
          break;
      case VK_CMD_SET_COLOR_WRITE_ENABLE_EXT:
          disp->CmdSetColorWriteEnableEXT(commandBuffer
             , cmd->u.set_color_write_enable_ext.attachment_count             , cmd->u.set_color_write_enable_ext.color_write_enables          );
          break;
      case VK_CMD_SET_EVENT2:
          disp->CmdSetEvent2(commandBuffer
             , cmd->u.set_event2.event             , cmd->u.set_event2.dependency_info          );
          break;
      case VK_CMD_RESET_EVENT2:
          disp->CmdResetEvent2(commandBuffer
             , cmd->u.reset_event2.event             , cmd->u.reset_event2.stage_mask          );
          break;
      case VK_CMD_WAIT_EVENTS2:
          disp->CmdWaitEvents2(commandBuffer
             , cmd->u.wait_events2.event_count             , cmd->u.wait_events2.events             , cmd->u.wait_events2.dependency_infos          );
          break;
      case VK_CMD_PIPELINE_BARRIER2:
          disp->CmdPipelineBarrier2(commandBuffer
             , cmd->u.pipeline_barrier2.dependency_info          );
          break;
      case VK_CMD_WRITE_TIMESTAMP2:
          disp->CmdWriteTimestamp2(commandBuffer
             , cmd->u.write_timestamp2.stage             , cmd->u.write_timestamp2.query_pool             , cmd->u.write_timestamp2.query          );
          break;
      case VK_CMD_WRITE_BUFFER_MARKER2_AMD:
          disp->CmdWriteBufferMarker2AMD(commandBuffer
             , cmd->u.write_buffer_marker2_amd.stage             , cmd->u.write_buffer_marker2_amd.dst_buffer             , cmd->u.write_buffer_marker2_amd.dst_offset             , cmd->u.write_buffer_marker2_amd.marker          );
          break;
      case VK_CMD_DECODE_VIDEO_KHR:
          disp->CmdDecodeVideoKHR(commandBuffer
             , cmd->u.decode_video_khr.decode_info          );
          break;
      case VK_CMD_BEGIN_VIDEO_CODING_KHR:
          disp->CmdBeginVideoCodingKHR(commandBuffer
             , cmd->u.begin_video_coding_khr.begin_info          );
          break;
      case VK_CMD_CONTROL_VIDEO_CODING_KHR:
          disp->CmdControlVideoCodingKHR(commandBuffer
             , cmd->u.control_video_coding_khr.coding_control_info          );
          break;
      case VK_CMD_END_VIDEO_CODING_KHR:
          disp->CmdEndVideoCodingKHR(commandBuffer
             , cmd->u.end_video_coding_khr.end_coding_info          );
          break;
      case VK_CMD_ENCODE_VIDEO_KHR:
          disp->CmdEncodeVideoKHR(commandBuffer
             , cmd->u.encode_video_khr.encode_info          );
          break;
      case VK_CMD_DECOMPRESS_MEMORY_NV:
          disp->CmdDecompressMemoryNV(commandBuffer
             , cmd->u.decompress_memory_nv.decompress_region_count             , cmd->u.decompress_memory_nv.decompress_memory_regions          );
          break;
      case VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_NV:
          disp->CmdDecompressMemoryIndirectCountNV(commandBuffer
             , cmd->u.decompress_memory_indirect_count_nv.indirect_commands_address             , cmd->u.decompress_memory_indirect_count_nv.indirect_commands_count_address             , cmd->u.decompress_memory_indirect_count_nv.stride          );
          break;
      case VK_CMD_BUILD_PARTITIONED_ACCELERATION_STRUCTURES_NV:
          disp->CmdBuildPartitionedAccelerationStructuresNV(commandBuffer
             , cmd->u.build_partitioned_acceleration_structures_nv.build_info          );
          break;
      case VK_CMD_DECOMPRESS_MEMORY_EXT:
          disp->CmdDecompressMemoryEXT(commandBuffer
             , cmd->u.decompress_memory_ext.decompress_memory_info_ext          );
          break;
      case VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_EXT:
          disp->CmdDecompressMemoryIndirectCountEXT(commandBuffer
             , cmd->u.decompress_memory_indirect_count_ext.decompression_method             , cmd->u.decompress_memory_indirect_count_ext.indirect_commands_address             , cmd->u.decompress_memory_indirect_count_ext.indirect_commands_count_address             , cmd->u.decompress_memory_indirect_count_ext.max_decompression_count             , cmd->u.decompress_memory_indirect_count_ext.stride          );
          break;
      case VK_CMD_CU_LAUNCH_KERNEL_NVX:
          disp->CmdCuLaunchKernelNVX(commandBuffer
             , cmd->u.cu_launch_kernel_nvx.launch_info          );
          break;
      case VK_CMD_BIND_DESCRIPTOR_BUFFERS_EXT:
          disp->CmdBindDescriptorBuffersEXT(commandBuffer
             , cmd->u.bind_descriptor_buffers_ext.buffer_count             , cmd->u.bind_descriptor_buffers_ext.binding_infos          );
          break;
      case VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS_EXT:
          disp->CmdSetDescriptorBufferOffsetsEXT(commandBuffer
             , cmd->u.set_descriptor_buffer_offsets_ext.pipeline_bind_point             , cmd->u.set_descriptor_buffer_offsets_ext.layout             , cmd->u.set_descriptor_buffer_offsets_ext.first_set             , cmd->u.set_descriptor_buffer_offsets_ext.set_count             , cmd->u.set_descriptor_buffer_offsets_ext.buffer_indices             , cmd->u.set_descriptor_buffer_offsets_ext.offsets          );
          break;
      case VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS_EXT:
          disp->CmdBindDescriptorBufferEmbeddedSamplersEXT(commandBuffer
             , cmd->u.bind_descriptor_buffer_embedded_samplers_ext.pipeline_bind_point             , cmd->u.bind_descriptor_buffer_embedded_samplers_ext.layout             , cmd->u.bind_descriptor_buffer_embedded_samplers_ext.set          );
          break;
      case VK_CMD_BEGIN_RENDERING:
          disp->CmdBeginRendering(commandBuffer
             , cmd->u.begin_rendering.rendering_info          );
          break;
      case VK_CMD_END_RENDERING:
          disp->CmdEndRendering(commandBuffer
          );
          break;
      case VK_CMD_END_RENDERING2_KHR:
          disp->CmdEndRendering2KHR(commandBuffer
             , cmd->u.end_rendering2_khr.rendering_end_info          );
          break;
      case VK_CMD_BUILD_MICROMAPS_EXT:
          disp->CmdBuildMicromapsEXT(commandBuffer
             , cmd->u.build_micromaps_ext.info_count             , cmd->u.build_micromaps_ext.infos          );
          break;
      case VK_CMD_COPY_MICROMAP_EXT:
          disp->CmdCopyMicromapEXT(commandBuffer
             , cmd->u.copy_micromap_ext.info          );
          break;
      case VK_CMD_COPY_MICROMAP_TO_MEMORY_EXT:
          disp->CmdCopyMicromapToMemoryEXT(commandBuffer
             , cmd->u.copy_micromap_to_memory_ext.info          );
          break;
      case VK_CMD_COPY_MEMORY_TO_MICROMAP_EXT:
          disp->CmdCopyMemoryToMicromapEXT(commandBuffer
             , cmd->u.copy_memory_to_micromap_ext.info          );
          break;
      case VK_CMD_WRITE_MICROMAPS_PROPERTIES_EXT:
          disp->CmdWriteMicromapsPropertiesEXT(commandBuffer
             , cmd->u.write_micromaps_properties_ext.micromap_count             , cmd->u.write_micromaps_properties_ext.micromaps             , cmd->u.write_micromaps_properties_ext.query_type             , cmd->u.write_micromaps_properties_ext.query_pool             , cmd->u.write_micromaps_properties_ext.first_query          );
          break;
      case VK_CMD_BIND_TILE_MEMORY_QCOM:
          disp->CmdBindTileMemoryQCOM(commandBuffer
             , cmd->u.bind_tile_memory_qcom.tile_memory_bind_info          );
          break;
      case VK_CMD_OPTICAL_FLOW_EXECUTE_NV:
          disp->CmdOpticalFlowExecuteNV(commandBuffer
             , cmd->u.optical_flow_execute_nv.session             , cmd->u.optical_flow_execute_nv.execute_info          );
          break;
      case VK_CMD_SET_DEPTH_BIAS2_EXT:
          disp->CmdSetDepthBias2EXT(commandBuffer
             , cmd->u.set_depth_bias2_ext.depth_bias_info          );
          break;
      case VK_CMD_BIND_SHADERS_EXT:
          disp->CmdBindShadersEXT(commandBuffer
             , cmd->u.bind_shaders_ext.stage_count             , cmd->u.bind_shaders_ext.stages             , cmd->u.bind_shaders_ext.shaders          );
          break;
      case VK_CMD_BEGIN_GPA_SESSION_AMD:
          disp->CmdBeginGpaSessionAMD(commandBuffer
             , cmd->u.begin_gpa_session_amd.gpa_session          );
          break;
      case VK_CMD_END_GPA_SESSION_AMD:
          disp->CmdEndGpaSessionAMD(commandBuffer
             , cmd->u.end_gpa_session_amd.gpa_session          );
          break;
      case VK_CMD_BEGIN_GPA_SAMPLE_AMD:
          disp->CmdBeginGpaSampleAMD(commandBuffer
             , cmd->u.begin_gpa_sample_amd.gpa_session             , cmd->u.begin_gpa_sample_amd.gpa_sample_begin_info             , cmd->u.begin_gpa_sample_amd.sample_id          );
          break;
      case VK_CMD_END_GPA_SAMPLE_AMD:
          disp->CmdEndGpaSampleAMD(commandBuffer
             , cmd->u.end_gpa_sample_amd.gpa_session             , cmd->u.end_gpa_sample_amd.sample_id          );
          break;
      case VK_CMD_COPY_GPA_SESSION_RESULTS_AMD:
          disp->CmdCopyGpaSessionResultsAMD(commandBuffer
             , cmd->u.copy_gpa_session_results_amd.gpa_session          );
          break;
      case VK_CMD_BIND_DESCRIPTOR_SETS2:
          disp->CmdBindDescriptorSets2(commandBuffer
             , cmd->u.bind_descriptor_sets2.bind_descriptor_sets_info          );
          break;
      case VK_CMD_PUSH_CONSTANTS2:
          disp->CmdPushConstants2(commandBuffer
             , cmd->u.push_constants2.push_constants_info          );
          break;
      case VK_CMD_PUSH_DESCRIPTOR_SET2:
          disp->CmdPushDescriptorSet2(commandBuffer
             , cmd->u.push_descriptor_set2.push_descriptor_set_info          );
          break;
      case VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE2:
          disp->CmdPushDescriptorSetWithTemplate2(commandBuffer
             , cmd->u.push_descriptor_set_with_template2.push_descriptor_set_with_template_info          );
          break;
      case VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS2_EXT:
          disp->CmdSetDescriptorBufferOffsets2EXT(commandBuffer
             , cmd->u.set_descriptor_buffer_offsets2_ext.set_descriptor_buffer_offsets_info          );
          break;
      case VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS2_EXT:
          disp->CmdBindDescriptorBufferEmbeddedSamplers2EXT(commandBuffer
             , cmd->u.bind_descriptor_buffer_embedded_samplers2_ext.bind_descriptor_buffer_embedded_samplers_info          );
          break;
      case VK_CMD_SET_RENDERING_ATTACHMENT_LOCATIONS:
          disp->CmdSetRenderingAttachmentLocations(commandBuffer
             , cmd->u.set_rendering_attachment_locations.location_info          );
          break;
      case VK_CMD_SET_RENDERING_INPUT_ATTACHMENT_INDICES:
          disp->CmdSetRenderingInputAttachmentIndices(commandBuffer
             , cmd->u.set_rendering_input_attachment_indices.input_attachment_index_info          );
          break;
      case VK_CMD_SET_DEPTH_CLAMP_RANGE_EXT:
          disp->CmdSetDepthClampRangeEXT(commandBuffer
             , cmd->u.set_depth_clamp_range_ext.depth_clamp_mode             , cmd->u.set_depth_clamp_range_ext.depth_clamp_range          );
          break;
      case VK_CMD_CONVERT_COOPERATIVE_VECTOR_MATRIX_NV:
          disp->CmdConvertCooperativeVectorMatrixNV(commandBuffer
             , cmd->u.convert_cooperative_vector_matrix_nv.info_count             , cmd->u.convert_cooperative_vector_matrix_nv.infos          );
          break;
      case VK_CMD_DISPATCH_TILE_QCOM:
          disp->CmdDispatchTileQCOM(commandBuffer
             , cmd->u.dispatch_tile_qcom.dispatch_tile_info          );
          break;
      case VK_CMD_BEGIN_PER_TILE_EXECUTION_QCOM:
          disp->CmdBeginPerTileExecutionQCOM(commandBuffer
             , cmd->u.begin_per_tile_execution_qcom.per_tile_begin_info          );
          break;
      case VK_CMD_END_PER_TILE_EXECUTION_QCOM:
          disp->CmdEndPerTileExecutionQCOM(commandBuffer
             , cmd->u.end_per_tile_execution_qcom.per_tile_end_info          );
          break;
      case VK_CMD_BEGIN_SHADER_INSTRUMENTATION_ARM:
          disp->CmdBeginShaderInstrumentationARM(commandBuffer
             , cmd->u.begin_shader_instrumentation_arm.instrumentation          );
          break;
      case VK_CMD_END_SHADER_INSTRUMENTATION_ARM:
          disp->CmdEndShaderInstrumentationARM(commandBuffer
          );
          break;
      case VK_CMD_COPY_TENSOR_ARM:
          disp->CmdCopyTensorARM(commandBuffer
             , cmd->u.copy_tensor_arm.copy_tensor_info          );
          break;
      case VK_CMD_DISPATCH_DATA_GRAPH_ARM:
          disp->CmdDispatchDataGraphARM(commandBuffer
             , cmd->u.dispatch_data_graph_arm.session             , cmd->u.dispatch_data_graph_arm.info          );
          break;
      case VK_CMD_SET_COMPUTE_OCCUPANCY_PRIORITY_NV:
          disp->CmdSetComputeOccupancyPriorityNV(commandBuffer
             , cmd->u.set_compute_occupancy_priority_nv.parameters          );
          break;
      case VK_CMD_BIND_SAMPLER_HEAP_EXT:
          disp->CmdBindSamplerHeapEXT(commandBuffer
             , cmd->u.bind_sampler_heap_ext.bind_info          );
          break;
      case VK_CMD_BIND_RESOURCE_HEAP_EXT:
          disp->CmdBindResourceHeapEXT(commandBuffer
             , cmd->u.bind_resource_heap_ext.bind_info          );
          break;
      case VK_CMD_PUSH_DATA_EXT:
          disp->CmdPushDataEXT(commandBuffer
             , cmd->u.push_data_ext.push_data_info          );
          break;
      case VK_CMD_COPY_MEMORY_KHR:
          disp->CmdCopyMemoryKHR(commandBuffer
             , cmd->u.copy_memory_khr.copy_memory_info          );
          break;
      case VK_CMD_COPY_MEMORY_TO_IMAGE_KHR:
          disp->CmdCopyMemoryToImageKHR(commandBuffer
             , cmd->u.copy_memory_to_image_khr.copy_memory_info          );
          break;
      case VK_CMD_COPY_IMAGE_TO_MEMORY_KHR:
          disp->CmdCopyImageToMemoryKHR(commandBuffer
             , cmd->u.copy_image_to_memory_khr.copy_memory_info          );
          break;
      case VK_CMD_UPDATE_MEMORY_KHR:
          disp->CmdUpdateMemoryKHR(commandBuffer
             , cmd->u.update_memory_khr.dst_range             , cmd->u.update_memory_khr.dst_flags             , cmd->u.update_memory_khr.data_size             , cmd->u.update_memory_khr.data          );
          break;
      case VK_CMD_FILL_MEMORY_KHR:
          disp->CmdFillMemoryKHR(commandBuffer
             , cmd->u.fill_memory_khr.dst_range             , cmd->u.fill_memory_khr.dst_flags             , cmd->u.fill_memory_khr.data          );
          break;
      case VK_CMD_COPY_QUERY_POOL_RESULTS_TO_MEMORY_KHR:
          disp->CmdCopyQueryPoolResultsToMemoryKHR(commandBuffer
             , cmd->u.copy_query_pool_results_to_memory_khr.query_pool             , cmd->u.copy_query_pool_results_to_memory_khr.first_query             , cmd->u.copy_query_pool_results_to_memory_khr.query_count             , cmd->u.copy_query_pool_results_to_memory_khr.dst_range             , cmd->u.copy_query_pool_results_to_memory_khr.dst_flags             , cmd->u.copy_query_pool_results_to_memory_khr.query_result_flags          );
          break;
      case VK_CMD_BEGIN_CONDITIONAL_RENDERING2_EXT:
          disp->CmdBeginConditionalRendering2EXT(commandBuffer
             , cmd->u.begin_conditional_rendering2_ext.conditional_rendering_begin          );
          break;
      case VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS2_EXT:
          disp->CmdBindTransformFeedbackBuffers2EXT(commandBuffer
             , cmd->u.bind_transform_feedback_buffers2_ext.first_binding             , cmd->u.bind_transform_feedback_buffers2_ext.binding_count             , cmd->u.bind_transform_feedback_buffers2_ext.binding_infos          );
          break;
      case VK_CMD_BEGIN_TRANSFORM_FEEDBACK2_EXT:
          disp->CmdBeginTransformFeedback2EXT(commandBuffer
             , cmd->u.begin_transform_feedback2_ext.first_counter_range             , cmd->u.begin_transform_feedback2_ext.counter_range_count             , cmd->u.begin_transform_feedback2_ext.counter_infos          );
          break;
      case VK_CMD_END_TRANSFORM_FEEDBACK2_EXT:
          disp->CmdEndTransformFeedback2EXT(commandBuffer
             , cmd->u.end_transform_feedback2_ext.first_counter_range             , cmd->u.end_transform_feedback2_ext.counter_range_count             , cmd->u.end_transform_feedback2_ext.counter_infos          );
          break;
      case VK_CMD_DRAW_INDIRECT_BYTE_COUNT2_EXT:
          disp->CmdDrawIndirectByteCount2EXT(commandBuffer
             , cmd->u.draw_indirect_byte_count2_ext.instance_count             , cmd->u.draw_indirect_byte_count2_ext.first_instance             , cmd->u.draw_indirect_byte_count2_ext.counter_info             , cmd->u.draw_indirect_byte_count2_ext.counter_offset             , cmd->u.draw_indirect_byte_count2_ext.vertex_stride          );
          break;
      case VK_CMD_WRITE_MARKER_TO_MEMORY_AMD:
          disp->CmdWriteMarkerToMemoryAMD(commandBuffer
             , cmd->u.write_marker_to_memory_amd.info          );
          break;
      case VK_CMD_BIND_INDEX_BUFFER3_KHR:
          disp->CmdBindIndexBuffer3KHR(commandBuffer
             , cmd->u.bind_index_buffer3_khr.info          );
          break;
      case VK_CMD_BIND_VERTEX_BUFFERS3_KHR:
          disp->CmdBindVertexBuffers3KHR(commandBuffer
             , cmd->u.bind_vertex_buffers3_khr.first_binding             , cmd->u.bind_vertex_buffers3_khr.binding_count             , cmd->u.bind_vertex_buffers3_khr.binding_infos          );
          break;
      case VK_CMD_DRAW_INDIRECT2_KHR:
          disp->CmdDrawIndirect2KHR(commandBuffer
             , cmd->u.draw_indirect2_khr.info          );
          break;
      case VK_CMD_DRAW_INDEXED_INDIRECT2_KHR:
          disp->CmdDrawIndexedIndirect2KHR(commandBuffer
             , cmd->u.draw_indexed_indirect2_khr.info          );
          break;
      case VK_CMD_DRAW_INDIRECT_COUNT2_KHR:
          disp->CmdDrawIndirectCount2KHR(commandBuffer
             , cmd->u.draw_indirect_count2_khr.info          );
          break;
      case VK_CMD_DRAW_INDEXED_INDIRECT_COUNT2_KHR:
          disp->CmdDrawIndexedIndirectCount2KHR(commandBuffer
             , cmd->u.draw_indexed_indirect_count2_khr.info          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT2_EXT:
          disp->CmdDrawMeshTasksIndirect2EXT(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect2_ext.info          );
          break;
      case VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT2_EXT:
          disp->CmdDrawMeshTasksIndirectCount2EXT(commandBuffer
             , cmd->u.draw_mesh_tasks_indirect_count2_ext.info          );
          break;
      case VK_CMD_DISPATCH_INDIRECT2_KHR:
          disp->CmdDispatchIndirect2KHR(commandBuffer
             , cmd->u.dispatch_indirect2_khr.info          );
          break;
      case VK_CMD_SET_DISPATCH_PARAMETERS_ARM:
          disp->CmdSetDispatchParametersARM(commandBuffer
             , cmd->u.set_dispatch_parameters_arm.dispatch_parameters          );
          break;
      default: UNREACHABLE("Unsupported command");
      }
   }
}




VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_pipeline(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, pipeline);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
   } else {
      vk_cmd_enqueue_CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetPrimitiveRestartIndexEXT(VkCommandBuffer commandBuffer, uint32_t primitiveRestartIndex)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_primitive_restart_index_ext(&cmd_buffer->cmd_queue,
                                       primitiveRestartIndex);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetPrimitiveRestartIndexEXT(VkCommandBuffer commandBuffer, uint32_t primitiveRestartIndex)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetPrimitiveRestartIndexEXT(commandBuffer, primitiveRestartIndex);
   } else {
      vk_cmd_enqueue_CmdSetPrimitiveRestartIndexEXT(commandBuffer, primitiveRestartIndex);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetAttachmentFeedbackLoopEnableEXT(VkCommandBuffer commandBuffer, VkImageAspectFlags aspectMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_attachment_feedback_loop_enable_ext(&cmd_buffer->cmd_queue,
                                       aspectMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetAttachmentFeedbackLoopEnableEXT(VkCommandBuffer commandBuffer, VkImageAspectFlags aspectMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetAttachmentFeedbackLoopEnableEXT(commandBuffer, aspectMask);
   } else {
      vk_cmd_enqueue_CmdSetAttachmentFeedbackLoopEnableEXT(commandBuffer, aspectMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport(&cmd_buffer->cmd_queue,
                                       firstViewport, viewportCount, pViewports);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
   } else {
      vk_cmd_enqueue_CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_scissor(&cmd_buffer->cmd_queue,
                                       firstScissor, scissorCount, pScissors);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
   } else {
      vk_cmd_enqueue_CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_line_width(&cmd_buffer->cmd_queue,
                                       lineWidth);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLineWidth(commandBuffer, lineWidth);
   } else {
      vk_cmd_enqueue_CmdSetLineWidth(commandBuffer, lineWidth);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_bias(&cmd_buffer->cmd_queue,
                                       depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
   } else {
      vk_cmd_enqueue_CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_blend_constants(&cmd_buffer->cmd_queue,
                                       blendConstants);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetBlendConstants(commandBuffer, blendConstants);
   } else {
      vk_cmd_enqueue_CmdSetBlendConstants(commandBuffer, blendConstants);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_bounds(&cmd_buffer->cmd_queue,
                                       minDepthBounds, maxDepthBounds);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
   } else {
      vk_cmd_enqueue_CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_stencil_compare_mask(&cmd_buffer->cmd_queue,
                                       faceMask, compareMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
   } else {
      vk_cmd_enqueue_CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_stencil_write_mask(&cmd_buffer->cmd_queue,
                                       faceMask, writeMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
   } else {
      vk_cmd_enqueue_CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_stencil_reference(&cmd_buffer->cmd_queue,
                                       faceMask, reference);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetStencilReference(commandBuffer, faceMask, reference);
   } else {
      vk_cmd_enqueue_CmdSetStencilReference(commandBuffer, faceMask, reference);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_descriptor_sets(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
   } else {
      vk_cmd_enqueue_CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_index_buffer(&cmd_buffer->cmd_queue,
                                       buffer, offset, indexType);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
   } else {
      vk_cmd_enqueue_CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_vertex_buffers(&cmd_buffer->cmd_queue,
                                       firstBinding, bindingCount, pBuffers, pOffsets);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
   } else {
      vk_cmd_enqueue_CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw(&cmd_buffer->cmd_queue,
                                       vertexCount, instanceCount, firstVertex, firstInstance);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
   } else {
      vk_cmd_enqueue_CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indexed(&cmd_buffer->cmd_queue,
                                       indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
   } else {
      vk_cmd_enqueue_CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
   }
}



/* vk_cmd_enqueue_CmdDrawMultiEXT() is hand-typed in vk_cmd_enqueue.c */

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMultiEXT(VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawInfoEXT* pVertexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
   } else {
      vk_cmd_enqueue_CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
   }
}



/* vk_cmd_enqueue_CmdDrawMultiIndexedEXT() is hand-typed in vk_cmd_enqueue.c */

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMultiIndexedEXT(VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawIndexedInfoEXT* pIndexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride, const int32_t* pVertexOffset)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
   } else {
      vk_cmd_enqueue_CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect(&cmd_buffer->cmd_queue,
                                       buffer, offset, drawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indexed_indirect(&cmd_buffer->cmd_queue,
                                       buffer, offset, drawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch(&cmd_buffer->cmd_queue,
                                       groupCountX, groupCountY, groupCountZ);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
   } else {
      vk_cmd_enqueue_CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch_indirect(&cmd_buffer->cmd_queue,
                                       buffer, offset);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatchIndirect(commandBuffer, buffer, offset);
   } else {
      vk_cmd_enqueue_CmdDispatchIndirect(commandBuffer, buffer, offset);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSubpassShadingHUAWEI(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_subpass_shading_huawei(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSubpassShadingHUAWEI(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSubpassShadingHUAWEI(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdSubpassShadingHUAWEI(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawClusterHUAWEI(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_cluster_huawei(&cmd_buffer->cmd_queue,
                                       groupCountX, groupCountY, groupCountZ);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawClusterHUAWEI(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawClusterHUAWEI(commandBuffer, groupCountX, groupCountY, groupCountZ);
   } else {
      vk_cmd_enqueue_CmdDrawClusterHUAWEI(commandBuffer, groupCountX, groupCountY, groupCountZ);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawClusterIndirectHUAWEI(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_cluster_indirect_huawei(&cmd_buffer->cmd_queue,
                                       buffer, offset);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawClusterIndirectHUAWEI(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawClusterIndirectHUAWEI(commandBuffer, buffer, offset);
   } else {
      vk_cmd_enqueue_CmdDrawClusterIndirectHUAWEI(commandBuffer, buffer, offset);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdUpdatePipelineIndirectBufferNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint           pipelineBindPoint, VkPipeline                    pipeline)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_update_pipeline_indirect_buffer_nv(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, pipeline);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdUpdatePipelineIndirectBufferNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint           pipelineBindPoint, VkPipeline                    pipeline)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdUpdatePipelineIndirectBufferNV(commandBuffer, pipelineBindPoint, pipeline);
   } else {
      vk_cmd_enqueue_CmdUpdatePipelineIndirectBufferNV(commandBuffer, pipelineBindPoint, pipeline);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_buffer(&cmd_buffer->cmd_queue,
                                       srcBuffer, dstBuffer, regionCount, pRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
   } else {
      vk_cmd_enqueue_CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_image(&cmd_buffer->cmd_queue,
                                       srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   } else {
      vk_cmd_enqueue_CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_blit_image(&cmd_buffer->cmd_queue,
                                       srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
   } else {
      vk_cmd_enqueue_CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_buffer_to_image(&cmd_buffer->cmd_queue,
                                       srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
   } else {
      vk_cmd_enqueue_CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_image_to_buffer(&cmd_buffer->cmd_queue,
                                       srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
   } else {
      vk_cmd_enqueue_CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_indirect_nv(&cmd_buffer->cmd_queue,
                                       copyBufferAddress, copyCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_indirect_khr(&cmd_buffer->cmd_queue,
                                       pCopyMemoryIndirectInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryIndirectKHR(commandBuffer, pCopyMemoryIndirectInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryIndirectKHR(commandBuffer, pCopyMemoryIndirectInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryToImageIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageSubresourceLayers* pImageSubresources)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_to_image_indirect_nv(&cmd_buffer->cmd_queue,
                                       copyBufferAddress, copyCount, stride, dstImage, dstImageLayout, pImageSubresources);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryToImageIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageSubresourceLayers* pImageSubresources)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryToImageIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride, dstImage, dstImageLayout, pImageSubresources);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryToImageIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride, dstImage, dstImageLayout, pImageSubresources);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryToImageIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_to_image_indirect_khr(&cmd_buffer->cmd_queue,
                                       pCopyMemoryToImageIndirectInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryToImageIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryToImageIndirectKHR(commandBuffer, pCopyMemoryToImageIndirectInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryToImageIndirectKHR(commandBuffer, pCopyMemoryToImageIndirectInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_update_buffer(&cmd_buffer->cmd_queue,
                                       dstBuffer, dstOffset, dataSize, pData);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
   } else {
      vk_cmd_enqueue_CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_fill_buffer(&cmd_buffer->cmd_queue,
                                       dstBuffer, dstOffset, size, data);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
   } else {
      vk_cmd_enqueue_CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_clear_color_image(&cmd_buffer->cmd_queue,
                                       image, imageLayout, pColor, rangeCount, pRanges);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
   } else {
      vk_cmd_enqueue_CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_clear_depth_stencil_image(&cmd_buffer->cmd_queue,
                                       image, imageLayout, pDepthStencil, rangeCount, pRanges);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
   } else {
      vk_cmd_enqueue_CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_clear_attachments(&cmd_buffer->cmd_queue,
                                       attachmentCount, pAttachments, rectCount, pRects);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
   } else {
      vk_cmd_enqueue_CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_resolve_image(&cmd_buffer->cmd_queue,
                                       srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   } else {
      vk_cmd_enqueue_CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_event(&cmd_buffer->cmd_queue,
                                       event, stageMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetEvent(commandBuffer, event, stageMask);
   } else {
      vk_cmd_enqueue_CmdSetEvent(commandBuffer, event, stageMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_reset_event(&cmd_buffer->cmd_queue,
                                       event, stageMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdResetEvent(commandBuffer, event, stageMask);
   } else {
      vk_cmd_enqueue_CmdResetEvent(commandBuffer, event, stageMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_wait_events(&cmd_buffer->cmd_queue,
                                       eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   } else {
      vk_cmd_enqueue_CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_pipeline_barrier(&cmd_buffer->cmd_queue,
                                       srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   } else {
      vk_cmd_enqueue_CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_query(&cmd_buffer->cmd_queue,
                                       queryPool, query, flags);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginQuery(commandBuffer, queryPool, query, flags);
   } else {
      vk_cmd_enqueue_CmdBeginQuery(commandBuffer, queryPool, query, flags);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_query(&cmd_buffer->cmd_queue,
                                       queryPool, query);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndQuery(commandBuffer, queryPool, query);
   } else {
      vk_cmd_enqueue_CmdEndQuery(commandBuffer, queryPool, query);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginConditionalRenderingEXT(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_conditional_rendering_ext(&cmd_buffer->cmd_queue,
                                       pConditionalRenderingBegin);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginConditionalRenderingEXT(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
   } else {
      vk_cmd_enqueue_CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndConditionalRenderingEXT(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_conditional_rendering_ext(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndConditionalRenderingEXT(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndConditionalRenderingEXT(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdEndConditionalRenderingEXT(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginCustomResolveEXT(VkCommandBuffer commandBuffer, const VkBeginCustomResolveInfoEXT* pBeginCustomResolveInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_custom_resolve_ext(&cmd_buffer->cmd_queue,
                                       pBeginCustomResolveInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginCustomResolveEXT(VkCommandBuffer commandBuffer, const VkBeginCustomResolveInfoEXT* pBeginCustomResolveInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginCustomResolveEXT(commandBuffer, pBeginCustomResolveInfo);
   } else {
      vk_cmd_enqueue_CmdBeginCustomResolveEXT(commandBuffer, pBeginCustomResolveInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_reset_query_pool(&cmd_buffer->cmd_queue,
                                       queryPool, firstQuery, queryCount);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
   } else {
      vk_cmd_enqueue_CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_timestamp(&cmd_buffer->cmd_queue,
                                       pipelineStage, queryPool, query);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
   } else {
      vk_cmd_enqueue_CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_query_pool_results(&cmd_buffer->cmd_queue,
                                       queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
   } else {
      vk_cmd_enqueue_CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_constants(&cmd_buffer->cmd_queue,
                                       layout, stageFlags, offset, size, pValues);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
   } else {
      vk_cmd_enqueue_CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_render_pass(&cmd_buffer->cmd_queue,
                                       pRenderPassBegin, contents);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
   } else {
      vk_cmd_enqueue_CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_next_subpass(&cmd_buffer->cmd_queue,
                                       contents);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdNextSubpass(commandBuffer, contents);
   } else {
      vk_cmd_enqueue_CmdNextSubpass(commandBuffer, contents);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndRenderPass(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_render_pass(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndRenderPass(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndRenderPass(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdEndRenderPass(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_execute_commands(&cmd_buffer->cmd_queue,
                                       commandBufferCount, pCommandBuffers);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
   } else {
      vk_cmd_enqueue_CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_debug_marker_begin_ext(&cmd_buffer->cmd_queue,
                                       pMarkerInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
   } else {
      vk_cmd_enqueue_CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_debug_marker_end_ext(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDebugMarkerEndEXT(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdDebugMarkerEndEXT(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_debug_marker_insert_ext(&cmd_buffer->cmd_queue,
                                       pMarkerInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
   } else {
      vk_cmd_enqueue_CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdExecuteGeneratedCommandsNV(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_execute_generated_commands_nv(&cmd_buffer->cmd_queue,
                                       isPreprocessed, pGeneratedCommandsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdExecuteGeneratedCommandsNV(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
   } else {
      vk_cmd_enqueue_CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPreprocessGeneratedCommandsNV(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_preprocess_generated_commands_nv(&cmd_buffer->cmd_queue,
                                       pGeneratedCommandsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPreprocessGeneratedCommandsNV(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
   } else {
      vk_cmd_enqueue_CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindPipelineShaderGroupNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline, uint32_t groupIndex)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_pipeline_shader_group_nv(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, pipeline, groupIndex);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindPipelineShaderGroupNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline, uint32_t groupIndex)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
   } else {
      vk_cmd_enqueue_CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdExecuteGeneratedCommandsEXT(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_execute_generated_commands_ext(&cmd_buffer->cmd_queue,
                                       isPreprocessed, pGeneratedCommandsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdExecuteGeneratedCommandsEXT(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdExecuteGeneratedCommandsEXT(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
   } else {
      vk_cmd_enqueue_CmdExecuteGeneratedCommandsEXT(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPreprocessGeneratedCommandsEXT(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo, VkCommandBuffer stateCommandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_preprocess_generated_commands_ext(&cmd_buffer->cmd_queue,
                                       pGeneratedCommandsInfo, stateCommandBuffer);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPreprocessGeneratedCommandsEXT(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo, VkCommandBuffer stateCommandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPreprocessGeneratedCommandsEXT(commandBuffer, pGeneratedCommandsInfo, stateCommandBuffer);
   } else {
      vk_cmd_enqueue_CmdPreprocessGeneratedCommandsEXT(commandBuffer, pGeneratedCommandsInfo, stateCommandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_descriptor_set(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushDescriptorSet(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
   } else {
      vk_cmd_enqueue_CmdPushDescriptorSet(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_device_mask(&cmd_buffer->cmd_queue,
                                       deviceMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDeviceMask(commandBuffer, deviceMask);
   } else {
      vk_cmd_enqueue_CmdSetDeviceMask(commandBuffer, deviceMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch_base(&cmd_buffer->cmd_queue,
                                       baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
   } else {
      vk_cmd_enqueue_CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushDescriptorSetWithTemplate(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_descriptor_set_with_template(&cmd_buffer->cmd_queue,
                                       descriptorUpdateTemplate, layout, set, pData);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushDescriptorSetWithTemplate(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushDescriptorSetWithTemplate(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
   } else {
      vk_cmd_enqueue_CmdPushDescriptorSetWithTemplate(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportWScalingNV* pViewportWScalings)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport_wscaling_nv(&cmd_buffer->cmd_queue,
                                       firstViewport, viewportCount, pViewportWScalings);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportWScalingNV* pViewportWScalings)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
   } else {
      vk_cmd_enqueue_CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer, uint32_t firstDiscardRectangle, uint32_t discardRectangleCount, const VkRect2D* pDiscardRectangles)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_discard_rectangle_ext(&cmd_buffer->cmd_queue,
                                       firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer, uint32_t firstDiscardRectangle, uint32_t discardRectangleCount, const VkRect2D* pDiscardRectangles)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
   } else {
      vk_cmd_enqueue_CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDiscardRectangleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 discardRectangleEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_discard_rectangle_enable_ext(&cmd_buffer->cmd_queue,
                                       discardRectangleEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDiscardRectangleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 discardRectangleEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDiscardRectangleEnableEXT(commandBuffer, discardRectangleEnable);
   } else {
      vk_cmd_enqueue_CmdSetDiscardRectangleEnableEXT(commandBuffer, discardRectangleEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDiscardRectangleModeEXT(VkCommandBuffer commandBuffer, VkDiscardRectangleModeEXT discardRectangleMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_discard_rectangle_mode_ext(&cmd_buffer->cmd_queue,
                                       discardRectangleMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDiscardRectangleModeEXT(VkCommandBuffer commandBuffer, VkDiscardRectangleModeEXT discardRectangleMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDiscardRectangleModeEXT(commandBuffer, discardRectangleMode);
   } else {
      vk_cmd_enqueue_CmdSetDiscardRectangleModeEXT(commandBuffer, discardRectangleMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT* pSampleLocationsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_sample_locations_ext(&cmd_buffer->cmd_queue,
                                       pSampleLocationsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT* pSampleLocationsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
   } else {
      vk_cmd_enqueue_CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_debug_utils_label_ext(&cmd_buffer->cmd_queue,
                                       pLabelInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
   } else {
      vk_cmd_enqueue_CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_debug_utils_label_ext(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndDebugUtilsLabelEXT(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdEndDebugUtilsLabelEXT(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_insert_debug_utils_label_ext(&cmd_buffer->cmd_queue,
                                       pLabelInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
   } else {
      vk_cmd_enqueue_CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_buffer_marker_amd(&cmd_buffer->cmd_queue,
                                       pipelineStage, dstBuffer, dstOffset, marker);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
   } else {
      vk_cmd_enqueue_CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginRenderPass2(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo*      pRenderPassBegin, const VkSubpassBeginInfo*      pSubpassBeginInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_render_pass2(&cmd_buffer->cmd_queue,
                                       pRenderPassBegin, pSubpassBeginInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginRenderPass2(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo*      pRenderPassBegin, const VkSubpassBeginInfo*      pSubpassBeginInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
   } else {
      vk_cmd_enqueue_CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdNextSubpass2(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo*      pSubpassBeginInfo, const VkSubpassEndInfo*        pSubpassEndInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_next_subpass2(&cmd_buffer->cmd_queue,
                                       pSubpassBeginInfo, pSubpassEndInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdNextSubpass2(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo*      pSubpassBeginInfo, const VkSubpassEndInfo*        pSubpassEndInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
   } else {
      vk_cmd_enqueue_CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndRenderPass2(VkCommandBuffer commandBuffer, const VkSubpassEndInfo*        pSubpassEndInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_render_pass2(&cmd_buffer->cmd_queue,
                                       pSubpassEndInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndRenderPass2(VkCommandBuffer commandBuffer, const VkSubpassEndInfo*        pSubpassEndInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
   } else {
      vk_cmd_enqueue_CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect_count(&cmd_buffer->cmd_queue,
                                       buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indexed_indirect_count(&cmd_buffer->cmd_queue,
                                       buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCheckpointNV(VkCommandBuffer commandBuffer, const void* pCheckpointMarker)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_checkpoint_nv(&cmd_buffer->cmd_queue,
                                       pCheckpointMarker);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCheckpointNV(VkCommandBuffer commandBuffer, const void* pCheckpointMarker)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
   } else {
      vk_cmd_enqueue_CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindTransformFeedbackBuffersEXT(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_transform_feedback_buffers_ext(&cmd_buffer->cmd_queue,
                                       firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindTransformFeedbackBuffersEXT(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
   } else {
      vk_cmd_enqueue_CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_transform_feedback_ext(&cmd_buffer->cmd_queue,
                                       firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   } else {
      vk_cmd_enqueue_CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_transform_feedback_ext(&cmd_buffer->cmd_queue,
                                       firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   } else {
      vk_cmd_enqueue_CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags, uint32_t index)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_query_indexed_ext(&cmd_buffer->cmd_queue,
                                       queryPool, query, flags, index);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags, uint32_t index)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
   } else {
      vk_cmd_enqueue_CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, uint32_t index)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_query_indexed_ext(&cmd_buffer->cmd_queue,
                                       queryPool, query, index);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, uint32_t index)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
   } else {
      vk_cmd_enqueue_CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirectByteCountEXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, VkBuffer counterBuffer, VkDeviceSize counterBufferOffset, uint32_t counterOffset, uint32_t vertexStride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect_byte_count_ext(&cmd_buffer->cmd_queue,
                                       instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirectByteCountEXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, VkBuffer counterBuffer, VkDeviceSize counterBufferOffset, uint32_t counterOffset, uint32_t vertexStride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
   } else {
      vk_cmd_enqueue_CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetExclusiveScissorNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkRect2D* pExclusiveScissors)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_exclusive_scissor_nv(&cmd_buffer->cmd_queue,
                                       firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetExclusiveScissorNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkRect2D* pExclusiveScissors)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
   } else {
      vk_cmd_enqueue_CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetExclusiveScissorEnableNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkBool32* pExclusiveScissorEnables)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_exclusive_scissor_enable_nv(&cmd_buffer->cmd_queue,
                                       firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissorEnables);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetExclusiveScissorEnableNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkBool32* pExclusiveScissorEnables)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetExclusiveScissorEnableNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissorEnables);
   } else {
      vk_cmd_enqueue_CmdSetExclusiveScissorEnableNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissorEnables);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindShadingRateImageNV(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_shading_rate_image_nv(&cmd_buffer->cmd_queue,
                                       imageView, imageLayout);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindShadingRateImageNV(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
   } else {
      vk_cmd_enqueue_CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewportShadingRatePaletteNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkShadingRatePaletteNV* pShadingRatePalettes)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport_shading_rate_palette_nv(&cmd_buffer->cmd_queue,
                                       firstViewport, viewportCount, pShadingRatePalettes);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewportShadingRatePaletteNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkShadingRatePaletteNV* pShadingRatePalettes)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
   } else {
      vk_cmd_enqueue_CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoarseSampleOrderNV(VkCommandBuffer commandBuffer, VkCoarseSampleOrderTypeNV sampleOrderType, uint32_t customSampleOrderCount, const VkCoarseSampleOrderCustomNV* pCustomSampleOrders)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coarse_sample_order_nv(&cmd_buffer->cmd_queue,
                                       sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoarseSampleOrderNV(VkCommandBuffer commandBuffer, VkCoarseSampleOrderTypeNV sampleOrderType, uint32_t customSampleOrderCount, const VkCoarseSampleOrderCustomNV* pCustomSampleOrders)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
   } else {
      vk_cmd_enqueue_CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksNV(VkCommandBuffer commandBuffer, uint32_t taskCount, uint32_t firstTask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_nv(&cmd_buffer->cmd_queue,
                                       taskCount, firstTask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksNV(VkCommandBuffer commandBuffer, uint32_t taskCount, uint32_t firstTask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirectNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect_nv(&cmd_buffer->cmd_queue,
                                       buffer, offset, drawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirectNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirectCountNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect_count_nv(&cmd_buffer->cmd_queue,
                                       buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirectCountNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksEXT(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_ext(&cmd_buffer->cmd_queue,
                                       groupCountX, groupCountY, groupCountZ);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksEXT(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksEXT(commandBuffer, groupCountX, groupCountY, groupCountZ);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksEXT(commandBuffer, groupCountX, groupCountY, groupCountZ);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirectEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect_ext(&cmd_buffer->cmd_queue,
                                       buffer, offset, drawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirectEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirectEXT(commandBuffer, buffer, offset, drawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirectEXT(commandBuffer, buffer, offset, drawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirectCountEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect_count_ext(&cmd_buffer->cmd_queue,
                                       buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirectCountEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirectCountEXT(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirectCountEXT(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindInvocationMaskHUAWEI(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_invocation_mask_huawei(&cmd_buffer->cmd_queue,
                                       imageView, imageLayout);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindInvocationMaskHUAWEI(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindInvocationMaskHUAWEI(commandBuffer, imageView, imageLayout);
   } else {
      vk_cmd_enqueue_CmdBindInvocationMaskHUAWEI(commandBuffer, imageView, imageLayout);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyAccelerationStructureNV(VkCommandBuffer commandBuffer, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkCopyAccelerationStructureModeKHR mode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_acceleration_structure_nv(&cmd_buffer->cmd_queue,
                                       dst, src, mode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyAccelerationStructureNV(VkCommandBuffer commandBuffer, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkCopyAccelerationStructureModeKHR mode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
   } else {
      vk_cmd_enqueue_CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_acceleration_structure_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyAccelerationStructureToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_acceleration_structure_to_memory_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyAccelerationStructureToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryToAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_to_acceleration_structure_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryToAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteAccelerationStructuresPropertiesKHR(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_acceleration_structures_properties_khr(&cmd_buffer->cmd_queue,
                                       accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteAccelerationStructuresPropertiesKHR(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   } else {
      vk_cmd_enqueue_CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteAccelerationStructuresPropertiesNV(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureNV* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_acceleration_structures_properties_nv(&cmd_buffer->cmd_queue,
                                       accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteAccelerationStructuresPropertiesNV(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureNV* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   } else {
      vk_cmd_enqueue_CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBuildAccelerationStructureNV(VkCommandBuffer commandBuffer, const VkAccelerationStructureInfoNV* pInfo, VkBuffer instanceData, VkDeviceSize instanceOffset, VkBool32 update, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer scratch, VkDeviceSize scratchOffset)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_build_acceleration_structure_nv(&cmd_buffer->cmd_queue,
                                       pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBuildAccelerationStructureNV(VkCommandBuffer commandBuffer, const VkAccelerationStructureInfoNV* pInfo, VkBuffer instanceData, VkDeviceSize instanceOffset, VkBool32 update, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer scratch, VkDeviceSize scratchOffset)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
   } else {
      vk_cmd_enqueue_CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdTraceRaysKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint32_t width, uint32_t height, uint32_t depth)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_trace_rays_khr(&cmd_buffer->cmd_queue,
                                       pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdTraceRaysKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint32_t width, uint32_t height, uint32_t depth)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
   } else {
      vk_cmd_enqueue_CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdTraceRaysNV(VkCommandBuffer commandBuffer, VkBuffer raygenShaderBindingTableBuffer, VkDeviceSize raygenShaderBindingOffset, VkBuffer missShaderBindingTableBuffer, VkDeviceSize missShaderBindingOffset, VkDeviceSize missShaderBindingStride, VkBuffer hitShaderBindingTableBuffer, VkDeviceSize hitShaderBindingOffset, VkDeviceSize hitShaderBindingStride, VkBuffer callableShaderBindingTableBuffer, VkDeviceSize callableShaderBindingOffset, VkDeviceSize callableShaderBindingStride, uint32_t width, uint32_t height, uint32_t depth)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_trace_rays_nv(&cmd_buffer->cmd_queue,
                                       raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdTraceRaysNV(VkCommandBuffer commandBuffer, VkBuffer raygenShaderBindingTableBuffer, VkDeviceSize raygenShaderBindingOffset, VkBuffer missShaderBindingTableBuffer, VkDeviceSize missShaderBindingOffset, VkDeviceSize missShaderBindingStride, VkBuffer hitShaderBindingTableBuffer, VkDeviceSize hitShaderBindingOffset, VkDeviceSize hitShaderBindingStride, VkBuffer callableShaderBindingTableBuffer, VkDeviceSize callableShaderBindingOffset, VkDeviceSize callableShaderBindingStride, uint32_t width, uint32_t height, uint32_t depth)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
   } else {
      vk_cmd_enqueue_CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdTraceRaysIndirectKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_trace_rays_indirect_khr(&cmd_buffer->cmd_queue,
                                       pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdTraceRaysIndirectKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
   } else {
      vk_cmd_enqueue_CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdTraceRaysIndirect2KHR(VkCommandBuffer commandBuffer, VkDeviceAddress indirectDeviceAddress)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_trace_rays_indirect2_khr(&cmd_buffer->cmd_queue,
                                       indirectDeviceAddress);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdTraceRaysIndirect2KHR(VkCommandBuffer commandBuffer, VkDeviceAddress indirectDeviceAddress)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdTraceRaysIndirect2KHR(commandBuffer, indirectDeviceAddress);
   } else {
      vk_cmd_enqueue_CmdTraceRaysIndirect2KHR(commandBuffer, indirectDeviceAddress);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBuildClusterAccelerationStructureIndirectNV(VkCommandBuffer                     commandBuffer, const VkClusterAccelerationStructureCommandsInfoNV*  pCommandInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_build_cluster_acceleration_structure_indirect_nv(&cmd_buffer->cmd_queue,
                                       pCommandInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBuildClusterAccelerationStructureIndirectNV(VkCommandBuffer                     commandBuffer, const VkClusterAccelerationStructureCommandsInfoNV*  pCommandInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBuildClusterAccelerationStructureIndirectNV(commandBuffer, pCommandInfos);
   } else {
      vk_cmd_enqueue_CmdBuildClusterAccelerationStructureIndirectNV(commandBuffer, pCommandInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRayTracingPipelineStackSizeKHR(VkCommandBuffer commandBuffer, uint32_t pipelineStackSize)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_ray_tracing_pipeline_stack_size_khr(&cmd_buffer->cmd_queue,
                                       pipelineStackSize);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRayTracingPipelineStackSizeKHR(VkCommandBuffer commandBuffer, uint32_t pipelineStackSize)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
   } else {
      vk_cmd_enqueue_CmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
   }
}
/* TODO: Generate vk_cmd_enqueue_CmdSetPerformanceMarkerINTEL() */
/* TODO: Generate vk_cmd_enqueue_CmdSetPerformanceStreamMarkerINTEL() */
/* TODO: Generate vk_cmd_enqueue_CmdSetPerformanceOverrideINTEL() */



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLineStipple(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_line_stipple(&cmd_buffer->cmd_queue,
                                       lineStippleFactor, lineStipplePattern);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLineStipple(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLineStipple(commandBuffer, lineStippleFactor, lineStipplePattern);
   } else {
      vk_cmd_enqueue_CmdSetLineStipple(commandBuffer, lineStippleFactor, lineStipplePattern);
   }
}



/* vk_cmd_enqueue_CmdBuildAccelerationStructuresKHR() is hand-typed in vk_cmd_enqueue.c */

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBuildAccelerationStructuresKHR(VkCommandBuffer                                    commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
   } else {
      vk_cmd_enqueue_CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
   }
}
/* TODO: Generate vk_cmd_enqueue_CmdBuildAccelerationStructuresIndirectKHR() */



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_cull_mode(&cmd_buffer->cmd_queue,
                                       cullMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCullMode(commandBuffer, cullMode);
   } else {
      vk_cmd_enqueue_CmdSetCullMode(commandBuffer, cullMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_front_face(&cmd_buffer->cmd_queue,
                                       frontFace);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetFrontFace(commandBuffer, frontFace);
   } else {
      vk_cmd_enqueue_CmdSetFrontFace(commandBuffer, frontFace);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_primitive_topology(&cmd_buffer->cmd_queue,
                                       primitiveTopology);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
   } else {
      vk_cmd_enqueue_CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport_with_count(&cmd_buffer->cmd_queue,
                                       viewportCount, pViewports);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
   } else {
      vk_cmd_enqueue_CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_scissor_with_count(&cmd_buffer->cmd_queue,
                                       scissorCount, pScissors);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
   } else {
      vk_cmd_enqueue_CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindIndexBuffer2(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_index_buffer2(&cmd_buffer->cmd_queue,
                                       buffer, offset, size, indexType);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindIndexBuffer2(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindIndexBuffer2(commandBuffer, buffer, offset, size, indexType);
   } else {
      vk_cmd_enqueue_CmdBindIndexBuffer2(commandBuffer, buffer, offset, size, indexType);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_vertex_buffers2(&cmd_buffer->cmd_queue,
                                       firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
   } else {
      vk_cmd_enqueue_CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_test_enable(&cmd_buffer->cmd_queue,
                                       depthTestEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_write_enable(&cmd_buffer->cmd_queue,
                                       depthWriteEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_compare_op(&cmd_buffer->cmd_queue,
                                       depthCompareOp);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
   } else {
      vk_cmd_enqueue_CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_bounds_test_enable(&cmd_buffer->cmd_queue,
                                       depthBoundsTestEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_stencil_test_enable(&cmd_buffer->cmd_queue,
                                       stencilTestEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
   } else {
      vk_cmd_enqueue_CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_stencil_op(&cmd_buffer->cmd_queue,
                                       faceMask, failOp, passOp, depthFailOp, compareOp);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
   } else {
      vk_cmd_enqueue_CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetPatchControlPointsEXT(VkCommandBuffer commandBuffer, uint32_t patchControlPoints)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_patch_control_points_ext(&cmd_buffer->cmd_queue,
                                       patchControlPoints);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetPatchControlPointsEXT(VkCommandBuffer commandBuffer, uint32_t patchControlPoints)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetPatchControlPointsEXT(commandBuffer, patchControlPoints);
   } else {
      vk_cmd_enqueue_CmdSetPatchControlPointsEXT(commandBuffer, patchControlPoints);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_rasterizer_discard_enable(&cmd_buffer->cmd_queue,
                                       rasterizerDiscardEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
   } else {
      vk_cmd_enqueue_CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_bias_enable(&cmd_buffer->cmd_queue,
                                       depthBiasEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLogicOpEXT(VkCommandBuffer commandBuffer, VkLogicOp logicOp)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_logic_op_ext(&cmd_buffer->cmd_queue,
                                       logicOp);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLogicOpEXT(VkCommandBuffer commandBuffer, VkLogicOp logicOp)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLogicOpEXT(commandBuffer, logicOp);
   } else {
      vk_cmd_enqueue_CmdSetLogicOpEXT(commandBuffer, logicOp);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_primitive_restart_enable(&cmd_buffer->cmd_queue,
                                       primitiveRestartEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
   } else {
      vk_cmd_enqueue_CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetTessellationDomainOriginEXT(VkCommandBuffer commandBuffer, VkTessellationDomainOrigin domainOrigin)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_tessellation_domain_origin_ext(&cmd_buffer->cmd_queue,
                                       domainOrigin);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetTessellationDomainOriginEXT(VkCommandBuffer commandBuffer, VkTessellationDomainOrigin domainOrigin)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetTessellationDomainOriginEXT(commandBuffer, domainOrigin);
   } else {
      vk_cmd_enqueue_CmdSetTessellationDomainOriginEXT(commandBuffer, domainOrigin);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthClampEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClampEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_clamp_enable_ext(&cmd_buffer->cmd_queue,
                                       depthClampEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthClampEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClampEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthClampEnableEXT(commandBuffer, depthClampEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthClampEnableEXT(commandBuffer, depthClampEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_polygon_mode_ext(&cmd_buffer->cmd_queue,
                                       polygonMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetPolygonModeEXT(commandBuffer, polygonMode);
   } else {
      vk_cmd_enqueue_CmdSetPolygonModeEXT(commandBuffer, polygonMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits  rasterizationSamples)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_rasterization_samples_ext(&cmd_buffer->cmd_queue,
                                       rasterizationSamples);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits  rasterizationSamples)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRasterizationSamplesEXT(commandBuffer, rasterizationSamples);
   } else {
      vk_cmd_enqueue_CmdSetRasterizationSamplesEXT(commandBuffer, rasterizationSamples);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits  samples, const VkSampleMask*    pSampleMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_sample_mask_ext(&cmd_buffer->cmd_queue,
                                       samples, pSampleMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits  samples, const VkSampleMask*    pSampleMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetSampleMaskEXT(commandBuffer, samples, pSampleMask);
   } else {
      vk_cmd_enqueue_CmdSetSampleMaskEXT(commandBuffer, samples, pSampleMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToCoverageEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_alpha_to_coverage_enable_ext(&cmd_buffer->cmd_queue,
                                       alphaToCoverageEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToCoverageEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetAlphaToCoverageEnableEXT(commandBuffer, alphaToCoverageEnable);
   } else {
      vk_cmd_enqueue_CmdSetAlphaToCoverageEnableEXT(commandBuffer, alphaToCoverageEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetAlphaToOneEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToOneEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_alpha_to_one_enable_ext(&cmd_buffer->cmd_queue,
                                       alphaToOneEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetAlphaToOneEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToOneEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetAlphaToOneEnableEXT(commandBuffer, alphaToOneEnable);
   } else {
      vk_cmd_enqueue_CmdSetAlphaToOneEnableEXT(commandBuffer, alphaToOneEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLogicOpEnableEXT(VkCommandBuffer commandBuffer, VkBool32 logicOpEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_logic_op_enable_ext(&cmd_buffer->cmd_queue,
                                       logicOpEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLogicOpEnableEXT(VkCommandBuffer commandBuffer, VkBool32 logicOpEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLogicOpEnableEXT(commandBuffer, logicOpEnable);
   } else {
      vk_cmd_enqueue_CmdSetLogicOpEnableEXT(commandBuffer, logicOpEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_color_blend_enable_ext(&cmd_buffer->cmd_queue,
                                       firstAttachment, attachmentCount, pColorBlendEnables);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetColorBlendEnableEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
   } else {
      vk_cmd_enqueue_CmdSetColorBlendEnableEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_color_blend_equation_ext(&cmd_buffer->cmd_queue,
                                       firstAttachment, attachmentCount, pColorBlendEquations);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetColorBlendEquationEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
   } else {
      vk_cmd_enqueue_CmdSetColorBlendEquationEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_color_write_mask_ext(&cmd_buffer->cmd_queue,
                                       firstAttachment, attachmentCount, pColorWriteMasks);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetColorWriteMaskEXT(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
   } else {
      vk_cmd_enqueue_CmdSetColorWriteMaskEXT(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRasterizationStreamEXT(VkCommandBuffer commandBuffer, uint32_t rasterizationStream)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_rasterization_stream_ext(&cmd_buffer->cmd_queue,
                                       rasterizationStream);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRasterizationStreamEXT(VkCommandBuffer commandBuffer, uint32_t rasterizationStream)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRasterizationStreamEXT(commandBuffer, rasterizationStream);
   } else {
      vk_cmd_enqueue_CmdSetRasterizationStreamEXT(commandBuffer, rasterizationStream);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetConservativeRasterizationModeEXT(VkCommandBuffer commandBuffer, VkConservativeRasterizationModeEXT conservativeRasterizationMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_conservative_rasterization_mode_ext(&cmd_buffer->cmd_queue,
                                       conservativeRasterizationMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetConservativeRasterizationModeEXT(VkCommandBuffer commandBuffer, VkConservativeRasterizationModeEXT conservativeRasterizationMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetConservativeRasterizationModeEXT(commandBuffer, conservativeRasterizationMode);
   } else {
      vk_cmd_enqueue_CmdSetConservativeRasterizationModeEXT(commandBuffer, conservativeRasterizationMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetExtraPrimitiveOverestimationSizeEXT(VkCommandBuffer commandBuffer, float extraPrimitiveOverestimationSize)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_extra_primitive_overestimation_size_ext(&cmd_buffer->cmd_queue,
                                       extraPrimitiveOverestimationSize);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetExtraPrimitiveOverestimationSizeEXT(VkCommandBuffer commandBuffer, float extraPrimitiveOverestimationSize)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetExtraPrimitiveOverestimationSizeEXT(commandBuffer, extraPrimitiveOverestimationSize);
   } else {
      vk_cmd_enqueue_CmdSetExtraPrimitiveOverestimationSizeEXT(commandBuffer, extraPrimitiveOverestimationSize);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthClipEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClipEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_clip_enable_ext(&cmd_buffer->cmd_queue,
                                       depthClipEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthClipEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClipEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthClipEnableEXT(commandBuffer, depthClipEnable);
   } else {
      vk_cmd_enqueue_CmdSetDepthClipEnableEXT(commandBuffer, depthClipEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetSampleLocationsEnableEXT(VkCommandBuffer commandBuffer, VkBool32 sampleLocationsEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_sample_locations_enable_ext(&cmd_buffer->cmd_queue,
                                       sampleLocationsEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetSampleLocationsEnableEXT(VkCommandBuffer commandBuffer, VkBool32 sampleLocationsEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetSampleLocationsEnableEXT(commandBuffer, sampleLocationsEnable);
   } else {
      vk_cmd_enqueue_CmdSetSampleLocationsEnableEXT(commandBuffer, sampleLocationsEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetColorBlendAdvancedEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendAdvancedEXT* pColorBlendAdvanced)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_color_blend_advanced_ext(&cmd_buffer->cmd_queue,
                                       firstAttachment, attachmentCount, pColorBlendAdvanced);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetColorBlendAdvancedEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendAdvancedEXT* pColorBlendAdvanced)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetColorBlendAdvancedEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendAdvanced);
   } else {
      vk_cmd_enqueue_CmdSetColorBlendAdvancedEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendAdvanced);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetProvokingVertexModeEXT(VkCommandBuffer commandBuffer, VkProvokingVertexModeEXT provokingVertexMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_provoking_vertex_mode_ext(&cmd_buffer->cmd_queue,
                                       provokingVertexMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetProvokingVertexModeEXT(VkCommandBuffer commandBuffer, VkProvokingVertexModeEXT provokingVertexMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetProvokingVertexModeEXT(commandBuffer, provokingVertexMode);
   } else {
      vk_cmd_enqueue_CmdSetProvokingVertexModeEXT(commandBuffer, provokingVertexMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLineRasterizationModeEXT(VkCommandBuffer commandBuffer, VkLineRasterizationModeEXT lineRasterizationMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_line_rasterization_mode_ext(&cmd_buffer->cmd_queue,
                                       lineRasterizationMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLineRasterizationModeEXT(VkCommandBuffer commandBuffer, VkLineRasterizationModeEXT lineRasterizationMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLineRasterizationModeEXT(commandBuffer, lineRasterizationMode);
   } else {
      vk_cmd_enqueue_CmdSetLineRasterizationModeEXT(commandBuffer, lineRasterizationMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetLineStippleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 stippledLineEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_line_stipple_enable_ext(&cmd_buffer->cmd_queue,
                                       stippledLineEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetLineStippleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 stippledLineEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetLineStippleEnableEXT(commandBuffer, stippledLineEnable);
   } else {
      vk_cmd_enqueue_CmdSetLineStippleEnableEXT(commandBuffer, stippledLineEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer commandBuffer, VkBool32 negativeOneToOne)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_clip_negative_one_to_one_ext(&cmd_buffer->cmd_queue,
                                       negativeOneToOne);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer commandBuffer, VkBool32 negativeOneToOne)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthClipNegativeOneToOneEXT(commandBuffer, negativeOneToOne);
   } else {
      vk_cmd_enqueue_CmdSetDepthClipNegativeOneToOneEXT(commandBuffer, negativeOneToOne);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewportWScalingEnableNV(VkCommandBuffer commandBuffer, VkBool32 viewportWScalingEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport_wscaling_enable_nv(&cmd_buffer->cmd_queue,
                                       viewportWScalingEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewportWScalingEnableNV(VkCommandBuffer commandBuffer, VkBool32 viewportWScalingEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewportWScalingEnableNV(commandBuffer, viewportWScalingEnable);
   } else {
      vk_cmd_enqueue_CmdSetViewportWScalingEnableNV(commandBuffer, viewportWScalingEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetViewportSwizzleNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportSwizzleNV* pViewportSwizzles)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_viewport_swizzle_nv(&cmd_buffer->cmd_queue,
                                       firstViewport, viewportCount, pViewportSwizzles);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetViewportSwizzleNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportSwizzleNV* pViewportSwizzles)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetViewportSwizzleNV(commandBuffer, firstViewport, viewportCount, pViewportSwizzles);
   } else {
      vk_cmd_enqueue_CmdSetViewportSwizzleNV(commandBuffer, firstViewport, viewportCount, pViewportSwizzles);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageToColorEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageToColorEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_to_color_enable_nv(&cmd_buffer->cmd_queue,
                                       coverageToColorEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageToColorEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageToColorEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageToColorEnableNV(commandBuffer, coverageToColorEnable);
   } else {
      vk_cmd_enqueue_CmdSetCoverageToColorEnableNV(commandBuffer, coverageToColorEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageToColorLocationNV(VkCommandBuffer commandBuffer, uint32_t coverageToColorLocation)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_to_color_location_nv(&cmd_buffer->cmd_queue,
                                       coverageToColorLocation);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageToColorLocationNV(VkCommandBuffer commandBuffer, uint32_t coverageToColorLocation)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageToColorLocationNV(commandBuffer, coverageToColorLocation);
   } else {
      vk_cmd_enqueue_CmdSetCoverageToColorLocationNV(commandBuffer, coverageToColorLocation);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageModulationModeNV(VkCommandBuffer commandBuffer, VkCoverageModulationModeNV coverageModulationMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_modulation_mode_nv(&cmd_buffer->cmd_queue,
                                       coverageModulationMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageModulationModeNV(VkCommandBuffer commandBuffer, VkCoverageModulationModeNV coverageModulationMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageModulationModeNV(commandBuffer, coverageModulationMode);
   } else {
      vk_cmd_enqueue_CmdSetCoverageModulationModeNV(commandBuffer, coverageModulationMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageModulationTableEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageModulationTableEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_modulation_table_enable_nv(&cmd_buffer->cmd_queue,
                                       coverageModulationTableEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageModulationTableEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageModulationTableEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageModulationTableEnableNV(commandBuffer, coverageModulationTableEnable);
   } else {
      vk_cmd_enqueue_CmdSetCoverageModulationTableEnableNV(commandBuffer, coverageModulationTableEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageModulationTableNV(VkCommandBuffer commandBuffer, uint32_t coverageModulationTableCount, const float* pCoverageModulationTable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_modulation_table_nv(&cmd_buffer->cmd_queue,
                                       coverageModulationTableCount, pCoverageModulationTable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageModulationTableNV(VkCommandBuffer commandBuffer, uint32_t coverageModulationTableCount, const float* pCoverageModulationTable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageModulationTableNV(commandBuffer, coverageModulationTableCount, pCoverageModulationTable);
   } else {
      vk_cmd_enqueue_CmdSetCoverageModulationTableNV(commandBuffer, coverageModulationTableCount, pCoverageModulationTable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetShadingRateImageEnableNV(VkCommandBuffer commandBuffer, VkBool32 shadingRateImageEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_shading_rate_image_enable_nv(&cmd_buffer->cmd_queue,
                                       shadingRateImageEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetShadingRateImageEnableNV(VkCommandBuffer commandBuffer, VkBool32 shadingRateImageEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetShadingRateImageEnableNV(commandBuffer, shadingRateImageEnable);
   } else {
      vk_cmd_enqueue_CmdSetShadingRateImageEnableNV(commandBuffer, shadingRateImageEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetCoverageReductionModeNV(VkCommandBuffer commandBuffer, VkCoverageReductionModeNV coverageReductionMode)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_coverage_reduction_mode_nv(&cmd_buffer->cmd_queue,
                                       coverageReductionMode);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetCoverageReductionModeNV(VkCommandBuffer commandBuffer, VkCoverageReductionModeNV coverageReductionMode)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetCoverageReductionModeNV(commandBuffer, coverageReductionMode);
   } else {
      vk_cmd_enqueue_CmdSetCoverageReductionModeNV(commandBuffer, coverageReductionMode);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRepresentativeFragmentTestEnableNV(VkCommandBuffer commandBuffer, VkBool32 representativeFragmentTestEnable)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_representative_fragment_test_enable_nv(&cmd_buffer->cmd_queue,
                                       representativeFragmentTestEnable);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRepresentativeFragmentTestEnableNV(VkCommandBuffer commandBuffer, VkBool32 representativeFragmentTestEnable)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRepresentativeFragmentTestEnableNV(commandBuffer, representativeFragmentTestEnable);
   } else {
      vk_cmd_enqueue_CmdSetRepresentativeFragmentTestEnableNV(commandBuffer, representativeFragmentTestEnable);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_buffer2(&cmd_buffer->cmd_queue,
                                       pCopyBufferInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
   } else {
      vk_cmd_enqueue_CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_image2(&cmd_buffer->cmd_queue,
                                       pCopyImageInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyImage2(commandBuffer, pCopyImageInfo);
   } else {
      vk_cmd_enqueue_CmdCopyImage2(commandBuffer, pCopyImageInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_blit_image2(&cmd_buffer->cmd_queue,
                                       pBlitImageInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBlitImage2(commandBuffer, pBlitImageInfo);
   } else {
      vk_cmd_enqueue_CmdBlitImage2(commandBuffer, pBlitImageInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_buffer_to_image2(&cmd_buffer->cmd_queue,
                                       pCopyBufferToImageInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
   } else {
      vk_cmd_enqueue_CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyImageToBuffer2(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_image_to_buffer2(&cmd_buffer->cmd_queue,
                                       pCopyImageToBufferInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyImageToBuffer2(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
   } else {
      vk_cmd_enqueue_CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdResolveImage2(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_resolve_image2(&cmd_buffer->cmd_queue,
                                       pResolveImageInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdResolveImage2(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdResolveImage2(commandBuffer, pResolveImageInfo);
   } else {
      vk_cmd_enqueue_CmdResolveImage2(commandBuffer, pResolveImageInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetFragmentShadingRateKHR(VkCommandBuffer           commandBuffer, const VkExtent2D*                           pFragmentSize, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2])
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_fragment_shading_rate_khr(&cmd_buffer->cmd_queue,
                                       pFragmentSize, combinerOps);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetFragmentShadingRateKHR(VkCommandBuffer           commandBuffer, const VkExtent2D*                           pFragmentSize, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2])
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetFragmentShadingRateKHR(commandBuffer, pFragmentSize, combinerOps);
   } else {
      vk_cmd_enqueue_CmdSetFragmentShadingRateKHR(commandBuffer, pFragmentSize, combinerOps);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetFragmentShadingRateEnumNV(VkCommandBuffer           commandBuffer, VkFragmentShadingRateNV                     shadingRate, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2])
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_fragment_shading_rate_enum_nv(&cmd_buffer->cmd_queue,
                                       shadingRate, combinerOps);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetFragmentShadingRateEnumNV(VkCommandBuffer           commandBuffer, VkFragmentShadingRateNV                     shadingRate, const VkFragmentShadingRateCombinerOpKHR    combinerOps[2])
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, combinerOps);
   } else {
      vk_cmd_enqueue_CmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, combinerOps);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetVertexInputEXT(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_vertex_input_ext(&cmd_buffer->cmd_queue,
                                       vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetVertexInputEXT(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
   } else {
      vk_cmd_enqueue_CmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetColorWriteEnableEXT(VkCommandBuffer       commandBuffer, uint32_t                                attachmentCount, const VkBool32*   pColorWriteEnables)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_color_write_enable_ext(&cmd_buffer->cmd_queue,
                                       attachmentCount, pColorWriteEnables);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetColorWriteEnableEXT(VkCommandBuffer       commandBuffer, uint32_t                                attachmentCount, const VkBool32*   pColorWriteEnables)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetColorWriteEnableEXT(commandBuffer, attachmentCount, pColorWriteEnables);
   } else {
      vk_cmd_enqueue_CmdSetColorWriteEnableEXT(commandBuffer, attachmentCount, pColorWriteEnables);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetEvent2(VkCommandBuffer                   commandBuffer, VkEvent                                             event, const VkDependencyInfo*                             pDependencyInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_event2(&cmd_buffer->cmd_queue,
                                       event, pDependencyInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetEvent2(VkCommandBuffer                   commandBuffer, VkEvent                                             event, const VkDependencyInfo*                             pDependencyInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetEvent2(commandBuffer, event, pDependencyInfo);
   } else {
      vk_cmd_enqueue_CmdSetEvent2(commandBuffer, event, pDependencyInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdResetEvent2(VkCommandBuffer                   commandBuffer, VkEvent                                             event, VkPipelineStageFlags2               stageMask)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_reset_event2(&cmd_buffer->cmd_queue,
                                       event, stageMask);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdResetEvent2(VkCommandBuffer                   commandBuffer, VkEvent                                             event, VkPipelineStageFlags2               stageMask)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdResetEvent2(commandBuffer, event, stageMask);
   } else {
      vk_cmd_enqueue_CmdResetEvent2(commandBuffer, event, stageMask);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWaitEvents2(VkCommandBuffer                   commandBuffer, uint32_t                                            eventCount, const VkEvent*                     pEvents, const VkDependencyInfo*            pDependencyInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_wait_events2(&cmd_buffer->cmd_queue,
                                       eventCount, pEvents, pDependencyInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWaitEvents2(VkCommandBuffer                   commandBuffer, uint32_t                                            eventCount, const VkEvent*                     pEvents, const VkDependencyInfo*            pDependencyInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
   } else {
      vk_cmd_enqueue_CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPipelineBarrier2(VkCommandBuffer                   commandBuffer, const VkDependencyInfo*                             pDependencyInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_pipeline_barrier2(&cmd_buffer->cmd_queue,
                                       pDependencyInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPipelineBarrier2(VkCommandBuffer                   commandBuffer, const VkDependencyInfo*                             pDependencyInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
   } else {
      vk_cmd_enqueue_CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteTimestamp2(VkCommandBuffer                   commandBuffer, VkPipelineStageFlags2               stage, VkQueryPool                                         queryPool, uint32_t                                            query)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_timestamp2(&cmd_buffer->cmd_queue,
                                       stage, queryPool, query);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteTimestamp2(VkCommandBuffer                   commandBuffer, VkPipelineStageFlags2               stage, VkQueryPool                                         queryPool, uint32_t                                            query)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
   } else {
      vk_cmd_enqueue_CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteBufferMarker2AMD(VkCommandBuffer                   commandBuffer, VkPipelineStageFlags2               stage, VkBuffer                                            dstBuffer, VkDeviceSize                                        dstOffset, uint32_t                                            marker)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_buffer_marker2_amd(&cmd_buffer->cmd_queue,
                                       stage, dstBuffer, dstOffset, marker);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteBufferMarker2AMD(VkCommandBuffer                   commandBuffer, VkPipelineStageFlags2               stage, VkBuffer                                            dstBuffer, VkDeviceSize                                        dstOffset, uint32_t                                            marker)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteBufferMarker2AMD(commandBuffer, stage, dstBuffer, dstOffset, marker);
   } else {
      vk_cmd_enqueue_CmdWriteBufferMarker2AMD(commandBuffer, stage, dstBuffer, dstOffset, marker);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDecodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoDecodeInfoKHR* pDecodeInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_decode_video_khr(&cmd_buffer->cmd_queue,
                                       pDecodeInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDecodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoDecodeInfoKHR* pDecodeInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDecodeVideoKHR(commandBuffer, pDecodeInfo);
   } else {
      vk_cmd_enqueue_CmdDecodeVideoKHR(commandBuffer, pDecodeInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoBeginCodingInfoKHR* pBeginInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_video_coding_khr(&cmd_buffer->cmd_queue,
                                       pBeginInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoBeginCodingInfoKHR* pBeginInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginVideoCodingKHR(commandBuffer, pBeginInfo);
   } else {
      vk_cmd_enqueue_CmdBeginVideoCodingKHR(commandBuffer, pBeginInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdControlVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoCodingControlInfoKHR* pCodingControlInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_control_video_coding_khr(&cmd_buffer->cmd_queue,
                                       pCodingControlInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdControlVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoCodingControlInfoKHR* pCodingControlInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdControlVideoCodingKHR(commandBuffer, pCodingControlInfo);
   } else {
      vk_cmd_enqueue_CmdControlVideoCodingKHR(commandBuffer, pCodingControlInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoEndCodingInfoKHR* pEndCodingInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_video_coding_khr(&cmd_buffer->cmd_queue,
                                       pEndCodingInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoEndCodingInfoKHR* pEndCodingInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndVideoCodingKHR(commandBuffer, pEndCodingInfo);
   } else {
      vk_cmd_enqueue_CmdEndVideoCodingKHR(commandBuffer, pEndCodingInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEncodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoEncodeInfoKHR* pEncodeInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_encode_video_khr(&cmd_buffer->cmd_queue,
                                       pEncodeInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEncodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoEncodeInfoKHR* pEncodeInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEncodeVideoKHR(commandBuffer, pEncodeInfo);
   } else {
      vk_cmd_enqueue_CmdEncodeVideoKHR(commandBuffer, pEncodeInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDecompressMemoryNV(VkCommandBuffer commandBuffer, uint32_t decompressRegionCount, const VkDecompressMemoryRegionNV* pDecompressMemoryRegions)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_decompress_memory_nv(&cmd_buffer->cmd_queue,
                                       decompressRegionCount, pDecompressMemoryRegions);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDecompressMemoryNV(VkCommandBuffer commandBuffer, uint32_t decompressRegionCount, const VkDecompressMemoryRegionNV* pDecompressMemoryRegions)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDecompressMemoryNV(commandBuffer, decompressRegionCount, pDecompressMemoryRegions);
   } else {
      vk_cmd_enqueue_CmdDecompressMemoryNV(commandBuffer, decompressRegionCount, pDecompressMemoryRegions);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDecompressMemoryIndirectCountNV(VkCommandBuffer commandBuffer, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_decompress_memory_indirect_count_nv(&cmd_buffer->cmd_queue,
                                       indirectCommandsAddress, indirectCommandsCountAddress, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDecompressMemoryIndirectCountNV(VkCommandBuffer commandBuffer, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDecompressMemoryIndirectCountNV(commandBuffer, indirectCommandsAddress, indirectCommandsCountAddress, stride);
   } else {
      vk_cmd_enqueue_CmdDecompressMemoryIndirectCountNV(commandBuffer, indirectCommandsAddress, indirectCommandsCountAddress, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBuildPartitionedAccelerationStructuresNV(VkCommandBuffer                     commandBuffer, const VkBuildPartitionedAccelerationStructureInfoNV*  pBuildInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_build_partitioned_acceleration_structures_nv(&cmd_buffer->cmd_queue,
                                       pBuildInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBuildPartitionedAccelerationStructuresNV(VkCommandBuffer                     commandBuffer, const VkBuildPartitionedAccelerationStructureInfoNV*  pBuildInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBuildPartitionedAccelerationStructuresNV(commandBuffer, pBuildInfo);
   } else {
      vk_cmd_enqueue_CmdBuildPartitionedAccelerationStructuresNV(commandBuffer, pBuildInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDecompressMemoryEXT(VkCommandBuffer commandBuffer, const VkDecompressMemoryInfoEXT* pDecompressMemoryInfoEXT)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_decompress_memory_ext(&cmd_buffer->cmd_queue,
                                       pDecompressMemoryInfoEXT);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDecompressMemoryEXT(VkCommandBuffer commandBuffer, const VkDecompressMemoryInfoEXT* pDecompressMemoryInfoEXT)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDecompressMemoryEXT(commandBuffer, pDecompressMemoryInfoEXT);
   } else {
      vk_cmd_enqueue_CmdDecompressMemoryEXT(commandBuffer, pDecompressMemoryInfoEXT);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDecompressMemoryIndirectCountEXT(VkCommandBuffer commandBuffer, VkMemoryDecompressionMethodFlagsEXT decompressionMethod, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t maxDecompressionCount, uint32_t stride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_decompress_memory_indirect_count_ext(&cmd_buffer->cmd_queue,
                                       decompressionMethod, indirectCommandsAddress, indirectCommandsCountAddress, maxDecompressionCount, stride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDecompressMemoryIndirectCountEXT(VkCommandBuffer commandBuffer, VkMemoryDecompressionMethodFlagsEXT decompressionMethod, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t maxDecompressionCount, uint32_t stride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDecompressMemoryIndirectCountEXT(commandBuffer, decompressionMethod, indirectCommandsAddress, indirectCommandsCountAddress, maxDecompressionCount, stride);
   } else {
      vk_cmd_enqueue_CmdDecompressMemoryIndirectCountEXT(commandBuffer, decompressionMethod, indirectCommandsAddress, indirectCommandsCountAddress, maxDecompressionCount, stride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCuLaunchKernelNVX(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX* pLaunchInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_cu_launch_kernel_nvx(&cmd_buffer->cmd_queue,
                                       pLaunchInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCuLaunchKernelNVX(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX* pLaunchInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCuLaunchKernelNVX(commandBuffer, pLaunchInfo);
   } else {
      vk_cmd_enqueue_CmdCuLaunchKernelNVX(commandBuffer, pLaunchInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindDescriptorBuffersEXT(VkCommandBuffer commandBuffer, uint32_t bufferCount, const VkDescriptorBufferBindingInfoEXT* pBindingInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_descriptor_buffers_ext(&cmd_buffer->cmd_queue,
                                       bufferCount, pBindingInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindDescriptorBuffersEXT(VkCommandBuffer commandBuffer, uint32_t bufferCount, const VkDescriptorBufferBindingInfoEXT* pBindingInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindDescriptorBuffersEXT(commandBuffer, bufferCount, pBindingInfos);
   } else {
      vk_cmd_enqueue_CmdBindDescriptorBuffersEXT(commandBuffer, bufferCount, pBindingInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDescriptorBufferOffsetsEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t setCount, const uint32_t* pBufferIndices, const VkDeviceSize* pOffsets)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_descriptor_buffer_offsets_ext(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, layout, firstSet, setCount, pBufferIndices, pOffsets);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDescriptorBufferOffsetsEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t setCount, const uint32_t* pBufferIndices, const VkDeviceSize* pOffsets)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDescriptorBufferOffsetsEXT(commandBuffer, pipelineBindPoint, layout, firstSet, setCount, pBufferIndices, pOffsets);
   } else {
      vk_cmd_enqueue_CmdSetDescriptorBufferOffsetsEXT(commandBuffer, pipelineBindPoint, layout, firstSet, setCount, pBufferIndices, pOffsets);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindDescriptorBufferEmbeddedSamplersEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers_ext(&cmd_buffer->cmd_queue,
                                       pipelineBindPoint, layout, set);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindDescriptorBufferEmbeddedSamplersEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindDescriptorBufferEmbeddedSamplersEXT(commandBuffer, pipelineBindPoint, layout, set);
   } else {
      vk_cmd_enqueue_CmdBindDescriptorBufferEmbeddedSamplersEXT(commandBuffer, pipelineBindPoint, layout, set);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginRendering(VkCommandBuffer                   commandBuffer, const VkRenderingInfo*                              pRenderingInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_rendering(&cmd_buffer->cmd_queue,
                                       pRenderingInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginRendering(VkCommandBuffer                   commandBuffer, const VkRenderingInfo*                              pRenderingInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginRendering(commandBuffer, pRenderingInfo);
   } else {
      vk_cmd_enqueue_CmdBeginRendering(commandBuffer, pRenderingInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndRendering(VkCommandBuffer                   commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_rendering(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndRendering(VkCommandBuffer                   commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndRendering(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdEndRendering(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndRendering2KHR(VkCommandBuffer                   commandBuffer, const VkRenderingEndInfoKHR*        pRenderingEndInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_rendering2_khr(&cmd_buffer->cmd_queue,
                                       pRenderingEndInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndRendering2KHR(VkCommandBuffer                   commandBuffer, const VkRenderingEndInfoKHR*        pRenderingEndInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndRendering2KHR(commandBuffer, pRenderingEndInfo);
   } else {
      vk_cmd_enqueue_CmdEndRendering2KHR(commandBuffer, pRenderingEndInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBuildMicromapsEXT(VkCommandBuffer             commandBuffer, uint32_t                                      infoCount, const VkMicromapBuildInfoEXT* pInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_build_micromaps_ext(&cmd_buffer->cmd_queue,
                                       infoCount, pInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBuildMicromapsEXT(VkCommandBuffer             commandBuffer, uint32_t                                      infoCount, const VkMicromapBuildInfoEXT* pInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBuildMicromapsEXT(commandBuffer, infoCount, pInfos);
   } else {
      vk_cmd_enqueue_CmdBuildMicromapsEXT(commandBuffer, infoCount, pInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMicromapEXT(VkCommandBuffer commandBuffer, const VkCopyMicromapInfoEXT*      pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_micromap_ext(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMicromapEXT(VkCommandBuffer commandBuffer, const VkCopyMicromapInfoEXT*      pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMicromapEXT(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMicromapEXT(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMicromapToMemoryEXT(VkCommandBuffer    commandBuffer, const VkCopyMicromapToMemoryInfoEXT* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_micromap_to_memory_ext(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMicromapToMemoryEXT(VkCommandBuffer    commandBuffer, const VkCopyMicromapToMemoryInfoEXT* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMicromapToMemoryEXT(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMicromapToMemoryEXT(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryToMicromapEXT(VkCommandBuffer    commandBuffer, const VkCopyMemoryToMicromapInfoEXT* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_to_micromap_ext(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryToMicromapEXT(VkCommandBuffer    commandBuffer, const VkCopyMemoryToMicromapInfoEXT* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryToMicromapEXT(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryToMicromapEXT(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteMicromapsPropertiesEXT(VkCommandBuffer commandBuffer, uint32_t                                 micromapCount, const VkMicromapEXT* pMicromaps, VkQueryType        queryType, VkQueryPool                              queryPool, uint32_t                                 firstQuery)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_micromaps_properties_ext(&cmd_buffer->cmd_queue,
                                       micromapCount, pMicromaps, queryType, queryPool, firstQuery);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteMicromapsPropertiesEXT(VkCommandBuffer commandBuffer, uint32_t                                 micromapCount, const VkMicromapEXT* pMicromaps, VkQueryType        queryType, VkQueryPool                              queryPool, uint32_t                                 firstQuery)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteMicromapsPropertiesEXT(commandBuffer, micromapCount, pMicromaps, queryType, queryPool, firstQuery);
   } else {
      vk_cmd_enqueue_CmdWriteMicromapsPropertiesEXT(commandBuffer, micromapCount, pMicromaps, queryType, queryPool, firstQuery);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindTileMemoryQCOM(VkCommandBuffer commandBuffer, const VkTileMemoryBindInfoQCOM* pTileMemoryBindInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_tile_memory_qcom(&cmd_buffer->cmd_queue,
                                       pTileMemoryBindInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindTileMemoryQCOM(VkCommandBuffer commandBuffer, const VkTileMemoryBindInfoQCOM* pTileMemoryBindInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindTileMemoryQCOM(commandBuffer, pTileMemoryBindInfo);
   } else {
      vk_cmd_enqueue_CmdBindTileMemoryQCOM(commandBuffer, pTileMemoryBindInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdOpticalFlowExecuteNV(VkCommandBuffer commandBuffer, VkOpticalFlowSessionNV session, const VkOpticalFlowExecuteInfoNV* pExecuteInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_optical_flow_execute_nv(&cmd_buffer->cmd_queue,
                                       session, pExecuteInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdOpticalFlowExecuteNV(VkCommandBuffer commandBuffer, VkOpticalFlowSessionNV session, const VkOpticalFlowExecuteInfoNV* pExecuteInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdOpticalFlowExecuteNV(commandBuffer, session, pExecuteInfo);
   } else {
      vk_cmd_enqueue_CmdOpticalFlowExecuteNV(commandBuffer, session, pExecuteInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthBias2EXT(VkCommandBuffer commandBuffer, const VkDepthBiasInfoEXT*         pDepthBiasInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_bias2_ext(&cmd_buffer->cmd_queue,
                                       pDepthBiasInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthBias2EXT(VkCommandBuffer commandBuffer, const VkDepthBiasInfoEXT*         pDepthBiasInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthBias2EXT(commandBuffer, pDepthBiasInfo);
   } else {
      vk_cmd_enqueue_CmdSetDepthBias2EXT(commandBuffer, pDepthBiasInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_shaders_ext(&cmd_buffer->cmd_queue,
                                       stageCount, pStages, pShaders);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindShadersEXT(commandBuffer, stageCount, pStages, pShaders);
   } else {
      vk_cmd_enqueue_CmdBindShadersEXT(commandBuffer, stageCount, pStages, pShaders);
   }
}
/* TODO: Generate vk_cmd_enqueue_CmdBeginGpaSessionAMD() */
/* TODO: Generate vk_cmd_enqueue_CmdEndGpaSessionAMD() */
/* TODO: Generate vk_cmd_enqueue_CmdBeginGpaSampleAMD() */



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndGpaSampleAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD                   gpaSession, uint32_t                          sampleID)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_gpa_sample_amd(&cmd_buffer->cmd_queue,
                                       gpaSession, sampleID);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndGpaSampleAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD                   gpaSession, uint32_t                          sampleID)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndGpaSampleAMD(commandBuffer, gpaSession, sampleID);
   } else {
      vk_cmd_enqueue_CmdEndGpaSampleAMD(commandBuffer, gpaSession, sampleID);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyGpaSessionResultsAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD                   gpaSession)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_gpa_session_results_amd(&cmd_buffer->cmd_queue,
                                       gpaSession);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyGpaSessionResultsAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD                   gpaSession)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyGpaSessionResultsAMD(commandBuffer, gpaSession);
   } else {
      vk_cmd_enqueue_CmdCopyGpaSessionResultsAMD(commandBuffer, gpaSession);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindDescriptorSets2(VkCommandBuffer commandBuffer, const VkBindDescriptorSetsInfo*   pBindDescriptorSetsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_descriptor_sets2(&cmd_buffer->cmd_queue,
                                       pBindDescriptorSetsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindDescriptorSets2(VkCommandBuffer commandBuffer, const VkBindDescriptorSetsInfo*   pBindDescriptorSetsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindDescriptorSets2(commandBuffer, pBindDescriptorSetsInfo);
   } else {
      vk_cmd_enqueue_CmdBindDescriptorSets2(commandBuffer, pBindDescriptorSetsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushConstants2(VkCommandBuffer commandBuffer, const VkPushConstantsInfo*        pPushConstantsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_constants2(&cmd_buffer->cmd_queue,
                                       pPushConstantsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushConstants2(VkCommandBuffer commandBuffer, const VkPushConstantsInfo*        pPushConstantsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushConstants2(commandBuffer, pPushConstantsInfo);
   } else {
      vk_cmd_enqueue_CmdPushConstants2(commandBuffer, pPushConstantsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushDescriptorSet2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetInfo*    pPushDescriptorSetInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_descriptor_set2(&cmd_buffer->cmd_queue,
                                       pPushDescriptorSetInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushDescriptorSet2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetInfo*    pPushDescriptorSetInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushDescriptorSet2(commandBuffer, pPushDescriptorSetInfo);
   } else {
      vk_cmd_enqueue_CmdPushDescriptorSet2(commandBuffer, pPushDescriptorSetInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushDescriptorSetWithTemplate2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_descriptor_set_with_template2(&cmd_buffer->cmd_queue,
                                       pPushDescriptorSetWithTemplateInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushDescriptorSetWithTemplate2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushDescriptorSetWithTemplate2(commandBuffer, pPushDescriptorSetWithTemplateInfo);
   } else {
      vk_cmd_enqueue_CmdPushDescriptorSetWithTemplate2(commandBuffer, pPushDescriptorSetWithTemplateInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDescriptorBufferOffsets2EXT(VkCommandBuffer commandBuffer, const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_descriptor_buffer_offsets2_ext(&cmd_buffer->cmd_queue,
                                       pSetDescriptorBufferOffsetsInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDescriptorBufferOffsets2EXT(VkCommandBuffer commandBuffer, const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDescriptorBufferOffsets2EXT(commandBuffer, pSetDescriptorBufferOffsetsInfo);
   } else {
      vk_cmd_enqueue_CmdSetDescriptorBufferOffsets2EXT(commandBuffer, pSetDescriptorBufferOffsetsInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindDescriptorBufferEmbeddedSamplers2EXT(VkCommandBuffer commandBuffer, const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers2_ext(&cmd_buffer->cmd_queue,
                                       pBindDescriptorBufferEmbeddedSamplersInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindDescriptorBufferEmbeddedSamplers2EXT(VkCommandBuffer commandBuffer, const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindDescriptorBufferEmbeddedSamplers2EXT(commandBuffer, pBindDescriptorBufferEmbeddedSamplersInfo);
   } else {
      vk_cmd_enqueue_CmdBindDescriptorBufferEmbeddedSamplers2EXT(commandBuffer, pBindDescriptorBufferEmbeddedSamplersInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRenderingAttachmentLocations(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_rendering_attachment_locations(&cmd_buffer->cmd_queue,
                                       pLocationInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRenderingAttachmentLocations(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRenderingAttachmentLocations(commandBuffer, pLocationInfo);
   } else {
      vk_cmd_enqueue_CmdSetRenderingAttachmentLocations(commandBuffer, pLocationInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetRenderingInputAttachmentIndices(VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_rendering_input_attachment_indices(&cmd_buffer->cmd_queue,
                                       pInputAttachmentIndexInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetRenderingInputAttachmentIndices(VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetRenderingInputAttachmentIndices(commandBuffer, pInputAttachmentIndexInfo);
   } else {
      vk_cmd_enqueue_CmdSetRenderingInputAttachmentIndices(commandBuffer, pInputAttachmentIndexInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDepthClampRangeEXT(VkCommandBuffer commandBuffer, VkDepthClampModeEXT depthClampMode, const VkDepthClampRangeEXT* pDepthClampRange)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_depth_clamp_range_ext(&cmd_buffer->cmd_queue,
                                       depthClampMode, pDepthClampRange);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDepthClampRangeEXT(VkCommandBuffer commandBuffer, VkDepthClampModeEXT depthClampMode, const VkDepthClampRangeEXT* pDepthClampRange)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDepthClampRangeEXT(commandBuffer, depthClampMode, pDepthClampRange);
   } else {
      vk_cmd_enqueue_CmdSetDepthClampRangeEXT(commandBuffer, depthClampMode, pDepthClampRange);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdConvertCooperativeVectorMatrixNV(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkConvertCooperativeVectorMatrixInfoNV* pInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_convert_cooperative_vector_matrix_nv(&cmd_buffer->cmd_queue,
                                       infoCount, pInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdConvertCooperativeVectorMatrixNV(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkConvertCooperativeVectorMatrixInfoNV* pInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdConvertCooperativeVectorMatrixNV(commandBuffer, infoCount, pInfos);
   } else {
      vk_cmd_enqueue_CmdConvertCooperativeVectorMatrixNV(commandBuffer, infoCount, pInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatchTileQCOM(VkCommandBuffer commandBuffer, const VkDispatchTileInfoQCOM* pDispatchTileInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch_tile_qcom(&cmd_buffer->cmd_queue,
                                       pDispatchTileInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatchTileQCOM(VkCommandBuffer commandBuffer, const VkDispatchTileInfoQCOM* pDispatchTileInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatchTileQCOM(commandBuffer, pDispatchTileInfo);
   } else {
      vk_cmd_enqueue_CmdDispatchTileQCOM(commandBuffer, pDispatchTileInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileBeginInfoQCOM* pPerTileBeginInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_per_tile_execution_qcom(&cmd_buffer->cmd_queue,
                                       pPerTileBeginInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileBeginInfoQCOM* pPerTileBeginInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginPerTileExecutionQCOM(commandBuffer, pPerTileBeginInfo);
   } else {
      vk_cmd_enqueue_CmdBeginPerTileExecutionQCOM(commandBuffer, pPerTileBeginInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileEndInfoQCOM* pPerTileEndInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_per_tile_execution_qcom(&cmd_buffer->cmd_queue,
                                       pPerTileEndInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileEndInfoQCOM* pPerTileEndInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndPerTileExecutionQCOM(commandBuffer, pPerTileEndInfo);
   } else {
      vk_cmd_enqueue_CmdEndPerTileExecutionQCOM(commandBuffer, pPerTileEndInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginShaderInstrumentationARM(VkCommandBuffer commandBuffer, VkShaderInstrumentationARM instrumentation)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_shader_instrumentation_arm(&cmd_buffer->cmd_queue,
                                       instrumentation);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginShaderInstrumentationARM(VkCommandBuffer commandBuffer, VkShaderInstrumentationARM instrumentation)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginShaderInstrumentationARM(commandBuffer, instrumentation);
   } else {
      vk_cmd_enqueue_CmdBeginShaderInstrumentationARM(commandBuffer, instrumentation);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndShaderInstrumentationARM(VkCommandBuffer commandBuffer)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_shader_instrumentation_arm(&cmd_buffer->cmd_queue);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndShaderInstrumentationARM(VkCommandBuffer commandBuffer)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndShaderInstrumentationARM(commandBuffer);
   } else {
      vk_cmd_enqueue_CmdEndShaderInstrumentationARM(commandBuffer);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyTensorARM(VkCommandBuffer commandBuffer, const VkCopyTensorInfoARM* pCopyTensorInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_tensor_arm(&cmd_buffer->cmd_queue,
                                       pCopyTensorInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyTensorARM(VkCommandBuffer commandBuffer, const VkCopyTensorInfoARM* pCopyTensorInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyTensorARM(commandBuffer, pCopyTensorInfo);
   } else {
      vk_cmd_enqueue_CmdCopyTensorARM(commandBuffer, pCopyTensorInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatchDataGraphARM(VkCommandBuffer commandBuffer, VkDataGraphPipelineSessionARM session, const VkDataGraphPipelineDispatchInfoARM* pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch_data_graph_arm(&cmd_buffer->cmd_queue,
                                       session, pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatchDataGraphARM(VkCommandBuffer commandBuffer, VkDataGraphPipelineSessionARM session, const VkDataGraphPipelineDispatchInfoARM* pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatchDataGraphARM(commandBuffer, session, pInfo);
   } else {
      vk_cmd_enqueue_CmdDispatchDataGraphARM(commandBuffer, session, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetComputeOccupancyPriorityNV(VkCommandBuffer commandBuffer, const VkComputeOccupancyPriorityParametersNV* pParameters)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_compute_occupancy_priority_nv(&cmd_buffer->cmd_queue,
                                       pParameters);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetComputeOccupancyPriorityNV(VkCommandBuffer commandBuffer, const VkComputeOccupancyPriorityParametersNV* pParameters)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetComputeOccupancyPriorityNV(commandBuffer, pParameters);
   } else {
      vk_cmd_enqueue_CmdSetComputeOccupancyPriorityNV(commandBuffer, pParameters);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindSamplerHeapEXT(VkCommandBuffer                   commandBuffer, const VkBindHeapInfoEXT*                            pBindInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_sampler_heap_ext(&cmd_buffer->cmd_queue,
                                       pBindInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindSamplerHeapEXT(VkCommandBuffer                   commandBuffer, const VkBindHeapInfoEXT*                            pBindInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindSamplerHeapEXT(commandBuffer, pBindInfo);
   } else {
      vk_cmd_enqueue_CmdBindSamplerHeapEXT(commandBuffer, pBindInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindResourceHeapEXT(VkCommandBuffer                   commandBuffer, const VkBindHeapInfoEXT*                            pBindInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_resource_heap_ext(&cmd_buffer->cmd_queue,
                                       pBindInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindResourceHeapEXT(VkCommandBuffer                   commandBuffer, const VkBindHeapInfoEXT*                            pBindInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindResourceHeapEXT(commandBuffer, pBindInfo);
   } else {
      vk_cmd_enqueue_CmdBindResourceHeapEXT(commandBuffer, pBindInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdPushDataEXT(VkCommandBuffer                   commandBuffer, const VkPushDataInfoEXT*                            pPushDataInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_push_data_ext(&cmd_buffer->cmd_queue,
                                       pPushDataInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdPushDataEXT(VkCommandBuffer                   commandBuffer, const VkPushDataInfoEXT*                            pPushDataInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdPushDataEXT(commandBuffer, pPushDataInfo);
   } else {
      vk_cmd_enqueue_CmdPushDataEXT(commandBuffer, pPushDataInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_khr(&cmd_buffer->cmd_queue,
                                       pCopyMemoryInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryKHR(commandBuffer, pCopyMemoryInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryKHR(commandBuffer, pCopyMemoryInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyMemoryToImageKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_memory_to_image_khr(&cmd_buffer->cmd_queue,
                                       pCopyMemoryInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyMemoryToImageKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyMemoryToImageKHR(commandBuffer, pCopyMemoryInfo);
   } else {
      vk_cmd_enqueue_CmdCopyMemoryToImageKHR(commandBuffer, pCopyMemoryInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyImageToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_image_to_memory_khr(&cmd_buffer->cmd_queue,
                                       pCopyMemoryInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyImageToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyImageToMemoryKHR(commandBuffer, pCopyMemoryInfo);
   } else {
      vk_cmd_enqueue_CmdCopyImageToMemoryKHR(commandBuffer, pCopyMemoryInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdUpdateMemoryKHR(VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, VkDeviceSize                        dataSize, const void*          pData)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_update_memory_khr(&cmd_buffer->cmd_queue,
                                       pDstRange, dstFlags, dataSize, pData);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdUpdateMemoryKHR(VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, VkDeviceSize                        dataSize, const void*          pData)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdUpdateMemoryKHR(commandBuffer, pDstRange, dstFlags, dataSize, pData);
   } else {
      vk_cmd_enqueue_CmdUpdateMemoryKHR(commandBuffer, pDstRange, dstFlags, dataSize, pData);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdFillMemoryKHR(VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, uint32_t                            data)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_fill_memory_khr(&cmd_buffer->cmd_queue,
                                       pDstRange, dstFlags, data);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdFillMemoryKHR(VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, uint32_t                            data)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdFillMemoryKHR(commandBuffer, pDstRange, dstFlags, data);
   } else {
      vk_cmd_enqueue_CmdFillMemoryKHR(commandBuffer, pDstRange, dstFlags, data);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdCopyQueryPoolResultsToMemoryKHR(VkCommandBuffer   commandBuffer, VkQueryPool                         queryPool, uint32_t                            firstQuery, uint32_t                            queryCount, const VkStridedDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR    dstFlags, VkQueryResultFlags  queryResultFlags)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_copy_query_pool_results_to_memory_khr(&cmd_buffer->cmd_queue,
                                       queryPool, firstQuery, queryCount, pDstRange, dstFlags, queryResultFlags);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdCopyQueryPoolResultsToMemoryKHR(VkCommandBuffer   commandBuffer, VkQueryPool                         queryPool, uint32_t                            firstQuery, uint32_t                            queryCount, const VkStridedDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR    dstFlags, VkQueryResultFlags  queryResultFlags)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdCopyQueryPoolResultsToMemoryKHR(commandBuffer, queryPool, firstQuery, queryCount, pDstRange, dstFlags, queryResultFlags);
   } else {
      vk_cmd_enqueue_CmdCopyQueryPoolResultsToMemoryKHR(commandBuffer, queryPool, firstQuery, queryCount, pDstRange, dstFlags, queryResultFlags);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginConditionalRendering2EXT(VkCommandBuffer   commandBuffer, const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_conditional_rendering2_ext(&cmd_buffer->cmd_queue,
                                       pConditionalRenderingBegin);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginConditionalRendering2EXT(VkCommandBuffer   commandBuffer, const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginConditionalRendering2EXT(commandBuffer, pConditionalRenderingBegin);
   } else {
      vk_cmd_enqueue_CmdBeginConditionalRendering2EXT(commandBuffer, pConditionalRenderingBegin);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindTransformFeedbackBuffers2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstBinding, uint32_t                            bindingCount, const VkBindTransformFeedbackBuffer2InfoEXT* pBindingInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_transform_feedback_buffers2_ext(&cmd_buffer->cmd_queue,
                                       firstBinding, bindingCount, pBindingInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindTransformFeedbackBuffers2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstBinding, uint32_t                            bindingCount, const VkBindTransformFeedbackBuffer2InfoEXT* pBindingInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindTransformFeedbackBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBindingInfos);
   } else {
      vk_cmd_enqueue_CmdBindTransformFeedbackBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBindingInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBeginTransformFeedback2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstCounterRange, uint32_t            counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_begin_transform_feedback2_ext(&cmd_buffer->cmd_queue,
                                       firstCounterRange, counterRangeCount, pCounterInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBeginTransformFeedback2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstCounterRange, uint32_t            counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBeginTransformFeedback2EXT(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
   } else {
      vk_cmd_enqueue_CmdBeginTransformFeedback2EXT(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdEndTransformFeedback2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstCounterRange, uint32_t            counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_end_transform_feedback2_ext(&cmd_buffer->cmd_queue,
                                       firstCounterRange, counterRangeCount, pCounterInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdEndTransformFeedback2EXT(VkCommandBuffer   commandBuffer, uint32_t                            firstCounterRange, uint32_t            counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdEndTransformFeedback2EXT(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
   } else {
      vk_cmd_enqueue_CmdEndTransformFeedback2EXT(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirectByteCount2EXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo, uint32_t counterOffset, uint32_t vertexStride)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect_byte_count2_ext(&cmd_buffer->cmd_queue,
                                       instanceCount, firstInstance, pCounterInfo, counterOffset, vertexStride);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirectByteCount2EXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo, uint32_t counterOffset, uint32_t vertexStride)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirectByteCount2EXT(commandBuffer, instanceCount, firstInstance, pCounterInfo, counterOffset, vertexStride);
   } else {
      vk_cmd_enqueue_CmdDrawIndirectByteCount2EXT(commandBuffer, instanceCount, firstInstance, pCounterInfo, counterOffset, vertexStride);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdWriteMarkerToMemoryAMD(VkCommandBuffer   commandBuffer, const VkMemoryMarkerInfoAMD*         pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_write_marker_to_memory_amd(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdWriteMarkerToMemoryAMD(VkCommandBuffer   commandBuffer, const VkMemoryMarkerInfoAMD*         pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdWriteMarkerToMemoryAMD(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdWriteMarkerToMemoryAMD(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindIndexBuffer3KHR(VkCommandBuffer   commandBuffer, const VkBindIndexBuffer3InfoKHR*    pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_index_buffer3_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindIndexBuffer3KHR(VkCommandBuffer   commandBuffer, const VkBindIndexBuffer3InfoKHR*    pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindIndexBuffer3KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdBindIndexBuffer3KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdBindVertexBuffers3KHR(VkCommandBuffer commandBuffer, uint32_t                            firstBinding, uint32_t                            bindingCount, const VkBindVertexBuffer3InfoKHR* pBindingInfos)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_bind_vertex_buffers3_khr(&cmd_buffer->cmd_queue,
                                       firstBinding, bindingCount, pBindingInfos);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdBindVertexBuffers3KHR(VkCommandBuffer commandBuffer, uint32_t                            firstBinding, uint32_t                            bindingCount, const VkBindVertexBuffer3InfoKHR* pBindingInfos)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdBindVertexBuffers3KHR(commandBuffer, firstBinding, bindingCount, pBindingInfos);
   } else {
      vk_cmd_enqueue_CmdBindVertexBuffers3KHR(commandBuffer, firstBinding, bindingCount, pBindingInfos);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect2_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirect2KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawIndirect2KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndexedIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indexed_indirect2_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndexedIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndexedIndirect2KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawIndexedIndirect2KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndirectCount2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indirect_count2_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndirectCount2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndirectCount2KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawIndirectCount2KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawIndexedIndirectCount2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_indexed_indirect_count2_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawIndexedIndirectCount2KHR(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawIndexedIndirectCount2KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawIndexedIndirectCount2KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirect2EXT(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect2_ext(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirect2EXT(VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirect2EXT(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirect2EXT(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDrawMeshTasksIndirectCount2EXT(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_draw_mesh_tasks_indirect_count2_ext(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDrawMeshTasksIndirectCount2EXT(VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDrawMeshTasksIndirectCount2EXT(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDrawMeshTasksIndirectCount2EXT(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdDispatchIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDispatchIndirect2InfoKHR*   pInfo)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_dispatch_indirect2_khr(&cmd_buffer->cmd_queue,
                                       pInfo);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdDispatchIndirect2KHR(VkCommandBuffer   commandBuffer, const VkDispatchIndirect2InfoKHR*   pInfo)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdDispatchIndirect2KHR(commandBuffer, pInfo);
   } else {
      vk_cmd_enqueue_CmdDispatchIndirect2KHR(commandBuffer, pInfo);
   }
}



VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_CmdSetDispatchParametersARM(VkCommandBuffer commandBuffer, const VkDispatchParametersARM*    pDispatchParameters)
{
   VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (vk_command_buffer_has_error(cmd_buffer))
      return;
   struct vk_cmd_queue_entry *cmd = vk_enqueue_cmd_set_dispatch_parameters_arm(&cmd_buffer->cmd_queue,
                                       pDispatchParameters);
   if (unlikely(!cmd))
      vk_command_buffer_set_error(cmd_buffer, VK_ERROR_OUT_OF_HOST_MEMORY);
}

VKAPI_ATTR void VKAPI_CALL
vk_cmd_enqueue_unless_primary_CmdSetDispatchParametersARM(VkCommandBuffer commandBuffer, const VkDispatchParametersARM*    pDispatchParameters)
{
    VK_FROM_HANDLE(vk_command_buffer, cmd_buffer, commandBuffer);

   if (cmd_buffer->level == VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
      const struct vk_device_dispatch_table *disp =
         cmd_buffer->base.device->command_dispatch_table;

      disp->CmdSetDispatchParametersARM(commandBuffer, pDispatchParameters);
   } else {
      vk_cmd_enqueue_CmdSetDispatchParametersARM(commandBuffer, pDispatchParameters);
   }
}
