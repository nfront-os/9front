
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

#pragma once

#include "util/list.h"
#include "util/ralloc.h"
#include "util/u_dynarray.h"

#define VK_PROTOTYPES
#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif

#include "vk_internal_exts.h"

#ifdef __cplusplus
extern "C" {
#endif

struct vk_device_dispatch_table;

struct vk_cmd_queue {
   linear_ctx *ctx;
   struct list_head cmds;
   struct util_dynarray pipeline_layouts;
   struct util_dynarray update_templates;
   struct util_dynarray set_layouts;
};

enum vk_cmd_type {
   VK_CMD_BIND_PIPELINE,
   VK_CMD_SET_PRIMITIVE_RESTART_INDEX_EXT,
   VK_CMD_SET_ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT,
   VK_CMD_SET_VIEWPORT,
   VK_CMD_SET_SCISSOR,
   VK_CMD_SET_LINE_WIDTH,
   VK_CMD_SET_DEPTH_BIAS,
   VK_CMD_SET_BLEND_CONSTANTS,
   VK_CMD_SET_DEPTH_BOUNDS,
   VK_CMD_SET_STENCIL_COMPARE_MASK,
   VK_CMD_SET_STENCIL_WRITE_MASK,
   VK_CMD_SET_STENCIL_REFERENCE,
   VK_CMD_BIND_DESCRIPTOR_SETS,
   VK_CMD_BIND_INDEX_BUFFER,
   VK_CMD_BIND_VERTEX_BUFFERS,
   VK_CMD_DRAW,
   VK_CMD_DRAW_INDEXED,
   VK_CMD_DRAW_MULTI_EXT,
   VK_CMD_DRAW_MULTI_INDEXED_EXT,
   VK_CMD_DRAW_INDIRECT,
   VK_CMD_DRAW_INDEXED_INDIRECT,
   VK_CMD_DISPATCH,
   VK_CMD_DISPATCH_INDIRECT,
   VK_CMD_SUBPASS_SHADING_HUAWEI,
   VK_CMD_DRAW_CLUSTER_HUAWEI,
   VK_CMD_DRAW_CLUSTER_INDIRECT_HUAWEI,
   VK_CMD_UPDATE_PIPELINE_INDIRECT_BUFFER_NV,
   VK_CMD_COPY_BUFFER,
   VK_CMD_COPY_IMAGE,
   VK_CMD_BLIT_IMAGE,
   VK_CMD_COPY_BUFFER_TO_IMAGE,
   VK_CMD_COPY_IMAGE_TO_BUFFER,
   VK_CMD_COPY_MEMORY_INDIRECT_NV,
   VK_CMD_COPY_MEMORY_INDIRECT_KHR,
   VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_NV,
   VK_CMD_COPY_MEMORY_TO_IMAGE_INDIRECT_KHR,
   VK_CMD_UPDATE_BUFFER,
   VK_CMD_FILL_BUFFER,
   VK_CMD_CLEAR_COLOR_IMAGE,
   VK_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
   VK_CMD_CLEAR_ATTACHMENTS,
   VK_CMD_RESOLVE_IMAGE,
   VK_CMD_SET_EVENT,
   VK_CMD_RESET_EVENT,
   VK_CMD_WAIT_EVENTS,
   VK_CMD_PIPELINE_BARRIER,
   VK_CMD_BEGIN_QUERY,
   VK_CMD_END_QUERY,
   VK_CMD_BEGIN_CONDITIONAL_RENDERING_EXT,
   VK_CMD_END_CONDITIONAL_RENDERING_EXT,
   VK_CMD_BEGIN_CUSTOM_RESOLVE_EXT,
   VK_CMD_RESET_QUERY_POOL,
   VK_CMD_WRITE_TIMESTAMP,
   VK_CMD_COPY_QUERY_POOL_RESULTS,
   VK_CMD_PUSH_CONSTANTS,
   VK_CMD_BEGIN_RENDER_PASS,
   VK_CMD_NEXT_SUBPASS,
   VK_CMD_END_RENDER_PASS,
   VK_CMD_EXECUTE_COMMANDS,
   VK_CMD_DEBUG_MARKER_BEGIN_EXT,
   VK_CMD_DEBUG_MARKER_END_EXT,
   VK_CMD_DEBUG_MARKER_INSERT_EXT,
   VK_CMD_EXECUTE_GENERATED_COMMANDS_NV,
   VK_CMD_PREPROCESS_GENERATED_COMMANDS_NV,
   VK_CMD_BIND_PIPELINE_SHADER_GROUP_NV,
   VK_CMD_EXECUTE_GENERATED_COMMANDS_EXT,
   VK_CMD_PREPROCESS_GENERATED_COMMANDS_EXT,
   VK_CMD_PUSH_DESCRIPTOR_SET,
   VK_CMD_SET_DEVICE_MASK,
   VK_CMD_DISPATCH_BASE,
   VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE,
   VK_CMD_SET_VIEWPORT_WSCALING_NV,
   VK_CMD_SET_DISCARD_RECTANGLE_EXT,
   VK_CMD_SET_DISCARD_RECTANGLE_ENABLE_EXT,
   VK_CMD_SET_DISCARD_RECTANGLE_MODE_EXT,
   VK_CMD_SET_SAMPLE_LOCATIONS_EXT,
   VK_CMD_BEGIN_DEBUG_UTILS_LABEL_EXT,
   VK_CMD_END_DEBUG_UTILS_LABEL_EXT,
   VK_CMD_INSERT_DEBUG_UTILS_LABEL_EXT,
   VK_CMD_WRITE_BUFFER_MARKER_AMD,
   VK_CMD_BEGIN_RENDER_PASS2,
   VK_CMD_NEXT_SUBPASS2,
   VK_CMD_END_RENDER_PASS2,
   VK_CMD_DRAW_INDIRECT_COUNT,
   VK_CMD_DRAW_INDEXED_INDIRECT_COUNT,
   VK_CMD_SET_CHECKPOINT_NV,
   VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS_EXT,
   VK_CMD_BEGIN_TRANSFORM_FEEDBACK_EXT,
   VK_CMD_END_TRANSFORM_FEEDBACK_EXT,
   VK_CMD_BEGIN_QUERY_INDEXED_EXT,
   VK_CMD_END_QUERY_INDEXED_EXT,
   VK_CMD_DRAW_INDIRECT_BYTE_COUNT_EXT,
   VK_CMD_SET_EXCLUSIVE_SCISSOR_NV,
   VK_CMD_SET_EXCLUSIVE_SCISSOR_ENABLE_NV,
   VK_CMD_BIND_SHADING_RATE_IMAGE_NV,
   VK_CMD_SET_VIEWPORT_SHADING_RATE_PALETTE_NV,
   VK_CMD_SET_COARSE_SAMPLE_ORDER_NV,
   VK_CMD_DRAW_MESH_TASKS_NV,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT_NV,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_NV,
   VK_CMD_DRAW_MESH_TASKS_EXT,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT_EXT,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT_EXT,
   VK_CMD_BIND_INVOCATION_MASK_HUAWEI,
   VK_CMD_COPY_ACCELERATION_STRUCTURE_NV,
   VK_CMD_COPY_ACCELERATION_STRUCTURE_KHR,
   VK_CMD_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_KHR,
   VK_CMD_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_KHR,
   VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_KHR,
   VK_CMD_WRITE_ACCELERATION_STRUCTURES_PROPERTIES_NV,
   VK_CMD_BUILD_ACCELERATION_STRUCTURE_NV,
   VK_CMD_TRACE_RAYS_KHR,
   VK_CMD_TRACE_RAYS_NV,
   VK_CMD_TRACE_RAYS_INDIRECT_KHR,
   VK_CMD_TRACE_RAYS_INDIRECT2_KHR,
   VK_CMD_BUILD_CLUSTER_ACCELERATION_STRUCTURE_INDIRECT_NV,
   VK_CMD_SET_RAY_TRACING_PIPELINE_STACK_SIZE_KHR,
   VK_CMD_SET_PERFORMANCE_MARKER_INTEL,
   VK_CMD_SET_PERFORMANCE_STREAM_MARKER_INTEL,
   VK_CMD_SET_PERFORMANCE_OVERRIDE_INTEL,
   VK_CMD_SET_LINE_STIPPLE,
   VK_CMD_BUILD_ACCELERATION_STRUCTURES_KHR,
   VK_CMD_BUILD_ACCELERATION_STRUCTURES_INDIRECT_KHR,
   VK_CMD_SET_CULL_MODE,
   VK_CMD_SET_FRONT_FACE,
   VK_CMD_SET_PRIMITIVE_TOPOLOGY,
   VK_CMD_SET_VIEWPORT_WITH_COUNT,
   VK_CMD_SET_SCISSOR_WITH_COUNT,
   VK_CMD_BIND_INDEX_BUFFER2,
   VK_CMD_BIND_VERTEX_BUFFERS2,
   VK_CMD_SET_DEPTH_TEST_ENABLE,
   VK_CMD_SET_DEPTH_WRITE_ENABLE,
   VK_CMD_SET_DEPTH_COMPARE_OP,
   VK_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE,
   VK_CMD_SET_STENCIL_TEST_ENABLE,
   VK_CMD_SET_STENCIL_OP,
   VK_CMD_SET_PATCH_CONTROL_POINTS_EXT,
   VK_CMD_SET_RASTERIZER_DISCARD_ENABLE,
   VK_CMD_SET_DEPTH_BIAS_ENABLE,
   VK_CMD_SET_LOGIC_OP_EXT,
   VK_CMD_SET_PRIMITIVE_RESTART_ENABLE,
   VK_CMD_SET_TESSELLATION_DOMAIN_ORIGIN_EXT,
   VK_CMD_SET_DEPTH_CLAMP_ENABLE_EXT,
   VK_CMD_SET_POLYGON_MODE_EXT,
   VK_CMD_SET_RASTERIZATION_SAMPLES_EXT,
   VK_CMD_SET_SAMPLE_MASK_EXT,
   VK_CMD_SET_ALPHA_TO_COVERAGE_ENABLE_EXT,
   VK_CMD_SET_ALPHA_TO_ONE_ENABLE_EXT,
   VK_CMD_SET_LOGIC_OP_ENABLE_EXT,
   VK_CMD_SET_COLOR_BLEND_ENABLE_EXT,
   VK_CMD_SET_COLOR_BLEND_EQUATION_EXT,
   VK_CMD_SET_COLOR_WRITE_MASK_EXT,
   VK_CMD_SET_RASTERIZATION_STREAM_EXT,
   VK_CMD_SET_CONSERVATIVE_RASTERIZATION_MODE_EXT,
   VK_CMD_SET_EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT,
   VK_CMD_SET_DEPTH_CLIP_ENABLE_EXT,
   VK_CMD_SET_SAMPLE_LOCATIONS_ENABLE_EXT,
   VK_CMD_SET_COLOR_BLEND_ADVANCED_EXT,
   VK_CMD_SET_PROVOKING_VERTEX_MODE_EXT,
   VK_CMD_SET_LINE_RASTERIZATION_MODE_EXT,
   VK_CMD_SET_LINE_STIPPLE_ENABLE_EXT,
   VK_CMD_SET_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT,
   VK_CMD_SET_VIEWPORT_WSCALING_ENABLE_NV,
   VK_CMD_SET_VIEWPORT_SWIZZLE_NV,
   VK_CMD_SET_COVERAGE_TO_COLOR_ENABLE_NV,
   VK_CMD_SET_COVERAGE_TO_COLOR_LOCATION_NV,
   VK_CMD_SET_COVERAGE_MODULATION_MODE_NV,
   VK_CMD_SET_COVERAGE_MODULATION_TABLE_ENABLE_NV,
   VK_CMD_SET_COVERAGE_MODULATION_TABLE_NV,
   VK_CMD_SET_SHADING_RATE_IMAGE_ENABLE_NV,
   VK_CMD_SET_COVERAGE_REDUCTION_MODE_NV,
   VK_CMD_SET_REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV,
   VK_CMD_COPY_BUFFER2,
   VK_CMD_COPY_IMAGE2,
   VK_CMD_BLIT_IMAGE2,
   VK_CMD_COPY_BUFFER_TO_IMAGE2,
   VK_CMD_COPY_IMAGE_TO_BUFFER2,
   VK_CMD_RESOLVE_IMAGE2,
   VK_CMD_SET_FRAGMENT_SHADING_RATE_KHR,
   VK_CMD_SET_FRAGMENT_SHADING_RATE_ENUM_NV,
   VK_CMD_SET_VERTEX_INPUT_EXT,
   VK_CMD_SET_COLOR_WRITE_ENABLE_EXT,
   VK_CMD_SET_EVENT2,
   VK_CMD_RESET_EVENT2,
   VK_CMD_WAIT_EVENTS2,
   VK_CMD_PIPELINE_BARRIER2,
   VK_CMD_WRITE_TIMESTAMP2,
   VK_CMD_WRITE_BUFFER_MARKER2_AMD,
   VK_CMD_DECODE_VIDEO_KHR,
   VK_CMD_BEGIN_VIDEO_CODING_KHR,
   VK_CMD_CONTROL_VIDEO_CODING_KHR,
   VK_CMD_END_VIDEO_CODING_KHR,
   VK_CMD_ENCODE_VIDEO_KHR,
   VK_CMD_DECOMPRESS_MEMORY_NV,
   VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_NV,
   VK_CMD_BUILD_PARTITIONED_ACCELERATION_STRUCTURES_NV,
   VK_CMD_DECOMPRESS_MEMORY_EXT,
   VK_CMD_DECOMPRESS_MEMORY_INDIRECT_COUNT_EXT,
   VK_CMD_CU_LAUNCH_KERNEL_NVX,
   VK_CMD_BIND_DESCRIPTOR_BUFFERS_EXT,
   VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS_EXT,
   VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS_EXT,
   VK_CMD_BEGIN_RENDERING,
   VK_CMD_END_RENDERING,
   VK_CMD_END_RENDERING2_KHR,
   VK_CMD_BUILD_MICROMAPS_EXT,
   VK_CMD_COPY_MICROMAP_EXT,
   VK_CMD_COPY_MICROMAP_TO_MEMORY_EXT,
   VK_CMD_COPY_MEMORY_TO_MICROMAP_EXT,
   VK_CMD_WRITE_MICROMAPS_PROPERTIES_EXT,
   VK_CMD_BIND_TILE_MEMORY_QCOM,
   VK_CMD_OPTICAL_FLOW_EXECUTE_NV,
   VK_CMD_SET_DEPTH_BIAS2_EXT,
   VK_CMD_BIND_SHADERS_EXT,
   VK_CMD_BEGIN_GPA_SESSION_AMD,
   VK_CMD_END_GPA_SESSION_AMD,
   VK_CMD_BEGIN_GPA_SAMPLE_AMD,
   VK_CMD_END_GPA_SAMPLE_AMD,
   VK_CMD_COPY_GPA_SESSION_RESULTS_AMD,
   VK_CMD_BIND_DESCRIPTOR_SETS2,
   VK_CMD_PUSH_CONSTANTS2,
   VK_CMD_PUSH_DESCRIPTOR_SET2,
   VK_CMD_PUSH_DESCRIPTOR_SET_WITH_TEMPLATE2,
   VK_CMD_SET_DESCRIPTOR_BUFFER_OFFSETS2_EXT,
   VK_CMD_BIND_DESCRIPTOR_BUFFER_EMBEDDED_SAMPLERS2_EXT,
   VK_CMD_SET_RENDERING_ATTACHMENT_LOCATIONS,
   VK_CMD_SET_RENDERING_INPUT_ATTACHMENT_INDICES,
   VK_CMD_SET_DEPTH_CLAMP_RANGE_EXT,
   VK_CMD_CONVERT_COOPERATIVE_VECTOR_MATRIX_NV,
   VK_CMD_DISPATCH_TILE_QCOM,
   VK_CMD_BEGIN_PER_TILE_EXECUTION_QCOM,
   VK_CMD_END_PER_TILE_EXECUTION_QCOM,
   VK_CMD_BEGIN_SHADER_INSTRUMENTATION_ARM,
   VK_CMD_END_SHADER_INSTRUMENTATION_ARM,
   VK_CMD_COPY_TENSOR_ARM,
   VK_CMD_DISPATCH_DATA_GRAPH_ARM,
   VK_CMD_SET_COMPUTE_OCCUPANCY_PRIORITY_NV,
   VK_CMD_BIND_SAMPLER_HEAP_EXT,
   VK_CMD_BIND_RESOURCE_HEAP_EXT,
   VK_CMD_PUSH_DATA_EXT,
   VK_CMD_COPY_MEMORY_KHR,
   VK_CMD_COPY_MEMORY_TO_IMAGE_KHR,
   VK_CMD_COPY_IMAGE_TO_MEMORY_KHR,
   VK_CMD_UPDATE_MEMORY_KHR,
   VK_CMD_FILL_MEMORY_KHR,
   VK_CMD_COPY_QUERY_POOL_RESULTS_TO_MEMORY_KHR,
   VK_CMD_BEGIN_CONDITIONAL_RENDERING2_EXT,
   VK_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS2_EXT,
   VK_CMD_BEGIN_TRANSFORM_FEEDBACK2_EXT,
   VK_CMD_END_TRANSFORM_FEEDBACK2_EXT,
   VK_CMD_DRAW_INDIRECT_BYTE_COUNT2_EXT,
   VK_CMD_WRITE_MARKER_TO_MEMORY_AMD,
   VK_CMD_BIND_INDEX_BUFFER3_KHR,
   VK_CMD_BIND_VERTEX_BUFFERS3_KHR,
   VK_CMD_DRAW_INDIRECT2_KHR,
   VK_CMD_DRAW_INDEXED_INDIRECT2_KHR,
   VK_CMD_DRAW_INDIRECT_COUNT2_KHR,
   VK_CMD_DRAW_INDEXED_INDIRECT_COUNT2_KHR,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT2_EXT,
   VK_CMD_DRAW_MESH_TASKS_INDIRECT_COUNT2_EXT,
   VK_CMD_DISPATCH_INDIRECT2_KHR,
   VK_CMD_SET_DISPATCH_PARAMETERS_ARM,
   VK_CMD_TYPE_COUNT,
};

extern const char *vk_cmd_queue_type_names[];
extern size_t vk_cmd_queue_type_sizes[];

struct vk_cmd_bind_pipeline {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipeline pipeline;
};
struct vk_cmd_set_primitive_restart_index_ext {
   uint32_t primitive_restart_index;
};
struct vk_cmd_set_attachment_feedback_loop_enable_ext {
   VkImageAspectFlags aspect_mask;
};
struct vk_cmd_set_viewport {
   uint32_t first_viewport;
   uint32_t viewport_count;
   VkViewport* viewports;
};
struct vk_cmd_set_scissor {
   uint32_t first_scissor;
   uint32_t scissor_count;
   VkRect2D* scissors;
};
struct vk_cmd_set_line_width {
   float line_width;
};
struct vk_cmd_set_depth_bias {
   float depth_bias_constant_factor;
   float depth_bias_clamp;
   float depth_bias_slope_factor;
};
struct vk_cmd_set_blend_constants {
   float blend_constants[4];
};
struct vk_cmd_set_depth_bounds {
   float min_depth_bounds;
   float max_depth_bounds;
};
struct vk_cmd_set_stencil_compare_mask {
   VkStencilFaceFlags face_mask;
   uint32_t compare_mask;
};
struct vk_cmd_set_stencil_write_mask {
   VkStencilFaceFlags face_mask;
   uint32_t write_mask;
};
struct vk_cmd_set_stencil_reference {
   VkStencilFaceFlags face_mask;
   uint32_t reference;
};
struct vk_cmd_bind_descriptor_sets {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipelineLayout layout;
   uint32_t first_set;
   uint32_t descriptor_set_count;
   VkDescriptorSet* descriptor_sets;
   uint32_t dynamic_offset_count;
   uint32_t* dynamic_offsets;
};
struct vk_cmd_bind_index_buffer {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkIndexType index_type;
};
struct vk_cmd_bind_vertex_buffers {
   uint32_t first_binding;
   uint32_t binding_count;
   VkBuffer* buffers;
   VkDeviceSize* offsets;
};
struct vk_cmd_draw {
   uint32_t vertex_count;
   uint32_t instance_count;
   uint32_t first_vertex;
   uint32_t first_instance;
};
struct vk_cmd_draw_indexed {
   uint32_t index_count;
   uint32_t instance_count;
   uint32_t first_index;
   int32_t vertex_offset;
   uint32_t first_instance;
};
struct vk_cmd_draw_multi_ext {
   uint32_t draw_count;
   VkMultiDrawInfoEXT* vertex_info;
   uint32_t instance_count;
   uint32_t first_instance;
   uint32_t stride;
};
struct vk_cmd_draw_multi_indexed_ext {
   uint32_t draw_count;
   VkMultiDrawIndexedInfoEXT* index_info;
   uint32_t instance_count;
   uint32_t first_instance;
   uint32_t stride;
   int32_t* vertex_offset;
};
struct vk_cmd_draw_indirect {
   VkBuffer buffer;
   VkDeviceSize offset;
   uint32_t draw_count;
   uint32_t stride;
};
struct vk_cmd_draw_indexed_indirect {
   VkBuffer buffer;
   VkDeviceSize offset;
   uint32_t draw_count;
   uint32_t stride;
};
struct vk_cmd_dispatch {
   uint32_t group_count_x;
   uint32_t group_count_y;
   uint32_t group_count_z;
};
struct vk_cmd_dispatch_indirect {
   VkBuffer buffer;
   VkDeviceSize offset;
};
struct vk_cmd_draw_cluster_huawei {
   uint32_t group_count_x;
   uint32_t group_count_y;
   uint32_t group_count_z;
};
struct vk_cmd_draw_cluster_indirect_huawei {
   VkBuffer buffer;
   VkDeviceSize offset;
};
struct vk_cmd_update_pipeline_indirect_buffer_nv {
   VkPipelineBindPoint           pipeline_bind_point;
   VkPipeline                    pipeline;
};
struct vk_cmd_copy_buffer {
   VkBuffer src_buffer;
   VkBuffer dst_buffer;
   uint32_t region_count;
   VkBufferCopy* regions;
};
struct vk_cmd_copy_image {
   VkImage src_image;
   VkImageLayout src_image_layout;
   VkImage dst_image;
   VkImageLayout dst_image_layout;
   uint32_t region_count;
   VkImageCopy* regions;
};
struct vk_cmd_blit_image {
   VkImage src_image;
   VkImageLayout src_image_layout;
   VkImage dst_image;
   VkImageLayout dst_image_layout;
   uint32_t region_count;
   VkImageBlit* regions;
   VkFilter filter;
};
struct vk_cmd_copy_buffer_to_image {
   VkBuffer src_buffer;
   VkImage dst_image;
   VkImageLayout dst_image_layout;
   uint32_t region_count;
   VkBufferImageCopy* regions;
};
struct vk_cmd_copy_image_to_buffer {
   VkImage src_image;
   VkImageLayout src_image_layout;
   VkBuffer dst_buffer;
   uint32_t region_count;
   VkBufferImageCopy* regions;
};
struct vk_cmd_copy_memory_indirect_nv {
   VkDeviceAddress copy_buffer_address;
   uint32_t copy_count;
   uint32_t stride;
};
struct vk_cmd_copy_memory_indirect_khr {
   VkCopyMemoryIndirectInfoKHR* copy_memory_indirect_info;
};
struct vk_cmd_copy_memory_to_image_indirect_nv {
   VkDeviceAddress copy_buffer_address;
   uint32_t copy_count;
   uint32_t stride;
   VkImage dst_image;
   VkImageLayout dst_image_layout;
   VkImageSubresourceLayers* image_subresources;
};
struct vk_cmd_copy_memory_to_image_indirect_khr {
   VkCopyMemoryToImageIndirectInfoKHR* copy_memory_to_image_indirect_info;
};
struct vk_cmd_update_buffer {
   VkBuffer dst_buffer;
   VkDeviceSize dst_offset;
   VkDeviceSize data_size;
   void* data;
};
struct vk_cmd_fill_buffer {
   VkBuffer dst_buffer;
   VkDeviceSize dst_offset;
   VkDeviceSize size;
   uint32_t data;
};
struct vk_cmd_clear_color_image {
   VkImage image;
   VkImageLayout image_layout;
   VkClearColorValue* color;
   uint32_t range_count;
   VkImageSubresourceRange* ranges;
};
struct vk_cmd_clear_depth_stencil_image {
   VkImage image;
   VkImageLayout image_layout;
   VkClearDepthStencilValue* depth_stencil;
   uint32_t range_count;
   VkImageSubresourceRange* ranges;
};
struct vk_cmd_clear_attachments {
   uint32_t attachment_count;
   VkClearAttachment* attachments;
   uint32_t rect_count;
   VkClearRect* rects;
};
struct vk_cmd_resolve_image {
   VkImage src_image;
   VkImageLayout src_image_layout;
   VkImage dst_image;
   VkImageLayout dst_image_layout;
   uint32_t region_count;
   VkImageResolve* regions;
};
struct vk_cmd_set_event {
   VkEvent event;
   VkPipelineStageFlags stage_mask;
};
struct vk_cmd_reset_event {
   VkEvent event;
   VkPipelineStageFlags stage_mask;
};
struct vk_cmd_wait_events {
   uint32_t event_count;
   VkEvent* events;
   VkPipelineStageFlags src_stage_mask;
   VkPipelineStageFlags dst_stage_mask;
   uint32_t memory_barrier_count;
   VkMemoryBarrier* memory_barriers;
   uint32_t buffer_memory_barrier_count;
   VkBufferMemoryBarrier* buffer_memory_barriers;
   uint32_t image_memory_barrier_count;
   VkImageMemoryBarrier* image_memory_barriers;
};
struct vk_cmd_pipeline_barrier {
   VkPipelineStageFlags src_stage_mask;
   VkPipelineStageFlags dst_stage_mask;
   VkDependencyFlags dependency_flags;
   uint32_t memory_barrier_count;
   VkMemoryBarrier* memory_barriers;
   uint32_t buffer_memory_barrier_count;
   VkBufferMemoryBarrier* buffer_memory_barriers;
   uint32_t image_memory_barrier_count;
   VkImageMemoryBarrier* image_memory_barriers;
};
struct vk_cmd_begin_query {
   VkQueryPool query_pool;
   uint32_t query;
   VkQueryControlFlags flags;
};
struct vk_cmd_end_query {
   VkQueryPool query_pool;
   uint32_t query;
};
struct vk_cmd_begin_conditional_rendering_ext {
   VkConditionalRenderingBeginInfoEXT* conditional_rendering_begin;
};
struct vk_cmd_begin_custom_resolve_ext {
   VkBeginCustomResolveInfoEXT* begin_custom_resolve_info;
};
struct vk_cmd_reset_query_pool {
   VkQueryPool query_pool;
   uint32_t first_query;
   uint32_t query_count;
};
struct vk_cmd_write_timestamp {
   VkPipelineStageFlagBits pipeline_stage;
   VkQueryPool query_pool;
   uint32_t query;
};
struct vk_cmd_copy_query_pool_results {
   VkQueryPool query_pool;
   uint32_t first_query;
   uint32_t query_count;
   VkBuffer dst_buffer;
   VkDeviceSize dst_offset;
   VkDeviceSize stride;
   VkQueryResultFlags flags;
};
struct vk_cmd_push_constants {
   VkPipelineLayout layout;
   VkShaderStageFlags stage_flags;
   uint32_t offset;
   uint32_t size;
   void* values;
};
struct vk_cmd_begin_render_pass {
   VkRenderPassBeginInfo* render_pass_begin;
   VkSubpassContents contents;
};
struct vk_cmd_next_subpass {
   VkSubpassContents contents;
};
struct vk_cmd_execute_commands {
   uint32_t command_buffer_count;
   VkCommandBuffer* command_buffers;
};
struct vk_cmd_debug_marker_begin_ext {
   VkDebugMarkerMarkerInfoEXT* marker_info;
};
struct vk_cmd_debug_marker_insert_ext {
   VkDebugMarkerMarkerInfoEXT* marker_info;
};
struct vk_cmd_execute_generated_commands_nv {
   VkBool32 is_preprocessed;
   VkGeneratedCommandsInfoNV* generated_commands_info;
};
struct vk_cmd_preprocess_generated_commands_nv {
   VkGeneratedCommandsInfoNV* generated_commands_info;
};
struct vk_cmd_bind_pipeline_shader_group_nv {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipeline pipeline;
   uint32_t group_index;
};
struct vk_cmd_execute_generated_commands_ext {
   VkBool32 is_preprocessed;
   VkGeneratedCommandsInfoEXT* generated_commands_info;
};
struct vk_cmd_preprocess_generated_commands_ext {
   VkGeneratedCommandsInfoEXT* generated_commands_info;
   VkCommandBuffer state_command_buffer;
};
struct vk_cmd_push_descriptor_set {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipelineLayout layout;
   uint32_t set;
   uint32_t descriptor_write_count;
   VkWriteDescriptorSet* descriptor_writes;
};
struct vk_cmd_set_device_mask {
   uint32_t device_mask;
};
struct vk_cmd_dispatch_base {
   uint32_t base_group_x;
   uint32_t base_group_y;
   uint32_t base_group_z;
   uint32_t group_count_x;
   uint32_t group_count_y;
   uint32_t group_count_z;
};
struct vk_cmd_push_descriptor_set_with_template {
   VkDescriptorUpdateTemplate descriptor_update_template;
   VkPipelineLayout layout;
   uint32_t set;
   void* data;
};
struct vk_cmd_set_viewport_wscaling_nv {
   uint32_t first_viewport;
   uint32_t viewport_count;
   VkViewportWScalingNV* viewport_wscalings;
};
struct vk_cmd_set_discard_rectangle_ext {
   uint32_t first_discard_rectangle;
   uint32_t discard_rectangle_count;
   VkRect2D* discard_rectangles;
};
struct vk_cmd_set_discard_rectangle_enable_ext {
   VkBool32 discard_rectangle_enable;
};
struct vk_cmd_set_discard_rectangle_mode_ext {
   VkDiscardRectangleModeEXT discard_rectangle_mode;
};
struct vk_cmd_set_sample_locations_ext {
   VkSampleLocationsInfoEXT* sample_locations_info;
};
struct vk_cmd_begin_debug_utils_label_ext {
   VkDebugUtilsLabelEXT* label_info;
};
struct vk_cmd_insert_debug_utils_label_ext {
   VkDebugUtilsLabelEXT* label_info;
};
struct vk_cmd_write_buffer_marker_amd {
   VkPipelineStageFlagBits pipeline_stage;
   VkBuffer dst_buffer;
   VkDeviceSize dst_offset;
   uint32_t marker;
};
struct vk_cmd_begin_render_pass2 {
   VkRenderPassBeginInfo*      render_pass_begin;
   VkSubpassBeginInfo*      subpass_begin_info;
};
struct vk_cmd_next_subpass2 {
   VkSubpassBeginInfo*      subpass_begin_info;
   VkSubpassEndInfo*        subpass_end_info;
};
struct vk_cmd_end_render_pass2 {
   VkSubpassEndInfo*        subpass_end_info;
};
struct vk_cmd_draw_indirect_count {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkBuffer count_buffer;
   VkDeviceSize count_buffer_offset;
   uint32_t max_draw_count;
   uint32_t stride;
};
struct vk_cmd_draw_indexed_indirect_count {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkBuffer count_buffer;
   VkDeviceSize count_buffer_offset;
   uint32_t max_draw_count;
   uint32_t stride;
};
struct vk_cmd_set_checkpoint_nv {
   void* checkpoint_marker;
};
struct vk_cmd_bind_transform_feedback_buffers_ext {
   uint32_t first_binding;
   uint32_t binding_count;
   VkBuffer* buffers;
   VkDeviceSize* offsets;
   VkDeviceSize* sizes;
};
struct vk_cmd_begin_transform_feedback_ext {
   uint32_t first_counter_buffer;
   uint32_t counter_buffer_count;
   VkBuffer* counter_buffers;
   VkDeviceSize* counter_buffer_offsets;
};
struct vk_cmd_end_transform_feedback_ext {
   uint32_t first_counter_buffer;
   uint32_t counter_buffer_count;
   VkBuffer* counter_buffers;
   VkDeviceSize* counter_buffer_offsets;
};
struct vk_cmd_begin_query_indexed_ext {
   VkQueryPool query_pool;
   uint32_t query;
   VkQueryControlFlags flags;
   uint32_t index;
};
struct vk_cmd_end_query_indexed_ext {
   VkQueryPool query_pool;
   uint32_t query;
   uint32_t index;
};
struct vk_cmd_draw_indirect_byte_count_ext {
   uint32_t instance_count;
   uint32_t first_instance;
   VkBuffer counter_buffer;
   VkDeviceSize counter_buffer_offset;
   uint32_t counter_offset;
   uint32_t vertex_stride;
};
struct vk_cmd_set_exclusive_scissor_nv {
   uint32_t first_exclusive_scissor;
   uint32_t exclusive_scissor_count;
   VkRect2D* exclusive_scissors;
};
struct vk_cmd_set_exclusive_scissor_enable_nv {
   uint32_t first_exclusive_scissor;
   uint32_t exclusive_scissor_count;
   VkBool32* exclusive_scissor_enables;
};
struct vk_cmd_bind_shading_rate_image_nv {
   VkImageView image_view;
   VkImageLayout image_layout;
};
struct vk_cmd_set_viewport_shading_rate_palette_nv {
   uint32_t first_viewport;
   uint32_t viewport_count;
   VkShadingRatePaletteNV* shading_rate_palettes;
};
struct vk_cmd_set_coarse_sample_order_nv {
   VkCoarseSampleOrderTypeNV sample_order_type;
   uint32_t custom_sample_order_count;
   VkCoarseSampleOrderCustomNV* custom_sample_orders;
};
struct vk_cmd_draw_mesh_tasks_nv {
   uint32_t task_count;
   uint32_t first_task;
};
struct vk_cmd_draw_mesh_tasks_indirect_nv {
   VkBuffer buffer;
   VkDeviceSize offset;
   uint32_t draw_count;
   uint32_t stride;
};
struct vk_cmd_draw_mesh_tasks_indirect_count_nv {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkBuffer count_buffer;
   VkDeviceSize count_buffer_offset;
   uint32_t max_draw_count;
   uint32_t stride;
};
struct vk_cmd_draw_mesh_tasks_ext {
   uint32_t group_count_x;
   uint32_t group_count_y;
   uint32_t group_count_z;
};
struct vk_cmd_draw_mesh_tasks_indirect_ext {
   VkBuffer buffer;
   VkDeviceSize offset;
   uint32_t draw_count;
   uint32_t stride;
};
struct vk_cmd_draw_mesh_tasks_indirect_count_ext {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkBuffer count_buffer;
   VkDeviceSize count_buffer_offset;
   uint32_t max_draw_count;
   uint32_t stride;
};
struct vk_cmd_bind_invocation_mask_huawei {
   VkImageView image_view;
   VkImageLayout image_layout;
};
struct vk_cmd_copy_acceleration_structure_nv {
   VkAccelerationStructureNV dst;
   VkAccelerationStructureNV src;
   VkCopyAccelerationStructureModeKHR mode;
};
struct vk_cmd_copy_acceleration_structure_khr {
   VkCopyAccelerationStructureInfoKHR* info;
};
struct vk_cmd_copy_acceleration_structure_to_memory_khr {
   VkCopyAccelerationStructureToMemoryInfoKHR* info;
};
struct vk_cmd_copy_memory_to_acceleration_structure_khr {
   VkCopyMemoryToAccelerationStructureInfoKHR* info;
};
struct vk_cmd_write_acceleration_structures_properties_khr {
   uint32_t acceleration_structure_count;
   VkAccelerationStructureKHR* acceleration_structures;
   VkQueryType query_type;
   VkQueryPool query_pool;
   uint32_t first_query;
};
struct vk_cmd_write_acceleration_structures_properties_nv {
   uint32_t acceleration_structure_count;
   VkAccelerationStructureNV* acceleration_structures;
   VkQueryType query_type;
   VkQueryPool query_pool;
   uint32_t first_query;
};
struct vk_cmd_build_acceleration_structure_nv {
   VkAccelerationStructureInfoNV* info;
   VkBuffer instance_data;
   VkDeviceSize instance_offset;
   VkBool32 update;
   VkAccelerationStructureNV dst;
   VkAccelerationStructureNV src;
   VkBuffer scratch;
   VkDeviceSize scratch_offset;
};
struct vk_cmd_trace_rays_khr {
   VkStridedDeviceAddressRegionKHR* raygen_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* miss_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* hit_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* callable_shader_binding_table;
   uint32_t width;
   uint32_t height;
   uint32_t depth;
};
struct vk_cmd_trace_rays_nv {
   VkBuffer raygen_shader_binding_table_buffer;
   VkDeviceSize raygen_shader_binding_offset;
   VkBuffer miss_shader_binding_table_buffer;
   VkDeviceSize miss_shader_binding_offset;
   VkDeviceSize miss_shader_binding_stride;
   VkBuffer hit_shader_binding_table_buffer;
   VkDeviceSize hit_shader_binding_offset;
   VkDeviceSize hit_shader_binding_stride;
   VkBuffer callable_shader_binding_table_buffer;
   VkDeviceSize callable_shader_binding_offset;
   VkDeviceSize callable_shader_binding_stride;
   uint32_t width;
   uint32_t height;
   uint32_t depth;
};
struct vk_cmd_trace_rays_indirect_khr {
   VkStridedDeviceAddressRegionKHR* raygen_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* miss_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* hit_shader_binding_table;
   VkStridedDeviceAddressRegionKHR* callable_shader_binding_table;
   VkDeviceAddress indirect_device_address;
};
struct vk_cmd_trace_rays_indirect2_khr {
   VkDeviceAddress indirect_device_address;
};
struct vk_cmd_build_cluster_acceleration_structure_indirect_nv {
   VkClusterAccelerationStructureCommandsInfoNV*  command_infos;
};
struct vk_cmd_set_ray_tracing_pipeline_stack_size_khr {
   uint32_t pipeline_stack_size;
};
struct vk_cmd_set_performance_marker_intel {
   VkPerformanceMarkerInfoINTEL* marker_info;
};
struct vk_cmd_set_performance_stream_marker_intel {
   VkPerformanceStreamMarkerInfoINTEL* marker_info;
};
struct vk_cmd_set_performance_override_intel {
   VkPerformanceOverrideInfoINTEL* override_info;
};
struct vk_cmd_set_line_stipple {
   uint32_t line_stipple_factor;
   uint16_t line_stipple_pattern;
};
struct vk_cmd_build_acceleration_structures_khr {
   uint32_t info_count;
   VkAccelerationStructureBuildGeometryInfoKHR* infos;
   const VkAccelerationStructureBuildRangeInfoKHR* * pp_build_range_infos;
};
struct vk_cmd_build_acceleration_structures_indirect_khr {
   uint32_t                                           info_count;
   VkAccelerationStructureBuildGeometryInfoKHR* infos;
   VkDeviceAddress*             indirect_device_addresses;
   uint32_t*                    indirect_strides;
   const uint32_t* *             pp_max_primitive_counts;
};
struct vk_cmd_set_cull_mode {
   VkCullModeFlags cull_mode;
};
struct vk_cmd_set_front_face {
   VkFrontFace front_face;
};
struct vk_cmd_set_primitive_topology {
   VkPrimitiveTopology primitive_topology;
};
struct vk_cmd_set_viewport_with_count {
   uint32_t viewport_count;
   VkViewport* viewports;
};
struct vk_cmd_set_scissor_with_count {
   uint32_t scissor_count;
   VkRect2D* scissors;
};
struct vk_cmd_bind_index_buffer2 {
   VkBuffer buffer;
   VkDeviceSize offset;
   VkDeviceSize size;
   VkIndexType index_type;
};
struct vk_cmd_bind_vertex_buffers2 {
   uint32_t first_binding;
   uint32_t binding_count;
   VkBuffer* buffers;
   VkDeviceSize* offsets;
   VkDeviceSize* sizes;
   VkDeviceSize* strides;
};
struct vk_cmd_set_depth_test_enable {
   VkBool32 depth_test_enable;
};
struct vk_cmd_set_depth_write_enable {
   VkBool32 depth_write_enable;
};
struct vk_cmd_set_depth_compare_op {
   VkCompareOp depth_compare_op;
};
struct vk_cmd_set_depth_bounds_test_enable {
   VkBool32 depth_bounds_test_enable;
};
struct vk_cmd_set_stencil_test_enable {
   VkBool32 stencil_test_enable;
};
struct vk_cmd_set_stencil_op {
   VkStencilFaceFlags face_mask;
   VkStencilOp fail_op;
   VkStencilOp pass_op;
   VkStencilOp depth_fail_op;
   VkCompareOp compare_op;
};
struct vk_cmd_set_patch_control_points_ext {
   uint32_t patch_control_points;
};
struct vk_cmd_set_rasterizer_discard_enable {
   VkBool32 rasterizer_discard_enable;
};
struct vk_cmd_set_depth_bias_enable {
   VkBool32 depth_bias_enable;
};
struct vk_cmd_set_logic_op_ext {
   VkLogicOp logic_op;
};
struct vk_cmd_set_primitive_restart_enable {
   VkBool32 primitive_restart_enable;
};
struct vk_cmd_set_tessellation_domain_origin_ext {
   VkTessellationDomainOrigin domain_origin;
};
struct vk_cmd_set_depth_clamp_enable_ext {
   VkBool32 depth_clamp_enable;
};
struct vk_cmd_set_polygon_mode_ext {
   VkPolygonMode polygon_mode;
};
struct vk_cmd_set_rasterization_samples_ext {
   VkSampleCountFlagBits  rasterization_samples;
};
struct vk_cmd_set_sample_mask_ext {
   VkSampleCountFlagBits  samples;
   VkSampleMask*    sample_mask;
};
struct vk_cmd_set_alpha_to_coverage_enable_ext {
   VkBool32 alpha_to_coverage_enable;
};
struct vk_cmd_set_alpha_to_one_enable_ext {
   VkBool32 alpha_to_one_enable;
};
struct vk_cmd_set_logic_op_enable_ext {
   VkBool32 logic_op_enable;
};
struct vk_cmd_set_color_blend_enable_ext {
   uint32_t first_attachment;
   uint32_t attachment_count;
   VkBool32* color_blend_enables;
};
struct vk_cmd_set_color_blend_equation_ext {
   uint32_t first_attachment;
   uint32_t attachment_count;
   VkColorBlendEquationEXT* color_blend_equations;
};
struct vk_cmd_set_color_write_mask_ext {
   uint32_t first_attachment;
   uint32_t attachment_count;
   VkColorComponentFlags* color_write_masks;
};
struct vk_cmd_set_rasterization_stream_ext {
   uint32_t rasterization_stream;
};
struct vk_cmd_set_conservative_rasterization_mode_ext {
   VkConservativeRasterizationModeEXT conservative_rasterization_mode;
};
struct vk_cmd_set_extra_primitive_overestimation_size_ext {
   float extra_primitive_overestimation_size;
};
struct vk_cmd_set_depth_clip_enable_ext {
   VkBool32 depth_clip_enable;
};
struct vk_cmd_set_sample_locations_enable_ext {
   VkBool32 sample_locations_enable;
};
struct vk_cmd_set_color_blend_advanced_ext {
   uint32_t first_attachment;
   uint32_t attachment_count;
   VkColorBlendAdvancedEXT* color_blend_advanced;
};
struct vk_cmd_set_provoking_vertex_mode_ext {
   VkProvokingVertexModeEXT provoking_vertex_mode;
};
struct vk_cmd_set_line_rasterization_mode_ext {
   VkLineRasterizationModeEXT line_rasterization_mode;
};
struct vk_cmd_set_line_stipple_enable_ext {
   VkBool32 stippled_line_enable;
};
struct vk_cmd_set_depth_clip_negative_one_to_one_ext {
   VkBool32 negative_one_to_one;
};
struct vk_cmd_set_viewport_wscaling_enable_nv {
   VkBool32 viewport_wscaling_enable;
};
struct vk_cmd_set_viewport_swizzle_nv {
   uint32_t first_viewport;
   uint32_t viewport_count;
   VkViewportSwizzleNV* viewport_swizzles;
};
struct vk_cmd_set_coverage_to_color_enable_nv {
   VkBool32 coverage_to_color_enable;
};
struct vk_cmd_set_coverage_to_color_location_nv {
   uint32_t coverage_to_color_location;
};
struct vk_cmd_set_coverage_modulation_mode_nv {
   VkCoverageModulationModeNV coverage_modulation_mode;
};
struct vk_cmd_set_coverage_modulation_table_enable_nv {
   VkBool32 coverage_modulation_table_enable;
};
struct vk_cmd_set_coverage_modulation_table_nv {
   uint32_t coverage_modulation_table_count;
   float* coverage_modulation_table;
};
struct vk_cmd_set_shading_rate_image_enable_nv {
   VkBool32 shading_rate_image_enable;
};
struct vk_cmd_set_coverage_reduction_mode_nv {
   VkCoverageReductionModeNV coverage_reduction_mode;
};
struct vk_cmd_set_representative_fragment_test_enable_nv {
   VkBool32 representative_fragment_test_enable;
};
struct vk_cmd_copy_buffer2 {
   VkCopyBufferInfo2* copy_buffer_info;
};
struct vk_cmd_copy_image2 {
   VkCopyImageInfo2* copy_image_info;
};
struct vk_cmd_blit_image2 {
   VkBlitImageInfo2* blit_image_info;
};
struct vk_cmd_copy_buffer_to_image2 {
   VkCopyBufferToImageInfo2* copy_buffer_to_image_info;
};
struct vk_cmd_copy_image_to_buffer2 {
   VkCopyImageToBufferInfo2* copy_image_to_buffer_info;
};
struct vk_cmd_resolve_image2 {
   VkResolveImageInfo2* resolve_image_info;
};
struct vk_cmd_set_fragment_shading_rate_khr {
   VkExtent2D*                           fragment_size;
   VkFragmentShadingRateCombinerOpKHR    combiner_ops[2];
};
struct vk_cmd_set_fragment_shading_rate_enum_nv {
   VkFragmentShadingRateNV                     shading_rate;
   VkFragmentShadingRateCombinerOpKHR    combiner_ops[2];
};
struct vk_cmd_set_vertex_input_ext {
   uint32_t vertex_binding_description_count;
   VkVertexInputBindingDescription2EXT* vertex_binding_descriptions;
   uint32_t vertex_attribute_description_count;
   VkVertexInputAttributeDescription2EXT* vertex_attribute_descriptions;
};
struct vk_cmd_set_color_write_enable_ext {
   uint32_t                                attachment_count;
   VkBool32*   color_write_enables;
};
struct vk_cmd_set_event2 {
   VkEvent                                             event;
   VkDependencyInfo*                             dependency_info;
};
struct vk_cmd_reset_event2 {
   VkEvent                                             event;
   VkPipelineStageFlags2               stage_mask;
};
struct vk_cmd_wait_events2 {
   uint32_t                                            event_count;
   VkEvent*                     events;
   VkDependencyInfo*            dependency_infos;
};
struct vk_cmd_pipeline_barrier2 {
   VkDependencyInfo*                             dependency_info;
};
struct vk_cmd_write_timestamp2 {
   VkPipelineStageFlags2               stage;
   VkQueryPool                                         query_pool;
   uint32_t                                            query;
};
struct vk_cmd_write_buffer_marker2_amd {
   VkPipelineStageFlags2               stage;
   VkBuffer                                            dst_buffer;
   VkDeviceSize                                        dst_offset;
   uint32_t                                            marker;
};
struct vk_cmd_decode_video_khr {
   VkVideoDecodeInfoKHR* decode_info;
};
struct vk_cmd_begin_video_coding_khr {
   VkVideoBeginCodingInfoKHR* begin_info;
};
struct vk_cmd_control_video_coding_khr {
   VkVideoCodingControlInfoKHR* coding_control_info;
};
struct vk_cmd_end_video_coding_khr {
   VkVideoEndCodingInfoKHR* end_coding_info;
};
struct vk_cmd_encode_video_khr {
   VkVideoEncodeInfoKHR* encode_info;
};
struct vk_cmd_decompress_memory_nv {
   uint32_t decompress_region_count;
   VkDecompressMemoryRegionNV* decompress_memory_regions;
};
struct vk_cmd_decompress_memory_indirect_count_nv {
   VkDeviceAddress indirect_commands_address;
   VkDeviceAddress indirect_commands_count_address;
   uint32_t stride;
};
struct vk_cmd_build_partitioned_acceleration_structures_nv {
   VkBuildPartitionedAccelerationStructureInfoNV*  build_info;
};
struct vk_cmd_decompress_memory_ext {
   VkDecompressMemoryInfoEXT* decompress_memory_info_ext;
};
struct vk_cmd_decompress_memory_indirect_count_ext {
   VkMemoryDecompressionMethodFlagsEXT decompression_method;
   VkDeviceAddress indirect_commands_address;
   VkDeviceAddress indirect_commands_count_address;
   uint32_t max_decompression_count;
   uint32_t stride;
};
struct vk_cmd_cu_launch_kernel_nvx {
   VkCuLaunchInfoNVX* launch_info;
};
struct vk_cmd_bind_descriptor_buffers_ext {
   uint32_t buffer_count;
   VkDescriptorBufferBindingInfoEXT* binding_infos;
};
struct vk_cmd_set_descriptor_buffer_offsets_ext {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipelineLayout layout;
   uint32_t first_set;
   uint32_t set_count;
   uint32_t* buffer_indices;
   VkDeviceSize* offsets;
};
struct vk_cmd_bind_descriptor_buffer_embedded_samplers_ext {
   VkPipelineBindPoint pipeline_bind_point;
   VkPipelineLayout layout;
   uint32_t set;
};
struct vk_cmd_begin_rendering {
   VkRenderingInfo*                              rendering_info;
};
struct vk_cmd_end_rendering2_khr {
   VkRenderingEndInfoKHR*        rendering_end_info;
};
struct vk_cmd_build_micromaps_ext {
   uint32_t                                      info_count;
   VkMicromapBuildInfoEXT* infos;
};
struct vk_cmd_copy_micromap_ext {
   VkCopyMicromapInfoEXT*      info;
};
struct vk_cmd_copy_micromap_to_memory_ext {
   VkCopyMicromapToMemoryInfoEXT* info;
};
struct vk_cmd_copy_memory_to_micromap_ext {
   VkCopyMemoryToMicromapInfoEXT* info;
};
struct vk_cmd_write_micromaps_properties_ext {
   uint32_t                                 micromap_count;
   VkMicromapEXT* micromaps;
   VkQueryType        query_type;
   VkQueryPool                              query_pool;
   uint32_t                                 first_query;
};
struct vk_cmd_bind_tile_memory_qcom {
   VkTileMemoryBindInfoQCOM* tile_memory_bind_info;
};
struct vk_cmd_optical_flow_execute_nv {
   VkOpticalFlowSessionNV session;
   VkOpticalFlowExecuteInfoNV* execute_info;
};
struct vk_cmd_set_depth_bias2_ext {
   VkDepthBiasInfoEXT*         depth_bias_info;
};
struct vk_cmd_bind_shaders_ext {
   uint32_t stage_count;
   VkShaderStageFlagBits* stages;
   VkShaderEXT* shaders;
};
struct vk_cmd_begin_gpa_session_amd {
   VkGpaSessionAMD                   gpa_session;
};
struct vk_cmd_end_gpa_session_amd {
   VkGpaSessionAMD                   gpa_session;
};
struct vk_cmd_begin_gpa_sample_amd {
   VkGpaSessionAMD                   gpa_session;
   VkGpaSampleBeginInfoAMD*    gpa_sample_begin_info;
   uint32_t*                         sample_id;
};
struct vk_cmd_end_gpa_sample_amd {
   VkGpaSessionAMD                   gpa_session;
   uint32_t                          sample_id;
};
struct vk_cmd_copy_gpa_session_results_amd {
   VkGpaSessionAMD                   gpa_session;
};
struct vk_cmd_bind_descriptor_sets2 {
   VkBindDescriptorSetsInfo*   bind_descriptor_sets_info;
};
struct vk_cmd_push_constants2 {
   VkPushConstantsInfo*        push_constants_info;
};
struct vk_cmd_push_descriptor_set2 {
   VkPushDescriptorSetInfo*    push_descriptor_set_info;
};
struct vk_cmd_push_descriptor_set_with_template2 {
   VkPushDescriptorSetWithTemplateInfo* push_descriptor_set_with_template_info;
};
struct vk_cmd_set_descriptor_buffer_offsets2_ext {
   VkSetDescriptorBufferOffsetsInfoEXT* set_descriptor_buffer_offsets_info;
};
struct vk_cmd_bind_descriptor_buffer_embedded_samplers2_ext {
   VkBindDescriptorBufferEmbeddedSamplersInfoEXT* bind_descriptor_buffer_embedded_samplers_info;
};
struct vk_cmd_set_rendering_attachment_locations {
   VkRenderingAttachmentLocationInfo* location_info;
};
struct vk_cmd_set_rendering_input_attachment_indices {
   VkRenderingInputAttachmentIndexInfo* input_attachment_index_info;
};
struct vk_cmd_set_depth_clamp_range_ext {
   VkDepthClampModeEXT depth_clamp_mode;
   VkDepthClampRangeEXT* depth_clamp_range;
};
struct vk_cmd_convert_cooperative_vector_matrix_nv {
   uint32_t info_count;
   VkConvertCooperativeVectorMatrixInfoNV* infos;
};
struct vk_cmd_dispatch_tile_qcom {
   VkDispatchTileInfoQCOM* dispatch_tile_info;
};
struct vk_cmd_begin_per_tile_execution_qcom {
   VkPerTileBeginInfoQCOM* per_tile_begin_info;
};
struct vk_cmd_end_per_tile_execution_qcom {
   VkPerTileEndInfoQCOM* per_tile_end_info;
};
struct vk_cmd_begin_shader_instrumentation_arm {
   VkShaderInstrumentationARM instrumentation;
};
struct vk_cmd_copy_tensor_arm {
   VkCopyTensorInfoARM* copy_tensor_info;
};
struct vk_cmd_dispatch_data_graph_arm {
   VkDataGraphPipelineSessionARM session;
   VkDataGraphPipelineDispatchInfoARM* info;
};
struct vk_cmd_set_compute_occupancy_priority_nv {
   VkComputeOccupancyPriorityParametersNV* parameters;
};
struct vk_cmd_bind_sampler_heap_ext {
   VkBindHeapInfoEXT*                            bind_info;
};
struct vk_cmd_bind_resource_heap_ext {
   VkBindHeapInfoEXT*                            bind_info;
};
struct vk_cmd_push_data_ext {
   VkPushDataInfoEXT*                            push_data_info;
};
struct vk_cmd_copy_memory_khr {
   VkCopyDeviceMemoryInfoKHR* copy_memory_info;
};
struct vk_cmd_copy_memory_to_image_khr {
   VkCopyDeviceMemoryImageInfoKHR* copy_memory_info;
};
struct vk_cmd_copy_image_to_memory_khr {
   VkCopyDeviceMemoryImageInfoKHR* copy_memory_info;
};
struct vk_cmd_update_memory_khr {
   VkDeviceAddressRangeKHR*      dst_range;
   VkAddressCommandFlagsKHR dst_flags;
   VkDeviceSize                        data_size;
   void*          data;
};
struct vk_cmd_fill_memory_khr {
   VkDeviceAddressRangeKHR*      dst_range;
   VkAddressCommandFlagsKHR dst_flags;
   uint32_t                            data;
};
struct vk_cmd_copy_query_pool_results_to_memory_khr {
   VkQueryPool                         query_pool;
   uint32_t                            first_query;
   uint32_t                            query_count;
   VkStridedDeviceAddressRangeKHR* dst_range;
   VkAddressCommandFlagsKHR    dst_flags;
   VkQueryResultFlags  query_result_flags;
};
struct vk_cmd_begin_conditional_rendering2_ext {
   VkConditionalRenderingBeginInfo2EXT* conditional_rendering_begin;
};
struct vk_cmd_bind_transform_feedback_buffers2_ext {
   uint32_t                            first_binding;
   uint32_t                            binding_count;
   VkBindTransformFeedbackBuffer2InfoEXT* binding_infos;
};
struct vk_cmd_begin_transform_feedback2_ext {
   uint32_t                            first_counter_range;
   uint32_t            counter_range_count;
   VkBindTransformFeedbackBuffer2InfoEXT* counter_infos;
};
struct vk_cmd_end_transform_feedback2_ext {
   uint32_t                            first_counter_range;
   uint32_t            counter_range_count;
   VkBindTransformFeedbackBuffer2InfoEXT* counter_infos;
};
struct vk_cmd_draw_indirect_byte_count2_ext {
   uint32_t instance_count;
   uint32_t first_instance;
   VkBindTransformFeedbackBuffer2InfoEXT* counter_info;
   uint32_t counter_offset;
   uint32_t vertex_stride;
};
struct vk_cmd_write_marker_to_memory_amd {
   VkMemoryMarkerInfoAMD*         info;
};
struct vk_cmd_bind_index_buffer3_khr {
   VkBindIndexBuffer3InfoKHR*    info;
};
struct vk_cmd_bind_vertex_buffers3_khr {
   uint32_t                            first_binding;
   uint32_t                            binding_count;
   VkBindVertexBuffer3InfoKHR* binding_infos;
};
struct vk_cmd_draw_indirect2_khr {
   VkDrawIndirect2InfoKHR*       info;
};
struct vk_cmd_draw_indexed_indirect2_khr {
   VkDrawIndirect2InfoKHR*       info;
};
struct vk_cmd_draw_indirect_count2_khr {
   VkDrawIndirectCount2InfoKHR*  info;
};
struct vk_cmd_draw_indexed_indirect_count2_khr {
   VkDrawIndirectCount2InfoKHR*  info;
};
struct vk_cmd_draw_mesh_tasks_indirect2_ext {
   VkDrawIndirect2InfoKHR*       info;
};
struct vk_cmd_draw_mesh_tasks_indirect_count2_ext {
   VkDrawIndirectCount2InfoKHR*  info;
};
struct vk_cmd_dispatch_indirect2_khr {
   VkDispatchIndirect2InfoKHR*   info;
};
struct vk_cmd_set_dispatch_parameters_arm {
   VkDispatchParametersARM*    dispatch_parameters;
};

struct vk_cmd_queue_entry;

struct vk_cmd_queue_entry {
   struct list_head cmd_link;
   enum vk_cmd_type type;
   union {
      struct vk_cmd_bind_pipeline bind_pipeline;
      struct vk_cmd_set_primitive_restart_index_ext set_primitive_restart_index_ext;
      struct vk_cmd_set_attachment_feedback_loop_enable_ext set_attachment_feedback_loop_enable_ext;
      struct vk_cmd_set_viewport set_viewport;
      struct vk_cmd_set_scissor set_scissor;
      struct vk_cmd_set_line_width set_line_width;
      struct vk_cmd_set_depth_bias set_depth_bias;
      struct vk_cmd_set_blend_constants set_blend_constants;
      struct vk_cmd_set_depth_bounds set_depth_bounds;
      struct vk_cmd_set_stencil_compare_mask set_stencil_compare_mask;
      struct vk_cmd_set_stencil_write_mask set_stencil_write_mask;
      struct vk_cmd_set_stencil_reference set_stencil_reference;
      struct vk_cmd_bind_descriptor_sets bind_descriptor_sets;
      struct vk_cmd_bind_index_buffer bind_index_buffer;
      struct vk_cmd_bind_vertex_buffers bind_vertex_buffers;
      struct vk_cmd_draw draw;
      struct vk_cmd_draw_indexed draw_indexed;
      struct vk_cmd_draw_multi_ext draw_multi_ext;
      struct vk_cmd_draw_multi_indexed_ext draw_multi_indexed_ext;
      struct vk_cmd_draw_indirect draw_indirect;
      struct vk_cmd_draw_indexed_indirect draw_indexed_indirect;
      struct vk_cmd_dispatch dispatch;
      struct vk_cmd_dispatch_indirect dispatch_indirect;
      struct vk_cmd_draw_cluster_huawei draw_cluster_huawei;
      struct vk_cmd_draw_cluster_indirect_huawei draw_cluster_indirect_huawei;
      struct vk_cmd_update_pipeline_indirect_buffer_nv update_pipeline_indirect_buffer_nv;
      struct vk_cmd_copy_buffer copy_buffer;
      struct vk_cmd_copy_image copy_image;
      struct vk_cmd_blit_image blit_image;
      struct vk_cmd_copy_buffer_to_image copy_buffer_to_image;
      struct vk_cmd_copy_image_to_buffer copy_image_to_buffer;
      struct vk_cmd_copy_memory_indirect_nv copy_memory_indirect_nv;
      struct vk_cmd_copy_memory_indirect_khr copy_memory_indirect_khr;
      struct vk_cmd_copy_memory_to_image_indirect_nv copy_memory_to_image_indirect_nv;
      struct vk_cmd_copy_memory_to_image_indirect_khr copy_memory_to_image_indirect_khr;
      struct vk_cmd_update_buffer update_buffer;
      struct vk_cmd_fill_buffer fill_buffer;
      struct vk_cmd_clear_color_image clear_color_image;
      struct vk_cmd_clear_depth_stencil_image clear_depth_stencil_image;
      struct vk_cmd_clear_attachments clear_attachments;
      struct vk_cmd_resolve_image resolve_image;
      struct vk_cmd_set_event set_event;
      struct vk_cmd_reset_event reset_event;
      struct vk_cmd_wait_events wait_events;
      struct vk_cmd_pipeline_barrier pipeline_barrier;
      struct vk_cmd_begin_query begin_query;
      struct vk_cmd_end_query end_query;
      struct vk_cmd_begin_conditional_rendering_ext begin_conditional_rendering_ext;
      struct vk_cmd_begin_custom_resolve_ext begin_custom_resolve_ext;
      struct vk_cmd_reset_query_pool reset_query_pool;
      struct vk_cmd_write_timestamp write_timestamp;
      struct vk_cmd_copy_query_pool_results copy_query_pool_results;
      struct vk_cmd_push_constants push_constants;
      struct vk_cmd_begin_render_pass begin_render_pass;
      struct vk_cmd_next_subpass next_subpass;
      struct vk_cmd_execute_commands execute_commands;
      struct vk_cmd_debug_marker_begin_ext debug_marker_begin_ext;
      struct vk_cmd_debug_marker_insert_ext debug_marker_insert_ext;
      struct vk_cmd_execute_generated_commands_nv execute_generated_commands_nv;
      struct vk_cmd_preprocess_generated_commands_nv preprocess_generated_commands_nv;
      struct vk_cmd_bind_pipeline_shader_group_nv bind_pipeline_shader_group_nv;
      struct vk_cmd_execute_generated_commands_ext execute_generated_commands_ext;
      struct vk_cmd_preprocess_generated_commands_ext preprocess_generated_commands_ext;
      struct vk_cmd_push_descriptor_set push_descriptor_set;
      struct vk_cmd_set_device_mask set_device_mask;
      struct vk_cmd_dispatch_base dispatch_base;
      struct vk_cmd_push_descriptor_set_with_template push_descriptor_set_with_template;
      struct vk_cmd_set_viewport_wscaling_nv set_viewport_wscaling_nv;
      struct vk_cmd_set_discard_rectangle_ext set_discard_rectangle_ext;
      struct vk_cmd_set_discard_rectangle_enable_ext set_discard_rectangle_enable_ext;
      struct vk_cmd_set_discard_rectangle_mode_ext set_discard_rectangle_mode_ext;
      struct vk_cmd_set_sample_locations_ext set_sample_locations_ext;
      struct vk_cmd_begin_debug_utils_label_ext begin_debug_utils_label_ext;
      struct vk_cmd_insert_debug_utils_label_ext insert_debug_utils_label_ext;
      struct vk_cmd_write_buffer_marker_amd write_buffer_marker_amd;
      struct vk_cmd_begin_render_pass2 begin_render_pass2;
      struct vk_cmd_next_subpass2 next_subpass2;
      struct vk_cmd_end_render_pass2 end_render_pass2;
      struct vk_cmd_draw_indirect_count draw_indirect_count;
      struct vk_cmd_draw_indexed_indirect_count draw_indexed_indirect_count;
      struct vk_cmd_set_checkpoint_nv set_checkpoint_nv;
      struct vk_cmd_bind_transform_feedback_buffers_ext bind_transform_feedback_buffers_ext;
      struct vk_cmd_begin_transform_feedback_ext begin_transform_feedback_ext;
      struct vk_cmd_end_transform_feedback_ext end_transform_feedback_ext;
      struct vk_cmd_begin_query_indexed_ext begin_query_indexed_ext;
      struct vk_cmd_end_query_indexed_ext end_query_indexed_ext;
      struct vk_cmd_draw_indirect_byte_count_ext draw_indirect_byte_count_ext;
      struct vk_cmd_set_exclusive_scissor_nv set_exclusive_scissor_nv;
      struct vk_cmd_set_exclusive_scissor_enable_nv set_exclusive_scissor_enable_nv;
      struct vk_cmd_bind_shading_rate_image_nv bind_shading_rate_image_nv;
      struct vk_cmd_set_viewport_shading_rate_palette_nv set_viewport_shading_rate_palette_nv;
      struct vk_cmd_set_coarse_sample_order_nv set_coarse_sample_order_nv;
      struct vk_cmd_draw_mesh_tasks_nv draw_mesh_tasks_nv;
      struct vk_cmd_draw_mesh_tasks_indirect_nv draw_mesh_tasks_indirect_nv;
      struct vk_cmd_draw_mesh_tasks_indirect_count_nv draw_mesh_tasks_indirect_count_nv;
      struct vk_cmd_draw_mesh_tasks_ext draw_mesh_tasks_ext;
      struct vk_cmd_draw_mesh_tasks_indirect_ext draw_mesh_tasks_indirect_ext;
      struct vk_cmd_draw_mesh_tasks_indirect_count_ext draw_mesh_tasks_indirect_count_ext;
      struct vk_cmd_bind_invocation_mask_huawei bind_invocation_mask_huawei;
      struct vk_cmd_copy_acceleration_structure_nv copy_acceleration_structure_nv;
      struct vk_cmd_copy_acceleration_structure_khr copy_acceleration_structure_khr;
      struct vk_cmd_copy_acceleration_structure_to_memory_khr copy_acceleration_structure_to_memory_khr;
      struct vk_cmd_copy_memory_to_acceleration_structure_khr copy_memory_to_acceleration_structure_khr;
      struct vk_cmd_write_acceleration_structures_properties_khr write_acceleration_structures_properties_khr;
      struct vk_cmd_write_acceleration_structures_properties_nv write_acceleration_structures_properties_nv;
      struct vk_cmd_build_acceleration_structure_nv build_acceleration_structure_nv;
      struct vk_cmd_trace_rays_khr trace_rays_khr;
      struct vk_cmd_trace_rays_nv trace_rays_nv;
      struct vk_cmd_trace_rays_indirect_khr trace_rays_indirect_khr;
      struct vk_cmd_trace_rays_indirect2_khr trace_rays_indirect2_khr;
      struct vk_cmd_build_cluster_acceleration_structure_indirect_nv build_cluster_acceleration_structure_indirect_nv;
      struct vk_cmd_set_ray_tracing_pipeline_stack_size_khr set_ray_tracing_pipeline_stack_size_khr;
      struct vk_cmd_set_performance_marker_intel set_performance_marker_intel;
      struct vk_cmd_set_performance_stream_marker_intel set_performance_stream_marker_intel;
      struct vk_cmd_set_performance_override_intel set_performance_override_intel;
      struct vk_cmd_set_line_stipple set_line_stipple;
      struct vk_cmd_build_acceleration_structures_khr build_acceleration_structures_khr;
      struct vk_cmd_build_acceleration_structures_indirect_khr build_acceleration_structures_indirect_khr;
      struct vk_cmd_set_cull_mode set_cull_mode;
      struct vk_cmd_set_front_face set_front_face;
      struct vk_cmd_set_primitive_topology set_primitive_topology;
      struct vk_cmd_set_viewport_with_count set_viewport_with_count;
      struct vk_cmd_set_scissor_with_count set_scissor_with_count;
      struct vk_cmd_bind_index_buffer2 bind_index_buffer2;
      struct vk_cmd_bind_vertex_buffers2 bind_vertex_buffers2;
      struct vk_cmd_set_depth_test_enable set_depth_test_enable;
      struct vk_cmd_set_depth_write_enable set_depth_write_enable;
      struct vk_cmd_set_depth_compare_op set_depth_compare_op;
      struct vk_cmd_set_depth_bounds_test_enable set_depth_bounds_test_enable;
      struct vk_cmd_set_stencil_test_enable set_stencil_test_enable;
      struct vk_cmd_set_stencil_op set_stencil_op;
      struct vk_cmd_set_patch_control_points_ext set_patch_control_points_ext;
      struct vk_cmd_set_rasterizer_discard_enable set_rasterizer_discard_enable;
      struct vk_cmd_set_depth_bias_enable set_depth_bias_enable;
      struct vk_cmd_set_logic_op_ext set_logic_op_ext;
      struct vk_cmd_set_primitive_restart_enable set_primitive_restart_enable;
      struct vk_cmd_set_tessellation_domain_origin_ext set_tessellation_domain_origin_ext;
      struct vk_cmd_set_depth_clamp_enable_ext set_depth_clamp_enable_ext;
      struct vk_cmd_set_polygon_mode_ext set_polygon_mode_ext;
      struct vk_cmd_set_rasterization_samples_ext set_rasterization_samples_ext;
      struct vk_cmd_set_sample_mask_ext set_sample_mask_ext;
      struct vk_cmd_set_alpha_to_coverage_enable_ext set_alpha_to_coverage_enable_ext;
      struct vk_cmd_set_alpha_to_one_enable_ext set_alpha_to_one_enable_ext;
      struct vk_cmd_set_logic_op_enable_ext set_logic_op_enable_ext;
      struct vk_cmd_set_color_blend_enable_ext set_color_blend_enable_ext;
      struct vk_cmd_set_color_blend_equation_ext set_color_blend_equation_ext;
      struct vk_cmd_set_color_write_mask_ext set_color_write_mask_ext;
      struct vk_cmd_set_rasterization_stream_ext set_rasterization_stream_ext;
      struct vk_cmd_set_conservative_rasterization_mode_ext set_conservative_rasterization_mode_ext;
      struct vk_cmd_set_extra_primitive_overestimation_size_ext set_extra_primitive_overestimation_size_ext;
      struct vk_cmd_set_depth_clip_enable_ext set_depth_clip_enable_ext;
      struct vk_cmd_set_sample_locations_enable_ext set_sample_locations_enable_ext;
      struct vk_cmd_set_color_blend_advanced_ext set_color_blend_advanced_ext;
      struct vk_cmd_set_provoking_vertex_mode_ext set_provoking_vertex_mode_ext;
      struct vk_cmd_set_line_rasterization_mode_ext set_line_rasterization_mode_ext;
      struct vk_cmd_set_line_stipple_enable_ext set_line_stipple_enable_ext;
      struct vk_cmd_set_depth_clip_negative_one_to_one_ext set_depth_clip_negative_one_to_one_ext;
      struct vk_cmd_set_viewport_wscaling_enable_nv set_viewport_wscaling_enable_nv;
      struct vk_cmd_set_viewport_swizzle_nv set_viewport_swizzle_nv;
      struct vk_cmd_set_coverage_to_color_enable_nv set_coverage_to_color_enable_nv;
      struct vk_cmd_set_coverage_to_color_location_nv set_coverage_to_color_location_nv;
      struct vk_cmd_set_coverage_modulation_mode_nv set_coverage_modulation_mode_nv;
      struct vk_cmd_set_coverage_modulation_table_enable_nv set_coverage_modulation_table_enable_nv;
      struct vk_cmd_set_coverage_modulation_table_nv set_coverage_modulation_table_nv;
      struct vk_cmd_set_shading_rate_image_enable_nv set_shading_rate_image_enable_nv;
      struct vk_cmd_set_coverage_reduction_mode_nv set_coverage_reduction_mode_nv;
      struct vk_cmd_set_representative_fragment_test_enable_nv set_representative_fragment_test_enable_nv;
      struct vk_cmd_copy_buffer2 copy_buffer2;
      struct vk_cmd_copy_image2 copy_image2;
      struct vk_cmd_blit_image2 blit_image2;
      struct vk_cmd_copy_buffer_to_image2 copy_buffer_to_image2;
      struct vk_cmd_copy_image_to_buffer2 copy_image_to_buffer2;
      struct vk_cmd_resolve_image2 resolve_image2;
      struct vk_cmd_set_fragment_shading_rate_khr set_fragment_shading_rate_khr;
      struct vk_cmd_set_fragment_shading_rate_enum_nv set_fragment_shading_rate_enum_nv;
      struct vk_cmd_set_vertex_input_ext set_vertex_input_ext;
      struct vk_cmd_set_color_write_enable_ext set_color_write_enable_ext;
      struct vk_cmd_set_event2 set_event2;
      struct vk_cmd_reset_event2 reset_event2;
      struct vk_cmd_wait_events2 wait_events2;
      struct vk_cmd_pipeline_barrier2 pipeline_barrier2;
      struct vk_cmd_write_timestamp2 write_timestamp2;
      struct vk_cmd_write_buffer_marker2_amd write_buffer_marker2_amd;
      struct vk_cmd_decode_video_khr decode_video_khr;
      struct vk_cmd_begin_video_coding_khr begin_video_coding_khr;
      struct vk_cmd_control_video_coding_khr control_video_coding_khr;
      struct vk_cmd_end_video_coding_khr end_video_coding_khr;
      struct vk_cmd_encode_video_khr encode_video_khr;
      struct vk_cmd_decompress_memory_nv decompress_memory_nv;
      struct vk_cmd_decompress_memory_indirect_count_nv decompress_memory_indirect_count_nv;
      struct vk_cmd_build_partitioned_acceleration_structures_nv build_partitioned_acceleration_structures_nv;
      struct vk_cmd_decompress_memory_ext decompress_memory_ext;
      struct vk_cmd_decompress_memory_indirect_count_ext decompress_memory_indirect_count_ext;
      struct vk_cmd_cu_launch_kernel_nvx cu_launch_kernel_nvx;
      struct vk_cmd_bind_descriptor_buffers_ext bind_descriptor_buffers_ext;
      struct vk_cmd_set_descriptor_buffer_offsets_ext set_descriptor_buffer_offsets_ext;
      struct vk_cmd_bind_descriptor_buffer_embedded_samplers_ext bind_descriptor_buffer_embedded_samplers_ext;
      struct vk_cmd_begin_rendering begin_rendering;
      struct vk_cmd_end_rendering2_khr end_rendering2_khr;
      struct vk_cmd_build_micromaps_ext build_micromaps_ext;
      struct vk_cmd_copy_micromap_ext copy_micromap_ext;
      struct vk_cmd_copy_micromap_to_memory_ext copy_micromap_to_memory_ext;
      struct vk_cmd_copy_memory_to_micromap_ext copy_memory_to_micromap_ext;
      struct vk_cmd_write_micromaps_properties_ext write_micromaps_properties_ext;
      struct vk_cmd_bind_tile_memory_qcom bind_tile_memory_qcom;
      struct vk_cmd_optical_flow_execute_nv optical_flow_execute_nv;
      struct vk_cmd_set_depth_bias2_ext set_depth_bias2_ext;
      struct vk_cmd_bind_shaders_ext bind_shaders_ext;
      struct vk_cmd_begin_gpa_session_amd begin_gpa_session_amd;
      struct vk_cmd_end_gpa_session_amd end_gpa_session_amd;
      struct vk_cmd_begin_gpa_sample_amd begin_gpa_sample_amd;
      struct vk_cmd_end_gpa_sample_amd end_gpa_sample_amd;
      struct vk_cmd_copy_gpa_session_results_amd copy_gpa_session_results_amd;
      struct vk_cmd_bind_descriptor_sets2 bind_descriptor_sets2;
      struct vk_cmd_push_constants2 push_constants2;
      struct vk_cmd_push_descriptor_set2 push_descriptor_set2;
      struct vk_cmd_push_descriptor_set_with_template2 push_descriptor_set_with_template2;
      struct vk_cmd_set_descriptor_buffer_offsets2_ext set_descriptor_buffer_offsets2_ext;
      struct vk_cmd_bind_descriptor_buffer_embedded_samplers2_ext bind_descriptor_buffer_embedded_samplers2_ext;
      struct vk_cmd_set_rendering_attachment_locations set_rendering_attachment_locations;
      struct vk_cmd_set_rendering_input_attachment_indices set_rendering_input_attachment_indices;
      struct vk_cmd_set_depth_clamp_range_ext set_depth_clamp_range_ext;
      struct vk_cmd_convert_cooperative_vector_matrix_nv convert_cooperative_vector_matrix_nv;
      struct vk_cmd_dispatch_tile_qcom dispatch_tile_qcom;
      struct vk_cmd_begin_per_tile_execution_qcom begin_per_tile_execution_qcom;
      struct vk_cmd_end_per_tile_execution_qcom end_per_tile_execution_qcom;
      struct vk_cmd_begin_shader_instrumentation_arm begin_shader_instrumentation_arm;
      struct vk_cmd_copy_tensor_arm copy_tensor_arm;
      struct vk_cmd_dispatch_data_graph_arm dispatch_data_graph_arm;
      struct vk_cmd_set_compute_occupancy_priority_nv set_compute_occupancy_priority_nv;
      struct vk_cmd_bind_sampler_heap_ext bind_sampler_heap_ext;
      struct vk_cmd_bind_resource_heap_ext bind_resource_heap_ext;
      struct vk_cmd_push_data_ext push_data_ext;
      struct vk_cmd_copy_memory_khr copy_memory_khr;
      struct vk_cmd_copy_memory_to_image_khr copy_memory_to_image_khr;
      struct vk_cmd_copy_image_to_memory_khr copy_image_to_memory_khr;
      struct vk_cmd_update_memory_khr update_memory_khr;
      struct vk_cmd_fill_memory_khr fill_memory_khr;
      struct vk_cmd_copy_query_pool_results_to_memory_khr copy_query_pool_results_to_memory_khr;
      struct vk_cmd_begin_conditional_rendering2_ext begin_conditional_rendering2_ext;
      struct vk_cmd_bind_transform_feedback_buffers2_ext bind_transform_feedback_buffers2_ext;
      struct vk_cmd_begin_transform_feedback2_ext begin_transform_feedback2_ext;
      struct vk_cmd_end_transform_feedback2_ext end_transform_feedback2_ext;
      struct vk_cmd_draw_indirect_byte_count2_ext draw_indirect_byte_count2_ext;
      struct vk_cmd_write_marker_to_memory_amd write_marker_to_memory_amd;
      struct vk_cmd_bind_index_buffer3_khr bind_index_buffer3_khr;
      struct vk_cmd_bind_vertex_buffers3_khr bind_vertex_buffers3_khr;
      struct vk_cmd_draw_indirect2_khr draw_indirect2_khr;
      struct vk_cmd_draw_indexed_indirect2_khr draw_indexed_indirect2_khr;
      struct vk_cmd_draw_indirect_count2_khr draw_indirect_count2_khr;
      struct vk_cmd_draw_indexed_indirect_count2_khr draw_indexed_indirect_count2_khr;
      struct vk_cmd_draw_mesh_tasks_indirect2_ext draw_mesh_tasks_indirect2_ext;
      struct vk_cmd_draw_mesh_tasks_indirect_count2_ext draw_mesh_tasks_indirect_count2_ext;
      struct vk_cmd_dispatch_indirect2_khr dispatch_indirect2_khr;
      struct vk_cmd_set_dispatch_parameters_arm set_dispatch_parameters_arm;
   } u;
};

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_pipeline(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipeline pipeline
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_restart_index_ext(struct vk_cmd_queue *queue
   , uint32_t primitiveRestartIndex
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_attachment_feedback_loop_enable_ext(struct vk_cmd_queue *queue
   , VkImageAspectFlags aspectMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport(struct vk_cmd_queue *queue
   , uint32_t firstViewport
   , uint32_t viewportCount
   , const VkViewport* pViewports
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_scissor(struct vk_cmd_queue *queue
   , uint32_t firstScissor
   , uint32_t scissorCount
   , const VkRect2D* pScissors
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_width(struct vk_cmd_queue *queue
   , float lineWidth
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias(struct vk_cmd_queue *queue
   , float depthBiasConstantFactor
   , float depthBiasClamp
   , float depthBiasSlopeFactor
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_blend_constants(struct vk_cmd_queue *queue
   , const float blendConstants[4]
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bounds(struct vk_cmd_queue *queue
   , float minDepthBounds
   , float maxDepthBounds
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_compare_mask(struct vk_cmd_queue *queue
   , VkStencilFaceFlags faceMask
   , uint32_t compareMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_write_mask(struct vk_cmd_queue *queue
   , VkStencilFaceFlags faceMask
   , uint32_t writeMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_reference(struct vk_cmd_queue *queue
   , VkStencilFaceFlags faceMask
   , uint32_t reference
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_sets(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipelineLayout layout
   , uint32_t firstSet
   , uint32_t descriptorSetCount
   , const VkDescriptorSet* pDescriptorSets
   , uint32_t dynamicOffsetCount
   , const uint32_t* pDynamicOffsets
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkIndexType indexType
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers(struct vk_cmd_queue *queue
   , uint32_t firstBinding
   , uint32_t bindingCount
   , const VkBuffer* pBuffers
   , const VkDeviceSize* pOffsets
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw(struct vk_cmd_queue *queue
   , uint32_t vertexCount
   , uint32_t instanceCount
   , uint32_t firstVertex
   , uint32_t firstInstance
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed(struct vk_cmd_queue *queue
   , uint32_t indexCount
   , uint32_t instanceCount
   , uint32_t firstIndex
   , int32_t vertexOffset
   , uint32_t firstInstance
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , uint32_t drawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , uint32_t drawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch(struct vk_cmd_queue *queue
   , uint32_t groupCountX
   , uint32_t groupCountY
   , uint32_t groupCountZ
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_indirect(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_subpass_shading_huawei(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_cluster_huawei(struct vk_cmd_queue *queue
   , uint32_t groupCountX
   , uint32_t groupCountY
   , uint32_t groupCountZ
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_cluster_indirect_huawei(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_update_pipeline_indirect_buffer_nv(struct vk_cmd_queue *queue
   , VkPipelineBindPoint           pipelineBindPoint
   , VkPipeline                    pipeline
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer(struct vk_cmd_queue *queue
   , VkBuffer srcBuffer
   , VkBuffer dstBuffer
   , uint32_t regionCount
   , const VkBufferCopy* pRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image(struct vk_cmd_queue *queue
   , VkImage srcImage
   , VkImageLayout srcImageLayout
   , VkImage dstImage
   , VkImageLayout dstImageLayout
   , uint32_t regionCount
   , const VkImageCopy* pRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_blit_image(struct vk_cmd_queue *queue
   , VkImage srcImage
   , VkImageLayout srcImageLayout
   , VkImage dstImage
   , VkImageLayout dstImageLayout
   , uint32_t regionCount
   , const VkImageBlit* pRegions
   , VkFilter filter
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer_to_image(struct vk_cmd_queue *queue
   , VkBuffer srcBuffer
   , VkImage dstImage
   , VkImageLayout dstImageLayout
   , uint32_t regionCount
   , const VkBufferImageCopy* pRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_buffer(struct vk_cmd_queue *queue
   , VkImage srcImage
   , VkImageLayout srcImageLayout
   , VkBuffer dstBuffer
   , uint32_t regionCount
   , const VkBufferImageCopy* pRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_indirect_nv(struct vk_cmd_queue *queue
   , VkDeviceAddress copyBufferAddress
   , uint32_t copyCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_indirect_khr(struct vk_cmd_queue *queue
   , const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_indirect_nv(struct vk_cmd_queue *queue
   , VkDeviceAddress copyBufferAddress
   , uint32_t copyCount
   , uint32_t stride
   , VkImage dstImage
   , VkImageLayout dstImageLayout
   , const VkImageSubresourceLayers* pImageSubresources
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_indirect_khr(struct vk_cmd_queue *queue
   , const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_update_buffer(struct vk_cmd_queue *queue
   , VkBuffer dstBuffer
   , VkDeviceSize dstOffset
   , VkDeviceSize dataSize
   , const void* pData
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_fill_buffer(struct vk_cmd_queue *queue
   , VkBuffer dstBuffer
   , VkDeviceSize dstOffset
   , VkDeviceSize size
   , uint32_t data
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_color_image(struct vk_cmd_queue *queue
   , VkImage image
   , VkImageLayout imageLayout
   , const VkClearColorValue* pColor
   , uint32_t rangeCount
   , const VkImageSubresourceRange* pRanges
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_depth_stencil_image(struct vk_cmd_queue *queue
   , VkImage image
   , VkImageLayout imageLayout
   , const VkClearDepthStencilValue* pDepthStencil
   , uint32_t rangeCount
   , const VkImageSubresourceRange* pRanges
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_clear_attachments(struct vk_cmd_queue *queue
   , uint32_t attachmentCount
   , const VkClearAttachment* pAttachments
   , uint32_t rectCount
   , const VkClearRect* pRects
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_resolve_image(struct vk_cmd_queue *queue
   , VkImage srcImage
   , VkImageLayout srcImageLayout
   , VkImage dstImage
   , VkImageLayout dstImageLayout
   , uint32_t regionCount
   , const VkImageResolve* pRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_event(struct vk_cmd_queue *queue
   , VkEvent event
   , VkPipelineStageFlags stageMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_event(struct vk_cmd_queue *queue
   , VkEvent event
   , VkPipelineStageFlags stageMask
  );

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
  );

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
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_query(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t query
   , VkQueryControlFlags flags
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_query(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t query
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_conditional_rendering_ext(struct vk_cmd_queue *queue
   , const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_conditional_rendering_ext(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_custom_resolve_ext(struct vk_cmd_queue *queue
   , const VkBeginCustomResolveInfoEXT* pBeginCustomResolveInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_query_pool(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t firstQuery
   , uint32_t queryCount
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_timestamp(struct vk_cmd_queue *queue
   , VkPipelineStageFlagBits pipelineStage
   , VkQueryPool queryPool
   , uint32_t query
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_query_pool_results(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t firstQuery
   , uint32_t queryCount
   , VkBuffer dstBuffer
   , VkDeviceSize dstOffset
   , VkDeviceSize stride
   , VkQueryResultFlags flags
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_constants(struct vk_cmd_queue *queue
   , VkPipelineLayout layout
   , VkShaderStageFlags stageFlags
   , uint32_t offset
   , uint32_t size
   , const void* pValues
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_render_pass(struct vk_cmd_queue *queue
   , const VkRenderPassBeginInfo* pRenderPassBegin
   , VkSubpassContents contents
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_next_subpass(struct vk_cmd_queue *queue
   , VkSubpassContents contents
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_render_pass(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_commands(struct vk_cmd_queue *queue
   , uint32_t commandBufferCount
   , const VkCommandBuffer* pCommandBuffers
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_begin_ext(struct vk_cmd_queue *queue
   , const VkDebugMarkerMarkerInfoEXT* pMarkerInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_end_ext(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_debug_marker_insert_ext(struct vk_cmd_queue *queue
   , const VkDebugMarkerMarkerInfoEXT* pMarkerInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_generated_commands_nv(struct vk_cmd_queue *queue
   , VkBool32 isPreprocessed
   , const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_preprocess_generated_commands_nv(struct vk_cmd_queue *queue
   , const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_pipeline_shader_group_nv(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipeline pipeline
   , uint32_t groupIndex
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_execute_generated_commands_ext(struct vk_cmd_queue *queue
   , VkBool32 isPreprocessed
   , const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_preprocess_generated_commands_ext(struct vk_cmd_queue *queue
   , const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo
   , VkCommandBuffer stateCommandBuffer
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipelineLayout layout
   , uint32_t set
   , uint32_t descriptorWriteCount
   , const VkWriteDescriptorSet* pDescriptorWrites
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_device_mask(struct vk_cmd_queue *queue
   , uint32_t deviceMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_base(struct vk_cmd_queue *queue
   , uint32_t baseGroupX
   , uint32_t baseGroupY
   , uint32_t baseGroupZ
   , uint32_t groupCountX
   , uint32_t groupCountY
   , uint32_t groupCountZ
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set_with_template(struct vk_cmd_queue *queue
   , VkDescriptorUpdateTemplate descriptorUpdateTemplate
   , VkPipelineLayout layout
   , uint32_t set
   , const void* pData
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_wscaling_nv(struct vk_cmd_queue *queue
   , uint32_t firstViewport
   , uint32_t viewportCount
   , const VkViewportWScalingNV* pViewportWScalings
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_ext(struct vk_cmd_queue *queue
   , uint32_t firstDiscardRectangle
   , uint32_t discardRectangleCount
   , const VkRect2D* pDiscardRectangles
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 discardRectangleEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_discard_rectangle_mode_ext(struct vk_cmd_queue *queue
   , VkDiscardRectangleModeEXT discardRectangleMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_locations_ext(struct vk_cmd_queue *queue
   , const VkSampleLocationsInfoEXT* pSampleLocationsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_debug_utils_label_ext(struct vk_cmd_queue *queue
   , const VkDebugUtilsLabelEXT* pLabelInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_debug_utils_label_ext(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_insert_debug_utils_label_ext(struct vk_cmd_queue *queue
   , const VkDebugUtilsLabelEXT* pLabelInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_buffer_marker_amd(struct vk_cmd_queue *queue
   , VkPipelineStageFlagBits pipelineStage
   , VkBuffer dstBuffer
   , VkDeviceSize dstOffset
   , uint32_t marker
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_render_pass2(struct vk_cmd_queue *queue
   , const VkRenderPassBeginInfo*      pRenderPassBegin
   , const VkSubpassBeginInfo*      pSubpassBeginInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_next_subpass2(struct vk_cmd_queue *queue
   , const VkSubpassBeginInfo*      pSubpassBeginInfo
   , const VkSubpassEndInfo*        pSubpassEndInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_render_pass2(struct vk_cmd_queue *queue
   , const VkSubpassEndInfo*        pSubpassEndInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_count(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkBuffer countBuffer
   , VkDeviceSize countBufferOffset
   , uint32_t maxDrawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect_count(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkBuffer countBuffer
   , VkDeviceSize countBufferOffset
   , uint32_t maxDrawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_checkpoint_nv(struct vk_cmd_queue *queue
   , const void* pCheckpointMarker
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_transform_feedback_buffers_ext(struct vk_cmd_queue *queue
   , uint32_t firstBinding
   , uint32_t bindingCount
   , const VkBuffer* pBuffers
   , const VkDeviceSize* pOffsets
   , const VkDeviceSize* pSizes
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_transform_feedback_ext(struct vk_cmd_queue *queue
   , uint32_t firstCounterBuffer
   , uint32_t counterBufferCount
   , const VkBuffer* pCounterBuffers
   , const VkDeviceSize* pCounterBufferOffsets
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_transform_feedback_ext(struct vk_cmd_queue *queue
   , uint32_t firstCounterBuffer
   , uint32_t counterBufferCount
   , const VkBuffer* pCounterBuffers
   , const VkDeviceSize* pCounterBufferOffsets
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_query_indexed_ext(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t query
   , VkQueryControlFlags flags
   , uint32_t index
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_query_indexed_ext(struct vk_cmd_queue *queue
   , VkQueryPool queryPool
   , uint32_t query
   , uint32_t index
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_byte_count_ext(struct vk_cmd_queue *queue
   , uint32_t instanceCount
   , uint32_t firstInstance
   , VkBuffer counterBuffer
   , VkDeviceSize counterBufferOffset
   , uint32_t counterOffset
   , uint32_t vertexStride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_exclusive_scissor_nv(struct vk_cmd_queue *queue
   , uint32_t firstExclusiveScissor
   , uint32_t exclusiveScissorCount
   , const VkRect2D* pExclusiveScissors
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_exclusive_scissor_enable_nv(struct vk_cmd_queue *queue
   , uint32_t firstExclusiveScissor
   , uint32_t exclusiveScissorCount
   , const VkBool32* pExclusiveScissorEnables
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_shading_rate_image_nv(struct vk_cmd_queue *queue
   , VkImageView imageView
   , VkImageLayout imageLayout
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_shading_rate_palette_nv(struct vk_cmd_queue *queue
   , uint32_t firstViewport
   , uint32_t viewportCount
   , const VkShadingRatePaletteNV* pShadingRatePalettes
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coarse_sample_order_nv(struct vk_cmd_queue *queue
   , VkCoarseSampleOrderTypeNV sampleOrderType
   , uint32_t customSampleOrderCount
   , const VkCoarseSampleOrderCustomNV* pCustomSampleOrders
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_nv(struct vk_cmd_queue *queue
   , uint32_t taskCount
   , uint32_t firstTask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_nv(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , uint32_t drawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count_nv(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkBuffer countBuffer
   , VkDeviceSize countBufferOffset
   , uint32_t maxDrawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_ext(struct vk_cmd_queue *queue
   , uint32_t groupCountX
   , uint32_t groupCountY
   , uint32_t groupCountZ
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_ext(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , uint32_t drawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count_ext(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkBuffer countBuffer
   , VkDeviceSize countBufferOffset
   , uint32_t maxDrawCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_invocation_mask_huawei(struct vk_cmd_queue *queue
   , VkImageView imageView
   , VkImageLayout imageLayout
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_nv(struct vk_cmd_queue *queue
   , VkAccelerationStructureNV dst
   , VkAccelerationStructureNV src
   , VkCopyAccelerationStructureModeKHR mode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_khr(struct vk_cmd_queue *queue
   , const VkCopyAccelerationStructureInfoKHR* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_acceleration_structure_to_memory_khr(struct vk_cmd_queue *queue
   , const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_acceleration_structure_khr(struct vk_cmd_queue *queue
   , const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_acceleration_structures_properties_khr(struct vk_cmd_queue *queue
   , uint32_t accelerationStructureCount
   , const VkAccelerationStructureKHR* pAccelerationStructures
   , VkQueryType queryType
   , VkQueryPool queryPool
   , uint32_t firstQuery
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_acceleration_structures_properties_nv(struct vk_cmd_queue *queue
   , uint32_t accelerationStructureCount
   , const VkAccelerationStructureNV* pAccelerationStructures
   , VkQueryType queryType
   , VkQueryPool queryPool
   , uint32_t firstQuery
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_build_acceleration_structure_nv(struct vk_cmd_queue *queue
   , const VkAccelerationStructureInfoNV* pInfo
   , VkBuffer instanceData
   , VkDeviceSize instanceOffset
   , VkBool32 update
   , VkAccelerationStructureNV dst
   , VkAccelerationStructureNV src
   , VkBuffer scratch
   , VkDeviceSize scratchOffset
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_khr(struct vk_cmd_queue *queue
   , const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable
   , uint32_t width
   , uint32_t height
   , uint32_t depth
  );

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
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_indirect_khr(struct vk_cmd_queue *queue
   , const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable
   , const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable
   , VkDeviceAddress indirectDeviceAddress
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_trace_rays_indirect2_khr(struct vk_cmd_queue *queue
   , VkDeviceAddress indirectDeviceAddress
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_build_cluster_acceleration_structure_indirect_nv(struct vk_cmd_queue *queue
   , const VkClusterAccelerationStructureCommandsInfoNV*  pCommandInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_ray_tracing_pipeline_stack_size_khr(struct vk_cmd_queue *queue
   , uint32_t pipelineStackSize
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_stipple(struct vk_cmd_queue *queue
   , uint32_t lineStippleFactor
   , uint16_t lineStipplePattern
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_cull_mode(struct vk_cmd_queue *queue
   , VkCullModeFlags cullMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_front_face(struct vk_cmd_queue *queue
   , VkFrontFace frontFace
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_topology(struct vk_cmd_queue *queue
   , VkPrimitiveTopology primitiveTopology
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_with_count(struct vk_cmd_queue *queue
   , uint32_t viewportCount
   , const VkViewport* pViewports
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_scissor_with_count(struct vk_cmd_queue *queue
   , uint32_t scissorCount
   , const VkRect2D* pScissors
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer2(struct vk_cmd_queue *queue
   , VkBuffer buffer
   , VkDeviceSize offset
   , VkDeviceSize size
   , VkIndexType indexType
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers2(struct vk_cmd_queue *queue
   , uint32_t firstBinding
   , uint32_t bindingCount
   , const VkBuffer* pBuffers
   , const VkDeviceSize* pOffsets
   , const VkDeviceSize* pSizes
   , const VkDeviceSize* pStrides
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_test_enable(struct vk_cmd_queue *queue
   , VkBool32 depthTestEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_write_enable(struct vk_cmd_queue *queue
   , VkBool32 depthWriteEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_compare_op(struct vk_cmd_queue *queue
   , VkCompareOp depthCompareOp
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bounds_test_enable(struct vk_cmd_queue *queue
   , VkBool32 depthBoundsTestEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_test_enable(struct vk_cmd_queue *queue
   , VkBool32 stencilTestEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_stencil_op(struct vk_cmd_queue *queue
   , VkStencilFaceFlags faceMask
   , VkStencilOp failOp
   , VkStencilOp passOp
   , VkStencilOp depthFailOp
   , VkCompareOp compareOp
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_patch_control_points_ext(struct vk_cmd_queue *queue
   , uint32_t patchControlPoints
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterizer_discard_enable(struct vk_cmd_queue *queue
   , VkBool32 rasterizerDiscardEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias_enable(struct vk_cmd_queue *queue
   , VkBool32 depthBiasEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_logic_op_ext(struct vk_cmd_queue *queue
   , VkLogicOp logicOp
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_primitive_restart_enable(struct vk_cmd_queue *queue
   , VkBool32 primitiveRestartEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_tessellation_domain_origin_ext(struct vk_cmd_queue *queue
   , VkTessellationDomainOrigin domainOrigin
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clamp_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 depthClampEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_polygon_mode_ext(struct vk_cmd_queue *queue
   , VkPolygonMode polygonMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterization_samples_ext(struct vk_cmd_queue *queue
   , VkSampleCountFlagBits  rasterizationSamples
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_mask_ext(struct vk_cmd_queue *queue
   , VkSampleCountFlagBits  samples
   , const VkSampleMask*    pSampleMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_alpha_to_coverage_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 alphaToCoverageEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_alpha_to_one_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 alphaToOneEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_logic_op_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 logicOpEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_enable_ext(struct vk_cmd_queue *queue
   , uint32_t firstAttachment
   , uint32_t attachmentCount
   , const VkBool32* pColorBlendEnables
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_equation_ext(struct vk_cmd_queue *queue
   , uint32_t firstAttachment
   , uint32_t attachmentCount
   , const VkColorBlendEquationEXT* pColorBlendEquations
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_write_mask_ext(struct vk_cmd_queue *queue
   , uint32_t firstAttachment
   , uint32_t attachmentCount
   , const VkColorComponentFlags* pColorWriteMasks
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rasterization_stream_ext(struct vk_cmd_queue *queue
   , uint32_t rasterizationStream
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_conservative_rasterization_mode_ext(struct vk_cmd_queue *queue
   , VkConservativeRasterizationModeEXT conservativeRasterizationMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_extra_primitive_overestimation_size_ext(struct vk_cmd_queue *queue
   , float extraPrimitiveOverestimationSize
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clip_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 depthClipEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_sample_locations_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 sampleLocationsEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_blend_advanced_ext(struct vk_cmd_queue *queue
   , uint32_t firstAttachment
   , uint32_t attachmentCount
   , const VkColorBlendAdvancedEXT* pColorBlendAdvanced
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_provoking_vertex_mode_ext(struct vk_cmd_queue *queue
   , VkProvokingVertexModeEXT provokingVertexMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_rasterization_mode_ext(struct vk_cmd_queue *queue
   , VkLineRasterizationModeEXT lineRasterizationMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_line_stipple_enable_ext(struct vk_cmd_queue *queue
   , VkBool32 stippledLineEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clip_negative_one_to_one_ext(struct vk_cmd_queue *queue
   , VkBool32 negativeOneToOne
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_wscaling_enable_nv(struct vk_cmd_queue *queue
   , VkBool32 viewportWScalingEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_viewport_swizzle_nv(struct vk_cmd_queue *queue
   , uint32_t firstViewport
   , uint32_t viewportCount
   , const VkViewportSwizzleNV* pViewportSwizzles
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_to_color_enable_nv(struct vk_cmd_queue *queue
   , VkBool32 coverageToColorEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_to_color_location_nv(struct vk_cmd_queue *queue
   , uint32_t coverageToColorLocation
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_mode_nv(struct vk_cmd_queue *queue
   , VkCoverageModulationModeNV coverageModulationMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_table_enable_nv(struct vk_cmd_queue *queue
   , VkBool32 coverageModulationTableEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_modulation_table_nv(struct vk_cmd_queue *queue
   , uint32_t coverageModulationTableCount
   , const float* pCoverageModulationTable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_shading_rate_image_enable_nv(struct vk_cmd_queue *queue
   , VkBool32 shadingRateImageEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_coverage_reduction_mode_nv(struct vk_cmd_queue *queue
   , VkCoverageReductionModeNV coverageReductionMode
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_representative_fragment_test_enable_nv(struct vk_cmd_queue *queue
   , VkBool32 representativeFragmentTestEnable
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer2(struct vk_cmd_queue *queue
   , const VkCopyBufferInfo2* pCopyBufferInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image2(struct vk_cmd_queue *queue
   , const VkCopyImageInfo2* pCopyImageInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_blit_image2(struct vk_cmd_queue *queue
   , const VkBlitImageInfo2* pBlitImageInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_buffer_to_image2(struct vk_cmd_queue *queue
   , const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_buffer2(struct vk_cmd_queue *queue
   , const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_resolve_image2(struct vk_cmd_queue *queue
   , const VkResolveImageInfo2* pResolveImageInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_fragment_shading_rate_khr(struct vk_cmd_queue *queue
   , const VkExtent2D*                           pFragmentSize
   , const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_fragment_shading_rate_enum_nv(struct vk_cmd_queue *queue
   , VkFragmentShadingRateNV                     shadingRate
   , const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_vertex_input_ext(struct vk_cmd_queue *queue
   , uint32_t vertexBindingDescriptionCount
   , const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions
   , uint32_t vertexAttributeDescriptionCount
   , const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_color_write_enable_ext(struct vk_cmd_queue *queue
   , uint32_t                                attachmentCount
   , const VkBool32*   pColorWriteEnables
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_event2(struct vk_cmd_queue *queue
   , VkEvent                                             event
   , const VkDependencyInfo*                             pDependencyInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_reset_event2(struct vk_cmd_queue *queue
   , VkEvent                                             event
   , VkPipelineStageFlags2               stageMask
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_wait_events2(struct vk_cmd_queue *queue
   , uint32_t                                            eventCount
   , const VkEvent*                     pEvents
   , const VkDependencyInfo*            pDependencyInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_pipeline_barrier2(struct vk_cmd_queue *queue
   , const VkDependencyInfo*                             pDependencyInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_timestamp2(struct vk_cmd_queue *queue
   , VkPipelineStageFlags2               stage
   , VkQueryPool                                         queryPool
   , uint32_t                                            query
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_buffer_marker2_amd(struct vk_cmd_queue *queue
   , VkPipelineStageFlags2               stage
   , VkBuffer                                            dstBuffer
   , VkDeviceSize                                        dstOffset
   , uint32_t                                            marker
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_decode_video_khr(struct vk_cmd_queue *queue
   , const VkVideoDecodeInfoKHR* pDecodeInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_video_coding_khr(struct vk_cmd_queue *queue
   , const VkVideoBeginCodingInfoKHR* pBeginInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_control_video_coding_khr(struct vk_cmd_queue *queue
   , const VkVideoCodingControlInfoKHR* pCodingControlInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_video_coding_khr(struct vk_cmd_queue *queue
   , const VkVideoEndCodingInfoKHR* pEndCodingInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_encode_video_khr(struct vk_cmd_queue *queue
   , const VkVideoEncodeInfoKHR* pEncodeInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_nv(struct vk_cmd_queue *queue
   , uint32_t decompressRegionCount
   , const VkDecompressMemoryRegionNV* pDecompressMemoryRegions
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_indirect_count_nv(struct vk_cmd_queue *queue
   , VkDeviceAddress indirectCommandsAddress
   , VkDeviceAddress indirectCommandsCountAddress
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_build_partitioned_acceleration_structures_nv(struct vk_cmd_queue *queue
   , const VkBuildPartitionedAccelerationStructureInfoNV*  pBuildInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_ext(struct vk_cmd_queue *queue
   , const VkDecompressMemoryInfoEXT* pDecompressMemoryInfoEXT
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_decompress_memory_indirect_count_ext(struct vk_cmd_queue *queue
   , VkMemoryDecompressionMethodFlagsEXT decompressionMethod
   , VkDeviceAddress indirectCommandsAddress
   , VkDeviceAddress indirectCommandsCountAddress
   , uint32_t maxDecompressionCount
   , uint32_t stride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_cu_launch_kernel_nvx(struct vk_cmd_queue *queue
   , const VkCuLaunchInfoNVX* pLaunchInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffers_ext(struct vk_cmd_queue *queue
   , uint32_t bufferCount
   , const VkDescriptorBufferBindingInfoEXT* pBindingInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_descriptor_buffer_offsets_ext(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipelineLayout layout
   , uint32_t firstSet
   , uint32_t setCount
   , const uint32_t* pBufferIndices
   , const VkDeviceSize* pOffsets
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers_ext(struct vk_cmd_queue *queue
   , VkPipelineBindPoint pipelineBindPoint
   , VkPipelineLayout layout
   , uint32_t set
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_rendering(struct vk_cmd_queue *queue
   , const VkRenderingInfo*                              pRenderingInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_rendering(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_rendering2_khr(struct vk_cmd_queue *queue
   , const VkRenderingEndInfoKHR*        pRenderingEndInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_build_micromaps_ext(struct vk_cmd_queue *queue
   , uint32_t                                      infoCount
   , const VkMicromapBuildInfoEXT* pInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_micromap_ext(struct vk_cmd_queue *queue
   , const VkCopyMicromapInfoEXT*      pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_micromap_to_memory_ext(struct vk_cmd_queue *queue
   , const VkCopyMicromapToMemoryInfoEXT* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_micromap_ext(struct vk_cmd_queue *queue
   , const VkCopyMemoryToMicromapInfoEXT* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_micromaps_properties_ext(struct vk_cmd_queue *queue
   , uint32_t                                 micromapCount
   , const VkMicromapEXT* pMicromaps
   , VkQueryType        queryType
   , VkQueryPool                              queryPool
   , uint32_t                                 firstQuery
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_tile_memory_qcom(struct vk_cmd_queue *queue
   , const VkTileMemoryBindInfoQCOM* pTileMemoryBindInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_optical_flow_execute_nv(struct vk_cmd_queue *queue
   , VkOpticalFlowSessionNV session
   , const VkOpticalFlowExecuteInfoNV* pExecuteInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_bias2_ext(struct vk_cmd_queue *queue
   , const VkDepthBiasInfoEXT*         pDepthBiasInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_shaders_ext(struct vk_cmd_queue *queue
   , uint32_t stageCount
   , const VkShaderStageFlagBits* pStages
   , const VkShaderEXT* pShaders
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_gpa_sample_amd(struct vk_cmd_queue *queue
   , VkGpaSessionAMD                   gpaSession
   , uint32_t                          sampleID
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_gpa_session_results_amd(struct vk_cmd_queue *queue
   , VkGpaSessionAMD                   gpaSession
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_sets2(struct vk_cmd_queue *queue
   , const VkBindDescriptorSetsInfo*   pBindDescriptorSetsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_constants2(struct vk_cmd_queue *queue
   , const VkPushConstantsInfo*        pPushConstantsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set2(struct vk_cmd_queue *queue
   , const VkPushDescriptorSetInfo*    pPushDescriptorSetInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_descriptor_set_with_template2(struct vk_cmd_queue *queue
   , const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_descriptor_buffer_offsets2_ext(struct vk_cmd_queue *queue
   , const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_descriptor_buffer_embedded_samplers2_ext(struct vk_cmd_queue *queue
   , const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rendering_attachment_locations(struct vk_cmd_queue *queue
   , const VkRenderingAttachmentLocationInfo* pLocationInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_rendering_input_attachment_indices(struct vk_cmd_queue *queue
   , const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_depth_clamp_range_ext(struct vk_cmd_queue *queue
   , VkDepthClampModeEXT depthClampMode
   , const VkDepthClampRangeEXT* pDepthClampRange
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_convert_cooperative_vector_matrix_nv(struct vk_cmd_queue *queue
   , uint32_t infoCount
   , const VkConvertCooperativeVectorMatrixInfoNV* pInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_tile_qcom(struct vk_cmd_queue *queue
   , const VkDispatchTileInfoQCOM* pDispatchTileInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_per_tile_execution_qcom(struct vk_cmd_queue *queue
   , const VkPerTileBeginInfoQCOM* pPerTileBeginInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_per_tile_execution_qcom(struct vk_cmd_queue *queue
   , const VkPerTileEndInfoQCOM* pPerTileEndInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_shader_instrumentation_arm(struct vk_cmd_queue *queue
   , VkShaderInstrumentationARM instrumentation
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_shader_instrumentation_arm(struct vk_cmd_queue *queue
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_tensor_arm(struct vk_cmd_queue *queue
   , const VkCopyTensorInfoARM* pCopyTensorInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_data_graph_arm(struct vk_cmd_queue *queue
   , VkDataGraphPipelineSessionARM session
   , const VkDataGraphPipelineDispatchInfoARM* pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_compute_occupancy_priority_nv(struct vk_cmd_queue *queue
   , const VkComputeOccupancyPriorityParametersNV* pParameters
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_sampler_heap_ext(struct vk_cmd_queue *queue
   , const VkBindHeapInfoEXT*                            pBindInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_resource_heap_ext(struct vk_cmd_queue *queue
   , const VkBindHeapInfoEXT*                            pBindInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_push_data_ext(struct vk_cmd_queue *queue
   , const VkPushDataInfoEXT*                            pPushDataInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_khr(struct vk_cmd_queue *queue
   , const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_memory_to_image_khr(struct vk_cmd_queue *queue
   , const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_image_to_memory_khr(struct vk_cmd_queue *queue
   , const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_update_memory_khr(struct vk_cmd_queue *queue
   , const VkDeviceAddressRangeKHR*      pDstRange
   , VkAddressCommandFlagsKHR dstFlags
   , VkDeviceSize                        dataSize
   , const void*          pData
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_fill_memory_khr(struct vk_cmd_queue *queue
   , const VkDeviceAddressRangeKHR*      pDstRange
   , VkAddressCommandFlagsKHR dstFlags
   , uint32_t                            data
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_copy_query_pool_results_to_memory_khr(struct vk_cmd_queue *queue
   , VkQueryPool                         queryPool
   , uint32_t                            firstQuery
   , uint32_t                            queryCount
   , const VkStridedDeviceAddressRangeKHR* pDstRange
   , VkAddressCommandFlagsKHR    dstFlags
   , VkQueryResultFlags  queryResultFlags
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_conditional_rendering2_ext(struct vk_cmd_queue *queue
   , const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_transform_feedback_buffers2_ext(struct vk_cmd_queue *queue
   , uint32_t                            firstBinding
   , uint32_t                            bindingCount
   , const VkBindTransformFeedbackBuffer2InfoEXT* pBindingInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_begin_transform_feedback2_ext(struct vk_cmd_queue *queue
   , uint32_t                            firstCounterRange
   , uint32_t            counterRangeCount
   , const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_end_transform_feedback2_ext(struct vk_cmd_queue *queue
   , uint32_t                            firstCounterRange
   , uint32_t            counterRangeCount
   , const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_byte_count2_ext(struct vk_cmd_queue *queue
   , uint32_t instanceCount
   , uint32_t firstInstance
   , const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo
   , uint32_t counterOffset
   , uint32_t vertexStride
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_write_marker_to_memory_amd(struct vk_cmd_queue *queue
   , const VkMemoryMarkerInfoAMD*         pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_index_buffer3_khr(struct vk_cmd_queue *queue
   , const VkBindIndexBuffer3InfoKHR*    pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_bind_vertex_buffers3_khr(struct vk_cmd_queue *queue
   , uint32_t                            firstBinding
   , uint32_t                            bindingCount
   , const VkBindVertexBuffer3InfoKHR* pBindingInfos
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect2_khr(struct vk_cmd_queue *queue
   , const VkDrawIndirect2InfoKHR*       pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect2_khr(struct vk_cmd_queue *queue
   , const VkDrawIndirect2InfoKHR*       pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indirect_count2_khr(struct vk_cmd_queue *queue
   , const VkDrawIndirectCount2InfoKHR*  pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_indexed_indirect_count2_khr(struct vk_cmd_queue *queue
   , const VkDrawIndirectCount2InfoKHR*  pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect2_ext(struct vk_cmd_queue *queue
   , const VkDrawIndirect2InfoKHR*       pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_draw_mesh_tasks_indirect_count2_ext(struct vk_cmd_queue *queue
   , const VkDrawIndirectCount2InfoKHR*  pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_dispatch_indirect2_khr(struct vk_cmd_queue *queue
   , const VkDispatchIndirect2InfoKHR*   pInfo
  );

  struct vk_cmd_queue_entry *vk_enqueue_cmd_set_dispatch_parameters_arm(struct vk_cmd_queue *queue
   , const VkDispatchParametersARM*    pDispatchParameters
  );


void vk_free_queue(struct vk_cmd_queue *queue);

static inline void
vk_cmd_queue_init(struct vk_cmd_queue *queue)
{
   linear_opts opts = {
      .min_buffer_size = 64 * 1024
   };
   queue->ctx = linear_context_with_opts(NULL, &opts);
   list_inithead(&queue->cmds);
   util_dynarray_init(&queue->pipeline_layouts, NULL);
   util_dynarray_init(&queue->update_templates, NULL);
   util_dynarray_init(&queue->set_layouts, NULL);
}

static inline void
vk_cmd_queue_reset(struct vk_cmd_queue *queue)
{
   vk_free_queue(queue);
   vk_cmd_queue_init(queue);
}

static inline void
vk_cmd_queue_finish(struct vk_cmd_queue *queue)
{
   vk_free_queue(queue);
}

void vk_cmd_queue_execute(struct vk_cmd_queue *queue,
                          VkCommandBuffer commandBuffer,
                          const struct vk_device_dispatch_table *disp);

#ifdef __cplusplus
}
#endif
