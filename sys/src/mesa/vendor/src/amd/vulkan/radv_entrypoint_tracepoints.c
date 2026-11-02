/* Copyright © 2026 Valve Corporation
 * SPDX-License-Identifier: MIT
 */

#include "radv_cmd_buffer.h"
#include "radv_entrypoints.h"
#include "radv_tracepoints.h"

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDraw(
   VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_Draw(cmd_buffer->utrace.trace, cmd_buffer, vertexCount, instanceCount);
   
   device->layer_dispatch.utrace.CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_Draw(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndexed(
   VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndexed(cmd_buffer->utrace.trace, cmd_buffer, indexCount, instanceCount);
   
   device->layer_dispatch.utrace.CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndexed(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMultiEXT(
   VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawInfoEXT* pVertexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMultiEXT(cmd_buffer->utrace.trace, cmd_buffer, drawCount, instanceCount);
   
   device->layer_dispatch.utrace.CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMultiEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMultiIndexedEXT(
   VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawIndexedInfoEXT* pIndexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride, const int32_t* pVertexOffset
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMultiIndexedEXT(cmd_buffer->utrace.trace, cmd_buffer, drawCount, instanceCount);
   
   device->layer_dispatch.utrace.CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMultiIndexedEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirect(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirect(cmd_buffer->utrace.trace, cmd_buffer, drawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndexedIndirect(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndexedIndirect(cmd_buffer->utrace.trace, cmd_buffer, drawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndexedIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDispatchIndirect(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DispatchIndirect(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdDispatchIndirect(commandBuffer, buffer, offset);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DispatchIndirect(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyMemoryIndirectKHR(
   VkCommandBuffer commandBuffer, const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyMemoryIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyMemoryIndirectKHR(commandBuffer, pCopyMemoryIndirectInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyMemoryIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyMemoryToImageIndirectKHR(
   VkCommandBuffer commandBuffer, const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyMemoryToImageIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyMemoryToImageIndirectKHR(commandBuffer, pCopyMemoryToImageIndirectInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyMemoryToImageIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdUpdateBuffer(
   VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_UpdateBuffer(cmd_buffer->utrace.trace, cmd_buffer, dataSize);
   
   device->layer_dispatch.utrace.CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_UpdateBuffer(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdFillBuffer(
   VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_FillBuffer(cmd_buffer->utrace.trace, cmd_buffer, size);
   
   device->layer_dispatch.utrace.CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_FillBuffer(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdClearColorImage(
   VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ClearColorImage(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ClearColorImage(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdClearDepthStencilImage(
   VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ClearDepthStencilImage(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ClearDepthStencilImage(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdClearAttachments(
   VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ClearAttachments(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ClearAttachments(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdResetQueryPool(
   VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ResetQueryPool(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ResetQueryPool(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyQueryPoolResults(
   VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyQueryPoolResults(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyQueryPoolResults(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdExecuteGeneratedCommandsEXT(
   VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ExecuteGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdExecuteGeneratedCommandsEXT(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ExecuteGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdPreprocessGeneratedCommandsEXT(
   VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo, VkCommandBuffer stateCommandBuffer
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_PreprocessGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdPreprocessGeneratedCommandsEXT(commandBuffer, pGeneratedCommandsInfo, stateCommandBuffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_PreprocessGeneratedCommandsEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDispatchBase(
   VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DispatchBase(cmd_buffer->utrace.trace, cmd_buffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
   
   device->layer_dispatch.utrace.CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DispatchBase(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirectCount(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirectCount(cmd_buffer->utrace.trace, cmd_buffer, maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirectCount(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndexedIndirectCount(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndexedIndirectCount(cmd_buffer->utrace.trace, cmd_buffer, maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndexedIndirectCount(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirectByteCountEXT(
   VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, VkBuffer counterBuffer, VkDeviceSize counterBufferOffset, uint32_t counterOffset, uint32_t vertexStride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirectByteCountEXT(cmd_buffer->utrace.trace, cmd_buffer, instanceCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirectByteCountEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMeshTasksEXT(
   VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMeshTasksEXT(cmd_buffer->utrace.trace, cmd_buffer, groupCountX, groupCountY, groupCountZ);
   
   device->layer_dispatch.utrace.CmdDrawMeshTasksEXT(commandBuffer, groupCountX, groupCountY, groupCountZ);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMeshTasksEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMeshTasksIndirectEXT(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMeshTasksIndirectEXT(cmd_buffer->utrace.trace, cmd_buffer, drawCount);
   
   device->layer_dispatch.utrace.CmdDrawMeshTasksIndirectEXT(commandBuffer, buffer, offset, drawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMeshTasksIndirectEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMeshTasksIndirectCountEXT(
   VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMeshTasksIndirectCountEXT(cmd_buffer->utrace.trace, cmd_buffer, maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawMeshTasksIndirectCountEXT(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMeshTasksIndirectCountEXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyAccelerationStructureKHR(
   VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyAccelerationStructureToMemoryKHR(
   VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyAccelerationStructureToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyAccelerationStructureToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyMemoryToAccelerationStructureKHR(
   VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyMemoryToAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyMemoryToAccelerationStructureKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdTraceRaysKHR(
   VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint32_t width, uint32_t height, uint32_t depth
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_TraceRaysKHR(cmd_buffer->utrace.trace, cmd_buffer, width, height, depth);
   
   device->layer_dispatch.utrace.CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_TraceRaysKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdTraceRaysIndirectKHR(
   VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_TraceRaysIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_TraceRaysIndirectKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdTraceRaysIndirect2KHR(
   VkCommandBuffer commandBuffer, VkDeviceAddress indirectDeviceAddress
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_TraceRaysIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdTraceRaysIndirect2KHR(commandBuffer, indirectDeviceAddress);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_TraceRaysIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdBuildAccelerationStructuresKHR(
   VkCommandBuffer                                    commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_BuildAccelerationStructuresKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_BuildAccelerationStructuresKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyBuffer2(
   VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyImage2(
   VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyImage2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyImage2(commandBuffer, pCopyImageInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdBlitImage2(
   VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_BlitImage2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdBlitImage2(commandBuffer, pBlitImageInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_BlitImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyBufferToImage2(
   VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyBufferToImage2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyBufferToImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyImageToBuffer2(
   VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyImageToBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyImageToBuffer2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdResolveImage2(
   VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ResolveImage2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdResolveImage2(commandBuffer, pResolveImageInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ResolveImage2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdSetEvent2(
   VkCommandBuffer                   commandBuffer, VkEvent                                             event, const VkDependencyInfo*                             pDependencyInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_SetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdSetEvent2(commandBuffer, event, pDependencyInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_SetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdResetEvent2(
   VkCommandBuffer                   commandBuffer, VkEvent                                             event, VkPipelineStageFlags2               stageMask
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_ResetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdResetEvent2(commandBuffer, event, stageMask);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_ResetEvent2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdWaitEvents2(
   VkCommandBuffer                   commandBuffer, uint32_t                                            eventCount, const VkEvent*                     pEvents, const VkDependencyInfo*            pDependencyInfos
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_WaitEvents2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_WaitEvents2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdPipelineBarrier2(
   VkCommandBuffer                   commandBuffer, const VkDependencyInfo*                             pDependencyInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_PipelineBarrier2(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_PipelineBarrier2(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyMemoryKHR(
   VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyMemoryKHR(commandBuffer, pCopyMemoryInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyMemoryToImageKHR(
   VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyMemoryToImageKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyMemoryToImageKHR(commandBuffer, pCopyMemoryInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyMemoryToImageKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyImageToMemoryKHR(
   VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyImageToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyImageToMemoryKHR(commandBuffer, pCopyMemoryInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyImageToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdUpdateMemoryKHR(
   VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, VkDeviceSize                        dataSize, const void*          pData
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_UpdateMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdUpdateMemoryKHR(commandBuffer, pDstRange, dstFlags, dataSize, pData);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_UpdateMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdFillMemoryKHR(
   VkCommandBuffer   commandBuffer, const VkDeviceAddressRangeKHR*      pDstRange, VkAddressCommandFlagsKHR dstFlags, uint32_t                            data
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_FillMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdFillMemoryKHR(commandBuffer, pDstRange, dstFlags, data);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_FillMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdCopyQueryPoolResultsToMemoryKHR(
   VkCommandBuffer   commandBuffer, VkQueryPool                         queryPool, uint32_t                            firstQuery, uint32_t                            queryCount, const VkStridedDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR    dstFlags, VkQueryResultFlags  queryResultFlags
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_CopyQueryPoolResultsToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdCopyQueryPoolResultsToMemoryKHR(commandBuffer, queryPool, firstQuery, queryCount, pDstRange, dstFlags, queryResultFlags);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_CopyQueryPoolResultsToMemoryKHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirectByteCount2EXT(
   VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo, uint32_t counterOffset, uint32_t vertexStride
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirectByteCount2EXT(cmd_buffer->utrace.trace, cmd_buffer, instanceCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirectByteCount2EXT(commandBuffer, instanceCount, firstInstance, pCounterInfo, counterOffset, vertexStride);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirectByteCount2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirect2KHR(
   VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer, pInfo->drawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirect2KHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndexedIndirect2KHR(
   VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndexedIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer, pInfo->drawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndexedIndirect2KHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndexedIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndirectCount2KHR(
   VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer, pInfo->maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndirectCount2KHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawIndexedIndirectCount2KHR(
   VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawIndexedIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer, pInfo->maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawIndexedIndirectCount2KHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawIndexedIndirectCount2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMeshTasksIndirect2EXT(
   VkCommandBuffer   commandBuffer, const VkDrawIndirect2InfoKHR*       pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMeshTasksIndirect2EXT(cmd_buffer->utrace.trace, cmd_buffer, pInfo->drawCount);
   
   device->layer_dispatch.utrace.CmdDrawMeshTasksIndirect2EXT(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMeshTasksIndirect2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDrawMeshTasksIndirectCount2EXT(
   VkCommandBuffer   commandBuffer, const VkDrawIndirectCount2InfoKHR*  pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DrawMeshTasksIndirectCount2EXT(cmd_buffer->utrace.trace, cmd_buffer, pInfo->maxDrawCount);
   
   device->layer_dispatch.utrace.CmdDrawMeshTasksIndirectCount2EXT(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DrawMeshTasksIndirectCount2EXT(cmd_buffer->utrace.trace, cmd_buffer);
}

VKAPI_ATTR void VKAPI_CALL
utrace_CmdDispatchIndirect2KHR(
   VkCommandBuffer   commandBuffer, const VkDispatchIndirect2InfoKHR*   pInfo
) {
   VK_FROM_HANDLE(radv_cmd_buffer, cmd_buffer, commandBuffer);
   struct radv_device *device = radv_cmd_buffer_device(cmd_buffer);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_begin_DispatchIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
   
   device->layer_dispatch.utrace.CmdDispatchIndirect2KHR(commandBuffer, pInfo);
   
   if (!cmd_buffer->state.meta.inside_meta_op)
      trace_radv_end_DispatchIndirect2KHR(cmd_buffer->utrace.trace, cmd_buffer);
}

