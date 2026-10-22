
/* Copyright © 2021 Intel Corporation
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

/* This file generated from vk_physical_device_features_gen.py, don't edit directly. */

#include "vk_common_entrypoints.h"
#include "vk_log.h"
#include "vk_physical_device.h"
#include "vk_physical_device_features.h"
#include "vk_util.h"

static VkResult
check_physical_device_features(struct vk_physical_device *physical_device,
                               const VkPhysicalDeviceFeatures *supported,
                               const VkPhysicalDeviceFeatures *enabled,
                               const char *struct_name)
{
   if (enabled->robustBufferAccess && !supported->robustBufferAccess)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "robustBufferAccess");
   if (enabled->fullDrawIndexUint32 && !supported->fullDrawIndexUint32)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "fullDrawIndexUint32");
   if (enabled->imageCubeArray && !supported->imageCubeArray)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "imageCubeArray");
   if (enabled->independentBlend && !supported->independentBlend)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "independentBlend");
   if (enabled->geometryShader && !supported->geometryShader)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "geometryShader");
   if (enabled->tessellationShader && !supported->tessellationShader)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "tessellationShader");
   if (enabled->sampleRateShading && !supported->sampleRateShading)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sampleRateShading");
   if (enabled->dualSrcBlend && !supported->dualSrcBlend)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "dualSrcBlend");
   if (enabled->logicOp && !supported->logicOp)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "logicOp");
   if (enabled->multiDrawIndirect && !supported->multiDrawIndirect)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "multiDrawIndirect");
   if (enabled->drawIndirectFirstInstance && !supported->drawIndirectFirstInstance)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "drawIndirectFirstInstance");
   if (enabled->depthClamp && !supported->depthClamp)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "depthClamp");
   if (enabled->depthBiasClamp && !supported->depthBiasClamp)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "depthBiasClamp");
   if (enabled->fillModeNonSolid && !supported->fillModeNonSolid)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "fillModeNonSolid");
   if (enabled->depthBounds && !supported->depthBounds)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "depthBounds");
   if (enabled->wideLines && !supported->wideLines)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "wideLines");
   if (enabled->largePoints && !supported->largePoints)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "largePoints");
   if (enabled->alphaToOne && !supported->alphaToOne)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "alphaToOne");
   if (enabled->multiViewport && !supported->multiViewport)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "multiViewport");
   if (enabled->samplerAnisotropy && !supported->samplerAnisotropy)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "samplerAnisotropy");
   if (enabled->textureCompressionETC2 && !supported->textureCompressionETC2)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "textureCompressionETC2");
   if (enabled->textureCompressionASTC_LDR && !supported->textureCompressionASTC_LDR)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "textureCompressionASTC_LDR");
   if (enabled->textureCompressionBC && !supported->textureCompressionBC)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "textureCompressionBC");
   if (enabled->occlusionQueryPrecise && !supported->occlusionQueryPrecise)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "occlusionQueryPrecise");
   if (enabled->pipelineStatisticsQuery && !supported->pipelineStatisticsQuery)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "pipelineStatisticsQuery");
   if (enabled->vertexPipelineStoresAndAtomics && !supported->vertexPipelineStoresAndAtomics)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "vertexPipelineStoresAndAtomics");
   if (enabled->fragmentStoresAndAtomics && !supported->fragmentStoresAndAtomics)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "fragmentStoresAndAtomics");
   if (enabled->shaderTessellationAndGeometryPointSize && !supported->shaderTessellationAndGeometryPointSize)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderTessellationAndGeometryPointSize");
   if (enabled->shaderImageGatherExtended && !supported->shaderImageGatherExtended)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderImageGatherExtended");
   if (enabled->shaderStorageImageExtendedFormats && !supported->shaderStorageImageExtendedFormats)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageImageExtendedFormats");
   if (enabled->shaderStorageImageMultisample && !supported->shaderStorageImageMultisample)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageImageMultisample");
   if (enabled->shaderStorageImageReadWithoutFormat && !supported->shaderStorageImageReadWithoutFormat)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageImageReadWithoutFormat");
   if (enabled->shaderStorageImageWriteWithoutFormat && !supported->shaderStorageImageWriteWithoutFormat)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageImageWriteWithoutFormat");
   if (enabled->shaderUniformBufferArrayDynamicIndexing && !supported->shaderUniformBufferArrayDynamicIndexing)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderUniformBufferArrayDynamicIndexing");
   if (enabled->shaderSampledImageArrayDynamicIndexing && !supported->shaderSampledImageArrayDynamicIndexing)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderSampledImageArrayDynamicIndexing");
   if (enabled->shaderStorageBufferArrayDynamicIndexing && !supported->shaderStorageBufferArrayDynamicIndexing)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageBufferArrayDynamicIndexing");
   if (enabled->shaderStorageImageArrayDynamicIndexing && !supported->shaderStorageImageArrayDynamicIndexing)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderStorageImageArrayDynamicIndexing");
   if (enabled->shaderClipDistance && !supported->shaderClipDistance)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderClipDistance");
   if (enabled->shaderCullDistance && !supported->shaderCullDistance)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderCullDistance");
   if (enabled->shaderFloat64 && !supported->shaderFloat64)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderFloat64");
   if (enabled->shaderInt64 && !supported->shaderInt64)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderInt64");
   if (enabled->shaderInt16 && !supported->shaderInt16)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderInt16");
   if (enabled->shaderResourceResidency && !supported->shaderResourceResidency)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderResourceResidency");
   if (enabled->shaderResourceMinLod && !supported->shaderResourceMinLod)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "shaderResourceMinLod");
   if (enabled->sparseBinding && !supported->sparseBinding)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseBinding");
   if (enabled->sparseResidencyBuffer && !supported->sparseResidencyBuffer)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidencyBuffer");
   if (enabled->sparseResidencyImage2D && !supported->sparseResidencyImage2D)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidencyImage2D");
   if (enabled->sparseResidencyImage3D && !supported->sparseResidencyImage3D)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidencyImage3D");
   if (enabled->sparseResidency2Samples && !supported->sparseResidency2Samples)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidency2Samples");
   if (enabled->sparseResidency4Samples && !supported->sparseResidency4Samples)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidency4Samples");
   if (enabled->sparseResidency8Samples && !supported->sparseResidency8Samples)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidency8Samples");
   if (enabled->sparseResidency16Samples && !supported->sparseResidency16Samples)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidency16Samples");
   if (enabled->sparseResidencyAliased && !supported->sparseResidencyAliased)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "sparseResidencyAliased");
   if (enabled->variableMultisampleRate && !supported->variableMultisampleRate)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "variableMultisampleRate");
   if (enabled->inheritedQueries && !supported->inheritedQueries)
      return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                       "%s.%s not supported", struct_name, "inheritedQueries");

   return VK_SUCCESS;
}

VkResult
vk_physical_device_check_device_features(struct vk_physical_device *physical_device,
                                         const VkDeviceCreateInfo *pCreateInfo)
{
   VkPhysicalDevice vk_physical_device =
      vk_physical_device_to_handle(physical_device);

   /* Query the device what kind of features are supported. */
   VkPhysicalDeviceFeatures2 supported_features2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
   };

   VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV = { .pNext = NULL };
   VkPhysicalDevicePushConstantBankFeaturesNV supported_VkPhysicalDevicePushConstantBankFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV supported_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV = { .pNext = NULL };
   VkPhysicalDevicePrivateDataFeatures supported_VkPhysicalDevicePrivateDataFeatures = { .pNext = NULL };
   VkPhysicalDeviceClusterAccelerationStructureFeaturesNV supported_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceVariablePointersFeatures supported_VkPhysicalDeviceVariablePointersFeatures = { .pNext = NULL };
   VkPhysicalDeviceMultiviewFeatures supported_VkPhysicalDeviceMultiviewFeatures = { .pNext = NULL };
   VkPhysicalDevicePresentIdFeaturesKHR supported_VkPhysicalDevicePresentIdFeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePresentId2FeaturesKHR supported_VkPhysicalDevicePresentId2FeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePresentWaitFeaturesKHR supported_VkPhysicalDevicePresentWaitFeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePresentWait2FeaturesKHR supported_VkPhysicalDevicePresentWait2FeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePresentTimingFeaturesEXT supported_VkPhysicalDevicePresentTimingFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevice16BitStorageFeatures supported_VkPhysicalDevice16BitStorageFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures supported_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures = { .pNext = NULL };
   VkPhysicalDeviceSamplerYcbcrConversionFeatures supported_VkPhysicalDeviceSamplerYcbcrConversionFeatures = { .pNext = NULL };
   VkPhysicalDeviceProtectedMemoryFeatures supported_VkPhysicalDeviceProtectedMemoryFeatures = { .pNext = NULL };
   VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT supported_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceMultiDrawFeaturesEXT supported_VkPhysicalDeviceMultiDrawFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceInlineUniformBlockFeatures supported_VkPhysicalDeviceInlineUniformBlockFeatures = { .pNext = NULL };
   VkPhysicalDeviceMaintenance4Features supported_VkPhysicalDeviceMaintenance4Features = { .pNext = NULL };
   VkPhysicalDeviceMaintenance5Features supported_VkPhysicalDeviceMaintenance5Features = { .pNext = NULL };
   VkPhysicalDeviceMaintenance6Features supported_VkPhysicalDeviceMaintenance6Features = { .pNext = NULL };
   VkPhysicalDeviceMaintenance7FeaturesKHR supported_VkPhysicalDeviceMaintenance7FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceMaintenance8FeaturesKHR supported_VkPhysicalDeviceMaintenance8FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceMaintenance9FeaturesKHR supported_VkPhysicalDeviceMaintenance9FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceMaintenance11FeaturesKHR supported_VkPhysicalDeviceMaintenance11FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceMaintenance10FeaturesKHR supported_VkPhysicalDeviceMaintenance10FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderDrawParametersFeatures supported_VkPhysicalDeviceShaderDrawParametersFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderFloat16Int8Features supported_VkPhysicalDeviceShaderFloat16Int8Features = { .pNext = NULL };
   VkPhysicalDeviceHostQueryResetFeatures supported_VkPhysicalDeviceHostQueryResetFeatures = { .pNext = NULL };
   VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM supported_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceGlobalPriorityQueryFeatures supported_VkPhysicalDeviceGlobalPriorityQueryFeatures = { .pNext = NULL };
   VkPhysicalDeviceDeviceMemoryReportFeaturesEXT supported_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDescriptorIndexingFeatures supported_VkPhysicalDeviceDescriptorIndexingFeatures = { .pNext = NULL };
   VkPhysicalDeviceTimelineSemaphoreFeatures supported_VkPhysicalDeviceTimelineSemaphoreFeatures = { .pNext = NULL };
   VkPhysicalDevice8BitStorageFeatures supported_VkPhysicalDevice8BitStorageFeatures = { .pNext = NULL };
   VkPhysicalDeviceConditionalRenderingFeaturesEXT supported_VkPhysicalDeviceConditionalRenderingFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceVulkanMemoryModelFeatures supported_VkPhysicalDeviceVulkanMemoryModelFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderAtomicInt64Features supported_VkPhysicalDeviceShaderAtomicInt64Features = { .pNext = NULL };
   VkPhysicalDeviceShaderAtomicFloatFeaturesEXT supported_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT supported_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceVertexAttributeDivisorFeatures supported_VkPhysicalDeviceVertexAttributeDivisorFeatures = { .pNext = NULL };
   VkPhysicalDeviceASTCDecodeFeaturesEXT supported_VkPhysicalDeviceASTCDecodeFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceTransformFeedbackFeaturesEXT supported_VkPhysicalDeviceTransformFeedbackFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV supported_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceExclusiveScissorFeaturesNV supported_VkPhysicalDeviceExclusiveScissorFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceCornerSampledImageFeaturesNV supported_VkPhysicalDeviceCornerSampledImageFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR supported_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderImageFootprintFeaturesNV supported_VkPhysicalDeviceShaderImageFootprintFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV supported_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceCopyMemoryIndirectFeaturesNV supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceMemoryDecompressionFeaturesEXT supported_VkPhysicalDeviceMemoryDecompressionFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShadingRateImageFeaturesNV supported_VkPhysicalDeviceShadingRateImageFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceInvocationMaskFeaturesHUAWEI supported_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI = { .pNext = NULL };
   VkPhysicalDeviceMeshShaderFeaturesNV supported_VkPhysicalDeviceMeshShaderFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceMeshShaderFeaturesEXT supported_VkPhysicalDeviceMeshShaderFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceAccelerationStructureFeaturesKHR supported_VkPhysicalDeviceAccelerationStructureFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRayTracingPipelineFeaturesKHR supported_VkPhysicalDeviceRayTracingPipelineFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRayQueryFeaturesKHR supported_VkPhysicalDeviceRayQueryFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR supported_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceFragmentDensityMapFeaturesEXT supported_VkPhysicalDeviceFragmentDensityMapFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceFragmentDensityMap2FeaturesEXT supported_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT supported_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceScalarBlockLayoutFeatures supported_VkPhysicalDeviceScalarBlockLayoutFeatures = { .pNext = NULL };
   VkPhysicalDeviceUniformBufferStandardLayoutFeatures supported_VkPhysicalDeviceUniformBufferStandardLayoutFeatures = { .pNext = NULL };
   VkPhysicalDeviceDepthClipEnableFeaturesEXT supported_VkPhysicalDeviceDepthClipEnableFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceMemoryPriorityFeaturesEXT supported_VkPhysicalDeviceMemoryPriorityFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT supported_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceBufferDeviceAddressFeatures supported_VkPhysicalDeviceBufferDeviceAddressFeatures = { .pNext = NULL };
   VkPhysicalDeviceBufferDeviceAddressFeaturesEXT supported_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceImagelessFramebufferFeatures supported_VkPhysicalDeviceImagelessFramebufferFeatures = { .pNext = NULL };
   VkPhysicalDeviceTextureCompressionASTCHDRFeatures supported_VkPhysicalDeviceTextureCompressionASTCHDRFeatures = { .pNext = NULL };
   VkPhysicalDeviceCooperativeMatrixFeaturesNV supported_VkPhysicalDeviceCooperativeMatrixFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceYcbcrImageArraysFeaturesEXT supported_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePresentBarrierFeaturesNV supported_VkPhysicalDevicePresentBarrierFeaturesNV = { .pNext = NULL };
   VkPhysicalDevicePerformanceQueryFeaturesKHR supported_VkPhysicalDevicePerformanceQueryFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceCoverageReductionModeFeaturesNV supported_VkPhysicalDeviceCoverageReductionModeFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL supported_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL = { .pNext = NULL };
   VkPhysicalDeviceShaderClockFeaturesKHR supported_VkPhysicalDeviceShaderClockFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceIndexTypeUint8Features supported_VkPhysicalDeviceIndexTypeUint8Features = { .pNext = NULL };
   VkPhysicalDeviceShaderSMBuiltinsFeaturesNV supported_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT supported_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures supported_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures = { .pNext = NULL };
   VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT supported_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR supported_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures supported_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures = { .pNext = NULL };
   VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT supported_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceSubgroupSizeControlFeatures supported_VkPhysicalDeviceSubgroupSizeControlFeatures = { .pNext = NULL };
   VkPhysicalDeviceLineRasterizationFeatures supported_VkPhysicalDeviceLineRasterizationFeatures = { .pNext = NULL };
   VkPhysicalDevicePipelineCreationCacheControlFeatures supported_VkPhysicalDevicePipelineCreationCacheControlFeatures = { .pNext = NULL };
   VkPhysicalDeviceVulkan11Features supported_VkPhysicalDeviceVulkan11Features = { .pNext = NULL };
   VkPhysicalDeviceVulkan12Features supported_VkPhysicalDeviceVulkan12Features = { .pNext = NULL };
   VkPhysicalDeviceVulkan13Features supported_VkPhysicalDeviceVulkan13Features = { .pNext = NULL };
   VkPhysicalDeviceVulkan14Features supported_VkPhysicalDeviceVulkan14Features = { .pNext = NULL };
   VkPhysicalDeviceCoherentMemoryFeaturesAMD supported_VkPhysicalDeviceCoherentMemoryFeaturesAMD = { .pNext = NULL };
   VkPhysicalDeviceGpaFeaturesAMD supported_VkPhysicalDeviceGpaFeaturesAMD = { .pNext = NULL };
   VkPhysicalDeviceCustomBorderColorFeaturesEXT supported_VkPhysicalDeviceCustomBorderColorFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceBorderColorSwizzleFeaturesEXT supported_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceExtendedDynamicStateFeaturesEXT supported_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceExtendedDynamicState2FeaturesEXT supported_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceExtendedDynamicState3FeaturesEXT supported_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceExtendedFlagsFeaturesKHR supported_VkPhysicalDeviceExtendedFlagsFeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV supported_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceDiagnosticsConfigFeaturesNV supported_VkPhysicalDeviceDiagnosticsConfigFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures supported_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR supported_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRobustness2FeaturesKHR supported_VkPhysicalDeviceRobustness2FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceImageRobustnessFeatures supported_VkPhysicalDeviceImageRobustnessFeatures = { .pNext = NULL };
   VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR supported_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR = { .pNext = NULL };
   VkPhysicalDevice4444FormatsFeaturesEXT supported_VkPhysicalDevice4444FormatsFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceSubpassShadingFeaturesHUAWEI supported_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI = { .pNext = NULL };
   VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI supported_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI = { .pNext = NULL };
   VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT supported_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceFragmentShadingRateFeaturesKHR supported_VkPhysicalDeviceFragmentShadingRateFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderTerminateInvocationFeatures supported_VkPhysicalDeviceShaderTerminateInvocationFeatures = { .pNext = NULL };
   VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV supported_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceImage2DViewOf3DFeaturesEXT supported_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT supported_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT supported_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT supported_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT supported_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDepthClipControlFeaturesEXT supported_VkPhysicalDeviceDepthClipControlFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT supported_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceCustomResolveFeaturesEXT supported_VkPhysicalDeviceCustomResolveFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDepthClampControlFeaturesEXT supported_VkPhysicalDeviceDepthClampControlFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT supported_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceExternalMemoryRDMAFeaturesNV supported_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR supported_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceColorWriteEnableFeaturesEXT supported_VkPhysicalDeviceColorWriteEnableFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceSynchronization2Features supported_VkPhysicalDeviceSynchronization2Features = { .pNext = NULL };
   VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR supported_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceHostImageCopyFeatures supported_VkPhysicalDeviceHostImageCopyFeatures = { .pNext = NULL };
   VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT supported_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceLegacyDitheringFeaturesEXT supported_VkPhysicalDeviceLegacyDitheringFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT supported_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT supported_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePipelineProtectedAccessFeatures supported_VkPhysicalDevicePipelineProtectedAccessFeatures = { .pNext = NULL };
   VkPhysicalDeviceVideoMaintenance1FeaturesKHR supported_VkPhysicalDeviceVideoMaintenance1FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoMaintenance2FeaturesKHR supported_VkPhysicalDeviceVideoMaintenance2FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoDecodeVP9FeaturesKHR supported_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR supported_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR supported_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoEncodeAV1FeaturesKHR supported_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceInheritedViewportScissorFeaturesNV supported_VkPhysicalDeviceInheritedViewportScissorFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT supported_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceProvokingVertexFeaturesEXT supported_VkPhysicalDeviceProvokingVertexFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR supported_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceDescriptorBufferFeaturesEXT supported_VkPhysicalDeviceDescriptorBufferFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderIntegerDotProductFeatures supported_VkPhysicalDeviceShaderIntegerDotProductFeatures = { .pNext = NULL };
   VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR supported_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderFmaFeaturesKHR supported_VkPhysicalDeviceShaderFmaFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRayTracingMotionBlurFeaturesNV supported_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceRayTracingValidationFeaturesNV supported_VkPhysicalDeviceRayTracingValidationFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV supported_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT supported_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDynamicRenderingFeatures supported_VkPhysicalDeviceDynamicRenderingFeatures = { .pNext = NULL };
   VkPhysicalDeviceImageViewMinLodFeaturesEXT supported_VkPhysicalDeviceImageViewMinLodFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT supported_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceLinearColorAttachmentFeaturesNV supported_VkPhysicalDeviceLinearColorAttachmentFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT supported_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePipelineBinaryFeaturesKHR supported_VkPhysicalDevicePipelineBinaryFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM supported_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE supported_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE = { .pNext = NULL };
   VkPhysicalDeviceNestedCommandBufferFeaturesEXT supported_VkPhysicalDeviceNestedCommandBufferFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT supported_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceImageCompressionControlFeaturesEXT supported_VkPhysicalDeviceImageCompressionControlFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT supported_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT supported_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceOpacityMicromapFeaturesKHR supported_VkPhysicalDeviceOpacityMicromapFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceOpacityMicromapFeaturesEXT supported_VkPhysicalDeviceOpacityMicromapFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePipelinePropertiesFeaturesEXT supported_VkPhysicalDevicePipelinePropertiesFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD supported_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD = { .pNext = NULL };
   VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT supported_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePipelineRobustnessFeatures supported_VkPhysicalDevicePipelineRobustnessFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM supported_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceImageProcessingFeaturesQCOM supported_VkPhysicalDeviceImageProcessingFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceTilePropertiesFeaturesQCOM supported_VkPhysicalDeviceTilePropertiesFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceAmigoProfilingFeaturesSEC supported_VkPhysicalDeviceAmigoProfilingFeaturesSEC = { .pNext = NULL };
   VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT supported_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceAddressBindingReportFeaturesEXT supported_VkPhysicalDeviceAddressBindingReportFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceOpticalFlowFeaturesNV supported_VkPhysicalDeviceOpticalFlowFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceFaultFeaturesEXT supported_VkPhysicalDeviceFaultFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceFaultFeaturesKHR supported_VkPhysicalDeviceFaultFeaturesKHR = { .pNext = NULL };
   VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT supported_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM supported_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceFrameBoundaryFeaturesEXT supported_VkPhysicalDeviceFrameBoundaryFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT supported_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR supported_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR supported_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceDepthBiasControlFeaturesEXT supported_VkPhysicalDeviceDepthBiasControlFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV supported_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM supported_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR supported_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM supported_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceShaderObjectFeaturesEXT supported_VkPhysicalDeviceShaderObjectFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderTileImageFeaturesEXT supported_VkPhysicalDeviceShaderTileImageFeaturesEXT = { .pNext = NULL };
#ifdef VK_USE_PLATFORM_SCREEN_QNX
   VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX supported_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX = { .pNext = NULL };
#endif
   VkPhysicalDeviceCooperativeMatrixFeaturesKHR supported_VkPhysicalDeviceCooperativeMatrixFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM supported_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceAntiLagFeaturesAMD supported_VkPhysicalDeviceAntiLagFeaturesAMD = { .pNext = NULL };
   VkPhysicalDeviceTileMemoryHeapFeaturesQCOM supported_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceCubicClampFeaturesQCOM supported_VkPhysicalDeviceCubicClampFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceYcbcrDegammaFeaturesQCOM supported_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceCubicWeightsFeaturesQCOM supported_VkPhysicalDeviceCubicWeightsFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceImageProcessing2FeaturesQCOM supported_VkPhysicalDeviceImageProcessing2FeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceImageProcessing3FeaturesQCOM supported_VkPhysicalDeviceImageProcessing3FeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV supported_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV = { .pNext = NULL };
   VkPhysicalDevicePerStageDescriptorSetFeaturesNV supported_VkPhysicalDevicePerStageDescriptorSetFeaturesNV = { .pNext = NULL };
#ifdef VK_USE_PLATFORM_ANDROID_KHR
   VkPhysicalDeviceExternalFormatResolveFeaturesANDROID supported_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID = { .pNext = NULL };
#endif
   VkPhysicalDeviceSchedulingControlsFeaturesARM supported_VkPhysicalDeviceSchedulingControlsFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG supported_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG = { .pNext = NULL };
   VkPhysicalDeviceRenderPassStripedFeaturesARM supported_VkPhysicalDeviceRenderPassStripedFeaturesARM = { .pNext = NULL };
   VkPhysicalDevicePipelineOpacityMicromapFeaturesARM supported_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR supported_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderSubgroupRotateFeatures supported_VkPhysicalDeviceShaderSubgroupRotateFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderExpectAssumeFeatures supported_VkPhysicalDeviceShaderExpectAssumeFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderFloatControls2Features supported_VkPhysicalDeviceShaderFloatControls2Features = { .pNext = NULL };
   VkPhysicalDeviceDynamicRenderingLocalReadFeatures supported_VkPhysicalDeviceDynamicRenderingLocalReadFeatures = { .pNext = NULL };
   VkPhysicalDeviceShaderQuadControlFeaturesKHR supported_VkPhysicalDeviceShaderQuadControlFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV supported_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceMapMemoryPlacedFeaturesEXT supported_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderBfloat16FeaturesKHR supported_VkPhysicalDeviceShaderBfloat16FeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceRawAccessChainsFeaturesNV supported_VkPhysicalDeviceRawAccessChainsFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceCommandBufferInheritanceFeaturesNV supported_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceImageAlignmentControlFeaturesMESA supported_VkPhysicalDeviceImageAlignmentControlFeaturesMESA = { .pNext = NULL };
   VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT supported_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT = { .pNext = NULL };
   VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR supported_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceCooperativeMatrix2FeaturesNV supported_VkPhysicalDeviceCooperativeMatrix2FeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV supported_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceHdrVividFeaturesHUAWEI supported_VkPhysicalDeviceHdrVividFeaturesHUAWEI = { .pNext = NULL };
   VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT supported_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDepthClampZeroOneFeaturesKHR supported_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceCooperativeVectorFeaturesNV supported_VkPhysicalDeviceCooperativeVectorFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceTileShadingFeaturesQCOM supported_VkPhysicalDeviceTileShadingFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE supported_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE = { .pNext = NULL };
   VkPhysicalDevicePresentMeteringFeaturesNV supported_VkPhysicalDevicePresentMeteringFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT supported_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE supported_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE = { .pNext = NULL };
   VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT supported_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceFormatPackFeaturesARM supported_VkPhysicalDeviceFormatPackFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceThrottleHintFeaturesSEC supported_VkPhysicalDeviceThrottleHintFeaturesSEC = { .pNext = NULL };
   VkPhysicalDeviceTensorFeaturesARM supported_VkPhysicalDeviceTensorFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceDescriptorBufferTensorFeaturesARM supported_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceShaderFloat8FeaturesEXT supported_VkPhysicalDeviceShaderFloat8FeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDataGraphFeaturesARM supported_VkPhysicalDeviceDataGraphFeaturesARM = { .pNext = NULL };
   VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC supported_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC = { .pNext = NULL };
   VkPhysicalDeviceDataGraphModelFeaturesQCOM supported_VkPhysicalDeviceDataGraphModelFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDeviceShaderUntypedPointersFeaturesKHR supported_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE supported_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE = { .pNext = NULL };
   VkPhysicalDeviceShader64BitIndexingFeaturesEXT supported_VkPhysicalDeviceShader64BitIndexingFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceQueuePerfHintFeaturesQCOM supported_VkPhysicalDeviceQueuePerfHintFeaturesQCOM = { .pNext = NULL };
   VkPhysicalDevicePerformanceCountersByRegionFeaturesARM supported_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV supported_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV = { .pNext = NULL };
   VkPhysicalDeviceShaderLongVectorFeaturesEXT supported_VkPhysicalDeviceShaderLongVectorFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT supported_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT supported_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderSplitBarrierFeaturesEXT supported_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceDescriptorHeapFeaturesEXT supported_VkPhysicalDeviceDescriptorHeapFeaturesEXT = { .pNext = NULL };
   VkPhysicalDeviceShaderInstrumentationFeaturesARM supported_VkPhysicalDeviceShaderInstrumentationFeaturesARM = { .pNext = NULL };
   VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR supported_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderConstantDataFeaturesKHR supported_VkPhysicalDeviceShaderConstantDataFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceShaderAbortFeaturesKHR supported_VkPhysicalDeviceShaderAbortFeaturesKHR = { .pNext = NULL };
   VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM supported_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM = { .pNext = NULL };

   vk_foreach_struct_const(features, pCreateInfo->pNext) {
      VkBaseOutStructure *supported = NULL;
      switch (features->sType) {
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_device_generated_commands))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_push_constant_bank))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePushConstantBankFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_device_generated_commands_compute))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_private_data))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePrivateDataFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_cluster_acceleration_structure))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_variable_pointers))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVariablePointersFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_multiview))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultiviewFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_present_id))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentIdFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_present_id2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentId2FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_present_wait))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentWaitFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_present_wait2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentWait2FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_present_timing))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentTimingFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_16bit_storage))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevice16BitStorageFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_subgroup_extended_types))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_sampler_ycbcr_conversion))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSamplerYcbcrConversionFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceProtectedMemoryFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_blend_operation_advanced))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_multi_draw))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultiDrawFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_inline_uniform_block))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceInlineUniformBlockFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_maintenance4))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance4Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_maintenance5))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance5Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_maintenance6))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance6Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_maintenance7))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance7FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_maintenance8))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance8FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_maintenance9))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance9FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_maintenance11))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance11FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_maintenance10))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMaintenance10FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderDrawParametersFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_float16_int8))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderFloat16Int8Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_host_query_reset))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceHostQueryResetFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_elapsed_timer_query))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_global_priority || physical_device->supported_extensions.EXT_global_priority_query))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceGlobalPriorityQueryFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_device_memory_report))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_descriptor_indexing))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorIndexingFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_timeline_semaphore))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTimelineSemaphoreFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_8bit_storage))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevice8BitStorageFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_conditional_rendering))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceConditionalRenderingFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_vulkan_memory_model))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVulkanMemoryModelFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_atomic_int64))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderAtomicInt64Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_atomic_float))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_atomic_float2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_vertex_attribute_divisor || physical_device->supported_extensions.KHR_vertex_attribute_divisor))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVertexAttributeDivisorFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_astc_decode_mode))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceASTCDecodeFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_transform_feedback))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTransformFeedbackFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_representative_fragment_test))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_scissor_exclusive))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExclusiveScissorFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_corner_sampled_image))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCornerSampledImageFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR:
         if (!(physical_device->supported_extensions.NV_compute_shader_derivatives || physical_device->supported_extensions.KHR_compute_shader_derivatives))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_shader_image_footprint))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderImageFootprintFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_dedicated_allocation_image_aliasing))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_copy_memory_indirect))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_copy_memory_indirect))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT:
         if (!(physical_device->supported_extensions.NV_memory_decompression || physical_device->supported_extensions.EXT_memory_decompression))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMemoryDecompressionFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_shading_rate_image))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShadingRateImageFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI:
         if (!(physical_device->supported_extensions.HUAWEI_invocation_mask))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_mesh_shader))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMeshShaderFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_mesh_shader))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMeshShaderFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_acceleration_structure))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAccelerationStructureFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_ray_tracing_pipeline))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingPipelineFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_ray_query))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayQueryFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_ray_tracing_maintenance1))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_fragment_density_map))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentDensityMapFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_fragment_density_map2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT:
         if (!(physical_device->supported_extensions.QCOM_fragment_density_map_offset || physical_device->supported_extensions.EXT_fragment_density_map_offset))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_scalar_block_layout))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceScalarBlockLayoutFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_uniform_buffer_standard_layout))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceUniformBufferStandardLayoutFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_depth_clip_enable))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDepthClipEnableFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_memory_priority))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMemoryPriorityFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_pageable_device_local_memory))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_buffer_device_address))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceBufferDeviceAddressFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_buffer_device_address))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_imageless_framebuffer))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImagelessFramebufferFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_texture_compression_astc_hdr))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTextureCompressionASTCHDRFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_cooperative_matrix))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeMatrixFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_ycbcr_image_arrays))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_present_barrier))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentBarrierFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_performance_query))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePerformanceQueryFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_coverage_reduction_mode))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCoverageReductionModeFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL:
         if (!(physical_device->supported_extensions.INTEL_shader_integer_functions2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_clock))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderClockFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_index_type_uint8 || physical_device->supported_extensions.KHR_index_type_uint8))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceIndexTypeUint8Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_shader_sm_builtins))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_fragment_shader_interlock))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_separate_depth_stencil_layouts))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_primitive_topology_list_restart))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_pipeline_executable_properties))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_shader_demote_to_helper_invocation))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_texel_buffer_alignment))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_subgroup_size_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSubgroupSizeControlFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_line_rasterization || physical_device->supported_extensions.KHR_line_rasterization))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceLineRasterizationFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_pipeline_creation_cache_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineCreationCacheControlFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVulkan11Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVulkan12Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVulkan13Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0)))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVulkan14Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD:
         if (!(physical_device->supported_extensions.AMD_device_coherent_memory))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCoherentMemoryFeaturesAMD;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD:
         if (!(physical_device->supported_extensions.AMD_gpa_interface))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceGpaFeaturesAMD;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_custom_border_color))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCustomBorderColorFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_border_color_swizzle))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state3))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_extended_flags))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExtendedFlagsFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_partitioned_acceleration_structure))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_device_diagnostics_config))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDiagnosticsConfigFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_zero_initialize_workgroup_memory))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_subgroup_uniform_control_flow))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR:
         if (!(physical_device->supported_extensions.EXT_robustness2 || physical_device->supported_extensions.KHR_robustness2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRobustness2FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_image_robustness))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageRobustnessFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_workgroup_memory_explicit_layout))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_4444_formats))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevice4444FormatsFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI:
         if (!(physical_device->supported_extensions.HUAWEI_subpass_shading))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI:
         if (!(physical_device->supported_extensions.HUAWEI_cluster_culling_shader))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_image_atomic_int64))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_fragment_shading_rate))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentShadingRateFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_shader_terminate_invocation))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderTerminateInvocationFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_fragment_shading_rate_enums))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_image_2d_view_of_3d))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_image_sliced_view_of_3d))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_attachment_feedback_loop_dynamic_state))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_legacy_vertex_attributes))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.VALVE_mutable_descriptor_type || physical_device->supported_extensions.EXT_mutable_descriptor_type))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_depth_clip_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDepthClipControlFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_zero_initialize_device_memory))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_custom_resolve))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCustomResolveFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_device_generated_commands))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_depth_clamp_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDepthClampControlFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_vertex_input_dynamic_state))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_external_memory_rdma))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_relaxed_extended_instruction))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_color_write_enable))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceColorWriteEnableFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_synchronization2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSynchronization2Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_unified_image_layouts))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_host_image_copy))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceHostImageCopyFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_primitives_generated_query))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_legacy_dithering))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceLegacyDitheringFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_multisampled_render_to_single_sampled))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_multisampled_render_to_swapchain))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_pipeline_protected_access))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineProtectedAccessFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_maintenance1))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoMaintenance1FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_maintenance2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoMaintenance2FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_decode_vp9))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_encode_quantization_map))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_encode_feedback2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_encode_av1))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_inherited_viewport_scissor))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceInheritedViewportScissorFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_ycbcr_2plane_444_formats))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_provoking_vertex))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceProvokingVertexFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_video_encode_intra_refresh))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_descriptor_buffer))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorBufferFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_shader_integer_dot_product))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderIntegerDotProductFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR:
         if (!(physical_device->supported_extensions.NV_fragment_shader_barycentric || physical_device->supported_extensions.KHR_fragment_shader_barycentric))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_fma))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderFmaFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_ray_tracing_motion_blur))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_ray_tracing_validation))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingValidationFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_ray_tracing_linear_swept_spheres))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_rgba10x6_formats))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_dynamic_rendering))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDynamicRenderingFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_image_view_min_lod))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageViewMinLodFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.ARM_rasterization_order_attachment_access || physical_device->supported_extensions.EXT_rasterization_order_attachment_access))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_linear_color_attachment))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceLinearColorAttachmentFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_graphics_pipeline_library))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_pipeline_binary))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineBinaryFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_data_graph_neural_accelerator_statistics))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE:
         if (!(physical_device->supported_extensions.VALVE_descriptor_set_host_mapping))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_nested_command_buffer))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceNestedCommandBufferFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_module_identifier))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_image_compression_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageCompressionControlFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_image_compression_control_swapchain))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_subpass_merge_feedback))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_opacity_micromap))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceOpacityMicromapFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_opacity_micromap))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceOpacityMicromapFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_pipeline_properties))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelinePropertiesFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD:
         if (!(physical_device->supported_extensions.AMD_shader_early_and_late_fragment_tests))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_non_seamless_cube_map))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_pipeline_robustness))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineRobustnessFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_shader_multiple_wait_queues))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_image_processing))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageProcessingFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_tile_properties))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTilePropertiesFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC:
         if (!(physical_device->supported_extensions.SEC_amigo_profiling))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAmigoProfilingFeaturesSEC;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_attachment_feedback_loop_layout))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_device_address_binding_report))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAddressBindingReportFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_optical_flow))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceOpticalFlowFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_device_fault))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFaultFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_device_fault))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFaultFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_pipeline_library_group_handles))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_shader_core_builtins))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_frame_boundary))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFrameBoundaryFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_dynamic_rendering_unused_attachments))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_internally_synchronized_queues))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR:
         if (!(physical_device->supported_extensions.EXT_swapchain_maintenance1 || physical_device->supported_extensions.KHR_swapchain_maintenance1))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_depth_bias_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDepthBiasControlFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_ray_tracing_invocation_reorder))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_ray_tracing_invocation_reorder))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_extended_sparse_address_space))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_multiview_per_view_viewports))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_ray_tracing_position_fetch))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_multiview_per_view_render_areas))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_object))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderObjectFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_tile_image))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderTileImageFeaturesEXT;
         break;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX:
         if (!(physical_device->supported_extensions.QNX_external_memory_screen_buffer))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX;
         break;
#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_cooperative_matrix))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeMatrixFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_cooperative_matrix_conversion))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD:
         if (!(physical_device->supported_extensions.AMD_anti_lag))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceAntiLagFeaturesAMD;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_tile_memory_heap))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_filter_cubic_clamp))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCubicClampFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_ycbcr_degamma))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_filter_cubic_weights))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCubicWeightsFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_image_processing2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageProcessing2FeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_image_processing3))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageProcessing3FeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_descriptor_pool_overallocation))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_per_stage_descriptor_set))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePerStageDescriptorSetFeaturesNV;
         break;
#ifdef VK_USE_PLATFORM_ANDROID_KHR
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID:
         if (!(physical_device->supported_extensions.ANDROID_external_format_resolve))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID;
         break;
#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_scheduling_controls))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceSchedulingControlsFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG:
         if (!(physical_device->supported_extensions.IMG_relaxed_line_rasterization))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_render_pass_striped))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRenderPassStripedFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_pipeline_opacity_micromap))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_maximal_reconvergence))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_subgroup_rotate))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSubgroupRotateFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_expect_assume))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderExpectAssumeFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_float_controls2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderFloatControls2Features;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES:
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_dynamic_rendering_local_read))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDynamicRenderingLocalReadFeatures;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_quad_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderQuadControlFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_shader_atomic_float16_vector))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_map_memory_placed))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_bfloat16))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderBfloat16FeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_raw_access_chains))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceRawAccessChainsFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_command_buffer_inheritance))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA:
         if (!(physical_device->supported_extensions.MESA_image_alignment_control))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceImageAlignmentControlFeaturesMESA;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_replicated_composites))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR:
         if (!(physical_device->supported_extensions.EXT_present_mode_fifo_latest_ready || physical_device->supported_extensions.KHR_present_mode_fifo_latest_ready))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_cooperative_matrix2))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeMatrix2FeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_cooperative_matrix_decode_vector))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI:
         if (!(physical_device->supported_extensions.HUAWEI_hdr_vivid))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceHdrVividFeaturesHUAWEI;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_vertex_attribute_robustness))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR:
         if (!(physical_device->supported_extensions.EXT_depth_clamp_zero_one || physical_device->supported_extensions.KHR_depth_clamp_zero_one))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_cooperative_vector))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceCooperativeVectorFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_tile_shading))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTileShadingFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE:
         if (!(physical_device->supported_extensions.VALVE_fragment_density_map_layered))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_present_metering))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePresentMeteringFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_uniform_buffer_unsized_array))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE:
         if (!(physical_device->supported_extensions.VALVE_shader_mixed_float_dot_product))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_primitive_restart_index))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_format_pack))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceFormatPackFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC:
         if (!(physical_device->supported_extensions.SEC_throttle_hint))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceThrottleHintFeaturesSEC;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_tensors))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTensorFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_tensors))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_float8))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderFloat8FeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_data_graph))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDataGraphFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC:
         if (!(physical_device->supported_extensions.SEC_pipeline_cache_incremental_mode))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_data_graph_model))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDataGraphModelFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_untyped_pointers))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE:
         if (!(physical_device->supported_extensions.VALVE_video_encode_rgb_conversion))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_64bit_indexing))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShader64BitIndexingFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM:
         if (!(physical_device->supported_extensions.QCOM_queue_perf_hint))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceQueuePerfHintFeaturesQCOM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_performance_counters_by_region))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV:
         if (!(physical_device->supported_extensions.NV_compute_occupancy_priority))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_long_vector))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderLongVectorFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_texture_compression_astc_3d))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_subgroup_partitioned))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_shader_split_barrier))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT:
         if (!(physical_device->supported_extensions.EXT_descriptor_heap))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDescriptorHeapFeaturesEXT;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_shader_instrumentation))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderInstrumentationFeaturesARM;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_device_address_commands))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_constant_data))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderConstantDataFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR:
         if (!(physical_device->supported_extensions.KHR_shader_abort))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceShaderAbortFeaturesKHR;
         break;
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM:
         if (!(physical_device->supported_extensions.ARM_data_graph_optical_flow))
            break;
         supported = (VkBaseOutStructure *) &supported_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM;
         break;
      default:
         break;
      }

      /* Not a feature struct. */
      if (!supported)
         continue;

      /* Ignore duplicated structs instead of failing to create the device. */
      if (supported->sType != 0) {
         vk_logw(VK_LOG_OBJS(physical_device),
                 "WARNING: Duplicate sType %s in the device creation chain.\n",
                 vk_StructureType_to_str(features->sType));
         continue;
      }

      /* Check for cycles in the list */
      if (supported->pNext != NULL)
         return VK_ERROR_UNKNOWN;

      supported->sType = features->sType;
      __vk_append_struct(&supported_features2, supported);
   }

   physical_device->dispatch_table.GetPhysicalDeviceFeatures2(
      vk_physical_device, &supported_features2);

   if (pCreateInfo->pEnabledFeatures) {
      VkResult result =
        check_physical_device_features(physical_device,
                                       &supported_features2.features,
                                       pCreateInfo->pEnabledFeatures,
                                       "VkPhysicalDeviceFeatures");
      if (result != VK_SUCCESS)
         return result;
   }

   /* Iterate through additional feature structs */
   vk_foreach_struct_const(features, pCreateInfo->pNext) {
      /* Check each feature boolean for given structure. */
      switch (features->sType) {
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
         const VkPhysicalDeviceFeatures2 *features2 = (const void *)features;
         VkResult result =
            check_physical_device_features(physical_device,
                                           &supported_features2.features,
                                           &features2->features,
                                           "VkPhysicalDeviceFeatures2.features");
         if (result != VK_SUCCESS)
            return result;
        break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_device_generated_commands))
            break;
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *a = &supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV;
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *b = (const void *) features;
         if (b->deviceGeneratedCommands && !a->deviceGeneratedCommands)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV", "deviceGeneratedCommands");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_push_constant_bank))
            break;
         const VkPhysicalDevicePushConstantBankFeaturesNV *a = &supported_VkPhysicalDevicePushConstantBankFeaturesNV;
         const VkPhysicalDevicePushConstantBankFeaturesNV *b = (const void *) features;
         if (b->pushConstantBank && !a->pushConstantBank)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePushConstantBankFeaturesNV", "pushConstantBank");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_device_generated_commands_compute))
            break;
         const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV *a = &supported_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV;
         const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV *b = (const void *) features;
         if (b->deviceGeneratedCompute && !a->deviceGeneratedCompute)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV", "deviceGeneratedCompute");
         if (b->deviceGeneratedComputePipelines && !a->deviceGeneratedComputePipelines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV", "deviceGeneratedComputePipelines");
         if (b->deviceGeneratedComputeCaptureReplay && !a->deviceGeneratedComputeCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV", "deviceGeneratedComputeCaptureReplay");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_private_data))
            break;
         const VkPhysicalDevicePrivateDataFeatures *a = &supported_VkPhysicalDevicePrivateDataFeatures;
         const VkPhysicalDevicePrivateDataFeatures *b = (const void *) features;
         if (b->privateData && !a->privateData)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrivateDataFeatures", "privateData");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_cluster_acceleration_structure))
            break;
         const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV *a = &supported_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV;
         const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV *b = (const void *) features;
         if (b->clusterAccelerationStructure && !a->clusterAccelerationStructure)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceClusterAccelerationStructureFeaturesNV", "clusterAccelerationStructure");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_variable_pointers))
            break;
         const VkPhysicalDeviceVariablePointersFeatures *a = &supported_VkPhysicalDeviceVariablePointersFeatures;
         const VkPhysicalDeviceVariablePointersFeatures *b = (const void *) features;
         if (b->variablePointersStorageBuffer && !a->variablePointersStorageBuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVariablePointersFeatures", "variablePointersStorageBuffer");
         if (b->variablePointers && !a->variablePointers)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVariablePointersFeatures", "variablePointers");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_multiview))
            break;
         const VkPhysicalDeviceMultiviewFeatures *a = &supported_VkPhysicalDeviceMultiviewFeatures;
         const VkPhysicalDeviceMultiviewFeatures *b = (const void *) features;
         if (b->multiview && !a->multiview)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiviewFeatures", "multiview");
         if (b->multiviewGeometryShader && !a->multiviewGeometryShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiviewFeatures", "multiviewGeometryShader");
         if (b->multiviewTessellationShader && !a->multiviewTessellationShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiviewFeatures", "multiviewTessellationShader");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_present_id))
            break;
         const VkPhysicalDevicePresentIdFeaturesKHR *a = &supported_VkPhysicalDevicePresentIdFeaturesKHR;
         const VkPhysicalDevicePresentIdFeaturesKHR *b = (const void *) features;
         if (b->presentId && !a->presentId)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentIdFeaturesKHR", "presentId");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_present_id2))
            break;
         const VkPhysicalDevicePresentId2FeaturesKHR *a = &supported_VkPhysicalDevicePresentId2FeaturesKHR;
         const VkPhysicalDevicePresentId2FeaturesKHR *b = (const void *) features;
         if (b->presentId2 && !a->presentId2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentId2FeaturesKHR", "presentId2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_present_wait))
            break;
         const VkPhysicalDevicePresentWaitFeaturesKHR *a = &supported_VkPhysicalDevicePresentWaitFeaturesKHR;
         const VkPhysicalDevicePresentWaitFeaturesKHR *b = (const void *) features;
         if (b->presentWait && !a->presentWait)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentWaitFeaturesKHR", "presentWait");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_present_wait2))
            break;
         const VkPhysicalDevicePresentWait2FeaturesKHR *a = &supported_VkPhysicalDevicePresentWait2FeaturesKHR;
         const VkPhysicalDevicePresentWait2FeaturesKHR *b = (const void *) features;
         if (b->presentWait2 && !a->presentWait2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentWait2FeaturesKHR", "presentWait2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_present_timing))
            break;
         const VkPhysicalDevicePresentTimingFeaturesEXT *a = &supported_VkPhysicalDevicePresentTimingFeaturesEXT;
         const VkPhysicalDevicePresentTimingFeaturesEXT *b = (const void *) features;
         if (b->presentTiming && !a->presentTiming)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentTimingFeaturesEXT", "presentTiming");
         if (b->presentAtAbsoluteTime && !a->presentAtAbsoluteTime)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentTimingFeaturesEXT", "presentAtAbsoluteTime");
         if (b->presentAtRelativeTime && !a->presentAtRelativeTime)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentTimingFeaturesEXT", "presentAtRelativeTime");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_16bit_storage))
            break;
         const VkPhysicalDevice16BitStorageFeatures *a = &supported_VkPhysicalDevice16BitStorageFeatures;
         const VkPhysicalDevice16BitStorageFeatures *b = (const void *) features;
         if (b->storageBuffer16BitAccess && !a->storageBuffer16BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice16BitStorageFeatures", "storageBuffer16BitAccess");
         if (b->uniformAndStorageBuffer16BitAccess && !a->uniformAndStorageBuffer16BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice16BitStorageFeatures", "uniformAndStorageBuffer16BitAccess");
         if (b->storagePushConstant16 && !a->storagePushConstant16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice16BitStorageFeatures", "storagePushConstant16");
         if (b->storageInputOutput16 && !a->storageInputOutput16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice16BitStorageFeatures", "storageInputOutput16");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_subgroup_extended_types))
            break;
         const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *a = &supported_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures;
         const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *b = (const void *) features;
         if (b->shaderSubgroupExtendedTypes && !a->shaderSubgroupExtendedTypes)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures", "shaderSubgroupExtendedTypes");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0) || physical_device->supported_extensions.KHR_sampler_ycbcr_conversion))
            break;
         const VkPhysicalDeviceSamplerYcbcrConversionFeatures *a = &supported_VkPhysicalDeviceSamplerYcbcrConversionFeatures;
         const VkPhysicalDeviceSamplerYcbcrConversionFeatures *b = (const void *) features;
         if (b->samplerYcbcrConversion && !a->samplerYcbcrConversion)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSamplerYcbcrConversionFeatures", "samplerYcbcrConversion");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0)))
            break;
         const VkPhysicalDeviceProtectedMemoryFeatures *a = &supported_VkPhysicalDeviceProtectedMemoryFeatures;
         const VkPhysicalDeviceProtectedMemoryFeatures *b = (const void *) features;
         if (b->protectedMemory && !a->protectedMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceProtectedMemoryFeatures", "protectedMemory");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_blend_operation_advanced))
            break;
         const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *a = &supported_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT;
         const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *b = (const void *) features;
         if (b->advancedBlendCoherentOperations && !a->advancedBlendCoherentOperations)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT", "advancedBlendCoherentOperations");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_multi_draw))
            break;
         const VkPhysicalDeviceMultiDrawFeaturesEXT *a = &supported_VkPhysicalDeviceMultiDrawFeaturesEXT;
         const VkPhysicalDeviceMultiDrawFeaturesEXT *b = (const void *) features;
         if (b->multiDraw && !a->multiDraw)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiDrawFeaturesEXT", "multiDraw");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_inline_uniform_block))
            break;
         const VkPhysicalDeviceInlineUniformBlockFeatures *a = &supported_VkPhysicalDeviceInlineUniformBlockFeatures;
         const VkPhysicalDeviceInlineUniformBlockFeatures *b = (const void *) features;
         if (b->inlineUniformBlock && !a->inlineUniformBlock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceInlineUniformBlockFeatures", "inlineUniformBlock");
         if (b->descriptorBindingInlineUniformBlockUpdateAfterBind && !a->descriptorBindingInlineUniformBlockUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceInlineUniformBlockFeatures", "descriptorBindingInlineUniformBlockUpdateAfterBind");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_maintenance4))
            break;
         const VkPhysicalDeviceMaintenance4Features *a = &supported_VkPhysicalDeviceMaintenance4Features;
         const VkPhysicalDeviceMaintenance4Features *b = (const void *) features;
         if (b->maintenance4 && !a->maintenance4)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance4Features", "maintenance4");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_maintenance5))
            break;
         const VkPhysicalDeviceMaintenance5Features *a = &supported_VkPhysicalDeviceMaintenance5Features;
         const VkPhysicalDeviceMaintenance5Features *b = (const void *) features;
         if (b->maintenance5 && !a->maintenance5)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance5Features", "maintenance5");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_maintenance6))
            break;
         const VkPhysicalDeviceMaintenance6Features *a = &supported_VkPhysicalDeviceMaintenance6Features;
         const VkPhysicalDeviceMaintenance6Features *b = (const void *) features;
         if (b->maintenance6 && !a->maintenance6)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance6Features", "maintenance6");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_maintenance7))
            break;
         const VkPhysicalDeviceMaintenance7FeaturesKHR *a = &supported_VkPhysicalDeviceMaintenance7FeaturesKHR;
         const VkPhysicalDeviceMaintenance7FeaturesKHR *b = (const void *) features;
         if (b->maintenance7 && !a->maintenance7)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance7FeaturesKHR", "maintenance7");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_maintenance8))
            break;
         const VkPhysicalDeviceMaintenance8FeaturesKHR *a = &supported_VkPhysicalDeviceMaintenance8FeaturesKHR;
         const VkPhysicalDeviceMaintenance8FeaturesKHR *b = (const void *) features;
         if (b->maintenance8 && !a->maintenance8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance8FeaturesKHR", "maintenance8");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_maintenance9))
            break;
         const VkPhysicalDeviceMaintenance9FeaturesKHR *a = &supported_VkPhysicalDeviceMaintenance9FeaturesKHR;
         const VkPhysicalDeviceMaintenance9FeaturesKHR *b = (const void *) features;
         if (b->maintenance9 && !a->maintenance9)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance9FeaturesKHR", "maintenance9");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_maintenance11))
            break;
         const VkPhysicalDeviceMaintenance11FeaturesKHR *a = &supported_VkPhysicalDeviceMaintenance11FeaturesKHR;
         const VkPhysicalDeviceMaintenance11FeaturesKHR *b = (const void *) features;
         if (b->maintenance11 && !a->maintenance11)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance11FeaturesKHR", "maintenance11");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_maintenance10))
            break;
         const VkPhysicalDeviceMaintenance10FeaturesKHR *a = &supported_VkPhysicalDeviceMaintenance10FeaturesKHR;
         const VkPhysicalDeviceMaintenance10FeaturesKHR *b = (const void *) features;
         if (b->maintenance10 && !a->maintenance10)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMaintenance10FeaturesKHR", "maintenance10");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 1, 0)))
            break;
         const VkPhysicalDeviceShaderDrawParametersFeatures *a = &supported_VkPhysicalDeviceShaderDrawParametersFeatures;
         const VkPhysicalDeviceShaderDrawParametersFeatures *b = (const void *) features;
         if (b->shaderDrawParameters && !a->shaderDrawParameters)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderDrawParametersFeatures", "shaderDrawParameters");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_float16_int8))
            break;
         const VkPhysicalDeviceShaderFloat16Int8Features *a = &supported_VkPhysicalDeviceShaderFloat16Int8Features;
         const VkPhysicalDeviceShaderFloat16Int8Features *b = (const void *) features;
         if (b->shaderFloat16 && !a->shaderFloat16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFloat16Int8Features", "shaderFloat16");
         if (b->shaderInt8 && !a->shaderInt8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFloat16Int8Features", "shaderInt8");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_host_query_reset))
            break;
         const VkPhysicalDeviceHostQueryResetFeatures *a = &supported_VkPhysicalDeviceHostQueryResetFeatures;
         const VkPhysicalDeviceHostQueryResetFeatures *b = (const void *) features;
         if (b->hostQueryReset && !a->hostQueryReset)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceHostQueryResetFeatures", "hostQueryReset");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_elapsed_timer_query))
            break;
         const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM *a = &supported_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM;
         const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM *b = (const void *) features;
         if (b->elapsedTimerQuery && !a->elapsedTimerQuery)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM", "elapsedTimerQuery");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_global_priority || physical_device->supported_extensions.EXT_global_priority_query))
            break;
         const VkPhysicalDeviceGlobalPriorityQueryFeatures *a = &supported_VkPhysicalDeviceGlobalPriorityQueryFeatures;
         const VkPhysicalDeviceGlobalPriorityQueryFeatures *b = (const void *) features;
         if (b->globalPriorityQuery && !a->globalPriorityQuery)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGlobalPriorityQueryFeatures", "globalPriorityQuery");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_device_memory_report))
            break;
         const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *a = &supported_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT;
         const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *b = (const void *) features;
         if (b->deviceMemoryReport && !a->deviceMemoryReport)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceMemoryReportFeaturesEXT", "deviceMemoryReport");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_descriptor_indexing))
            break;
         const VkPhysicalDeviceDescriptorIndexingFeatures *a = &supported_VkPhysicalDeviceDescriptorIndexingFeatures;
         const VkPhysicalDeviceDescriptorIndexingFeatures *b = (const void *) features;
         if (b->shaderInputAttachmentArrayDynamicIndexing && !a->shaderInputAttachmentArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderInputAttachmentArrayDynamicIndexing");
         if (b->shaderUniformTexelBufferArrayDynamicIndexing && !a->shaderUniformTexelBufferArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderUniformTexelBufferArrayDynamicIndexing");
         if (b->shaderStorageTexelBufferArrayDynamicIndexing && !a->shaderStorageTexelBufferArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderStorageTexelBufferArrayDynamicIndexing");
         if (b->shaderUniformBufferArrayNonUniformIndexing && !a->shaderUniformBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderUniformBufferArrayNonUniformIndexing");
         if (b->shaderSampledImageArrayNonUniformIndexing && !a->shaderSampledImageArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderSampledImageArrayNonUniformIndexing");
         if (b->shaderStorageBufferArrayNonUniformIndexing && !a->shaderStorageBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderStorageBufferArrayNonUniformIndexing");
         if (b->shaderStorageImageArrayNonUniformIndexing && !a->shaderStorageImageArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderStorageImageArrayNonUniformIndexing");
         if (b->shaderInputAttachmentArrayNonUniformIndexing && !a->shaderInputAttachmentArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderInputAttachmentArrayNonUniformIndexing");
         if (b->shaderUniformTexelBufferArrayNonUniformIndexing && !a->shaderUniformTexelBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderUniformTexelBufferArrayNonUniformIndexing");
         if (b->shaderStorageTexelBufferArrayNonUniformIndexing && !a->shaderStorageTexelBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "shaderStorageTexelBufferArrayNonUniformIndexing");
         if (b->descriptorBindingUniformBufferUpdateAfterBind && !a->descriptorBindingUniformBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingUniformBufferUpdateAfterBind");
         if (b->descriptorBindingSampledImageUpdateAfterBind && !a->descriptorBindingSampledImageUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingSampledImageUpdateAfterBind");
         if (b->descriptorBindingStorageImageUpdateAfterBind && !a->descriptorBindingStorageImageUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingStorageImageUpdateAfterBind");
         if (b->descriptorBindingStorageBufferUpdateAfterBind && !a->descriptorBindingStorageBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingStorageBufferUpdateAfterBind");
         if (b->descriptorBindingUniformTexelBufferUpdateAfterBind && !a->descriptorBindingUniformTexelBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingUniformTexelBufferUpdateAfterBind");
         if (b->descriptorBindingStorageTexelBufferUpdateAfterBind && !a->descriptorBindingStorageTexelBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingStorageTexelBufferUpdateAfterBind");
         if (b->descriptorBindingUpdateUnusedWhilePending && !a->descriptorBindingUpdateUnusedWhilePending)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingUpdateUnusedWhilePending");
         if (b->descriptorBindingPartiallyBound && !a->descriptorBindingPartiallyBound)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingPartiallyBound");
         if (b->descriptorBindingVariableDescriptorCount && !a->descriptorBindingVariableDescriptorCount)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "descriptorBindingVariableDescriptorCount");
         if (b->runtimeDescriptorArray && !a->runtimeDescriptorArray)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorIndexingFeatures", "runtimeDescriptorArray");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_timeline_semaphore))
            break;
         const VkPhysicalDeviceTimelineSemaphoreFeatures *a = &supported_VkPhysicalDeviceTimelineSemaphoreFeatures;
         const VkPhysicalDeviceTimelineSemaphoreFeatures *b = (const void *) features;
         if (b->timelineSemaphore && !a->timelineSemaphore)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTimelineSemaphoreFeatures", "timelineSemaphore");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_8bit_storage))
            break;
         const VkPhysicalDevice8BitStorageFeatures *a = &supported_VkPhysicalDevice8BitStorageFeatures;
         const VkPhysicalDevice8BitStorageFeatures *b = (const void *) features;
         if (b->storageBuffer8BitAccess && !a->storageBuffer8BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess");
         if (b->uniformAndStorageBuffer8BitAccess && !a->uniformAndStorageBuffer8BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice8BitStorageFeatures", "uniformAndStorageBuffer8BitAccess");
         if (b->storagePushConstant8 && !a->storagePushConstant8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice8BitStorageFeatures", "storagePushConstant8");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_conditional_rendering))
            break;
         const VkPhysicalDeviceConditionalRenderingFeaturesEXT *a = &supported_VkPhysicalDeviceConditionalRenderingFeaturesEXT;
         const VkPhysicalDeviceConditionalRenderingFeaturesEXT *b = (const void *) features;
         if (b->conditionalRendering && !a->conditionalRendering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceConditionalRenderingFeaturesEXT", "conditionalRendering");
         if (b->inheritedConditionalRendering && !a->inheritedConditionalRendering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceConditionalRenderingFeaturesEXT", "inheritedConditionalRendering");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_vulkan_memory_model))
            break;
         const VkPhysicalDeviceVulkanMemoryModelFeatures *a = &supported_VkPhysicalDeviceVulkanMemoryModelFeatures;
         const VkPhysicalDeviceVulkanMemoryModelFeatures *b = (const void *) features;
         if (b->vulkanMemoryModel && !a->vulkanMemoryModel)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkanMemoryModelFeatures", "vulkanMemoryModel");
         if (b->vulkanMemoryModelDeviceScope && !a->vulkanMemoryModelDeviceScope)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkanMemoryModelFeatures", "vulkanMemoryModelDeviceScope");
         if (b->vulkanMemoryModelAvailabilityVisibilityChains && !a->vulkanMemoryModelAvailabilityVisibilityChains)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkanMemoryModelFeatures", "vulkanMemoryModelAvailabilityVisibilityChains");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_shader_atomic_int64))
            break;
         const VkPhysicalDeviceShaderAtomicInt64Features *a = &supported_VkPhysicalDeviceShaderAtomicInt64Features;
         const VkPhysicalDeviceShaderAtomicInt64Features *b = (const void *) features;
         if (b->shaderBufferInt64Atomics && !a->shaderBufferInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicInt64Features", "shaderBufferInt64Atomics");
         if (b->shaderSharedInt64Atomics && !a->shaderSharedInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicInt64Features", "shaderSharedInt64Atomics");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_atomic_float))
            break;
         const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *a = &supported_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT;
         const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *b = (const void *) features;
         if (b->shaderBufferFloat32Atomics && !a->shaderBufferFloat32Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderBufferFloat32Atomics");
         if (b->shaderBufferFloat32AtomicAdd && !a->shaderBufferFloat32AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderBufferFloat32AtomicAdd");
         if (b->shaderBufferFloat64Atomics && !a->shaderBufferFloat64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderBufferFloat64Atomics");
         if (b->shaderBufferFloat64AtomicAdd && !a->shaderBufferFloat64AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderBufferFloat64AtomicAdd");
         if (b->shaderSharedFloat32Atomics && !a->shaderSharedFloat32Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderSharedFloat32Atomics");
         if (b->shaderSharedFloat32AtomicAdd && !a->shaderSharedFloat32AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderSharedFloat32AtomicAdd");
         if (b->shaderSharedFloat64Atomics && !a->shaderSharedFloat64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderSharedFloat64Atomics");
         if (b->shaderSharedFloat64AtomicAdd && !a->shaderSharedFloat64AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderSharedFloat64AtomicAdd");
         if (b->shaderImageFloat32Atomics && !a->shaderImageFloat32Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderImageFloat32Atomics");
         if (b->shaderImageFloat32AtomicAdd && !a->shaderImageFloat32AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "shaderImageFloat32AtomicAdd");
         if (b->sparseImageFloat32Atomics && !a->sparseImageFloat32Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "sparseImageFloat32Atomics");
         if (b->sparseImageFloat32AtomicAdd && !a->sparseImageFloat32AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", "sparseImageFloat32AtomicAdd");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_atomic_float2))
            break;
         const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *a = &supported_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT;
         const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *b = (const void *) features;
         if (b->shaderBufferFloat16Atomics && !a->shaderBufferFloat16Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderBufferFloat16Atomics");
         if (b->shaderBufferFloat16AtomicAdd && !a->shaderBufferFloat16AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderBufferFloat16AtomicAdd");
         if (b->shaderBufferFloat16AtomicMinMax && !a->shaderBufferFloat16AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderBufferFloat16AtomicMinMax");
         if (b->shaderBufferFloat32AtomicMinMax && !a->shaderBufferFloat32AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderBufferFloat32AtomicMinMax");
         if (b->shaderBufferFloat64AtomicMinMax && !a->shaderBufferFloat64AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderBufferFloat64AtomicMinMax");
         if (b->shaderSharedFloat16Atomics && !a->shaderSharedFloat16Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderSharedFloat16Atomics");
         if (b->shaderSharedFloat16AtomicAdd && !a->shaderSharedFloat16AtomicAdd)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderSharedFloat16AtomicAdd");
         if (b->shaderSharedFloat16AtomicMinMax && !a->shaderSharedFloat16AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderSharedFloat16AtomicMinMax");
         if (b->shaderSharedFloat32AtomicMinMax && !a->shaderSharedFloat32AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderSharedFloat32AtomicMinMax");
         if (b->shaderSharedFloat64AtomicMinMax && !a->shaderSharedFloat64AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderSharedFloat64AtomicMinMax");
         if (b->shaderImageFloat32AtomicMinMax && !a->shaderImageFloat32AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "shaderImageFloat32AtomicMinMax");
         if (b->sparseImageFloat32AtomicMinMax && !a->sparseImageFloat32AtomicMinMax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", "sparseImageFloat32AtomicMinMax");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_vertex_attribute_divisor || physical_device->supported_extensions.KHR_vertex_attribute_divisor))
            break;
         const VkPhysicalDeviceVertexAttributeDivisorFeatures *a = &supported_VkPhysicalDeviceVertexAttributeDivisorFeatures;
         const VkPhysicalDeviceVertexAttributeDivisorFeatures *b = (const void *) features;
         if (b->vertexAttributeInstanceRateDivisor && !a->vertexAttributeInstanceRateDivisor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVertexAttributeDivisorFeatures", "vertexAttributeInstanceRateDivisor");
         if (b->vertexAttributeInstanceRateZeroDivisor && !a->vertexAttributeInstanceRateZeroDivisor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVertexAttributeDivisorFeatures", "vertexAttributeInstanceRateZeroDivisor");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_astc_decode_mode))
            break;
         const VkPhysicalDeviceASTCDecodeFeaturesEXT *a = &supported_VkPhysicalDeviceASTCDecodeFeaturesEXT;
         const VkPhysicalDeviceASTCDecodeFeaturesEXT *b = (const void *) features;
         if (b->decodeModeSharedExponent && !a->decodeModeSharedExponent)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceASTCDecodeFeaturesEXT", "decodeModeSharedExponent");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_transform_feedback))
            break;
         const VkPhysicalDeviceTransformFeedbackFeaturesEXT *a = &supported_VkPhysicalDeviceTransformFeedbackFeaturesEXT;
         const VkPhysicalDeviceTransformFeedbackFeaturesEXT *b = (const void *) features;
         if (b->transformFeedback && !a->transformFeedback)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTransformFeedbackFeaturesEXT", "transformFeedback");
         if (b->geometryStreams && !a->geometryStreams)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTransformFeedbackFeaturesEXT", "geometryStreams");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_representative_fragment_test))
            break;
         const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *a = &supported_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV;
         const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *b = (const void *) features;
         if (b->representativeFragmentTest && !a->representativeFragmentTest)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV", "representativeFragmentTest");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_scissor_exclusive))
            break;
         const VkPhysicalDeviceExclusiveScissorFeaturesNV *a = &supported_VkPhysicalDeviceExclusiveScissorFeaturesNV;
         const VkPhysicalDeviceExclusiveScissorFeaturesNV *b = (const void *) features;
         if (b->exclusiveScissor && !a->exclusiveScissor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExclusiveScissorFeaturesNV", "exclusiveScissor");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_corner_sampled_image))
            break;
         const VkPhysicalDeviceCornerSampledImageFeaturesNV *a = &supported_VkPhysicalDeviceCornerSampledImageFeaturesNV;
         const VkPhysicalDeviceCornerSampledImageFeaturesNV *b = (const void *) features;
         if (b->cornerSampledImage && !a->cornerSampledImage)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCornerSampledImageFeaturesNV", "cornerSampledImage");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.NV_compute_shader_derivatives || physical_device->supported_extensions.KHR_compute_shader_derivatives))
            break;
         const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR *a = &supported_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR;
         const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR *b = (const void *) features;
         if (b->computeDerivativeGroupQuads && !a->computeDerivativeGroupQuads)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR", "computeDerivativeGroupQuads");
         if (b->computeDerivativeGroupLinear && !a->computeDerivativeGroupLinear)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR", "computeDerivativeGroupLinear");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_shader_image_footprint))
            break;
         const VkPhysicalDeviceShaderImageFootprintFeaturesNV *a = &supported_VkPhysicalDeviceShaderImageFootprintFeaturesNV;
         const VkPhysicalDeviceShaderImageFootprintFeaturesNV *b = (const void *) features;
         if (b->imageFootprint && !a->imageFootprint)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderImageFootprintFeaturesNV", "imageFootprint");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_dedicated_allocation_image_aliasing))
            break;
         const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *a = &supported_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV;
         const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *b = (const void *) features;
         if (b->dedicatedAllocationImageAliasing && !a->dedicatedAllocationImageAliasing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV", "dedicatedAllocationImageAliasing");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_copy_memory_indirect))
            break;
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR *a = &supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR;
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR *b = (const void *) features;
         if (b->indirectMemoryCopy && !a->indirectMemoryCopy)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR", "indirectMemoryCopy");
         if (b->indirectMemoryToImageCopy && !a->indirectMemoryToImageCopy)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR", "indirectMemoryToImageCopy");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_copy_memory_indirect))
            break;
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV *a = &supported_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV;
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV *b = (const void *) features;
         if (b->indirectCopy && !a->indirectCopy)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCopyMemoryIndirectFeaturesNV", "indirectCopy");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.NV_memory_decompression || physical_device->supported_extensions.EXT_memory_decompression))
            break;
         const VkPhysicalDeviceMemoryDecompressionFeaturesEXT *a = &supported_VkPhysicalDeviceMemoryDecompressionFeaturesEXT;
         const VkPhysicalDeviceMemoryDecompressionFeaturesEXT *b = (const void *) features;
         if (b->memoryDecompression && !a->memoryDecompression)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMemoryDecompressionFeaturesEXT", "memoryDecompression");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_shading_rate_image))
            break;
         const VkPhysicalDeviceShadingRateImageFeaturesNV *a = &supported_VkPhysicalDeviceShadingRateImageFeaturesNV;
         const VkPhysicalDeviceShadingRateImageFeaturesNV *b = (const void *) features;
         if (b->shadingRateImage && !a->shadingRateImage)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShadingRateImageFeaturesNV", "shadingRateImage");
         if (b->shadingRateCoarseSampleOrder && !a->shadingRateCoarseSampleOrder)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShadingRateImageFeaturesNV", "shadingRateCoarseSampleOrder");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI: {
         if (!(physical_device->supported_extensions.HUAWEI_invocation_mask))
            break;
         const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *a = &supported_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI;
         const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *b = (const void *) features;
         if (b->invocationMask && !a->invocationMask)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceInvocationMaskFeaturesHUAWEI", "invocationMask");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_mesh_shader))
            break;
         const VkPhysicalDeviceMeshShaderFeaturesNV *a = &supported_VkPhysicalDeviceMeshShaderFeaturesNV;
         const VkPhysicalDeviceMeshShaderFeaturesNV *b = (const void *) features;
         if (b->taskShader && !a->taskShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesNV", "taskShader");
         if (b->meshShader && !a->meshShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesNV", "meshShader");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_mesh_shader))
            break;
         const VkPhysicalDeviceMeshShaderFeaturesEXT *a = &supported_VkPhysicalDeviceMeshShaderFeaturesEXT;
         const VkPhysicalDeviceMeshShaderFeaturesEXT *b = (const void *) features;
         if (b->taskShader && !a->taskShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesEXT", "taskShader");
         if (b->meshShader && !a->meshShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesEXT", "meshShader");
         if (b->multiviewMeshShader && !a->multiviewMeshShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesEXT", "multiviewMeshShader");
         if (b->primitiveFragmentShadingRateMeshShader && !a->primitiveFragmentShadingRateMeshShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesEXT", "primitiveFragmentShadingRateMeshShader");
         if (b->meshShaderQueries && !a->meshShaderQueries)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMeshShaderFeaturesEXT", "meshShaderQueries");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_acceleration_structure))
            break;
         const VkPhysicalDeviceAccelerationStructureFeaturesKHR *a = &supported_VkPhysicalDeviceAccelerationStructureFeaturesKHR;
         const VkPhysicalDeviceAccelerationStructureFeaturesKHR *b = (const void *) features;
         if (b->accelerationStructure && !a->accelerationStructure)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAccelerationStructureFeaturesKHR", "accelerationStructure");
         if (b->accelerationStructureCaptureReplay && !a->accelerationStructureCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAccelerationStructureFeaturesKHR", "accelerationStructureCaptureReplay");
         if (b->accelerationStructureIndirectBuild && !a->accelerationStructureIndirectBuild)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAccelerationStructureFeaturesKHR", "accelerationStructureIndirectBuild");
         if (b->accelerationStructureHostCommands && !a->accelerationStructureHostCommands)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAccelerationStructureFeaturesKHR", "accelerationStructureHostCommands");
         if (b->descriptorBindingAccelerationStructureUpdateAfterBind && !a->descriptorBindingAccelerationStructureUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAccelerationStructureFeaturesKHR", "descriptorBindingAccelerationStructureUpdateAfterBind");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_ray_tracing_pipeline))
            break;
         const VkPhysicalDeviceRayTracingPipelineFeaturesKHR *a = &supported_VkPhysicalDeviceRayTracingPipelineFeaturesKHR;
         const VkPhysicalDeviceRayTracingPipelineFeaturesKHR *b = (const void *) features;
         if (b->rayTracingPipeline && !a->rayTracingPipeline)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", "rayTracingPipeline");
         if (b->rayTracingPipelineShaderGroupHandleCaptureReplay && !a->rayTracingPipelineShaderGroupHandleCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", "rayTracingPipelineShaderGroupHandleCaptureReplay");
         if (b->rayTracingPipelineShaderGroupHandleCaptureReplayMixed && !a->rayTracingPipelineShaderGroupHandleCaptureReplayMixed)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", "rayTracingPipelineShaderGroupHandleCaptureReplayMixed");
         if (b->rayTracingPipelineTraceRaysIndirect && !a->rayTracingPipelineTraceRaysIndirect)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", "rayTracingPipelineTraceRaysIndirect");
         if (b->rayTraversalPrimitiveCulling && !a->rayTraversalPrimitiveCulling)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", "rayTraversalPrimitiveCulling");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_ray_query))
            break;
         const VkPhysicalDeviceRayQueryFeaturesKHR *a = &supported_VkPhysicalDeviceRayQueryFeaturesKHR;
         const VkPhysicalDeviceRayQueryFeaturesKHR *b = (const void *) features;
         if (b->rayQuery && !a->rayQuery)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayQueryFeaturesKHR", "rayQuery");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_ray_tracing_maintenance1))
            break;
         const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *a = &supported_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR;
         const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *b = (const void *) features;
         if (b->rayTracingMaintenance1 && !a->rayTracingMaintenance1)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR", "rayTracingMaintenance1");
         if (b->rayTracingPipelineTraceRaysIndirect2 && !a->rayTracingPipelineTraceRaysIndirect2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR", "rayTracingPipelineTraceRaysIndirect2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_fragment_density_map))
            break;
         const VkPhysicalDeviceFragmentDensityMapFeaturesEXT *a = &supported_VkPhysicalDeviceFragmentDensityMapFeaturesEXT;
         const VkPhysicalDeviceFragmentDensityMapFeaturesEXT *b = (const void *) features;
         if (b->fragmentDensityMap && !a->fragmentDensityMap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMapFeaturesEXT", "fragmentDensityMap");
         if (b->fragmentDensityMapDynamic && !a->fragmentDensityMapDynamic)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMapFeaturesEXT", "fragmentDensityMapDynamic");
         if (b->fragmentDensityMapNonSubsampledImages && !a->fragmentDensityMapNonSubsampledImages)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMapFeaturesEXT", "fragmentDensityMapNonSubsampledImages");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_fragment_density_map2))
            break;
         const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *a = &supported_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT;
         const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *b = (const void *) features;
         if (b->fragmentDensityMapDeferred && !a->fragmentDensityMapDeferred)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMap2FeaturesEXT", "fragmentDensityMapDeferred");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.QCOM_fragment_density_map_offset || physical_device->supported_extensions.EXT_fragment_density_map_offset))
            break;
         const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT *a = &supported_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT;
         const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT *b = (const void *) features;
         if (b->fragmentDensityMapOffset && !a->fragmentDensityMapOffset)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT", "fragmentDensityMapOffset");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.EXT_scalar_block_layout))
            break;
         const VkPhysicalDeviceScalarBlockLayoutFeatures *a = &supported_VkPhysicalDeviceScalarBlockLayoutFeatures;
         const VkPhysicalDeviceScalarBlockLayoutFeatures *b = (const void *) features;
         if (b->scalarBlockLayout && !a->scalarBlockLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceScalarBlockLayoutFeatures", "scalarBlockLayout");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_uniform_buffer_standard_layout))
            break;
         const VkPhysicalDeviceUniformBufferStandardLayoutFeatures *a = &supported_VkPhysicalDeviceUniformBufferStandardLayoutFeatures;
         const VkPhysicalDeviceUniformBufferStandardLayoutFeatures *b = (const void *) features;
         if (b->uniformBufferStandardLayout && !a->uniformBufferStandardLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceUniformBufferStandardLayoutFeatures", "uniformBufferStandardLayout");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_depth_clip_enable))
            break;
         const VkPhysicalDeviceDepthClipEnableFeaturesEXT *a = &supported_VkPhysicalDeviceDepthClipEnableFeaturesEXT;
         const VkPhysicalDeviceDepthClipEnableFeaturesEXT *b = (const void *) features;
         if (b->depthClipEnable && !a->depthClipEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthClipEnableFeaturesEXT", "depthClipEnable");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_memory_priority))
            break;
         const VkPhysicalDeviceMemoryPriorityFeaturesEXT *a = &supported_VkPhysicalDeviceMemoryPriorityFeaturesEXT;
         const VkPhysicalDeviceMemoryPriorityFeaturesEXT *b = (const void *) features;
         if (b->memoryPriority && !a->memoryPriority)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMemoryPriorityFeaturesEXT", "memoryPriority");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_pageable_device_local_memory))
            break;
         const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *a = &supported_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT;
         const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *b = (const void *) features;
         if (b->pageableDeviceLocalMemory && !a->pageableDeviceLocalMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT", "pageableDeviceLocalMemory");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_buffer_device_address))
            break;
         const VkPhysicalDeviceBufferDeviceAddressFeatures *a = &supported_VkPhysicalDeviceBufferDeviceAddressFeatures;
         const VkPhysicalDeviceBufferDeviceAddressFeatures *b = (const void *) features;
         if (b->bufferDeviceAddress && !a->bufferDeviceAddress)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeatures", "bufferDeviceAddress");
         if (b->bufferDeviceAddressCaptureReplay && !a->bufferDeviceAddressCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeatures", "bufferDeviceAddressCaptureReplay");
         if (b->bufferDeviceAddressMultiDevice && !a->bufferDeviceAddressMultiDevice)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeatures", "bufferDeviceAddressMultiDevice");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_buffer_device_address))
            break;
         const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT *a = &supported_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT;
         const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT *b = (const void *) features;
         if (b->bufferDeviceAddress && !a->bufferDeviceAddress)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT", "bufferDeviceAddress");
         if (b->bufferDeviceAddressCaptureReplay && !a->bufferDeviceAddressCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT", "bufferDeviceAddressCaptureReplay");
         if (b->bufferDeviceAddressMultiDevice && !a->bufferDeviceAddressMultiDevice)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT", "bufferDeviceAddressMultiDevice");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_imageless_framebuffer))
            break;
         const VkPhysicalDeviceImagelessFramebufferFeatures *a = &supported_VkPhysicalDeviceImagelessFramebufferFeatures;
         const VkPhysicalDeviceImagelessFramebufferFeatures *b = (const void *) features;
         if (b->imagelessFramebuffer && !a->imagelessFramebuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImagelessFramebufferFeatures", "imagelessFramebuffer");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_texture_compression_astc_hdr))
            break;
         const VkPhysicalDeviceTextureCompressionASTCHDRFeatures *a = &supported_VkPhysicalDeviceTextureCompressionASTCHDRFeatures;
         const VkPhysicalDeviceTextureCompressionASTCHDRFeatures *b = (const void *) features;
         if (b->textureCompressionASTC_HDR && !a->textureCompressionASTC_HDR)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTextureCompressionASTCHDRFeatures", "textureCompressionASTC_HDR");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_cooperative_matrix))
            break;
         const VkPhysicalDeviceCooperativeMatrixFeaturesNV *a = &supported_VkPhysicalDeviceCooperativeMatrixFeaturesNV;
         const VkPhysicalDeviceCooperativeMatrixFeaturesNV *b = (const void *) features;
         if (b->cooperativeMatrix && !a->cooperativeMatrix)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixFeaturesNV", "cooperativeMatrix");
         if (b->cooperativeMatrixRobustBufferAccess && !a->cooperativeMatrixRobustBufferAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixFeaturesNV", "cooperativeMatrixRobustBufferAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_ycbcr_image_arrays))
            break;
         const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *a = &supported_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT;
         const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *b = (const void *) features;
         if (b->ycbcrImageArrays && !a->ycbcrImageArrays)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceYcbcrImageArraysFeaturesEXT", "ycbcrImageArrays");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_present_barrier))
            break;
         const VkPhysicalDevicePresentBarrierFeaturesNV *a = &supported_VkPhysicalDevicePresentBarrierFeaturesNV;
         const VkPhysicalDevicePresentBarrierFeaturesNV *b = (const void *) features;
         if (b->presentBarrier && !a->presentBarrier)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentBarrierFeaturesNV", "presentBarrier");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_performance_query))
            break;
         const VkPhysicalDevicePerformanceQueryFeaturesKHR *a = &supported_VkPhysicalDevicePerformanceQueryFeaturesKHR;
         const VkPhysicalDevicePerformanceQueryFeaturesKHR *b = (const void *) features;
         if (b->performanceCounterQueryPools && !a->performanceCounterQueryPools)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePerformanceQueryFeaturesKHR", "performanceCounterQueryPools");
         if (b->performanceCounterMultipleQueryPools && !a->performanceCounterMultipleQueryPools)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePerformanceQueryFeaturesKHR", "performanceCounterMultipleQueryPools");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_coverage_reduction_mode))
            break;
         const VkPhysicalDeviceCoverageReductionModeFeaturesNV *a = &supported_VkPhysicalDeviceCoverageReductionModeFeaturesNV;
         const VkPhysicalDeviceCoverageReductionModeFeaturesNV *b = (const void *) features;
         if (b->coverageReductionMode && !a->coverageReductionMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCoverageReductionModeFeaturesNV", "coverageReductionMode");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL: {
         if (!(physical_device->supported_extensions.INTEL_shader_integer_functions2))
            break;
         const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *a = &supported_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL;
         const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *b = (const void *) features;
         if (b->shaderIntegerFunctions2 && !a->shaderIntegerFunctions2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL", "shaderIntegerFunctions2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_clock))
            break;
         const VkPhysicalDeviceShaderClockFeaturesKHR *a = &supported_VkPhysicalDeviceShaderClockFeaturesKHR;
         const VkPhysicalDeviceShaderClockFeaturesKHR *b = (const void *) features;
         if (b->shaderSubgroupClock && !a->shaderSubgroupClock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderClockFeaturesKHR", "shaderSubgroupClock");
         if (b->shaderDeviceClock && !a->shaderDeviceClock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderClockFeaturesKHR", "shaderDeviceClock");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_index_type_uint8 || physical_device->supported_extensions.KHR_index_type_uint8))
            break;
         const VkPhysicalDeviceIndexTypeUint8Features *a = &supported_VkPhysicalDeviceIndexTypeUint8Features;
         const VkPhysicalDeviceIndexTypeUint8Features *b = (const void *) features;
         if (b->indexTypeUint8 && !a->indexTypeUint8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceIndexTypeUint8Features", "indexTypeUint8");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_shader_sm_builtins))
            break;
         const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *a = &supported_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV;
         const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *b = (const void *) features;
         if (b->shaderSMBuiltins && !a->shaderSMBuiltins)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSMBuiltinsFeaturesNV", "shaderSMBuiltins");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_fragment_shader_interlock))
            break;
         const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *a = &supported_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT;
         const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *b = (const void *) features;
         if (b->fragmentShaderSampleInterlock && !a->fragmentShaderSampleInterlock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT", "fragmentShaderSampleInterlock");
         if (b->fragmentShaderPixelInterlock && !a->fragmentShaderPixelInterlock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT", "fragmentShaderPixelInterlock");
         if (b->fragmentShaderShadingRateInterlock && !a->fragmentShaderShadingRateInterlock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT", "fragmentShaderShadingRateInterlock");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0) || physical_device->supported_extensions.KHR_separate_depth_stencil_layouts))
            break;
         const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures *a = &supported_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures;
         const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures *b = (const void *) features;
         if (b->separateDepthStencilLayouts && !a->separateDepthStencilLayouts)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures", "separateDepthStencilLayouts");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_primitive_topology_list_restart))
            break;
         const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *a = &supported_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT;
         const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *b = (const void *) features;
         if (b->primitiveTopologyListRestart && !a->primitiveTopologyListRestart)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT", "primitiveTopologyListRestart");
         if (b->primitiveTopologyPatchListRestart && !a->primitiveTopologyPatchListRestart)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT", "primitiveTopologyPatchListRestart");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_pipeline_executable_properties))
            break;
         const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *a = &supported_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR;
         const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *b = (const void *) features;
         if (b->pipelineExecutableInfo && !a->pipelineExecutableInfo)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR", "pipelineExecutableInfo");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_shader_demote_to_helper_invocation))
            break;
         const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures *a = &supported_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures;
         const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures *b = (const void *) features;
         if (b->shaderDemoteToHelperInvocation && !a->shaderDemoteToHelperInvocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures", "shaderDemoteToHelperInvocation");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_texel_buffer_alignment))
            break;
         const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *a = &supported_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT;
         const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *b = (const void *) features;
         if (b->texelBufferAlignment && !a->texelBufferAlignment)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT", "texelBufferAlignment");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_subgroup_size_control))
            break;
         const VkPhysicalDeviceSubgroupSizeControlFeatures *a = &supported_VkPhysicalDeviceSubgroupSizeControlFeatures;
         const VkPhysicalDeviceSubgroupSizeControlFeatures *b = (const void *) features;
         if (b->subgroupSizeControl && !a->subgroupSizeControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSubgroupSizeControlFeatures", "subgroupSizeControl");
         if (b->computeFullSubgroups && !a->computeFullSubgroups)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSubgroupSizeControlFeatures", "computeFullSubgroups");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_line_rasterization || physical_device->supported_extensions.KHR_line_rasterization))
            break;
         const VkPhysicalDeviceLineRasterizationFeatures *a = &supported_VkPhysicalDeviceLineRasterizationFeatures;
         const VkPhysicalDeviceLineRasterizationFeatures *b = (const void *) features;
         if (b->rectangularLines && !a->rectangularLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "rectangularLines");
         if (b->bresenhamLines && !a->bresenhamLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "bresenhamLines");
         if (b->smoothLines && !a->smoothLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "smoothLines");
         if (b->stippledRectangularLines && !a->stippledRectangularLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "stippledRectangularLines");
         if (b->stippledBresenhamLines && !a->stippledBresenhamLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "stippledBresenhamLines");
         if (b->stippledSmoothLines && !a->stippledSmoothLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLineRasterizationFeatures", "stippledSmoothLines");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_pipeline_creation_cache_control))
            break;
         const VkPhysicalDevicePipelineCreationCacheControlFeatures *a = &supported_VkPhysicalDevicePipelineCreationCacheControlFeatures;
         const VkPhysicalDevicePipelineCreationCacheControlFeatures *b = (const void *) features;
         if (b->pipelineCreationCacheControl && !a->pipelineCreationCacheControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineCreationCacheControlFeatures", "pipelineCreationCacheControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0)))
            break;
         const VkPhysicalDeviceVulkan11Features *a = &supported_VkPhysicalDeviceVulkan11Features;
         const VkPhysicalDeviceVulkan11Features *b = (const void *) features;
         if (b->storageBuffer16BitAccess && !a->storageBuffer16BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "storageBuffer16BitAccess");
         if (b->uniformAndStorageBuffer16BitAccess && !a->uniformAndStorageBuffer16BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "uniformAndStorageBuffer16BitAccess");
         if (b->storagePushConstant16 && !a->storagePushConstant16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "storagePushConstant16");
         if (b->storageInputOutput16 && !a->storageInputOutput16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "storageInputOutput16");
         if (b->multiview && !a->multiview)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "multiview");
         if (b->multiviewGeometryShader && !a->multiviewGeometryShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "multiviewGeometryShader");
         if (b->multiviewTessellationShader && !a->multiviewTessellationShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "multiviewTessellationShader");
         if (b->variablePointersStorageBuffer && !a->variablePointersStorageBuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "variablePointersStorageBuffer");
         if (b->variablePointers && !a->variablePointers)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "variablePointers");
         if (b->protectedMemory && !a->protectedMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "protectedMemory");
         if (b->samplerYcbcrConversion && !a->samplerYcbcrConversion)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "samplerYcbcrConversion");
         if (b->shaderDrawParameters && !a->shaderDrawParameters)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan11Features", "shaderDrawParameters");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 2, 0)))
            break;
         const VkPhysicalDeviceVulkan12Features *a = &supported_VkPhysicalDeviceVulkan12Features;
         const VkPhysicalDeviceVulkan12Features *b = (const void *) features;
         if (b->samplerMirrorClampToEdge && !a->samplerMirrorClampToEdge)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge");
         if (b->drawIndirectCount && !a->drawIndirectCount)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "drawIndirectCount");
         if (b->storageBuffer8BitAccess && !a->storageBuffer8BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess");
         if (b->uniformAndStorageBuffer8BitAccess && !a->uniformAndStorageBuffer8BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "uniformAndStorageBuffer8BitAccess");
         if (b->storagePushConstant8 && !a->storagePushConstant8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "storagePushConstant8");
         if (b->shaderBufferInt64Atomics && !a->shaderBufferInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderBufferInt64Atomics");
         if (b->shaderSharedInt64Atomics && !a->shaderSharedInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderSharedInt64Atomics");
         if (b->shaderFloat16 && !a->shaderFloat16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderFloat16");
         if (b->shaderInt8 && !a->shaderInt8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderInt8");
         if (b->descriptorIndexing && !a->descriptorIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorIndexing");
         if (b->shaderInputAttachmentArrayDynamicIndexing && !a->shaderInputAttachmentArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderInputAttachmentArrayDynamicIndexing");
         if (b->shaderUniformTexelBufferArrayDynamicIndexing && !a->shaderUniformTexelBufferArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderUniformTexelBufferArrayDynamicIndexing");
         if (b->shaderStorageTexelBufferArrayDynamicIndexing && !a->shaderStorageTexelBufferArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderStorageTexelBufferArrayDynamicIndexing");
         if (b->shaderUniformBufferArrayNonUniformIndexing && !a->shaderUniformBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderUniformBufferArrayNonUniformIndexing");
         if (b->shaderSampledImageArrayNonUniformIndexing && !a->shaderSampledImageArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderSampledImageArrayNonUniformIndexing");
         if (b->shaderStorageBufferArrayNonUniformIndexing && !a->shaderStorageBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderStorageBufferArrayNonUniformIndexing");
         if (b->shaderStorageImageArrayNonUniformIndexing && !a->shaderStorageImageArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderStorageImageArrayNonUniformIndexing");
         if (b->shaderInputAttachmentArrayNonUniformIndexing && !a->shaderInputAttachmentArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderInputAttachmentArrayNonUniformIndexing");
         if (b->shaderUniformTexelBufferArrayNonUniformIndexing && !a->shaderUniformTexelBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderUniformTexelBufferArrayNonUniformIndexing");
         if (b->shaderStorageTexelBufferArrayNonUniformIndexing && !a->shaderStorageTexelBufferArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderStorageTexelBufferArrayNonUniformIndexing");
         if (b->descriptorBindingUniformBufferUpdateAfterBind && !a->descriptorBindingUniformBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingUniformBufferUpdateAfterBind");
         if (b->descriptorBindingSampledImageUpdateAfterBind && !a->descriptorBindingSampledImageUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingSampledImageUpdateAfterBind");
         if (b->descriptorBindingStorageImageUpdateAfterBind && !a->descriptorBindingStorageImageUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingStorageImageUpdateAfterBind");
         if (b->descriptorBindingStorageBufferUpdateAfterBind && !a->descriptorBindingStorageBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingStorageBufferUpdateAfterBind");
         if (b->descriptorBindingUniformTexelBufferUpdateAfterBind && !a->descriptorBindingUniformTexelBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingUniformTexelBufferUpdateAfterBind");
         if (b->descriptorBindingStorageTexelBufferUpdateAfterBind && !a->descriptorBindingStorageTexelBufferUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingStorageTexelBufferUpdateAfterBind");
         if (b->descriptorBindingUpdateUnusedWhilePending && !a->descriptorBindingUpdateUnusedWhilePending)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingUpdateUnusedWhilePending");
         if (b->descriptorBindingPartiallyBound && !a->descriptorBindingPartiallyBound)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingPartiallyBound");
         if (b->descriptorBindingVariableDescriptorCount && !a->descriptorBindingVariableDescriptorCount)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "descriptorBindingVariableDescriptorCount");
         if (b->runtimeDescriptorArray && !a->runtimeDescriptorArray)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "runtimeDescriptorArray");
         if (b->samplerFilterMinmax && !a->samplerFilterMinmax)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "samplerFilterMinmax");
         if (b->scalarBlockLayout && !a->scalarBlockLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "scalarBlockLayout");
         if (b->imagelessFramebuffer && !a->imagelessFramebuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "imagelessFramebuffer");
         if (b->uniformBufferStandardLayout && !a->uniformBufferStandardLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "uniformBufferStandardLayout");
         if (b->shaderSubgroupExtendedTypes && !a->shaderSubgroupExtendedTypes)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderSubgroupExtendedTypes");
         if (b->separateDepthStencilLayouts && !a->separateDepthStencilLayouts)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "separateDepthStencilLayouts");
         if (b->hostQueryReset && !a->hostQueryReset)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "hostQueryReset");
         if (b->timelineSemaphore && !a->timelineSemaphore)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "timelineSemaphore");
         if (b->bufferDeviceAddress && !a->bufferDeviceAddress)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "bufferDeviceAddress");
         if (b->bufferDeviceAddressCaptureReplay && !a->bufferDeviceAddressCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "bufferDeviceAddressCaptureReplay");
         if (b->bufferDeviceAddressMultiDevice && !a->bufferDeviceAddressMultiDevice)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "bufferDeviceAddressMultiDevice");
         if (b->vulkanMemoryModel && !a->vulkanMemoryModel)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "vulkanMemoryModel");
         if (b->vulkanMemoryModelDeviceScope && !a->vulkanMemoryModelDeviceScope)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "vulkanMemoryModelDeviceScope");
         if (b->vulkanMemoryModelAvailabilityVisibilityChains && !a->vulkanMemoryModelAvailabilityVisibilityChains)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "vulkanMemoryModelAvailabilityVisibilityChains");
         if (b->shaderOutputViewportIndex && !a->shaderOutputViewportIndex)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderOutputViewportIndex");
         if (b->shaderOutputLayer && !a->shaderOutputLayer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "shaderOutputLayer");
         if (b->subgroupBroadcastDynamicId && !a->subgroupBroadcastDynamicId)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan12Features", "subgroupBroadcastDynamicId");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0)))
            break;
         const VkPhysicalDeviceVulkan13Features *a = &supported_VkPhysicalDeviceVulkan13Features;
         const VkPhysicalDeviceVulkan13Features *b = (const void *) features;
         if (b->robustImageAccess && !a->robustImageAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "robustImageAccess");
         if (b->inlineUniformBlock && !a->inlineUniformBlock)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "inlineUniformBlock");
         if (b->descriptorBindingInlineUniformBlockUpdateAfterBind && !a->descriptorBindingInlineUniformBlockUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "descriptorBindingInlineUniformBlockUpdateAfterBind");
         if (b->pipelineCreationCacheControl && !a->pipelineCreationCacheControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "pipelineCreationCacheControl");
         if (b->privateData && !a->privateData)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "privateData");
         if (b->shaderDemoteToHelperInvocation && !a->shaderDemoteToHelperInvocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "shaderDemoteToHelperInvocation");
         if (b->shaderTerminateInvocation && !a->shaderTerminateInvocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "shaderTerminateInvocation");
         if (b->subgroupSizeControl && !a->subgroupSizeControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "subgroupSizeControl");
         if (b->computeFullSubgroups && !a->computeFullSubgroups)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "computeFullSubgroups");
         if (b->synchronization2 && !a->synchronization2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "synchronization2");
         if (b->textureCompressionASTC_HDR && !a->textureCompressionASTC_HDR)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "textureCompressionASTC_HDR");
         if (b->shaderZeroInitializeWorkgroupMemory && !a->shaderZeroInitializeWorkgroupMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "shaderZeroInitializeWorkgroupMemory");
         if (b->dynamicRendering && !a->dynamicRendering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "dynamicRendering");
         if (b->shaderIntegerDotProduct && !a->shaderIntegerDotProduct)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "shaderIntegerDotProduct");
         if (b->maintenance4 && !a->maintenance4)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan13Features", "maintenance4");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0)))
            break;
         const VkPhysicalDeviceVulkan14Features *a = &supported_VkPhysicalDeviceVulkan14Features;
         const VkPhysicalDeviceVulkan14Features *b = (const void *) features;
         if (b->globalPriorityQuery && !a->globalPriorityQuery)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "globalPriorityQuery");
         if (b->shaderSubgroupRotate && !a->shaderSubgroupRotate)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate");
         if (b->shaderSubgroupRotateClustered && !a->shaderSubgroupRotateClustered)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotateClustered");
         if (b->shaderFloatControls2 && !a->shaderFloatControls2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "shaderFloatControls2");
         if (b->shaderExpectAssume && !a->shaderExpectAssume)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "shaderExpectAssume");
         if (b->rectangularLines && !a->rectangularLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "rectangularLines");
         if (b->bresenhamLines && !a->bresenhamLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "bresenhamLines");
         if (b->smoothLines && !a->smoothLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "smoothLines");
         if (b->stippledRectangularLines && !a->stippledRectangularLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "stippledRectangularLines");
         if (b->stippledBresenhamLines && !a->stippledBresenhamLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "stippledBresenhamLines");
         if (b->stippledSmoothLines && !a->stippledSmoothLines)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "stippledSmoothLines");
         if (b->vertexAttributeInstanceRateDivisor && !a->vertexAttributeInstanceRateDivisor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "vertexAttributeInstanceRateDivisor");
         if (b->vertexAttributeInstanceRateZeroDivisor && !a->vertexAttributeInstanceRateZeroDivisor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "vertexAttributeInstanceRateZeroDivisor");
         if (b->indexTypeUint8 && !a->indexTypeUint8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "indexTypeUint8");
         if (b->dynamicRenderingLocalRead && !a->dynamicRenderingLocalRead)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "dynamicRenderingLocalRead");
         if (b->maintenance5 && !a->maintenance5)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "maintenance5");
         if (b->maintenance6 && !a->maintenance6)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "maintenance6");
         if (b->pipelineProtectedAccess && !a->pipelineProtectedAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "pipelineProtectedAccess");
         if (b->pipelineRobustness && !a->pipelineRobustness)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "pipelineRobustness");
         if (b->hostImageCopy && !a->hostImageCopy)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "hostImageCopy");
         if (b->pushDescriptor && !a->pushDescriptor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVulkan14Features", "pushDescriptor");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD: {
         if (!(physical_device->supported_extensions.AMD_device_coherent_memory))
            break;
         const VkPhysicalDeviceCoherentMemoryFeaturesAMD *a = &supported_VkPhysicalDeviceCoherentMemoryFeaturesAMD;
         const VkPhysicalDeviceCoherentMemoryFeaturesAMD *b = (const void *) features;
         if (b->deviceCoherentMemory && !a->deviceCoherentMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCoherentMemoryFeaturesAMD", "deviceCoherentMemory");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD: {
         if (!(physical_device->supported_extensions.AMD_gpa_interface))
            break;
         const VkPhysicalDeviceGpaFeaturesAMD *a = &supported_VkPhysicalDeviceGpaFeaturesAMD;
         const VkPhysicalDeviceGpaFeaturesAMD *b = (const void *) features;
         if (b->perfCounters && !a->perfCounters)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGpaFeaturesAMD", "perfCounters");
         if (b->streamingPerfCounters && !a->streamingPerfCounters)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGpaFeaturesAMD", "streamingPerfCounters");
         if (b->sqThreadTracing && !a->sqThreadTracing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGpaFeaturesAMD", "sqThreadTracing");
         if (b->clockModes && !a->clockModes)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGpaFeaturesAMD", "clockModes");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_custom_border_color))
            break;
         const VkPhysicalDeviceCustomBorderColorFeaturesEXT *a = &supported_VkPhysicalDeviceCustomBorderColorFeaturesEXT;
         const VkPhysicalDeviceCustomBorderColorFeaturesEXT *b = (const void *) features;
         if (b->customBorderColors && !a->customBorderColors)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCustomBorderColorFeaturesEXT", "customBorderColors");
         if (b->customBorderColorWithoutFormat && !a->customBorderColorWithoutFormat)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCustomBorderColorFeaturesEXT", "customBorderColorWithoutFormat");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_border_color_swizzle))
            break;
         const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *a = &supported_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT;
         const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *b = (const void *) features;
         if (b->borderColorSwizzle && !a->borderColorSwizzle)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBorderColorSwizzleFeaturesEXT", "borderColorSwizzle");
         if (b->borderColorSwizzleFromImage && !a->borderColorSwizzleFromImage)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceBorderColorSwizzleFeaturesEXT", "borderColorSwizzleFromImage");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state))
            break;
         const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *a = &supported_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT;
         const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *b = (const void *) features;
         if (b->extendedDynamicState && !a->extendedDynamicState)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicStateFeaturesEXT", "extendedDynamicState");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state2))
            break;
         const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *a = &supported_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT;
         const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *b = (const void *) features;
         if (b->extendedDynamicState2 && !a->extendedDynamicState2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT", "extendedDynamicState2");
         if (b->extendedDynamicState2LogicOp && !a->extendedDynamicState2LogicOp)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT", "extendedDynamicState2LogicOp");
         if (b->extendedDynamicState2PatchControlPoints && !a->extendedDynamicState2PatchControlPoints)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT", "extendedDynamicState2PatchControlPoints");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_extended_dynamic_state3))
            break;
         const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT *a = &supported_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT;
         const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT *b = (const void *) features;
         if (b->extendedDynamicState3TessellationDomainOrigin && !a->extendedDynamicState3TessellationDomainOrigin)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3TessellationDomainOrigin");
         if (b->extendedDynamicState3DepthClampEnable && !a->extendedDynamicState3DepthClampEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3DepthClampEnable");
         if (b->extendedDynamicState3PolygonMode && !a->extendedDynamicState3PolygonMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3PolygonMode");
         if (b->extendedDynamicState3RasterizationSamples && !a->extendedDynamicState3RasterizationSamples)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3RasterizationSamples");
         if (b->extendedDynamicState3SampleMask && !a->extendedDynamicState3SampleMask)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3SampleMask");
         if (b->extendedDynamicState3AlphaToCoverageEnable && !a->extendedDynamicState3AlphaToCoverageEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3AlphaToCoverageEnable");
         if (b->extendedDynamicState3AlphaToOneEnable && !a->extendedDynamicState3AlphaToOneEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3AlphaToOneEnable");
         if (b->extendedDynamicState3LogicOpEnable && !a->extendedDynamicState3LogicOpEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3LogicOpEnable");
         if (b->extendedDynamicState3ColorBlendEnable && !a->extendedDynamicState3ColorBlendEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ColorBlendEnable");
         if (b->extendedDynamicState3ColorBlendEquation && !a->extendedDynamicState3ColorBlendEquation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ColorBlendEquation");
         if (b->extendedDynamicState3ColorWriteMask && !a->extendedDynamicState3ColorWriteMask)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ColorWriteMask");
         if (b->extendedDynamicState3RasterizationStream && !a->extendedDynamicState3RasterizationStream)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3RasterizationStream");
         if (b->extendedDynamicState3ConservativeRasterizationMode && !a->extendedDynamicState3ConservativeRasterizationMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ConservativeRasterizationMode");
         if (b->extendedDynamicState3ExtraPrimitiveOverestimationSize && !a->extendedDynamicState3ExtraPrimitiveOverestimationSize)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ExtraPrimitiveOverestimationSize");
         if (b->extendedDynamicState3DepthClipEnable && !a->extendedDynamicState3DepthClipEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3DepthClipEnable");
         if (b->extendedDynamicState3SampleLocationsEnable && !a->extendedDynamicState3SampleLocationsEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3SampleLocationsEnable");
         if (b->extendedDynamicState3ColorBlendAdvanced && !a->extendedDynamicState3ColorBlendAdvanced)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ColorBlendAdvanced");
         if (b->extendedDynamicState3ProvokingVertexMode && !a->extendedDynamicState3ProvokingVertexMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ProvokingVertexMode");
         if (b->extendedDynamicState3LineRasterizationMode && !a->extendedDynamicState3LineRasterizationMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3LineRasterizationMode");
         if (b->extendedDynamicState3LineStippleEnable && !a->extendedDynamicState3LineStippleEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3LineStippleEnable");
         if (b->extendedDynamicState3DepthClipNegativeOneToOne && !a->extendedDynamicState3DepthClipNegativeOneToOne)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3DepthClipNegativeOneToOne");
         if (b->extendedDynamicState3ViewportWScalingEnable && !a->extendedDynamicState3ViewportWScalingEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ViewportWScalingEnable");
         if (b->extendedDynamicState3ViewportSwizzle && !a->extendedDynamicState3ViewportSwizzle)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ViewportSwizzle");
         if (b->extendedDynamicState3CoverageToColorEnable && !a->extendedDynamicState3CoverageToColorEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageToColorEnable");
         if (b->extendedDynamicState3CoverageToColorLocation && !a->extendedDynamicState3CoverageToColorLocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageToColorLocation");
         if (b->extendedDynamicState3CoverageModulationMode && !a->extendedDynamicState3CoverageModulationMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageModulationMode");
         if (b->extendedDynamicState3CoverageModulationTableEnable && !a->extendedDynamicState3CoverageModulationTableEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageModulationTableEnable");
         if (b->extendedDynamicState3CoverageModulationTable && !a->extendedDynamicState3CoverageModulationTable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageModulationTable");
         if (b->extendedDynamicState3CoverageReductionMode && !a->extendedDynamicState3CoverageReductionMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3CoverageReductionMode");
         if (b->extendedDynamicState3RepresentativeFragmentTestEnable && !a->extendedDynamicState3RepresentativeFragmentTestEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3RepresentativeFragmentTestEnable");
         if (b->extendedDynamicState3ShadingRateImageEnable && !a->extendedDynamicState3ShadingRateImageEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedDynamicState3FeaturesEXT", "extendedDynamicState3ShadingRateImageEnable");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_extended_flags))
            break;
         const VkPhysicalDeviceExtendedFlagsFeaturesKHR *a = &supported_VkPhysicalDeviceExtendedFlagsFeaturesKHR;
         const VkPhysicalDeviceExtendedFlagsFeaturesKHR *b = (const void *) features;
         if (b->extendedFlags && !a->extendedFlags)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedFlagsFeaturesKHR", "extendedFlags");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_partitioned_acceleration_structure))
            break;
         const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV *a = &supported_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV;
         const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV *b = (const void *) features;
         if (b->partitionedAccelerationStructure && !a->partitionedAccelerationStructure)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV", "partitionedAccelerationStructure");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_device_diagnostics_config))
            break;
         const VkPhysicalDeviceDiagnosticsConfigFeaturesNV *a = &supported_VkPhysicalDeviceDiagnosticsConfigFeaturesNV;
         const VkPhysicalDeviceDiagnosticsConfigFeaturesNV *b = (const void *) features;
         if (b->diagnosticsConfig && !a->diagnosticsConfig)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDiagnosticsConfigFeaturesNV", "diagnosticsConfig");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_zero_initialize_workgroup_memory))
            break;
         const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures *a = &supported_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures;
         const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures *b = (const void *) features;
         if (b->shaderZeroInitializeWorkgroupMemory && !a->shaderZeroInitializeWorkgroupMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures", "shaderZeroInitializeWorkgroupMemory");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_subgroup_uniform_control_flow))
            break;
         const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *a = &supported_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR;
         const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *b = (const void *) features;
         if (b->shaderSubgroupUniformControlFlow && !a->shaderSubgroupUniformControlFlow)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR", "shaderSubgroupUniformControlFlow");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.EXT_robustness2 || physical_device->supported_extensions.KHR_robustness2))
            break;
         const VkPhysicalDeviceRobustness2FeaturesKHR *a = &supported_VkPhysicalDeviceRobustness2FeaturesKHR;
         const VkPhysicalDeviceRobustness2FeaturesKHR *b = (const void *) features;
         if (b->robustBufferAccess2 && !a->robustBufferAccess2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRobustness2FeaturesKHR", "robustBufferAccess2");
         if (b->robustImageAccess2 && !a->robustImageAccess2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRobustness2FeaturesKHR", "robustImageAccess2");
         if (b->nullDescriptor && !a->nullDescriptor)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRobustness2FeaturesKHR", "nullDescriptor");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.EXT_image_robustness))
            break;
         const VkPhysicalDeviceImageRobustnessFeatures *a = &supported_VkPhysicalDeviceImageRobustnessFeatures;
         const VkPhysicalDeviceImageRobustnessFeatures *b = (const void *) features;
         if (b->robustImageAccess && !a->robustImageAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageRobustnessFeatures", "robustImageAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_workgroup_memory_explicit_layout))
            break;
         const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *a = &supported_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR;
         const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *b = (const void *) features;
         if (b->workgroupMemoryExplicitLayout && !a->workgroupMemoryExplicitLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR", "workgroupMemoryExplicitLayout");
         if (b->workgroupMemoryExplicitLayoutScalarBlockLayout && !a->workgroupMemoryExplicitLayoutScalarBlockLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR", "workgroupMemoryExplicitLayoutScalarBlockLayout");
         if (b->workgroupMemoryExplicitLayout8BitAccess && !a->workgroupMemoryExplicitLayout8BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR", "workgroupMemoryExplicitLayout8BitAccess");
         if (b->workgroupMemoryExplicitLayout16BitAccess && !a->workgroupMemoryExplicitLayout16BitAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR", "workgroupMemoryExplicitLayout16BitAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_4444_formats))
            break;
         const VkPhysicalDevice4444FormatsFeaturesEXT *a = &supported_VkPhysicalDevice4444FormatsFeaturesEXT;
         const VkPhysicalDevice4444FormatsFeaturesEXT *b = (const void *) features;
         if (b->formatA4R4G4B4 && !a->formatA4R4G4B4)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice4444FormatsFeaturesEXT", "formatA4R4G4B4");
         if (b->formatA4B4G4R4 && !a->formatA4B4G4R4)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevice4444FormatsFeaturesEXT", "formatA4B4G4R4");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI: {
         if (!(physical_device->supported_extensions.HUAWEI_subpass_shading))
            break;
         const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *a = &supported_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI;
         const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *b = (const void *) features;
         if (b->subpassShading && !a->subpassShading)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSubpassShadingFeaturesHUAWEI", "subpassShading");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI: {
         if (!(physical_device->supported_extensions.HUAWEI_cluster_culling_shader))
            break;
         const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI *a = &supported_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI;
         const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI *b = (const void *) features;
         if (b->clustercullingShader && !a->clustercullingShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI", "clustercullingShader");
         if (b->multiviewClusterCullingShader && !a->multiviewClusterCullingShader)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI", "multiviewClusterCullingShader");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_image_atomic_int64))
            break;
         const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *a = &supported_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT;
         const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *b = (const void *) features;
         if (b->shaderImageInt64Atomics && !a->shaderImageInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT", "shaderImageInt64Atomics");
         if (b->sparseImageInt64Atomics && !a->sparseImageInt64Atomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT", "sparseImageInt64Atomics");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_fragment_shading_rate))
            break;
         const VkPhysicalDeviceFragmentShadingRateFeaturesKHR *a = &supported_VkPhysicalDeviceFragmentShadingRateFeaturesKHR;
         const VkPhysicalDeviceFragmentShadingRateFeaturesKHR *b = (const void *) features;
         if (b->pipelineFragmentShadingRate && !a->pipelineFragmentShadingRate)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateFeaturesKHR", "pipelineFragmentShadingRate");
         if (b->primitiveFragmentShadingRate && !a->primitiveFragmentShadingRate)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateFeaturesKHR", "primitiveFragmentShadingRate");
         if (b->attachmentFragmentShadingRate && !a->attachmentFragmentShadingRate)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateFeaturesKHR", "attachmentFragmentShadingRate");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_shader_terminate_invocation))
            break;
         const VkPhysicalDeviceShaderTerminateInvocationFeatures *a = &supported_VkPhysicalDeviceShaderTerminateInvocationFeatures;
         const VkPhysicalDeviceShaderTerminateInvocationFeatures *b = (const void *) features;
         if (b->shaderTerminateInvocation && !a->shaderTerminateInvocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderTerminateInvocationFeatures", "shaderTerminateInvocation");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_fragment_shading_rate_enums))
            break;
         const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *a = &supported_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV;
         const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *b = (const void *) features;
         if (b->fragmentShadingRateEnums && !a->fragmentShadingRateEnums)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV", "fragmentShadingRateEnums");
         if (b->supersampleFragmentShadingRates && !a->supersampleFragmentShadingRates)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV", "supersampleFragmentShadingRates");
         if (b->noInvocationFragmentShadingRates && !a->noInvocationFragmentShadingRates)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV", "noInvocationFragmentShadingRates");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_image_2d_view_of_3d))
            break;
         const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *a = &supported_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT;
         const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *b = (const void *) features;
         if (b->image2DViewOf3D && !a->image2DViewOf3D)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT", "image2DViewOf3D");
         if (b->sampler2DViewOf3D && !a->sampler2DViewOf3D)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT", "sampler2DViewOf3D");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_image_sliced_view_of_3d))
            break;
         const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT *a = &supported_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT;
         const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT *b = (const void *) features;
         if (b->imageSlicedViewOf3D && !a->imageSlicedViewOf3D)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT", "imageSlicedViewOf3D");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_attachment_feedback_loop_dynamic_state))
            break;
         const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT *a = &supported_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT;
         const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT *b = (const void *) features;
         if (b->attachmentFeedbackLoopDynamicState && !a->attachmentFeedbackLoopDynamicState)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT", "attachmentFeedbackLoopDynamicState");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_legacy_vertex_attributes))
            break;
         const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT *a = &supported_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT;
         const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT *b = (const void *) features;
         if (b->legacyVertexAttributes && !a->legacyVertexAttributes)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT", "legacyVertexAttributes");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.VALVE_mutable_descriptor_type || physical_device->supported_extensions.EXT_mutable_descriptor_type))
            break;
         const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT *a = &supported_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT;
         const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT *b = (const void *) features;
         if (b->mutableDescriptorType && !a->mutableDescriptorType)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_depth_clip_control))
            break;
         const VkPhysicalDeviceDepthClipControlFeaturesEXT *a = &supported_VkPhysicalDeviceDepthClipControlFeaturesEXT;
         const VkPhysicalDeviceDepthClipControlFeaturesEXT *b = (const void *) features;
         if (b->depthClipControl && !a->depthClipControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthClipControlFeaturesEXT", "depthClipControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_zero_initialize_device_memory))
            break;
         const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT *a = &supported_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT;
         const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT *b = (const void *) features;
         if (b->zeroInitializeDeviceMemory && !a->zeroInitializeDeviceMemory)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT", "zeroInitializeDeviceMemory");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_custom_resolve))
            break;
         const VkPhysicalDeviceCustomResolveFeaturesEXT *a = &supported_VkPhysicalDeviceCustomResolveFeaturesEXT;
         const VkPhysicalDeviceCustomResolveFeaturesEXT *b = (const void *) features;
         if (b->customResolve && !a->customResolve)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCustomResolveFeaturesEXT", "customResolve");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_device_generated_commands))
            break;
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT *a = &supported_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT;
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT *b = (const void *) features;
         if (b->deviceGeneratedCommands && !a->deviceGeneratedCommands)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT", "deviceGeneratedCommands");
         if (b->dynamicGeneratedPipelineLayout && !a->dynamicGeneratedPipelineLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT", "dynamicGeneratedPipelineLayout");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_depth_clamp_control))
            break;
         const VkPhysicalDeviceDepthClampControlFeaturesEXT *a = &supported_VkPhysicalDeviceDepthClampControlFeaturesEXT;
         const VkPhysicalDeviceDepthClampControlFeaturesEXT *b = (const void *) features;
         if (b->depthClampControl && !a->depthClampControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthClampControlFeaturesEXT", "depthClampControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_vertex_input_dynamic_state))
            break;
         const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *a = &supported_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT;
         const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *b = (const void *) features;
         if (b->vertexInputDynamicState && !a->vertexInputDynamicState)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT", "vertexInputDynamicState");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_external_memory_rdma))
            break;
         const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *a = &supported_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV;
         const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *b = (const void *) features;
         if (b->externalMemoryRDMA && !a->externalMemoryRDMA)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExternalMemoryRDMAFeaturesNV", "externalMemoryRDMA");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_relaxed_extended_instruction))
            break;
         const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR *a = &supported_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR;
         const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR *b = (const void *) features;
         if (b->shaderRelaxedExtendedInstruction && !a->shaderRelaxedExtendedInstruction)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR", "shaderRelaxedExtendedInstruction");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_color_write_enable))
            break;
         const VkPhysicalDeviceColorWriteEnableFeaturesEXT *a = &supported_VkPhysicalDeviceColorWriteEnableFeaturesEXT;
         const VkPhysicalDeviceColorWriteEnableFeaturesEXT *b = (const void *) features;
         if (b->colorWriteEnable && !a->colorWriteEnable)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceColorWriteEnableFeaturesEXT", "colorWriteEnable");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_synchronization2))
            break;
         const VkPhysicalDeviceSynchronization2Features *a = &supported_VkPhysicalDeviceSynchronization2Features;
         const VkPhysicalDeviceSynchronization2Features *b = (const void *) features;
         if (b->synchronization2 && !a->synchronization2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSynchronization2Features", "synchronization2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_unified_image_layouts))
            break;
         const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR *a = &supported_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR;
         const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR *b = (const void *) features;
         if (b->unifiedImageLayouts && !a->unifiedImageLayouts)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR", "unifiedImageLayouts");
         if (b->unifiedImageLayoutsVideo && !a->unifiedImageLayoutsVideo)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR", "unifiedImageLayoutsVideo");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_host_image_copy))
            break;
         const VkPhysicalDeviceHostImageCopyFeatures *a = &supported_VkPhysicalDeviceHostImageCopyFeatures;
         const VkPhysicalDeviceHostImageCopyFeatures *b = (const void *) features;
         if (b->hostImageCopy && !a->hostImageCopy)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceHostImageCopyFeatures", "hostImageCopy");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_primitives_generated_query))
            break;
         const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *a = &supported_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT;
         const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *b = (const void *) features;
         if (b->primitivesGeneratedQuery && !a->primitivesGeneratedQuery)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT", "primitivesGeneratedQuery");
         if (b->primitivesGeneratedQueryWithRasterizerDiscard && !a->primitivesGeneratedQueryWithRasterizerDiscard)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT", "primitivesGeneratedQueryWithRasterizerDiscard");
         if (b->primitivesGeneratedQueryWithNonZeroStreams && !a->primitivesGeneratedQueryWithNonZeroStreams)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT", "primitivesGeneratedQueryWithNonZeroStreams");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_legacy_dithering))
            break;
         const VkPhysicalDeviceLegacyDitheringFeaturesEXT *a = &supported_VkPhysicalDeviceLegacyDitheringFeaturesEXT;
         const VkPhysicalDeviceLegacyDitheringFeaturesEXT *b = (const void *) features;
         if (b->legacyDithering && !a->legacyDithering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLegacyDitheringFeaturesEXT", "legacyDithering");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_multisampled_render_to_single_sampled))
            break;
         const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *a = &supported_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT;
         const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *b = (const void *) features;
         if (b->multisampledRenderToSingleSampled && !a->multisampledRenderToSingleSampled)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT", "multisampledRenderToSingleSampled");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_multisampled_render_to_swapchain))
            break;
         const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT *a = &supported_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT;
         const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT *b = (const void *) features;
         if (b->multisampledRenderToSwapchain && !a->multisampledRenderToSwapchain)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT", "multisampledRenderToSwapchain");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_pipeline_protected_access))
            break;
         const VkPhysicalDevicePipelineProtectedAccessFeatures *a = &supported_VkPhysicalDevicePipelineProtectedAccessFeatures;
         const VkPhysicalDevicePipelineProtectedAccessFeatures *b = (const void *) features;
         if (b->pipelineProtectedAccess && !a->pipelineProtectedAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineProtectedAccessFeatures", "pipelineProtectedAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_maintenance1))
            break;
         const VkPhysicalDeviceVideoMaintenance1FeaturesKHR *a = &supported_VkPhysicalDeviceVideoMaintenance1FeaturesKHR;
         const VkPhysicalDeviceVideoMaintenance1FeaturesKHR *b = (const void *) features;
         if (b->videoMaintenance1 && !a->videoMaintenance1)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoMaintenance1FeaturesKHR", "videoMaintenance1");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_maintenance2))
            break;
         const VkPhysicalDeviceVideoMaintenance2FeaturesKHR *a = &supported_VkPhysicalDeviceVideoMaintenance2FeaturesKHR;
         const VkPhysicalDeviceVideoMaintenance2FeaturesKHR *b = (const void *) features;
         if (b->videoMaintenance2 && !a->videoMaintenance2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoMaintenance2FeaturesKHR", "videoMaintenance2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_decode_vp9))
            break;
         const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR *a = &supported_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR;
         const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR *b = (const void *) features;
         if (b->videoDecodeVP9 && !a->videoDecodeVP9)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoDecodeVP9FeaturesKHR", "videoDecodeVP9");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_encode_quantization_map))
            break;
         const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR *a = &supported_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR;
         const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR *b = (const void *) features;
         if (b->videoEncodeQuantizationMap && !a->videoEncodeQuantizationMap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR", "videoEncodeQuantizationMap");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_encode_feedback2))
            break;
         const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR *a = &supported_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR;
         const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR *b = (const void *) features;
         if (b->videoEncodeFeedback2 && !a->videoEncodeFeedback2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR", "videoEncodeFeedback2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_encode_av1))
            break;
         const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR *a = &supported_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR;
         const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR *b = (const void *) features;
         if (b->videoEncodeAV1 && !a->videoEncodeAV1)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoEncodeAV1FeaturesKHR", "videoEncodeAV1");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_inherited_viewport_scissor))
            break;
         const VkPhysicalDeviceInheritedViewportScissorFeaturesNV *a = &supported_VkPhysicalDeviceInheritedViewportScissorFeaturesNV;
         const VkPhysicalDeviceInheritedViewportScissorFeaturesNV *b = (const void *) features;
         if (b->inheritedViewportScissor2D && !a->inheritedViewportScissor2D)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceInheritedViewportScissorFeaturesNV", "inheritedViewportScissor2D");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_ycbcr_2plane_444_formats))
            break;
         const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *a = &supported_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT;
         const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *b = (const void *) features;
         if (b->ycbcr2plane444Formats && !a->ycbcr2plane444Formats)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT", "ycbcr2plane444Formats");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_provoking_vertex))
            break;
         const VkPhysicalDeviceProvokingVertexFeaturesEXT *a = &supported_VkPhysicalDeviceProvokingVertexFeaturesEXT;
         const VkPhysicalDeviceProvokingVertexFeaturesEXT *b = (const void *) features;
         if (b->provokingVertexLast && !a->provokingVertexLast)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceProvokingVertexFeaturesEXT", "provokingVertexLast");
         if (b->transformFeedbackPreservesProvokingVertex && !a->transformFeedbackPreservesProvokingVertex)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceProvokingVertexFeaturesEXT", "transformFeedbackPreservesProvokingVertex");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_video_encode_intra_refresh))
            break;
         const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR *a = &supported_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR;
         const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR *b = (const void *) features;
         if (b->videoEncodeIntraRefresh && !a->videoEncodeIntraRefresh)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR", "videoEncodeIntraRefresh");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_descriptor_buffer))
            break;
         const VkPhysicalDeviceDescriptorBufferFeaturesEXT *a = &supported_VkPhysicalDeviceDescriptorBufferFeaturesEXT;
         const VkPhysicalDeviceDescriptorBufferFeaturesEXT *b = (const void *) features;
         if (b->descriptorBuffer && !a->descriptorBuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorBufferFeaturesEXT", "descriptorBuffer");
         if (b->descriptorBufferCaptureReplay && !a->descriptorBufferCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorBufferFeaturesEXT", "descriptorBufferCaptureReplay");
         if (b->descriptorBufferImageLayoutIgnored && !a->descriptorBufferImageLayoutIgnored)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorBufferFeaturesEXT", "descriptorBufferImageLayoutIgnored");
         if (b->descriptorBufferPushDescriptors && !a->descriptorBufferPushDescriptors)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorBufferFeaturesEXT", "descriptorBufferPushDescriptors");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_shader_integer_dot_product))
            break;
         const VkPhysicalDeviceShaderIntegerDotProductFeatures *a = &supported_VkPhysicalDeviceShaderIntegerDotProductFeatures;
         const VkPhysicalDeviceShaderIntegerDotProductFeatures *b = (const void *) features;
         if (b->shaderIntegerDotProduct && !a->shaderIntegerDotProduct)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderIntegerDotProductFeatures", "shaderIntegerDotProduct");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.NV_fragment_shader_barycentric || physical_device->supported_extensions.KHR_fragment_shader_barycentric))
            break;
         const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR *a = &supported_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR;
         const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR *b = (const void *) features;
         if (b->fragmentShaderBarycentric && !a->fragmentShaderBarycentric)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR", "fragmentShaderBarycentric");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_fma))
            break;
         const VkPhysicalDeviceShaderFmaFeaturesKHR *a = &supported_VkPhysicalDeviceShaderFmaFeaturesKHR;
         const VkPhysicalDeviceShaderFmaFeaturesKHR *b = (const void *) features;
         if (b->shaderFmaFloat16 && !a->shaderFmaFloat16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFmaFeaturesKHR", "shaderFmaFloat16");
         if (b->shaderFmaFloat32 && !a->shaderFmaFloat32)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFmaFeaturesKHR", "shaderFmaFloat32");
         if (b->shaderFmaFloat64 && !a->shaderFmaFloat64)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFmaFeaturesKHR", "shaderFmaFloat64");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_ray_tracing_motion_blur))
            break;
         const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *a = &supported_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV;
         const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *b = (const void *) features;
         if (b->rayTracingMotionBlur && !a->rayTracingMotionBlur)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingMotionBlurFeaturesNV", "rayTracingMotionBlur");
         if (b->rayTracingMotionBlurPipelineTraceRaysIndirect && !a->rayTracingMotionBlurPipelineTraceRaysIndirect)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingMotionBlurFeaturesNV", "rayTracingMotionBlurPipelineTraceRaysIndirect");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_ray_tracing_validation))
            break;
         const VkPhysicalDeviceRayTracingValidationFeaturesNV *a = &supported_VkPhysicalDeviceRayTracingValidationFeaturesNV;
         const VkPhysicalDeviceRayTracingValidationFeaturesNV *b = (const void *) features;
         if (b->rayTracingValidation && !a->rayTracingValidation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingValidationFeaturesNV", "rayTracingValidation");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_ray_tracing_linear_swept_spheres))
            break;
         const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV *a = &supported_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV;
         const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV *b = (const void *) features;
         if (b->spheres && !a->spheres)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV", "spheres");
         if (b->linearSweptSpheres && !a->linearSweptSpheres)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV", "linearSweptSpheres");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_rgba10x6_formats))
            break;
         const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *a = &supported_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT;
         const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *b = (const void *) features;
         if (b->formatRgba10x6WithoutYCbCrSampler && !a->formatRgba10x6WithoutYCbCrSampler)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT", "formatRgba10x6WithoutYCbCrSampler");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 3, 0) || physical_device->supported_extensions.KHR_dynamic_rendering))
            break;
         const VkPhysicalDeviceDynamicRenderingFeatures *a = &supported_VkPhysicalDeviceDynamicRenderingFeatures;
         const VkPhysicalDeviceDynamicRenderingFeatures *b = (const void *) features;
         if (b->dynamicRendering && !a->dynamicRendering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDynamicRenderingFeatures", "dynamicRendering");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_image_view_min_lod))
            break;
         const VkPhysicalDeviceImageViewMinLodFeaturesEXT *a = &supported_VkPhysicalDeviceImageViewMinLodFeaturesEXT;
         const VkPhysicalDeviceImageViewMinLodFeaturesEXT *b = (const void *) features;
         if (b->minLod && !a->minLod)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageViewMinLodFeaturesEXT", "minLod");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.ARM_rasterization_order_attachment_access || physical_device->supported_extensions.EXT_rasterization_order_attachment_access))
            break;
         const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT *a = &supported_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT;
         const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT *b = (const void *) features;
         if (b->rasterizationOrderColorAttachmentAccess && !a->rasterizationOrderColorAttachmentAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT", "rasterizationOrderColorAttachmentAccess");
         if (b->rasterizationOrderDepthAttachmentAccess && !a->rasterizationOrderDepthAttachmentAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT", "rasterizationOrderDepthAttachmentAccess");
         if (b->rasterizationOrderStencilAttachmentAccess && !a->rasterizationOrderStencilAttachmentAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT", "rasterizationOrderStencilAttachmentAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_linear_color_attachment))
            break;
         const VkPhysicalDeviceLinearColorAttachmentFeaturesNV *a = &supported_VkPhysicalDeviceLinearColorAttachmentFeaturesNV;
         const VkPhysicalDeviceLinearColorAttachmentFeaturesNV *b = (const void *) features;
         if (b->linearColorAttachment && !a->linearColorAttachment)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceLinearColorAttachmentFeaturesNV", "linearColorAttachment");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_graphics_pipeline_library))
            break;
         const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *a = &supported_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT;
         const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *b = (const void *) features;
         if (b->graphicsPipelineLibrary && !a->graphicsPipelineLibrary)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT", "graphicsPipelineLibrary");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_pipeline_binary))
            break;
         const VkPhysicalDevicePipelineBinaryFeaturesKHR *a = &supported_VkPhysicalDevicePipelineBinaryFeaturesKHR;
         const VkPhysicalDevicePipelineBinaryFeaturesKHR *b = (const void *) features;
         if (b->pipelineBinaries && !a->pipelineBinaries)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineBinaryFeaturesKHR", "pipelineBinaries");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_data_graph_neural_accelerator_statistics))
            break;
         const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM *a = &supported_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM;
         const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM *b = (const void *) features;
         if (b->dataGraphNeuralAcceleratorStatistics && !a->dataGraphNeuralAcceleratorStatistics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM", "dataGraphNeuralAcceleratorStatistics");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE: {
         if (!(physical_device->supported_extensions.VALVE_descriptor_set_host_mapping))
            break;
         const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *a = &supported_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE;
         const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *b = (const void *) features;
         if (b->descriptorSetHostMapping && !a->descriptorSetHostMapping)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE", "descriptorSetHostMapping");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_nested_command_buffer))
            break;
         const VkPhysicalDeviceNestedCommandBufferFeaturesEXT *a = &supported_VkPhysicalDeviceNestedCommandBufferFeaturesEXT;
         const VkPhysicalDeviceNestedCommandBufferFeaturesEXT *b = (const void *) features;
         if (b->nestedCommandBuffer && !a->nestedCommandBuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceNestedCommandBufferFeaturesEXT", "nestedCommandBuffer");
         if (b->nestedCommandBufferRendering && !a->nestedCommandBufferRendering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceNestedCommandBufferFeaturesEXT", "nestedCommandBufferRendering");
         if (b->nestedCommandBufferSimultaneousUse && !a->nestedCommandBufferSimultaneousUse)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceNestedCommandBufferFeaturesEXT", "nestedCommandBufferSimultaneousUse");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_module_identifier))
            break;
         const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *a = &supported_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT;
         const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *b = (const void *) features;
         if (b->shaderModuleIdentifier && !a->shaderModuleIdentifier)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT", "shaderModuleIdentifier");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_image_compression_control))
            break;
         const VkPhysicalDeviceImageCompressionControlFeaturesEXT *a = &supported_VkPhysicalDeviceImageCompressionControlFeaturesEXT;
         const VkPhysicalDeviceImageCompressionControlFeaturesEXT *b = (const void *) features;
         if (b->imageCompressionControl && !a->imageCompressionControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageCompressionControlFeaturesEXT", "imageCompressionControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_image_compression_control_swapchain))
            break;
         const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *a = &supported_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT;
         const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *b = (const void *) features;
         if (b->imageCompressionControlSwapchain && !a->imageCompressionControlSwapchain)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT", "imageCompressionControlSwapchain");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_subpass_merge_feedback))
            break;
         const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *a = &supported_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT;
         const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *b = (const void *) features;
         if (b->subpassMergeFeedback && !a->subpassMergeFeedback)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT", "subpassMergeFeedback");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_opacity_micromap))
            break;
         const VkPhysicalDeviceOpacityMicromapFeaturesKHR *a = &supported_VkPhysicalDeviceOpacityMicromapFeaturesKHR;
         const VkPhysicalDeviceOpacityMicromapFeaturesKHR *b = (const void *) features;
         if (b->micromap && !a->micromap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceOpacityMicromapFeaturesKHR", "micromap");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_opacity_micromap))
            break;
         const VkPhysicalDeviceOpacityMicromapFeaturesEXT *a = &supported_VkPhysicalDeviceOpacityMicromapFeaturesEXT;
         const VkPhysicalDeviceOpacityMicromapFeaturesEXT *b = (const void *) features;
         if (b->micromap && !a->micromap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceOpacityMicromapFeaturesEXT", "micromap");
         if (b->micromapCaptureReplay && !a->micromapCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceOpacityMicromapFeaturesEXT", "micromapCaptureReplay");
         if (b->micromapHostCommands && !a->micromapHostCommands)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceOpacityMicromapFeaturesEXT", "micromapHostCommands");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_pipeline_properties))
            break;
         const VkPhysicalDevicePipelinePropertiesFeaturesEXT *a = &supported_VkPhysicalDevicePipelinePropertiesFeaturesEXT;
         const VkPhysicalDevicePipelinePropertiesFeaturesEXT *b = (const void *) features;
         if (b->pipelinePropertiesIdentifier && !a->pipelinePropertiesIdentifier)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelinePropertiesFeaturesEXT", "pipelinePropertiesIdentifier");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD: {
         if (!(physical_device->supported_extensions.AMD_shader_early_and_late_fragment_tests))
            break;
         const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *a = &supported_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD;
         const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *b = (const void *) features;
         if (b->shaderEarlyAndLateFragmentTests && !a->shaderEarlyAndLateFragmentTests)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD", "shaderEarlyAndLateFragmentTests");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_non_seamless_cube_map))
            break;
         const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *a = &supported_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT;
         const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *b = (const void *) features;
         if (b->nonSeamlessCubeMap && !a->nonSeamlessCubeMap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT", "nonSeamlessCubeMap");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.EXT_pipeline_robustness))
            break;
         const VkPhysicalDevicePipelineRobustnessFeatures *a = &supported_VkPhysicalDevicePipelineRobustnessFeatures;
         const VkPhysicalDevicePipelineRobustnessFeatures *b = (const void *) features;
         if (b->pipelineRobustness && !a->pipelineRobustness)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineRobustnessFeatures", "pipelineRobustness");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_shader_multiple_wait_queues))
            break;
         const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM *a = &supported_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM;
         const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM *b = (const void *) features;
         if (b->shaderMultipleWaitQueues && !a->shaderMultipleWaitQueues)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM", "shaderMultipleWaitQueues");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_image_processing))
            break;
         const VkPhysicalDeviceImageProcessingFeaturesQCOM *a = &supported_VkPhysicalDeviceImageProcessingFeaturesQCOM;
         const VkPhysicalDeviceImageProcessingFeaturesQCOM *b = (const void *) features;
         if (b->textureSampleWeighted && !a->textureSampleWeighted)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessingFeaturesQCOM", "textureSampleWeighted");
         if (b->textureBoxFilter && !a->textureBoxFilter)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessingFeaturesQCOM", "textureBoxFilter");
         if (b->textureBlockMatch && !a->textureBlockMatch)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessingFeaturesQCOM", "textureBlockMatch");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_tile_properties))
            break;
         const VkPhysicalDeviceTilePropertiesFeaturesQCOM *a = &supported_VkPhysicalDeviceTilePropertiesFeaturesQCOM;
         const VkPhysicalDeviceTilePropertiesFeaturesQCOM *b = (const void *) features;
         if (b->tileProperties && !a->tileProperties)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTilePropertiesFeaturesQCOM", "tileProperties");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC: {
         if (!(physical_device->supported_extensions.SEC_amigo_profiling))
            break;
         const VkPhysicalDeviceAmigoProfilingFeaturesSEC *a = &supported_VkPhysicalDeviceAmigoProfilingFeaturesSEC;
         const VkPhysicalDeviceAmigoProfilingFeaturesSEC *b = (const void *) features;
         if (b->amigoProfiling && !a->amigoProfiling)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAmigoProfilingFeaturesSEC", "amigoProfiling");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_attachment_feedback_loop_layout))
            break;
         const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *a = &supported_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT;
         const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *b = (const void *) features;
         if (b->attachmentFeedbackLoopLayout && !a->attachmentFeedbackLoopLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT", "attachmentFeedbackLoopLayout");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_device_address_binding_report))
            break;
         const VkPhysicalDeviceAddressBindingReportFeaturesEXT *a = &supported_VkPhysicalDeviceAddressBindingReportFeaturesEXT;
         const VkPhysicalDeviceAddressBindingReportFeaturesEXT *b = (const void *) features;
         if (b->reportAddressBinding && !a->reportAddressBinding)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAddressBindingReportFeaturesEXT", "reportAddressBinding");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_optical_flow))
            break;
         const VkPhysicalDeviceOpticalFlowFeaturesNV *a = &supported_VkPhysicalDeviceOpticalFlowFeaturesNV;
         const VkPhysicalDeviceOpticalFlowFeaturesNV *b = (const void *) features;
         if (b->opticalFlow && !a->opticalFlow)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceOpticalFlowFeaturesNV", "opticalFlow");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_device_fault))
            break;
         const VkPhysicalDeviceFaultFeaturesEXT *a = &supported_VkPhysicalDeviceFaultFeaturesEXT;
         const VkPhysicalDeviceFaultFeaturesEXT *b = (const void *) features;
         if (b->deviceFault && !a->deviceFault)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesEXT", "deviceFault");
         if (b->deviceFaultVendorBinary && !a->deviceFaultVendorBinary)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesEXT", "deviceFaultVendorBinary");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_device_fault))
            break;
         const VkPhysicalDeviceFaultFeaturesKHR *a = &supported_VkPhysicalDeviceFaultFeaturesKHR;
         const VkPhysicalDeviceFaultFeaturesKHR *b = (const void *) features;
         if (b->deviceFault && !a->deviceFault)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesKHR", "deviceFault");
         if (b->deviceFaultVendorBinary && !a->deviceFaultVendorBinary)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesKHR", "deviceFaultVendorBinary");
         if (b->deviceFaultReportMasked && !a->deviceFaultReportMasked)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesKHR", "deviceFaultReportMasked");
         if (b->deviceFaultDeviceLostOnMasked && !a->deviceFaultDeviceLostOnMasked)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFaultFeaturesKHR", "deviceFaultDeviceLostOnMasked");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_pipeline_library_group_handles))
            break;
         const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT *a = &supported_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT;
         const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT *b = (const void *) features;
         if (b->pipelineLibraryGroupHandles && !a->pipelineLibraryGroupHandles)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT", "pipelineLibraryGroupHandles");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_shader_core_builtins))
            break;
         const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM *a = &supported_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM;
         const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM *b = (const void *) features;
         if (b->shaderCoreBuiltins && !a->shaderCoreBuiltins)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM", "shaderCoreBuiltins");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_frame_boundary))
            break;
         const VkPhysicalDeviceFrameBoundaryFeaturesEXT *a = &supported_VkPhysicalDeviceFrameBoundaryFeaturesEXT;
         const VkPhysicalDeviceFrameBoundaryFeaturesEXT *b = (const void *) features;
         if (b->frameBoundary && !a->frameBoundary)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFrameBoundaryFeaturesEXT", "frameBoundary");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_dynamic_rendering_unused_attachments))
            break;
         const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT *a = &supported_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT;
         const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT *b = (const void *) features;
         if (b->dynamicRenderingUnusedAttachments && !a->dynamicRenderingUnusedAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT", "dynamicRenderingUnusedAttachments");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_internally_synchronized_queues))
            break;
         const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR *a = &supported_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR;
         const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR *b = (const void *) features;
         if (b->internallySynchronizedQueues && !a->internallySynchronizedQueues)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR", "internallySynchronizedQueues");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.EXT_swapchain_maintenance1 || physical_device->supported_extensions.KHR_swapchain_maintenance1))
            break;
         const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR *a = &supported_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR;
         const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR *b = (const void *) features;
         if (b->swapchainMaintenance1 && !a->swapchainMaintenance1)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR", "swapchainMaintenance1");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_depth_bias_control))
            break;
         const VkPhysicalDeviceDepthBiasControlFeaturesEXT *a = &supported_VkPhysicalDeviceDepthBiasControlFeaturesEXT;
         const VkPhysicalDeviceDepthBiasControlFeaturesEXT *b = (const void *) features;
         if (b->depthBiasControl && !a->depthBiasControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthBiasControlFeaturesEXT", "depthBiasControl");
         if (b->leastRepresentableValueForceUnormRepresentation && !a->leastRepresentableValueForceUnormRepresentation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthBiasControlFeaturesEXT", "leastRepresentableValueForceUnormRepresentation");
         if (b->floatRepresentation && !a->floatRepresentation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthBiasControlFeaturesEXT", "floatRepresentation");
         if (b->depthBiasExact && !a->depthBiasExact)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthBiasControlFeaturesEXT", "depthBiasExact");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_ray_tracing_invocation_reorder))
            break;
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT *a = &supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT;
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT *b = (const void *) features;
         if (b->rayTracingInvocationReorder && !a->rayTracingInvocationReorder)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT", "rayTracingInvocationReorder");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_ray_tracing_invocation_reorder))
            break;
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV *a = &supported_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV;
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV *b = (const void *) features;
         if (b->rayTracingInvocationReorder && !a->rayTracingInvocationReorder)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV", "rayTracingInvocationReorder");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_extended_sparse_address_space))
            break;
         const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV *a = &supported_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV;
         const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV *b = (const void *) features;
         if (b->extendedSparseAddressSpace && !a->extendedSparseAddressSpace)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV", "extendedSparseAddressSpace");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_multiview_per_view_viewports))
            break;
         const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM *a = &supported_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM;
         const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM *b = (const void *) features;
         if (b->multiviewPerViewViewports && !a->multiviewPerViewViewports)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM", "multiviewPerViewViewports");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_ray_tracing_position_fetch))
            break;
         const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR *a = &supported_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR;
         const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR *b = (const void *) features;
         if (b->rayTracingPositionFetch && !a->rayTracingPositionFetch)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR", "rayTracingPositionFetch");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_multiview_per_view_render_areas))
            break;
         const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM *a = &supported_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM;
         const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM *b = (const void *) features;
         if (b->multiviewPerViewRenderAreas && !a->multiviewPerViewRenderAreas)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM", "multiviewPerViewRenderAreas");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_object))
            break;
         const VkPhysicalDeviceShaderObjectFeaturesEXT *a = &supported_VkPhysicalDeviceShaderObjectFeaturesEXT;
         const VkPhysicalDeviceShaderObjectFeaturesEXT *b = (const void *) features;
         if (b->shaderObject && !a->shaderObject)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderObjectFeaturesEXT", "shaderObject");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_tile_image))
            break;
         const VkPhysicalDeviceShaderTileImageFeaturesEXT *a = &supported_VkPhysicalDeviceShaderTileImageFeaturesEXT;
         const VkPhysicalDeviceShaderTileImageFeaturesEXT *b = (const void *) features;
         if (b->shaderTileImageColorReadAccess && !a->shaderTileImageColorReadAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderTileImageFeaturesEXT", "shaderTileImageColorReadAccess");
         if (b->shaderTileImageDepthReadAccess && !a->shaderTileImageDepthReadAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderTileImageFeaturesEXT", "shaderTileImageDepthReadAccess");
         if (b->shaderTileImageStencilReadAccess && !a->shaderTileImageStencilReadAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderTileImageFeaturesEXT", "shaderTileImageStencilReadAccess");
         break;
      }
#ifdef VK_USE_PLATFORM_SCREEN_QNX
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX: {
         if (!(physical_device->supported_extensions.QNX_external_memory_screen_buffer))
            break;
         const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX *a = &supported_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX;
         const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX *b = (const void *) features;
         if (b->screenBufferImport && !a->screenBufferImport)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX", "screenBufferImport");
         break;
      }
#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_cooperative_matrix))
            break;
         const VkPhysicalDeviceCooperativeMatrixFeaturesKHR *a = &supported_VkPhysicalDeviceCooperativeMatrixFeaturesKHR;
         const VkPhysicalDeviceCooperativeMatrixFeaturesKHR *b = (const void *) features;
         if (b->cooperativeMatrix && !a->cooperativeMatrix)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixFeaturesKHR", "cooperativeMatrix");
         if (b->cooperativeMatrixRobustBufferAccess && !a->cooperativeMatrixRobustBufferAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixFeaturesKHR", "cooperativeMatrixRobustBufferAccess");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_cooperative_matrix_conversion))
            break;
         const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM *a = &supported_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM;
         const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM *b = (const void *) features;
         if (b->cooperativeMatrixConversion && !a->cooperativeMatrixConversion)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM", "cooperativeMatrixConversion");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD: {
         if (!(physical_device->supported_extensions.AMD_anti_lag))
            break;
         const VkPhysicalDeviceAntiLagFeaturesAMD *a = &supported_VkPhysicalDeviceAntiLagFeaturesAMD;
         const VkPhysicalDeviceAntiLagFeaturesAMD *b = (const void *) features;
         if (b->antiLag && !a->antiLag)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceAntiLagFeaturesAMD", "antiLag");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_tile_memory_heap))
            break;
         const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM *a = &supported_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM;
         const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM *b = (const void *) features;
         if (b->tileMemoryHeap && !a->tileMemoryHeap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileMemoryHeapFeaturesQCOM", "tileMemoryHeap");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_filter_cubic_clamp))
            break;
         const VkPhysicalDeviceCubicClampFeaturesQCOM *a = &supported_VkPhysicalDeviceCubicClampFeaturesQCOM;
         const VkPhysicalDeviceCubicClampFeaturesQCOM *b = (const void *) features;
         if (b->cubicRangeClamp && !a->cubicRangeClamp)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCubicClampFeaturesQCOM", "cubicRangeClamp");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_ycbcr_degamma))
            break;
         const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM *a = &supported_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM;
         const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM *b = (const void *) features;
         if (b->ycbcrDegamma && !a->ycbcrDegamma)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceYcbcrDegammaFeaturesQCOM", "ycbcrDegamma");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_filter_cubic_weights))
            break;
         const VkPhysicalDeviceCubicWeightsFeaturesQCOM *a = &supported_VkPhysicalDeviceCubicWeightsFeaturesQCOM;
         const VkPhysicalDeviceCubicWeightsFeaturesQCOM *b = (const void *) features;
         if (b->selectableCubicWeights && !a->selectableCubicWeights)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCubicWeightsFeaturesQCOM", "selectableCubicWeights");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_image_processing2))
            break;
         const VkPhysicalDeviceImageProcessing2FeaturesQCOM *a = &supported_VkPhysicalDeviceImageProcessing2FeaturesQCOM;
         const VkPhysicalDeviceImageProcessing2FeaturesQCOM *b = (const void *) features;
         if (b->textureBlockMatch2 && !a->textureBlockMatch2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessing2FeaturesQCOM", "textureBlockMatch2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_image_processing3))
            break;
         const VkPhysicalDeviceImageProcessing3FeaturesQCOM *a = &supported_VkPhysicalDeviceImageProcessing3FeaturesQCOM;
         const VkPhysicalDeviceImageProcessing3FeaturesQCOM *b = (const void *) features;
         if (b->imageGatherLinear && !a->imageGatherLinear)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessing3FeaturesQCOM", "imageGatherLinear");
         if (b->imageGatherExtendedModes && !a->imageGatherExtendedModes)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessing3FeaturesQCOM", "imageGatherExtendedModes");
         if (b->blockMatchExtendedClampToEdge && !a->blockMatchExtendedClampToEdge)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageProcessing3FeaturesQCOM", "blockMatchExtendedClampToEdge");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_descriptor_pool_overallocation))
            break;
         const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV *a = &supported_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV;
         const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV *b = (const void *) features;
         if (b->descriptorPoolOverallocation && !a->descriptorPoolOverallocation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV", "descriptorPoolOverallocation");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_per_stage_descriptor_set))
            break;
         const VkPhysicalDevicePerStageDescriptorSetFeaturesNV *a = &supported_VkPhysicalDevicePerStageDescriptorSetFeaturesNV;
         const VkPhysicalDevicePerStageDescriptorSetFeaturesNV *b = (const void *) features;
         if (b->perStageDescriptorSet && !a->perStageDescriptorSet)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePerStageDescriptorSetFeaturesNV", "perStageDescriptorSet");
         if (b->dynamicPipelineLayout && !a->dynamicPipelineLayout)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePerStageDescriptorSetFeaturesNV", "dynamicPipelineLayout");
         break;
      }
#ifdef VK_USE_PLATFORM_ANDROID_KHR
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID: {
         if (!(physical_device->supported_extensions.ANDROID_external_format_resolve))
            break;
         const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID *a = &supported_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID;
         const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID *b = (const void *) features;
         if (b->externalFormatResolve && !a->externalFormatResolve)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceExternalFormatResolveFeaturesANDROID", "externalFormatResolve");
         break;
      }
#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_scheduling_controls))
            break;
         const VkPhysicalDeviceSchedulingControlsFeaturesARM *a = &supported_VkPhysicalDeviceSchedulingControlsFeaturesARM;
         const VkPhysicalDeviceSchedulingControlsFeaturesARM *b = (const void *) features;
         if (b->schedulingControls && !a->schedulingControls)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceSchedulingControlsFeaturesARM", "schedulingControls");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG: {
         if (!(physical_device->supported_extensions.IMG_relaxed_line_rasterization))
            break;
         const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG *a = &supported_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG;
         const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG *b = (const void *) features;
         if (b->relaxedLineRasterization && !a->relaxedLineRasterization)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG", "relaxedLineRasterization");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_render_pass_striped))
            break;
         const VkPhysicalDeviceRenderPassStripedFeaturesARM *a = &supported_VkPhysicalDeviceRenderPassStripedFeaturesARM;
         const VkPhysicalDeviceRenderPassStripedFeaturesARM *b = (const void *) features;
         if (b->renderPassStriped && !a->renderPassStriped)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRenderPassStripedFeaturesARM", "renderPassStriped");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_pipeline_opacity_micromap))
            break;
         const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM *a = &supported_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM;
         const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM *b = (const void *) features;
         if (b->pipelineOpacityMicromap && !a->pipelineOpacityMicromap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineOpacityMicromapFeaturesARM", "pipelineOpacityMicromap");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_maximal_reconvergence))
            break;
         const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR *a = &supported_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR;
         const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR *b = (const void *) features;
         if (b->shaderMaximalReconvergence && !a->shaderMaximalReconvergence)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR", "shaderMaximalReconvergence");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_subgroup_rotate))
            break;
         const VkPhysicalDeviceShaderSubgroupRotateFeatures *a = &supported_VkPhysicalDeviceShaderSubgroupRotateFeatures;
         const VkPhysicalDeviceShaderSubgroupRotateFeatures *b = (const void *) features;
         if (b->shaderSubgroupRotate && !a->shaderSubgroupRotate)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate");
         if (b->shaderSubgroupRotateClustered && !a->shaderSubgroupRotateClustered)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotateClustered");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_expect_assume))
            break;
         const VkPhysicalDeviceShaderExpectAssumeFeatures *a = &supported_VkPhysicalDeviceShaderExpectAssumeFeatures;
         const VkPhysicalDeviceShaderExpectAssumeFeatures *b = (const void *) features;
         if (b->shaderExpectAssume && !a->shaderExpectAssume)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderExpectAssumeFeatures", "shaderExpectAssume");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_shader_float_controls2))
            break;
         const VkPhysicalDeviceShaderFloatControls2Features *a = &supported_VkPhysicalDeviceShaderFloatControls2Features;
         const VkPhysicalDeviceShaderFloatControls2Features *b = (const void *) features;
         if (b->shaderFloatControls2 && !a->shaderFloatControls2)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFloatControls2Features", "shaderFloatControls2");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES: {
         if (!(physical_device->properties.apiVersion >= VK_MAKE_VERSION(1, 4, 0) || physical_device->supported_extensions.KHR_dynamic_rendering_local_read))
            break;
         const VkPhysicalDeviceDynamicRenderingLocalReadFeatures *a = &supported_VkPhysicalDeviceDynamicRenderingLocalReadFeatures;
         const VkPhysicalDeviceDynamicRenderingLocalReadFeatures *b = (const void *) features;
         if (b->dynamicRenderingLocalRead && !a->dynamicRenderingLocalRead)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDynamicRenderingLocalReadFeatures", "dynamicRenderingLocalRead");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_quad_control))
            break;
         const VkPhysicalDeviceShaderQuadControlFeaturesKHR *a = &supported_VkPhysicalDeviceShaderQuadControlFeaturesKHR;
         const VkPhysicalDeviceShaderQuadControlFeaturesKHR *b = (const void *) features;
         if (b->shaderQuadControl && !a->shaderQuadControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderQuadControlFeaturesKHR", "shaderQuadControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_shader_atomic_float16_vector))
            break;
         const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV *a = &supported_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV;
         const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV *b = (const void *) features;
         if (b->shaderFloat16VectorAtomics && !a->shaderFloat16VectorAtomics)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV", "shaderFloat16VectorAtomics");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_map_memory_placed))
            break;
         const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT *a = &supported_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT;
         const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT *b = (const void *) features;
         if (b->memoryMapPlaced && !a->memoryMapPlaced)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMapMemoryPlacedFeaturesEXT", "memoryMapPlaced");
         if (b->memoryMapRangePlaced && !a->memoryMapRangePlaced)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMapMemoryPlacedFeaturesEXT", "memoryMapRangePlaced");
         if (b->memoryUnmapReserve && !a->memoryUnmapReserve)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceMapMemoryPlacedFeaturesEXT", "memoryUnmapReserve");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_bfloat16))
            break;
         const VkPhysicalDeviceShaderBfloat16FeaturesKHR *a = &supported_VkPhysicalDeviceShaderBfloat16FeaturesKHR;
         const VkPhysicalDeviceShaderBfloat16FeaturesKHR *b = (const void *) features;
         if (b->shaderBFloat16Type && !a->shaderBFloat16Type)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderBfloat16FeaturesKHR", "shaderBFloat16Type");
         if (b->shaderBFloat16DotProduct && !a->shaderBFloat16DotProduct)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderBfloat16FeaturesKHR", "shaderBFloat16DotProduct");
         if (b->shaderBFloat16CooperativeMatrix && !a->shaderBFloat16CooperativeMatrix)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderBfloat16FeaturesKHR", "shaderBFloat16CooperativeMatrix");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_raw_access_chains))
            break;
         const VkPhysicalDeviceRawAccessChainsFeaturesNV *a = &supported_VkPhysicalDeviceRawAccessChainsFeaturesNV;
         const VkPhysicalDeviceRawAccessChainsFeaturesNV *b = (const void *) features;
         if (b->shaderRawAccessChains && !a->shaderRawAccessChains)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceRawAccessChainsFeaturesNV", "shaderRawAccessChains");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_command_buffer_inheritance))
            break;
         const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV *a = &supported_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV;
         const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV *b = (const void *) features;
         if (b->commandBufferInheritance && !a->commandBufferInheritance)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCommandBufferInheritanceFeaturesNV", "commandBufferInheritance");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA: {
         if (!(physical_device->supported_extensions.MESA_image_alignment_control))
            break;
         const VkPhysicalDeviceImageAlignmentControlFeaturesMESA *a = &supported_VkPhysicalDeviceImageAlignmentControlFeaturesMESA;
         const VkPhysicalDeviceImageAlignmentControlFeaturesMESA *b = (const void *) features;
         if (b->imageAlignmentControl && !a->imageAlignmentControl)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceImageAlignmentControlFeaturesMESA", "imageAlignmentControl");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_replicated_composites))
            break;
         const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT *a = &supported_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT;
         const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT *b = (const void *) features;
         if (b->shaderReplicatedComposites && !a->shaderReplicatedComposites)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT", "shaderReplicatedComposites");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.EXT_present_mode_fifo_latest_ready || physical_device->supported_extensions.KHR_present_mode_fifo_latest_ready))
            break;
         const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR *a = &supported_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR;
         const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR *b = (const void *) features;
         if (b->presentModeFifoLatestReady && !a->presentModeFifoLatestReady)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR", "presentModeFifoLatestReady");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_cooperative_matrix2))
            break;
         const VkPhysicalDeviceCooperativeMatrix2FeaturesNV *a = &supported_VkPhysicalDeviceCooperativeMatrix2FeaturesNV;
         const VkPhysicalDeviceCooperativeMatrix2FeaturesNV *b = (const void *) features;
         if (b->cooperativeMatrixWorkgroupScope && !a->cooperativeMatrixWorkgroupScope)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixWorkgroupScope");
         if (b->cooperativeMatrixFlexibleDimensions && !a->cooperativeMatrixFlexibleDimensions)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixFlexibleDimensions");
         if (b->cooperativeMatrixReductions && !a->cooperativeMatrixReductions)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixReductions");
         if (b->cooperativeMatrixConversions && !a->cooperativeMatrixConversions)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixConversions");
         if (b->cooperativeMatrixPerElementOperations && !a->cooperativeMatrixPerElementOperations)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixPerElementOperations");
         if (b->cooperativeMatrixTensorAddressing && !a->cooperativeMatrixTensorAddressing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixTensorAddressing");
         if (b->cooperativeMatrixBlockLoads && !a->cooperativeMatrixBlockLoads)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrix2FeaturesNV", "cooperativeMatrixBlockLoads");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_cooperative_matrix_decode_vector))
            break;
         const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV *a = &supported_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV;
         const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV *b = (const void *) features;
         if (b->cooperativeMatrixDecodeVector && !a->cooperativeMatrixDecodeVector)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV", "cooperativeMatrixDecodeVector");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI: {
         if (!(physical_device->supported_extensions.HUAWEI_hdr_vivid))
            break;
         const VkPhysicalDeviceHdrVividFeaturesHUAWEI *a = &supported_VkPhysicalDeviceHdrVividFeaturesHUAWEI;
         const VkPhysicalDeviceHdrVividFeaturesHUAWEI *b = (const void *) features;
         if (b->hdrVivid && !a->hdrVivid)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceHdrVividFeaturesHUAWEI", "hdrVivid");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_vertex_attribute_robustness))
            break;
         const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT *a = &supported_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT;
         const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT *b = (const void *) features;
         if (b->vertexAttributeRobustness && !a->vertexAttributeRobustness)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT", "vertexAttributeRobustness");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.EXT_depth_clamp_zero_one || physical_device->supported_extensions.KHR_depth_clamp_zero_one))
            break;
         const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR *a = &supported_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR;
         const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR *b = (const void *) features;
         if (b->depthClampZeroOne && !a->depthClampZeroOne)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDepthClampZeroOneFeaturesKHR", "depthClampZeroOne");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_cooperative_vector))
            break;
         const VkPhysicalDeviceCooperativeVectorFeaturesNV *a = &supported_VkPhysicalDeviceCooperativeVectorFeaturesNV;
         const VkPhysicalDeviceCooperativeVectorFeaturesNV *b = (const void *) features;
         if (b->cooperativeVector && !a->cooperativeVector)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeVectorFeaturesNV", "cooperativeVector");
         if (b->cooperativeVectorTraining && !a->cooperativeVectorTraining)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceCooperativeVectorFeaturesNV", "cooperativeVectorTraining");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_tile_shading))
            break;
         const VkPhysicalDeviceTileShadingFeaturesQCOM *a = &supported_VkPhysicalDeviceTileShadingFeaturesQCOM;
         const VkPhysicalDeviceTileShadingFeaturesQCOM *b = (const void *) features;
         if (b->tileShading && !a->tileShading)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShading");
         if (b->tileShadingFragmentStage && !a->tileShadingFragmentStage)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingFragmentStage");
         if (b->tileShadingColorAttachments && !a->tileShadingColorAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingColorAttachments");
         if (b->tileShadingDepthAttachments && !a->tileShadingDepthAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingDepthAttachments");
         if (b->tileShadingStencilAttachments && !a->tileShadingStencilAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingStencilAttachments");
         if (b->tileShadingInputAttachments && !a->tileShadingInputAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingInputAttachments");
         if (b->tileShadingSampledAttachments && !a->tileShadingSampledAttachments)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingSampledAttachments");
         if (b->tileShadingPerTileDraw && !a->tileShadingPerTileDraw)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingPerTileDraw");
         if (b->tileShadingPerTileDispatch && !a->tileShadingPerTileDispatch)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingPerTileDispatch");
         if (b->tileShadingDispatchTile && !a->tileShadingDispatchTile)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingDispatchTile");
         if (b->tileShadingApron && !a->tileShadingApron)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingApron");
         if (b->tileShadingAnisotropicApron && !a->tileShadingAnisotropicApron)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingAnisotropicApron");
         if (b->tileShadingAtomicOps && !a->tileShadingAtomicOps)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingAtomicOps");
         if (b->tileShadingImageProcessing && !a->tileShadingImageProcessing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTileShadingFeaturesQCOM", "tileShadingImageProcessing");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE: {
         if (!(physical_device->supported_extensions.VALVE_fragment_density_map_layered))
            break;
         const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE *a = &supported_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE;
         const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE *b = (const void *) features;
         if (b->fragmentDensityMapLayered && !a->fragmentDensityMapLayered)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE", "fragmentDensityMapLayered");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_present_metering))
            break;
         const VkPhysicalDevicePresentMeteringFeaturesNV *a = &supported_VkPhysicalDevicePresentMeteringFeaturesNV;
         const VkPhysicalDevicePresentMeteringFeaturesNV *b = (const void *) features;
         if (b->presentMetering && !a->presentMetering)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePresentMeteringFeaturesNV", "presentMetering");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_uniform_buffer_unsized_array))
            break;
         const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT *a = &supported_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT;
         const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT *b = (const void *) features;
         if (b->shaderUniformBufferUnsizedArray && !a->shaderUniformBufferUnsizedArray)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT", "shaderUniformBufferUnsizedArray");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE: {
         if (!(physical_device->supported_extensions.VALVE_shader_mixed_float_dot_product))
            break;
         const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE *a = &supported_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE;
         const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE *b = (const void *) features;
         if (b->shaderMixedFloatDotProductFloat16AccFloat32 && !a->shaderMixedFloatDotProductFloat16AccFloat32)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE", "shaderMixedFloatDotProductFloat16AccFloat32");
         if (b->shaderMixedFloatDotProductFloat16AccFloat16 && !a->shaderMixedFloatDotProductFloat16AccFloat16)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE", "shaderMixedFloatDotProductFloat16AccFloat16");
         if (b->shaderMixedFloatDotProductBFloat16Acc && !a->shaderMixedFloatDotProductBFloat16Acc)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE", "shaderMixedFloatDotProductBFloat16Acc");
         if (b->shaderMixedFloatDotProductFloat8AccFloat32 && !a->shaderMixedFloatDotProductFloat8AccFloat32)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE", "shaderMixedFloatDotProductFloat8AccFloat32");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_primitive_restart_index))
            break;
         const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT *a = &supported_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT;
         const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT *b = (const void *) features;
         if (b->primitiveRestartIndex && !a->primitiveRestartIndex)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT", "primitiveRestartIndex");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_format_pack))
            break;
         const VkPhysicalDeviceFormatPackFeaturesARM *a = &supported_VkPhysicalDeviceFormatPackFeaturesARM;
         const VkPhysicalDeviceFormatPackFeaturesARM *b = (const void *) features;
         if (b->formatPack && !a->formatPack)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceFormatPackFeaturesARM", "formatPack");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC: {
         if (!(physical_device->supported_extensions.SEC_throttle_hint))
            break;
         const VkPhysicalDeviceThrottleHintFeaturesSEC *a = &supported_VkPhysicalDeviceThrottleHintFeaturesSEC;
         const VkPhysicalDeviceThrottleHintFeaturesSEC *b = (const void *) features;
         if (b->throttleHint && !a->throttleHint)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceThrottleHintFeaturesSEC", "throttleHint");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_tensors))
            break;
         const VkPhysicalDeviceTensorFeaturesARM *a = &supported_VkPhysicalDeviceTensorFeaturesARM;
         const VkPhysicalDeviceTensorFeaturesARM *b = (const void *) features;
         if (b->tensorNonPacked && !a->tensorNonPacked)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "tensorNonPacked");
         if (b->shaderTensorAccess && !a->shaderTensorAccess)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "shaderTensorAccess");
         if (b->shaderStorageTensorArrayDynamicIndexing && !a->shaderStorageTensorArrayDynamicIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "shaderStorageTensorArrayDynamicIndexing");
         if (b->shaderStorageTensorArrayNonUniformIndexing && !a->shaderStorageTensorArrayNonUniformIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "shaderStorageTensorArrayNonUniformIndexing");
         if (b->descriptorBindingStorageTensorUpdateAfterBind && !a->descriptorBindingStorageTensorUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "descriptorBindingStorageTensorUpdateAfterBind");
         if (b->tensors && !a->tensors)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTensorFeaturesARM", "tensors");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_tensors))
            break;
         const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM *a = &supported_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM;
         const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM *b = (const void *) features;
         if (b->descriptorBufferTensorDescriptors && !a->descriptorBufferTensorDescriptors)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorBufferTensorFeaturesARM", "descriptorBufferTensorDescriptors");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_float8))
            break;
         const VkPhysicalDeviceShaderFloat8FeaturesEXT *a = &supported_VkPhysicalDeviceShaderFloat8FeaturesEXT;
         const VkPhysicalDeviceShaderFloat8FeaturesEXT *b = (const void *) features;
         if (b->shaderFloat8 && !a->shaderFloat8)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFloat8FeaturesEXT", "shaderFloat8");
         if (b->shaderFloat8CooperativeMatrix && !a->shaderFloat8CooperativeMatrix)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderFloat8FeaturesEXT", "shaderFloat8CooperativeMatrix");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_data_graph))
            break;
         const VkPhysicalDeviceDataGraphFeaturesARM *a = &supported_VkPhysicalDeviceDataGraphFeaturesARM;
         const VkPhysicalDeviceDataGraphFeaturesARM *b = (const void *) features;
         if (b->dataGraph && !a->dataGraph)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphFeaturesARM", "dataGraph");
         if (b->dataGraphUpdateAfterBind && !a->dataGraphUpdateAfterBind)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphFeaturesARM", "dataGraphUpdateAfterBind");
         if (b->dataGraphSpecializationConstants && !a->dataGraphSpecializationConstants)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphFeaturesARM", "dataGraphSpecializationConstants");
         if (b->dataGraphDescriptorBuffer && !a->dataGraphDescriptorBuffer)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphFeaturesARM", "dataGraphDescriptorBuffer");
         if (b->dataGraphShaderModule && !a->dataGraphShaderModule)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphFeaturesARM", "dataGraphShaderModule");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC: {
         if (!(physical_device->supported_extensions.SEC_pipeline_cache_incremental_mode))
            break;
         const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC *a = &supported_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC;
         const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC *b = (const void *) features;
         if (b->pipelineCacheIncrementalMode && !a->pipelineCacheIncrementalMode)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC", "pipelineCacheIncrementalMode");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_data_graph_model))
            break;
         const VkPhysicalDeviceDataGraphModelFeaturesQCOM *a = &supported_VkPhysicalDeviceDataGraphModelFeaturesQCOM;
         const VkPhysicalDeviceDataGraphModelFeaturesQCOM *b = (const void *) features;
         if (b->dataGraphModel && !a->dataGraphModel)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphModelFeaturesQCOM", "dataGraphModel");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_untyped_pointers))
            break;
         const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR *a = &supported_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR;
         const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR *b = (const void *) features;
         if (b->shaderUntypedPointers && !a->shaderUntypedPointers)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderUntypedPointersFeaturesKHR", "shaderUntypedPointers");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE: {
         if (!(physical_device->supported_extensions.VALVE_video_encode_rgb_conversion))
            break;
         const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE *a = &supported_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE;
         const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE *b = (const void *) features;
         if (b->videoEncodeRgbConversion && !a->videoEncodeRgbConversion)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE", "videoEncodeRgbConversion");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_64bit_indexing))
            break;
         const VkPhysicalDeviceShader64BitIndexingFeaturesEXT *a = &supported_VkPhysicalDeviceShader64BitIndexingFeaturesEXT;
         const VkPhysicalDeviceShader64BitIndexingFeaturesEXT *b = (const void *) features;
         if (b->shader64BitIndexing && !a->shader64BitIndexing)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShader64BitIndexingFeaturesEXT", "shader64BitIndexing");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM: {
         if (!(physical_device->supported_extensions.QCOM_queue_perf_hint))
            break;
         const VkPhysicalDeviceQueuePerfHintFeaturesQCOM *a = &supported_VkPhysicalDeviceQueuePerfHintFeaturesQCOM;
         const VkPhysicalDeviceQueuePerfHintFeaturesQCOM *b = (const void *) features;
         if (b->queuePerfHint && !a->queuePerfHint)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceQueuePerfHintFeaturesQCOM", "queuePerfHint");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_performance_counters_by_region))
            break;
         const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM *a = &supported_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM;
         const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM *b = (const void *) features;
         if (b->performanceCountersByRegion && !a->performanceCountersByRegion)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDevicePerformanceCountersByRegionFeaturesARM", "performanceCountersByRegion");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV: {
         if (!(physical_device->supported_extensions.NV_compute_occupancy_priority))
            break;
         const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV *a = &supported_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV;
         const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV *b = (const void *) features;
         if (b->computeOccupancyPriority && !a->computeOccupancyPriority)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV", "computeOccupancyPriority");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_long_vector))
            break;
         const VkPhysicalDeviceShaderLongVectorFeaturesEXT *a = &supported_VkPhysicalDeviceShaderLongVectorFeaturesEXT;
         const VkPhysicalDeviceShaderLongVectorFeaturesEXT *b = (const void *) features;
         if (b->longVector && !a->longVector)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderLongVectorFeaturesEXT", "longVector");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_texture_compression_astc_3d))
            break;
         const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT *a = &supported_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT;
         const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT *b = (const void *) features;
         if (b->textureCompressionASTC_3D && !a->textureCompressionASTC_3D)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT", "textureCompressionASTC_3D");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_subgroup_partitioned))
            break;
         const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT *a = &supported_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT;
         const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT *b = (const void *) features;
         if (b->shaderSubgroupPartitioned && !a->shaderSubgroupPartitioned)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT", "shaderSubgroupPartitioned");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_shader_split_barrier))
            break;
         const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT *a = &supported_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT;
         const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT *b = (const void *) features;
         if (b->shaderSplitBarrier && !a->shaderSplitBarrier)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderSplitBarrierFeaturesEXT", "shaderSplitBarrier");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT: {
         if (!(physical_device->supported_extensions.EXT_descriptor_heap))
            break;
         const VkPhysicalDeviceDescriptorHeapFeaturesEXT *a = &supported_VkPhysicalDeviceDescriptorHeapFeaturesEXT;
         const VkPhysicalDeviceDescriptorHeapFeaturesEXT *b = (const void *) features;
         if (b->descriptorHeap && !a->descriptorHeap)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorHeapFeaturesEXT", "descriptorHeap");
         if (b->descriptorHeapCaptureReplay && !a->descriptorHeapCaptureReplay)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDescriptorHeapFeaturesEXT", "descriptorHeapCaptureReplay");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_shader_instrumentation))
            break;
         const VkPhysicalDeviceShaderInstrumentationFeaturesARM *a = &supported_VkPhysicalDeviceShaderInstrumentationFeaturesARM;
         const VkPhysicalDeviceShaderInstrumentationFeaturesARM *b = (const void *) features;
         if (b->shaderInstrumentation && !a->shaderInstrumentation)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderInstrumentationFeaturesARM", "shaderInstrumentation");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_device_address_commands))
            break;
         const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR *a = &supported_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR;
         const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR *b = (const void *) features;
         if (b->deviceAddressCommands && !a->deviceAddressCommands)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR", "deviceAddressCommands");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_constant_data))
            break;
         const VkPhysicalDeviceShaderConstantDataFeaturesKHR *a = &supported_VkPhysicalDeviceShaderConstantDataFeaturesKHR;
         const VkPhysicalDeviceShaderConstantDataFeaturesKHR *b = (const void *) features;
         if (b->shaderConstantData && !a->shaderConstantData)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderConstantDataFeaturesKHR", "shaderConstantData");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR: {
         if (!(physical_device->supported_extensions.KHR_shader_abort))
            break;
         const VkPhysicalDeviceShaderAbortFeaturesKHR *a = &supported_VkPhysicalDeviceShaderAbortFeaturesKHR;
         const VkPhysicalDeviceShaderAbortFeaturesKHR *b = (const void *) features;
         if (b->shaderAbort && !a->shaderAbort)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceShaderAbortFeaturesKHR", "shaderAbort");
         break;
      }
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM: {
         if (!(physical_device->supported_extensions.ARM_data_graph_optical_flow))
            break;
         const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM *a = &supported_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM;
         const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM *b = (const void *) features;
         if (b->dataGraphOpticalFlow && !a->dataGraphOpticalFlow)
            return vk_errorf(physical_device, VK_ERROR_FEATURE_NOT_PRESENT,
                             "%s.%s not supported", "VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM", "dataGraphOpticalFlow");
         break;
      }
      default:
         break;
      }
   } // for each extension structure
   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vk_common_GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                     VkPhysicalDeviceFeatures2 *pFeatures)
{
   VK_FROM_HANDLE(vk_physical_device, pdevice, physicalDevice);

   pFeatures->features.robustBufferAccess = pdevice->supported_features.robustBufferAccess;
   pFeatures->features.fullDrawIndexUint32 = pdevice->supported_features.fullDrawIndexUint32;
   pFeatures->features.imageCubeArray = pdevice->supported_features.imageCubeArray;
   pFeatures->features.independentBlend = pdevice->supported_features.independentBlend;
   pFeatures->features.geometryShader = pdevice->supported_features.geometryShader;
   pFeatures->features.tessellationShader = pdevice->supported_features.tessellationShader;
   pFeatures->features.sampleRateShading = pdevice->supported_features.sampleRateShading;
   pFeatures->features.dualSrcBlend = pdevice->supported_features.dualSrcBlend;
   pFeatures->features.logicOp = pdevice->supported_features.logicOp;
   pFeatures->features.multiDrawIndirect = pdevice->supported_features.multiDrawIndirect;
   pFeatures->features.drawIndirectFirstInstance = pdevice->supported_features.drawIndirectFirstInstance;
   pFeatures->features.depthClamp = pdevice->supported_features.depthClamp;
   pFeatures->features.depthBiasClamp = pdevice->supported_features.depthBiasClamp;
   pFeatures->features.fillModeNonSolid = pdevice->supported_features.fillModeNonSolid;
   pFeatures->features.depthBounds = pdevice->supported_features.depthBounds;
   pFeatures->features.wideLines = pdevice->supported_features.wideLines;
   pFeatures->features.largePoints = pdevice->supported_features.largePoints;
   pFeatures->features.alphaToOne = pdevice->supported_features.alphaToOne;
   pFeatures->features.multiViewport = pdevice->supported_features.multiViewport;
   pFeatures->features.samplerAnisotropy = pdevice->supported_features.samplerAnisotropy;
   pFeatures->features.textureCompressionETC2 = pdevice->supported_features.textureCompressionETC2;
   pFeatures->features.textureCompressionASTC_LDR = pdevice->supported_features.textureCompressionASTC_LDR;
   pFeatures->features.textureCompressionBC = pdevice->supported_features.textureCompressionBC;
   pFeatures->features.occlusionQueryPrecise = pdevice->supported_features.occlusionQueryPrecise;
   pFeatures->features.pipelineStatisticsQuery = pdevice->supported_features.pipelineStatisticsQuery;
   pFeatures->features.vertexPipelineStoresAndAtomics = pdevice->supported_features.vertexPipelineStoresAndAtomics;
   pFeatures->features.fragmentStoresAndAtomics = pdevice->supported_features.fragmentStoresAndAtomics;
   pFeatures->features.shaderTessellationAndGeometryPointSize = pdevice->supported_features.shaderTessellationAndGeometryPointSize;
   pFeatures->features.shaderImageGatherExtended = pdevice->supported_features.shaderImageGatherExtended;
   pFeatures->features.shaderStorageImageExtendedFormats = pdevice->supported_features.shaderStorageImageExtendedFormats;
   pFeatures->features.shaderStorageImageMultisample = pdevice->supported_features.shaderStorageImageMultisample;
   pFeatures->features.shaderStorageImageReadWithoutFormat = pdevice->supported_features.shaderStorageImageReadWithoutFormat;
   pFeatures->features.shaderStorageImageWriteWithoutFormat = pdevice->supported_features.shaderStorageImageWriteWithoutFormat;
   pFeatures->features.shaderUniformBufferArrayDynamicIndexing = pdevice->supported_features.shaderUniformBufferArrayDynamicIndexing;
   pFeatures->features.shaderSampledImageArrayDynamicIndexing = pdevice->supported_features.shaderSampledImageArrayDynamicIndexing;
   pFeatures->features.shaderStorageBufferArrayDynamicIndexing = pdevice->supported_features.shaderStorageBufferArrayDynamicIndexing;
   pFeatures->features.shaderStorageImageArrayDynamicIndexing = pdevice->supported_features.shaderStorageImageArrayDynamicIndexing;
   pFeatures->features.shaderClipDistance = pdevice->supported_features.shaderClipDistance;
   pFeatures->features.shaderCullDistance = pdevice->supported_features.shaderCullDistance;
   pFeatures->features.shaderFloat64 = pdevice->supported_features.shaderFloat64;
   pFeatures->features.shaderInt64 = pdevice->supported_features.shaderInt64;
   pFeatures->features.shaderInt16 = pdevice->supported_features.shaderInt16;
   pFeatures->features.shaderResourceResidency = pdevice->supported_features.shaderResourceResidency;
   pFeatures->features.shaderResourceMinLod = pdevice->supported_features.shaderResourceMinLod;
   pFeatures->features.sparseBinding = pdevice->supported_features.sparseBinding;
   pFeatures->features.sparseResidencyBuffer = pdevice->supported_features.sparseResidencyBuffer;
   pFeatures->features.sparseResidencyImage2D = pdevice->supported_features.sparseResidencyImage2D;
   pFeatures->features.sparseResidencyImage3D = pdevice->supported_features.sparseResidencyImage3D;
   pFeatures->features.sparseResidency2Samples = pdevice->supported_features.sparseResidency2Samples;
   pFeatures->features.sparseResidency4Samples = pdevice->supported_features.sparseResidency4Samples;
   pFeatures->features.sparseResidency8Samples = pdevice->supported_features.sparseResidency8Samples;
   pFeatures->features.sparseResidency16Samples = pdevice->supported_features.sparseResidency16Samples;
   pFeatures->features.sparseResidencyAliased = pdevice->supported_features.sparseResidencyAliased;
   pFeatures->features.variableMultisampleRate = pdevice->supported_features.variableMultisampleRate;
   pFeatures->features.inheritedQueries = pdevice->supported_features.inheritedQueries;

   vk_foreach_struct(ext, pFeatures) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV: {
         VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *features = (void *) ext;
         features->deviceGeneratedCommands = pdevice->supported_features.deviceGeneratedCommandsNV;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV: {
         VkPhysicalDevicePushConstantBankFeaturesNV *features = (void *) ext;
         features->pushConstantBank = pdevice->supported_features.pushConstantBank;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV: {
         VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV *features = (void *) ext;
         features->deviceGeneratedCompute = pdevice->supported_features.deviceGeneratedCompute;
         features->deviceGeneratedComputePipelines = pdevice->supported_features.deviceGeneratedComputePipelines;
         features->deviceGeneratedComputeCaptureReplay = pdevice->supported_features.deviceGeneratedComputeCaptureReplay;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES: {
         VkPhysicalDevicePrivateDataFeatures *features = (void *) ext;
         features->privateData = pdevice->supported_features.privateData;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV: {
         VkPhysicalDeviceClusterAccelerationStructureFeaturesNV *features = (void *) ext;
         features->clusterAccelerationStructure = pdevice->supported_features.clusterAccelerationStructure;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
         VkPhysicalDeviceVariablePointersFeatures *features = (void *) ext;
         features->variablePointersStorageBuffer = pdevice->supported_features.variablePointersStorageBuffer;
         features->variablePointers = pdevice->supported_features.variablePointers;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
         VkPhysicalDeviceMultiviewFeatures *features = (void *) ext;
         features->multiview = pdevice->supported_features.multiview;
         features->multiviewGeometryShader = pdevice->supported_features.multiviewGeometryShader;
         features->multiviewTessellationShader = pdevice->supported_features.multiviewTessellationShader;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR: {
         VkPhysicalDevicePresentIdFeaturesKHR *features = (void *) ext;
         features->presentId = pdevice->supported_features.presentId;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR: {
         VkPhysicalDevicePresentId2FeaturesKHR *features = (void *) ext;
         features->presentId2 = pdevice->supported_features.presentId2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR: {
         VkPhysicalDevicePresentWaitFeaturesKHR *features = (void *) ext;
         features->presentWait = pdevice->supported_features.presentWait;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR: {
         VkPhysicalDevicePresentWait2FeaturesKHR *features = (void *) ext;
         features->presentWait2 = pdevice->supported_features.presentWait2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT: {
         VkPhysicalDevicePresentTimingFeaturesEXT *features = (void *) ext;
         features->presentTiming = pdevice->supported_features.presentTiming;
         features->presentAtAbsoluteTime = pdevice->supported_features.presentAtAbsoluteTime;
         features->presentAtRelativeTime = pdevice->supported_features.presentAtRelativeTime;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
         VkPhysicalDevice16BitStorageFeatures *features = (void *) ext;
         features->storageBuffer16BitAccess = pdevice->supported_features.storageBuffer16BitAccess;
         features->uniformAndStorageBuffer16BitAccess = pdevice->supported_features.uniformAndStorageBuffer16BitAccess;
         features->storagePushConstant16 = pdevice->supported_features.storagePushConstant16;
         features->storageInputOutput16 = pdevice->supported_features.storageInputOutput16;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES: {
         VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *features = (void *) ext;
         features->shaderSubgroupExtendedTypes = pdevice->supported_features.shaderSubgroupExtendedTypes;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES: {
         VkPhysicalDeviceSamplerYcbcrConversionFeatures *features = (void *) ext;
         features->samplerYcbcrConversion = pdevice->supported_features.samplerYcbcrConversion;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES: {
         VkPhysicalDeviceProtectedMemoryFeatures *features = (void *) ext;
         features->protectedMemory = pdevice->supported_features.protectedMemory;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT: {
         VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *features = (void *) ext;
         features->advancedBlendCoherentOperations = pdevice->supported_features.advancedBlendCoherentOperations;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT: {
         VkPhysicalDeviceMultiDrawFeaturesEXT *features = (void *) ext;
         features->multiDraw = pdevice->supported_features.multiDraw;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES: {
         VkPhysicalDeviceInlineUniformBlockFeatures *features = (void *) ext;
         features->inlineUniformBlock = pdevice->supported_features.inlineUniformBlock;
         features->descriptorBindingInlineUniformBlockUpdateAfterBind = pdevice->supported_features.descriptorBindingInlineUniformBlockUpdateAfterBind;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES: {
         VkPhysicalDeviceMaintenance4Features *features = (void *) ext;
         features->maintenance4 = pdevice->supported_features.maintenance4;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES: {
         VkPhysicalDeviceMaintenance5Features *features = (void *) ext;
         features->maintenance5 = pdevice->supported_features.maintenance5;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES: {
         VkPhysicalDeviceMaintenance6Features *features = (void *) ext;
         features->maintenance6 = pdevice->supported_features.maintenance6;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR: {
         VkPhysicalDeviceMaintenance7FeaturesKHR *features = (void *) ext;
         features->maintenance7 = pdevice->supported_features.maintenance7;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR: {
         VkPhysicalDeviceMaintenance8FeaturesKHR *features = (void *) ext;
         features->maintenance8 = pdevice->supported_features.maintenance8;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR: {
         VkPhysicalDeviceMaintenance9FeaturesKHR *features = (void *) ext;
         features->maintenance9 = pdevice->supported_features.maintenance9;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR: {
         VkPhysicalDeviceMaintenance11FeaturesKHR *features = (void *) ext;
         features->maintenance11 = pdevice->supported_features.maintenance11;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR: {
         VkPhysicalDeviceMaintenance10FeaturesKHR *features = (void *) ext;
         features->maintenance10 = pdevice->supported_features.maintenance10;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
         VkPhysicalDeviceShaderDrawParametersFeatures *features = (void *) ext;
         features->shaderDrawParameters = pdevice->supported_features.shaderDrawParameters;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES: {
         VkPhysicalDeviceShaderFloat16Int8Features *features = (void *) ext;
         features->shaderFloat16 = pdevice->supported_features.shaderFloat16;
         features->shaderInt8 = pdevice->supported_features.shaderInt8;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES: {
         VkPhysicalDeviceHostQueryResetFeatures *features = (void *) ext;
         features->hostQueryReset = pdevice->supported_features.hostQueryReset;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM: {
         VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM *features = (void *) ext;
         features->elapsedTimerQuery = pdevice->supported_features.elapsedTimerQuery;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES: {
         VkPhysicalDeviceGlobalPriorityQueryFeatures *features = (void *) ext;
         features->globalPriorityQuery = pdevice->supported_features.globalPriorityQuery;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT: {
         VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *features = (void *) ext;
         features->deviceMemoryReport = pdevice->supported_features.deviceMemoryReport;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES: {
         VkPhysicalDeviceDescriptorIndexingFeatures *features = (void *) ext;
         features->shaderInputAttachmentArrayDynamicIndexing = pdevice->supported_features.shaderInputAttachmentArrayDynamicIndexing;
         features->shaderUniformTexelBufferArrayDynamicIndexing = pdevice->supported_features.shaderUniformTexelBufferArrayDynamicIndexing;
         features->shaderStorageTexelBufferArrayDynamicIndexing = pdevice->supported_features.shaderStorageTexelBufferArrayDynamicIndexing;
         features->shaderUniformBufferArrayNonUniformIndexing = pdevice->supported_features.shaderUniformBufferArrayNonUniformIndexing;
         features->shaderSampledImageArrayNonUniformIndexing = pdevice->supported_features.shaderSampledImageArrayNonUniformIndexing;
         features->shaderStorageBufferArrayNonUniformIndexing = pdevice->supported_features.shaderStorageBufferArrayNonUniformIndexing;
         features->shaderStorageImageArrayNonUniformIndexing = pdevice->supported_features.shaderStorageImageArrayNonUniformIndexing;
         features->shaderInputAttachmentArrayNonUniformIndexing = pdevice->supported_features.shaderInputAttachmentArrayNonUniformIndexing;
         features->shaderUniformTexelBufferArrayNonUniformIndexing = pdevice->supported_features.shaderUniformTexelBufferArrayNonUniformIndexing;
         features->shaderStorageTexelBufferArrayNonUniformIndexing = pdevice->supported_features.shaderStorageTexelBufferArrayNonUniformIndexing;
         features->descriptorBindingUniformBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingUniformBufferUpdateAfterBind;
         features->descriptorBindingSampledImageUpdateAfterBind = pdevice->supported_features.descriptorBindingSampledImageUpdateAfterBind;
         features->descriptorBindingStorageImageUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageImageUpdateAfterBind;
         features->descriptorBindingStorageBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageBufferUpdateAfterBind;
         features->descriptorBindingUniformTexelBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingUniformTexelBufferUpdateAfterBind;
         features->descriptorBindingStorageTexelBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageTexelBufferUpdateAfterBind;
         features->descriptorBindingUpdateUnusedWhilePending = pdevice->supported_features.descriptorBindingUpdateUnusedWhilePending;
         features->descriptorBindingPartiallyBound = pdevice->supported_features.descriptorBindingPartiallyBound;
         features->descriptorBindingVariableDescriptorCount = pdevice->supported_features.descriptorBindingVariableDescriptorCount;
         features->runtimeDescriptorArray = pdevice->supported_features.runtimeDescriptorArray;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES: {
         VkPhysicalDeviceTimelineSemaphoreFeatures *features = (void *) ext;
         features->timelineSemaphore = pdevice->supported_features.timelineSemaphore;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES: {
         VkPhysicalDevice8BitStorageFeatures *features = (void *) ext;
         features->storageBuffer8BitAccess = pdevice->supported_features.storageBuffer8BitAccess;
         features->uniformAndStorageBuffer8BitAccess = pdevice->supported_features.uniformAndStorageBuffer8BitAccess;
         features->storagePushConstant8 = pdevice->supported_features.storagePushConstant8;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT: {
         VkPhysicalDeviceConditionalRenderingFeaturesEXT *features = (void *) ext;
         features->conditionalRendering = pdevice->supported_features.conditionalRendering;
         features->inheritedConditionalRendering = pdevice->supported_features.inheritedConditionalRendering;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES: {
         VkPhysicalDeviceVulkanMemoryModelFeatures *features = (void *) ext;
         features->vulkanMemoryModel = pdevice->supported_features.vulkanMemoryModel;
         features->vulkanMemoryModelDeviceScope = pdevice->supported_features.vulkanMemoryModelDeviceScope;
         features->vulkanMemoryModelAvailabilityVisibilityChains = pdevice->supported_features.vulkanMemoryModelAvailabilityVisibilityChains;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES: {
         VkPhysicalDeviceShaderAtomicInt64Features *features = (void *) ext;
         features->shaderBufferInt64Atomics = pdevice->supported_features.shaderBufferInt64Atomics;
         features->shaderSharedInt64Atomics = pdevice->supported_features.shaderSharedInt64Atomics;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT: {
         VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *features = (void *) ext;
         features->shaderBufferFloat32Atomics = pdevice->supported_features.shaderBufferFloat32Atomics;
         features->shaderBufferFloat32AtomicAdd = pdevice->supported_features.shaderBufferFloat32AtomicAdd;
         features->shaderBufferFloat64Atomics = pdevice->supported_features.shaderBufferFloat64Atomics;
         features->shaderBufferFloat64AtomicAdd = pdevice->supported_features.shaderBufferFloat64AtomicAdd;
         features->shaderSharedFloat32Atomics = pdevice->supported_features.shaderSharedFloat32Atomics;
         features->shaderSharedFloat32AtomicAdd = pdevice->supported_features.shaderSharedFloat32AtomicAdd;
         features->shaderSharedFloat64Atomics = pdevice->supported_features.shaderSharedFloat64Atomics;
         features->shaderSharedFloat64AtomicAdd = pdevice->supported_features.shaderSharedFloat64AtomicAdd;
         features->shaderImageFloat32Atomics = pdevice->supported_features.shaderImageFloat32Atomics;
         features->shaderImageFloat32AtomicAdd = pdevice->supported_features.shaderImageFloat32AtomicAdd;
         features->sparseImageFloat32Atomics = pdevice->supported_features.sparseImageFloat32Atomics;
         features->sparseImageFloat32AtomicAdd = pdevice->supported_features.sparseImageFloat32AtomicAdd;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT: {
         VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *features = (void *) ext;
         features->shaderBufferFloat16Atomics = pdevice->supported_features.shaderBufferFloat16Atomics;
         features->shaderBufferFloat16AtomicAdd = pdevice->supported_features.shaderBufferFloat16AtomicAdd;
         features->shaderBufferFloat16AtomicMinMax = pdevice->supported_features.shaderBufferFloat16AtomicMinMax;
         features->shaderBufferFloat32AtomicMinMax = pdevice->supported_features.shaderBufferFloat32AtomicMinMax;
         features->shaderBufferFloat64AtomicMinMax = pdevice->supported_features.shaderBufferFloat64AtomicMinMax;
         features->shaderSharedFloat16Atomics = pdevice->supported_features.shaderSharedFloat16Atomics;
         features->shaderSharedFloat16AtomicAdd = pdevice->supported_features.shaderSharedFloat16AtomicAdd;
         features->shaderSharedFloat16AtomicMinMax = pdevice->supported_features.shaderSharedFloat16AtomicMinMax;
         features->shaderSharedFloat32AtomicMinMax = pdevice->supported_features.shaderSharedFloat32AtomicMinMax;
         features->shaderSharedFloat64AtomicMinMax = pdevice->supported_features.shaderSharedFloat64AtomicMinMax;
         features->shaderImageFloat32AtomicMinMax = pdevice->supported_features.shaderImageFloat32AtomicMinMax;
         features->sparseImageFloat32AtomicMinMax = pdevice->supported_features.sparseImageFloat32AtomicMinMax;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES: {
         VkPhysicalDeviceVertexAttributeDivisorFeatures *features = (void *) ext;
         features->vertexAttributeInstanceRateDivisor = pdevice->supported_features.vertexAttributeInstanceRateDivisor;
         features->vertexAttributeInstanceRateZeroDivisor = pdevice->supported_features.vertexAttributeInstanceRateZeroDivisor;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT: {
         VkPhysicalDeviceASTCDecodeFeaturesEXT *features = (void *) ext;
         features->decodeModeSharedExponent = pdevice->supported_features.decodeModeSharedExponent;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT: {
         VkPhysicalDeviceTransformFeedbackFeaturesEXT *features = (void *) ext;
         features->transformFeedback = pdevice->supported_features.transformFeedback;
         features->geometryStreams = pdevice->supported_features.geometryStreams;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV: {
         VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *features = (void *) ext;
         features->representativeFragmentTest = pdevice->supported_features.representativeFragmentTest;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV: {
         VkPhysicalDeviceExclusiveScissorFeaturesNV *features = (void *) ext;
         features->exclusiveScissor = pdevice->supported_features.exclusiveScissor;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV: {
         VkPhysicalDeviceCornerSampledImageFeaturesNV *features = (void *) ext;
         features->cornerSampledImage = pdevice->supported_features.cornerSampledImage;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR: {
         VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR *features = (void *) ext;
         features->computeDerivativeGroupQuads = pdevice->supported_features.computeDerivativeGroupQuads;
         features->computeDerivativeGroupLinear = pdevice->supported_features.computeDerivativeGroupLinear;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV: {
         VkPhysicalDeviceShaderImageFootprintFeaturesNV *features = (void *) ext;
         features->imageFootprint = pdevice->supported_features.imageFootprint;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV: {
         VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *features = (void *) ext;
         features->dedicatedAllocationImageAliasing = pdevice->supported_features.dedicatedAllocationImageAliasing;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR: {
         VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR *features = (void *) ext;
         features->indirectMemoryCopy = pdevice->supported_features.indirectMemoryCopy;
         features->indirectMemoryToImageCopy = pdevice->supported_features.indirectMemoryToImageCopy;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV: {
         VkPhysicalDeviceCopyMemoryIndirectFeaturesNV *features = (void *) ext;
         features->indirectCopy = pdevice->supported_features.indirectCopy;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT: {
         VkPhysicalDeviceMemoryDecompressionFeaturesEXT *features = (void *) ext;
         features->memoryDecompression = pdevice->supported_features.memoryDecompression;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV: {
         VkPhysicalDeviceShadingRateImageFeaturesNV *features = (void *) ext;
         features->shadingRateImage = pdevice->supported_features.shadingRateImage;
         features->shadingRateCoarseSampleOrder = pdevice->supported_features.shadingRateCoarseSampleOrder;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI: {
         VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *features = (void *) ext;
         features->invocationMask = pdevice->supported_features.invocationMask;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV: {
         VkPhysicalDeviceMeshShaderFeaturesNV *features = (void *) ext;
         features->taskShader = pdevice->supported_features.taskShaderNV;
         features->meshShader = pdevice->supported_features.meshShaderNV;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT: {
         VkPhysicalDeviceMeshShaderFeaturesEXT *features = (void *) ext;
         features->taskShader = pdevice->supported_features.taskShader;
         features->meshShader = pdevice->supported_features.meshShader;
         features->multiviewMeshShader = pdevice->supported_features.multiviewMeshShader;
         features->primitiveFragmentShadingRateMeshShader = pdevice->supported_features.primitiveFragmentShadingRateMeshShader;
         features->meshShaderQueries = pdevice->supported_features.meshShaderQueries;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR: {
         VkPhysicalDeviceAccelerationStructureFeaturesKHR *features = (void *) ext;
         features->accelerationStructure = pdevice->supported_features.accelerationStructure;
         features->accelerationStructureCaptureReplay = pdevice->supported_features.accelerationStructureCaptureReplay;
         features->accelerationStructureIndirectBuild = pdevice->supported_features.accelerationStructureIndirectBuild;
         features->accelerationStructureHostCommands = pdevice->supported_features.accelerationStructureHostCommands;
         features->descriptorBindingAccelerationStructureUpdateAfterBind = pdevice->supported_features.descriptorBindingAccelerationStructureUpdateAfterBind;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR: {
         VkPhysicalDeviceRayTracingPipelineFeaturesKHR *features = (void *) ext;
         features->rayTracingPipeline = pdevice->supported_features.rayTracingPipeline;
         features->rayTracingPipelineShaderGroupHandleCaptureReplay = pdevice->supported_features.rayTracingPipelineShaderGroupHandleCaptureReplay;
         features->rayTracingPipelineShaderGroupHandleCaptureReplayMixed = pdevice->supported_features.rayTracingPipelineShaderGroupHandleCaptureReplayMixed;
         features->rayTracingPipelineTraceRaysIndirect = pdevice->supported_features.rayTracingPipelineTraceRaysIndirect;
         features->rayTraversalPrimitiveCulling = pdevice->supported_features.rayTraversalPrimitiveCulling;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR: {
         VkPhysicalDeviceRayQueryFeaturesKHR *features = (void *) ext;
         features->rayQuery = pdevice->supported_features.rayQuery;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR: {
         VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *features = (void *) ext;
         features->rayTracingMaintenance1 = pdevice->supported_features.rayTracingMaintenance1;
         features->rayTracingPipelineTraceRaysIndirect2 = pdevice->supported_features.rayTracingPipelineTraceRaysIndirect2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT: {
         VkPhysicalDeviceFragmentDensityMapFeaturesEXT *features = (void *) ext;
         features->fragmentDensityMap = pdevice->supported_features.fragmentDensityMap;
         features->fragmentDensityMapDynamic = pdevice->supported_features.fragmentDensityMapDynamic;
         features->fragmentDensityMapNonSubsampledImages = pdevice->supported_features.fragmentDensityMapNonSubsampledImages;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT: {
         VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *features = (void *) ext;
         features->fragmentDensityMapDeferred = pdevice->supported_features.fragmentDensityMapDeferred;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT: {
         VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT *features = (void *) ext;
         features->fragmentDensityMapOffset = pdevice->supported_features.fragmentDensityMapOffset;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES: {
         VkPhysicalDeviceScalarBlockLayoutFeatures *features = (void *) ext;
         features->scalarBlockLayout = pdevice->supported_features.scalarBlockLayout;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES: {
         VkPhysicalDeviceUniformBufferStandardLayoutFeatures *features = (void *) ext;
         features->uniformBufferStandardLayout = pdevice->supported_features.uniformBufferStandardLayout;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT: {
         VkPhysicalDeviceDepthClipEnableFeaturesEXT *features = (void *) ext;
         features->depthClipEnable = pdevice->supported_features.depthClipEnable;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT: {
         VkPhysicalDeviceMemoryPriorityFeaturesEXT *features = (void *) ext;
         features->memoryPriority = pdevice->supported_features.memoryPriority;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT: {
         VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *features = (void *) ext;
         features->pageableDeviceLocalMemory = pdevice->supported_features.pageableDeviceLocalMemory;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES: {
         VkPhysicalDeviceBufferDeviceAddressFeatures *features = (void *) ext;
         features->bufferDeviceAddress = pdevice->supported_features.bufferDeviceAddress;
         features->bufferDeviceAddressCaptureReplay = pdevice->supported_features.bufferDeviceAddressCaptureReplay;
         features->bufferDeviceAddressMultiDevice = pdevice->supported_features.bufferDeviceAddressMultiDevice;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT: {
         VkPhysicalDeviceBufferDeviceAddressFeaturesEXT *features = (void *) ext;
         features->bufferDeviceAddress = pdevice->supported_features.bufferDeviceAddress;
         features->bufferDeviceAddressCaptureReplay = pdevice->supported_features.bufferDeviceAddressCaptureReplayEXT;
         features->bufferDeviceAddressMultiDevice = pdevice->supported_features.bufferDeviceAddressMultiDevice;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES: {
         VkPhysicalDeviceImagelessFramebufferFeatures *features = (void *) ext;
         features->imagelessFramebuffer = pdevice->supported_features.imagelessFramebuffer;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES: {
         VkPhysicalDeviceTextureCompressionASTCHDRFeatures *features = (void *) ext;
         features->textureCompressionASTC_HDR = pdevice->supported_features.textureCompressionASTC_HDR;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV: {
         VkPhysicalDeviceCooperativeMatrixFeaturesNV *features = (void *) ext;
         features->cooperativeMatrix = pdevice->supported_features.cooperativeMatrixNV;
         features->cooperativeMatrixRobustBufferAccess = pdevice->supported_features.cooperativeMatrixRobustBufferAccessNV;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT: {
         VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *features = (void *) ext;
         features->ycbcrImageArrays = pdevice->supported_features.ycbcrImageArrays;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV: {
         VkPhysicalDevicePresentBarrierFeaturesNV *features = (void *) ext;
         features->presentBarrier = pdevice->supported_features.presentBarrier;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR: {
         VkPhysicalDevicePerformanceQueryFeaturesKHR *features = (void *) ext;
         features->performanceCounterQueryPools = pdevice->supported_features.performanceCounterQueryPools;
         features->performanceCounterMultipleQueryPools = pdevice->supported_features.performanceCounterMultipleQueryPools;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV: {
         VkPhysicalDeviceCoverageReductionModeFeaturesNV *features = (void *) ext;
         features->coverageReductionMode = pdevice->supported_features.coverageReductionMode;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL: {
         VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *features = (void *) ext;
         features->shaderIntegerFunctions2 = pdevice->supported_features.shaderIntegerFunctions2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR: {
         VkPhysicalDeviceShaderClockFeaturesKHR *features = (void *) ext;
         features->shaderSubgroupClock = pdevice->supported_features.shaderSubgroupClock;
         features->shaderDeviceClock = pdevice->supported_features.shaderDeviceClock;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES: {
         VkPhysicalDeviceIndexTypeUint8Features *features = (void *) ext;
         features->indexTypeUint8 = pdevice->supported_features.indexTypeUint8;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV: {
         VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *features = (void *) ext;
         features->shaderSMBuiltins = pdevice->supported_features.shaderSMBuiltins;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT: {
         VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *features = (void *) ext;
         features->fragmentShaderSampleInterlock = pdevice->supported_features.fragmentShaderSampleInterlock;
         features->fragmentShaderPixelInterlock = pdevice->supported_features.fragmentShaderPixelInterlock;
         features->fragmentShaderShadingRateInterlock = pdevice->supported_features.fragmentShaderShadingRateInterlock;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES: {
         VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures *features = (void *) ext;
         features->separateDepthStencilLayouts = pdevice->supported_features.separateDepthStencilLayouts;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT: {
         VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *features = (void *) ext;
         features->primitiveTopologyListRestart = pdevice->supported_features.primitiveTopologyListRestart;
         features->primitiveTopologyPatchListRestart = pdevice->supported_features.primitiveTopologyPatchListRestart;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR: {
         VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *features = (void *) ext;
         features->pipelineExecutableInfo = pdevice->supported_features.pipelineExecutableInfo;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES: {
         VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures *features = (void *) ext;
         features->shaderDemoteToHelperInvocation = pdevice->supported_features.shaderDemoteToHelperInvocation;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT: {
         VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *features = (void *) ext;
         features->texelBufferAlignment = pdevice->supported_features.texelBufferAlignment;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES: {
         VkPhysicalDeviceSubgroupSizeControlFeatures *features = (void *) ext;
         features->subgroupSizeControl = pdevice->supported_features.subgroupSizeControl;
         features->computeFullSubgroups = pdevice->supported_features.computeFullSubgroups;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES: {
         VkPhysicalDeviceLineRasterizationFeatures *features = (void *) ext;
         features->rectangularLines = pdevice->supported_features.rectangularLines;
         features->bresenhamLines = pdevice->supported_features.bresenhamLines;
         features->smoothLines = pdevice->supported_features.smoothLines;
         features->stippledRectangularLines = pdevice->supported_features.stippledRectangularLines;
         features->stippledBresenhamLines = pdevice->supported_features.stippledBresenhamLines;
         features->stippledSmoothLines = pdevice->supported_features.stippledSmoothLines;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES: {
         VkPhysicalDevicePipelineCreationCacheControlFeatures *features = (void *) ext;
         features->pipelineCreationCacheControl = pdevice->supported_features.pipelineCreationCacheControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
         VkPhysicalDeviceVulkan11Features *features = (void *) ext;
         features->storageBuffer16BitAccess = pdevice->supported_features.storageBuffer16BitAccess;
         features->uniformAndStorageBuffer16BitAccess = pdevice->supported_features.uniformAndStorageBuffer16BitAccess;
         features->storagePushConstant16 = pdevice->supported_features.storagePushConstant16;
         features->storageInputOutput16 = pdevice->supported_features.storageInputOutput16;
         features->multiview = pdevice->supported_features.multiview;
         features->multiviewGeometryShader = pdevice->supported_features.multiviewGeometryShader;
         features->multiviewTessellationShader = pdevice->supported_features.multiviewTessellationShader;
         features->variablePointersStorageBuffer = pdevice->supported_features.variablePointersStorageBuffer;
         features->variablePointers = pdevice->supported_features.variablePointers;
         features->protectedMemory = pdevice->supported_features.protectedMemory;
         features->samplerYcbcrConversion = pdevice->supported_features.samplerYcbcrConversion;
         features->shaderDrawParameters = pdevice->supported_features.shaderDrawParameters;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
         VkPhysicalDeviceVulkan12Features *features = (void *) ext;
         features->samplerMirrorClampToEdge = pdevice->supported_features.samplerMirrorClampToEdge;
         features->drawIndirectCount = pdevice->supported_features.drawIndirectCount;
         features->storageBuffer8BitAccess = pdevice->supported_features.storageBuffer8BitAccess;
         features->uniformAndStorageBuffer8BitAccess = pdevice->supported_features.uniformAndStorageBuffer8BitAccess;
         features->storagePushConstant8 = pdevice->supported_features.storagePushConstant8;
         features->shaderBufferInt64Atomics = pdevice->supported_features.shaderBufferInt64Atomics;
         features->shaderSharedInt64Atomics = pdevice->supported_features.shaderSharedInt64Atomics;
         features->shaderFloat16 = pdevice->supported_features.shaderFloat16;
         features->shaderInt8 = pdevice->supported_features.shaderInt8;
         features->descriptorIndexing = pdevice->supported_features.descriptorIndexing;
         features->shaderInputAttachmentArrayDynamicIndexing = pdevice->supported_features.shaderInputAttachmentArrayDynamicIndexing;
         features->shaderUniformTexelBufferArrayDynamicIndexing = pdevice->supported_features.shaderUniformTexelBufferArrayDynamicIndexing;
         features->shaderStorageTexelBufferArrayDynamicIndexing = pdevice->supported_features.shaderStorageTexelBufferArrayDynamicIndexing;
         features->shaderUniformBufferArrayNonUniformIndexing = pdevice->supported_features.shaderUniformBufferArrayNonUniformIndexing;
         features->shaderSampledImageArrayNonUniformIndexing = pdevice->supported_features.shaderSampledImageArrayNonUniformIndexing;
         features->shaderStorageBufferArrayNonUniformIndexing = pdevice->supported_features.shaderStorageBufferArrayNonUniformIndexing;
         features->shaderStorageImageArrayNonUniformIndexing = pdevice->supported_features.shaderStorageImageArrayNonUniformIndexing;
         features->shaderInputAttachmentArrayNonUniformIndexing = pdevice->supported_features.shaderInputAttachmentArrayNonUniformIndexing;
         features->shaderUniformTexelBufferArrayNonUniformIndexing = pdevice->supported_features.shaderUniformTexelBufferArrayNonUniformIndexing;
         features->shaderStorageTexelBufferArrayNonUniformIndexing = pdevice->supported_features.shaderStorageTexelBufferArrayNonUniformIndexing;
         features->descriptorBindingUniformBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingUniformBufferUpdateAfterBind;
         features->descriptorBindingSampledImageUpdateAfterBind = pdevice->supported_features.descriptorBindingSampledImageUpdateAfterBind;
         features->descriptorBindingStorageImageUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageImageUpdateAfterBind;
         features->descriptorBindingStorageBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageBufferUpdateAfterBind;
         features->descriptorBindingUniformTexelBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingUniformTexelBufferUpdateAfterBind;
         features->descriptorBindingStorageTexelBufferUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageTexelBufferUpdateAfterBind;
         features->descriptorBindingUpdateUnusedWhilePending = pdevice->supported_features.descriptorBindingUpdateUnusedWhilePending;
         features->descriptorBindingPartiallyBound = pdevice->supported_features.descriptorBindingPartiallyBound;
         features->descriptorBindingVariableDescriptorCount = pdevice->supported_features.descriptorBindingVariableDescriptorCount;
         features->runtimeDescriptorArray = pdevice->supported_features.runtimeDescriptorArray;
         features->samplerFilterMinmax = pdevice->supported_features.samplerFilterMinmax;
         features->scalarBlockLayout = pdevice->supported_features.scalarBlockLayout;
         features->imagelessFramebuffer = pdevice->supported_features.imagelessFramebuffer;
         features->uniformBufferStandardLayout = pdevice->supported_features.uniformBufferStandardLayout;
         features->shaderSubgroupExtendedTypes = pdevice->supported_features.shaderSubgroupExtendedTypes;
         features->separateDepthStencilLayouts = pdevice->supported_features.separateDepthStencilLayouts;
         features->hostQueryReset = pdevice->supported_features.hostQueryReset;
         features->timelineSemaphore = pdevice->supported_features.timelineSemaphore;
         features->bufferDeviceAddress = pdevice->supported_features.bufferDeviceAddress;
         features->bufferDeviceAddressCaptureReplay = pdevice->supported_features.bufferDeviceAddressCaptureReplay;
         features->bufferDeviceAddressMultiDevice = pdevice->supported_features.bufferDeviceAddressMultiDevice;
         features->vulkanMemoryModel = pdevice->supported_features.vulkanMemoryModel;
         features->vulkanMemoryModelDeviceScope = pdevice->supported_features.vulkanMemoryModelDeviceScope;
         features->vulkanMemoryModelAvailabilityVisibilityChains = pdevice->supported_features.vulkanMemoryModelAvailabilityVisibilityChains;
         features->shaderOutputViewportIndex = pdevice->supported_features.shaderOutputViewportIndex;
         features->shaderOutputLayer = pdevice->supported_features.shaderOutputLayer;
         features->subgroupBroadcastDynamicId = pdevice->supported_features.subgroupBroadcastDynamicId;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
         VkPhysicalDeviceVulkan13Features *features = (void *) ext;
         features->robustImageAccess = pdevice->supported_features.robustImageAccess;
         features->inlineUniformBlock = pdevice->supported_features.inlineUniformBlock;
         features->descriptorBindingInlineUniformBlockUpdateAfterBind = pdevice->supported_features.descriptorBindingInlineUniformBlockUpdateAfterBind;
         features->pipelineCreationCacheControl = pdevice->supported_features.pipelineCreationCacheControl;
         features->privateData = pdevice->supported_features.privateData;
         features->shaderDemoteToHelperInvocation = pdevice->supported_features.shaderDemoteToHelperInvocation;
         features->shaderTerminateInvocation = pdevice->supported_features.shaderTerminateInvocation;
         features->subgroupSizeControl = pdevice->supported_features.subgroupSizeControl;
         features->computeFullSubgroups = pdevice->supported_features.computeFullSubgroups;
         features->synchronization2 = pdevice->supported_features.synchronization2;
         features->textureCompressionASTC_HDR = pdevice->supported_features.textureCompressionASTC_HDR;
         features->shaderZeroInitializeWorkgroupMemory = pdevice->supported_features.shaderZeroInitializeWorkgroupMemory;
         features->dynamicRendering = pdevice->supported_features.dynamicRendering;
         features->shaderIntegerDotProduct = pdevice->supported_features.shaderIntegerDotProduct;
         features->maintenance4 = pdevice->supported_features.maintenance4;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES: {
         VkPhysicalDeviceVulkan14Features *features = (void *) ext;
         features->globalPriorityQuery = pdevice->supported_features.globalPriorityQuery;
         features->shaderSubgroupRotate = pdevice->supported_features.shaderSubgroupRotate;
         features->shaderSubgroupRotateClustered = pdevice->supported_features.shaderSubgroupRotateClustered;
         features->shaderFloatControls2 = pdevice->supported_features.shaderFloatControls2;
         features->shaderExpectAssume = pdevice->supported_features.shaderExpectAssume;
         features->rectangularLines = pdevice->supported_features.rectangularLines;
         features->bresenhamLines = pdevice->supported_features.bresenhamLines;
         features->smoothLines = pdevice->supported_features.smoothLines;
         features->stippledRectangularLines = pdevice->supported_features.stippledRectangularLines;
         features->stippledBresenhamLines = pdevice->supported_features.stippledBresenhamLines;
         features->stippledSmoothLines = pdevice->supported_features.stippledSmoothLines;
         features->vertexAttributeInstanceRateDivisor = pdevice->supported_features.vertexAttributeInstanceRateDivisor;
         features->vertexAttributeInstanceRateZeroDivisor = pdevice->supported_features.vertexAttributeInstanceRateZeroDivisor;
         features->indexTypeUint8 = pdevice->supported_features.indexTypeUint8;
         features->dynamicRenderingLocalRead = pdevice->supported_features.dynamicRenderingLocalRead;
         features->maintenance5 = pdevice->supported_features.maintenance5;
         features->maintenance6 = pdevice->supported_features.maintenance6;
         features->pipelineProtectedAccess = pdevice->supported_features.pipelineProtectedAccess;
         features->pipelineRobustness = pdevice->supported_features.pipelineRobustness;
         features->hostImageCopy = pdevice->supported_features.hostImageCopy;
         features->pushDescriptor = pdevice->supported_features.pushDescriptor;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD: {
         VkPhysicalDeviceCoherentMemoryFeaturesAMD *features = (void *) ext;
         features->deviceCoherentMemory = pdevice->supported_features.deviceCoherentMemory;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD: {
         VkPhysicalDeviceGpaFeaturesAMD *features = (void *) ext;
         features->perfCounters = pdevice->supported_features.perfCounters;
         features->streamingPerfCounters = pdevice->supported_features.streamingPerfCounters;
         features->sqThreadTracing = pdevice->supported_features.sqThreadTracing;
         features->clockModes = pdevice->supported_features.clockModes;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT: {
         VkPhysicalDeviceCustomBorderColorFeaturesEXT *features = (void *) ext;
         features->customBorderColors = pdevice->supported_features.customBorderColors;
         features->customBorderColorWithoutFormat = pdevice->supported_features.customBorderColorWithoutFormat;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT: {
         VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *features = (void *) ext;
         features->borderColorSwizzle = pdevice->supported_features.borderColorSwizzle;
         features->borderColorSwizzleFromImage = pdevice->supported_features.borderColorSwizzleFromImage;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT: {
         VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *features = (void *) ext;
         features->extendedDynamicState = pdevice->supported_features.extendedDynamicState;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT: {
         VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *features = (void *) ext;
         features->extendedDynamicState2 = pdevice->supported_features.extendedDynamicState2;
         features->extendedDynamicState2LogicOp = pdevice->supported_features.extendedDynamicState2LogicOp;
         features->extendedDynamicState2PatchControlPoints = pdevice->supported_features.extendedDynamicState2PatchControlPoints;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT: {
         VkPhysicalDeviceExtendedDynamicState3FeaturesEXT *features = (void *) ext;
         features->extendedDynamicState3TessellationDomainOrigin = pdevice->supported_features.extendedDynamicState3TessellationDomainOrigin;
         features->extendedDynamicState3DepthClampEnable = pdevice->supported_features.extendedDynamicState3DepthClampEnable;
         features->extendedDynamicState3PolygonMode = pdevice->supported_features.extendedDynamicState3PolygonMode;
         features->extendedDynamicState3RasterizationSamples = pdevice->supported_features.extendedDynamicState3RasterizationSamples;
         features->extendedDynamicState3SampleMask = pdevice->supported_features.extendedDynamicState3SampleMask;
         features->extendedDynamicState3AlphaToCoverageEnable = pdevice->supported_features.extendedDynamicState3AlphaToCoverageEnable;
         features->extendedDynamicState3AlphaToOneEnable = pdevice->supported_features.extendedDynamicState3AlphaToOneEnable;
         features->extendedDynamicState3LogicOpEnable = pdevice->supported_features.extendedDynamicState3LogicOpEnable;
         features->extendedDynamicState3ColorBlendEnable = pdevice->supported_features.extendedDynamicState3ColorBlendEnable;
         features->extendedDynamicState3ColorBlendEquation = pdevice->supported_features.extendedDynamicState3ColorBlendEquation;
         features->extendedDynamicState3ColorWriteMask = pdevice->supported_features.extendedDynamicState3ColorWriteMask;
         features->extendedDynamicState3RasterizationStream = pdevice->supported_features.extendedDynamicState3RasterizationStream;
         features->extendedDynamicState3ConservativeRasterizationMode = pdevice->supported_features.extendedDynamicState3ConservativeRasterizationMode;
         features->extendedDynamicState3ExtraPrimitiveOverestimationSize = pdevice->supported_features.extendedDynamicState3ExtraPrimitiveOverestimationSize;
         features->extendedDynamicState3DepthClipEnable = pdevice->supported_features.extendedDynamicState3DepthClipEnable;
         features->extendedDynamicState3SampleLocationsEnable = pdevice->supported_features.extendedDynamicState3SampleLocationsEnable;
         features->extendedDynamicState3ColorBlendAdvanced = pdevice->supported_features.extendedDynamicState3ColorBlendAdvanced;
         features->extendedDynamicState3ProvokingVertexMode = pdevice->supported_features.extendedDynamicState3ProvokingVertexMode;
         features->extendedDynamicState3LineRasterizationMode = pdevice->supported_features.extendedDynamicState3LineRasterizationMode;
         features->extendedDynamicState3LineStippleEnable = pdevice->supported_features.extendedDynamicState3LineStippleEnable;
         features->extendedDynamicState3DepthClipNegativeOneToOne = pdevice->supported_features.extendedDynamicState3DepthClipNegativeOneToOne;
         features->extendedDynamicState3ViewportWScalingEnable = pdevice->supported_features.extendedDynamicState3ViewportWScalingEnable;
         features->extendedDynamicState3ViewportSwizzle = pdevice->supported_features.extendedDynamicState3ViewportSwizzle;
         features->extendedDynamicState3CoverageToColorEnable = pdevice->supported_features.extendedDynamicState3CoverageToColorEnable;
         features->extendedDynamicState3CoverageToColorLocation = pdevice->supported_features.extendedDynamicState3CoverageToColorLocation;
         features->extendedDynamicState3CoverageModulationMode = pdevice->supported_features.extendedDynamicState3CoverageModulationMode;
         features->extendedDynamicState3CoverageModulationTableEnable = pdevice->supported_features.extendedDynamicState3CoverageModulationTableEnable;
         features->extendedDynamicState3CoverageModulationTable = pdevice->supported_features.extendedDynamicState3CoverageModulationTable;
         features->extendedDynamicState3CoverageReductionMode = pdevice->supported_features.extendedDynamicState3CoverageReductionMode;
         features->extendedDynamicState3RepresentativeFragmentTestEnable = pdevice->supported_features.extendedDynamicState3RepresentativeFragmentTestEnable;
         features->extendedDynamicState3ShadingRateImageEnable = pdevice->supported_features.extendedDynamicState3ShadingRateImageEnable;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR: {
         VkPhysicalDeviceExtendedFlagsFeaturesKHR *features = (void *) ext;
         features->extendedFlags = pdevice->supported_features.extendedFlags;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV: {
         VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV *features = (void *) ext;
         features->partitionedAccelerationStructure = pdevice->supported_features.partitionedAccelerationStructure;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV: {
         VkPhysicalDeviceDiagnosticsConfigFeaturesNV *features = (void *) ext;
         features->diagnosticsConfig = pdevice->supported_features.diagnosticsConfig;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES: {
         VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures *features = (void *) ext;
         features->shaderZeroInitializeWorkgroupMemory = pdevice->supported_features.shaderZeroInitializeWorkgroupMemory;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR: {
         VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *features = (void *) ext;
         features->shaderSubgroupUniformControlFlow = pdevice->supported_features.shaderSubgroupUniformControlFlow;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR: {
         VkPhysicalDeviceRobustness2FeaturesKHR *features = (void *) ext;
         features->robustBufferAccess2 = pdevice->supported_features.robustBufferAccess2;
         features->robustImageAccess2 = pdevice->supported_features.robustImageAccess2;
         features->nullDescriptor = pdevice->supported_features.nullDescriptor;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES: {
         VkPhysicalDeviceImageRobustnessFeatures *features = (void *) ext;
         features->robustImageAccess = pdevice->supported_features.robustImageAccess;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR: {
         VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *features = (void *) ext;
         features->workgroupMemoryExplicitLayout = pdevice->supported_features.workgroupMemoryExplicitLayout;
         features->workgroupMemoryExplicitLayoutScalarBlockLayout = pdevice->supported_features.workgroupMemoryExplicitLayoutScalarBlockLayout;
         features->workgroupMemoryExplicitLayout8BitAccess = pdevice->supported_features.workgroupMemoryExplicitLayout8BitAccess;
         features->workgroupMemoryExplicitLayout16BitAccess = pdevice->supported_features.workgroupMemoryExplicitLayout16BitAccess;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT: {
         VkPhysicalDevice4444FormatsFeaturesEXT *features = (void *) ext;
         features->formatA4R4G4B4 = pdevice->supported_features.formatA4R4G4B4;
         features->formatA4B4G4R4 = pdevice->supported_features.formatA4B4G4R4;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI: {
         VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *features = (void *) ext;
         features->subpassShading = pdevice->supported_features.subpassShading;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI: {
         VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI *features = (void *) ext;
         features->clustercullingShader = pdevice->supported_features.clustercullingShader;
         features->multiviewClusterCullingShader = pdevice->supported_features.multiviewClusterCullingShader;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT: {
         VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *features = (void *) ext;
         features->shaderImageInt64Atomics = pdevice->supported_features.shaderImageInt64Atomics;
         features->sparseImageInt64Atomics = pdevice->supported_features.sparseImageInt64Atomics;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR: {
         VkPhysicalDeviceFragmentShadingRateFeaturesKHR *features = (void *) ext;
         features->pipelineFragmentShadingRate = pdevice->supported_features.pipelineFragmentShadingRate;
         features->primitiveFragmentShadingRate = pdevice->supported_features.primitiveFragmentShadingRate;
         features->attachmentFragmentShadingRate = pdevice->supported_features.attachmentFragmentShadingRate;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES: {
         VkPhysicalDeviceShaderTerminateInvocationFeatures *features = (void *) ext;
         features->shaderTerminateInvocation = pdevice->supported_features.shaderTerminateInvocation;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV: {
         VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *features = (void *) ext;
         features->fragmentShadingRateEnums = pdevice->supported_features.fragmentShadingRateEnums;
         features->supersampleFragmentShadingRates = pdevice->supported_features.supersampleFragmentShadingRates;
         features->noInvocationFragmentShadingRates = pdevice->supported_features.noInvocationFragmentShadingRates;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT: {
         VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *features = (void *) ext;
         features->image2DViewOf3D = pdevice->supported_features.image2DViewOf3D;
         features->sampler2DViewOf3D = pdevice->supported_features.sampler2DViewOf3D;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT: {
         VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT *features = (void *) ext;
         features->imageSlicedViewOf3D = pdevice->supported_features.imageSlicedViewOf3D;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT: {
         VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT *features = (void *) ext;
         features->attachmentFeedbackLoopDynamicState = pdevice->supported_features.attachmentFeedbackLoopDynamicState;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT: {
         VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT *features = (void *) ext;
         features->legacyVertexAttributes = pdevice->supported_features.legacyVertexAttributes;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT: {
         VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT *features = (void *) ext;
         features->mutableDescriptorType = pdevice->supported_features.mutableDescriptorType;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT: {
         VkPhysicalDeviceDepthClipControlFeaturesEXT *features = (void *) ext;
         features->depthClipControl = pdevice->supported_features.depthClipControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT: {
         VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT *features = (void *) ext;
         features->zeroInitializeDeviceMemory = pdevice->supported_features.zeroInitializeDeviceMemory;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT: {
         VkPhysicalDeviceCustomResolveFeaturesEXT *features = (void *) ext;
         features->customResolve = pdevice->supported_features.customResolve;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT: {
         VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT *features = (void *) ext;
         features->deviceGeneratedCommands = pdevice->supported_features.deviceGeneratedCommands;
         features->dynamicGeneratedPipelineLayout = pdevice->supported_features.dynamicGeneratedPipelineLayout;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT: {
         VkPhysicalDeviceDepthClampControlFeaturesEXT *features = (void *) ext;
         features->depthClampControl = pdevice->supported_features.depthClampControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT: {
         VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *features = (void *) ext;
         features->vertexInputDynamicState = pdevice->supported_features.vertexInputDynamicState;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV: {
         VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *features = (void *) ext;
         features->externalMemoryRDMA = pdevice->supported_features.externalMemoryRDMA;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR: {
         VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR *features = (void *) ext;
         features->shaderRelaxedExtendedInstruction = pdevice->supported_features.shaderRelaxedExtendedInstruction;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT: {
         VkPhysicalDeviceColorWriteEnableFeaturesEXT *features = (void *) ext;
         features->colorWriteEnable = pdevice->supported_features.colorWriteEnable;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES: {
         VkPhysicalDeviceSynchronization2Features *features = (void *) ext;
         features->synchronization2 = pdevice->supported_features.synchronization2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR: {
         VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR *features = (void *) ext;
         features->unifiedImageLayouts = pdevice->supported_features.unifiedImageLayouts;
         features->unifiedImageLayoutsVideo = pdevice->supported_features.unifiedImageLayoutsVideo;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES: {
         VkPhysicalDeviceHostImageCopyFeatures *features = (void *) ext;
         features->hostImageCopy = pdevice->supported_features.hostImageCopy;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT: {
         VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *features = (void *) ext;
         features->primitivesGeneratedQuery = pdevice->supported_features.primitivesGeneratedQuery;
         features->primitivesGeneratedQueryWithRasterizerDiscard = pdevice->supported_features.primitivesGeneratedQueryWithRasterizerDiscard;
         features->primitivesGeneratedQueryWithNonZeroStreams = pdevice->supported_features.primitivesGeneratedQueryWithNonZeroStreams;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT: {
         VkPhysicalDeviceLegacyDitheringFeaturesEXT *features = (void *) ext;
         features->legacyDithering = pdevice->supported_features.legacyDithering;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT: {
         VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *features = (void *) ext;
         features->multisampledRenderToSingleSampled = pdevice->supported_features.multisampledRenderToSingleSampled;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT: {
         VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT *features = (void *) ext;
         features->multisampledRenderToSwapchain = pdevice->supported_features.multisampledRenderToSwapchain;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES: {
         VkPhysicalDevicePipelineProtectedAccessFeatures *features = (void *) ext;
         features->pipelineProtectedAccess = pdevice->supported_features.pipelineProtectedAccess;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR: {
         VkPhysicalDeviceVideoMaintenance1FeaturesKHR *features = (void *) ext;
         features->videoMaintenance1 = pdevice->supported_features.videoMaintenance1;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR: {
         VkPhysicalDeviceVideoMaintenance2FeaturesKHR *features = (void *) ext;
         features->videoMaintenance2 = pdevice->supported_features.videoMaintenance2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR: {
         VkPhysicalDeviceVideoDecodeVP9FeaturesKHR *features = (void *) ext;
         features->videoDecodeVP9 = pdevice->supported_features.videoDecodeVP9;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR: {
         VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR *features = (void *) ext;
         features->videoEncodeQuantizationMap = pdevice->supported_features.videoEncodeQuantizationMap;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR: {
         VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR *features = (void *) ext;
         features->videoEncodeFeedback2 = pdevice->supported_features.videoEncodeFeedback2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR: {
         VkPhysicalDeviceVideoEncodeAV1FeaturesKHR *features = (void *) ext;
         features->videoEncodeAV1 = pdevice->supported_features.videoEncodeAV1;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV: {
         VkPhysicalDeviceInheritedViewportScissorFeaturesNV *features = (void *) ext;
         features->inheritedViewportScissor2D = pdevice->supported_features.inheritedViewportScissor2D;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT: {
         VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *features = (void *) ext;
         features->ycbcr2plane444Formats = pdevice->supported_features.ycbcr2plane444Formats;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT: {
         VkPhysicalDeviceProvokingVertexFeaturesEXT *features = (void *) ext;
         features->provokingVertexLast = pdevice->supported_features.provokingVertexLast;
         features->transformFeedbackPreservesProvokingVertex = pdevice->supported_features.transformFeedbackPreservesProvokingVertex;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR: {
         VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR *features = (void *) ext;
         features->videoEncodeIntraRefresh = pdevice->supported_features.videoEncodeIntraRefresh;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT: {
         VkPhysicalDeviceDescriptorBufferFeaturesEXT *features = (void *) ext;
         features->descriptorBuffer = pdevice->supported_features.descriptorBuffer;
         features->descriptorBufferCaptureReplay = pdevice->supported_features.descriptorBufferCaptureReplay;
         features->descriptorBufferImageLayoutIgnored = pdevice->supported_features.descriptorBufferImageLayoutIgnored;
         features->descriptorBufferPushDescriptors = pdevice->supported_features.descriptorBufferPushDescriptors;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES: {
         VkPhysicalDeviceShaderIntegerDotProductFeatures *features = (void *) ext;
         features->shaderIntegerDotProduct = pdevice->supported_features.shaderIntegerDotProduct;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR: {
         VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR *features = (void *) ext;
         features->fragmentShaderBarycentric = pdevice->supported_features.fragmentShaderBarycentric;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR: {
         VkPhysicalDeviceShaderFmaFeaturesKHR *features = (void *) ext;
         features->shaderFmaFloat16 = pdevice->supported_features.shaderFmaFloat16;
         features->shaderFmaFloat32 = pdevice->supported_features.shaderFmaFloat32;
         features->shaderFmaFloat64 = pdevice->supported_features.shaderFmaFloat64;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV: {
         VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *features = (void *) ext;
         features->rayTracingMotionBlur = pdevice->supported_features.rayTracingMotionBlur;
         features->rayTracingMotionBlurPipelineTraceRaysIndirect = pdevice->supported_features.rayTracingMotionBlurPipelineTraceRaysIndirect;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV: {
         VkPhysicalDeviceRayTracingValidationFeaturesNV *features = (void *) ext;
         features->rayTracingValidation = pdevice->supported_features.rayTracingValidation;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV: {
         VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV *features = (void *) ext;
         features->spheres = pdevice->supported_features.spheres;
         features->linearSweptSpheres = pdevice->supported_features.linearSweptSpheres;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT: {
         VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *features = (void *) ext;
         features->formatRgba10x6WithoutYCbCrSampler = pdevice->supported_features.formatRgba10x6WithoutYCbCrSampler;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES: {
         VkPhysicalDeviceDynamicRenderingFeatures *features = (void *) ext;
         features->dynamicRendering = pdevice->supported_features.dynamicRendering;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT: {
         VkPhysicalDeviceImageViewMinLodFeaturesEXT *features = (void *) ext;
         features->minLod = pdevice->supported_features.minLod;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT: {
         VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT *features = (void *) ext;
         features->rasterizationOrderColorAttachmentAccess = pdevice->supported_features.rasterizationOrderColorAttachmentAccess;
         features->rasterizationOrderDepthAttachmentAccess = pdevice->supported_features.rasterizationOrderDepthAttachmentAccess;
         features->rasterizationOrderStencilAttachmentAccess = pdevice->supported_features.rasterizationOrderStencilAttachmentAccess;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV: {
         VkPhysicalDeviceLinearColorAttachmentFeaturesNV *features = (void *) ext;
         features->linearColorAttachment = pdevice->supported_features.linearColorAttachment;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT: {
         VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *features = (void *) ext;
         features->graphicsPipelineLibrary = pdevice->supported_features.graphicsPipelineLibrary;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR: {
         VkPhysicalDevicePipelineBinaryFeaturesKHR *features = (void *) ext;
         features->pipelineBinaries = pdevice->supported_features.pipelineBinaries;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM: {
         VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM *features = (void *) ext;
         features->dataGraphNeuralAcceleratorStatistics = pdevice->supported_features.dataGraphNeuralAcceleratorStatistics;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE: {
         VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *features = (void *) ext;
         features->descriptorSetHostMapping = pdevice->supported_features.descriptorSetHostMapping;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT: {
         VkPhysicalDeviceNestedCommandBufferFeaturesEXT *features = (void *) ext;
         features->nestedCommandBuffer = pdevice->supported_features.nestedCommandBuffer;
         features->nestedCommandBufferRendering = pdevice->supported_features.nestedCommandBufferRendering;
         features->nestedCommandBufferSimultaneousUse = pdevice->supported_features.nestedCommandBufferSimultaneousUse;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT: {
         VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *features = (void *) ext;
         features->shaderModuleIdentifier = pdevice->supported_features.shaderModuleIdentifier;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT: {
         VkPhysicalDeviceImageCompressionControlFeaturesEXT *features = (void *) ext;
         features->imageCompressionControl = pdevice->supported_features.imageCompressionControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT: {
         VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *features = (void *) ext;
         features->imageCompressionControlSwapchain = pdevice->supported_features.imageCompressionControlSwapchain;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT: {
         VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *features = (void *) ext;
         features->subpassMergeFeedback = pdevice->supported_features.subpassMergeFeedback;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR: {
         VkPhysicalDeviceOpacityMicromapFeaturesKHR *features = (void *) ext;
         features->micromap = pdevice->supported_features.micromap;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT: {
         VkPhysicalDeviceOpacityMicromapFeaturesEXT *features = (void *) ext;
         features->micromap = pdevice->supported_features.micromapEXT;
         features->micromapCaptureReplay = pdevice->supported_features.micromapCaptureReplay;
         features->micromapHostCommands = pdevice->supported_features.micromapHostCommands;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT: {
         VkPhysicalDevicePipelinePropertiesFeaturesEXT *features = (void *) ext;
         features->pipelinePropertiesIdentifier = pdevice->supported_features.pipelinePropertiesIdentifier;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD: {
         VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *features = (void *) ext;
         features->shaderEarlyAndLateFragmentTests = pdevice->supported_features.shaderEarlyAndLateFragmentTests;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT: {
         VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *features = (void *) ext;
         features->nonSeamlessCubeMap = pdevice->supported_features.nonSeamlessCubeMap;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES: {
         VkPhysicalDevicePipelineRobustnessFeatures *features = (void *) ext;
         features->pipelineRobustness = pdevice->supported_features.pipelineRobustness;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM: {
         VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM *features = (void *) ext;
         features->shaderMultipleWaitQueues = pdevice->supported_features.shaderMultipleWaitQueues;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM: {
         VkPhysicalDeviceImageProcessingFeaturesQCOM *features = (void *) ext;
         features->textureSampleWeighted = pdevice->supported_features.textureSampleWeighted;
         features->textureBoxFilter = pdevice->supported_features.textureBoxFilter;
         features->textureBlockMatch = pdevice->supported_features.textureBlockMatch;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM: {
         VkPhysicalDeviceTilePropertiesFeaturesQCOM *features = (void *) ext;
         features->tileProperties = pdevice->supported_features.tileProperties;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC: {
         VkPhysicalDeviceAmigoProfilingFeaturesSEC *features = (void *) ext;
         features->amigoProfiling = pdevice->supported_features.amigoProfiling;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT: {
         VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *features = (void *) ext;
         features->attachmentFeedbackLoopLayout = pdevice->supported_features.attachmentFeedbackLoopLayout;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT: {
         VkPhysicalDeviceAddressBindingReportFeaturesEXT *features = (void *) ext;
         features->reportAddressBinding = pdevice->supported_features.reportAddressBinding;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV: {
         VkPhysicalDeviceOpticalFlowFeaturesNV *features = (void *) ext;
         features->opticalFlow = pdevice->supported_features.opticalFlow;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT: {
         VkPhysicalDeviceFaultFeaturesEXT *features = (void *) ext;
         features->deviceFault = pdevice->supported_features.deviceFaultEXT;
         features->deviceFaultVendorBinary = pdevice->supported_features.deviceFaultVendorBinaryEXT;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR: {
         VkPhysicalDeviceFaultFeaturesKHR *features = (void *) ext;
         features->deviceFault = pdevice->supported_features.deviceFault;
         features->deviceFaultVendorBinary = pdevice->supported_features.deviceFaultVendorBinary;
         features->deviceFaultReportMasked = pdevice->supported_features.deviceFaultReportMasked;
         features->deviceFaultDeviceLostOnMasked = pdevice->supported_features.deviceFaultDeviceLostOnMasked;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT: {
         VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT *features = (void *) ext;
         features->pipelineLibraryGroupHandles = pdevice->supported_features.pipelineLibraryGroupHandles;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM: {
         VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM *features = (void *) ext;
         features->shaderCoreBuiltins = pdevice->supported_features.shaderCoreBuiltins;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT: {
         VkPhysicalDeviceFrameBoundaryFeaturesEXT *features = (void *) ext;
         features->frameBoundary = pdevice->supported_features.frameBoundary;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT: {
         VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT *features = (void *) ext;
         features->dynamicRenderingUnusedAttachments = pdevice->supported_features.dynamicRenderingUnusedAttachments;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR: {
         VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR *features = (void *) ext;
         features->internallySynchronizedQueues = pdevice->supported_features.internallySynchronizedQueues;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR: {
         VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR *features = (void *) ext;
         features->swapchainMaintenance1 = pdevice->supported_features.swapchainMaintenance1;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT: {
         VkPhysicalDeviceDepthBiasControlFeaturesEXT *features = (void *) ext;
         features->depthBiasControl = pdevice->supported_features.depthBiasControl;
         features->leastRepresentableValueForceUnormRepresentation = pdevice->supported_features.leastRepresentableValueForceUnormRepresentation;
         features->floatRepresentation = pdevice->supported_features.floatRepresentation;
         features->depthBiasExact = pdevice->supported_features.depthBiasExact;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT: {
         VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT *features = (void *) ext;
         features->rayTracingInvocationReorder = pdevice->supported_features.rayTracingInvocationReorder;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV: {
         VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV *features = (void *) ext;
         features->rayTracingInvocationReorder = pdevice->supported_features.rayTracingInvocationReorderNV;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV: {
         VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV *features = (void *) ext;
         features->extendedSparseAddressSpace = pdevice->supported_features.extendedSparseAddressSpace;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM: {
         VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM *features = (void *) ext;
         features->multiviewPerViewViewports = pdevice->supported_features.multiviewPerViewViewports;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR: {
         VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR *features = (void *) ext;
         features->rayTracingPositionFetch = pdevice->supported_features.rayTracingPositionFetch;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM: {
         VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM *features = (void *) ext;
         features->multiviewPerViewRenderAreas = pdevice->supported_features.multiviewPerViewRenderAreas;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT: {
         VkPhysicalDeviceShaderObjectFeaturesEXT *features = (void *) ext;
         features->shaderObject = pdevice->supported_features.shaderObject;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT: {
         VkPhysicalDeviceShaderTileImageFeaturesEXT *features = (void *) ext;
         features->shaderTileImageColorReadAccess = pdevice->supported_features.shaderTileImageColorReadAccess;
         features->shaderTileImageDepthReadAccess = pdevice->supported_features.shaderTileImageDepthReadAccess;
         features->shaderTileImageStencilReadAccess = pdevice->supported_features.shaderTileImageStencilReadAccess;
         break;
      }

#ifdef VK_USE_PLATFORM_SCREEN_QNX
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX: {
         VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX *features = (void *) ext;
         features->screenBufferImport = pdevice->supported_features.screenBufferImport;
         break;
      }

#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR: {
         VkPhysicalDeviceCooperativeMatrixFeaturesKHR *features = (void *) ext;
         features->cooperativeMatrix = pdevice->supported_features.cooperativeMatrix;
         features->cooperativeMatrixRobustBufferAccess = pdevice->supported_features.cooperativeMatrixRobustBufferAccess;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM: {
         VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM *features = (void *) ext;
         features->cooperativeMatrixConversion = pdevice->supported_features.cooperativeMatrixConversion;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD: {
         VkPhysicalDeviceAntiLagFeaturesAMD *features = (void *) ext;
         features->antiLag = pdevice->supported_features.antiLag;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM: {
         VkPhysicalDeviceTileMemoryHeapFeaturesQCOM *features = (void *) ext;
         features->tileMemoryHeap = pdevice->supported_features.tileMemoryHeap;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM: {
         VkPhysicalDeviceCubicClampFeaturesQCOM *features = (void *) ext;
         features->cubicRangeClamp = pdevice->supported_features.cubicRangeClamp;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM: {
         VkPhysicalDeviceYcbcrDegammaFeaturesQCOM *features = (void *) ext;
         features->ycbcrDegamma = pdevice->supported_features.ycbcrDegamma;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM: {
         VkPhysicalDeviceCubicWeightsFeaturesQCOM *features = (void *) ext;
         features->selectableCubicWeights = pdevice->supported_features.selectableCubicWeights;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM: {
         VkPhysicalDeviceImageProcessing2FeaturesQCOM *features = (void *) ext;
         features->textureBlockMatch2 = pdevice->supported_features.textureBlockMatch2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM: {
         VkPhysicalDeviceImageProcessing3FeaturesQCOM *features = (void *) ext;
         features->imageGatherLinear = pdevice->supported_features.imageGatherLinear;
         features->imageGatherExtendedModes = pdevice->supported_features.imageGatherExtendedModes;
         features->blockMatchExtendedClampToEdge = pdevice->supported_features.blockMatchExtendedClampToEdge;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV: {
         VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV *features = (void *) ext;
         features->descriptorPoolOverallocation = pdevice->supported_features.descriptorPoolOverallocation;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV: {
         VkPhysicalDevicePerStageDescriptorSetFeaturesNV *features = (void *) ext;
         features->perStageDescriptorSet = pdevice->supported_features.perStageDescriptorSet;
         features->dynamicPipelineLayout = pdevice->supported_features.dynamicPipelineLayout;
         break;
      }

#ifdef VK_USE_PLATFORM_ANDROID_KHR
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID: {
         VkPhysicalDeviceExternalFormatResolveFeaturesANDROID *features = (void *) ext;
         features->externalFormatResolve = pdevice->supported_features.externalFormatResolve;
         break;
      }

#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM: {
         VkPhysicalDeviceSchedulingControlsFeaturesARM *features = (void *) ext;
         features->schedulingControls = pdevice->supported_features.schedulingControls;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG: {
         VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG *features = (void *) ext;
         features->relaxedLineRasterization = pdevice->supported_features.relaxedLineRasterization;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM: {
         VkPhysicalDeviceRenderPassStripedFeaturesARM *features = (void *) ext;
         features->renderPassStriped = pdevice->supported_features.renderPassStriped;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM: {
         VkPhysicalDevicePipelineOpacityMicromapFeaturesARM *features = (void *) ext;
         features->pipelineOpacityMicromap = pdevice->supported_features.pipelineOpacityMicromap;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR: {
         VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR *features = (void *) ext;
         features->shaderMaximalReconvergence = pdevice->supported_features.shaderMaximalReconvergence;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES: {
         VkPhysicalDeviceShaderSubgroupRotateFeatures *features = (void *) ext;
         features->shaderSubgroupRotate = pdevice->supported_features.shaderSubgroupRotate;
         features->shaderSubgroupRotateClustered = pdevice->supported_features.shaderSubgroupRotateClustered;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES: {
         VkPhysicalDeviceShaderExpectAssumeFeatures *features = (void *) ext;
         features->shaderExpectAssume = pdevice->supported_features.shaderExpectAssume;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES: {
         VkPhysicalDeviceShaderFloatControls2Features *features = (void *) ext;
         features->shaderFloatControls2 = pdevice->supported_features.shaderFloatControls2;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES: {
         VkPhysicalDeviceDynamicRenderingLocalReadFeatures *features = (void *) ext;
         features->dynamicRenderingLocalRead = pdevice->supported_features.dynamicRenderingLocalRead;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR: {
         VkPhysicalDeviceShaderQuadControlFeaturesKHR *features = (void *) ext;
         features->shaderQuadControl = pdevice->supported_features.shaderQuadControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV: {
         VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV *features = (void *) ext;
         features->shaderFloat16VectorAtomics = pdevice->supported_features.shaderFloat16VectorAtomics;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT: {
         VkPhysicalDeviceMapMemoryPlacedFeaturesEXT *features = (void *) ext;
         features->memoryMapPlaced = pdevice->supported_features.memoryMapPlaced;
         features->memoryMapRangePlaced = pdevice->supported_features.memoryMapRangePlaced;
         features->memoryUnmapReserve = pdevice->supported_features.memoryUnmapReserve;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR: {
         VkPhysicalDeviceShaderBfloat16FeaturesKHR *features = (void *) ext;
         features->shaderBFloat16Type = pdevice->supported_features.shaderBFloat16Type;
         features->shaderBFloat16DotProduct = pdevice->supported_features.shaderBFloat16DotProduct;
         features->shaderBFloat16CooperativeMatrix = pdevice->supported_features.shaderBFloat16CooperativeMatrix;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV: {
         VkPhysicalDeviceRawAccessChainsFeaturesNV *features = (void *) ext;
         features->shaderRawAccessChains = pdevice->supported_features.shaderRawAccessChains;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV: {
         VkPhysicalDeviceCommandBufferInheritanceFeaturesNV *features = (void *) ext;
         features->commandBufferInheritance = pdevice->supported_features.commandBufferInheritance;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA: {
         VkPhysicalDeviceImageAlignmentControlFeaturesMESA *features = (void *) ext;
         features->imageAlignmentControl = pdevice->supported_features.imageAlignmentControl;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT: {
         VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT *features = (void *) ext;
         features->shaderReplicatedComposites = pdevice->supported_features.shaderReplicatedComposites;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR: {
         VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR *features = (void *) ext;
         features->presentModeFifoLatestReady = pdevice->supported_features.presentModeFifoLatestReady;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV: {
         VkPhysicalDeviceCooperativeMatrix2FeaturesNV *features = (void *) ext;
         features->cooperativeMatrixWorkgroupScope = pdevice->supported_features.cooperativeMatrixWorkgroupScope;
         features->cooperativeMatrixFlexibleDimensions = pdevice->supported_features.cooperativeMatrixFlexibleDimensions;
         features->cooperativeMatrixReductions = pdevice->supported_features.cooperativeMatrixReductions;
         features->cooperativeMatrixConversions = pdevice->supported_features.cooperativeMatrixConversions;
         features->cooperativeMatrixPerElementOperations = pdevice->supported_features.cooperativeMatrixPerElementOperations;
         features->cooperativeMatrixTensorAddressing = pdevice->supported_features.cooperativeMatrixTensorAddressing;
         features->cooperativeMatrixBlockLoads = pdevice->supported_features.cooperativeMatrixBlockLoads;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV: {
         VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV *features = (void *) ext;
         features->cooperativeMatrixDecodeVector = pdevice->supported_features.cooperativeMatrixDecodeVector;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI: {
         VkPhysicalDeviceHdrVividFeaturesHUAWEI *features = (void *) ext;
         features->hdrVivid = pdevice->supported_features.hdrVivid;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT: {
         VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT *features = (void *) ext;
         features->vertexAttributeRobustness = pdevice->supported_features.vertexAttributeRobustness;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR: {
         VkPhysicalDeviceDepthClampZeroOneFeaturesKHR *features = (void *) ext;
         features->depthClampZeroOne = pdevice->supported_features.depthClampZeroOne;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV: {
         VkPhysicalDeviceCooperativeVectorFeaturesNV *features = (void *) ext;
         features->cooperativeVector = pdevice->supported_features.cooperativeVector;
         features->cooperativeVectorTraining = pdevice->supported_features.cooperativeVectorTraining;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM: {
         VkPhysicalDeviceTileShadingFeaturesQCOM *features = (void *) ext;
         features->tileShading = pdevice->supported_features.tileShading;
         features->tileShadingFragmentStage = pdevice->supported_features.tileShadingFragmentStage;
         features->tileShadingColorAttachments = pdevice->supported_features.tileShadingColorAttachments;
         features->tileShadingDepthAttachments = pdevice->supported_features.tileShadingDepthAttachments;
         features->tileShadingStencilAttachments = pdevice->supported_features.tileShadingStencilAttachments;
         features->tileShadingInputAttachments = pdevice->supported_features.tileShadingInputAttachments;
         features->tileShadingSampledAttachments = pdevice->supported_features.tileShadingSampledAttachments;
         features->tileShadingPerTileDraw = pdevice->supported_features.tileShadingPerTileDraw;
         features->tileShadingPerTileDispatch = pdevice->supported_features.tileShadingPerTileDispatch;
         features->tileShadingDispatchTile = pdevice->supported_features.tileShadingDispatchTile;
         features->tileShadingApron = pdevice->supported_features.tileShadingApron;
         features->tileShadingAnisotropicApron = pdevice->supported_features.tileShadingAnisotropicApron;
         features->tileShadingAtomicOps = pdevice->supported_features.tileShadingAtomicOps;
         features->tileShadingImageProcessing = pdevice->supported_features.tileShadingImageProcessing;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE: {
         VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE *features = (void *) ext;
         features->fragmentDensityMapLayered = pdevice->supported_features.fragmentDensityMapLayered;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV: {
         VkPhysicalDevicePresentMeteringFeaturesNV *features = (void *) ext;
         features->presentMetering = pdevice->supported_features.presentMetering;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT: {
         VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT *features = (void *) ext;
         features->shaderUniformBufferUnsizedArray = pdevice->supported_features.shaderUniformBufferUnsizedArray;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE: {
         VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE *features = (void *) ext;
         features->shaderMixedFloatDotProductFloat16AccFloat32 = pdevice->supported_features.shaderMixedFloatDotProductFloat16AccFloat32;
         features->shaderMixedFloatDotProductFloat16AccFloat16 = pdevice->supported_features.shaderMixedFloatDotProductFloat16AccFloat16;
         features->shaderMixedFloatDotProductBFloat16Acc = pdevice->supported_features.shaderMixedFloatDotProductBFloat16Acc;
         features->shaderMixedFloatDotProductFloat8AccFloat32 = pdevice->supported_features.shaderMixedFloatDotProductFloat8AccFloat32;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT: {
         VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT *features = (void *) ext;
         features->primitiveRestartIndex = pdevice->supported_features.primitiveRestartIndex;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM: {
         VkPhysicalDeviceFormatPackFeaturesARM *features = (void *) ext;
         features->formatPack = pdevice->supported_features.formatPack;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC: {
         VkPhysicalDeviceThrottleHintFeaturesSEC *features = (void *) ext;
         features->throttleHint = pdevice->supported_features.throttleHint;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM: {
         VkPhysicalDeviceTensorFeaturesARM *features = (void *) ext;
         features->tensorNonPacked = pdevice->supported_features.tensorNonPacked;
         features->shaderTensorAccess = pdevice->supported_features.shaderTensorAccess;
         features->shaderStorageTensorArrayDynamicIndexing = pdevice->supported_features.shaderStorageTensorArrayDynamicIndexing;
         features->shaderStorageTensorArrayNonUniformIndexing = pdevice->supported_features.shaderStorageTensorArrayNonUniformIndexing;
         features->descriptorBindingStorageTensorUpdateAfterBind = pdevice->supported_features.descriptorBindingStorageTensorUpdateAfterBind;
         features->tensors = pdevice->supported_features.tensors;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM: {
         VkPhysicalDeviceDescriptorBufferTensorFeaturesARM *features = (void *) ext;
         features->descriptorBufferTensorDescriptors = pdevice->supported_features.descriptorBufferTensorDescriptors;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT: {
         VkPhysicalDeviceShaderFloat8FeaturesEXT *features = (void *) ext;
         features->shaderFloat8 = pdevice->supported_features.shaderFloat8;
         features->shaderFloat8CooperativeMatrix = pdevice->supported_features.shaderFloat8CooperativeMatrix;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM: {
         VkPhysicalDeviceDataGraphFeaturesARM *features = (void *) ext;
         features->dataGraph = pdevice->supported_features.dataGraph;
         features->dataGraphUpdateAfterBind = pdevice->supported_features.dataGraphUpdateAfterBind;
         features->dataGraphSpecializationConstants = pdevice->supported_features.dataGraphSpecializationConstants;
         features->dataGraphDescriptorBuffer = pdevice->supported_features.dataGraphDescriptorBuffer;
         features->dataGraphShaderModule = pdevice->supported_features.dataGraphShaderModule;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC: {
         VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC *features = (void *) ext;
         features->pipelineCacheIncrementalMode = pdevice->supported_features.pipelineCacheIncrementalMode;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM: {
         VkPhysicalDeviceDataGraphModelFeaturesQCOM *features = (void *) ext;
         features->dataGraphModel = pdevice->supported_features.dataGraphModel;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR: {
         VkPhysicalDeviceShaderUntypedPointersFeaturesKHR *features = (void *) ext;
         features->shaderUntypedPointers = pdevice->supported_features.shaderUntypedPointers;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE: {
         VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE *features = (void *) ext;
         features->videoEncodeRgbConversion = pdevice->supported_features.videoEncodeRgbConversion;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT: {
         VkPhysicalDeviceShader64BitIndexingFeaturesEXT *features = (void *) ext;
         features->shader64BitIndexing = pdevice->supported_features.shader64BitIndexing;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM: {
         VkPhysicalDeviceQueuePerfHintFeaturesQCOM *features = (void *) ext;
         features->queuePerfHint = pdevice->supported_features.queuePerfHint;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM: {
         VkPhysicalDevicePerformanceCountersByRegionFeaturesARM *features = (void *) ext;
         features->performanceCountersByRegion = pdevice->supported_features.performanceCountersByRegion;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV: {
         VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV *features = (void *) ext;
         features->computeOccupancyPriority = pdevice->supported_features.computeOccupancyPriority;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT: {
         VkPhysicalDeviceShaderLongVectorFeaturesEXT *features = (void *) ext;
         features->longVector = pdevice->supported_features.longVector;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT: {
         VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT *features = (void *) ext;
         features->textureCompressionASTC_3D = pdevice->supported_features.textureCompressionASTC_3D;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT: {
         VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT *features = (void *) ext;
         features->shaderSubgroupPartitioned = pdevice->supported_features.shaderSubgroupPartitioned;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT: {
         VkPhysicalDeviceShaderSplitBarrierFeaturesEXT *features = (void *) ext;
         features->shaderSplitBarrier = pdevice->supported_features.shaderSplitBarrier;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT: {
         VkPhysicalDeviceDescriptorHeapFeaturesEXT *features = (void *) ext;
         features->descriptorHeap = pdevice->supported_features.descriptorHeap;
         features->descriptorHeapCaptureReplay = pdevice->supported_features.descriptorHeapCaptureReplay;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM: {
         VkPhysicalDeviceShaderInstrumentationFeaturesARM *features = (void *) ext;
         features->shaderInstrumentation = pdevice->supported_features.shaderInstrumentation;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR: {
         VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR *features = (void *) ext;
         features->deviceAddressCommands = pdevice->supported_features.deviceAddressCommands;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR: {
         VkPhysicalDeviceShaderConstantDataFeaturesKHR *features = (void *) ext;
         features->shaderConstantData = pdevice->supported_features.shaderConstantData;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR: {
         VkPhysicalDeviceShaderAbortFeaturesKHR *features = (void *) ext;
         features->shaderAbort = pdevice->supported_features.shaderAbort;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM: {
         VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM *features = (void *) ext;
         features->dataGraphOpticalFlow = pdevice->supported_features.dataGraphOpticalFlow;
         break;
      }

      default:
         break;
      }
   }
}

void
vk_set_physical_device_features(struct vk_features *all_features,
                                const VkPhysicalDeviceFeatures2 *pFeatures)
{
   vk_foreach_struct_const(ext, pFeatures) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
         const VkPhysicalDeviceFeatures2 *features = (const void *) ext;
         vk_set_physical_device_features_1_0(all_features, &features->features);
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV: {
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *features = (const void *) ext;
         if (features->deviceGeneratedCommands)
            all_features->deviceGeneratedCommandsNV = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_CONSTANT_BANK_FEATURES_NV: {
         const VkPhysicalDevicePushConstantBankFeaturesNV *features = (const void *) ext;
         if (features->pushConstantBank)
            all_features->pushConstantBank = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_COMPUTE_FEATURES_NV: {
         const VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV *features = (const void *) ext;
         if (features->deviceGeneratedCompute)
            all_features->deviceGeneratedCompute = true;
         if (features->deviceGeneratedComputePipelines)
            all_features->deviceGeneratedComputePipelines = true;
         if (features->deviceGeneratedComputeCaptureReplay)
            all_features->deviceGeneratedComputeCaptureReplay = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES: {
         const VkPhysicalDevicePrivateDataFeatures *features = (const void *) ext;
         if (features->privateData)
            all_features->privateData = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_ACCELERATION_STRUCTURE_FEATURES_NV: {
         const VkPhysicalDeviceClusterAccelerationStructureFeaturesNV *features = (const void *) ext;
         if (features->clusterAccelerationStructure)
            all_features->clusterAccelerationStructure = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
         const VkPhysicalDeviceVariablePointersFeatures *features = (const void *) ext;
         if (features->variablePointersStorageBuffer)
            all_features->variablePointersStorageBuffer = true;
         if (features->variablePointers)
            all_features->variablePointers = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
         const VkPhysicalDeviceMultiviewFeatures *features = (const void *) ext;
         if (features->multiview)
            all_features->multiview = true;
         if (features->multiviewGeometryShader)
            all_features->multiviewGeometryShader = true;
         if (features->multiviewTessellationShader)
            all_features->multiviewTessellationShader = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR: {
         const VkPhysicalDevicePresentIdFeaturesKHR *features = (const void *) ext;
         if (features->presentId)
            all_features->presentId = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_2_FEATURES_KHR: {
         const VkPhysicalDevicePresentId2FeaturesKHR *features = (const void *) ext;
         if (features->presentId2)
            all_features->presentId2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR: {
         const VkPhysicalDevicePresentWaitFeaturesKHR *features = (const void *) ext;
         if (features->presentWait)
            all_features->presentWait = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_2_FEATURES_KHR: {
         const VkPhysicalDevicePresentWait2FeaturesKHR *features = (const void *) ext;
         if (features->presentWait2)
            all_features->presentWait2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT: {
         const VkPhysicalDevicePresentTimingFeaturesEXT *features = (const void *) ext;
         if (features->presentTiming)
            all_features->presentTiming = true;
         if (features->presentAtAbsoluteTime)
            all_features->presentAtAbsoluteTime = true;
         if (features->presentAtRelativeTime)
            all_features->presentAtRelativeTime = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
         const VkPhysicalDevice16BitStorageFeatures *features = (const void *) ext;
         if (features->storageBuffer16BitAccess)
            all_features->storageBuffer16BitAccess = true;
         if (features->uniformAndStorageBuffer16BitAccess)
            all_features->uniformAndStorageBuffer16BitAccess = true;
         if (features->storagePushConstant16)
            all_features->storagePushConstant16 = true;
         if (features->storageInputOutput16)
            all_features->storageInputOutput16 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES: {
         const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *features = (const void *) ext;
         if (features->shaderSubgroupExtendedTypes)
            all_features->shaderSubgroupExtendedTypes = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES: {
         const VkPhysicalDeviceSamplerYcbcrConversionFeatures *features = (const void *) ext;
         if (features->samplerYcbcrConversion)
            all_features->samplerYcbcrConversion = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES: {
         const VkPhysicalDeviceProtectedMemoryFeatures *features = (const void *) ext;
         if (features->protectedMemory)
            all_features->protectedMemory = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT: {
         const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *features = (const void *) ext;
         if (features->advancedBlendCoherentOperations)
            all_features->advancedBlendCoherentOperations = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT: {
         const VkPhysicalDeviceMultiDrawFeaturesEXT *features = (const void *) ext;
         if (features->multiDraw)
            all_features->multiDraw = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES: {
         const VkPhysicalDeviceInlineUniformBlockFeatures *features = (const void *) ext;
         if (features->inlineUniformBlock)
            all_features->inlineUniformBlock = true;
         if (features->descriptorBindingInlineUniformBlockUpdateAfterBind)
            all_features->descriptorBindingInlineUniformBlockUpdateAfterBind = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES: {
         const VkPhysicalDeviceMaintenance4Features *features = (const void *) ext;
         if (features->maintenance4)
            all_features->maintenance4 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES: {
         const VkPhysicalDeviceMaintenance5Features *features = (const void *) ext;
         if (features->maintenance5)
            all_features->maintenance5 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES: {
         const VkPhysicalDeviceMaintenance6Features *features = (const void *) ext;
         if (features->maintenance6)
            all_features->maintenance6 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR: {
         const VkPhysicalDeviceMaintenance7FeaturesKHR *features = (const void *) ext;
         if (features->maintenance7)
            all_features->maintenance7 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_8_FEATURES_KHR: {
         const VkPhysicalDeviceMaintenance8FeaturesKHR *features = (const void *) ext;
         if (features->maintenance8)
            all_features->maintenance8 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_9_FEATURES_KHR: {
         const VkPhysicalDeviceMaintenance9FeaturesKHR *features = (const void *) ext;
         if (features->maintenance9)
            all_features->maintenance9 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_11_FEATURES_KHR: {
         const VkPhysicalDeviceMaintenance11FeaturesKHR *features = (const void *) ext;
         if (features->maintenance11)
            all_features->maintenance11 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_10_FEATURES_KHR: {
         const VkPhysicalDeviceMaintenance10FeaturesKHR *features = (const void *) ext;
         if (features->maintenance10)
            all_features->maintenance10 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
         const VkPhysicalDeviceShaderDrawParametersFeatures *features = (const void *) ext;
         if (features->shaderDrawParameters)
            all_features->shaderDrawParameters = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES: {
         const VkPhysicalDeviceShaderFloat16Int8Features *features = (const void *) ext;
         if (features->shaderFloat16)
            all_features->shaderFloat16 = true;
         if (features->shaderInt8)
            all_features->shaderInt8 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES: {
         const VkPhysicalDeviceHostQueryResetFeatures *features = (const void *) ext;
         if (features->hostQueryReset)
            all_features->hostQueryReset = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ELAPSED_TIMER_QUERY_FEATURES_QCOM: {
         const VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM *features = (const void *) ext;
         if (features->elapsedTimerQuery)
            all_features->elapsedTimerQuery = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES: {
         const VkPhysicalDeviceGlobalPriorityQueryFeatures *features = (const void *) ext;
         if (features->globalPriorityQuery)
            all_features->globalPriorityQuery = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT: {
         const VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *features = (const void *) ext;
         if (features->deviceMemoryReport)
            all_features->deviceMemoryReport = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES: {
         const VkPhysicalDeviceDescriptorIndexingFeatures *features = (const void *) ext;
         if (features->shaderInputAttachmentArrayDynamicIndexing)
            all_features->shaderInputAttachmentArrayDynamicIndexing = true;
         if (features->shaderUniformTexelBufferArrayDynamicIndexing)
            all_features->shaderUniformTexelBufferArrayDynamicIndexing = true;
         if (features->shaderStorageTexelBufferArrayDynamicIndexing)
            all_features->shaderStorageTexelBufferArrayDynamicIndexing = true;
         if (features->shaderUniformBufferArrayNonUniformIndexing)
            all_features->shaderUniformBufferArrayNonUniformIndexing = true;
         if (features->shaderSampledImageArrayNonUniformIndexing)
            all_features->shaderSampledImageArrayNonUniformIndexing = true;
         if (features->shaderStorageBufferArrayNonUniformIndexing)
            all_features->shaderStorageBufferArrayNonUniformIndexing = true;
         if (features->shaderStorageImageArrayNonUniformIndexing)
            all_features->shaderStorageImageArrayNonUniformIndexing = true;
         if (features->shaderInputAttachmentArrayNonUniformIndexing)
            all_features->shaderInputAttachmentArrayNonUniformIndexing = true;
         if (features->shaderUniformTexelBufferArrayNonUniformIndexing)
            all_features->shaderUniformTexelBufferArrayNonUniformIndexing = true;
         if (features->shaderStorageTexelBufferArrayNonUniformIndexing)
            all_features->shaderStorageTexelBufferArrayNonUniformIndexing = true;
         if (features->descriptorBindingUniformBufferUpdateAfterBind)
            all_features->descriptorBindingUniformBufferUpdateAfterBind = true;
         if (features->descriptorBindingSampledImageUpdateAfterBind)
            all_features->descriptorBindingSampledImageUpdateAfterBind = true;
         if (features->descriptorBindingStorageImageUpdateAfterBind)
            all_features->descriptorBindingStorageImageUpdateAfterBind = true;
         if (features->descriptorBindingStorageBufferUpdateAfterBind)
            all_features->descriptorBindingStorageBufferUpdateAfterBind = true;
         if (features->descriptorBindingUniformTexelBufferUpdateAfterBind)
            all_features->descriptorBindingUniformTexelBufferUpdateAfterBind = true;
         if (features->descriptorBindingStorageTexelBufferUpdateAfterBind)
            all_features->descriptorBindingStorageTexelBufferUpdateAfterBind = true;
         if (features->descriptorBindingUpdateUnusedWhilePending)
            all_features->descriptorBindingUpdateUnusedWhilePending = true;
         if (features->descriptorBindingPartiallyBound)
            all_features->descriptorBindingPartiallyBound = true;
         if (features->descriptorBindingVariableDescriptorCount)
            all_features->descriptorBindingVariableDescriptorCount = true;
         if (features->runtimeDescriptorArray)
            all_features->runtimeDescriptorArray = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES: {
         const VkPhysicalDeviceTimelineSemaphoreFeatures *features = (const void *) ext;
         if (features->timelineSemaphore)
            all_features->timelineSemaphore = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES: {
         const VkPhysicalDevice8BitStorageFeatures *features = (const void *) ext;
         if (features->storageBuffer8BitAccess)
            all_features->storageBuffer8BitAccess = true;
         if (features->uniformAndStorageBuffer8BitAccess)
            all_features->uniformAndStorageBuffer8BitAccess = true;
         if (features->storagePushConstant8)
            all_features->storagePushConstant8 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT: {
         const VkPhysicalDeviceConditionalRenderingFeaturesEXT *features = (const void *) ext;
         if (features->conditionalRendering)
            all_features->conditionalRendering = true;
         if (features->inheritedConditionalRendering)
            all_features->inheritedConditionalRendering = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES: {
         const VkPhysicalDeviceVulkanMemoryModelFeatures *features = (const void *) ext;
         if (features->vulkanMemoryModel)
            all_features->vulkanMemoryModel = true;
         if (features->vulkanMemoryModelDeviceScope)
            all_features->vulkanMemoryModelDeviceScope = true;
         if (features->vulkanMemoryModelAvailabilityVisibilityChains)
            all_features->vulkanMemoryModelAvailabilityVisibilityChains = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES: {
         const VkPhysicalDeviceShaderAtomicInt64Features *features = (const void *) ext;
         if (features->shaderBufferInt64Atomics)
            all_features->shaderBufferInt64Atomics = true;
         if (features->shaderSharedInt64Atomics)
            all_features->shaderSharedInt64Atomics = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT: {
         const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *features = (const void *) ext;
         if (features->shaderBufferFloat32Atomics)
            all_features->shaderBufferFloat32Atomics = true;
         if (features->shaderBufferFloat32AtomicAdd)
            all_features->shaderBufferFloat32AtomicAdd = true;
         if (features->shaderBufferFloat64Atomics)
            all_features->shaderBufferFloat64Atomics = true;
         if (features->shaderBufferFloat64AtomicAdd)
            all_features->shaderBufferFloat64AtomicAdd = true;
         if (features->shaderSharedFloat32Atomics)
            all_features->shaderSharedFloat32Atomics = true;
         if (features->shaderSharedFloat32AtomicAdd)
            all_features->shaderSharedFloat32AtomicAdd = true;
         if (features->shaderSharedFloat64Atomics)
            all_features->shaderSharedFloat64Atomics = true;
         if (features->shaderSharedFloat64AtomicAdd)
            all_features->shaderSharedFloat64AtomicAdd = true;
         if (features->shaderImageFloat32Atomics)
            all_features->shaderImageFloat32Atomics = true;
         if (features->shaderImageFloat32AtomicAdd)
            all_features->shaderImageFloat32AtomicAdd = true;
         if (features->sparseImageFloat32Atomics)
            all_features->sparseImageFloat32Atomics = true;
         if (features->sparseImageFloat32AtomicAdd)
            all_features->sparseImageFloat32AtomicAdd = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT: {
         const VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *features = (const void *) ext;
         if (features->shaderBufferFloat16Atomics)
            all_features->shaderBufferFloat16Atomics = true;
         if (features->shaderBufferFloat16AtomicAdd)
            all_features->shaderBufferFloat16AtomicAdd = true;
         if (features->shaderBufferFloat16AtomicMinMax)
            all_features->shaderBufferFloat16AtomicMinMax = true;
         if (features->shaderBufferFloat32AtomicMinMax)
            all_features->shaderBufferFloat32AtomicMinMax = true;
         if (features->shaderBufferFloat64AtomicMinMax)
            all_features->shaderBufferFloat64AtomicMinMax = true;
         if (features->shaderSharedFloat16Atomics)
            all_features->shaderSharedFloat16Atomics = true;
         if (features->shaderSharedFloat16AtomicAdd)
            all_features->shaderSharedFloat16AtomicAdd = true;
         if (features->shaderSharedFloat16AtomicMinMax)
            all_features->shaderSharedFloat16AtomicMinMax = true;
         if (features->shaderSharedFloat32AtomicMinMax)
            all_features->shaderSharedFloat32AtomicMinMax = true;
         if (features->shaderSharedFloat64AtomicMinMax)
            all_features->shaderSharedFloat64AtomicMinMax = true;
         if (features->shaderImageFloat32AtomicMinMax)
            all_features->shaderImageFloat32AtomicMinMax = true;
         if (features->sparseImageFloat32AtomicMinMax)
            all_features->sparseImageFloat32AtomicMinMax = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES: {
         const VkPhysicalDeviceVertexAttributeDivisorFeatures *features = (const void *) ext;
         if (features->vertexAttributeInstanceRateDivisor)
            all_features->vertexAttributeInstanceRateDivisor = true;
         if (features->vertexAttributeInstanceRateZeroDivisor)
            all_features->vertexAttributeInstanceRateZeroDivisor = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT: {
         const VkPhysicalDeviceASTCDecodeFeaturesEXT *features = (const void *) ext;
         if (features->decodeModeSharedExponent)
            all_features->decodeModeSharedExponent = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT: {
         const VkPhysicalDeviceTransformFeedbackFeaturesEXT *features = (const void *) ext;
         if (features->transformFeedback)
            all_features->transformFeedback = true;
         if (features->geometryStreams)
            all_features->geometryStreams = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV: {
         const VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *features = (const void *) ext;
         if (features->representativeFragmentTest)
            all_features->representativeFragmentTest = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV: {
         const VkPhysicalDeviceExclusiveScissorFeaturesNV *features = (const void *) ext;
         if (features->exclusiveScissor)
            all_features->exclusiveScissor = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV: {
         const VkPhysicalDeviceCornerSampledImageFeaturesNV *features = (const void *) ext;
         if (features->cornerSampledImage)
            all_features->cornerSampledImage = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_KHR: {
         const VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR *features = (const void *) ext;
         if (features->computeDerivativeGroupQuads)
            all_features->computeDerivativeGroupQuads = true;
         if (features->computeDerivativeGroupLinear)
            all_features->computeDerivativeGroupLinear = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV: {
         const VkPhysicalDeviceShaderImageFootprintFeaturesNV *features = (const void *) ext;
         if (features->imageFootprint)
            all_features->imageFootprint = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV: {
         const VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *features = (const void *) ext;
         if (features->dedicatedAllocationImageAliasing)
            all_features->dedicatedAllocationImageAliasing = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_KHR: {
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR *features = (const void *) ext;
         if (features->indirectMemoryCopy)
            all_features->indirectMemoryCopy = true;
         if (features->indirectMemoryToImageCopy)
            all_features->indirectMemoryToImageCopy = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COPY_MEMORY_INDIRECT_FEATURES_NV: {
         const VkPhysicalDeviceCopyMemoryIndirectFeaturesNV *features = (const void *) ext;
         if (features->indirectCopy)
            all_features->indirectCopy = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_DECOMPRESSION_FEATURES_EXT: {
         const VkPhysicalDeviceMemoryDecompressionFeaturesEXT *features = (const void *) ext;
         if (features->memoryDecompression)
            all_features->memoryDecompression = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV: {
         const VkPhysicalDeviceShadingRateImageFeaturesNV *features = (const void *) ext;
         if (features->shadingRateImage)
            all_features->shadingRateImage = true;
         if (features->shadingRateCoarseSampleOrder)
            all_features->shadingRateCoarseSampleOrder = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI: {
         const VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *features = (const void *) ext;
         if (features->invocationMask)
            all_features->invocationMask = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV: {
         const VkPhysicalDeviceMeshShaderFeaturesNV *features = (const void *) ext;
         if (features->taskShader)
            all_features->taskShaderNV = true;
         if (features->meshShader)
            all_features->meshShaderNV = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT: {
         const VkPhysicalDeviceMeshShaderFeaturesEXT *features = (const void *) ext;
         if (features->taskShader)
            all_features->taskShader = true;
         if (features->meshShader)
            all_features->meshShader = true;
         if (features->multiviewMeshShader)
            all_features->multiviewMeshShader = true;
         if (features->primitiveFragmentShadingRateMeshShader)
            all_features->primitiveFragmentShadingRateMeshShader = true;
         if (features->meshShaderQueries)
            all_features->meshShaderQueries = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR: {
         const VkPhysicalDeviceAccelerationStructureFeaturesKHR *features = (const void *) ext;
         if (features->accelerationStructure)
            all_features->accelerationStructure = true;
         if (features->accelerationStructureCaptureReplay)
            all_features->accelerationStructureCaptureReplay = true;
         if (features->accelerationStructureIndirectBuild)
            all_features->accelerationStructureIndirectBuild = true;
         if (features->accelerationStructureHostCommands)
            all_features->accelerationStructureHostCommands = true;
         if (features->descriptorBindingAccelerationStructureUpdateAfterBind)
            all_features->descriptorBindingAccelerationStructureUpdateAfterBind = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR: {
         const VkPhysicalDeviceRayTracingPipelineFeaturesKHR *features = (const void *) ext;
         if (features->rayTracingPipeline)
            all_features->rayTracingPipeline = true;
         if (features->rayTracingPipelineShaderGroupHandleCaptureReplay)
            all_features->rayTracingPipelineShaderGroupHandleCaptureReplay = true;
         if (features->rayTracingPipelineShaderGroupHandleCaptureReplayMixed)
            all_features->rayTracingPipelineShaderGroupHandleCaptureReplayMixed = true;
         if (features->rayTracingPipelineTraceRaysIndirect)
            all_features->rayTracingPipelineTraceRaysIndirect = true;
         if (features->rayTraversalPrimitiveCulling)
            all_features->rayTraversalPrimitiveCulling = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR: {
         const VkPhysicalDeviceRayQueryFeaturesKHR *features = (const void *) ext;
         if (features->rayQuery)
            all_features->rayQuery = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR: {
         const VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *features = (const void *) ext;
         if (features->rayTracingMaintenance1)
            all_features->rayTracingMaintenance1 = true;
         if (features->rayTracingPipelineTraceRaysIndirect2)
            all_features->rayTracingPipelineTraceRaysIndirect2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT: {
         const VkPhysicalDeviceFragmentDensityMapFeaturesEXT *features = (const void *) ext;
         if (features->fragmentDensityMap)
            all_features->fragmentDensityMap = true;
         if (features->fragmentDensityMapDynamic)
            all_features->fragmentDensityMapDynamic = true;
         if (features->fragmentDensityMapNonSubsampledImages)
            all_features->fragmentDensityMapNonSubsampledImages = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT: {
         const VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *features = (const void *) ext;
         if (features->fragmentDensityMapDeferred)
            all_features->fragmentDensityMapDeferred = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_EXT: {
         const VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT *features = (const void *) ext;
         if (features->fragmentDensityMapOffset)
            all_features->fragmentDensityMapOffset = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES: {
         const VkPhysicalDeviceScalarBlockLayoutFeatures *features = (const void *) ext;
         if (features->scalarBlockLayout)
            all_features->scalarBlockLayout = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES: {
         const VkPhysicalDeviceUniformBufferStandardLayoutFeatures *features = (const void *) ext;
         if (features->uniformBufferStandardLayout)
            all_features->uniformBufferStandardLayout = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT: {
         const VkPhysicalDeviceDepthClipEnableFeaturesEXT *features = (const void *) ext;
         if (features->depthClipEnable)
            all_features->depthClipEnable = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT: {
         const VkPhysicalDeviceMemoryPriorityFeaturesEXT *features = (const void *) ext;
         if (features->memoryPriority)
            all_features->memoryPriority = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT: {
         const VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *features = (const void *) ext;
         if (features->pageableDeviceLocalMemory)
            all_features->pageableDeviceLocalMemory = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES: {
         const VkPhysicalDeviceBufferDeviceAddressFeatures *features = (const void *) ext;
         if (features->bufferDeviceAddress)
            all_features->bufferDeviceAddress = true;
         if (features->bufferDeviceAddressCaptureReplay)
            all_features->bufferDeviceAddressCaptureReplay = true;
         if (features->bufferDeviceAddressMultiDevice)
            all_features->bufferDeviceAddressMultiDevice = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT: {
         const VkPhysicalDeviceBufferDeviceAddressFeaturesEXT *features = (const void *) ext;
         if (features->bufferDeviceAddress)
            all_features->bufferDeviceAddress = true;
         if (features->bufferDeviceAddressCaptureReplay)
            all_features->bufferDeviceAddressCaptureReplayEXT = true;
         if (features->bufferDeviceAddressMultiDevice)
            all_features->bufferDeviceAddressMultiDevice = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES: {
         const VkPhysicalDeviceImagelessFramebufferFeatures *features = (const void *) ext;
         if (features->imagelessFramebuffer)
            all_features->imagelessFramebuffer = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES: {
         const VkPhysicalDeviceTextureCompressionASTCHDRFeatures *features = (const void *) ext;
         if (features->textureCompressionASTC_HDR)
            all_features->textureCompressionASTC_HDR = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV: {
         const VkPhysicalDeviceCooperativeMatrixFeaturesNV *features = (const void *) ext;
         if (features->cooperativeMatrix)
            all_features->cooperativeMatrixNV = true;
         if (features->cooperativeMatrixRobustBufferAccess)
            all_features->cooperativeMatrixRobustBufferAccessNV = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT: {
         const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *features = (const void *) ext;
         if (features->ycbcrImageArrays)
            all_features->ycbcrImageArrays = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_BARRIER_FEATURES_NV: {
         const VkPhysicalDevicePresentBarrierFeaturesNV *features = (const void *) ext;
         if (features->presentBarrier)
            all_features->presentBarrier = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR: {
         const VkPhysicalDevicePerformanceQueryFeaturesKHR *features = (const void *) ext;
         if (features->performanceCounterQueryPools)
            all_features->performanceCounterQueryPools = true;
         if (features->performanceCounterMultipleQueryPools)
            all_features->performanceCounterMultipleQueryPools = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV: {
         const VkPhysicalDeviceCoverageReductionModeFeaturesNV *features = (const void *) ext;
         if (features->coverageReductionMode)
            all_features->coverageReductionMode = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL: {
         const VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *features = (const void *) ext;
         if (features->shaderIntegerFunctions2)
            all_features->shaderIntegerFunctions2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR: {
         const VkPhysicalDeviceShaderClockFeaturesKHR *features = (const void *) ext;
         if (features->shaderSubgroupClock)
            all_features->shaderSubgroupClock = true;
         if (features->shaderDeviceClock)
            all_features->shaderDeviceClock = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES: {
         const VkPhysicalDeviceIndexTypeUint8Features *features = (const void *) ext;
         if (features->indexTypeUint8)
            all_features->indexTypeUint8 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV: {
         const VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *features = (const void *) ext;
         if (features->shaderSMBuiltins)
            all_features->shaderSMBuiltins = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT: {
         const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *features = (const void *) ext;
         if (features->fragmentShaderSampleInterlock)
            all_features->fragmentShaderSampleInterlock = true;
         if (features->fragmentShaderPixelInterlock)
            all_features->fragmentShaderPixelInterlock = true;
         if (features->fragmentShaderShadingRateInterlock)
            all_features->fragmentShaderShadingRateInterlock = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES: {
         const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures *features = (const void *) ext;
         if (features->separateDepthStencilLayouts)
            all_features->separateDepthStencilLayouts = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT: {
         const VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *features = (const void *) ext;
         if (features->primitiveTopologyListRestart)
            all_features->primitiveTopologyListRestart = true;
         if (features->primitiveTopologyPatchListRestart)
            all_features->primitiveTopologyPatchListRestart = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR: {
         const VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *features = (const void *) ext;
         if (features->pipelineExecutableInfo)
            all_features->pipelineExecutableInfo = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES: {
         const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures *features = (const void *) ext;
         if (features->shaderDemoteToHelperInvocation)
            all_features->shaderDemoteToHelperInvocation = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT: {
         const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *features = (const void *) ext;
         if (features->texelBufferAlignment)
            all_features->texelBufferAlignment = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES: {
         const VkPhysicalDeviceSubgroupSizeControlFeatures *features = (const void *) ext;
         if (features->subgroupSizeControl)
            all_features->subgroupSizeControl = true;
         if (features->computeFullSubgroups)
            all_features->computeFullSubgroups = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES: {
         const VkPhysicalDeviceLineRasterizationFeatures *features = (const void *) ext;
         if (features->rectangularLines)
            all_features->rectangularLines = true;
         if (features->bresenhamLines)
            all_features->bresenhamLines = true;
         if (features->smoothLines)
            all_features->smoothLines = true;
         if (features->stippledRectangularLines)
            all_features->stippledRectangularLines = true;
         if (features->stippledBresenhamLines)
            all_features->stippledBresenhamLines = true;
         if (features->stippledSmoothLines)
            all_features->stippledSmoothLines = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES: {
         const VkPhysicalDevicePipelineCreationCacheControlFeatures *features = (const void *) ext;
         if (features->pipelineCreationCacheControl)
            all_features->pipelineCreationCacheControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
         const VkPhysicalDeviceVulkan11Features *features = (const void *) ext;
         if (features->storageBuffer16BitAccess)
            all_features->storageBuffer16BitAccess = true;
         if (features->uniformAndStorageBuffer16BitAccess)
            all_features->uniformAndStorageBuffer16BitAccess = true;
         if (features->storagePushConstant16)
            all_features->storagePushConstant16 = true;
         if (features->storageInputOutput16)
            all_features->storageInputOutput16 = true;
         if (features->multiview)
            all_features->multiview = true;
         if (features->multiviewGeometryShader)
            all_features->multiviewGeometryShader = true;
         if (features->multiviewTessellationShader)
            all_features->multiviewTessellationShader = true;
         if (features->variablePointersStorageBuffer)
            all_features->variablePointersStorageBuffer = true;
         if (features->variablePointers)
            all_features->variablePointers = true;
         if (features->protectedMemory)
            all_features->protectedMemory = true;
         if (features->samplerYcbcrConversion)
            all_features->samplerYcbcrConversion = true;
         if (features->shaderDrawParameters)
            all_features->shaderDrawParameters = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
         const VkPhysicalDeviceVulkan12Features *features = (const void *) ext;
         if (features->samplerMirrorClampToEdge)
            all_features->samplerMirrorClampToEdge = true;
         if (features->drawIndirectCount)
            all_features->drawIndirectCount = true;
         if (features->storageBuffer8BitAccess)
            all_features->storageBuffer8BitAccess = true;
         if (features->uniformAndStorageBuffer8BitAccess)
            all_features->uniformAndStorageBuffer8BitAccess = true;
         if (features->storagePushConstant8)
            all_features->storagePushConstant8 = true;
         if (features->shaderBufferInt64Atomics)
            all_features->shaderBufferInt64Atomics = true;
         if (features->shaderSharedInt64Atomics)
            all_features->shaderSharedInt64Atomics = true;
         if (features->shaderFloat16)
            all_features->shaderFloat16 = true;
         if (features->shaderInt8)
            all_features->shaderInt8 = true;
         if (features->descriptorIndexing)
            all_features->descriptorIndexing = true;
         if (features->shaderInputAttachmentArrayDynamicIndexing)
            all_features->shaderInputAttachmentArrayDynamicIndexing = true;
         if (features->shaderUniformTexelBufferArrayDynamicIndexing)
            all_features->shaderUniformTexelBufferArrayDynamicIndexing = true;
         if (features->shaderStorageTexelBufferArrayDynamicIndexing)
            all_features->shaderStorageTexelBufferArrayDynamicIndexing = true;
         if (features->shaderUniformBufferArrayNonUniformIndexing)
            all_features->shaderUniformBufferArrayNonUniformIndexing = true;
         if (features->shaderSampledImageArrayNonUniformIndexing)
            all_features->shaderSampledImageArrayNonUniformIndexing = true;
         if (features->shaderStorageBufferArrayNonUniformIndexing)
            all_features->shaderStorageBufferArrayNonUniformIndexing = true;
         if (features->shaderStorageImageArrayNonUniformIndexing)
            all_features->shaderStorageImageArrayNonUniformIndexing = true;
         if (features->shaderInputAttachmentArrayNonUniformIndexing)
            all_features->shaderInputAttachmentArrayNonUniformIndexing = true;
         if (features->shaderUniformTexelBufferArrayNonUniformIndexing)
            all_features->shaderUniformTexelBufferArrayNonUniformIndexing = true;
         if (features->shaderStorageTexelBufferArrayNonUniformIndexing)
            all_features->shaderStorageTexelBufferArrayNonUniformIndexing = true;
         if (features->descriptorBindingUniformBufferUpdateAfterBind)
            all_features->descriptorBindingUniformBufferUpdateAfterBind = true;
         if (features->descriptorBindingSampledImageUpdateAfterBind)
            all_features->descriptorBindingSampledImageUpdateAfterBind = true;
         if (features->descriptorBindingStorageImageUpdateAfterBind)
            all_features->descriptorBindingStorageImageUpdateAfterBind = true;
         if (features->descriptorBindingStorageBufferUpdateAfterBind)
            all_features->descriptorBindingStorageBufferUpdateAfterBind = true;
         if (features->descriptorBindingUniformTexelBufferUpdateAfterBind)
            all_features->descriptorBindingUniformTexelBufferUpdateAfterBind = true;
         if (features->descriptorBindingStorageTexelBufferUpdateAfterBind)
            all_features->descriptorBindingStorageTexelBufferUpdateAfterBind = true;
         if (features->descriptorBindingUpdateUnusedWhilePending)
            all_features->descriptorBindingUpdateUnusedWhilePending = true;
         if (features->descriptorBindingPartiallyBound)
            all_features->descriptorBindingPartiallyBound = true;
         if (features->descriptorBindingVariableDescriptorCount)
            all_features->descriptorBindingVariableDescriptorCount = true;
         if (features->runtimeDescriptorArray)
            all_features->runtimeDescriptorArray = true;
         if (features->samplerFilterMinmax)
            all_features->samplerFilterMinmax = true;
         if (features->scalarBlockLayout)
            all_features->scalarBlockLayout = true;
         if (features->imagelessFramebuffer)
            all_features->imagelessFramebuffer = true;
         if (features->uniformBufferStandardLayout)
            all_features->uniformBufferStandardLayout = true;
         if (features->shaderSubgroupExtendedTypes)
            all_features->shaderSubgroupExtendedTypes = true;
         if (features->separateDepthStencilLayouts)
            all_features->separateDepthStencilLayouts = true;
         if (features->hostQueryReset)
            all_features->hostQueryReset = true;
         if (features->timelineSemaphore)
            all_features->timelineSemaphore = true;
         if (features->bufferDeviceAddress)
            all_features->bufferDeviceAddress = true;
         if (features->bufferDeviceAddressCaptureReplay)
            all_features->bufferDeviceAddressCaptureReplay = true;
         if (features->bufferDeviceAddressMultiDevice)
            all_features->bufferDeviceAddressMultiDevice = true;
         if (features->vulkanMemoryModel)
            all_features->vulkanMemoryModel = true;
         if (features->vulkanMemoryModelDeviceScope)
            all_features->vulkanMemoryModelDeviceScope = true;
         if (features->vulkanMemoryModelAvailabilityVisibilityChains)
            all_features->vulkanMemoryModelAvailabilityVisibilityChains = true;
         if (features->shaderOutputViewportIndex)
            all_features->shaderOutputViewportIndex = true;
         if (features->shaderOutputLayer)
            all_features->shaderOutputLayer = true;
         if (features->subgroupBroadcastDynamicId)
            all_features->subgroupBroadcastDynamicId = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
         const VkPhysicalDeviceVulkan13Features *features = (const void *) ext;
         if (features->robustImageAccess)
            all_features->robustImageAccess = true;
         if (features->inlineUniformBlock)
            all_features->inlineUniformBlock = true;
         if (features->descriptorBindingInlineUniformBlockUpdateAfterBind)
            all_features->descriptorBindingInlineUniformBlockUpdateAfterBind = true;
         if (features->pipelineCreationCacheControl)
            all_features->pipelineCreationCacheControl = true;
         if (features->privateData)
            all_features->privateData = true;
         if (features->shaderDemoteToHelperInvocation)
            all_features->shaderDemoteToHelperInvocation = true;
         if (features->shaderTerminateInvocation)
            all_features->shaderTerminateInvocation = true;
         if (features->subgroupSizeControl)
            all_features->subgroupSizeControl = true;
         if (features->computeFullSubgroups)
            all_features->computeFullSubgroups = true;
         if (features->synchronization2)
            all_features->synchronization2 = true;
         if (features->textureCompressionASTC_HDR)
            all_features->textureCompressionASTC_HDR = true;
         if (features->shaderZeroInitializeWorkgroupMemory)
            all_features->shaderZeroInitializeWorkgroupMemory = true;
         if (features->dynamicRendering)
            all_features->dynamicRendering = true;
         if (features->shaderIntegerDotProduct)
            all_features->shaderIntegerDotProduct = true;
         if (features->maintenance4)
            all_features->maintenance4 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES: {
         const VkPhysicalDeviceVulkan14Features *features = (const void *) ext;
         if (features->globalPriorityQuery)
            all_features->globalPriorityQuery = true;
         if (features->shaderSubgroupRotate)
            all_features->shaderSubgroupRotate = true;
         if (features->shaderSubgroupRotateClustered)
            all_features->shaderSubgroupRotateClustered = true;
         if (features->shaderFloatControls2)
            all_features->shaderFloatControls2 = true;
         if (features->shaderExpectAssume)
            all_features->shaderExpectAssume = true;
         if (features->rectangularLines)
            all_features->rectangularLines = true;
         if (features->bresenhamLines)
            all_features->bresenhamLines = true;
         if (features->smoothLines)
            all_features->smoothLines = true;
         if (features->stippledRectangularLines)
            all_features->stippledRectangularLines = true;
         if (features->stippledBresenhamLines)
            all_features->stippledBresenhamLines = true;
         if (features->stippledSmoothLines)
            all_features->stippledSmoothLines = true;
         if (features->vertexAttributeInstanceRateDivisor)
            all_features->vertexAttributeInstanceRateDivisor = true;
         if (features->vertexAttributeInstanceRateZeroDivisor)
            all_features->vertexAttributeInstanceRateZeroDivisor = true;
         if (features->indexTypeUint8)
            all_features->indexTypeUint8 = true;
         if (features->dynamicRenderingLocalRead)
            all_features->dynamicRenderingLocalRead = true;
         if (features->maintenance5)
            all_features->maintenance5 = true;
         if (features->maintenance6)
            all_features->maintenance6 = true;
         if (features->pipelineProtectedAccess)
            all_features->pipelineProtectedAccess = true;
         if (features->pipelineRobustness)
            all_features->pipelineRobustness = true;
         if (features->hostImageCopy)
            all_features->hostImageCopy = true;
         if (features->pushDescriptor)
            all_features->pushDescriptor = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD: {
         const VkPhysicalDeviceCoherentMemoryFeaturesAMD *features = (const void *) ext;
         if (features->deviceCoherentMemory)
            all_features->deviceCoherentMemory = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GPA_FEATURES_AMD: {
         const VkPhysicalDeviceGpaFeaturesAMD *features = (const void *) ext;
         if (features->perfCounters)
            all_features->perfCounters = true;
         if (features->streamingPerfCounters)
            all_features->streamingPerfCounters = true;
         if (features->sqThreadTracing)
            all_features->sqThreadTracing = true;
         if (features->clockModes)
            all_features->clockModes = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT: {
         const VkPhysicalDeviceCustomBorderColorFeaturesEXT *features = (const void *) ext;
         if (features->customBorderColors)
            all_features->customBorderColors = true;
         if (features->customBorderColorWithoutFormat)
            all_features->customBorderColorWithoutFormat = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT: {
         const VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *features = (const void *) ext;
         if (features->borderColorSwizzle)
            all_features->borderColorSwizzle = true;
         if (features->borderColorSwizzleFromImage)
            all_features->borderColorSwizzleFromImage = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT: {
         const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *features = (const void *) ext;
         if (features->extendedDynamicState)
            all_features->extendedDynamicState = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT: {
         const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *features = (const void *) ext;
         if (features->extendedDynamicState2)
            all_features->extendedDynamicState2 = true;
         if (features->extendedDynamicState2LogicOp)
            all_features->extendedDynamicState2LogicOp = true;
         if (features->extendedDynamicState2PatchControlPoints)
            all_features->extendedDynamicState2PatchControlPoints = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT: {
         const VkPhysicalDeviceExtendedDynamicState3FeaturesEXT *features = (const void *) ext;
         if (features->extendedDynamicState3TessellationDomainOrigin)
            all_features->extendedDynamicState3TessellationDomainOrigin = true;
         if (features->extendedDynamicState3DepthClampEnable)
            all_features->extendedDynamicState3DepthClampEnable = true;
         if (features->extendedDynamicState3PolygonMode)
            all_features->extendedDynamicState3PolygonMode = true;
         if (features->extendedDynamicState3RasterizationSamples)
            all_features->extendedDynamicState3RasterizationSamples = true;
         if (features->extendedDynamicState3SampleMask)
            all_features->extendedDynamicState3SampleMask = true;
         if (features->extendedDynamicState3AlphaToCoverageEnable)
            all_features->extendedDynamicState3AlphaToCoverageEnable = true;
         if (features->extendedDynamicState3AlphaToOneEnable)
            all_features->extendedDynamicState3AlphaToOneEnable = true;
         if (features->extendedDynamicState3LogicOpEnable)
            all_features->extendedDynamicState3LogicOpEnable = true;
         if (features->extendedDynamicState3ColorBlendEnable)
            all_features->extendedDynamicState3ColorBlendEnable = true;
         if (features->extendedDynamicState3ColorBlendEquation)
            all_features->extendedDynamicState3ColorBlendEquation = true;
         if (features->extendedDynamicState3ColorWriteMask)
            all_features->extendedDynamicState3ColorWriteMask = true;
         if (features->extendedDynamicState3RasterizationStream)
            all_features->extendedDynamicState3RasterizationStream = true;
         if (features->extendedDynamicState3ConservativeRasterizationMode)
            all_features->extendedDynamicState3ConservativeRasterizationMode = true;
         if (features->extendedDynamicState3ExtraPrimitiveOverestimationSize)
            all_features->extendedDynamicState3ExtraPrimitiveOverestimationSize = true;
         if (features->extendedDynamicState3DepthClipEnable)
            all_features->extendedDynamicState3DepthClipEnable = true;
         if (features->extendedDynamicState3SampleLocationsEnable)
            all_features->extendedDynamicState3SampleLocationsEnable = true;
         if (features->extendedDynamicState3ColorBlendAdvanced)
            all_features->extendedDynamicState3ColorBlendAdvanced = true;
         if (features->extendedDynamicState3ProvokingVertexMode)
            all_features->extendedDynamicState3ProvokingVertexMode = true;
         if (features->extendedDynamicState3LineRasterizationMode)
            all_features->extendedDynamicState3LineRasterizationMode = true;
         if (features->extendedDynamicState3LineStippleEnable)
            all_features->extendedDynamicState3LineStippleEnable = true;
         if (features->extendedDynamicState3DepthClipNegativeOneToOne)
            all_features->extendedDynamicState3DepthClipNegativeOneToOne = true;
         if (features->extendedDynamicState3ViewportWScalingEnable)
            all_features->extendedDynamicState3ViewportWScalingEnable = true;
         if (features->extendedDynamicState3ViewportSwizzle)
            all_features->extendedDynamicState3ViewportSwizzle = true;
         if (features->extendedDynamicState3CoverageToColorEnable)
            all_features->extendedDynamicState3CoverageToColorEnable = true;
         if (features->extendedDynamicState3CoverageToColorLocation)
            all_features->extendedDynamicState3CoverageToColorLocation = true;
         if (features->extendedDynamicState3CoverageModulationMode)
            all_features->extendedDynamicState3CoverageModulationMode = true;
         if (features->extendedDynamicState3CoverageModulationTableEnable)
            all_features->extendedDynamicState3CoverageModulationTableEnable = true;
         if (features->extendedDynamicState3CoverageModulationTable)
            all_features->extendedDynamicState3CoverageModulationTable = true;
         if (features->extendedDynamicState3CoverageReductionMode)
            all_features->extendedDynamicState3CoverageReductionMode = true;
         if (features->extendedDynamicState3RepresentativeFragmentTestEnable)
            all_features->extendedDynamicState3RepresentativeFragmentTestEnable = true;
         if (features->extendedDynamicState3ShadingRateImageEnable)
            all_features->extendedDynamicState3ShadingRateImageEnable = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_FLAGS_FEATURES_KHR: {
         const VkPhysicalDeviceExtendedFlagsFeaturesKHR *features = (const void *) ext;
         if (features->extendedFlags)
            all_features->extendedFlags = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PARTITIONED_ACCELERATION_STRUCTURE_FEATURES_NV: {
         const VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV *features = (const void *) ext;
         if (features->partitionedAccelerationStructure)
            all_features->partitionedAccelerationStructure = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV: {
         const VkPhysicalDeviceDiagnosticsConfigFeaturesNV *features = (const void *) ext;
         if (features->diagnosticsConfig)
            all_features->diagnosticsConfig = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES: {
         const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures *features = (const void *) ext;
         if (features->shaderZeroInitializeWorkgroupMemory)
            all_features->shaderZeroInitializeWorkgroupMemory = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR: {
         const VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *features = (const void *) ext;
         if (features->shaderSubgroupUniformControlFlow)
            all_features->shaderSubgroupUniformControlFlow = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR: {
         const VkPhysicalDeviceRobustness2FeaturesKHR *features = (const void *) ext;
         if (features->robustBufferAccess2)
            all_features->robustBufferAccess2 = true;
         if (features->robustImageAccess2)
            all_features->robustImageAccess2 = true;
         if (features->nullDescriptor)
            all_features->nullDescriptor = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES: {
         const VkPhysicalDeviceImageRobustnessFeatures *features = (const void *) ext;
         if (features->robustImageAccess)
            all_features->robustImageAccess = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR: {
         const VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *features = (const void *) ext;
         if (features->workgroupMemoryExplicitLayout)
            all_features->workgroupMemoryExplicitLayout = true;
         if (features->workgroupMemoryExplicitLayoutScalarBlockLayout)
            all_features->workgroupMemoryExplicitLayoutScalarBlockLayout = true;
         if (features->workgroupMemoryExplicitLayout8BitAccess)
            all_features->workgroupMemoryExplicitLayout8BitAccess = true;
         if (features->workgroupMemoryExplicitLayout16BitAccess)
            all_features->workgroupMemoryExplicitLayout16BitAccess = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT: {
         const VkPhysicalDevice4444FormatsFeaturesEXT *features = (const void *) ext;
         if (features->formatA4R4G4B4)
            all_features->formatA4R4G4B4 = true;
         if (features->formatA4B4G4R4)
            all_features->formatA4B4G4R4 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI: {
         const VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *features = (const void *) ext;
         if (features->subpassShading)
            all_features->subpassShading = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CLUSTER_CULLING_SHADER_FEATURES_HUAWEI: {
         const VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI *features = (const void *) ext;
         if (features->clustercullingShader)
            all_features->clustercullingShader = true;
         if (features->multiviewClusterCullingShader)
            all_features->multiviewClusterCullingShader = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT: {
         const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *features = (const void *) ext;
         if (features->shaderImageInt64Atomics)
            all_features->shaderImageInt64Atomics = true;
         if (features->sparseImageInt64Atomics)
            all_features->sparseImageInt64Atomics = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR: {
         const VkPhysicalDeviceFragmentShadingRateFeaturesKHR *features = (const void *) ext;
         if (features->pipelineFragmentShadingRate)
            all_features->pipelineFragmentShadingRate = true;
         if (features->primitiveFragmentShadingRate)
            all_features->primitiveFragmentShadingRate = true;
         if (features->attachmentFragmentShadingRate)
            all_features->attachmentFragmentShadingRate = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES: {
         const VkPhysicalDeviceShaderTerminateInvocationFeatures *features = (const void *) ext;
         if (features->shaderTerminateInvocation)
            all_features->shaderTerminateInvocation = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV: {
         const VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *features = (const void *) ext;
         if (features->fragmentShadingRateEnums)
            all_features->fragmentShadingRateEnums = true;
         if (features->supersampleFragmentShadingRates)
            all_features->supersampleFragmentShadingRates = true;
         if (features->noInvocationFragmentShadingRates)
            all_features->noInvocationFragmentShadingRates = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT: {
         const VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *features = (const void *) ext;
         if (features->image2DViewOf3D)
            all_features->image2DViewOf3D = true;
         if (features->sampler2DViewOf3D)
            all_features->sampler2DViewOf3D = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_SLICED_VIEW_OF_3D_FEATURES_EXT: {
         const VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT *features = (const void *) ext;
         if (features->imageSlicedViewOf3D)
            all_features->imageSlicedViewOf3D = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_DYNAMIC_STATE_FEATURES_EXT: {
         const VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT *features = (const void *) ext;
         if (features->attachmentFeedbackLoopDynamicState)
            all_features->attachmentFeedbackLoopDynamicState = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_VERTEX_ATTRIBUTES_FEATURES_EXT: {
         const VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT *features = (const void *) ext;
         if (features->legacyVertexAttributes)
            all_features->legacyVertexAttributes = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT: {
         const VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT *features = (const void *) ext;
         if (features->mutableDescriptorType)
            all_features->mutableDescriptorType = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT: {
         const VkPhysicalDeviceDepthClipControlFeaturesEXT *features = (const void *) ext;
         if (features->depthClipControl)
            all_features->depthClipControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_DEVICE_MEMORY_FEATURES_EXT: {
         const VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT *features = (const void *) ext;
         if (features->zeroInitializeDeviceMemory)
            all_features->zeroInitializeDeviceMemory = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_RESOLVE_FEATURES_EXT: {
         const VkPhysicalDeviceCustomResolveFeaturesEXT *features = (const void *) ext;
         if (features->customResolve)
            all_features->customResolve = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_EXT: {
         const VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT *features = (const void *) ext;
         if (features->deviceGeneratedCommands)
            all_features->deviceGeneratedCommands = true;
         if (features->dynamicGeneratedPipelineLayout)
            all_features->dynamicGeneratedPipelineLayout = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_CONTROL_FEATURES_EXT: {
         const VkPhysicalDeviceDepthClampControlFeaturesEXT *features = (const void *) ext;
         if (features->depthClampControl)
            all_features->depthClampControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT: {
         const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *features = (const void *) ext;
         if (features->vertexInputDynamicState)
            all_features->vertexInputDynamicState = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV: {
         const VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *features = (const void *) ext;
         if (features->externalMemoryRDMA)
            all_features->externalMemoryRDMA = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR: {
         const VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR *features = (const void *) ext;
         if (features->shaderRelaxedExtendedInstruction)
            all_features->shaderRelaxedExtendedInstruction = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT: {
         const VkPhysicalDeviceColorWriteEnableFeaturesEXT *features = (const void *) ext;
         if (features->colorWriteEnable)
            all_features->colorWriteEnable = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES: {
         const VkPhysicalDeviceSynchronization2Features *features = (const void *) ext;
         if (features->synchronization2)
            all_features->synchronization2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR: {
         const VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR *features = (const void *) ext;
         if (features->unifiedImageLayouts)
            all_features->unifiedImageLayouts = true;
         if (features->unifiedImageLayoutsVideo)
            all_features->unifiedImageLayoutsVideo = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES: {
         const VkPhysicalDeviceHostImageCopyFeatures *features = (const void *) ext;
         if (features->hostImageCopy)
            all_features->hostImageCopy = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT: {
         const VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *features = (const void *) ext;
         if (features->primitivesGeneratedQuery)
            all_features->primitivesGeneratedQuery = true;
         if (features->primitivesGeneratedQueryWithRasterizerDiscard)
            all_features->primitivesGeneratedQueryWithRasterizerDiscard = true;
         if (features->primitivesGeneratedQueryWithNonZeroStreams)
            all_features->primitivesGeneratedQueryWithNonZeroStreams = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LEGACY_DITHERING_FEATURES_EXT: {
         const VkPhysicalDeviceLegacyDitheringFeaturesEXT *features = (const void *) ext;
         if (features->legacyDithering)
            all_features->legacyDithering = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT: {
         const VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *features = (const void *) ext;
         if (features->multisampledRenderToSingleSampled)
            all_features->multisampledRenderToSingleSampled = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SWAPCHAIN_FEATURES_EXT: {
         const VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT *features = (const void *) ext;
         if (features->multisampledRenderToSwapchain)
            all_features->multisampledRenderToSwapchain = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES: {
         const VkPhysicalDevicePipelineProtectedAccessFeatures *features = (const void *) ext;
         if (features->pipelineProtectedAccess)
            all_features->pipelineProtectedAccess = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_1_FEATURES_KHR: {
         const VkPhysicalDeviceVideoMaintenance1FeaturesKHR *features = (const void *) ext;
         if (features->videoMaintenance1)
            all_features->videoMaintenance1 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_MAINTENANCE_2_FEATURES_KHR: {
         const VkPhysicalDeviceVideoMaintenance2FeaturesKHR *features = (const void *) ext;
         if (features->videoMaintenance2)
            all_features->videoMaintenance2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_DECODE_VP9_FEATURES_KHR: {
         const VkPhysicalDeviceVideoDecodeVP9FeaturesKHR *features = (const void *) ext;
         if (features->videoDecodeVP9)
            all_features->videoDecodeVP9 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_QUANTIZATION_MAP_FEATURES_KHR: {
         const VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR *features = (const void *) ext;
         if (features->videoEncodeQuantizationMap)
            all_features->videoEncodeQuantizationMap = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_FEEDBACK_2_FEATURES_KHR: {
         const VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR *features = (const void *) ext;
         if (features->videoEncodeFeedback2)
            all_features->videoEncodeFeedback2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_AV1_FEATURES_KHR: {
         const VkPhysicalDeviceVideoEncodeAV1FeaturesKHR *features = (const void *) ext;
         if (features->videoEncodeAV1)
            all_features->videoEncodeAV1 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV: {
         const VkPhysicalDeviceInheritedViewportScissorFeaturesNV *features = (const void *) ext;
         if (features->inheritedViewportScissor2D)
            all_features->inheritedViewportScissor2D = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT: {
         const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *features = (const void *) ext;
         if (features->ycbcr2plane444Formats)
            all_features->ycbcr2plane444Formats = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT: {
         const VkPhysicalDeviceProvokingVertexFeaturesEXT *features = (const void *) ext;
         if (features->provokingVertexLast)
            all_features->provokingVertexLast = true;
         if (features->transformFeedbackPreservesProvokingVertex)
            all_features->transformFeedbackPreservesProvokingVertex = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_INTRA_REFRESH_FEATURES_KHR: {
         const VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR *features = (const void *) ext;
         if (features->videoEncodeIntraRefresh)
            all_features->videoEncodeIntraRefresh = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT: {
         const VkPhysicalDeviceDescriptorBufferFeaturesEXT *features = (const void *) ext;
         if (features->descriptorBuffer)
            all_features->descriptorBuffer = true;
         if (features->descriptorBufferCaptureReplay)
            all_features->descriptorBufferCaptureReplay = true;
         if (features->descriptorBufferImageLayoutIgnored)
            all_features->descriptorBufferImageLayoutIgnored = true;
         if (features->descriptorBufferPushDescriptors)
            all_features->descriptorBufferPushDescriptors = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES: {
         const VkPhysicalDeviceShaderIntegerDotProductFeatures *features = (const void *) ext;
         if (features->shaderIntegerDotProduct)
            all_features->shaderIntegerDotProduct = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_KHR: {
         const VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR *features = (const void *) ext;
         if (features->fragmentShaderBarycentric)
            all_features->fragmentShaderBarycentric = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FMA_FEATURES_KHR: {
         const VkPhysicalDeviceShaderFmaFeaturesKHR *features = (const void *) ext;
         if (features->shaderFmaFloat16)
            all_features->shaderFmaFloat16 = true;
         if (features->shaderFmaFloat32)
            all_features->shaderFmaFloat32 = true;
         if (features->shaderFmaFloat64)
            all_features->shaderFmaFloat64 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV: {
         const VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *features = (const void *) ext;
         if (features->rayTracingMotionBlur)
            all_features->rayTracingMotionBlur = true;
         if (features->rayTracingMotionBlurPipelineTraceRaysIndirect)
            all_features->rayTracingMotionBlurPipelineTraceRaysIndirect = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV: {
         const VkPhysicalDeviceRayTracingValidationFeaturesNV *features = (const void *) ext;
         if (features->rayTracingValidation)
            all_features->rayTracingValidation = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_LINEAR_SWEPT_SPHERES_FEATURES_NV: {
         const VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV *features = (const void *) ext;
         if (features->spheres)
            all_features->spheres = true;
         if (features->linearSweptSpheres)
            all_features->linearSweptSpheres = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT: {
         const VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *features = (const void *) ext;
         if (features->formatRgba10x6WithoutYCbCrSampler)
            all_features->formatRgba10x6WithoutYCbCrSampler = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES: {
         const VkPhysicalDeviceDynamicRenderingFeatures *features = (const void *) ext;
         if (features->dynamicRendering)
            all_features->dynamicRendering = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT: {
         const VkPhysicalDeviceImageViewMinLodFeaturesEXT *features = (const void *) ext;
         if (features->minLod)
            all_features->minLod = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_EXT: {
         const VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT *features = (const void *) ext;
         if (features->rasterizationOrderColorAttachmentAccess)
            all_features->rasterizationOrderColorAttachmentAccess = true;
         if (features->rasterizationOrderDepthAttachmentAccess)
            all_features->rasterizationOrderDepthAttachmentAccess = true;
         if (features->rasterizationOrderStencilAttachmentAccess)
            all_features->rasterizationOrderStencilAttachmentAccess = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV: {
         const VkPhysicalDeviceLinearColorAttachmentFeaturesNV *features = (const void *) ext;
         if (features->linearColorAttachment)
            all_features->linearColorAttachment = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT: {
         const VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *features = (const void *) ext;
         if (features->graphicsPipelineLibrary)
            all_features->graphicsPipelineLibrary = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_BINARY_FEATURES_KHR: {
         const VkPhysicalDevicePipelineBinaryFeaturesKHR *features = (const void *) ext;
         if (features->pipelineBinaries)
            all_features->pipelineBinaries = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_NEURAL_ACCELERATOR_STATISTICS_FEATURES_ARM: {
         const VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM *features = (const void *) ext;
         if (features->dataGraphNeuralAcceleratorStatistics)
            all_features->dataGraphNeuralAcceleratorStatistics = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE: {
         const VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *features = (const void *) ext;
         if (features->descriptorSetHostMapping)
            all_features->descriptorSetHostMapping = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT: {
         const VkPhysicalDeviceNestedCommandBufferFeaturesEXT *features = (const void *) ext;
         if (features->nestedCommandBuffer)
            all_features->nestedCommandBuffer = true;
         if (features->nestedCommandBufferRendering)
            all_features->nestedCommandBufferRendering = true;
         if (features->nestedCommandBufferSimultaneousUse)
            all_features->nestedCommandBufferSimultaneousUse = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT: {
         const VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *features = (const void *) ext;
         if (features->shaderModuleIdentifier)
            all_features->shaderModuleIdentifier = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT: {
         const VkPhysicalDeviceImageCompressionControlFeaturesEXT *features = (const void *) ext;
         if (features->imageCompressionControl)
            all_features->imageCompressionControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT: {
         const VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *features = (const void *) ext;
         if (features->imageCompressionControlSwapchain)
            all_features->imageCompressionControlSwapchain = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT: {
         const VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *features = (const void *) ext;
         if (features->subpassMergeFeedback)
            all_features->subpassMergeFeedback = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_KHR: {
         const VkPhysicalDeviceOpacityMicromapFeaturesKHR *features = (const void *) ext;
         if (features->micromap)
            all_features->micromap = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT: {
         const VkPhysicalDeviceOpacityMicromapFeaturesEXT *features = (const void *) ext;
         if (features->micromap)
            all_features->micromapEXT = true;
         if (features->micromapCaptureReplay)
            all_features->micromapCaptureReplay = true;
         if (features->micromapHostCommands)
            all_features->micromapHostCommands = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT: {
         const VkPhysicalDevicePipelinePropertiesFeaturesEXT *features = (const void *) ext;
         if (features->pipelinePropertiesIdentifier)
            all_features->pipelinePropertiesIdentifier = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD: {
         const VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *features = (const void *) ext;
         if (features->shaderEarlyAndLateFragmentTests)
            all_features->shaderEarlyAndLateFragmentTests = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT: {
         const VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *features = (const void *) ext;
         if (features->nonSeamlessCubeMap)
            all_features->nonSeamlessCubeMap = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES: {
         const VkPhysicalDevicePipelineRobustnessFeatures *features = (const void *) ext;
         if (features->pipelineRobustness)
            all_features->pipelineRobustness = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MULTIPLE_WAIT_QUEUES_FEATURES_QCOM: {
         const VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM *features = (const void *) ext;
         if (features->shaderMultipleWaitQueues)
            all_features->shaderMultipleWaitQueues = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM: {
         const VkPhysicalDeviceImageProcessingFeaturesQCOM *features = (const void *) ext;
         if (features->textureSampleWeighted)
            all_features->textureSampleWeighted = true;
         if (features->textureBoxFilter)
            all_features->textureBoxFilter = true;
         if (features->textureBlockMatch)
            all_features->textureBlockMatch = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM: {
         const VkPhysicalDeviceTilePropertiesFeaturesQCOM *features = (const void *) ext;
         if (features->tileProperties)
            all_features->tileProperties = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC: {
         const VkPhysicalDeviceAmigoProfilingFeaturesSEC *features = (const void *) ext;
         if (features->amigoProfiling)
            all_features->amigoProfiling = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT: {
         const VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *features = (const void *) ext;
         if (features->attachmentFeedbackLoopLayout)
            all_features->attachmentFeedbackLoopLayout = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ADDRESS_BINDING_REPORT_FEATURES_EXT: {
         const VkPhysicalDeviceAddressBindingReportFeaturesEXT *features = (const void *) ext;
         if (features->reportAddressBinding)
            all_features->reportAddressBinding = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_FEATURES_NV: {
         const VkPhysicalDeviceOpticalFlowFeaturesNV *features = (const void *) ext;
         if (features->opticalFlow)
            all_features->opticalFlow = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_EXT: {
         const VkPhysicalDeviceFaultFeaturesEXT *features = (const void *) ext;
         if (features->deviceFault)
            all_features->deviceFaultEXT = true;
         if (features->deviceFaultVendorBinary)
            all_features->deviceFaultVendorBinaryEXT = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FAULT_FEATURES_KHR: {
         const VkPhysicalDeviceFaultFeaturesKHR *features = (const void *) ext;
         if (features->deviceFault)
            all_features->deviceFault = true;
         if (features->deviceFaultVendorBinary)
            all_features->deviceFaultVendorBinary = true;
         if (features->deviceFaultReportMasked)
            all_features->deviceFaultReportMasked = true;
         if (features->deviceFaultDeviceLostOnMasked)
            all_features->deviceFaultDeviceLostOnMasked = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_LIBRARY_GROUP_HANDLES_FEATURES_EXT: {
         const VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT *features = (const void *) ext;
         if (features->pipelineLibraryGroupHandles)
            all_features->pipelineLibraryGroupHandles = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_FEATURES_ARM: {
         const VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM *features = (const void *) ext;
         if (features->shaderCoreBuiltins)
            all_features->shaderCoreBuiltins = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAME_BOUNDARY_FEATURES_EXT: {
         const VkPhysicalDeviceFrameBoundaryFeaturesEXT *features = (const void *) ext;
         if (features->frameBoundary)
            all_features->frameBoundary = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT: {
         const VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT *features = (const void *) ext;
         if (features->dynamicRenderingUnusedAttachments)
            all_features->dynamicRenderingUnusedAttachments = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INTERNALLY_SYNCHRONIZED_QUEUES_FEATURES_KHR: {
         const VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR *features = (const void *) ext;
         if (features->internallySynchronizedQueues)
            all_features->internallySynchronizedQueues = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_KHR: {
         const VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR *features = (const void *) ext;
         if (features->swapchainMaintenance1)
            all_features->swapchainMaintenance1 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT: {
         const VkPhysicalDeviceDepthBiasControlFeaturesEXT *features = (const void *) ext;
         if (features->depthBiasControl)
            all_features->depthBiasControl = true;
         if (features->leastRepresentableValueForceUnormRepresentation)
            all_features->leastRepresentableValueForceUnormRepresentation = true;
         if (features->floatRepresentation)
            all_features->floatRepresentation = true;
         if (features->depthBiasExact)
            all_features->depthBiasExact = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_EXT: {
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT *features = (const void *) ext;
         if (features->rayTracingInvocationReorder)
            all_features->rayTracingInvocationReorder = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_INVOCATION_REORDER_FEATURES_NV: {
         const VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV *features = (const void *) ext;
         if (features->rayTracingInvocationReorder)
            all_features->rayTracingInvocationReorderNV = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_SPARSE_ADDRESS_SPACE_FEATURES_NV: {
         const VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV *features = (const void *) ext;
         if (features->extendedSparseAddressSpace)
            all_features->extendedSparseAddressSpace = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_VIEWPORTS_FEATURES_QCOM: {
         const VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM *features = (const void *) ext;
         if (features->multiviewPerViewViewports)
            all_features->multiviewPerViewViewports = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_POSITION_FETCH_FEATURES_KHR: {
         const VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR *features = (const void *) ext;
         if (features->rayTracingPositionFetch)
            all_features->rayTracingPositionFetch = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_RENDER_AREAS_FEATURES_QCOM: {
         const VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM *features = (const void *) ext;
         if (features->multiviewPerViewRenderAreas)
            all_features->multiviewPerViewRenderAreas = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT: {
         const VkPhysicalDeviceShaderObjectFeaturesEXT *features = (const void *) ext;
         if (features->shaderObject)
            all_features->shaderObject = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TILE_IMAGE_FEATURES_EXT: {
         const VkPhysicalDeviceShaderTileImageFeaturesEXT *features = (const void *) ext;
         if (features->shaderTileImageColorReadAccess)
            all_features->shaderTileImageColorReadAccess = true;
         if (features->shaderTileImageDepthReadAccess)
            all_features->shaderTileImageDepthReadAccess = true;
         if (features->shaderTileImageStencilReadAccess)
            all_features->shaderTileImageStencilReadAccess = true;
         break;
      }

#ifdef VK_USE_PLATFORM_SCREEN_QNX
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX: {
         const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX *features = (const void *) ext;
         if (features->screenBufferImport)
            all_features->screenBufferImport = true;
         break;
      }

#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR: {
         const VkPhysicalDeviceCooperativeMatrixFeaturesKHR *features = (const void *) ext;
         if (features->cooperativeMatrix)
            all_features->cooperativeMatrix = true;
         if (features->cooperativeMatrixRobustBufferAccess)
            all_features->cooperativeMatrixRobustBufferAccess = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_CONVERSION_FEATURES_QCOM: {
         const VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM *features = (const void *) ext;
         if (features->cooperativeMatrixConversion)
            all_features->cooperativeMatrixConversion = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ANTI_LAG_FEATURES_AMD: {
         const VkPhysicalDeviceAntiLagFeaturesAMD *features = (const void *) ext;
         if (features->antiLag)
            all_features->antiLag = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_MEMORY_HEAP_FEATURES_QCOM: {
         const VkPhysicalDeviceTileMemoryHeapFeaturesQCOM *features = (const void *) ext;
         if (features->tileMemoryHeap)
            all_features->tileMemoryHeap = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_CLAMP_FEATURES_QCOM: {
         const VkPhysicalDeviceCubicClampFeaturesQCOM *features = (const void *) ext;
         if (features->cubicRangeClamp)
            all_features->cubicRangeClamp = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_DEGAMMA_FEATURES_QCOM: {
         const VkPhysicalDeviceYcbcrDegammaFeaturesQCOM *features = (const void *) ext;
         if (features->ycbcrDegamma)
            all_features->ycbcrDegamma = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUBIC_WEIGHTS_FEATURES_QCOM: {
         const VkPhysicalDeviceCubicWeightsFeaturesQCOM *features = (const void *) ext;
         if (features->selectableCubicWeights)
            all_features->selectableCubicWeights = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_2_FEATURES_QCOM: {
         const VkPhysicalDeviceImageProcessing2FeaturesQCOM *features = (const void *) ext;
         if (features->textureBlockMatch2)
            all_features->textureBlockMatch2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_3_FEATURES_QCOM: {
         const VkPhysicalDeviceImageProcessing3FeaturesQCOM *features = (const void *) ext;
         if (features->imageGatherLinear)
            all_features->imageGatherLinear = true;
         if (features->imageGatherExtendedModes)
            all_features->imageGatherExtendedModes = true;
         if (features->blockMatchExtendedClampToEdge)
            all_features->blockMatchExtendedClampToEdge = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_POOL_OVERALLOCATION_FEATURES_NV: {
         const VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV *features = (const void *) ext;
         if (features->descriptorPoolOverallocation)
            all_features->descriptorPoolOverallocation = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PER_STAGE_DESCRIPTOR_SET_FEATURES_NV: {
         const VkPhysicalDevicePerStageDescriptorSetFeaturesNV *features = (const void *) ext;
         if (features->perStageDescriptorSet)
            all_features->perStageDescriptorSet = true;
         if (features->dynamicPipelineLayout)
            all_features->dynamicPipelineLayout = true;
         break;
      }

#ifdef VK_USE_PLATFORM_ANDROID_KHR
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FORMAT_RESOLVE_FEATURES_ANDROID: {
         const VkPhysicalDeviceExternalFormatResolveFeaturesANDROID *features = (const void *) ext;
         if (features->externalFormatResolve)
            all_features->externalFormatResolve = true;
         break;
      }

#endif
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCHEDULING_CONTROLS_FEATURES_ARM: {
         const VkPhysicalDeviceSchedulingControlsFeaturesARM *features = (const void *) ext;
         if (features->schedulingControls)
            all_features->schedulingControls = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RELAXED_LINE_RASTERIZATION_FEATURES_IMG: {
         const VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG *features = (const void *) ext;
         if (features->relaxedLineRasterization)
            all_features->relaxedLineRasterization = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RENDER_PASS_STRIPED_FEATURES_ARM: {
         const VkPhysicalDeviceRenderPassStripedFeaturesARM *features = (const void *) ext;
         if (features->renderPassStriped)
            all_features->renderPassStriped = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_OPACITY_MICROMAP_FEATURES_ARM: {
         const VkPhysicalDevicePipelineOpacityMicromapFeaturesARM *features = (const void *) ext;
         if (features->pipelineOpacityMicromap)
            all_features->pipelineOpacityMicromap = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MAXIMAL_RECONVERGENCE_FEATURES_KHR: {
         const VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR *features = (const void *) ext;
         if (features->shaderMaximalReconvergence)
            all_features->shaderMaximalReconvergence = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES: {
         const VkPhysicalDeviceShaderSubgroupRotateFeatures *features = (const void *) ext;
         if (features->shaderSubgroupRotate)
            all_features->shaderSubgroupRotate = true;
         if (features->shaderSubgroupRotateClustered)
            all_features->shaderSubgroupRotateClustered = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES: {
         const VkPhysicalDeviceShaderExpectAssumeFeatures *features = (const void *) ext;
         if (features->shaderExpectAssume)
            all_features->shaderExpectAssume = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES: {
         const VkPhysicalDeviceShaderFloatControls2Features *features = (const void *) ext;
         if (features->shaderFloatControls2)
            all_features->shaderFloatControls2 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES: {
         const VkPhysicalDeviceDynamicRenderingLocalReadFeatures *features = (const void *) ext;
         if (features->dynamicRenderingLocalRead)
            all_features->dynamicRenderingLocalRead = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_QUAD_CONTROL_FEATURES_KHR: {
         const VkPhysicalDeviceShaderQuadControlFeaturesKHR *features = (const void *) ext;
         if (features->shaderQuadControl)
            all_features->shaderQuadControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT16_VECTOR_FEATURES_NV: {
         const VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV *features = (const void *) ext;
         if (features->shaderFloat16VectorAtomics)
            all_features->shaderFloat16VectorAtomics = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAP_MEMORY_PLACED_FEATURES_EXT: {
         const VkPhysicalDeviceMapMemoryPlacedFeaturesEXT *features = (const void *) ext;
         if (features->memoryMapPlaced)
            all_features->memoryMapPlaced = true;
         if (features->memoryMapRangePlaced)
            all_features->memoryMapRangePlaced = true;
         if (features->memoryUnmapReserve)
            all_features->memoryUnmapReserve = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_BFLOAT16_FEATURES_KHR: {
         const VkPhysicalDeviceShaderBfloat16FeaturesKHR *features = (const void *) ext;
         if (features->shaderBFloat16Type)
            all_features->shaderBFloat16Type = true;
         if (features->shaderBFloat16DotProduct)
            all_features->shaderBFloat16DotProduct = true;
         if (features->shaderBFloat16CooperativeMatrix)
            all_features->shaderBFloat16CooperativeMatrix = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAW_ACCESS_CHAINS_FEATURES_NV: {
         const VkPhysicalDeviceRawAccessChainsFeaturesNV *features = (const void *) ext;
         if (features->shaderRawAccessChains)
            all_features->shaderRawAccessChains = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMMAND_BUFFER_INHERITANCE_FEATURES_NV: {
         const VkPhysicalDeviceCommandBufferInheritanceFeaturesNV *features = (const void *) ext;
         if (features->commandBufferInheritance)
            all_features->commandBufferInheritance = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ALIGNMENT_CONTROL_FEATURES_MESA: {
         const VkPhysicalDeviceImageAlignmentControlFeaturesMESA *features = (const void *) ext;
         if (features->imageAlignmentControl)
            all_features->imageAlignmentControl = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_REPLICATED_COMPOSITES_FEATURES_EXT: {
         const VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT *features = (const void *) ext;
         if (features->shaderReplicatedComposites)
            all_features->shaderReplicatedComposites = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_MODE_FIFO_LATEST_READY_FEATURES_KHR: {
         const VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR *features = (const void *) ext;
         if (features->presentModeFifoLatestReady)
            all_features->presentModeFifoLatestReady = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_2_FEATURES_NV: {
         const VkPhysicalDeviceCooperativeMatrix2FeaturesNV *features = (const void *) ext;
         if (features->cooperativeMatrixWorkgroupScope)
            all_features->cooperativeMatrixWorkgroupScope = true;
         if (features->cooperativeMatrixFlexibleDimensions)
            all_features->cooperativeMatrixFlexibleDimensions = true;
         if (features->cooperativeMatrixReductions)
            all_features->cooperativeMatrixReductions = true;
         if (features->cooperativeMatrixConversions)
            all_features->cooperativeMatrixConversions = true;
         if (features->cooperativeMatrixPerElementOperations)
            all_features->cooperativeMatrixPerElementOperations = true;
         if (features->cooperativeMatrixTensorAddressing)
            all_features->cooperativeMatrixTensorAddressing = true;
         if (features->cooperativeMatrixBlockLoads)
            all_features->cooperativeMatrixBlockLoads = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_DECODE_VECTOR_FEATURES_NV: {
         const VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV *features = (const void *) ext;
         if (features->cooperativeMatrixDecodeVector)
            all_features->cooperativeMatrixDecodeVector = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HDR_VIVID_FEATURES_HUAWEI: {
         const VkPhysicalDeviceHdrVividFeaturesHUAWEI *features = (const void *) ext;
         if (features->hdrVivid)
            all_features->hdrVivid = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_ROBUSTNESS_FEATURES_EXT: {
         const VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT *features = (const void *) ext;
         if (features->vertexAttributeRobustness)
            all_features->vertexAttributeRobustness = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLAMP_ZERO_ONE_FEATURES_KHR: {
         const VkPhysicalDeviceDepthClampZeroOneFeaturesKHR *features = (const void *) ext;
         if (features->depthClampZeroOne)
            all_features->depthClampZeroOne = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_VECTOR_FEATURES_NV: {
         const VkPhysicalDeviceCooperativeVectorFeaturesNV *features = (const void *) ext;
         if (features->cooperativeVector)
            all_features->cooperativeVector = true;
         if (features->cooperativeVectorTraining)
            all_features->cooperativeVectorTraining = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_SHADING_FEATURES_QCOM: {
         const VkPhysicalDeviceTileShadingFeaturesQCOM *features = (const void *) ext;
         if (features->tileShading)
            all_features->tileShading = true;
         if (features->tileShadingFragmentStage)
            all_features->tileShadingFragmentStage = true;
         if (features->tileShadingColorAttachments)
            all_features->tileShadingColorAttachments = true;
         if (features->tileShadingDepthAttachments)
            all_features->tileShadingDepthAttachments = true;
         if (features->tileShadingStencilAttachments)
            all_features->tileShadingStencilAttachments = true;
         if (features->tileShadingInputAttachments)
            all_features->tileShadingInputAttachments = true;
         if (features->tileShadingSampledAttachments)
            all_features->tileShadingSampledAttachments = true;
         if (features->tileShadingPerTileDraw)
            all_features->tileShadingPerTileDraw = true;
         if (features->tileShadingPerTileDispatch)
            all_features->tileShadingPerTileDispatch = true;
         if (features->tileShadingDispatchTile)
            all_features->tileShadingDispatchTile = true;
         if (features->tileShadingApron)
            all_features->tileShadingApron = true;
         if (features->tileShadingAnisotropicApron)
            all_features->tileShadingAnisotropicApron = true;
         if (features->tileShadingAtomicOps)
            all_features->tileShadingAtomicOps = true;
         if (features->tileShadingImageProcessing)
            all_features->tileShadingImageProcessing = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_LAYERED_FEATURES_VALVE: {
         const VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE *features = (const void *) ext;
         if (features->fragmentDensityMapLayered)
            all_features->fragmentDensityMapLayered = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_METERING_FEATURES_NV: {
         const VkPhysicalDevicePresentMeteringFeaturesNV *features = (const void *) ext;
         if (features->presentMetering)
            all_features->presentMetering = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNIFORM_BUFFER_UNSIZED_ARRAY_FEATURES_EXT: {
         const VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT *features = (const void *) ext;
         if (features->shaderUniformBufferUnsizedArray)
            all_features->shaderUniformBufferUnsizedArray = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MIXED_FLOAT_DOT_PRODUCT_FEATURES_VALVE: {
         const VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE *features = (const void *) ext;
         if (features->shaderMixedFloatDotProductFloat16AccFloat32)
            all_features->shaderMixedFloatDotProductFloat16AccFloat32 = true;
         if (features->shaderMixedFloatDotProductFloat16AccFloat16)
            all_features->shaderMixedFloatDotProductFloat16AccFloat16 = true;
         if (features->shaderMixedFloatDotProductBFloat16Acc)
            all_features->shaderMixedFloatDotProductBFloat16Acc = true;
         if (features->shaderMixedFloatDotProductFloat8AccFloat32)
            all_features->shaderMixedFloatDotProductFloat8AccFloat32 = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_RESTART_INDEX_FEATURES_EXT: {
         const VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT *features = (const void *) ext;
         if (features->primitiveRestartIndex)
            all_features->primitiveRestartIndex = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FORMAT_PACK_FEATURES_ARM: {
         const VkPhysicalDeviceFormatPackFeaturesARM *features = (const void *) ext;
         if (features->formatPack)
            all_features->formatPack = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_THROTTLE_HINT_FEATURES_SEC: {
         const VkPhysicalDeviceThrottleHintFeaturesSEC *features = (const void *) ext;
         if (features->throttleHint)
            all_features->throttleHint = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TENSOR_FEATURES_ARM: {
         const VkPhysicalDeviceTensorFeaturesARM *features = (const void *) ext;
         if (features->tensorNonPacked)
            all_features->tensorNonPacked = true;
         if (features->shaderTensorAccess)
            all_features->shaderTensorAccess = true;
         if (features->shaderStorageTensorArrayDynamicIndexing)
            all_features->shaderStorageTensorArrayDynamicIndexing = true;
         if (features->shaderStorageTensorArrayNonUniformIndexing)
            all_features->shaderStorageTensorArrayNonUniformIndexing = true;
         if (features->descriptorBindingStorageTensorUpdateAfterBind)
            all_features->descriptorBindingStorageTensorUpdateAfterBind = true;
         if (features->tensors)
            all_features->tensors = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_TENSOR_FEATURES_ARM: {
         const VkPhysicalDeviceDescriptorBufferTensorFeaturesARM *features = (const void *) ext;
         if (features->descriptorBufferTensorDescriptors)
            all_features->descriptorBufferTensorDescriptors = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT8_FEATURES_EXT: {
         const VkPhysicalDeviceShaderFloat8FeaturesEXT *features = (const void *) ext;
         if (features->shaderFloat8)
            all_features->shaderFloat8 = true;
         if (features->shaderFloat8CooperativeMatrix)
            all_features->shaderFloat8CooperativeMatrix = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_FEATURES_ARM: {
         const VkPhysicalDeviceDataGraphFeaturesARM *features = (const void *) ext;
         if (features->dataGraph)
            all_features->dataGraph = true;
         if (features->dataGraphUpdateAfterBind)
            all_features->dataGraphUpdateAfterBind = true;
         if (features->dataGraphSpecializationConstants)
            all_features->dataGraphSpecializationConstants = true;
         if (features->dataGraphDescriptorBuffer)
            all_features->dataGraphDescriptorBuffer = true;
         if (features->dataGraphShaderModule)
            all_features->dataGraphShaderModule = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CACHE_INCREMENTAL_MODE_FEATURES_SEC: {
         const VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC *features = (const void *) ext;
         if (features->pipelineCacheIncrementalMode)
            all_features->pipelineCacheIncrementalMode = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_MODEL_FEATURES_QCOM: {
         const VkPhysicalDeviceDataGraphModelFeaturesQCOM *features = (const void *) ext;
         if (features->dataGraphModel)
            all_features->dataGraphModel = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR: {
         const VkPhysicalDeviceShaderUntypedPointersFeaturesKHR *features = (const void *) ext;
         if (features->shaderUntypedPointers)
            all_features->shaderUntypedPointers = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_ENCODE_RGB_CONVERSION_FEATURES_VALVE: {
         const VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE *features = (const void *) ext;
         if (features->videoEncodeRgbConversion)
            all_features->videoEncodeRgbConversion = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_64_BIT_INDEXING_FEATURES_EXT: {
         const VkPhysicalDeviceShader64BitIndexingFeaturesEXT *features = (const void *) ext;
         if (features->shader64BitIndexing)
            all_features->shader64BitIndexing = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_QUEUE_PERF_HINT_FEATURES_QCOM: {
         const VkPhysicalDeviceQueuePerfHintFeaturesQCOM *features = (const void *) ext;
         if (features->queuePerfHint)
            all_features->queuePerfHint = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_COUNTERS_BY_REGION_FEATURES_ARM: {
         const VkPhysicalDevicePerformanceCountersByRegionFeaturesARM *features = (const void *) ext;
         if (features->performanceCountersByRegion)
            all_features->performanceCountersByRegion = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_OCCUPANCY_PRIORITY_FEATURES_NV: {
         const VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV *features = (const void *) ext;
         if (features->computeOccupancyPriority)
            all_features->computeOccupancyPriority = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_LONG_VECTOR_FEATURES_EXT: {
         const VkPhysicalDeviceShaderLongVectorFeaturesEXT *features = (const void *) ext;
         if (features->longVector)
            all_features->longVector = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_3D_FEATURES_EXT: {
         const VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT *features = (const void *) ext;
         if (features->textureCompressionASTC_3D)
            all_features->textureCompressionASTC_3D = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_PARTITIONED_FEATURES_EXT: {
         const VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT *features = (const void *) ext;
         if (features->shaderSubgroupPartitioned)
            all_features->shaderSubgroupPartitioned = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SPLIT_BARRIER_FEATURES_EXT: {
         const VkPhysicalDeviceShaderSplitBarrierFeaturesEXT *features = (const void *) ext;
         if (features->shaderSplitBarrier)
            all_features->shaderSplitBarrier = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT: {
         const VkPhysicalDeviceDescriptorHeapFeaturesEXT *features = (const void *) ext;
         if (features->descriptorHeap)
            all_features->descriptorHeap = true;
         if (features->descriptorHeapCaptureReplay)
            all_features->descriptorHeapCaptureReplay = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INSTRUMENTATION_FEATURES_ARM: {
         const VkPhysicalDeviceShaderInstrumentationFeaturesARM *features = (const void *) ext;
         if (features->shaderInstrumentation)
            all_features->shaderInstrumentation = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_ADDRESS_COMMANDS_FEATURES_KHR: {
         const VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR *features = (const void *) ext;
         if (features->deviceAddressCommands)
            all_features->deviceAddressCommands = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CONSTANT_DATA_FEATURES_KHR: {
         const VkPhysicalDeviceShaderConstantDataFeaturesKHR *features = (const void *) ext;
         if (features->shaderConstantData)
            all_features->shaderConstantData = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ABORT_FEATURES_KHR: {
         const VkPhysicalDeviceShaderAbortFeaturesKHR *features = (const void *) ext;
         if (features->shaderAbort)
            all_features->shaderAbort = true;
         break;
      }

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DATA_GRAPH_OPTICAL_FLOW_FEATURES_ARM: {
         const VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM *features = (const void *) ext;
         if (features->dataGraphOpticalFlow)
            all_features->dataGraphOpticalFlow = true;
         break;
      }

      default:
         break;
      }
   }
}

void
vk_set_physical_device_features_1_0(struct vk_features *all_features,
                                    const VkPhysicalDeviceFeatures *pFeatures)
{
   if (pFeatures->robustBufferAccess)
      all_features->robustBufferAccess = true;
   if (pFeatures->fullDrawIndexUint32)
      all_features->fullDrawIndexUint32 = true;
   if (pFeatures->imageCubeArray)
      all_features->imageCubeArray = true;
   if (pFeatures->independentBlend)
      all_features->independentBlend = true;
   if (pFeatures->geometryShader)
      all_features->geometryShader = true;
   if (pFeatures->tessellationShader)
      all_features->tessellationShader = true;
   if (pFeatures->sampleRateShading)
      all_features->sampleRateShading = true;
   if (pFeatures->dualSrcBlend)
      all_features->dualSrcBlend = true;
   if (pFeatures->logicOp)
      all_features->logicOp = true;
   if (pFeatures->multiDrawIndirect)
      all_features->multiDrawIndirect = true;
   if (pFeatures->drawIndirectFirstInstance)
      all_features->drawIndirectFirstInstance = true;
   if (pFeatures->depthClamp)
      all_features->depthClamp = true;
   if (pFeatures->depthBiasClamp)
      all_features->depthBiasClamp = true;
   if (pFeatures->fillModeNonSolid)
      all_features->fillModeNonSolid = true;
   if (pFeatures->depthBounds)
      all_features->depthBounds = true;
   if (pFeatures->wideLines)
      all_features->wideLines = true;
   if (pFeatures->largePoints)
      all_features->largePoints = true;
   if (pFeatures->alphaToOne)
      all_features->alphaToOne = true;
   if (pFeatures->multiViewport)
      all_features->multiViewport = true;
   if (pFeatures->samplerAnisotropy)
      all_features->samplerAnisotropy = true;
   if (pFeatures->textureCompressionETC2)
      all_features->textureCompressionETC2 = true;
   if (pFeatures->textureCompressionASTC_LDR)
      all_features->textureCompressionASTC_LDR = true;
   if (pFeatures->textureCompressionBC)
      all_features->textureCompressionBC = true;
   if (pFeatures->occlusionQueryPrecise)
      all_features->occlusionQueryPrecise = true;
   if (pFeatures->pipelineStatisticsQuery)
      all_features->pipelineStatisticsQuery = true;
   if (pFeatures->vertexPipelineStoresAndAtomics)
      all_features->vertexPipelineStoresAndAtomics = true;
   if (pFeatures->fragmentStoresAndAtomics)
      all_features->fragmentStoresAndAtomics = true;
   if (pFeatures->shaderTessellationAndGeometryPointSize)
      all_features->shaderTessellationAndGeometryPointSize = true;
   if (pFeatures->shaderImageGatherExtended)
      all_features->shaderImageGatherExtended = true;
   if (pFeatures->shaderStorageImageExtendedFormats)
      all_features->shaderStorageImageExtendedFormats = true;
   if (pFeatures->shaderStorageImageMultisample)
      all_features->shaderStorageImageMultisample = true;
   if (pFeatures->shaderStorageImageReadWithoutFormat)
      all_features->shaderStorageImageReadWithoutFormat = true;
   if (pFeatures->shaderStorageImageWriteWithoutFormat)
      all_features->shaderStorageImageWriteWithoutFormat = true;
   if (pFeatures->shaderUniformBufferArrayDynamicIndexing)
      all_features->shaderUniformBufferArrayDynamicIndexing = true;
   if (pFeatures->shaderSampledImageArrayDynamicIndexing)
      all_features->shaderSampledImageArrayDynamicIndexing = true;
   if (pFeatures->shaderStorageBufferArrayDynamicIndexing)
      all_features->shaderStorageBufferArrayDynamicIndexing = true;
   if (pFeatures->shaderStorageImageArrayDynamicIndexing)
      all_features->shaderStorageImageArrayDynamicIndexing = true;
   if (pFeatures->shaderClipDistance)
      all_features->shaderClipDistance = true;
   if (pFeatures->shaderCullDistance)
      all_features->shaderCullDistance = true;
   if (pFeatures->shaderFloat64)
      all_features->shaderFloat64 = true;
   if (pFeatures->shaderInt64)
      all_features->shaderInt64 = true;
   if (pFeatures->shaderInt16)
      all_features->shaderInt16 = true;
   if (pFeatures->shaderResourceResidency)
      all_features->shaderResourceResidency = true;
   if (pFeatures->shaderResourceMinLod)
      all_features->shaderResourceMinLod = true;
   if (pFeatures->sparseBinding)
      all_features->sparseBinding = true;
   if (pFeatures->sparseResidencyBuffer)
      all_features->sparseResidencyBuffer = true;
   if (pFeatures->sparseResidencyImage2D)
      all_features->sparseResidencyImage2D = true;
   if (pFeatures->sparseResidencyImage3D)
      all_features->sparseResidencyImage3D = true;
   if (pFeatures->sparseResidency2Samples)
      all_features->sparseResidency2Samples = true;
   if (pFeatures->sparseResidency4Samples)
      all_features->sparseResidency4Samples = true;
   if (pFeatures->sparseResidency8Samples)
      all_features->sparseResidency8Samples = true;
   if (pFeatures->sparseResidency16Samples)
      all_features->sparseResidency16Samples = true;
   if (pFeatures->sparseResidencyAliased)
      all_features->sparseResidencyAliased = true;
   if (pFeatures->variableMultisampleRate)
      all_features->variableMultisampleRate = true;
   if (pFeatures->inheritedQueries)
      all_features->inheritedQueries = true;
}
