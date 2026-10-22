/*
 * Copyright 2020 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* This file generated from vk_dispatch_table_gen.py, don't edit directly. */

#include "vk_dispatch_table.h"

#include "util/macros.h"
#include "string.h"




void
vk_instance_dispatch_table_load(struct vk_instance_dispatch_table *table,
                               PFN_vkGetInstanceProcAddr gpa,
                               VkInstance obj)
{
    table->GetInstanceProcAddr = gpa;
    table->CreateInstance = (PFN_vkCreateInstance) gpa(obj, "vkCreateInstance");
    table->DestroyInstance = (PFN_vkDestroyInstance) gpa(obj, "vkDestroyInstance");
    table->EnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) gpa(obj, "vkEnumeratePhysicalDevices");
    table->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) gpa(obj, "vkGetInstanceProcAddr");
    table->EnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) gpa(obj, "vkEnumerateInstanceVersion");
    table->EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) gpa(obj, "vkEnumerateInstanceLayerProperties");
    table->EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) gpa(obj, "vkEnumerateInstanceExtensionProperties");
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->CreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR) gpa(obj, "vkCreateAndroidSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_OHOS
    table->CreateSurfaceOHOS = (PFN_vkCreateSurfaceOHOS) gpa(obj, "vkCreateSurfaceOHOS");
#endif
    table->CreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR) gpa(obj, "vkCreateDisplayPlaneSurfaceKHR");
    table->DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) gpa(obj, "vkDestroySurfaceKHR");
#ifdef VK_USE_PLATFORM_VI_NN
    table->CreateViSurfaceNN = (PFN_vkCreateViSurfaceNN) gpa(obj, "vkCreateViSurfaceNN");
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    table->CreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) gpa(obj, "vkCreateWaylandSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_UBM_SEC
    table->CreateUbmSurfaceSEC = (PFN_vkCreateUbmSurfaceSEC) gpa(obj, "vkCreateUbmSurfaceSEC");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) gpa(obj, "vkCreateWin32SurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    table->CreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) gpa(obj, "vkCreateXlibSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    table->CreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR) gpa(obj, "vkCreateXcbSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    table->CreateDirectFBSurfaceEXT = (PFN_vkCreateDirectFBSurfaceEXT) gpa(obj, "vkCreateDirectFBSurfaceEXT");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->CreateImagePipeSurfaceFUCHSIA = (PFN_vkCreateImagePipeSurfaceFUCHSIA) gpa(obj, "vkCreateImagePipeSurfaceFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_GGP
    table->CreateStreamDescriptorSurfaceGGP = (PFN_vkCreateStreamDescriptorSurfaceGGP) gpa(obj, "vkCreateStreamDescriptorSurfaceGGP");
#endif
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->CreateScreenSurfaceQNX = (PFN_vkCreateScreenSurfaceQNX) gpa(obj, "vkCreateScreenSurfaceQNX");
#endif
    table->CreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) gpa(obj, "vkCreateDebugReportCallbackEXT");
    table->DestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) gpa(obj, "vkDestroyDebugReportCallbackEXT");
    table->DebugReportMessageEXT = (PFN_vkDebugReportMessageEXT) gpa(obj, "vkDebugReportMessageEXT");
    table->EnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) gpa(obj, "vkEnumeratePhysicalDeviceGroups");
    if (table->EnumeratePhysicalDeviceGroups == NULL) {
        table->EnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) gpa(obj, "vkEnumeratePhysicalDeviceGroupsKHR");
    }
    #ifdef VK_USE_PLATFORM_IOS_MVK
    table->CreateIOSSurfaceMVK = (PFN_vkCreateIOSSurfaceMVK) gpa(obj, "vkCreateIOSSurfaceMVK");
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    table->CreateMacOSSurfaceMVK = (PFN_vkCreateMacOSSurfaceMVK) gpa(obj, "vkCreateMacOSSurfaceMVK");
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->CreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT) gpa(obj, "vkCreateMetalSurfaceEXT");
#endif
    table->CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) gpa(obj, "vkCreateDebugUtilsMessengerEXT");
    table->DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) gpa(obj, "vkDestroyDebugUtilsMessengerEXT");
    table->SubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT) gpa(obj, "vkSubmitDebugUtilsMessageEXT");
    table->CreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT) gpa(obj, "vkCreateHeadlessSurfaceEXT");
#ifdef VK_USE_PLATFORM_PLAN9_9FRONT
    table->CreatePlan9SurfacePLAN9 = (PFN_vkCreatePlan9SurfacePLAN9) gpa(obj, "vkCreatePlan9SurfacePLAN9");
#endif
    table->GetExternalComputeQueueDataNV = (PFN_vkGetExternalComputeQueueDataNV) gpa(obj, "vkGetExternalComputeQueueDataNV");
}



void
vk_physical_device_dispatch_table_load(struct vk_physical_device_dispatch_table *table,
                               PFN_vkGetInstanceProcAddr gpa,
                               VkInstance obj)
{
    table->GetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) gpa(obj, "vkGetPhysicalDeviceProperties");
    table->GetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties");
    table->GetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) gpa(obj, "vkGetPhysicalDeviceMemoryProperties");
    table->GetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures) gpa(obj, "vkGetPhysicalDeviceFeatures");
    table->GetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties) gpa(obj, "vkGetPhysicalDeviceFormatProperties");
    table->GetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties");
    table->CreateDevice = (PFN_vkCreateDevice) gpa(obj, "vkCreateDevice");
    table->EnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties) gpa(obj, "vkEnumerateDeviceLayerProperties");
    table->EnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) gpa(obj, "vkEnumerateDeviceExtensionProperties");
    table->GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties");
    table->GetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceDisplayPropertiesKHR");
    table->GetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR) gpa(obj, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    table->GetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR) gpa(obj, "vkGetDisplayPlaneSupportedDisplaysKHR");
    table->GetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR) gpa(obj, "vkGetDisplayModePropertiesKHR");
    table->CreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR) gpa(obj, "vkCreateDisplayModeKHR");
    table->GetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR) gpa(obj, "vkGetDisplayPlaneCapabilitiesKHR");
    table->GetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceSupportKHR");
    table->GetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    table->GetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    table->GetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) gpa(obj, "vkGetPhysicalDeviceSurfacePresentModesKHR");
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    table->GetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_UBM_SEC
    table->GetPhysicalDeviceUbmPresentationSupportSEC = (PFN_vkGetPhysicalDeviceUbmPresentationSupportSEC) gpa(obj, "vkGetPhysicalDeviceUbmPresentationSupportSEC");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    table->GetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    table->GetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    table->GetPhysicalDeviceDirectFBPresentationSupportEXT = (PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT) gpa(obj, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
#endif
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->GetPhysicalDeviceScreenPresentationSupportQNX = (PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX) gpa(obj, "vkGetPhysicalDeviceScreenPresentationSupportQNX");
#endif
    table->GetPhysicalDeviceExternalImageFormatPropertiesNV = (PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV) gpa(obj, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    table->GetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) gpa(obj, "vkGetPhysicalDeviceFeatures2");
    if (table->GetPhysicalDeviceFeatures2 == NULL) {
        table->GetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) gpa(obj, "vkGetPhysicalDeviceFeatures2KHR");
    }
        table->GetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) gpa(obj, "vkGetPhysicalDeviceProperties2");
    if (table->GetPhysicalDeviceProperties2 == NULL) {
        table->GetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) gpa(obj, "vkGetPhysicalDeviceProperties2KHR");
    }
        table->GetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) gpa(obj, "vkGetPhysicalDeviceFormatProperties2");
    if (table->GetPhysicalDeviceFormatProperties2 == NULL) {
        table->GetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) gpa(obj, "vkGetPhysicalDeviceFormatProperties2KHR");
    }
        table->GetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties2");
    if (table->GetPhysicalDeviceImageFormatProperties2 == NULL) {
        table->GetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties2KHR");
    }
        table->GetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties2");
    if (table->GetPhysicalDeviceQueueFamilyProperties2 == NULL) {
        table->GetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    }
        table->GetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) gpa(obj, "vkGetPhysicalDeviceMemoryProperties2");
    if (table->GetPhysicalDeviceMemoryProperties2 == NULL) {
        table->GetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) gpa(obj, "vkGetPhysicalDeviceMemoryProperties2KHR");
    }
        table->GetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    if (table->GetPhysicalDeviceSparseImageFormatProperties2 == NULL) {
        table->GetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
    }
        table->GetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) gpa(obj, "vkGetPhysicalDeviceExternalBufferProperties");
    if (table->GetPhysicalDeviceExternalBufferProperties == NULL) {
        table->GetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) gpa(obj, "vkGetPhysicalDeviceExternalBufferPropertiesKHR");
    }
        table->GetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) gpa(obj, "vkGetPhysicalDeviceExternalSemaphoreProperties");
    if (table->GetPhysicalDeviceExternalSemaphoreProperties == NULL) {
        table->GetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) gpa(obj, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
    }
        table->GetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) gpa(obj, "vkGetPhysicalDeviceExternalFenceProperties");
    if (table->GetPhysicalDeviceExternalFenceProperties == NULL) {
        table->GetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) gpa(obj, "vkGetPhysicalDeviceExternalFencePropertiesKHR");
    }
        table->ReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT) gpa(obj, "vkReleaseDisplayEXT");
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    table->AcquireXlibDisplayEXT = (PFN_vkAcquireXlibDisplayEXT) gpa(obj, "vkAcquireXlibDisplayEXT");
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    table->GetRandROutputDisplayEXT = (PFN_vkGetRandROutputDisplayEXT) gpa(obj, "vkGetRandROutputDisplayEXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->AcquireWinrtDisplayNV = (PFN_vkAcquireWinrtDisplayNV) gpa(obj, "vkAcquireWinrtDisplayNV");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetWinrtDisplayNV = (PFN_vkGetWinrtDisplayNV) gpa(obj, "vkGetWinrtDisplayNV");
#endif
    table->GetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    table->GetPhysicalDevicePresentRectanglesKHR = (PFN_vkGetPhysicalDevicePresentRectanglesKHR) gpa(obj, "vkGetPhysicalDevicePresentRectanglesKHR");
    table->GetPhysicalDeviceMultisamplePropertiesEXT = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT) gpa(obj, "vkGetPhysicalDeviceMultisamplePropertiesEXT");
    table->GetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    table->GetPhysicalDeviceSurfaceFormats2KHR = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR) gpa(obj, "vkGetPhysicalDeviceSurfaceFormats2KHR");
    table->GetPhysicalDeviceDisplayProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayProperties2KHR) gpa(obj, "vkGetPhysicalDeviceDisplayProperties2KHR");
    table->GetPhysicalDeviceDisplayPlaneProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR) gpa(obj, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    table->GetDisplayModeProperties2KHR = (PFN_vkGetDisplayModeProperties2KHR) gpa(obj, "vkGetDisplayModeProperties2KHR");
    table->GetDisplayPlaneCapabilities2KHR = (PFN_vkGetDisplayPlaneCapabilities2KHR) gpa(obj, "vkGetDisplayPlaneCapabilities2KHR");
    table->GetPhysicalDeviceCalibrateableTimeDomainsKHR = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR) gpa(obj, "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR");
    if (table->GetPhysicalDeviceCalibrateableTimeDomainsKHR == NULL) {
        table->GetPhysicalDeviceCalibrateableTimeDomainsKHR = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR) gpa(obj, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    }
        table->GetPhysicalDeviceCooperativeMatrixPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetPhysicalDeviceSurfacePresentModes2EXT = (PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT) gpa(obj, "vkGetPhysicalDeviceSurfacePresentModes2EXT");
#endif
    table->EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR) gpa(obj, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    table->GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = (PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR) gpa(obj, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    table->GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = (PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV) gpa(obj, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    table->GetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties) gpa(obj, "vkGetPhysicalDeviceToolProperties");
    if (table->GetPhysicalDeviceToolProperties == NULL) {
        table->GetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties) gpa(obj, "vkGetPhysicalDeviceToolPropertiesEXT");
    }
        table->GetPhysicalDeviceFragmentShadingRatesKHR = (PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR) gpa(obj, "vkGetPhysicalDeviceFragmentShadingRatesKHR");
    table->GetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    table->GetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    table->GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    table->AcquireDrmDisplayEXT = (PFN_vkAcquireDrmDisplayEXT) gpa(obj, "vkAcquireDrmDisplayEXT");
    table->GetDrmDisplayEXT = (PFN_vkGetDrmDisplayEXT) gpa(obj, "vkGetDrmDisplayEXT");
    table->GetPhysicalDeviceOpticalFlowImageFormatsNV = (PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV) gpa(obj, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV");
    table->GetPhysicalDeviceCooperativeMatrixPropertiesKHR = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR");
    table->GetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV");
    table->GetPhysicalDeviceCooperativeVectorPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeVectorPropertiesNV");
    table->EnumeratePhysicalDeviceShaderInstrumentationMetricsARM = (PFN_vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM) gpa(obj, "vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM");
    table->GetPhysicalDeviceExternalTensorPropertiesARM = (PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM) gpa(obj, "vkGetPhysicalDeviceExternalTensorPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphPropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM");
    table->EnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM) gpa(obj, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM");
    table->GetPhysicalDeviceDescriptorSizeEXT = (PFN_vkGetPhysicalDeviceDescriptorSizeEXT) gpa(obj, "vkGetPhysicalDeviceDescriptorSizeEXT");
    table->GetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM");
}



void
vk_device_dispatch_table_load(struct vk_device_dispatch_table *table,
                               PFN_vkGetDeviceProcAddr gpa,
                               VkDevice obj)
{
    table->GetDeviceProcAddr = gpa;
    table->GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) gpa(obj, "vkGetDeviceProcAddr");
    table->DestroyDevice = (PFN_vkDestroyDevice) gpa(obj, "vkDestroyDevice");
    table->GetDeviceQueue = (PFN_vkGetDeviceQueue) gpa(obj, "vkGetDeviceQueue");
    table->QueueSubmit = (PFN_vkQueueSubmit) gpa(obj, "vkQueueSubmit");
    table->QueueWaitIdle = (PFN_vkQueueWaitIdle) gpa(obj, "vkQueueWaitIdle");
    table->DeviceWaitIdle = (PFN_vkDeviceWaitIdle) gpa(obj, "vkDeviceWaitIdle");
    table->AllocateMemory = (PFN_vkAllocateMemory) gpa(obj, "vkAllocateMemory");
    table->FreeMemory = (PFN_vkFreeMemory) gpa(obj, "vkFreeMemory");
    table->MapMemory = (PFN_vkMapMemory) gpa(obj, "vkMapMemory");
    table->UnmapMemory = (PFN_vkUnmapMemory) gpa(obj, "vkUnmapMemory");
    table->FlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges) gpa(obj, "vkFlushMappedMemoryRanges");
    table->InvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges) gpa(obj, "vkInvalidateMappedMemoryRanges");
    table->GetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment) gpa(obj, "vkGetDeviceMemoryCommitment");
    table->GetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) gpa(obj, "vkGetBufferMemoryRequirements");
    table->BindBufferMemory = (PFN_vkBindBufferMemory) gpa(obj, "vkBindBufferMemory");
    table->GetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements) gpa(obj, "vkGetImageMemoryRequirements");
    table->BindImageMemory = (PFN_vkBindImageMemory) gpa(obj, "vkBindImageMemory");
    table->GetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements) gpa(obj, "vkGetImageSparseMemoryRequirements");
    table->QueueBindSparse = (PFN_vkQueueBindSparse) gpa(obj, "vkQueueBindSparse");
    table->CreateFence = (PFN_vkCreateFence) gpa(obj, "vkCreateFence");
    table->DestroyFence = (PFN_vkDestroyFence) gpa(obj, "vkDestroyFence");
    table->ResetFences = (PFN_vkResetFences) gpa(obj, "vkResetFences");
    table->GetFenceStatus = (PFN_vkGetFenceStatus) gpa(obj, "vkGetFenceStatus");
    table->WaitForFences = (PFN_vkWaitForFences) gpa(obj, "vkWaitForFences");
    table->CreateSemaphore = (PFN_vkCreateSemaphore) gpa(obj, "vkCreateSemaphore");
    table->DestroySemaphore = (PFN_vkDestroySemaphore) gpa(obj, "vkDestroySemaphore");
    table->CreateEvent = (PFN_vkCreateEvent) gpa(obj, "vkCreateEvent");
    table->DestroyEvent = (PFN_vkDestroyEvent) gpa(obj, "vkDestroyEvent");
    table->GetEventStatus = (PFN_vkGetEventStatus) gpa(obj, "vkGetEventStatus");
    table->SetEvent = (PFN_vkSetEvent) gpa(obj, "vkSetEvent");
    table->ResetEvent = (PFN_vkResetEvent) gpa(obj, "vkResetEvent");
    table->CreateQueryPool = (PFN_vkCreateQueryPool) gpa(obj, "vkCreateQueryPool");
    table->DestroyQueryPool = (PFN_vkDestroyQueryPool) gpa(obj, "vkDestroyQueryPool");
    table->GetQueryPoolResults = (PFN_vkGetQueryPoolResults) gpa(obj, "vkGetQueryPoolResults");
    table->ResetQueryPool = (PFN_vkResetQueryPool) gpa(obj, "vkResetQueryPool");
    if (table->ResetQueryPool == NULL) {
        table->ResetQueryPool = (PFN_vkResetQueryPool) gpa(obj, "vkResetQueryPoolEXT");
    }
        table->CreateBuffer = (PFN_vkCreateBuffer) gpa(obj, "vkCreateBuffer");
    table->DestroyBuffer = (PFN_vkDestroyBuffer) gpa(obj, "vkDestroyBuffer");
    table->CreateBufferView = (PFN_vkCreateBufferView) gpa(obj, "vkCreateBufferView");
    table->DestroyBufferView = (PFN_vkDestroyBufferView) gpa(obj, "vkDestroyBufferView");
    table->CreateImage = (PFN_vkCreateImage) gpa(obj, "vkCreateImage");
    table->DestroyImage = (PFN_vkDestroyImage) gpa(obj, "vkDestroyImage");
    table->GetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout) gpa(obj, "vkGetImageSubresourceLayout");
    table->CreateImageView = (PFN_vkCreateImageView) gpa(obj, "vkCreateImageView");
    table->DestroyImageView = (PFN_vkDestroyImageView) gpa(obj, "vkDestroyImageView");
    table->CreateShaderModule = (PFN_vkCreateShaderModule) gpa(obj, "vkCreateShaderModule");
    table->DestroyShaderModule = (PFN_vkDestroyShaderModule) gpa(obj, "vkDestroyShaderModule");
    table->CreatePipelineCache = (PFN_vkCreatePipelineCache) gpa(obj, "vkCreatePipelineCache");
    table->DestroyPipelineCache = (PFN_vkDestroyPipelineCache) gpa(obj, "vkDestroyPipelineCache");
    table->GetPipelineCacheData = (PFN_vkGetPipelineCacheData) gpa(obj, "vkGetPipelineCacheData");
    table->MergePipelineCaches = (PFN_vkMergePipelineCaches) gpa(obj, "vkMergePipelineCaches");
    table->CreatePipelineBinariesKHR = (PFN_vkCreatePipelineBinariesKHR) gpa(obj, "vkCreatePipelineBinariesKHR");
    table->DestroyPipelineBinaryKHR = (PFN_vkDestroyPipelineBinaryKHR) gpa(obj, "vkDestroyPipelineBinaryKHR");
    table->GetPipelineKeyKHR = (PFN_vkGetPipelineKeyKHR) gpa(obj, "vkGetPipelineKeyKHR");
    table->GetPipelineBinaryDataKHR = (PFN_vkGetPipelineBinaryDataKHR) gpa(obj, "vkGetPipelineBinaryDataKHR");
    table->ReleaseCapturedPipelineDataKHR = (PFN_vkReleaseCapturedPipelineDataKHR) gpa(obj, "vkReleaseCapturedPipelineDataKHR");
    table->CreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) gpa(obj, "vkCreateGraphicsPipelines");
    table->CreateComputePipelines = (PFN_vkCreateComputePipelines) gpa(obj, "vkCreateComputePipelines");
    table->GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = (PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI) gpa(obj, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    table->DestroyPipeline = (PFN_vkDestroyPipeline) gpa(obj, "vkDestroyPipeline");
    table->CreatePipelineLayout = (PFN_vkCreatePipelineLayout) gpa(obj, "vkCreatePipelineLayout");
    table->DestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) gpa(obj, "vkDestroyPipelineLayout");
    table->CreateSampler = (PFN_vkCreateSampler) gpa(obj, "vkCreateSampler");
    table->DestroySampler = (PFN_vkDestroySampler) gpa(obj, "vkDestroySampler");
    table->CreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) gpa(obj, "vkCreateDescriptorSetLayout");
    table->DestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) gpa(obj, "vkDestroyDescriptorSetLayout");
    table->CreateDescriptorPool = (PFN_vkCreateDescriptorPool) gpa(obj, "vkCreateDescriptorPool");
    table->DestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) gpa(obj, "vkDestroyDescriptorPool");
    table->ResetDescriptorPool = (PFN_vkResetDescriptorPool) gpa(obj, "vkResetDescriptorPool");
    table->AllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) gpa(obj, "vkAllocateDescriptorSets");
    table->FreeDescriptorSets = (PFN_vkFreeDescriptorSets) gpa(obj, "vkFreeDescriptorSets");
    table->UpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) gpa(obj, "vkUpdateDescriptorSets");
    table->CreateFramebuffer = (PFN_vkCreateFramebuffer) gpa(obj, "vkCreateFramebuffer");
    table->DestroyFramebuffer = (PFN_vkDestroyFramebuffer) gpa(obj, "vkDestroyFramebuffer");
    table->CreateRenderPass = (PFN_vkCreateRenderPass) gpa(obj, "vkCreateRenderPass");
    table->DestroyRenderPass = (PFN_vkDestroyRenderPass) gpa(obj, "vkDestroyRenderPass");
    table->GetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity) gpa(obj, "vkGetRenderAreaGranularity");
    table->GetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity) gpa(obj, "vkGetRenderingAreaGranularity");
    if (table->GetRenderingAreaGranularity == NULL) {
        table->GetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity) gpa(obj, "vkGetRenderingAreaGranularityKHR");
    }
        table->CreateCommandPool = (PFN_vkCreateCommandPool) gpa(obj, "vkCreateCommandPool");
    table->DestroyCommandPool = (PFN_vkDestroyCommandPool) gpa(obj, "vkDestroyCommandPool");
    table->ResetCommandPool = (PFN_vkResetCommandPool) gpa(obj, "vkResetCommandPool");
    table->AllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) gpa(obj, "vkAllocateCommandBuffers");
    table->FreeCommandBuffers = (PFN_vkFreeCommandBuffers) gpa(obj, "vkFreeCommandBuffers");
    table->BeginCommandBuffer = (PFN_vkBeginCommandBuffer) gpa(obj, "vkBeginCommandBuffer");
    table->EndCommandBuffer = (PFN_vkEndCommandBuffer) gpa(obj, "vkEndCommandBuffer");
    table->ResetCommandBuffer = (PFN_vkResetCommandBuffer) gpa(obj, "vkResetCommandBuffer");
    table->CmdBindPipeline = (PFN_vkCmdBindPipeline) gpa(obj, "vkCmdBindPipeline");
    table->CmdSetPrimitiveRestartIndexEXT = (PFN_vkCmdSetPrimitiveRestartIndexEXT) gpa(obj, "vkCmdSetPrimitiveRestartIndexEXT");
    table->CmdSetAttachmentFeedbackLoopEnableEXT = (PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT) gpa(obj, "vkCmdSetAttachmentFeedbackLoopEnableEXT");
    table->CmdSetViewport = (PFN_vkCmdSetViewport) gpa(obj, "vkCmdSetViewport");
    table->CmdSetScissor = (PFN_vkCmdSetScissor) gpa(obj, "vkCmdSetScissor");
    table->CmdSetLineWidth = (PFN_vkCmdSetLineWidth) gpa(obj, "vkCmdSetLineWidth");
    table->CmdSetDepthBias = (PFN_vkCmdSetDepthBias) gpa(obj, "vkCmdSetDepthBias");
    table->CmdSetBlendConstants = (PFN_vkCmdSetBlendConstants) gpa(obj, "vkCmdSetBlendConstants");
    table->CmdSetDepthBounds = (PFN_vkCmdSetDepthBounds) gpa(obj, "vkCmdSetDepthBounds");
    table->CmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask) gpa(obj, "vkCmdSetStencilCompareMask");
    table->CmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask) gpa(obj, "vkCmdSetStencilWriteMask");
    table->CmdSetStencilReference = (PFN_vkCmdSetStencilReference) gpa(obj, "vkCmdSetStencilReference");
    table->CmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) gpa(obj, "vkCmdBindDescriptorSets");
    table->CmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) gpa(obj, "vkCmdBindIndexBuffer");
    table->CmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) gpa(obj, "vkCmdBindVertexBuffers");
    table->CmdDraw = (PFN_vkCmdDraw) gpa(obj, "vkCmdDraw");
    table->CmdDrawIndexed = (PFN_vkCmdDrawIndexed) gpa(obj, "vkCmdDrawIndexed");
    table->CmdDrawMultiEXT = (PFN_vkCmdDrawMultiEXT) gpa(obj, "vkCmdDrawMultiEXT");
    table->CmdDrawMultiIndexedEXT = (PFN_vkCmdDrawMultiIndexedEXT) gpa(obj, "vkCmdDrawMultiIndexedEXT");
    table->CmdDrawIndirect = (PFN_vkCmdDrawIndirect) gpa(obj, "vkCmdDrawIndirect");
    table->CmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect) gpa(obj, "vkCmdDrawIndexedIndirect");
    table->CmdDispatch = (PFN_vkCmdDispatch) gpa(obj, "vkCmdDispatch");
    table->CmdDispatchIndirect = (PFN_vkCmdDispatchIndirect) gpa(obj, "vkCmdDispatchIndirect");
    table->CmdSubpassShadingHUAWEI = (PFN_vkCmdSubpassShadingHUAWEI) gpa(obj, "vkCmdSubpassShadingHUAWEI");
    table->CmdDrawClusterHUAWEI = (PFN_vkCmdDrawClusterHUAWEI) gpa(obj, "vkCmdDrawClusterHUAWEI");
    table->CmdDrawClusterIndirectHUAWEI = (PFN_vkCmdDrawClusterIndirectHUAWEI) gpa(obj, "vkCmdDrawClusterIndirectHUAWEI");
    table->CmdUpdatePipelineIndirectBufferNV = (PFN_vkCmdUpdatePipelineIndirectBufferNV) gpa(obj, "vkCmdUpdatePipelineIndirectBufferNV");
    table->CmdCopyBuffer = (PFN_vkCmdCopyBuffer) gpa(obj, "vkCmdCopyBuffer");
    table->CmdCopyImage = (PFN_vkCmdCopyImage) gpa(obj, "vkCmdCopyImage");
    table->CmdBlitImage = (PFN_vkCmdBlitImage) gpa(obj, "vkCmdBlitImage");
    table->CmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) gpa(obj, "vkCmdCopyBufferToImage");
    table->CmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer) gpa(obj, "vkCmdCopyImageToBuffer");
    table->CmdCopyMemoryIndirectNV = (PFN_vkCmdCopyMemoryIndirectNV) gpa(obj, "vkCmdCopyMemoryIndirectNV");
    table->CmdCopyMemoryIndirectKHR = (PFN_vkCmdCopyMemoryIndirectKHR) gpa(obj, "vkCmdCopyMemoryIndirectKHR");
    table->CmdCopyMemoryToImageIndirectNV = (PFN_vkCmdCopyMemoryToImageIndirectNV) gpa(obj, "vkCmdCopyMemoryToImageIndirectNV");
    table->CmdCopyMemoryToImageIndirectKHR = (PFN_vkCmdCopyMemoryToImageIndirectKHR) gpa(obj, "vkCmdCopyMemoryToImageIndirectKHR");
    table->CmdUpdateBuffer = (PFN_vkCmdUpdateBuffer) gpa(obj, "vkCmdUpdateBuffer");
    table->CmdFillBuffer = (PFN_vkCmdFillBuffer) gpa(obj, "vkCmdFillBuffer");
    table->CmdClearColorImage = (PFN_vkCmdClearColorImage) gpa(obj, "vkCmdClearColorImage");
    table->CmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage) gpa(obj, "vkCmdClearDepthStencilImage");
    table->CmdClearAttachments = (PFN_vkCmdClearAttachments) gpa(obj, "vkCmdClearAttachments");
    table->CmdResolveImage = (PFN_vkCmdResolveImage) gpa(obj, "vkCmdResolveImage");
    table->CmdSetEvent = (PFN_vkCmdSetEvent) gpa(obj, "vkCmdSetEvent");
    table->CmdResetEvent = (PFN_vkCmdResetEvent) gpa(obj, "vkCmdResetEvent");
    table->CmdWaitEvents = (PFN_vkCmdWaitEvents) gpa(obj, "vkCmdWaitEvents");
    table->CmdPipelineBarrier = (PFN_vkCmdPipelineBarrier) gpa(obj, "vkCmdPipelineBarrier");
    table->CmdBeginQuery = (PFN_vkCmdBeginQuery) gpa(obj, "vkCmdBeginQuery");
    table->CmdEndQuery = (PFN_vkCmdEndQuery) gpa(obj, "vkCmdEndQuery");
    table->CmdBeginConditionalRenderingEXT = (PFN_vkCmdBeginConditionalRenderingEXT) gpa(obj, "vkCmdBeginConditionalRenderingEXT");
    table->CmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT) gpa(obj, "vkCmdEndConditionalRenderingEXT");
    table->CmdBeginCustomResolveEXT = (PFN_vkCmdBeginCustomResolveEXT) gpa(obj, "vkCmdBeginCustomResolveEXT");
    table->CmdResetQueryPool = (PFN_vkCmdResetQueryPool) gpa(obj, "vkCmdResetQueryPool");
    table->CmdWriteTimestamp = (PFN_vkCmdWriteTimestamp) gpa(obj, "vkCmdWriteTimestamp");
    table->CmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults) gpa(obj, "vkCmdCopyQueryPoolResults");
    table->CmdPushConstants = (PFN_vkCmdPushConstants) gpa(obj, "vkCmdPushConstants");
    table->CmdBeginRenderPass = (PFN_vkCmdBeginRenderPass) gpa(obj, "vkCmdBeginRenderPass");
    table->CmdNextSubpass = (PFN_vkCmdNextSubpass) gpa(obj, "vkCmdNextSubpass");
    table->CmdEndRenderPass = (PFN_vkCmdEndRenderPass) gpa(obj, "vkCmdEndRenderPass");
    table->CmdExecuteCommands = (PFN_vkCmdExecuteCommands) gpa(obj, "vkCmdExecuteCommands");
    table->CreateSharedSwapchainsKHR = (PFN_vkCreateSharedSwapchainsKHR) gpa(obj, "vkCreateSharedSwapchainsKHR");
    table->CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) gpa(obj, "vkCreateSwapchainKHR");
    table->DestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) gpa(obj, "vkDestroySwapchainKHR");
    table->GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) gpa(obj, "vkGetSwapchainImagesKHR");
    table->AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) gpa(obj, "vkAcquireNextImageKHR");
    table->QueuePresentKHR = (PFN_vkQueuePresentKHR) gpa(obj, "vkQueuePresentKHR");
    table->DebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) gpa(obj, "vkDebugMarkerSetObjectNameEXT");
    table->DebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT) gpa(obj, "vkDebugMarkerSetObjectTagEXT");
    table->CmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT) gpa(obj, "vkCmdDebugMarkerBeginEXT");
    table->CmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT) gpa(obj, "vkCmdDebugMarkerEndEXT");
    table->CmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT) gpa(obj, "vkCmdDebugMarkerInsertEXT");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandleNV = (PFN_vkGetMemoryWin32HandleNV) gpa(obj, "vkGetMemoryWin32HandleNV");
#endif
    table->CmdExecuteGeneratedCommandsNV = (PFN_vkCmdExecuteGeneratedCommandsNV) gpa(obj, "vkCmdExecuteGeneratedCommandsNV");
    table->CmdPreprocessGeneratedCommandsNV = (PFN_vkCmdPreprocessGeneratedCommandsNV) gpa(obj, "vkCmdPreprocessGeneratedCommandsNV");
    table->CmdBindPipelineShaderGroupNV = (PFN_vkCmdBindPipelineShaderGroupNV) gpa(obj, "vkCmdBindPipelineShaderGroupNV");
    table->GetGeneratedCommandsMemoryRequirementsNV = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV) gpa(obj, "vkGetGeneratedCommandsMemoryRequirementsNV");
    table->CreateIndirectCommandsLayoutNV = (PFN_vkCreateIndirectCommandsLayoutNV) gpa(obj, "vkCreateIndirectCommandsLayoutNV");
    table->DestroyIndirectCommandsLayoutNV = (PFN_vkDestroyIndirectCommandsLayoutNV) gpa(obj, "vkDestroyIndirectCommandsLayoutNV");
    table->CmdExecuteGeneratedCommandsEXT = (PFN_vkCmdExecuteGeneratedCommandsEXT) gpa(obj, "vkCmdExecuteGeneratedCommandsEXT");
    table->CmdPreprocessGeneratedCommandsEXT = (PFN_vkCmdPreprocessGeneratedCommandsEXT) gpa(obj, "vkCmdPreprocessGeneratedCommandsEXT");
    table->GetGeneratedCommandsMemoryRequirementsEXT = (PFN_vkGetGeneratedCommandsMemoryRequirementsEXT) gpa(obj, "vkGetGeneratedCommandsMemoryRequirementsEXT");
    table->CreateIndirectCommandsLayoutEXT = (PFN_vkCreateIndirectCommandsLayoutEXT) gpa(obj, "vkCreateIndirectCommandsLayoutEXT");
    table->DestroyIndirectCommandsLayoutEXT = (PFN_vkDestroyIndirectCommandsLayoutEXT) gpa(obj, "vkDestroyIndirectCommandsLayoutEXT");
    table->CreateIndirectExecutionSetEXT = (PFN_vkCreateIndirectExecutionSetEXT) gpa(obj, "vkCreateIndirectExecutionSetEXT");
    table->DestroyIndirectExecutionSetEXT = (PFN_vkDestroyIndirectExecutionSetEXT) gpa(obj, "vkDestroyIndirectExecutionSetEXT");
    table->UpdateIndirectExecutionSetPipelineEXT = (PFN_vkUpdateIndirectExecutionSetPipelineEXT) gpa(obj, "vkUpdateIndirectExecutionSetPipelineEXT");
    table->UpdateIndirectExecutionSetShaderEXT = (PFN_vkUpdateIndirectExecutionSetShaderEXT) gpa(obj, "vkUpdateIndirectExecutionSetShaderEXT");
    table->CmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSet) gpa(obj, "vkCmdPushDescriptorSet");
    if (table->CmdPushDescriptorSet == NULL) {
        table->CmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSet) gpa(obj, "vkCmdPushDescriptorSetKHR");
    }
        table->TrimCommandPool = (PFN_vkTrimCommandPool) gpa(obj, "vkTrimCommandPool");
    if (table->TrimCommandPool == NULL) {
        table->TrimCommandPool = (PFN_vkTrimCommandPool) gpa(obj, "vkTrimCommandPoolKHR");
    }
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR) gpa(obj, "vkGetMemoryWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR) gpa(obj, "vkGetMemoryWin32HandlePropertiesKHR");
#endif
    table->GetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) gpa(obj, "vkGetMemoryFdKHR");
    table->GetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR) gpa(obj, "vkGetMemoryFdPropertiesKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetMemoryZirconHandleFUCHSIA = (PFN_vkGetMemoryZirconHandleFUCHSIA) gpa(obj, "vkGetMemoryZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetMemoryZirconHandlePropertiesFUCHSIA = (PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA) gpa(obj, "vkGetMemoryZirconHandlePropertiesFUCHSIA");
#endif
    table->GetMemoryRemoteAddressNV = (PFN_vkGetMemoryRemoteAddressNV) gpa(obj, "vkGetMemoryRemoteAddressNV");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetSemaphoreWin32HandleKHR = (PFN_vkGetSemaphoreWin32HandleKHR) gpa(obj, "vkGetSemaphoreWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR) gpa(obj, "vkImportSemaphoreWin32HandleKHR");
#endif
    table->GetSemaphoreFdKHR = (PFN_vkGetSemaphoreFdKHR) gpa(obj, "vkGetSemaphoreFdKHR");
    table->ImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR) gpa(obj, "vkImportSemaphoreFdKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetSemaphoreZirconHandleFUCHSIA = (PFN_vkGetSemaphoreZirconHandleFUCHSIA) gpa(obj, "vkGetSemaphoreZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->ImportSemaphoreZirconHandleFUCHSIA = (PFN_vkImportSemaphoreZirconHandleFUCHSIA) gpa(obj, "vkImportSemaphoreZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetFenceWin32HandleKHR = (PFN_vkGetFenceWin32HandleKHR) gpa(obj, "vkGetFenceWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ImportFenceWin32HandleKHR = (PFN_vkImportFenceWin32HandleKHR) gpa(obj, "vkImportFenceWin32HandleKHR");
#endif
    table->GetFenceFdKHR = (PFN_vkGetFenceFdKHR) gpa(obj, "vkGetFenceFdKHR");
    table->ImportFenceFdKHR = (PFN_vkImportFenceFdKHR) gpa(obj, "vkImportFenceFdKHR");
    table->DisplayPowerControlEXT = (PFN_vkDisplayPowerControlEXT) gpa(obj, "vkDisplayPowerControlEXT");
    table->RegisterDeviceEventEXT = (PFN_vkRegisterDeviceEventEXT) gpa(obj, "vkRegisterDeviceEventEXT");
    table->RegisterDisplayEventEXT = (PFN_vkRegisterDisplayEventEXT) gpa(obj, "vkRegisterDisplayEventEXT");
    table->GetSwapchainCounterEXT = (PFN_vkGetSwapchainCounterEXT) gpa(obj, "vkGetSwapchainCounterEXT");
    table->GetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) gpa(obj, "vkGetDeviceGroupPeerMemoryFeatures");
    if (table->GetDeviceGroupPeerMemoryFeatures == NULL) {
        table->GetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) gpa(obj, "vkGetDeviceGroupPeerMemoryFeaturesKHR");
    }
        table->BindBufferMemory2 = (PFN_vkBindBufferMemory2) gpa(obj, "vkBindBufferMemory2");
    if (table->BindBufferMemory2 == NULL) {
        table->BindBufferMemory2 = (PFN_vkBindBufferMemory2) gpa(obj, "vkBindBufferMemory2KHR");
    }
        table->BindImageMemory2 = (PFN_vkBindImageMemory2) gpa(obj, "vkBindImageMemory2");
    if (table->BindImageMemory2 == NULL) {
        table->BindImageMemory2 = (PFN_vkBindImageMemory2) gpa(obj, "vkBindImageMemory2KHR");
    }
        table->CmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) gpa(obj, "vkCmdSetDeviceMask");
    if (table->CmdSetDeviceMask == NULL) {
        table->CmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) gpa(obj, "vkCmdSetDeviceMaskKHR");
    }
        table->GetDeviceGroupPresentCapabilitiesKHR = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR) gpa(obj, "vkGetDeviceGroupPresentCapabilitiesKHR");
    table->GetDeviceGroupSurfacePresentModesKHR = (PFN_vkGetDeviceGroupSurfacePresentModesKHR) gpa(obj, "vkGetDeviceGroupSurfacePresentModesKHR");
    table->AcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR) gpa(obj, "vkAcquireNextImage2KHR");
    table->CmdDispatchBase = (PFN_vkCmdDispatchBase) gpa(obj, "vkCmdDispatchBase");
    if (table->CmdDispatchBase == NULL) {
        table->CmdDispatchBase = (PFN_vkCmdDispatchBase) gpa(obj, "vkCmdDispatchBaseKHR");
    }
        table->CreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) gpa(obj, "vkCreateDescriptorUpdateTemplate");
    if (table->CreateDescriptorUpdateTemplate == NULL) {
        table->CreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) gpa(obj, "vkCreateDescriptorUpdateTemplateKHR");
    }
        table->DestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) gpa(obj, "vkDestroyDescriptorUpdateTemplate");
    if (table->DestroyDescriptorUpdateTemplate == NULL) {
        table->DestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) gpa(obj, "vkDestroyDescriptorUpdateTemplateKHR");
    }
        table->UpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) gpa(obj, "vkUpdateDescriptorSetWithTemplate");
    if (table->UpdateDescriptorSetWithTemplate == NULL) {
        table->UpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) gpa(obj, "vkUpdateDescriptorSetWithTemplateKHR");
    }
        table->CmdPushDescriptorSetWithTemplate = (PFN_vkCmdPushDescriptorSetWithTemplate) gpa(obj, "vkCmdPushDescriptorSetWithTemplate");
    if (table->CmdPushDescriptorSetWithTemplate == NULL) {
        table->CmdPushDescriptorSetWithTemplate = (PFN_vkCmdPushDescriptorSetWithTemplate) gpa(obj, "vkCmdPushDescriptorSetWithTemplateKHR");
    }
        table->SetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT) gpa(obj, "vkSetHdrMetadataEXT");
    table->GetSwapchainStatusKHR = (PFN_vkGetSwapchainStatusKHR) gpa(obj, "vkGetSwapchainStatusKHR");
    table->GetRefreshCycleDurationGOOGLE = (PFN_vkGetRefreshCycleDurationGOOGLE) gpa(obj, "vkGetRefreshCycleDurationGOOGLE");
    table->GetPastPresentationTimingGOOGLE = (PFN_vkGetPastPresentationTimingGOOGLE) gpa(obj, "vkGetPastPresentationTimingGOOGLE");
    table->CmdSetViewportWScalingNV = (PFN_vkCmdSetViewportWScalingNV) gpa(obj, "vkCmdSetViewportWScalingNV");
    table->CmdSetDiscardRectangleEXT = (PFN_vkCmdSetDiscardRectangleEXT) gpa(obj, "vkCmdSetDiscardRectangleEXT");
    table->CmdSetDiscardRectangleEnableEXT = (PFN_vkCmdSetDiscardRectangleEnableEXT) gpa(obj, "vkCmdSetDiscardRectangleEnableEXT");
    table->CmdSetDiscardRectangleModeEXT = (PFN_vkCmdSetDiscardRectangleModeEXT) gpa(obj, "vkCmdSetDiscardRectangleModeEXT");
    table->CmdSetSampleLocationsEXT = (PFN_vkCmdSetSampleLocationsEXT) gpa(obj, "vkCmdSetSampleLocationsEXT");
    table->GetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) gpa(obj, "vkGetBufferMemoryRequirements2");
    if (table->GetBufferMemoryRequirements2 == NULL) {
        table->GetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) gpa(obj, "vkGetBufferMemoryRequirements2KHR");
    }
        table->GetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) gpa(obj, "vkGetImageMemoryRequirements2");
    if (table->GetImageMemoryRequirements2 == NULL) {
        table->GetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) gpa(obj, "vkGetImageMemoryRequirements2KHR");
    }
        table->GetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) gpa(obj, "vkGetImageSparseMemoryRequirements2");
    if (table->GetImageSparseMemoryRequirements2 == NULL) {
        table->GetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) gpa(obj, "vkGetImageSparseMemoryRequirements2KHR");
    }
        table->GetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements) gpa(obj, "vkGetDeviceBufferMemoryRequirements");
    if (table->GetDeviceBufferMemoryRequirements == NULL) {
        table->GetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements) gpa(obj, "vkGetDeviceBufferMemoryRequirementsKHR");
    }
        table->GetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements) gpa(obj, "vkGetDeviceImageMemoryRequirements");
    if (table->GetDeviceImageMemoryRequirements == NULL) {
        table->GetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements) gpa(obj, "vkGetDeviceImageMemoryRequirementsKHR");
    }
        table->GetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements) gpa(obj, "vkGetDeviceImageSparseMemoryRequirements");
    if (table->GetDeviceImageSparseMemoryRequirements == NULL) {
        table->GetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements) gpa(obj, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    }
        table->CreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) gpa(obj, "vkCreateSamplerYcbcrConversion");
    if (table->CreateSamplerYcbcrConversion == NULL) {
        table->CreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) gpa(obj, "vkCreateSamplerYcbcrConversionKHR");
    }
        table->DestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) gpa(obj, "vkDestroySamplerYcbcrConversion");
    if (table->DestroySamplerYcbcrConversion == NULL) {
        table->DestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) gpa(obj, "vkDestroySamplerYcbcrConversionKHR");
    }
        table->GetDeviceQueue2 = (PFN_vkGetDeviceQueue2) gpa(obj, "vkGetDeviceQueue2");
    table->CreateValidationCacheEXT = (PFN_vkCreateValidationCacheEXT) gpa(obj, "vkCreateValidationCacheEXT");
    table->DestroyValidationCacheEXT = (PFN_vkDestroyValidationCacheEXT) gpa(obj, "vkDestroyValidationCacheEXT");
    table->GetValidationCacheDataEXT = (PFN_vkGetValidationCacheDataEXT) gpa(obj, "vkGetValidationCacheDataEXT");
    table->MergeValidationCachesEXT = (PFN_vkMergeValidationCachesEXT) gpa(obj, "vkMergeValidationCachesEXT");
    table->GetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) gpa(obj, "vkGetDescriptorSetLayoutSupport");
    if (table->GetDescriptorSetLayoutSupport == NULL) {
        table->GetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) gpa(obj, "vkGetDescriptorSetLayoutSupportKHR");
    }
    #ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetSwapchainGrallocUsageANDROID = (PFN_vkGetSwapchainGrallocUsageANDROID) gpa(obj, "vkGetSwapchainGrallocUsageANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetSwapchainGrallocUsage2ANDROID = (PFN_vkGetSwapchainGrallocUsage2ANDROID) gpa(obj, "vkGetSwapchainGrallocUsage2ANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->AcquireImageANDROID = (PFN_vkAcquireImageANDROID) gpa(obj, "vkAcquireImageANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->QueueSignalReleaseImageANDROID = (PFN_vkQueueSignalReleaseImageANDROID) gpa(obj, "vkQueueSignalReleaseImageANDROID");
#endif
    table->GetShaderInfoAMD = (PFN_vkGetShaderInfoAMD) gpa(obj, "vkGetShaderInfoAMD");
    table->SetLocalDimmingAMD = (PFN_vkSetLocalDimmingAMD) gpa(obj, "vkSetLocalDimmingAMD");
    table->GetCalibratedTimestampsKHR = (PFN_vkGetCalibratedTimestampsKHR) gpa(obj, "vkGetCalibratedTimestampsKHR");
    if (table->GetCalibratedTimestampsKHR == NULL) {
        table->GetCalibratedTimestampsKHR = (PFN_vkGetCalibratedTimestampsKHR) gpa(obj, "vkGetCalibratedTimestampsEXT");
    }
        table->SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) gpa(obj, "vkSetDebugUtilsObjectNameEXT");
    table->SetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT) gpa(obj, "vkSetDebugUtilsObjectTagEXT");
    table->QueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT) gpa(obj, "vkQueueBeginDebugUtilsLabelEXT");
    table->QueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT) gpa(obj, "vkQueueEndDebugUtilsLabelEXT");
    table->QueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT) gpa(obj, "vkQueueInsertDebugUtilsLabelEXT");
    table->CmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT) gpa(obj, "vkCmdBeginDebugUtilsLabelEXT");
    table->CmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT) gpa(obj, "vkCmdEndDebugUtilsLabelEXT");
    table->CmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT) gpa(obj, "vkCmdInsertDebugUtilsLabelEXT");
    table->GetMemoryHostPointerPropertiesEXT = (PFN_vkGetMemoryHostPointerPropertiesEXT) gpa(obj, "vkGetMemoryHostPointerPropertiesEXT");
    table->CmdWriteBufferMarkerAMD = (PFN_vkCmdWriteBufferMarkerAMD) gpa(obj, "vkCmdWriteBufferMarkerAMD");
    table->CreateRenderPass2 = (PFN_vkCreateRenderPass2) gpa(obj, "vkCreateRenderPass2");
    if (table->CreateRenderPass2 == NULL) {
        table->CreateRenderPass2 = (PFN_vkCreateRenderPass2) gpa(obj, "vkCreateRenderPass2KHR");
    }
        table->CmdBeginRenderPass2 = (PFN_vkCmdBeginRenderPass2) gpa(obj, "vkCmdBeginRenderPass2");
    if (table->CmdBeginRenderPass2 == NULL) {
        table->CmdBeginRenderPass2 = (PFN_vkCmdBeginRenderPass2) gpa(obj, "vkCmdBeginRenderPass2KHR");
    }
        table->CmdNextSubpass2 = (PFN_vkCmdNextSubpass2) gpa(obj, "vkCmdNextSubpass2");
    if (table->CmdNextSubpass2 == NULL) {
        table->CmdNextSubpass2 = (PFN_vkCmdNextSubpass2) gpa(obj, "vkCmdNextSubpass2KHR");
    }
        table->CmdEndRenderPass2 = (PFN_vkCmdEndRenderPass2) gpa(obj, "vkCmdEndRenderPass2");
    if (table->CmdEndRenderPass2 == NULL) {
        table->CmdEndRenderPass2 = (PFN_vkCmdEndRenderPass2) gpa(obj, "vkCmdEndRenderPass2KHR");
    }
        table->GetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) gpa(obj, "vkGetSemaphoreCounterValue");
    if (table->GetSemaphoreCounterValue == NULL) {
        table->GetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) gpa(obj, "vkGetSemaphoreCounterValueKHR");
    }
        table->WaitSemaphores = (PFN_vkWaitSemaphores) gpa(obj, "vkWaitSemaphores");
    if (table->WaitSemaphores == NULL) {
        table->WaitSemaphores = (PFN_vkWaitSemaphores) gpa(obj, "vkWaitSemaphoresKHR");
    }
        table->SignalSemaphore = (PFN_vkSignalSemaphore) gpa(obj, "vkSignalSemaphore");
    if (table->SignalSemaphore == NULL) {
        table->SignalSemaphore = (PFN_vkSignalSemaphore) gpa(obj, "vkSignalSemaphoreKHR");
    }
    #ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetAndroidHardwareBufferPropertiesANDROID = (PFN_vkGetAndroidHardwareBufferPropertiesANDROID) gpa(obj, "vkGetAndroidHardwareBufferPropertiesANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetMemoryAndroidHardwareBufferANDROID = (PFN_vkGetMemoryAndroidHardwareBufferANDROID) gpa(obj, "vkGetMemoryAndroidHardwareBufferANDROID");
#endif
    table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) gpa(obj, "vkCmdDrawIndirectCount");
    if (table->CmdDrawIndirectCount == NULL) {
        table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) gpa(obj, "vkCmdDrawIndirectCountKHR");
    }
    if (table->CmdDrawIndirectCount == NULL) {
        table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) gpa(obj, "vkCmdDrawIndirectCountAMD");
    }
            table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) gpa(obj, "vkCmdDrawIndexedIndirectCount");
    if (table->CmdDrawIndexedIndirectCount == NULL) {
        table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) gpa(obj, "vkCmdDrawIndexedIndirectCountKHR");
    }
    if (table->CmdDrawIndexedIndirectCount == NULL) {
        table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) gpa(obj, "vkCmdDrawIndexedIndirectCountAMD");
    }
            table->CmdSetCheckpointNV = (PFN_vkCmdSetCheckpointNV) gpa(obj, "vkCmdSetCheckpointNV");
    table->GetQueueCheckpointDataNV = (PFN_vkGetQueueCheckpointDataNV) gpa(obj, "vkGetQueueCheckpointDataNV");
    table->CmdBindTransformFeedbackBuffersEXT = (PFN_vkCmdBindTransformFeedbackBuffersEXT) gpa(obj, "vkCmdBindTransformFeedbackBuffersEXT");
    table->CmdBeginTransformFeedbackEXT = (PFN_vkCmdBeginTransformFeedbackEXT) gpa(obj, "vkCmdBeginTransformFeedbackEXT");
    table->CmdEndTransformFeedbackEXT = (PFN_vkCmdEndTransformFeedbackEXT) gpa(obj, "vkCmdEndTransformFeedbackEXT");
    table->CmdBeginQueryIndexedEXT = (PFN_vkCmdBeginQueryIndexedEXT) gpa(obj, "vkCmdBeginQueryIndexedEXT");
    table->CmdEndQueryIndexedEXT = (PFN_vkCmdEndQueryIndexedEXT) gpa(obj, "vkCmdEndQueryIndexedEXT");
    table->CmdDrawIndirectByteCountEXT = (PFN_vkCmdDrawIndirectByteCountEXT) gpa(obj, "vkCmdDrawIndirectByteCountEXT");
    table->CmdSetExclusiveScissorNV = (PFN_vkCmdSetExclusiveScissorNV) gpa(obj, "vkCmdSetExclusiveScissorNV");
    table->CmdSetExclusiveScissorEnableNV = (PFN_vkCmdSetExclusiveScissorEnableNV) gpa(obj, "vkCmdSetExclusiveScissorEnableNV");
    table->CmdBindShadingRateImageNV = (PFN_vkCmdBindShadingRateImageNV) gpa(obj, "vkCmdBindShadingRateImageNV");
    table->CmdSetViewportShadingRatePaletteNV = (PFN_vkCmdSetViewportShadingRatePaletteNV) gpa(obj, "vkCmdSetViewportShadingRatePaletteNV");
    table->CmdSetCoarseSampleOrderNV = (PFN_vkCmdSetCoarseSampleOrderNV) gpa(obj, "vkCmdSetCoarseSampleOrderNV");
    table->CmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV) gpa(obj, "vkCmdDrawMeshTasksNV");
    table->CmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV) gpa(obj, "vkCmdDrawMeshTasksIndirectNV");
    table->CmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV) gpa(obj, "vkCmdDrawMeshTasksIndirectCountNV");
    table->CmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT) gpa(obj, "vkCmdDrawMeshTasksEXT");
    table->CmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT) gpa(obj, "vkCmdDrawMeshTasksIndirectEXT");
    table->CmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT) gpa(obj, "vkCmdDrawMeshTasksIndirectCountEXT");
    table->CompileDeferredNV = (PFN_vkCompileDeferredNV) gpa(obj, "vkCompileDeferredNV");
    table->CreateAccelerationStructureNV = (PFN_vkCreateAccelerationStructureNV) gpa(obj, "vkCreateAccelerationStructureNV");
    table->CmdBindInvocationMaskHUAWEI = (PFN_vkCmdBindInvocationMaskHUAWEI) gpa(obj, "vkCmdBindInvocationMaskHUAWEI");
    table->DestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR) gpa(obj, "vkDestroyAccelerationStructureKHR");
    table->DestroyAccelerationStructureNV = (PFN_vkDestroyAccelerationStructureNV) gpa(obj, "vkDestroyAccelerationStructureNV");
    table->GetAccelerationStructureMemoryRequirementsNV = (PFN_vkGetAccelerationStructureMemoryRequirementsNV) gpa(obj, "vkGetAccelerationStructureMemoryRequirementsNV");
    table->BindAccelerationStructureMemoryNV = (PFN_vkBindAccelerationStructureMemoryNV) gpa(obj, "vkBindAccelerationStructureMemoryNV");
    table->CmdCopyAccelerationStructureNV = (PFN_vkCmdCopyAccelerationStructureNV) gpa(obj, "vkCmdCopyAccelerationStructureNV");
    table->CmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR) gpa(obj, "vkCmdCopyAccelerationStructureKHR");
    table->CopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR) gpa(obj, "vkCopyAccelerationStructureKHR");
    table->CmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR) gpa(obj, "vkCmdCopyAccelerationStructureToMemoryKHR");
    table->CopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR) gpa(obj, "vkCopyAccelerationStructureToMemoryKHR");
    table->CmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR) gpa(obj, "vkCmdCopyMemoryToAccelerationStructureKHR");
    table->CopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR) gpa(obj, "vkCopyMemoryToAccelerationStructureKHR");
    table->CmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR) gpa(obj, "vkCmdWriteAccelerationStructuresPropertiesKHR");
    table->CmdWriteAccelerationStructuresPropertiesNV = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV) gpa(obj, "vkCmdWriteAccelerationStructuresPropertiesNV");
    table->CmdBuildAccelerationStructureNV = (PFN_vkCmdBuildAccelerationStructureNV) gpa(obj, "vkCmdBuildAccelerationStructureNV");
    table->WriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR) gpa(obj, "vkWriteAccelerationStructuresPropertiesKHR");
    table->CmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR) gpa(obj, "vkCmdTraceRaysKHR");
    table->CmdTraceRaysNV = (PFN_vkCmdTraceRaysNV) gpa(obj, "vkCmdTraceRaysNV");
    table->GetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR) gpa(obj, "vkGetRayTracingShaderGroupHandlesKHR");
    if (table->GetRayTracingShaderGroupHandlesKHR == NULL) {
        table->GetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR) gpa(obj, "vkGetRayTracingShaderGroupHandlesNV");
    }
        table->GetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR) gpa(obj, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    table->GetAccelerationStructureHandleNV = (PFN_vkGetAccelerationStructureHandleNV) gpa(obj, "vkGetAccelerationStructureHandleNV");
    table->CreateRayTracingPipelinesNV = (PFN_vkCreateRayTracingPipelinesNV) gpa(obj, "vkCreateRayTracingPipelinesNV");
    table->CreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR) gpa(obj, "vkCreateRayTracingPipelinesKHR");
    table->CmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR) gpa(obj, "vkCmdTraceRaysIndirectKHR");
    table->CmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR) gpa(obj, "vkCmdTraceRaysIndirect2KHR");
    table->GetClusterAccelerationStructureBuildSizesNV = (PFN_vkGetClusterAccelerationStructureBuildSizesNV) gpa(obj, "vkGetClusterAccelerationStructureBuildSizesNV");
    table->CmdBuildClusterAccelerationStructureIndirectNV = (PFN_vkCmdBuildClusterAccelerationStructureIndirectNV) gpa(obj, "vkCmdBuildClusterAccelerationStructureIndirectNV");
    table->GetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR) gpa(obj, "vkGetDeviceAccelerationStructureCompatibilityKHR");
    table->GetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR) gpa(obj, "vkGetRayTracingShaderGroupStackSizeKHR");
    table->CmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR) gpa(obj, "vkCmdSetRayTracingPipelineStackSizeKHR");
    table->GetImageViewHandleNVX = (PFN_vkGetImageViewHandleNVX) gpa(obj, "vkGetImageViewHandleNVX");
    table->GetImageViewHandle64NVX = (PFN_vkGetImageViewHandle64NVX) gpa(obj, "vkGetImageViewHandle64NVX");
    table->GetImageViewAddressNVX = (PFN_vkGetImageViewAddressNVX) gpa(obj, "vkGetImageViewAddressNVX");
    table->GetDeviceCombinedImageSamplerIndexNVX = (PFN_vkGetDeviceCombinedImageSamplerIndexNVX) gpa(obj, "vkGetDeviceCombinedImageSamplerIndexNVX");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetDeviceGroupSurfacePresentModes2EXT = (PFN_vkGetDeviceGroupSurfacePresentModes2EXT) gpa(obj, "vkGetDeviceGroupSurfacePresentModes2EXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->AcquireFullScreenExclusiveModeEXT = (PFN_vkAcquireFullScreenExclusiveModeEXT) gpa(obj, "vkAcquireFullScreenExclusiveModeEXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ReleaseFullScreenExclusiveModeEXT = (PFN_vkReleaseFullScreenExclusiveModeEXT) gpa(obj, "vkReleaseFullScreenExclusiveModeEXT");
#endif
    table->AcquireProfilingLockKHR = (PFN_vkAcquireProfilingLockKHR) gpa(obj, "vkAcquireProfilingLockKHR");
    table->ReleaseProfilingLockKHR = (PFN_vkReleaseProfilingLockKHR) gpa(obj, "vkReleaseProfilingLockKHR");
    table->GetImageDrmFormatModifierPropertiesEXT = (PFN_vkGetImageDrmFormatModifierPropertiesEXT) gpa(obj, "vkGetImageDrmFormatModifierPropertiesEXT");
    table->GetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress) gpa(obj, "vkGetBufferOpaqueCaptureAddress");
    if (table->GetBufferOpaqueCaptureAddress == NULL) {
        table->GetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress) gpa(obj, "vkGetBufferOpaqueCaptureAddressKHR");
    }
        table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) gpa(obj, "vkGetBufferDeviceAddress");
    if (table->GetBufferDeviceAddress == NULL) {
        table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) gpa(obj, "vkGetBufferDeviceAddressKHR");
    }
    if (table->GetBufferDeviceAddress == NULL) {
        table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) gpa(obj, "vkGetBufferDeviceAddressEXT");
    }
            table->InitializePerformanceApiINTEL = (PFN_vkInitializePerformanceApiINTEL) gpa(obj, "vkInitializePerformanceApiINTEL");
    table->UninitializePerformanceApiINTEL = (PFN_vkUninitializePerformanceApiINTEL) gpa(obj, "vkUninitializePerformanceApiINTEL");
    table->CmdSetPerformanceMarkerINTEL = (PFN_vkCmdSetPerformanceMarkerINTEL) gpa(obj, "vkCmdSetPerformanceMarkerINTEL");
    table->CmdSetPerformanceStreamMarkerINTEL = (PFN_vkCmdSetPerformanceStreamMarkerINTEL) gpa(obj, "vkCmdSetPerformanceStreamMarkerINTEL");
    table->CmdSetPerformanceOverrideINTEL = (PFN_vkCmdSetPerformanceOverrideINTEL) gpa(obj, "vkCmdSetPerformanceOverrideINTEL");
    table->AcquirePerformanceConfigurationINTEL = (PFN_vkAcquirePerformanceConfigurationINTEL) gpa(obj, "vkAcquirePerformanceConfigurationINTEL");
    table->ReleasePerformanceConfigurationINTEL = (PFN_vkReleasePerformanceConfigurationINTEL) gpa(obj, "vkReleasePerformanceConfigurationINTEL");
    table->QueueSetPerformanceConfigurationINTEL = (PFN_vkQueueSetPerformanceConfigurationINTEL) gpa(obj, "vkQueueSetPerformanceConfigurationINTEL");
    table->GetPerformanceParameterINTEL = (PFN_vkGetPerformanceParameterINTEL) gpa(obj, "vkGetPerformanceParameterINTEL");
    table->GetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress) gpa(obj, "vkGetDeviceMemoryOpaqueCaptureAddress");
    if (table->GetDeviceMemoryOpaqueCaptureAddress == NULL) {
        table->GetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress) gpa(obj, "vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    }
        table->GetPipelineExecutablePropertiesKHR = (PFN_vkGetPipelineExecutablePropertiesKHR) gpa(obj, "vkGetPipelineExecutablePropertiesKHR");
    table->GetPipelineExecutableStatisticsKHR = (PFN_vkGetPipelineExecutableStatisticsKHR) gpa(obj, "vkGetPipelineExecutableStatisticsKHR");
    table->GetPipelineExecutableInternalRepresentationsKHR = (PFN_vkGetPipelineExecutableInternalRepresentationsKHR) gpa(obj, "vkGetPipelineExecutableInternalRepresentationsKHR");
    table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) gpa(obj, "vkCmdSetLineStipple");
    if (table->CmdSetLineStipple == NULL) {
        table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) gpa(obj, "vkCmdSetLineStippleKHR");
    }
    if (table->CmdSetLineStipple == NULL) {
        table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) gpa(obj, "vkCmdSetLineStippleEXT");
    }
            table->CreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR) gpa(obj, "vkCreateAccelerationStructureKHR");
    table->CmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR) gpa(obj, "vkCmdBuildAccelerationStructuresKHR");
    table->CmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR) gpa(obj, "vkCmdBuildAccelerationStructuresIndirectKHR");
    table->BuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR) gpa(obj, "vkBuildAccelerationStructuresKHR");
    table->GetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR) gpa(obj, "vkGetAccelerationStructureDeviceAddressKHR");
    table->CreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR) gpa(obj, "vkCreateDeferredOperationKHR");
    table->DestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR) gpa(obj, "vkDestroyDeferredOperationKHR");
    table->GetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR) gpa(obj, "vkGetDeferredOperationMaxConcurrencyKHR");
    table->GetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR) gpa(obj, "vkGetDeferredOperationResultKHR");
    table->DeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR) gpa(obj, "vkDeferredOperationJoinKHR");
    table->GetPipelineIndirectMemoryRequirementsNV = (PFN_vkGetPipelineIndirectMemoryRequirementsNV) gpa(obj, "vkGetPipelineIndirectMemoryRequirementsNV");
    table->GetPipelineIndirectDeviceAddressNV = (PFN_vkGetPipelineIndirectDeviceAddressNV) gpa(obj, "vkGetPipelineIndirectDeviceAddressNV");
    table->AntiLagUpdateAMD = (PFN_vkAntiLagUpdateAMD) gpa(obj, "vkAntiLagUpdateAMD");
    table->CmdSetCullMode = (PFN_vkCmdSetCullMode) gpa(obj, "vkCmdSetCullMode");
    if (table->CmdSetCullMode == NULL) {
        table->CmdSetCullMode = (PFN_vkCmdSetCullMode) gpa(obj, "vkCmdSetCullModeEXT");
    }
        table->CmdSetFrontFace = (PFN_vkCmdSetFrontFace) gpa(obj, "vkCmdSetFrontFace");
    if (table->CmdSetFrontFace == NULL) {
        table->CmdSetFrontFace = (PFN_vkCmdSetFrontFace) gpa(obj, "vkCmdSetFrontFaceEXT");
    }
        table->CmdSetPrimitiveTopology = (PFN_vkCmdSetPrimitiveTopology) gpa(obj, "vkCmdSetPrimitiveTopology");
    if (table->CmdSetPrimitiveTopology == NULL) {
        table->CmdSetPrimitiveTopology = (PFN_vkCmdSetPrimitiveTopology) gpa(obj, "vkCmdSetPrimitiveTopologyEXT");
    }
        table->CmdSetViewportWithCount = (PFN_vkCmdSetViewportWithCount) gpa(obj, "vkCmdSetViewportWithCount");
    if (table->CmdSetViewportWithCount == NULL) {
        table->CmdSetViewportWithCount = (PFN_vkCmdSetViewportWithCount) gpa(obj, "vkCmdSetViewportWithCountEXT");
    }
        table->CmdSetScissorWithCount = (PFN_vkCmdSetScissorWithCount) gpa(obj, "vkCmdSetScissorWithCount");
    if (table->CmdSetScissorWithCount == NULL) {
        table->CmdSetScissorWithCount = (PFN_vkCmdSetScissorWithCount) gpa(obj, "vkCmdSetScissorWithCountEXT");
    }
        table->CmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2) gpa(obj, "vkCmdBindIndexBuffer2");
    if (table->CmdBindIndexBuffer2 == NULL) {
        table->CmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2) gpa(obj, "vkCmdBindIndexBuffer2KHR");
    }
        table->CmdBindVertexBuffers2 = (PFN_vkCmdBindVertexBuffers2) gpa(obj, "vkCmdBindVertexBuffers2");
    if (table->CmdBindVertexBuffers2 == NULL) {
        table->CmdBindVertexBuffers2 = (PFN_vkCmdBindVertexBuffers2) gpa(obj, "vkCmdBindVertexBuffers2EXT");
    }
        table->CmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable) gpa(obj, "vkCmdSetDepthTestEnable");
    if (table->CmdSetDepthTestEnable == NULL) {
        table->CmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable) gpa(obj, "vkCmdSetDepthTestEnableEXT");
    }
        table->CmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) gpa(obj, "vkCmdSetDepthWriteEnable");
    if (table->CmdSetDepthWriteEnable == NULL) {
        table->CmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) gpa(obj, "vkCmdSetDepthWriteEnableEXT");
    }
        table->CmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) gpa(obj, "vkCmdSetDepthCompareOp");
    if (table->CmdSetDepthCompareOp == NULL) {
        table->CmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) gpa(obj, "vkCmdSetDepthCompareOpEXT");
    }
        table->CmdSetDepthBoundsTestEnable = (PFN_vkCmdSetDepthBoundsTestEnable) gpa(obj, "vkCmdSetDepthBoundsTestEnable");
    if (table->CmdSetDepthBoundsTestEnable == NULL) {
        table->CmdSetDepthBoundsTestEnable = (PFN_vkCmdSetDepthBoundsTestEnable) gpa(obj, "vkCmdSetDepthBoundsTestEnableEXT");
    }
        table->CmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable) gpa(obj, "vkCmdSetStencilTestEnable");
    if (table->CmdSetStencilTestEnable == NULL) {
        table->CmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable) gpa(obj, "vkCmdSetStencilTestEnableEXT");
    }
        table->CmdSetStencilOp = (PFN_vkCmdSetStencilOp) gpa(obj, "vkCmdSetStencilOp");
    if (table->CmdSetStencilOp == NULL) {
        table->CmdSetStencilOp = (PFN_vkCmdSetStencilOp) gpa(obj, "vkCmdSetStencilOpEXT");
    }
        table->CmdSetPatchControlPointsEXT = (PFN_vkCmdSetPatchControlPointsEXT) gpa(obj, "vkCmdSetPatchControlPointsEXT");
    table->CmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable) gpa(obj, "vkCmdSetRasterizerDiscardEnable");
    if (table->CmdSetRasterizerDiscardEnable == NULL) {
        table->CmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable) gpa(obj, "vkCmdSetRasterizerDiscardEnableEXT");
    }
        table->CmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) gpa(obj, "vkCmdSetDepthBiasEnable");
    if (table->CmdSetDepthBiasEnable == NULL) {
        table->CmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) gpa(obj, "vkCmdSetDepthBiasEnableEXT");
    }
        table->CmdSetLogicOpEXT = (PFN_vkCmdSetLogicOpEXT) gpa(obj, "vkCmdSetLogicOpEXT");
    table->CmdSetPrimitiveRestartEnable = (PFN_vkCmdSetPrimitiveRestartEnable) gpa(obj, "vkCmdSetPrimitiveRestartEnable");
    if (table->CmdSetPrimitiveRestartEnable == NULL) {
        table->CmdSetPrimitiveRestartEnable = (PFN_vkCmdSetPrimitiveRestartEnable) gpa(obj, "vkCmdSetPrimitiveRestartEnableEXT");
    }
        table->CmdSetTessellationDomainOriginEXT = (PFN_vkCmdSetTessellationDomainOriginEXT) gpa(obj, "vkCmdSetTessellationDomainOriginEXT");
    table->CmdSetDepthClampEnableEXT = (PFN_vkCmdSetDepthClampEnableEXT) gpa(obj, "vkCmdSetDepthClampEnableEXT");
    table->CmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) gpa(obj, "vkCmdSetPolygonModeEXT");
    table->CmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT) gpa(obj, "vkCmdSetRasterizationSamplesEXT");
    table->CmdSetSampleMaskEXT = (PFN_vkCmdSetSampleMaskEXT) gpa(obj, "vkCmdSetSampleMaskEXT");
    table->CmdSetAlphaToCoverageEnableEXT = (PFN_vkCmdSetAlphaToCoverageEnableEXT) gpa(obj, "vkCmdSetAlphaToCoverageEnableEXT");
    table->CmdSetAlphaToOneEnableEXT = (PFN_vkCmdSetAlphaToOneEnableEXT) gpa(obj, "vkCmdSetAlphaToOneEnableEXT");
    table->CmdSetLogicOpEnableEXT = (PFN_vkCmdSetLogicOpEnableEXT) gpa(obj, "vkCmdSetLogicOpEnableEXT");
    table->CmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) gpa(obj, "vkCmdSetColorBlendEnableEXT");
    table->CmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) gpa(obj, "vkCmdSetColorBlendEquationEXT");
    table->CmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) gpa(obj, "vkCmdSetColorWriteMaskEXT");
    table->CmdSetRasterizationStreamEXT = (PFN_vkCmdSetRasterizationStreamEXT) gpa(obj, "vkCmdSetRasterizationStreamEXT");
    table->CmdSetConservativeRasterizationModeEXT = (PFN_vkCmdSetConservativeRasterizationModeEXT) gpa(obj, "vkCmdSetConservativeRasterizationModeEXT");
    table->CmdSetExtraPrimitiveOverestimationSizeEXT = (PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT) gpa(obj, "vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    table->CmdSetDepthClipEnableEXT = (PFN_vkCmdSetDepthClipEnableEXT) gpa(obj, "vkCmdSetDepthClipEnableEXT");
    table->CmdSetSampleLocationsEnableEXT = (PFN_vkCmdSetSampleLocationsEnableEXT) gpa(obj, "vkCmdSetSampleLocationsEnableEXT");
    table->CmdSetColorBlendAdvancedEXT = (PFN_vkCmdSetColorBlendAdvancedEXT) gpa(obj, "vkCmdSetColorBlendAdvancedEXT");
    table->CmdSetProvokingVertexModeEXT = (PFN_vkCmdSetProvokingVertexModeEXT) gpa(obj, "vkCmdSetProvokingVertexModeEXT");
    table->CmdSetLineRasterizationModeEXT = (PFN_vkCmdSetLineRasterizationModeEXT) gpa(obj, "vkCmdSetLineRasterizationModeEXT");
    table->CmdSetLineStippleEnableEXT = (PFN_vkCmdSetLineStippleEnableEXT) gpa(obj, "vkCmdSetLineStippleEnableEXT");
    table->CmdSetDepthClipNegativeOneToOneEXT = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT) gpa(obj, "vkCmdSetDepthClipNegativeOneToOneEXT");
    table->CmdSetViewportWScalingEnableNV = (PFN_vkCmdSetViewportWScalingEnableNV) gpa(obj, "vkCmdSetViewportWScalingEnableNV");
    table->CmdSetViewportSwizzleNV = (PFN_vkCmdSetViewportSwizzleNV) gpa(obj, "vkCmdSetViewportSwizzleNV");
    table->CmdSetCoverageToColorEnableNV = (PFN_vkCmdSetCoverageToColorEnableNV) gpa(obj, "vkCmdSetCoverageToColorEnableNV");
    table->CmdSetCoverageToColorLocationNV = (PFN_vkCmdSetCoverageToColorLocationNV) gpa(obj, "vkCmdSetCoverageToColorLocationNV");
    table->CmdSetCoverageModulationModeNV = (PFN_vkCmdSetCoverageModulationModeNV) gpa(obj, "vkCmdSetCoverageModulationModeNV");
    table->CmdSetCoverageModulationTableEnableNV = (PFN_vkCmdSetCoverageModulationTableEnableNV) gpa(obj, "vkCmdSetCoverageModulationTableEnableNV");
    table->CmdSetCoverageModulationTableNV = (PFN_vkCmdSetCoverageModulationTableNV) gpa(obj, "vkCmdSetCoverageModulationTableNV");
    table->CmdSetShadingRateImageEnableNV = (PFN_vkCmdSetShadingRateImageEnableNV) gpa(obj, "vkCmdSetShadingRateImageEnableNV");
    table->CmdSetCoverageReductionModeNV = (PFN_vkCmdSetCoverageReductionModeNV) gpa(obj, "vkCmdSetCoverageReductionModeNV");
    table->CmdSetRepresentativeFragmentTestEnableNV = (PFN_vkCmdSetRepresentativeFragmentTestEnableNV) gpa(obj, "vkCmdSetRepresentativeFragmentTestEnableNV");
    table->CreatePrivateDataSlot = (PFN_vkCreatePrivateDataSlot) gpa(obj, "vkCreatePrivateDataSlot");
    if (table->CreatePrivateDataSlot == NULL) {
        table->CreatePrivateDataSlot = (PFN_vkCreatePrivateDataSlot) gpa(obj, "vkCreatePrivateDataSlotEXT");
    }
        table->DestroyPrivateDataSlot = (PFN_vkDestroyPrivateDataSlot) gpa(obj, "vkDestroyPrivateDataSlot");
    if (table->DestroyPrivateDataSlot == NULL) {
        table->DestroyPrivateDataSlot = (PFN_vkDestroyPrivateDataSlot) gpa(obj, "vkDestroyPrivateDataSlotEXT");
    }
        table->SetPrivateData = (PFN_vkSetPrivateData) gpa(obj, "vkSetPrivateData");
    if (table->SetPrivateData == NULL) {
        table->SetPrivateData = (PFN_vkSetPrivateData) gpa(obj, "vkSetPrivateDataEXT");
    }
        table->GetPrivateData = (PFN_vkGetPrivateData) gpa(obj, "vkGetPrivateData");
    if (table->GetPrivateData == NULL) {
        table->GetPrivateData = (PFN_vkGetPrivateData) gpa(obj, "vkGetPrivateDataEXT");
    }
        table->CmdCopyBuffer2 = (PFN_vkCmdCopyBuffer2) gpa(obj, "vkCmdCopyBuffer2");
    if (table->CmdCopyBuffer2 == NULL) {
        table->CmdCopyBuffer2 = (PFN_vkCmdCopyBuffer2) gpa(obj, "vkCmdCopyBuffer2KHR");
    }
        table->CmdCopyImage2 = (PFN_vkCmdCopyImage2) gpa(obj, "vkCmdCopyImage2");
    if (table->CmdCopyImage2 == NULL) {
        table->CmdCopyImage2 = (PFN_vkCmdCopyImage2) gpa(obj, "vkCmdCopyImage2KHR");
    }
        table->CmdBlitImage2 = (PFN_vkCmdBlitImage2) gpa(obj, "vkCmdBlitImage2");
    if (table->CmdBlitImage2 == NULL) {
        table->CmdBlitImage2 = (PFN_vkCmdBlitImage2) gpa(obj, "vkCmdBlitImage2KHR");
    }
        table->CmdCopyBufferToImage2 = (PFN_vkCmdCopyBufferToImage2) gpa(obj, "vkCmdCopyBufferToImage2");
    if (table->CmdCopyBufferToImage2 == NULL) {
        table->CmdCopyBufferToImage2 = (PFN_vkCmdCopyBufferToImage2) gpa(obj, "vkCmdCopyBufferToImage2KHR");
    }
        table->CmdCopyImageToBuffer2 = (PFN_vkCmdCopyImageToBuffer2) gpa(obj, "vkCmdCopyImageToBuffer2");
    if (table->CmdCopyImageToBuffer2 == NULL) {
        table->CmdCopyImageToBuffer2 = (PFN_vkCmdCopyImageToBuffer2) gpa(obj, "vkCmdCopyImageToBuffer2KHR");
    }
        table->CmdResolveImage2 = (PFN_vkCmdResolveImage2) gpa(obj, "vkCmdResolveImage2");
    if (table->CmdResolveImage2 == NULL) {
        table->CmdResolveImage2 = (PFN_vkCmdResolveImage2) gpa(obj, "vkCmdResolveImage2KHR");
    }
        table->CmdSetFragmentShadingRateKHR = (PFN_vkCmdSetFragmentShadingRateKHR) gpa(obj, "vkCmdSetFragmentShadingRateKHR");
    table->CmdSetFragmentShadingRateEnumNV = (PFN_vkCmdSetFragmentShadingRateEnumNV) gpa(obj, "vkCmdSetFragmentShadingRateEnumNV");
    table->GetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR) gpa(obj, "vkGetAccelerationStructureBuildSizesKHR");
    table->CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT) gpa(obj, "vkCmdSetVertexInputEXT");
    table->CmdSetColorWriteEnableEXT = (PFN_vkCmdSetColorWriteEnableEXT) gpa(obj, "vkCmdSetColorWriteEnableEXT");
    table->CmdSetEvent2 = (PFN_vkCmdSetEvent2) gpa(obj, "vkCmdSetEvent2");
    if (table->CmdSetEvent2 == NULL) {
        table->CmdSetEvent2 = (PFN_vkCmdSetEvent2) gpa(obj, "vkCmdSetEvent2KHR");
    }
        table->CmdResetEvent2 = (PFN_vkCmdResetEvent2) gpa(obj, "vkCmdResetEvent2");
    if (table->CmdResetEvent2 == NULL) {
        table->CmdResetEvent2 = (PFN_vkCmdResetEvent2) gpa(obj, "vkCmdResetEvent2KHR");
    }
        table->CmdWaitEvents2 = (PFN_vkCmdWaitEvents2) gpa(obj, "vkCmdWaitEvents2");
    if (table->CmdWaitEvents2 == NULL) {
        table->CmdWaitEvents2 = (PFN_vkCmdWaitEvents2) gpa(obj, "vkCmdWaitEvents2KHR");
    }
        table->CmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) gpa(obj, "vkCmdPipelineBarrier2");
    if (table->CmdPipelineBarrier2 == NULL) {
        table->CmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) gpa(obj, "vkCmdPipelineBarrier2KHR");
    }
        table->QueueSubmit2 = (PFN_vkQueueSubmit2) gpa(obj, "vkQueueSubmit2");
    if (table->QueueSubmit2 == NULL) {
        table->QueueSubmit2 = (PFN_vkQueueSubmit2) gpa(obj, "vkQueueSubmit2KHR");
    }
        table->CmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2) gpa(obj, "vkCmdWriteTimestamp2");
    if (table->CmdWriteTimestamp2 == NULL) {
        table->CmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2) gpa(obj, "vkCmdWriteTimestamp2KHR");
    }
        table->CmdWriteBufferMarker2AMD = (PFN_vkCmdWriteBufferMarker2AMD) gpa(obj, "vkCmdWriteBufferMarker2AMD");
    table->GetQueueCheckpointData2NV = (PFN_vkGetQueueCheckpointData2NV) gpa(obj, "vkGetQueueCheckpointData2NV");
    table->CopyMemoryToImage = (PFN_vkCopyMemoryToImage) gpa(obj, "vkCopyMemoryToImage");
    if (table->CopyMemoryToImage == NULL) {
        table->CopyMemoryToImage = (PFN_vkCopyMemoryToImage) gpa(obj, "vkCopyMemoryToImageEXT");
    }
        table->CopyImageToMemory = (PFN_vkCopyImageToMemory) gpa(obj, "vkCopyImageToMemory");
    if (table->CopyImageToMemory == NULL) {
        table->CopyImageToMemory = (PFN_vkCopyImageToMemory) gpa(obj, "vkCopyImageToMemoryEXT");
    }
        table->CopyImageToImage = (PFN_vkCopyImageToImage) gpa(obj, "vkCopyImageToImage");
    if (table->CopyImageToImage == NULL) {
        table->CopyImageToImage = (PFN_vkCopyImageToImage) gpa(obj, "vkCopyImageToImageEXT");
    }
        table->TransitionImageLayout = (PFN_vkTransitionImageLayout) gpa(obj, "vkTransitionImageLayout");
    if (table->TransitionImageLayout == NULL) {
        table->TransitionImageLayout = (PFN_vkTransitionImageLayout) gpa(obj, "vkTransitionImageLayoutEXT");
    }
        table->CreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR) gpa(obj, "vkCreateVideoSessionKHR");
    table->DestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR) gpa(obj, "vkDestroyVideoSessionKHR");
    table->CreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR) gpa(obj, "vkCreateVideoSessionParametersKHR");
    table->UpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR) gpa(obj, "vkUpdateVideoSessionParametersKHR");
    table->GetEncodedVideoSessionParametersKHR = (PFN_vkGetEncodedVideoSessionParametersKHR) gpa(obj, "vkGetEncodedVideoSessionParametersKHR");
    table->DestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR) gpa(obj, "vkDestroyVideoSessionParametersKHR");
    table->GetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR) gpa(obj, "vkGetVideoSessionMemoryRequirementsKHR");
    table->BindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR) gpa(obj, "vkBindVideoSessionMemoryKHR");
    table->CmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR) gpa(obj, "vkCmdDecodeVideoKHR");
    table->CmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR) gpa(obj, "vkCmdBeginVideoCodingKHR");
    table->CmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR) gpa(obj, "vkCmdControlVideoCodingKHR");
    table->CmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR) gpa(obj, "vkCmdEndVideoCodingKHR");
    table->CmdEncodeVideoKHR = (PFN_vkCmdEncodeVideoKHR) gpa(obj, "vkCmdEncodeVideoKHR");
    table->CmdDecompressMemoryNV = (PFN_vkCmdDecompressMemoryNV) gpa(obj, "vkCmdDecompressMemoryNV");
    table->CmdDecompressMemoryIndirectCountNV = (PFN_vkCmdDecompressMemoryIndirectCountNV) gpa(obj, "vkCmdDecompressMemoryIndirectCountNV");
    table->GetPartitionedAccelerationStructuresBuildSizesNV = (PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV) gpa(obj, "vkGetPartitionedAccelerationStructuresBuildSizesNV");
    table->CmdBuildPartitionedAccelerationStructuresNV = (PFN_vkCmdBuildPartitionedAccelerationStructuresNV) gpa(obj, "vkCmdBuildPartitionedAccelerationStructuresNV");
    table->CmdDecompressMemoryEXT = (PFN_vkCmdDecompressMemoryEXT) gpa(obj, "vkCmdDecompressMemoryEXT");
    table->CmdDecompressMemoryIndirectCountEXT = (PFN_vkCmdDecompressMemoryIndirectCountEXT) gpa(obj, "vkCmdDecompressMemoryIndirectCountEXT");
    table->CreateCuModuleNVX = (PFN_vkCreateCuModuleNVX) gpa(obj, "vkCreateCuModuleNVX");
    table->CreateCuFunctionNVX = (PFN_vkCreateCuFunctionNVX) gpa(obj, "vkCreateCuFunctionNVX");
    table->DestroyCuModuleNVX = (PFN_vkDestroyCuModuleNVX) gpa(obj, "vkDestroyCuModuleNVX");
    table->DestroyCuFunctionNVX = (PFN_vkDestroyCuFunctionNVX) gpa(obj, "vkDestroyCuFunctionNVX");
    table->CmdCuLaunchKernelNVX = (PFN_vkCmdCuLaunchKernelNVX) gpa(obj, "vkCmdCuLaunchKernelNVX");
    table->GetDescriptorSetLayoutSizeEXT = (PFN_vkGetDescriptorSetLayoutSizeEXT) gpa(obj, "vkGetDescriptorSetLayoutSizeEXT");
    table->GetDescriptorSetLayoutBindingOffsetEXT = (PFN_vkGetDescriptorSetLayoutBindingOffsetEXT) gpa(obj, "vkGetDescriptorSetLayoutBindingOffsetEXT");
    table->GetDescriptorEXT = (PFN_vkGetDescriptorEXT) gpa(obj, "vkGetDescriptorEXT");
    table->CmdBindDescriptorBuffersEXT = (PFN_vkCmdBindDescriptorBuffersEXT) gpa(obj, "vkCmdBindDescriptorBuffersEXT");
    table->CmdSetDescriptorBufferOffsetsEXT = (PFN_vkCmdSetDescriptorBufferOffsetsEXT) gpa(obj, "vkCmdSetDescriptorBufferOffsetsEXT");
    table->CmdBindDescriptorBufferEmbeddedSamplersEXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT) gpa(obj, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
    table->GetBufferOpaqueCaptureDescriptorDataEXT = (PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetBufferOpaqueCaptureDescriptorDataEXT");
    table->GetImageOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetImageOpaqueCaptureDescriptorDataEXT");
    table->GetImageViewOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetImageViewOpaqueCaptureDescriptorDataEXT");
    table->GetSamplerOpaqueCaptureDescriptorDataEXT = (PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetSamplerOpaqueCaptureDescriptorDataEXT");
    table->GetAccelerationStructureOpaqueCaptureDescriptorDataEXT = (PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT");
    table->SetDeviceMemoryPriorityEXT = (PFN_vkSetDeviceMemoryPriorityEXT) gpa(obj, "vkSetDeviceMemoryPriorityEXT");
    table->WaitForPresent2KHR = (PFN_vkWaitForPresent2KHR) gpa(obj, "vkWaitForPresent2KHR");
    table->WaitForPresentKHR = (PFN_vkWaitForPresentKHR) gpa(obj, "vkWaitForPresentKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->CreateBufferCollectionFUCHSIA = (PFN_vkCreateBufferCollectionFUCHSIA) gpa(obj, "vkCreateBufferCollectionFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->SetBufferCollectionBufferConstraintsFUCHSIA = (PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA) gpa(obj, "vkSetBufferCollectionBufferConstraintsFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->SetBufferCollectionImageConstraintsFUCHSIA = (PFN_vkSetBufferCollectionImageConstraintsFUCHSIA) gpa(obj, "vkSetBufferCollectionImageConstraintsFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->DestroyBufferCollectionFUCHSIA = (PFN_vkDestroyBufferCollectionFUCHSIA) gpa(obj, "vkDestroyBufferCollectionFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetBufferCollectionPropertiesFUCHSIA = (PFN_vkGetBufferCollectionPropertiesFUCHSIA) gpa(obj, "vkGetBufferCollectionPropertiesFUCHSIA");
#endif
    table->CmdBeginRendering = (PFN_vkCmdBeginRendering) gpa(obj, "vkCmdBeginRendering");
    if (table->CmdBeginRendering == NULL) {
        table->CmdBeginRendering = (PFN_vkCmdBeginRendering) gpa(obj, "vkCmdBeginRenderingKHR");
    }
        table->CmdEndRendering = (PFN_vkCmdEndRendering) gpa(obj, "vkCmdEndRendering");
    if (table->CmdEndRendering == NULL) {
        table->CmdEndRendering = (PFN_vkCmdEndRendering) gpa(obj, "vkCmdEndRenderingKHR");
    }
    table->CmdEndRendering2KHR = (PFN_vkCmdEndRendering2KHR) gpa(obj, "vkCmdEndRendering2KHR");
    if (table->CmdEndRendering2KHR == NULL) {
        table->CmdEndRendering2KHR = (PFN_vkCmdEndRendering2KHR) gpa(obj, "vkCmdEndRendering2EXT");
    }
            table->GetDescriptorSetLayoutHostMappingInfoVALVE = (PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE) gpa(obj, "vkGetDescriptorSetLayoutHostMappingInfoVALVE");
    table->GetDescriptorSetHostMappingVALVE = (PFN_vkGetDescriptorSetHostMappingVALVE) gpa(obj, "vkGetDescriptorSetHostMappingVALVE");
    table->CreateMicromapEXT = (PFN_vkCreateMicromapEXT) gpa(obj, "vkCreateMicromapEXT");
    table->CmdBuildMicromapsEXT = (PFN_vkCmdBuildMicromapsEXT) gpa(obj, "vkCmdBuildMicromapsEXT");
    table->BuildMicromapsEXT = (PFN_vkBuildMicromapsEXT) gpa(obj, "vkBuildMicromapsEXT");
    table->DestroyMicromapEXT = (PFN_vkDestroyMicromapEXT) gpa(obj, "vkDestroyMicromapEXT");
    table->CmdCopyMicromapEXT = (PFN_vkCmdCopyMicromapEXT) gpa(obj, "vkCmdCopyMicromapEXT");
    table->CopyMicromapEXT = (PFN_vkCopyMicromapEXT) gpa(obj, "vkCopyMicromapEXT");
    table->CmdCopyMicromapToMemoryEXT = (PFN_vkCmdCopyMicromapToMemoryEXT) gpa(obj, "vkCmdCopyMicromapToMemoryEXT");
    table->CopyMicromapToMemoryEXT = (PFN_vkCopyMicromapToMemoryEXT) gpa(obj, "vkCopyMicromapToMemoryEXT");
    table->CmdCopyMemoryToMicromapEXT = (PFN_vkCmdCopyMemoryToMicromapEXT) gpa(obj, "vkCmdCopyMemoryToMicromapEXT");
    table->CopyMemoryToMicromapEXT = (PFN_vkCopyMemoryToMicromapEXT) gpa(obj, "vkCopyMemoryToMicromapEXT");
    table->CmdWriteMicromapsPropertiesEXT = (PFN_vkCmdWriteMicromapsPropertiesEXT) gpa(obj, "vkCmdWriteMicromapsPropertiesEXT");
    table->WriteMicromapsPropertiesEXT = (PFN_vkWriteMicromapsPropertiesEXT) gpa(obj, "vkWriteMicromapsPropertiesEXT");
    table->GetDeviceMicromapCompatibilityEXT = (PFN_vkGetDeviceMicromapCompatibilityEXT) gpa(obj, "vkGetDeviceMicromapCompatibilityEXT");
    table->GetMicromapBuildSizesEXT = (PFN_vkGetMicromapBuildSizesEXT) gpa(obj, "vkGetMicromapBuildSizesEXT");
    table->GetShaderModuleIdentifierEXT = (PFN_vkGetShaderModuleIdentifierEXT) gpa(obj, "vkGetShaderModuleIdentifierEXT");
    table->GetShaderModuleCreateInfoIdentifierEXT = (PFN_vkGetShaderModuleCreateInfoIdentifierEXT) gpa(obj, "vkGetShaderModuleCreateInfoIdentifierEXT");
    table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) gpa(obj, "vkGetImageSubresourceLayout2");
    if (table->GetImageSubresourceLayout2 == NULL) {
        table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) gpa(obj, "vkGetImageSubresourceLayout2KHR");
    }
    if (table->GetImageSubresourceLayout2 == NULL) {
        table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) gpa(obj, "vkGetImageSubresourceLayout2EXT");
    }
            table->GetPipelinePropertiesEXT = (PFN_vkGetPipelinePropertiesEXT) gpa(obj, "vkGetPipelinePropertiesEXT");
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->ExportMetalObjectsEXT = (PFN_vkExportMetalObjectsEXT) gpa(obj, "vkExportMetalObjectsEXT");
#endif
    table->CmdBindTileMemoryQCOM = (PFN_vkCmdBindTileMemoryQCOM) gpa(obj, "vkCmdBindTileMemoryQCOM");
    table->GetFramebufferTilePropertiesQCOM = (PFN_vkGetFramebufferTilePropertiesQCOM) gpa(obj, "vkGetFramebufferTilePropertiesQCOM");
    table->GetDynamicRenderingTilePropertiesQCOM = (PFN_vkGetDynamicRenderingTilePropertiesQCOM) gpa(obj, "vkGetDynamicRenderingTilePropertiesQCOM");
    table->CreateOpticalFlowSessionNV = (PFN_vkCreateOpticalFlowSessionNV) gpa(obj, "vkCreateOpticalFlowSessionNV");
    table->DestroyOpticalFlowSessionNV = (PFN_vkDestroyOpticalFlowSessionNV) gpa(obj, "vkDestroyOpticalFlowSessionNV");
    table->BindOpticalFlowSessionImageNV = (PFN_vkBindOpticalFlowSessionImageNV) gpa(obj, "vkBindOpticalFlowSessionImageNV");
    table->CmdOpticalFlowExecuteNV = (PFN_vkCmdOpticalFlowExecuteNV) gpa(obj, "vkCmdOpticalFlowExecuteNV");
    table->GetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT) gpa(obj, "vkGetDeviceFaultInfoEXT");
    table->GetDeviceFaultReportsKHR = (PFN_vkGetDeviceFaultReportsKHR) gpa(obj, "vkGetDeviceFaultReportsKHR");
    table->GetDeviceFaultDebugInfoKHR = (PFN_vkGetDeviceFaultDebugInfoKHR) gpa(obj, "vkGetDeviceFaultDebugInfoKHR");
    table->CmdSetDepthBias2EXT = (PFN_vkCmdSetDepthBias2EXT) gpa(obj, "vkCmdSetDepthBias2EXT");
    table->ReleaseSwapchainImagesKHR = (PFN_vkReleaseSwapchainImagesKHR) gpa(obj, "vkReleaseSwapchainImagesKHR");
    if (table->ReleaseSwapchainImagesKHR == NULL) {
        table->ReleaseSwapchainImagesKHR = (PFN_vkReleaseSwapchainImagesKHR) gpa(obj, "vkReleaseSwapchainImagesEXT");
    }
        table->GetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout) gpa(obj, "vkGetDeviceImageSubresourceLayout");
    if (table->GetDeviceImageSubresourceLayout == NULL) {
        table->GetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout) gpa(obj, "vkGetDeviceImageSubresourceLayoutKHR");
    }
        table->MapMemory2 = (PFN_vkMapMemory2) gpa(obj, "vkMapMemory2");
    if (table->MapMemory2 == NULL) {
        table->MapMemory2 = (PFN_vkMapMemory2) gpa(obj, "vkMapMemory2KHR");
    }
        table->UnmapMemory2 = (PFN_vkUnmapMemory2) gpa(obj, "vkUnmapMemory2");
    if (table->UnmapMemory2 == NULL) {
        table->UnmapMemory2 = (PFN_vkUnmapMemory2) gpa(obj, "vkUnmapMemory2KHR");
    }
        table->CreateShadersEXT = (PFN_vkCreateShadersEXT) gpa(obj, "vkCreateShadersEXT");
    table->DestroyShaderEXT = (PFN_vkDestroyShaderEXT) gpa(obj, "vkDestroyShaderEXT");
    table->GetShaderBinaryDataEXT = (PFN_vkGetShaderBinaryDataEXT) gpa(obj, "vkGetShaderBinaryDataEXT");
    table->CmdBindShadersEXT = (PFN_vkCmdBindShadersEXT) gpa(obj, "vkCmdBindShadersEXT");
    table->SetSwapchainPresentTimingQueueSizeEXT = (PFN_vkSetSwapchainPresentTimingQueueSizeEXT) gpa(obj, "vkSetSwapchainPresentTimingQueueSizeEXT");
    table->GetSwapchainTimingPropertiesEXT = (PFN_vkGetSwapchainTimingPropertiesEXT) gpa(obj, "vkGetSwapchainTimingPropertiesEXT");
    table->GetSwapchainTimeDomainPropertiesEXT = (PFN_vkGetSwapchainTimeDomainPropertiesEXT) gpa(obj, "vkGetSwapchainTimeDomainPropertiesEXT");
    table->GetPastPresentationTimingEXT = (PFN_vkGetPastPresentationTimingEXT) gpa(obj, "vkGetPastPresentationTimingEXT");
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->GetScreenBufferPropertiesQNX = (PFN_vkGetScreenBufferPropertiesQNX) gpa(obj, "vkGetScreenBufferPropertiesQNX");
#endif
    table->CreateGpaSessionAMD = (PFN_vkCreateGpaSessionAMD) gpa(obj, "vkCreateGpaSessionAMD");
    table->DestroyGpaSessionAMD = (PFN_vkDestroyGpaSessionAMD) gpa(obj, "vkDestroyGpaSessionAMD");
    table->SetGpaDeviceClockModeAMD = (PFN_vkSetGpaDeviceClockModeAMD) gpa(obj, "vkSetGpaDeviceClockModeAMD");
    table->GetGpaDeviceClockInfoAMD = (PFN_vkGetGpaDeviceClockInfoAMD) gpa(obj, "vkGetGpaDeviceClockInfoAMD");
    table->CmdBeginGpaSessionAMD = (PFN_vkCmdBeginGpaSessionAMD) gpa(obj, "vkCmdBeginGpaSessionAMD");
    table->CmdEndGpaSessionAMD = (PFN_vkCmdEndGpaSessionAMD) gpa(obj, "vkCmdEndGpaSessionAMD");
    table->CmdBeginGpaSampleAMD = (PFN_vkCmdBeginGpaSampleAMD) gpa(obj, "vkCmdBeginGpaSampleAMD");
    table->CmdEndGpaSampleAMD = (PFN_vkCmdEndGpaSampleAMD) gpa(obj, "vkCmdEndGpaSampleAMD");
    table->GetGpaSessionStatusAMD = (PFN_vkGetGpaSessionStatusAMD) gpa(obj, "vkGetGpaSessionStatusAMD");
    table->GetGpaSessionResultsAMD = (PFN_vkGetGpaSessionResultsAMD) gpa(obj, "vkGetGpaSessionResultsAMD");
    table->ResetGpaSessionAMD = (PFN_vkResetGpaSessionAMD) gpa(obj, "vkResetGpaSessionAMD");
    table->CmdCopyGpaSessionResultsAMD = (PFN_vkCmdCopyGpaSessionResultsAMD) gpa(obj, "vkCmdCopyGpaSessionResultsAMD");
    table->CmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2) gpa(obj, "vkCmdBindDescriptorSets2");
    if (table->CmdBindDescriptorSets2 == NULL) {
        table->CmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2) gpa(obj, "vkCmdBindDescriptorSets2KHR");
    }
        table->CmdPushConstants2 = (PFN_vkCmdPushConstants2) gpa(obj, "vkCmdPushConstants2");
    if (table->CmdPushConstants2 == NULL) {
        table->CmdPushConstants2 = (PFN_vkCmdPushConstants2) gpa(obj, "vkCmdPushConstants2KHR");
    }
        table->CmdPushDescriptorSet2 = (PFN_vkCmdPushDescriptorSet2) gpa(obj, "vkCmdPushDescriptorSet2");
    if (table->CmdPushDescriptorSet2 == NULL) {
        table->CmdPushDescriptorSet2 = (PFN_vkCmdPushDescriptorSet2) gpa(obj, "vkCmdPushDescriptorSet2KHR");
    }
        table->CmdPushDescriptorSetWithTemplate2 = (PFN_vkCmdPushDescriptorSetWithTemplate2) gpa(obj, "vkCmdPushDescriptorSetWithTemplate2");
    if (table->CmdPushDescriptorSetWithTemplate2 == NULL) {
        table->CmdPushDescriptorSetWithTemplate2 = (PFN_vkCmdPushDescriptorSetWithTemplate2) gpa(obj, "vkCmdPushDescriptorSetWithTemplate2KHR");
    }
        table->CmdSetDescriptorBufferOffsets2EXT = (PFN_vkCmdSetDescriptorBufferOffsets2EXT) gpa(obj, "vkCmdSetDescriptorBufferOffsets2EXT");
    table->CmdBindDescriptorBufferEmbeddedSamplers2EXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT) gpa(obj, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT");
    table->SetLatencySleepModeNV = (PFN_vkSetLatencySleepModeNV) gpa(obj, "vkSetLatencySleepModeNV");
    table->LatencySleepNV = (PFN_vkLatencySleepNV) gpa(obj, "vkLatencySleepNV");
    table->SetLatencyMarkerNV = (PFN_vkSetLatencyMarkerNV) gpa(obj, "vkSetLatencyMarkerNV");
    table->GetLatencyTimingsNV = (PFN_vkGetLatencyTimingsNV) gpa(obj, "vkGetLatencyTimingsNV");
    table->QueueNotifyOutOfBandNV = (PFN_vkQueueNotifyOutOfBandNV) gpa(obj, "vkQueueNotifyOutOfBandNV");
    table->CmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations) gpa(obj, "vkCmdSetRenderingAttachmentLocations");
    if (table->CmdSetRenderingAttachmentLocations == NULL) {
        table->CmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations) gpa(obj, "vkCmdSetRenderingAttachmentLocationsKHR");
    }
        table->CmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices) gpa(obj, "vkCmdSetRenderingInputAttachmentIndices");
    if (table->CmdSetRenderingInputAttachmentIndices == NULL) {
        table->CmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices) gpa(obj, "vkCmdSetRenderingInputAttachmentIndicesKHR");
    }
        table->CmdSetDepthClampRangeEXT = (PFN_vkCmdSetDepthClampRangeEXT) gpa(obj, "vkCmdSetDepthClampRangeEXT");
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->GetMemoryMetalHandleEXT = (PFN_vkGetMemoryMetalHandleEXT) gpa(obj, "vkGetMemoryMetalHandleEXT");
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->GetMemoryMetalHandlePropertiesEXT = (PFN_vkGetMemoryMetalHandlePropertiesEXT) gpa(obj, "vkGetMemoryMetalHandlePropertiesEXT");
#endif
    table->ConvertCooperativeVectorMatrixNV = (PFN_vkConvertCooperativeVectorMatrixNV) gpa(obj, "vkConvertCooperativeVectorMatrixNV");
    table->CmdConvertCooperativeVectorMatrixNV = (PFN_vkCmdConvertCooperativeVectorMatrixNV) gpa(obj, "vkCmdConvertCooperativeVectorMatrixNV");
    table->CmdDispatchTileQCOM = (PFN_vkCmdDispatchTileQCOM) gpa(obj, "vkCmdDispatchTileQCOM");
    table->CmdBeginPerTileExecutionQCOM = (PFN_vkCmdBeginPerTileExecutionQCOM) gpa(obj, "vkCmdBeginPerTileExecutionQCOM");
    table->CmdEndPerTileExecutionQCOM = (PFN_vkCmdEndPerTileExecutionQCOM) gpa(obj, "vkCmdEndPerTileExecutionQCOM");
    table->CreateExternalComputeQueueNV = (PFN_vkCreateExternalComputeQueueNV) gpa(obj, "vkCreateExternalComputeQueueNV");
    table->DestroyExternalComputeQueueNV = (PFN_vkDestroyExternalComputeQueueNV) gpa(obj, "vkDestroyExternalComputeQueueNV");
    table->CreateShaderInstrumentationARM = (PFN_vkCreateShaderInstrumentationARM) gpa(obj, "vkCreateShaderInstrumentationARM");
    table->DestroyShaderInstrumentationARM = (PFN_vkDestroyShaderInstrumentationARM) gpa(obj, "vkDestroyShaderInstrumentationARM");
    table->CmdBeginShaderInstrumentationARM = (PFN_vkCmdBeginShaderInstrumentationARM) gpa(obj, "vkCmdBeginShaderInstrumentationARM");
    table->CmdEndShaderInstrumentationARM = (PFN_vkCmdEndShaderInstrumentationARM) gpa(obj, "vkCmdEndShaderInstrumentationARM");
    table->GetShaderInstrumentationValuesARM = (PFN_vkGetShaderInstrumentationValuesARM) gpa(obj, "vkGetShaderInstrumentationValuesARM");
    table->ClearShaderInstrumentationMetricsARM = (PFN_vkClearShaderInstrumentationMetricsARM) gpa(obj, "vkClearShaderInstrumentationMetricsARM");
    table->CreateTensorARM = (PFN_vkCreateTensorARM) gpa(obj, "vkCreateTensorARM");
    table->DestroyTensorARM = (PFN_vkDestroyTensorARM) gpa(obj, "vkDestroyTensorARM");
    table->CreateTensorViewARM = (PFN_vkCreateTensorViewARM) gpa(obj, "vkCreateTensorViewARM");
    table->DestroyTensorViewARM = (PFN_vkDestroyTensorViewARM) gpa(obj, "vkDestroyTensorViewARM");
    table->GetTensorMemoryRequirementsARM = (PFN_vkGetTensorMemoryRequirementsARM) gpa(obj, "vkGetTensorMemoryRequirementsARM");
    table->BindTensorMemoryARM = (PFN_vkBindTensorMemoryARM) gpa(obj, "vkBindTensorMemoryARM");
    table->GetDeviceTensorMemoryRequirementsARM = (PFN_vkGetDeviceTensorMemoryRequirementsARM) gpa(obj, "vkGetDeviceTensorMemoryRequirementsARM");
    table->CmdCopyTensorARM = (PFN_vkCmdCopyTensorARM) gpa(obj, "vkCmdCopyTensorARM");
    table->GetTensorOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorOpaqueCaptureDescriptorDataARM) gpa(obj, "vkGetTensorOpaqueCaptureDescriptorDataARM");
    table->GetTensorViewOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM) gpa(obj, "vkGetTensorViewOpaqueCaptureDescriptorDataARM");
    table->CreateDataGraphPipelinesARM = (PFN_vkCreateDataGraphPipelinesARM) gpa(obj, "vkCreateDataGraphPipelinesARM");
    table->CreateDataGraphPipelineSessionARM = (PFN_vkCreateDataGraphPipelineSessionARM) gpa(obj, "vkCreateDataGraphPipelineSessionARM");
    table->GetDataGraphPipelineSessionBindPointRequirementsARM = (PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM) gpa(obj, "vkGetDataGraphPipelineSessionBindPointRequirementsARM");
    table->GetDataGraphPipelineSessionMemoryRequirementsARM = (PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM) gpa(obj, "vkGetDataGraphPipelineSessionMemoryRequirementsARM");
    table->BindDataGraphPipelineSessionMemoryARM = (PFN_vkBindDataGraphPipelineSessionMemoryARM) gpa(obj, "vkBindDataGraphPipelineSessionMemoryARM");
    table->DestroyDataGraphPipelineSessionARM = (PFN_vkDestroyDataGraphPipelineSessionARM) gpa(obj, "vkDestroyDataGraphPipelineSessionARM");
    table->CmdDispatchDataGraphARM = (PFN_vkCmdDispatchDataGraphARM) gpa(obj, "vkCmdDispatchDataGraphARM");
    table->GetDataGraphPipelineAvailablePropertiesARM = (PFN_vkGetDataGraphPipelineAvailablePropertiesARM) gpa(obj, "vkGetDataGraphPipelineAvailablePropertiesARM");
    table->GetDataGraphPipelinePropertiesARM = (PFN_vkGetDataGraphPipelinePropertiesARM) gpa(obj, "vkGetDataGraphPipelinePropertiesARM");
#ifdef VK_USE_PLATFORM_OHOS
    table->GetNativeBufferPropertiesOHOS = (PFN_vkGetNativeBufferPropertiesOHOS) gpa(obj, "vkGetNativeBufferPropertiesOHOS");
#endif
#ifdef VK_USE_PLATFORM_OHOS
    table->GetMemoryNativeBufferOHOS = (PFN_vkGetMemoryNativeBufferOHOS) gpa(obj, "vkGetMemoryNativeBufferOHOS");
#endif
    table->QueueSetPerfHintQCOM = (PFN_vkQueueSetPerfHintQCOM) gpa(obj, "vkQueueSetPerfHintQCOM");
    table->CmdSetComputeOccupancyPriorityNV = (PFN_vkCmdSetComputeOccupancyPriorityNV) gpa(obj, "vkCmdSetComputeOccupancyPriorityNV");
    table->WriteSamplerDescriptorsEXT = (PFN_vkWriteSamplerDescriptorsEXT) gpa(obj, "vkWriteSamplerDescriptorsEXT");
    table->WriteResourceDescriptorsEXT = (PFN_vkWriteResourceDescriptorsEXT) gpa(obj, "vkWriteResourceDescriptorsEXT");
    table->CmdBindSamplerHeapEXT = (PFN_vkCmdBindSamplerHeapEXT) gpa(obj, "vkCmdBindSamplerHeapEXT");
    table->CmdBindResourceHeapEXT = (PFN_vkCmdBindResourceHeapEXT) gpa(obj, "vkCmdBindResourceHeapEXT");
    table->CmdPushDataEXT = (PFN_vkCmdPushDataEXT) gpa(obj, "vkCmdPushDataEXT");
    table->RegisterCustomBorderColorEXT = (PFN_vkRegisterCustomBorderColorEXT) gpa(obj, "vkRegisterCustomBorderColorEXT");
    table->UnregisterCustomBorderColorEXT = (PFN_vkUnregisterCustomBorderColorEXT) gpa(obj, "vkUnregisterCustomBorderColorEXT");
    table->GetImageOpaqueCaptureDataEXT = (PFN_vkGetImageOpaqueCaptureDataEXT) gpa(obj, "vkGetImageOpaqueCaptureDataEXT");
    table->GetTensorOpaqueCaptureDataARM = (PFN_vkGetTensorOpaqueCaptureDataARM) gpa(obj, "vkGetTensorOpaqueCaptureDataARM");
    table->CmdCopyMemoryKHR = (PFN_vkCmdCopyMemoryKHR) gpa(obj, "vkCmdCopyMemoryKHR");
    table->CmdCopyMemoryToImageKHR = (PFN_vkCmdCopyMemoryToImageKHR) gpa(obj, "vkCmdCopyMemoryToImageKHR");
    table->CmdCopyImageToMemoryKHR = (PFN_vkCmdCopyImageToMemoryKHR) gpa(obj, "vkCmdCopyImageToMemoryKHR");
    table->CmdUpdateMemoryKHR = (PFN_vkCmdUpdateMemoryKHR) gpa(obj, "vkCmdUpdateMemoryKHR");
    table->CmdFillMemoryKHR = (PFN_vkCmdFillMemoryKHR) gpa(obj, "vkCmdFillMemoryKHR");
    table->CmdCopyQueryPoolResultsToMemoryKHR = (PFN_vkCmdCopyQueryPoolResultsToMemoryKHR) gpa(obj, "vkCmdCopyQueryPoolResultsToMemoryKHR");
    table->CmdBeginConditionalRendering2EXT = (PFN_vkCmdBeginConditionalRendering2EXT) gpa(obj, "vkCmdBeginConditionalRendering2EXT");
    table->CmdBindTransformFeedbackBuffers2EXT = (PFN_vkCmdBindTransformFeedbackBuffers2EXT) gpa(obj, "vkCmdBindTransformFeedbackBuffers2EXT");
    table->CmdBeginTransformFeedback2EXT = (PFN_vkCmdBeginTransformFeedback2EXT) gpa(obj, "vkCmdBeginTransformFeedback2EXT");
    table->CmdEndTransformFeedback2EXT = (PFN_vkCmdEndTransformFeedback2EXT) gpa(obj, "vkCmdEndTransformFeedback2EXT");
    table->CmdDrawIndirectByteCount2EXT = (PFN_vkCmdDrawIndirectByteCount2EXT) gpa(obj, "vkCmdDrawIndirectByteCount2EXT");
    table->CmdWriteMarkerToMemoryAMD = (PFN_vkCmdWriteMarkerToMemoryAMD) gpa(obj, "vkCmdWriteMarkerToMemoryAMD");
    table->CmdBindIndexBuffer3KHR = (PFN_vkCmdBindIndexBuffer3KHR) gpa(obj, "vkCmdBindIndexBuffer3KHR");
    table->CmdBindVertexBuffers3KHR = (PFN_vkCmdBindVertexBuffers3KHR) gpa(obj, "vkCmdBindVertexBuffers3KHR");
    table->CmdDrawIndirect2KHR = (PFN_vkCmdDrawIndirect2KHR) gpa(obj, "vkCmdDrawIndirect2KHR");
    table->CmdDrawIndexedIndirect2KHR = (PFN_vkCmdDrawIndexedIndirect2KHR) gpa(obj, "vkCmdDrawIndexedIndirect2KHR");
    table->CmdDrawIndirectCount2KHR = (PFN_vkCmdDrawIndirectCount2KHR) gpa(obj, "vkCmdDrawIndirectCount2KHR");
    table->CmdDrawIndexedIndirectCount2KHR = (PFN_vkCmdDrawIndexedIndirectCount2KHR) gpa(obj, "vkCmdDrawIndexedIndirectCount2KHR");
    table->CmdDrawMeshTasksIndirect2EXT = (PFN_vkCmdDrawMeshTasksIndirect2EXT) gpa(obj, "vkCmdDrawMeshTasksIndirect2EXT");
    table->CmdDrawMeshTasksIndirectCount2EXT = (PFN_vkCmdDrawMeshTasksIndirectCount2EXT) gpa(obj, "vkCmdDrawMeshTasksIndirectCount2EXT");
    table->CmdDispatchIndirect2KHR = (PFN_vkCmdDispatchIndirect2KHR) gpa(obj, "vkCmdDispatchIndirect2KHR");
    table->CreateAccelerationStructure2KHR = (PFN_vkCreateAccelerationStructure2KHR) gpa(obj, "vkCreateAccelerationStructure2KHR");
    table->CmdSetDispatchParametersARM = (PFN_vkCmdSetDispatchParametersARM) gpa(obj, "vkCmdSetDispatchParametersARM");
}





void
vk_instance_uncompacted_dispatch_table_load(struct vk_instance_uncompacted_dispatch_table *table,
                               PFN_vkGetInstanceProcAddr gpa,
                               VkInstance obj)
{
    table->GetInstanceProcAddr = gpa;
    table->CreateInstance = (PFN_vkCreateInstance) gpa(obj, "vkCreateInstance");
    table->DestroyInstance = (PFN_vkDestroyInstance) gpa(obj, "vkDestroyInstance");
    table->EnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) gpa(obj, "vkEnumeratePhysicalDevices");
    table->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) gpa(obj, "vkGetInstanceProcAddr");
    table->EnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) gpa(obj, "vkEnumerateInstanceVersion");
    table->EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) gpa(obj, "vkEnumerateInstanceLayerProperties");
    table->EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) gpa(obj, "vkEnumerateInstanceExtensionProperties");
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->CreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR) gpa(obj, "vkCreateAndroidSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_OHOS
    table->CreateSurfaceOHOS = (PFN_vkCreateSurfaceOHOS) gpa(obj, "vkCreateSurfaceOHOS");
#endif
    table->CreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR) gpa(obj, "vkCreateDisplayPlaneSurfaceKHR");
    table->DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) gpa(obj, "vkDestroySurfaceKHR");
#ifdef VK_USE_PLATFORM_VI_NN
    table->CreateViSurfaceNN = (PFN_vkCreateViSurfaceNN) gpa(obj, "vkCreateViSurfaceNN");
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    table->CreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) gpa(obj, "vkCreateWaylandSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_UBM_SEC
    table->CreateUbmSurfaceSEC = (PFN_vkCreateUbmSurfaceSEC) gpa(obj, "vkCreateUbmSurfaceSEC");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) gpa(obj, "vkCreateWin32SurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    table->CreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) gpa(obj, "vkCreateXlibSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    table->CreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR) gpa(obj, "vkCreateXcbSurfaceKHR");
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    table->CreateDirectFBSurfaceEXT = (PFN_vkCreateDirectFBSurfaceEXT) gpa(obj, "vkCreateDirectFBSurfaceEXT");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->CreateImagePipeSurfaceFUCHSIA = (PFN_vkCreateImagePipeSurfaceFUCHSIA) gpa(obj, "vkCreateImagePipeSurfaceFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_GGP
    table->CreateStreamDescriptorSurfaceGGP = (PFN_vkCreateStreamDescriptorSurfaceGGP) gpa(obj, "vkCreateStreamDescriptorSurfaceGGP");
#endif
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->CreateScreenSurfaceQNX = (PFN_vkCreateScreenSurfaceQNX) gpa(obj, "vkCreateScreenSurfaceQNX");
#endif
    table->CreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) gpa(obj, "vkCreateDebugReportCallbackEXT");
    table->DestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) gpa(obj, "vkDestroyDebugReportCallbackEXT");
    table->DebugReportMessageEXT = (PFN_vkDebugReportMessageEXT) gpa(obj, "vkDebugReportMessageEXT");
    table->EnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) gpa(obj, "vkEnumeratePhysicalDeviceGroups");
    table->EnumeratePhysicalDeviceGroupsKHR = (PFN_vkEnumeratePhysicalDeviceGroupsKHR) gpa(obj, "vkEnumeratePhysicalDeviceGroupsKHR");
    if (table->EnumeratePhysicalDeviceGroups && !table->EnumeratePhysicalDeviceGroupsKHR)
       table->EnumeratePhysicalDeviceGroupsKHR = (PFN_vkEnumeratePhysicalDeviceGroupsKHR) table->EnumeratePhysicalDeviceGroups;
    if (!table->EnumeratePhysicalDeviceGroups)
       table->EnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) table->EnumeratePhysicalDeviceGroupsKHR;
    #ifdef VK_USE_PLATFORM_IOS_MVK
    table->CreateIOSSurfaceMVK = (PFN_vkCreateIOSSurfaceMVK) gpa(obj, "vkCreateIOSSurfaceMVK");
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    table->CreateMacOSSurfaceMVK = (PFN_vkCreateMacOSSurfaceMVK) gpa(obj, "vkCreateMacOSSurfaceMVK");
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->CreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT) gpa(obj, "vkCreateMetalSurfaceEXT");
#endif
    table->CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) gpa(obj, "vkCreateDebugUtilsMessengerEXT");
    table->DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) gpa(obj, "vkDestroyDebugUtilsMessengerEXT");
    table->SubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT) gpa(obj, "vkSubmitDebugUtilsMessageEXT");
    table->CreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT) gpa(obj, "vkCreateHeadlessSurfaceEXT");
#ifdef VK_USE_PLATFORM_PLAN9_9FRONT
    table->CreatePlan9SurfacePLAN9 = (PFN_vkCreatePlan9SurfacePLAN9) gpa(obj, "vkCreatePlan9SurfacePLAN9");
#endif
    table->GetExternalComputeQueueDataNV = (PFN_vkGetExternalComputeQueueDataNV) gpa(obj, "vkGetExternalComputeQueueDataNV");
}



void
vk_physical_device_uncompacted_dispatch_table_load(struct vk_physical_device_uncompacted_dispatch_table *table,
                               PFN_vkGetInstanceProcAddr gpa,
                               VkInstance obj)
{
    table->GetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) gpa(obj, "vkGetPhysicalDeviceProperties");
    table->GetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties");
    table->GetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) gpa(obj, "vkGetPhysicalDeviceMemoryProperties");
    table->GetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures) gpa(obj, "vkGetPhysicalDeviceFeatures");
    table->GetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties) gpa(obj, "vkGetPhysicalDeviceFormatProperties");
    table->GetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties");
    table->CreateDevice = (PFN_vkCreateDevice) gpa(obj, "vkCreateDevice");
    table->EnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties) gpa(obj, "vkEnumerateDeviceLayerProperties");
    table->EnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) gpa(obj, "vkEnumerateDeviceExtensionProperties");
    table->GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties");
    table->GetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceDisplayPropertiesKHR");
    table->GetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR) gpa(obj, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    table->GetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR) gpa(obj, "vkGetDisplayPlaneSupportedDisplaysKHR");
    table->GetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR) gpa(obj, "vkGetDisplayModePropertiesKHR");
    table->CreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR) gpa(obj, "vkCreateDisplayModeKHR");
    table->GetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR) gpa(obj, "vkGetDisplayPlaneCapabilitiesKHR");
    table->GetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceSupportKHR");
    table->GetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    table->GetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) gpa(obj, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    table->GetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) gpa(obj, "vkGetPhysicalDeviceSurfacePresentModesKHR");
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    table->GetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_UBM_SEC
    table->GetPhysicalDeviceUbmPresentationSupportSEC = (PFN_vkGetPhysicalDeviceUbmPresentationSupportSEC) gpa(obj, "vkGetPhysicalDeviceUbmPresentationSupportSEC");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    table->GetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    table->GetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR) gpa(obj, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    table->GetPhysicalDeviceDirectFBPresentationSupportEXT = (PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT) gpa(obj, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
#endif
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->GetPhysicalDeviceScreenPresentationSupportQNX = (PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX) gpa(obj, "vkGetPhysicalDeviceScreenPresentationSupportQNX");
#endif
    table->GetPhysicalDeviceExternalImageFormatPropertiesNV = (PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV) gpa(obj, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    table->GetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) gpa(obj, "vkGetPhysicalDeviceFeatures2");
    table->GetPhysicalDeviceFeatures2KHR = (PFN_vkGetPhysicalDeviceFeatures2KHR) gpa(obj, "vkGetPhysicalDeviceFeatures2KHR");
    if (table->GetPhysicalDeviceFeatures2 && !table->GetPhysicalDeviceFeatures2KHR)
       table->GetPhysicalDeviceFeatures2KHR = (PFN_vkGetPhysicalDeviceFeatures2KHR) table->GetPhysicalDeviceFeatures2;
    if (!table->GetPhysicalDeviceFeatures2)
       table->GetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) table->GetPhysicalDeviceFeatures2KHR;
        table->GetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) gpa(obj, "vkGetPhysicalDeviceProperties2");
    table->GetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) gpa(obj, "vkGetPhysicalDeviceProperties2KHR");
    if (table->GetPhysicalDeviceProperties2 && !table->GetPhysicalDeviceProperties2KHR)
       table->GetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) table->GetPhysicalDeviceProperties2;
    if (!table->GetPhysicalDeviceProperties2)
       table->GetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) table->GetPhysicalDeviceProperties2KHR;
        table->GetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) gpa(obj, "vkGetPhysicalDeviceFormatProperties2");
    table->GetPhysicalDeviceFormatProperties2KHR = (PFN_vkGetPhysicalDeviceFormatProperties2KHR) gpa(obj, "vkGetPhysicalDeviceFormatProperties2KHR");
    if (table->GetPhysicalDeviceFormatProperties2 && !table->GetPhysicalDeviceFormatProperties2KHR)
       table->GetPhysicalDeviceFormatProperties2KHR = (PFN_vkGetPhysicalDeviceFormatProperties2KHR) table->GetPhysicalDeviceFormatProperties2;
    if (!table->GetPhysicalDeviceFormatProperties2)
       table->GetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) table->GetPhysicalDeviceFormatProperties2KHR;
        table->GetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties2");
    table->GetPhysicalDeviceImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceImageFormatProperties2KHR) gpa(obj, "vkGetPhysicalDeviceImageFormatProperties2KHR");
    if (table->GetPhysicalDeviceImageFormatProperties2 && !table->GetPhysicalDeviceImageFormatProperties2KHR)
       table->GetPhysicalDeviceImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceImageFormatProperties2KHR) table->GetPhysicalDeviceImageFormatProperties2;
    if (!table->GetPhysicalDeviceImageFormatProperties2)
       table->GetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) table->GetPhysicalDeviceImageFormatProperties2KHR;
        table->GetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties2");
    table->GetPhysicalDeviceQueueFamilyProperties2KHR = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR) gpa(obj, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    if (table->GetPhysicalDeviceQueueFamilyProperties2 && !table->GetPhysicalDeviceQueueFamilyProperties2KHR)
       table->GetPhysicalDeviceQueueFamilyProperties2KHR = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR) table->GetPhysicalDeviceQueueFamilyProperties2;
    if (!table->GetPhysicalDeviceQueueFamilyProperties2)
       table->GetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) table->GetPhysicalDeviceQueueFamilyProperties2KHR;
        table->GetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) gpa(obj, "vkGetPhysicalDeviceMemoryProperties2");
    table->GetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR) gpa(obj, "vkGetPhysicalDeviceMemoryProperties2KHR");
    if (table->GetPhysicalDeviceMemoryProperties2 && !table->GetPhysicalDeviceMemoryProperties2KHR)
       table->GetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR) table->GetPhysicalDeviceMemoryProperties2;
    if (!table->GetPhysicalDeviceMemoryProperties2)
       table->GetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) table->GetPhysicalDeviceMemoryProperties2KHR;
        table->GetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties2");
    table->GetPhysicalDeviceSparseImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR) gpa(obj, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
    if (table->GetPhysicalDeviceSparseImageFormatProperties2 && !table->GetPhysicalDeviceSparseImageFormatProperties2KHR)
       table->GetPhysicalDeviceSparseImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR) table->GetPhysicalDeviceSparseImageFormatProperties2;
    if (!table->GetPhysicalDeviceSparseImageFormatProperties2)
       table->GetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) table->GetPhysicalDeviceSparseImageFormatProperties2KHR;
        table->GetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) gpa(obj, "vkGetPhysicalDeviceExternalBufferProperties");
    table->GetPhysicalDeviceExternalBufferPropertiesKHR = (PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceExternalBufferPropertiesKHR");
    if (table->GetPhysicalDeviceExternalBufferProperties && !table->GetPhysicalDeviceExternalBufferPropertiesKHR)
       table->GetPhysicalDeviceExternalBufferPropertiesKHR = (PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR) table->GetPhysicalDeviceExternalBufferProperties;
    if (!table->GetPhysicalDeviceExternalBufferProperties)
       table->GetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) table->GetPhysicalDeviceExternalBufferPropertiesKHR;
        table->GetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) gpa(obj, "vkGetPhysicalDeviceExternalSemaphoreProperties");
    table->GetPhysicalDeviceExternalSemaphorePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR) gpa(obj, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
    if (table->GetPhysicalDeviceExternalSemaphoreProperties && !table->GetPhysicalDeviceExternalSemaphorePropertiesKHR)
       table->GetPhysicalDeviceExternalSemaphorePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR) table->GetPhysicalDeviceExternalSemaphoreProperties;
    if (!table->GetPhysicalDeviceExternalSemaphoreProperties)
       table->GetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) table->GetPhysicalDeviceExternalSemaphorePropertiesKHR;
        table->GetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) gpa(obj, "vkGetPhysicalDeviceExternalFenceProperties");
    table->GetPhysicalDeviceExternalFencePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR) gpa(obj, "vkGetPhysicalDeviceExternalFencePropertiesKHR");
    if (table->GetPhysicalDeviceExternalFenceProperties && !table->GetPhysicalDeviceExternalFencePropertiesKHR)
       table->GetPhysicalDeviceExternalFencePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR) table->GetPhysicalDeviceExternalFenceProperties;
    if (!table->GetPhysicalDeviceExternalFenceProperties)
       table->GetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) table->GetPhysicalDeviceExternalFencePropertiesKHR;
        table->ReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT) gpa(obj, "vkReleaseDisplayEXT");
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    table->AcquireXlibDisplayEXT = (PFN_vkAcquireXlibDisplayEXT) gpa(obj, "vkAcquireXlibDisplayEXT");
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    table->GetRandROutputDisplayEXT = (PFN_vkGetRandROutputDisplayEXT) gpa(obj, "vkGetRandROutputDisplayEXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->AcquireWinrtDisplayNV = (PFN_vkAcquireWinrtDisplayNV) gpa(obj, "vkAcquireWinrtDisplayNV");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetWinrtDisplayNV = (PFN_vkGetWinrtDisplayNV) gpa(obj, "vkGetWinrtDisplayNV");
#endif
    table->GetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    table->GetPhysicalDevicePresentRectanglesKHR = (PFN_vkGetPhysicalDevicePresentRectanglesKHR) gpa(obj, "vkGetPhysicalDevicePresentRectanglesKHR");
    table->GetPhysicalDeviceMultisamplePropertiesEXT = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT) gpa(obj, "vkGetPhysicalDeviceMultisamplePropertiesEXT");
    table->GetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR) gpa(obj, "vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    table->GetPhysicalDeviceSurfaceFormats2KHR = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR) gpa(obj, "vkGetPhysicalDeviceSurfaceFormats2KHR");
    table->GetPhysicalDeviceDisplayProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayProperties2KHR) gpa(obj, "vkGetPhysicalDeviceDisplayProperties2KHR");
    table->GetPhysicalDeviceDisplayPlaneProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR) gpa(obj, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    table->GetDisplayModeProperties2KHR = (PFN_vkGetDisplayModeProperties2KHR) gpa(obj, "vkGetDisplayModeProperties2KHR");
    table->GetDisplayPlaneCapabilities2KHR = (PFN_vkGetDisplayPlaneCapabilities2KHR) gpa(obj, "vkGetDisplayPlaneCapabilities2KHR");
    table->GetPhysicalDeviceCalibrateableTimeDomainsKHR = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR) gpa(obj, "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR");
    table->GetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) gpa(obj, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    if (table->GetPhysicalDeviceCalibrateableTimeDomainsKHR && !table->GetPhysicalDeviceCalibrateableTimeDomainsEXT)
       table->GetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) table->GetPhysicalDeviceCalibrateableTimeDomainsKHR;
    if (!table->GetPhysicalDeviceCalibrateableTimeDomainsKHR)
       table->GetPhysicalDeviceCalibrateableTimeDomainsKHR = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR) table->GetPhysicalDeviceCalibrateableTimeDomainsEXT;
        table->GetPhysicalDeviceCooperativeMatrixPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetPhysicalDeviceSurfacePresentModes2EXT = (PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT) gpa(obj, "vkGetPhysicalDeviceSurfacePresentModes2EXT");
#endif
    table->EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR) gpa(obj, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    table->GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = (PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR) gpa(obj, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    table->GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = (PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV) gpa(obj, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    table->GetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties) gpa(obj, "vkGetPhysicalDeviceToolProperties");
    table->GetPhysicalDeviceToolPropertiesEXT = (PFN_vkGetPhysicalDeviceToolPropertiesEXT) gpa(obj, "vkGetPhysicalDeviceToolPropertiesEXT");
    if (table->GetPhysicalDeviceToolProperties && !table->GetPhysicalDeviceToolPropertiesEXT)
       table->GetPhysicalDeviceToolPropertiesEXT = (PFN_vkGetPhysicalDeviceToolPropertiesEXT) table->GetPhysicalDeviceToolProperties;
    if (!table->GetPhysicalDeviceToolProperties)
       table->GetPhysicalDeviceToolProperties = (PFN_vkGetPhysicalDeviceToolProperties) table->GetPhysicalDeviceToolPropertiesEXT;
        table->GetPhysicalDeviceFragmentShadingRatesKHR = (PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR) gpa(obj, "vkGetPhysicalDeviceFragmentShadingRatesKHR");
    table->GetPhysicalDeviceVideoCapabilitiesKHR = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    table->GetPhysicalDeviceVideoFormatPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    table->GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    table->AcquireDrmDisplayEXT = (PFN_vkAcquireDrmDisplayEXT) gpa(obj, "vkAcquireDrmDisplayEXT");
    table->GetDrmDisplayEXT = (PFN_vkGetDrmDisplayEXT) gpa(obj, "vkGetDrmDisplayEXT");
    table->GetPhysicalDeviceOpticalFlowImageFormatsNV = (PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV) gpa(obj, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV");
    table->GetPhysicalDeviceCooperativeMatrixPropertiesKHR = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR");
    table->GetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV");
    table->GetPhysicalDeviceCooperativeVectorPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV) gpa(obj, "vkGetPhysicalDeviceCooperativeVectorPropertiesNV");
    table->EnumeratePhysicalDeviceShaderInstrumentationMetricsARM = (PFN_vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM) gpa(obj, "vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM");
    table->GetPhysicalDeviceExternalTensorPropertiesARM = (PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM) gpa(obj, "vkGetPhysicalDeviceExternalTensorPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphPropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM");
    table->EnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM) gpa(obj, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM");
    table->GetPhysicalDeviceDescriptorSizeEXT = (PFN_vkGetPhysicalDeviceDescriptorSizeEXT) gpa(obj, "vkGetPhysicalDeviceDescriptorSizeEXT");
    table->GetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM");
    table->GetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM) gpa(obj, "vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM");
}



void
vk_device_uncompacted_dispatch_table_load(struct vk_device_uncompacted_dispatch_table *table,
                               PFN_vkGetDeviceProcAddr gpa,
                               VkDevice obj)
{
    table->GetDeviceProcAddr = gpa;
    table->GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) gpa(obj, "vkGetDeviceProcAddr");
    table->DestroyDevice = (PFN_vkDestroyDevice) gpa(obj, "vkDestroyDevice");
    table->GetDeviceQueue = (PFN_vkGetDeviceQueue) gpa(obj, "vkGetDeviceQueue");
    table->QueueSubmit = (PFN_vkQueueSubmit) gpa(obj, "vkQueueSubmit");
    table->QueueWaitIdle = (PFN_vkQueueWaitIdle) gpa(obj, "vkQueueWaitIdle");
    table->DeviceWaitIdle = (PFN_vkDeviceWaitIdle) gpa(obj, "vkDeviceWaitIdle");
    table->AllocateMemory = (PFN_vkAllocateMemory) gpa(obj, "vkAllocateMemory");
    table->FreeMemory = (PFN_vkFreeMemory) gpa(obj, "vkFreeMemory");
    table->MapMemory = (PFN_vkMapMemory) gpa(obj, "vkMapMemory");
    table->UnmapMemory = (PFN_vkUnmapMemory) gpa(obj, "vkUnmapMemory");
    table->FlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges) gpa(obj, "vkFlushMappedMemoryRanges");
    table->InvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges) gpa(obj, "vkInvalidateMappedMemoryRanges");
    table->GetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment) gpa(obj, "vkGetDeviceMemoryCommitment");
    table->GetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) gpa(obj, "vkGetBufferMemoryRequirements");
    table->BindBufferMemory = (PFN_vkBindBufferMemory) gpa(obj, "vkBindBufferMemory");
    table->GetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements) gpa(obj, "vkGetImageMemoryRequirements");
    table->BindImageMemory = (PFN_vkBindImageMemory) gpa(obj, "vkBindImageMemory");
    table->GetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements) gpa(obj, "vkGetImageSparseMemoryRequirements");
    table->QueueBindSparse = (PFN_vkQueueBindSparse) gpa(obj, "vkQueueBindSparse");
    table->CreateFence = (PFN_vkCreateFence) gpa(obj, "vkCreateFence");
    table->DestroyFence = (PFN_vkDestroyFence) gpa(obj, "vkDestroyFence");
    table->ResetFences = (PFN_vkResetFences) gpa(obj, "vkResetFences");
    table->GetFenceStatus = (PFN_vkGetFenceStatus) gpa(obj, "vkGetFenceStatus");
    table->WaitForFences = (PFN_vkWaitForFences) gpa(obj, "vkWaitForFences");
    table->CreateSemaphore = (PFN_vkCreateSemaphore) gpa(obj, "vkCreateSemaphore");
    table->DestroySemaphore = (PFN_vkDestroySemaphore) gpa(obj, "vkDestroySemaphore");
    table->CreateEvent = (PFN_vkCreateEvent) gpa(obj, "vkCreateEvent");
    table->DestroyEvent = (PFN_vkDestroyEvent) gpa(obj, "vkDestroyEvent");
    table->GetEventStatus = (PFN_vkGetEventStatus) gpa(obj, "vkGetEventStatus");
    table->SetEvent = (PFN_vkSetEvent) gpa(obj, "vkSetEvent");
    table->ResetEvent = (PFN_vkResetEvent) gpa(obj, "vkResetEvent");
    table->CreateQueryPool = (PFN_vkCreateQueryPool) gpa(obj, "vkCreateQueryPool");
    table->DestroyQueryPool = (PFN_vkDestroyQueryPool) gpa(obj, "vkDestroyQueryPool");
    table->GetQueryPoolResults = (PFN_vkGetQueryPoolResults) gpa(obj, "vkGetQueryPoolResults");
    table->ResetQueryPool = (PFN_vkResetQueryPool) gpa(obj, "vkResetQueryPool");
    table->ResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT) gpa(obj, "vkResetQueryPoolEXT");
    if (table->ResetQueryPool && !table->ResetQueryPoolEXT)
       table->ResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT) table->ResetQueryPool;
    if (!table->ResetQueryPool)
       table->ResetQueryPool = (PFN_vkResetQueryPool) table->ResetQueryPoolEXT;
        table->CreateBuffer = (PFN_vkCreateBuffer) gpa(obj, "vkCreateBuffer");
    table->DestroyBuffer = (PFN_vkDestroyBuffer) gpa(obj, "vkDestroyBuffer");
    table->CreateBufferView = (PFN_vkCreateBufferView) gpa(obj, "vkCreateBufferView");
    table->DestroyBufferView = (PFN_vkDestroyBufferView) gpa(obj, "vkDestroyBufferView");
    table->CreateImage = (PFN_vkCreateImage) gpa(obj, "vkCreateImage");
    table->DestroyImage = (PFN_vkDestroyImage) gpa(obj, "vkDestroyImage");
    table->GetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout) gpa(obj, "vkGetImageSubresourceLayout");
    table->CreateImageView = (PFN_vkCreateImageView) gpa(obj, "vkCreateImageView");
    table->DestroyImageView = (PFN_vkDestroyImageView) gpa(obj, "vkDestroyImageView");
    table->CreateShaderModule = (PFN_vkCreateShaderModule) gpa(obj, "vkCreateShaderModule");
    table->DestroyShaderModule = (PFN_vkDestroyShaderModule) gpa(obj, "vkDestroyShaderModule");
    table->CreatePipelineCache = (PFN_vkCreatePipelineCache) gpa(obj, "vkCreatePipelineCache");
    table->DestroyPipelineCache = (PFN_vkDestroyPipelineCache) gpa(obj, "vkDestroyPipelineCache");
    table->GetPipelineCacheData = (PFN_vkGetPipelineCacheData) gpa(obj, "vkGetPipelineCacheData");
    table->MergePipelineCaches = (PFN_vkMergePipelineCaches) gpa(obj, "vkMergePipelineCaches");
    table->CreatePipelineBinariesKHR = (PFN_vkCreatePipelineBinariesKHR) gpa(obj, "vkCreatePipelineBinariesKHR");
    table->DestroyPipelineBinaryKHR = (PFN_vkDestroyPipelineBinaryKHR) gpa(obj, "vkDestroyPipelineBinaryKHR");
    table->GetPipelineKeyKHR = (PFN_vkGetPipelineKeyKHR) gpa(obj, "vkGetPipelineKeyKHR");
    table->GetPipelineBinaryDataKHR = (PFN_vkGetPipelineBinaryDataKHR) gpa(obj, "vkGetPipelineBinaryDataKHR");
    table->ReleaseCapturedPipelineDataKHR = (PFN_vkReleaseCapturedPipelineDataKHR) gpa(obj, "vkReleaseCapturedPipelineDataKHR");
    table->CreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) gpa(obj, "vkCreateGraphicsPipelines");
    table->CreateComputePipelines = (PFN_vkCreateComputePipelines) gpa(obj, "vkCreateComputePipelines");
    table->GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = (PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI) gpa(obj, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    table->DestroyPipeline = (PFN_vkDestroyPipeline) gpa(obj, "vkDestroyPipeline");
    table->CreatePipelineLayout = (PFN_vkCreatePipelineLayout) gpa(obj, "vkCreatePipelineLayout");
    table->DestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) gpa(obj, "vkDestroyPipelineLayout");
    table->CreateSampler = (PFN_vkCreateSampler) gpa(obj, "vkCreateSampler");
    table->DestroySampler = (PFN_vkDestroySampler) gpa(obj, "vkDestroySampler");
    table->CreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) gpa(obj, "vkCreateDescriptorSetLayout");
    table->DestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) gpa(obj, "vkDestroyDescriptorSetLayout");
    table->CreateDescriptorPool = (PFN_vkCreateDescriptorPool) gpa(obj, "vkCreateDescriptorPool");
    table->DestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) gpa(obj, "vkDestroyDescriptorPool");
    table->ResetDescriptorPool = (PFN_vkResetDescriptorPool) gpa(obj, "vkResetDescriptorPool");
    table->AllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) gpa(obj, "vkAllocateDescriptorSets");
    table->FreeDescriptorSets = (PFN_vkFreeDescriptorSets) gpa(obj, "vkFreeDescriptorSets");
    table->UpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) gpa(obj, "vkUpdateDescriptorSets");
    table->CreateFramebuffer = (PFN_vkCreateFramebuffer) gpa(obj, "vkCreateFramebuffer");
    table->DestroyFramebuffer = (PFN_vkDestroyFramebuffer) gpa(obj, "vkDestroyFramebuffer");
    table->CreateRenderPass = (PFN_vkCreateRenderPass) gpa(obj, "vkCreateRenderPass");
    table->DestroyRenderPass = (PFN_vkDestroyRenderPass) gpa(obj, "vkDestroyRenderPass");
    table->GetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity) gpa(obj, "vkGetRenderAreaGranularity");
    table->GetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity) gpa(obj, "vkGetRenderingAreaGranularity");
    table->GetRenderingAreaGranularityKHR = (PFN_vkGetRenderingAreaGranularityKHR) gpa(obj, "vkGetRenderingAreaGranularityKHR");
    if (table->GetRenderingAreaGranularity && !table->GetRenderingAreaGranularityKHR)
       table->GetRenderingAreaGranularityKHR = (PFN_vkGetRenderingAreaGranularityKHR) table->GetRenderingAreaGranularity;
    if (!table->GetRenderingAreaGranularity)
       table->GetRenderingAreaGranularity = (PFN_vkGetRenderingAreaGranularity) table->GetRenderingAreaGranularityKHR;
        table->CreateCommandPool = (PFN_vkCreateCommandPool) gpa(obj, "vkCreateCommandPool");
    table->DestroyCommandPool = (PFN_vkDestroyCommandPool) gpa(obj, "vkDestroyCommandPool");
    table->ResetCommandPool = (PFN_vkResetCommandPool) gpa(obj, "vkResetCommandPool");
    table->AllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) gpa(obj, "vkAllocateCommandBuffers");
    table->FreeCommandBuffers = (PFN_vkFreeCommandBuffers) gpa(obj, "vkFreeCommandBuffers");
    table->BeginCommandBuffer = (PFN_vkBeginCommandBuffer) gpa(obj, "vkBeginCommandBuffer");
    table->EndCommandBuffer = (PFN_vkEndCommandBuffer) gpa(obj, "vkEndCommandBuffer");
    table->ResetCommandBuffer = (PFN_vkResetCommandBuffer) gpa(obj, "vkResetCommandBuffer");
    table->CmdBindPipeline = (PFN_vkCmdBindPipeline) gpa(obj, "vkCmdBindPipeline");
    table->CmdSetPrimitiveRestartIndexEXT = (PFN_vkCmdSetPrimitiveRestartIndexEXT) gpa(obj, "vkCmdSetPrimitiveRestartIndexEXT");
    table->CmdSetAttachmentFeedbackLoopEnableEXT = (PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT) gpa(obj, "vkCmdSetAttachmentFeedbackLoopEnableEXT");
    table->CmdSetViewport = (PFN_vkCmdSetViewport) gpa(obj, "vkCmdSetViewport");
    table->CmdSetScissor = (PFN_vkCmdSetScissor) gpa(obj, "vkCmdSetScissor");
    table->CmdSetLineWidth = (PFN_vkCmdSetLineWidth) gpa(obj, "vkCmdSetLineWidth");
    table->CmdSetDepthBias = (PFN_vkCmdSetDepthBias) gpa(obj, "vkCmdSetDepthBias");
    table->CmdSetBlendConstants = (PFN_vkCmdSetBlendConstants) gpa(obj, "vkCmdSetBlendConstants");
    table->CmdSetDepthBounds = (PFN_vkCmdSetDepthBounds) gpa(obj, "vkCmdSetDepthBounds");
    table->CmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask) gpa(obj, "vkCmdSetStencilCompareMask");
    table->CmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask) gpa(obj, "vkCmdSetStencilWriteMask");
    table->CmdSetStencilReference = (PFN_vkCmdSetStencilReference) gpa(obj, "vkCmdSetStencilReference");
    table->CmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) gpa(obj, "vkCmdBindDescriptorSets");
    table->CmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) gpa(obj, "vkCmdBindIndexBuffer");
    table->CmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) gpa(obj, "vkCmdBindVertexBuffers");
    table->CmdDraw = (PFN_vkCmdDraw) gpa(obj, "vkCmdDraw");
    table->CmdDrawIndexed = (PFN_vkCmdDrawIndexed) gpa(obj, "vkCmdDrawIndexed");
    table->CmdDrawMultiEXT = (PFN_vkCmdDrawMultiEXT) gpa(obj, "vkCmdDrawMultiEXT");
    table->CmdDrawMultiIndexedEXT = (PFN_vkCmdDrawMultiIndexedEXT) gpa(obj, "vkCmdDrawMultiIndexedEXT");
    table->CmdDrawIndirect = (PFN_vkCmdDrawIndirect) gpa(obj, "vkCmdDrawIndirect");
    table->CmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect) gpa(obj, "vkCmdDrawIndexedIndirect");
    table->CmdDispatch = (PFN_vkCmdDispatch) gpa(obj, "vkCmdDispatch");
    table->CmdDispatchIndirect = (PFN_vkCmdDispatchIndirect) gpa(obj, "vkCmdDispatchIndirect");
    table->CmdSubpassShadingHUAWEI = (PFN_vkCmdSubpassShadingHUAWEI) gpa(obj, "vkCmdSubpassShadingHUAWEI");
    table->CmdDrawClusterHUAWEI = (PFN_vkCmdDrawClusterHUAWEI) gpa(obj, "vkCmdDrawClusterHUAWEI");
    table->CmdDrawClusterIndirectHUAWEI = (PFN_vkCmdDrawClusterIndirectHUAWEI) gpa(obj, "vkCmdDrawClusterIndirectHUAWEI");
    table->CmdUpdatePipelineIndirectBufferNV = (PFN_vkCmdUpdatePipelineIndirectBufferNV) gpa(obj, "vkCmdUpdatePipelineIndirectBufferNV");
    table->CmdCopyBuffer = (PFN_vkCmdCopyBuffer) gpa(obj, "vkCmdCopyBuffer");
    table->CmdCopyImage = (PFN_vkCmdCopyImage) gpa(obj, "vkCmdCopyImage");
    table->CmdBlitImage = (PFN_vkCmdBlitImage) gpa(obj, "vkCmdBlitImage");
    table->CmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) gpa(obj, "vkCmdCopyBufferToImage");
    table->CmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer) gpa(obj, "vkCmdCopyImageToBuffer");
    table->CmdCopyMemoryIndirectNV = (PFN_vkCmdCopyMemoryIndirectNV) gpa(obj, "vkCmdCopyMemoryIndirectNV");
    table->CmdCopyMemoryIndirectKHR = (PFN_vkCmdCopyMemoryIndirectKHR) gpa(obj, "vkCmdCopyMemoryIndirectKHR");
    table->CmdCopyMemoryToImageIndirectNV = (PFN_vkCmdCopyMemoryToImageIndirectNV) gpa(obj, "vkCmdCopyMemoryToImageIndirectNV");
    table->CmdCopyMemoryToImageIndirectKHR = (PFN_vkCmdCopyMemoryToImageIndirectKHR) gpa(obj, "vkCmdCopyMemoryToImageIndirectKHR");
    table->CmdUpdateBuffer = (PFN_vkCmdUpdateBuffer) gpa(obj, "vkCmdUpdateBuffer");
    table->CmdFillBuffer = (PFN_vkCmdFillBuffer) gpa(obj, "vkCmdFillBuffer");
    table->CmdClearColorImage = (PFN_vkCmdClearColorImage) gpa(obj, "vkCmdClearColorImage");
    table->CmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage) gpa(obj, "vkCmdClearDepthStencilImage");
    table->CmdClearAttachments = (PFN_vkCmdClearAttachments) gpa(obj, "vkCmdClearAttachments");
    table->CmdResolveImage = (PFN_vkCmdResolveImage) gpa(obj, "vkCmdResolveImage");
    table->CmdSetEvent = (PFN_vkCmdSetEvent) gpa(obj, "vkCmdSetEvent");
    table->CmdResetEvent = (PFN_vkCmdResetEvent) gpa(obj, "vkCmdResetEvent");
    table->CmdWaitEvents = (PFN_vkCmdWaitEvents) gpa(obj, "vkCmdWaitEvents");
    table->CmdPipelineBarrier = (PFN_vkCmdPipelineBarrier) gpa(obj, "vkCmdPipelineBarrier");
    table->CmdBeginQuery = (PFN_vkCmdBeginQuery) gpa(obj, "vkCmdBeginQuery");
    table->CmdEndQuery = (PFN_vkCmdEndQuery) gpa(obj, "vkCmdEndQuery");
    table->CmdBeginConditionalRenderingEXT = (PFN_vkCmdBeginConditionalRenderingEXT) gpa(obj, "vkCmdBeginConditionalRenderingEXT");
    table->CmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT) gpa(obj, "vkCmdEndConditionalRenderingEXT");
    table->CmdBeginCustomResolveEXT = (PFN_vkCmdBeginCustomResolveEXT) gpa(obj, "vkCmdBeginCustomResolveEXT");
    table->CmdResetQueryPool = (PFN_vkCmdResetQueryPool) gpa(obj, "vkCmdResetQueryPool");
    table->CmdWriteTimestamp = (PFN_vkCmdWriteTimestamp) gpa(obj, "vkCmdWriteTimestamp");
    table->CmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults) gpa(obj, "vkCmdCopyQueryPoolResults");
    table->CmdPushConstants = (PFN_vkCmdPushConstants) gpa(obj, "vkCmdPushConstants");
    table->CmdBeginRenderPass = (PFN_vkCmdBeginRenderPass) gpa(obj, "vkCmdBeginRenderPass");
    table->CmdNextSubpass = (PFN_vkCmdNextSubpass) gpa(obj, "vkCmdNextSubpass");
    table->CmdEndRenderPass = (PFN_vkCmdEndRenderPass) gpa(obj, "vkCmdEndRenderPass");
    table->CmdExecuteCommands = (PFN_vkCmdExecuteCommands) gpa(obj, "vkCmdExecuteCommands");
    table->CreateSharedSwapchainsKHR = (PFN_vkCreateSharedSwapchainsKHR) gpa(obj, "vkCreateSharedSwapchainsKHR");
    table->CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) gpa(obj, "vkCreateSwapchainKHR");
    table->DestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) gpa(obj, "vkDestroySwapchainKHR");
    table->GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) gpa(obj, "vkGetSwapchainImagesKHR");
    table->AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) gpa(obj, "vkAcquireNextImageKHR");
    table->QueuePresentKHR = (PFN_vkQueuePresentKHR) gpa(obj, "vkQueuePresentKHR");
    table->DebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) gpa(obj, "vkDebugMarkerSetObjectNameEXT");
    table->DebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT) gpa(obj, "vkDebugMarkerSetObjectTagEXT");
    table->CmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT) gpa(obj, "vkCmdDebugMarkerBeginEXT");
    table->CmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT) gpa(obj, "vkCmdDebugMarkerEndEXT");
    table->CmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT) gpa(obj, "vkCmdDebugMarkerInsertEXT");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandleNV = (PFN_vkGetMemoryWin32HandleNV) gpa(obj, "vkGetMemoryWin32HandleNV");
#endif
    table->CmdExecuteGeneratedCommandsNV = (PFN_vkCmdExecuteGeneratedCommandsNV) gpa(obj, "vkCmdExecuteGeneratedCommandsNV");
    table->CmdPreprocessGeneratedCommandsNV = (PFN_vkCmdPreprocessGeneratedCommandsNV) gpa(obj, "vkCmdPreprocessGeneratedCommandsNV");
    table->CmdBindPipelineShaderGroupNV = (PFN_vkCmdBindPipelineShaderGroupNV) gpa(obj, "vkCmdBindPipelineShaderGroupNV");
    table->GetGeneratedCommandsMemoryRequirementsNV = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV) gpa(obj, "vkGetGeneratedCommandsMemoryRequirementsNV");
    table->CreateIndirectCommandsLayoutNV = (PFN_vkCreateIndirectCommandsLayoutNV) gpa(obj, "vkCreateIndirectCommandsLayoutNV");
    table->DestroyIndirectCommandsLayoutNV = (PFN_vkDestroyIndirectCommandsLayoutNV) gpa(obj, "vkDestroyIndirectCommandsLayoutNV");
    table->CmdExecuteGeneratedCommandsEXT = (PFN_vkCmdExecuteGeneratedCommandsEXT) gpa(obj, "vkCmdExecuteGeneratedCommandsEXT");
    table->CmdPreprocessGeneratedCommandsEXT = (PFN_vkCmdPreprocessGeneratedCommandsEXT) gpa(obj, "vkCmdPreprocessGeneratedCommandsEXT");
    table->GetGeneratedCommandsMemoryRequirementsEXT = (PFN_vkGetGeneratedCommandsMemoryRequirementsEXT) gpa(obj, "vkGetGeneratedCommandsMemoryRequirementsEXT");
    table->CreateIndirectCommandsLayoutEXT = (PFN_vkCreateIndirectCommandsLayoutEXT) gpa(obj, "vkCreateIndirectCommandsLayoutEXT");
    table->DestroyIndirectCommandsLayoutEXT = (PFN_vkDestroyIndirectCommandsLayoutEXT) gpa(obj, "vkDestroyIndirectCommandsLayoutEXT");
    table->CreateIndirectExecutionSetEXT = (PFN_vkCreateIndirectExecutionSetEXT) gpa(obj, "vkCreateIndirectExecutionSetEXT");
    table->DestroyIndirectExecutionSetEXT = (PFN_vkDestroyIndirectExecutionSetEXT) gpa(obj, "vkDestroyIndirectExecutionSetEXT");
    table->UpdateIndirectExecutionSetPipelineEXT = (PFN_vkUpdateIndirectExecutionSetPipelineEXT) gpa(obj, "vkUpdateIndirectExecutionSetPipelineEXT");
    table->UpdateIndirectExecutionSetShaderEXT = (PFN_vkUpdateIndirectExecutionSetShaderEXT) gpa(obj, "vkUpdateIndirectExecutionSetShaderEXT");
    table->CmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSet) gpa(obj, "vkCmdPushDescriptorSet");
    table->CmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR) gpa(obj, "vkCmdPushDescriptorSetKHR");
    if (table->CmdPushDescriptorSet && !table->CmdPushDescriptorSetKHR)
       table->CmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR) table->CmdPushDescriptorSet;
    if (!table->CmdPushDescriptorSet)
       table->CmdPushDescriptorSet = (PFN_vkCmdPushDescriptorSet) table->CmdPushDescriptorSetKHR;
        table->TrimCommandPool = (PFN_vkTrimCommandPool) gpa(obj, "vkTrimCommandPool");
    table->TrimCommandPoolKHR = (PFN_vkTrimCommandPoolKHR) gpa(obj, "vkTrimCommandPoolKHR");
    if (table->TrimCommandPool && !table->TrimCommandPoolKHR)
       table->TrimCommandPoolKHR = (PFN_vkTrimCommandPoolKHR) table->TrimCommandPool;
    if (!table->TrimCommandPool)
       table->TrimCommandPool = (PFN_vkTrimCommandPool) table->TrimCommandPoolKHR;
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR) gpa(obj, "vkGetMemoryWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR) gpa(obj, "vkGetMemoryWin32HandlePropertiesKHR");
#endif
    table->GetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) gpa(obj, "vkGetMemoryFdKHR");
    table->GetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR) gpa(obj, "vkGetMemoryFdPropertiesKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetMemoryZirconHandleFUCHSIA = (PFN_vkGetMemoryZirconHandleFUCHSIA) gpa(obj, "vkGetMemoryZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetMemoryZirconHandlePropertiesFUCHSIA = (PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA) gpa(obj, "vkGetMemoryZirconHandlePropertiesFUCHSIA");
#endif
    table->GetMemoryRemoteAddressNV = (PFN_vkGetMemoryRemoteAddressNV) gpa(obj, "vkGetMemoryRemoteAddressNV");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetSemaphoreWin32HandleKHR = (PFN_vkGetSemaphoreWin32HandleKHR) gpa(obj, "vkGetSemaphoreWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR) gpa(obj, "vkImportSemaphoreWin32HandleKHR");
#endif
    table->GetSemaphoreFdKHR = (PFN_vkGetSemaphoreFdKHR) gpa(obj, "vkGetSemaphoreFdKHR");
    table->ImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR) gpa(obj, "vkImportSemaphoreFdKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetSemaphoreZirconHandleFUCHSIA = (PFN_vkGetSemaphoreZirconHandleFUCHSIA) gpa(obj, "vkGetSemaphoreZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->ImportSemaphoreZirconHandleFUCHSIA = (PFN_vkImportSemaphoreZirconHandleFUCHSIA) gpa(obj, "vkImportSemaphoreZirconHandleFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetFenceWin32HandleKHR = (PFN_vkGetFenceWin32HandleKHR) gpa(obj, "vkGetFenceWin32HandleKHR");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ImportFenceWin32HandleKHR = (PFN_vkImportFenceWin32HandleKHR) gpa(obj, "vkImportFenceWin32HandleKHR");
#endif
    table->GetFenceFdKHR = (PFN_vkGetFenceFdKHR) gpa(obj, "vkGetFenceFdKHR");
    table->ImportFenceFdKHR = (PFN_vkImportFenceFdKHR) gpa(obj, "vkImportFenceFdKHR");
    table->DisplayPowerControlEXT = (PFN_vkDisplayPowerControlEXT) gpa(obj, "vkDisplayPowerControlEXT");
    table->RegisterDeviceEventEXT = (PFN_vkRegisterDeviceEventEXT) gpa(obj, "vkRegisterDeviceEventEXT");
    table->RegisterDisplayEventEXT = (PFN_vkRegisterDisplayEventEXT) gpa(obj, "vkRegisterDisplayEventEXT");
    table->GetSwapchainCounterEXT = (PFN_vkGetSwapchainCounterEXT) gpa(obj, "vkGetSwapchainCounterEXT");
    table->GetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) gpa(obj, "vkGetDeviceGroupPeerMemoryFeatures");
    table->GetDeviceGroupPeerMemoryFeaturesKHR = (PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR) gpa(obj, "vkGetDeviceGroupPeerMemoryFeaturesKHR");
    if (table->GetDeviceGroupPeerMemoryFeatures && !table->GetDeviceGroupPeerMemoryFeaturesKHR)
       table->GetDeviceGroupPeerMemoryFeaturesKHR = (PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR) table->GetDeviceGroupPeerMemoryFeatures;
    if (!table->GetDeviceGroupPeerMemoryFeatures)
       table->GetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) table->GetDeviceGroupPeerMemoryFeaturesKHR;
        table->BindBufferMemory2 = (PFN_vkBindBufferMemory2) gpa(obj, "vkBindBufferMemory2");
    table->BindBufferMemory2KHR = (PFN_vkBindBufferMemory2KHR) gpa(obj, "vkBindBufferMemory2KHR");
    if (table->BindBufferMemory2 && !table->BindBufferMemory2KHR)
       table->BindBufferMemory2KHR = (PFN_vkBindBufferMemory2KHR) table->BindBufferMemory2;
    if (!table->BindBufferMemory2)
       table->BindBufferMemory2 = (PFN_vkBindBufferMemory2) table->BindBufferMemory2KHR;
        table->BindImageMemory2 = (PFN_vkBindImageMemory2) gpa(obj, "vkBindImageMemory2");
    table->BindImageMemory2KHR = (PFN_vkBindImageMemory2KHR) gpa(obj, "vkBindImageMemory2KHR");
    if (table->BindImageMemory2 && !table->BindImageMemory2KHR)
       table->BindImageMemory2KHR = (PFN_vkBindImageMemory2KHR) table->BindImageMemory2;
    if (!table->BindImageMemory2)
       table->BindImageMemory2 = (PFN_vkBindImageMemory2) table->BindImageMemory2KHR;
        table->CmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) gpa(obj, "vkCmdSetDeviceMask");
    table->CmdSetDeviceMaskKHR = (PFN_vkCmdSetDeviceMaskKHR) gpa(obj, "vkCmdSetDeviceMaskKHR");
    if (table->CmdSetDeviceMask && !table->CmdSetDeviceMaskKHR)
       table->CmdSetDeviceMaskKHR = (PFN_vkCmdSetDeviceMaskKHR) table->CmdSetDeviceMask;
    if (!table->CmdSetDeviceMask)
       table->CmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) table->CmdSetDeviceMaskKHR;
        table->GetDeviceGroupPresentCapabilitiesKHR = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR) gpa(obj, "vkGetDeviceGroupPresentCapabilitiesKHR");
    table->GetDeviceGroupSurfacePresentModesKHR = (PFN_vkGetDeviceGroupSurfacePresentModesKHR) gpa(obj, "vkGetDeviceGroupSurfacePresentModesKHR");
    table->AcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR) gpa(obj, "vkAcquireNextImage2KHR");
    table->CmdDispatchBase = (PFN_vkCmdDispatchBase) gpa(obj, "vkCmdDispatchBase");
    table->CmdDispatchBaseKHR = (PFN_vkCmdDispatchBaseKHR) gpa(obj, "vkCmdDispatchBaseKHR");
    if (table->CmdDispatchBase && !table->CmdDispatchBaseKHR)
       table->CmdDispatchBaseKHR = (PFN_vkCmdDispatchBaseKHR) table->CmdDispatchBase;
    if (!table->CmdDispatchBase)
       table->CmdDispatchBase = (PFN_vkCmdDispatchBase) table->CmdDispatchBaseKHR;
        table->CreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) gpa(obj, "vkCreateDescriptorUpdateTemplate");
    table->CreateDescriptorUpdateTemplateKHR = (PFN_vkCreateDescriptorUpdateTemplateKHR) gpa(obj, "vkCreateDescriptorUpdateTemplateKHR");
    if (table->CreateDescriptorUpdateTemplate && !table->CreateDescriptorUpdateTemplateKHR)
       table->CreateDescriptorUpdateTemplateKHR = (PFN_vkCreateDescriptorUpdateTemplateKHR) table->CreateDescriptorUpdateTemplate;
    if (!table->CreateDescriptorUpdateTemplate)
       table->CreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) table->CreateDescriptorUpdateTemplateKHR;
        table->DestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) gpa(obj, "vkDestroyDescriptorUpdateTemplate");
    table->DestroyDescriptorUpdateTemplateKHR = (PFN_vkDestroyDescriptorUpdateTemplateKHR) gpa(obj, "vkDestroyDescriptorUpdateTemplateKHR");
    if (table->DestroyDescriptorUpdateTemplate && !table->DestroyDescriptorUpdateTemplateKHR)
       table->DestroyDescriptorUpdateTemplateKHR = (PFN_vkDestroyDescriptorUpdateTemplateKHR) table->DestroyDescriptorUpdateTemplate;
    if (!table->DestroyDescriptorUpdateTemplate)
       table->DestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) table->DestroyDescriptorUpdateTemplateKHR;
        table->UpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) gpa(obj, "vkUpdateDescriptorSetWithTemplate");
    table->UpdateDescriptorSetWithTemplateKHR = (PFN_vkUpdateDescriptorSetWithTemplateKHR) gpa(obj, "vkUpdateDescriptorSetWithTemplateKHR");
    if (table->UpdateDescriptorSetWithTemplate && !table->UpdateDescriptorSetWithTemplateKHR)
       table->UpdateDescriptorSetWithTemplateKHR = (PFN_vkUpdateDescriptorSetWithTemplateKHR) table->UpdateDescriptorSetWithTemplate;
    if (!table->UpdateDescriptorSetWithTemplate)
       table->UpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) table->UpdateDescriptorSetWithTemplateKHR;
        table->CmdPushDescriptorSetWithTemplate = (PFN_vkCmdPushDescriptorSetWithTemplate) gpa(obj, "vkCmdPushDescriptorSetWithTemplate");
    table->CmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) gpa(obj, "vkCmdPushDescriptorSetWithTemplateKHR");
    if (table->CmdPushDescriptorSetWithTemplate && !table->CmdPushDescriptorSetWithTemplateKHR)
       table->CmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) table->CmdPushDescriptorSetWithTemplate;
    if (!table->CmdPushDescriptorSetWithTemplate)
       table->CmdPushDescriptorSetWithTemplate = (PFN_vkCmdPushDescriptorSetWithTemplate) table->CmdPushDescriptorSetWithTemplateKHR;
        table->SetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT) gpa(obj, "vkSetHdrMetadataEXT");
    table->GetSwapchainStatusKHR = (PFN_vkGetSwapchainStatusKHR) gpa(obj, "vkGetSwapchainStatusKHR");
    table->GetRefreshCycleDurationGOOGLE = (PFN_vkGetRefreshCycleDurationGOOGLE) gpa(obj, "vkGetRefreshCycleDurationGOOGLE");
    table->GetPastPresentationTimingGOOGLE = (PFN_vkGetPastPresentationTimingGOOGLE) gpa(obj, "vkGetPastPresentationTimingGOOGLE");
    table->CmdSetViewportWScalingNV = (PFN_vkCmdSetViewportWScalingNV) gpa(obj, "vkCmdSetViewportWScalingNV");
    table->CmdSetDiscardRectangleEXT = (PFN_vkCmdSetDiscardRectangleEXT) gpa(obj, "vkCmdSetDiscardRectangleEXT");
    table->CmdSetDiscardRectangleEnableEXT = (PFN_vkCmdSetDiscardRectangleEnableEXT) gpa(obj, "vkCmdSetDiscardRectangleEnableEXT");
    table->CmdSetDiscardRectangleModeEXT = (PFN_vkCmdSetDiscardRectangleModeEXT) gpa(obj, "vkCmdSetDiscardRectangleModeEXT");
    table->CmdSetSampleLocationsEXT = (PFN_vkCmdSetSampleLocationsEXT) gpa(obj, "vkCmdSetSampleLocationsEXT");
    table->GetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) gpa(obj, "vkGetBufferMemoryRequirements2");
    table->GetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2KHR) gpa(obj, "vkGetBufferMemoryRequirements2KHR");
    if (table->GetBufferMemoryRequirements2 && !table->GetBufferMemoryRequirements2KHR)
       table->GetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2KHR) table->GetBufferMemoryRequirements2;
    if (!table->GetBufferMemoryRequirements2)
       table->GetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) table->GetBufferMemoryRequirements2KHR;
        table->GetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) gpa(obj, "vkGetImageMemoryRequirements2");
    table->GetImageMemoryRequirements2KHR = (PFN_vkGetImageMemoryRequirements2KHR) gpa(obj, "vkGetImageMemoryRequirements2KHR");
    if (table->GetImageMemoryRequirements2 && !table->GetImageMemoryRequirements2KHR)
       table->GetImageMemoryRequirements2KHR = (PFN_vkGetImageMemoryRequirements2KHR) table->GetImageMemoryRequirements2;
    if (!table->GetImageMemoryRequirements2)
       table->GetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) table->GetImageMemoryRequirements2KHR;
        table->GetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) gpa(obj, "vkGetImageSparseMemoryRequirements2");
    table->GetImageSparseMemoryRequirements2KHR = (PFN_vkGetImageSparseMemoryRequirements2KHR) gpa(obj, "vkGetImageSparseMemoryRequirements2KHR");
    if (table->GetImageSparseMemoryRequirements2 && !table->GetImageSparseMemoryRequirements2KHR)
       table->GetImageSparseMemoryRequirements2KHR = (PFN_vkGetImageSparseMemoryRequirements2KHR) table->GetImageSparseMemoryRequirements2;
    if (!table->GetImageSparseMemoryRequirements2)
       table->GetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) table->GetImageSparseMemoryRequirements2KHR;
        table->GetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements) gpa(obj, "vkGetDeviceBufferMemoryRequirements");
    table->GetDeviceBufferMemoryRequirementsKHR = (PFN_vkGetDeviceBufferMemoryRequirementsKHR) gpa(obj, "vkGetDeviceBufferMemoryRequirementsKHR");
    if (table->GetDeviceBufferMemoryRequirements && !table->GetDeviceBufferMemoryRequirementsKHR)
       table->GetDeviceBufferMemoryRequirementsKHR = (PFN_vkGetDeviceBufferMemoryRequirementsKHR) table->GetDeviceBufferMemoryRequirements;
    if (!table->GetDeviceBufferMemoryRequirements)
       table->GetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements) table->GetDeviceBufferMemoryRequirementsKHR;
        table->GetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements) gpa(obj, "vkGetDeviceImageMemoryRequirements");
    table->GetDeviceImageMemoryRequirementsKHR = (PFN_vkGetDeviceImageMemoryRequirementsKHR) gpa(obj, "vkGetDeviceImageMemoryRequirementsKHR");
    if (table->GetDeviceImageMemoryRequirements && !table->GetDeviceImageMemoryRequirementsKHR)
       table->GetDeviceImageMemoryRequirementsKHR = (PFN_vkGetDeviceImageMemoryRequirementsKHR) table->GetDeviceImageMemoryRequirements;
    if (!table->GetDeviceImageMemoryRequirements)
       table->GetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements) table->GetDeviceImageMemoryRequirementsKHR;
        table->GetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements) gpa(obj, "vkGetDeviceImageSparseMemoryRequirements");
    table->GetDeviceImageSparseMemoryRequirementsKHR = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR) gpa(obj, "vkGetDeviceImageSparseMemoryRequirementsKHR");
    if (table->GetDeviceImageSparseMemoryRequirements && !table->GetDeviceImageSparseMemoryRequirementsKHR)
       table->GetDeviceImageSparseMemoryRequirementsKHR = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR) table->GetDeviceImageSparseMemoryRequirements;
    if (!table->GetDeviceImageSparseMemoryRequirements)
       table->GetDeviceImageSparseMemoryRequirements = (PFN_vkGetDeviceImageSparseMemoryRequirements) table->GetDeviceImageSparseMemoryRequirementsKHR;
        table->CreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) gpa(obj, "vkCreateSamplerYcbcrConversion");
    table->CreateSamplerYcbcrConversionKHR = (PFN_vkCreateSamplerYcbcrConversionKHR) gpa(obj, "vkCreateSamplerYcbcrConversionKHR");
    if (table->CreateSamplerYcbcrConversion && !table->CreateSamplerYcbcrConversionKHR)
       table->CreateSamplerYcbcrConversionKHR = (PFN_vkCreateSamplerYcbcrConversionKHR) table->CreateSamplerYcbcrConversion;
    if (!table->CreateSamplerYcbcrConversion)
       table->CreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) table->CreateSamplerYcbcrConversionKHR;
        table->DestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) gpa(obj, "vkDestroySamplerYcbcrConversion");
    table->DestroySamplerYcbcrConversionKHR = (PFN_vkDestroySamplerYcbcrConversionKHR) gpa(obj, "vkDestroySamplerYcbcrConversionKHR");
    if (table->DestroySamplerYcbcrConversion && !table->DestroySamplerYcbcrConversionKHR)
       table->DestroySamplerYcbcrConversionKHR = (PFN_vkDestroySamplerYcbcrConversionKHR) table->DestroySamplerYcbcrConversion;
    if (!table->DestroySamplerYcbcrConversion)
       table->DestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) table->DestroySamplerYcbcrConversionKHR;
        table->GetDeviceQueue2 = (PFN_vkGetDeviceQueue2) gpa(obj, "vkGetDeviceQueue2");
    table->CreateValidationCacheEXT = (PFN_vkCreateValidationCacheEXT) gpa(obj, "vkCreateValidationCacheEXT");
    table->DestroyValidationCacheEXT = (PFN_vkDestroyValidationCacheEXT) gpa(obj, "vkDestroyValidationCacheEXT");
    table->GetValidationCacheDataEXT = (PFN_vkGetValidationCacheDataEXT) gpa(obj, "vkGetValidationCacheDataEXT");
    table->MergeValidationCachesEXT = (PFN_vkMergeValidationCachesEXT) gpa(obj, "vkMergeValidationCachesEXT");
    table->GetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) gpa(obj, "vkGetDescriptorSetLayoutSupport");
    table->GetDescriptorSetLayoutSupportKHR = (PFN_vkGetDescriptorSetLayoutSupportKHR) gpa(obj, "vkGetDescriptorSetLayoutSupportKHR");
    if (table->GetDescriptorSetLayoutSupport && !table->GetDescriptorSetLayoutSupportKHR)
       table->GetDescriptorSetLayoutSupportKHR = (PFN_vkGetDescriptorSetLayoutSupportKHR) table->GetDescriptorSetLayoutSupport;
    if (!table->GetDescriptorSetLayoutSupport)
       table->GetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) table->GetDescriptorSetLayoutSupportKHR;
    #ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetSwapchainGrallocUsageANDROID = (PFN_vkGetSwapchainGrallocUsageANDROID) gpa(obj, "vkGetSwapchainGrallocUsageANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetSwapchainGrallocUsage2ANDROID = (PFN_vkGetSwapchainGrallocUsage2ANDROID) gpa(obj, "vkGetSwapchainGrallocUsage2ANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->AcquireImageANDROID = (PFN_vkAcquireImageANDROID) gpa(obj, "vkAcquireImageANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->QueueSignalReleaseImageANDROID = (PFN_vkQueueSignalReleaseImageANDROID) gpa(obj, "vkQueueSignalReleaseImageANDROID");
#endif
    table->GetShaderInfoAMD = (PFN_vkGetShaderInfoAMD) gpa(obj, "vkGetShaderInfoAMD");
    table->SetLocalDimmingAMD = (PFN_vkSetLocalDimmingAMD) gpa(obj, "vkSetLocalDimmingAMD");
    table->GetCalibratedTimestampsKHR = (PFN_vkGetCalibratedTimestampsKHR) gpa(obj, "vkGetCalibratedTimestampsKHR");
    table->GetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT) gpa(obj, "vkGetCalibratedTimestampsEXT");
    if (table->GetCalibratedTimestampsKHR && !table->GetCalibratedTimestampsEXT)
       table->GetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT) table->GetCalibratedTimestampsKHR;
    if (!table->GetCalibratedTimestampsKHR)
       table->GetCalibratedTimestampsKHR = (PFN_vkGetCalibratedTimestampsKHR) table->GetCalibratedTimestampsEXT;
        table->SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) gpa(obj, "vkSetDebugUtilsObjectNameEXT");
    table->SetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT) gpa(obj, "vkSetDebugUtilsObjectTagEXT");
    table->QueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT) gpa(obj, "vkQueueBeginDebugUtilsLabelEXT");
    table->QueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT) gpa(obj, "vkQueueEndDebugUtilsLabelEXT");
    table->QueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT) gpa(obj, "vkQueueInsertDebugUtilsLabelEXT");
    table->CmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT) gpa(obj, "vkCmdBeginDebugUtilsLabelEXT");
    table->CmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT) gpa(obj, "vkCmdEndDebugUtilsLabelEXT");
    table->CmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT) gpa(obj, "vkCmdInsertDebugUtilsLabelEXT");
    table->GetMemoryHostPointerPropertiesEXT = (PFN_vkGetMemoryHostPointerPropertiesEXT) gpa(obj, "vkGetMemoryHostPointerPropertiesEXT");
    table->CmdWriteBufferMarkerAMD = (PFN_vkCmdWriteBufferMarkerAMD) gpa(obj, "vkCmdWriteBufferMarkerAMD");
    table->CreateRenderPass2 = (PFN_vkCreateRenderPass2) gpa(obj, "vkCreateRenderPass2");
    table->CreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR) gpa(obj, "vkCreateRenderPass2KHR");
    if (table->CreateRenderPass2 && !table->CreateRenderPass2KHR)
       table->CreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR) table->CreateRenderPass2;
    if (!table->CreateRenderPass2)
       table->CreateRenderPass2 = (PFN_vkCreateRenderPass2) table->CreateRenderPass2KHR;
        table->CmdBeginRenderPass2 = (PFN_vkCmdBeginRenderPass2) gpa(obj, "vkCmdBeginRenderPass2");
    table->CmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR) gpa(obj, "vkCmdBeginRenderPass2KHR");
    if (table->CmdBeginRenderPass2 && !table->CmdBeginRenderPass2KHR)
       table->CmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR) table->CmdBeginRenderPass2;
    if (!table->CmdBeginRenderPass2)
       table->CmdBeginRenderPass2 = (PFN_vkCmdBeginRenderPass2) table->CmdBeginRenderPass2KHR;
        table->CmdNextSubpass2 = (PFN_vkCmdNextSubpass2) gpa(obj, "vkCmdNextSubpass2");
    table->CmdNextSubpass2KHR = (PFN_vkCmdNextSubpass2KHR) gpa(obj, "vkCmdNextSubpass2KHR");
    if (table->CmdNextSubpass2 && !table->CmdNextSubpass2KHR)
       table->CmdNextSubpass2KHR = (PFN_vkCmdNextSubpass2KHR) table->CmdNextSubpass2;
    if (!table->CmdNextSubpass2)
       table->CmdNextSubpass2 = (PFN_vkCmdNextSubpass2) table->CmdNextSubpass2KHR;
        table->CmdEndRenderPass2 = (PFN_vkCmdEndRenderPass2) gpa(obj, "vkCmdEndRenderPass2");
    table->CmdEndRenderPass2KHR = (PFN_vkCmdEndRenderPass2KHR) gpa(obj, "vkCmdEndRenderPass2KHR");
    if (table->CmdEndRenderPass2 && !table->CmdEndRenderPass2KHR)
       table->CmdEndRenderPass2KHR = (PFN_vkCmdEndRenderPass2KHR) table->CmdEndRenderPass2;
    if (!table->CmdEndRenderPass2)
       table->CmdEndRenderPass2 = (PFN_vkCmdEndRenderPass2) table->CmdEndRenderPass2KHR;
        table->GetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) gpa(obj, "vkGetSemaphoreCounterValue");
    table->GetSemaphoreCounterValueKHR = (PFN_vkGetSemaphoreCounterValueKHR) gpa(obj, "vkGetSemaphoreCounterValueKHR");
    if (table->GetSemaphoreCounterValue && !table->GetSemaphoreCounterValueKHR)
       table->GetSemaphoreCounterValueKHR = (PFN_vkGetSemaphoreCounterValueKHR) table->GetSemaphoreCounterValue;
    if (!table->GetSemaphoreCounterValue)
       table->GetSemaphoreCounterValue = (PFN_vkGetSemaphoreCounterValue) table->GetSemaphoreCounterValueKHR;
        table->WaitSemaphores = (PFN_vkWaitSemaphores) gpa(obj, "vkWaitSemaphores");
    table->WaitSemaphoresKHR = (PFN_vkWaitSemaphoresKHR) gpa(obj, "vkWaitSemaphoresKHR");
    if (table->WaitSemaphores && !table->WaitSemaphoresKHR)
       table->WaitSemaphoresKHR = (PFN_vkWaitSemaphoresKHR) table->WaitSemaphores;
    if (!table->WaitSemaphores)
       table->WaitSemaphores = (PFN_vkWaitSemaphores) table->WaitSemaphoresKHR;
        table->SignalSemaphore = (PFN_vkSignalSemaphore) gpa(obj, "vkSignalSemaphore");
    table->SignalSemaphoreKHR = (PFN_vkSignalSemaphoreKHR) gpa(obj, "vkSignalSemaphoreKHR");
    if (table->SignalSemaphore && !table->SignalSemaphoreKHR)
       table->SignalSemaphoreKHR = (PFN_vkSignalSemaphoreKHR) table->SignalSemaphore;
    if (!table->SignalSemaphore)
       table->SignalSemaphore = (PFN_vkSignalSemaphore) table->SignalSemaphoreKHR;
    #ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetAndroidHardwareBufferPropertiesANDROID = (PFN_vkGetAndroidHardwareBufferPropertiesANDROID) gpa(obj, "vkGetAndroidHardwareBufferPropertiesANDROID");
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    table->GetMemoryAndroidHardwareBufferANDROID = (PFN_vkGetMemoryAndroidHardwareBufferANDROID) gpa(obj, "vkGetMemoryAndroidHardwareBufferANDROID");
#endif
    table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) gpa(obj, "vkCmdDrawIndirectCount");
    table->CmdDrawIndirectCountKHR = (PFN_vkCmdDrawIndirectCountKHR) gpa(obj, "vkCmdDrawIndirectCountKHR");
    if (table->CmdDrawIndirectCount && !table->CmdDrawIndirectCountKHR)
       table->CmdDrawIndirectCountKHR = (PFN_vkCmdDrawIndirectCountKHR) table->CmdDrawIndirectCount;
    if (!table->CmdDrawIndirectCount)
       table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) table->CmdDrawIndirectCountKHR;
    table->CmdDrawIndirectCountAMD = (PFN_vkCmdDrawIndirectCountAMD) gpa(obj, "vkCmdDrawIndirectCountAMD");
    if (table->CmdDrawIndirectCount && !table->CmdDrawIndirectCountAMD)
       table->CmdDrawIndirectCountAMD = (PFN_vkCmdDrawIndirectCountAMD) table->CmdDrawIndirectCount;
    if (!table->CmdDrawIndirectCount)
       table->CmdDrawIndirectCount = (PFN_vkCmdDrawIndirectCount) table->CmdDrawIndirectCountAMD;
            table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) gpa(obj, "vkCmdDrawIndexedIndirectCount");
    table->CmdDrawIndexedIndirectCountKHR = (PFN_vkCmdDrawIndexedIndirectCountKHR) gpa(obj, "vkCmdDrawIndexedIndirectCountKHR");
    if (table->CmdDrawIndexedIndirectCount && !table->CmdDrawIndexedIndirectCountKHR)
       table->CmdDrawIndexedIndirectCountKHR = (PFN_vkCmdDrawIndexedIndirectCountKHR) table->CmdDrawIndexedIndirectCount;
    if (!table->CmdDrawIndexedIndirectCount)
       table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) table->CmdDrawIndexedIndirectCountKHR;
    table->CmdDrawIndexedIndirectCountAMD = (PFN_vkCmdDrawIndexedIndirectCountAMD) gpa(obj, "vkCmdDrawIndexedIndirectCountAMD");
    if (table->CmdDrawIndexedIndirectCount && !table->CmdDrawIndexedIndirectCountAMD)
       table->CmdDrawIndexedIndirectCountAMD = (PFN_vkCmdDrawIndexedIndirectCountAMD) table->CmdDrawIndexedIndirectCount;
    if (!table->CmdDrawIndexedIndirectCount)
       table->CmdDrawIndexedIndirectCount = (PFN_vkCmdDrawIndexedIndirectCount) table->CmdDrawIndexedIndirectCountAMD;
            table->CmdSetCheckpointNV = (PFN_vkCmdSetCheckpointNV) gpa(obj, "vkCmdSetCheckpointNV");
    table->GetQueueCheckpointDataNV = (PFN_vkGetQueueCheckpointDataNV) gpa(obj, "vkGetQueueCheckpointDataNV");
    table->CmdBindTransformFeedbackBuffersEXT = (PFN_vkCmdBindTransformFeedbackBuffersEXT) gpa(obj, "vkCmdBindTransformFeedbackBuffersEXT");
    table->CmdBeginTransformFeedbackEXT = (PFN_vkCmdBeginTransformFeedbackEXT) gpa(obj, "vkCmdBeginTransformFeedbackEXT");
    table->CmdEndTransformFeedbackEXT = (PFN_vkCmdEndTransformFeedbackEXT) gpa(obj, "vkCmdEndTransformFeedbackEXT");
    table->CmdBeginQueryIndexedEXT = (PFN_vkCmdBeginQueryIndexedEXT) gpa(obj, "vkCmdBeginQueryIndexedEXT");
    table->CmdEndQueryIndexedEXT = (PFN_vkCmdEndQueryIndexedEXT) gpa(obj, "vkCmdEndQueryIndexedEXT");
    table->CmdDrawIndirectByteCountEXT = (PFN_vkCmdDrawIndirectByteCountEXT) gpa(obj, "vkCmdDrawIndirectByteCountEXT");
    table->CmdSetExclusiveScissorNV = (PFN_vkCmdSetExclusiveScissorNV) gpa(obj, "vkCmdSetExclusiveScissorNV");
    table->CmdSetExclusiveScissorEnableNV = (PFN_vkCmdSetExclusiveScissorEnableNV) gpa(obj, "vkCmdSetExclusiveScissorEnableNV");
    table->CmdBindShadingRateImageNV = (PFN_vkCmdBindShadingRateImageNV) gpa(obj, "vkCmdBindShadingRateImageNV");
    table->CmdSetViewportShadingRatePaletteNV = (PFN_vkCmdSetViewportShadingRatePaletteNV) gpa(obj, "vkCmdSetViewportShadingRatePaletteNV");
    table->CmdSetCoarseSampleOrderNV = (PFN_vkCmdSetCoarseSampleOrderNV) gpa(obj, "vkCmdSetCoarseSampleOrderNV");
    table->CmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV) gpa(obj, "vkCmdDrawMeshTasksNV");
    table->CmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV) gpa(obj, "vkCmdDrawMeshTasksIndirectNV");
    table->CmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV) gpa(obj, "vkCmdDrawMeshTasksIndirectCountNV");
    table->CmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT) gpa(obj, "vkCmdDrawMeshTasksEXT");
    table->CmdDrawMeshTasksIndirectEXT = (PFN_vkCmdDrawMeshTasksIndirectEXT) gpa(obj, "vkCmdDrawMeshTasksIndirectEXT");
    table->CmdDrawMeshTasksIndirectCountEXT = (PFN_vkCmdDrawMeshTasksIndirectCountEXT) gpa(obj, "vkCmdDrawMeshTasksIndirectCountEXT");
    table->CompileDeferredNV = (PFN_vkCompileDeferredNV) gpa(obj, "vkCompileDeferredNV");
    table->CreateAccelerationStructureNV = (PFN_vkCreateAccelerationStructureNV) gpa(obj, "vkCreateAccelerationStructureNV");
    table->CmdBindInvocationMaskHUAWEI = (PFN_vkCmdBindInvocationMaskHUAWEI) gpa(obj, "vkCmdBindInvocationMaskHUAWEI");
    table->DestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR) gpa(obj, "vkDestroyAccelerationStructureKHR");
    table->DestroyAccelerationStructureNV = (PFN_vkDestroyAccelerationStructureNV) gpa(obj, "vkDestroyAccelerationStructureNV");
    table->GetAccelerationStructureMemoryRequirementsNV = (PFN_vkGetAccelerationStructureMemoryRequirementsNV) gpa(obj, "vkGetAccelerationStructureMemoryRequirementsNV");
    table->BindAccelerationStructureMemoryNV = (PFN_vkBindAccelerationStructureMemoryNV) gpa(obj, "vkBindAccelerationStructureMemoryNV");
    table->CmdCopyAccelerationStructureNV = (PFN_vkCmdCopyAccelerationStructureNV) gpa(obj, "vkCmdCopyAccelerationStructureNV");
    table->CmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR) gpa(obj, "vkCmdCopyAccelerationStructureKHR");
    table->CopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR) gpa(obj, "vkCopyAccelerationStructureKHR");
    table->CmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR) gpa(obj, "vkCmdCopyAccelerationStructureToMemoryKHR");
    table->CopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR) gpa(obj, "vkCopyAccelerationStructureToMemoryKHR");
    table->CmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR) gpa(obj, "vkCmdCopyMemoryToAccelerationStructureKHR");
    table->CopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR) gpa(obj, "vkCopyMemoryToAccelerationStructureKHR");
    table->CmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR) gpa(obj, "vkCmdWriteAccelerationStructuresPropertiesKHR");
    table->CmdWriteAccelerationStructuresPropertiesNV = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV) gpa(obj, "vkCmdWriteAccelerationStructuresPropertiesNV");
    table->CmdBuildAccelerationStructureNV = (PFN_vkCmdBuildAccelerationStructureNV) gpa(obj, "vkCmdBuildAccelerationStructureNV");
    table->WriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR) gpa(obj, "vkWriteAccelerationStructuresPropertiesKHR");
    table->CmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR) gpa(obj, "vkCmdTraceRaysKHR");
    table->CmdTraceRaysNV = (PFN_vkCmdTraceRaysNV) gpa(obj, "vkCmdTraceRaysNV");
    table->GetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR) gpa(obj, "vkGetRayTracingShaderGroupHandlesKHR");
    table->GetRayTracingShaderGroupHandlesNV = (PFN_vkGetRayTracingShaderGroupHandlesNV) gpa(obj, "vkGetRayTracingShaderGroupHandlesNV");
    if (table->GetRayTracingShaderGroupHandlesKHR && !table->GetRayTracingShaderGroupHandlesNV)
       table->GetRayTracingShaderGroupHandlesNV = (PFN_vkGetRayTracingShaderGroupHandlesNV) table->GetRayTracingShaderGroupHandlesKHR;
    if (!table->GetRayTracingShaderGroupHandlesKHR)
       table->GetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR) table->GetRayTracingShaderGroupHandlesNV;
        table->GetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR) gpa(obj, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    table->GetAccelerationStructureHandleNV = (PFN_vkGetAccelerationStructureHandleNV) gpa(obj, "vkGetAccelerationStructureHandleNV");
    table->CreateRayTracingPipelinesNV = (PFN_vkCreateRayTracingPipelinesNV) gpa(obj, "vkCreateRayTracingPipelinesNV");
    table->CreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR) gpa(obj, "vkCreateRayTracingPipelinesKHR");
    table->CmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR) gpa(obj, "vkCmdTraceRaysIndirectKHR");
    table->CmdTraceRaysIndirect2KHR = (PFN_vkCmdTraceRaysIndirect2KHR) gpa(obj, "vkCmdTraceRaysIndirect2KHR");
    table->GetClusterAccelerationStructureBuildSizesNV = (PFN_vkGetClusterAccelerationStructureBuildSizesNV) gpa(obj, "vkGetClusterAccelerationStructureBuildSizesNV");
    table->CmdBuildClusterAccelerationStructureIndirectNV = (PFN_vkCmdBuildClusterAccelerationStructureIndirectNV) gpa(obj, "vkCmdBuildClusterAccelerationStructureIndirectNV");
    table->GetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR) gpa(obj, "vkGetDeviceAccelerationStructureCompatibilityKHR");
    table->GetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR) gpa(obj, "vkGetRayTracingShaderGroupStackSizeKHR");
    table->CmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR) gpa(obj, "vkCmdSetRayTracingPipelineStackSizeKHR");
    table->GetImageViewHandleNVX = (PFN_vkGetImageViewHandleNVX) gpa(obj, "vkGetImageViewHandleNVX");
    table->GetImageViewHandle64NVX = (PFN_vkGetImageViewHandle64NVX) gpa(obj, "vkGetImageViewHandle64NVX");
    table->GetImageViewAddressNVX = (PFN_vkGetImageViewAddressNVX) gpa(obj, "vkGetImageViewAddressNVX");
    table->GetDeviceCombinedImageSamplerIndexNVX = (PFN_vkGetDeviceCombinedImageSamplerIndexNVX) gpa(obj, "vkGetDeviceCombinedImageSamplerIndexNVX");
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->GetDeviceGroupSurfacePresentModes2EXT = (PFN_vkGetDeviceGroupSurfacePresentModes2EXT) gpa(obj, "vkGetDeviceGroupSurfacePresentModes2EXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->AcquireFullScreenExclusiveModeEXT = (PFN_vkAcquireFullScreenExclusiveModeEXT) gpa(obj, "vkAcquireFullScreenExclusiveModeEXT");
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    table->ReleaseFullScreenExclusiveModeEXT = (PFN_vkReleaseFullScreenExclusiveModeEXT) gpa(obj, "vkReleaseFullScreenExclusiveModeEXT");
#endif
    table->AcquireProfilingLockKHR = (PFN_vkAcquireProfilingLockKHR) gpa(obj, "vkAcquireProfilingLockKHR");
    table->ReleaseProfilingLockKHR = (PFN_vkReleaseProfilingLockKHR) gpa(obj, "vkReleaseProfilingLockKHR");
    table->GetImageDrmFormatModifierPropertiesEXT = (PFN_vkGetImageDrmFormatModifierPropertiesEXT) gpa(obj, "vkGetImageDrmFormatModifierPropertiesEXT");
    table->GetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress) gpa(obj, "vkGetBufferOpaqueCaptureAddress");
    table->GetBufferOpaqueCaptureAddressKHR = (PFN_vkGetBufferOpaqueCaptureAddressKHR) gpa(obj, "vkGetBufferOpaqueCaptureAddressKHR");
    if (table->GetBufferOpaqueCaptureAddress && !table->GetBufferOpaqueCaptureAddressKHR)
       table->GetBufferOpaqueCaptureAddressKHR = (PFN_vkGetBufferOpaqueCaptureAddressKHR) table->GetBufferOpaqueCaptureAddress;
    if (!table->GetBufferOpaqueCaptureAddress)
       table->GetBufferOpaqueCaptureAddress = (PFN_vkGetBufferOpaqueCaptureAddress) table->GetBufferOpaqueCaptureAddressKHR;
        table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) gpa(obj, "vkGetBufferDeviceAddress");
    table->GetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR) gpa(obj, "vkGetBufferDeviceAddressKHR");
    if (table->GetBufferDeviceAddress && !table->GetBufferDeviceAddressKHR)
       table->GetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR) table->GetBufferDeviceAddress;
    if (!table->GetBufferDeviceAddress)
       table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) table->GetBufferDeviceAddressKHR;
    table->GetBufferDeviceAddressEXT = (PFN_vkGetBufferDeviceAddressEXT) gpa(obj, "vkGetBufferDeviceAddressEXT");
    if (table->GetBufferDeviceAddress && !table->GetBufferDeviceAddressEXT)
       table->GetBufferDeviceAddressEXT = (PFN_vkGetBufferDeviceAddressEXT) table->GetBufferDeviceAddress;
    if (!table->GetBufferDeviceAddress)
       table->GetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress) table->GetBufferDeviceAddressEXT;
            table->InitializePerformanceApiINTEL = (PFN_vkInitializePerformanceApiINTEL) gpa(obj, "vkInitializePerformanceApiINTEL");
    table->UninitializePerformanceApiINTEL = (PFN_vkUninitializePerformanceApiINTEL) gpa(obj, "vkUninitializePerformanceApiINTEL");
    table->CmdSetPerformanceMarkerINTEL = (PFN_vkCmdSetPerformanceMarkerINTEL) gpa(obj, "vkCmdSetPerformanceMarkerINTEL");
    table->CmdSetPerformanceStreamMarkerINTEL = (PFN_vkCmdSetPerformanceStreamMarkerINTEL) gpa(obj, "vkCmdSetPerformanceStreamMarkerINTEL");
    table->CmdSetPerformanceOverrideINTEL = (PFN_vkCmdSetPerformanceOverrideINTEL) gpa(obj, "vkCmdSetPerformanceOverrideINTEL");
    table->AcquirePerformanceConfigurationINTEL = (PFN_vkAcquirePerformanceConfigurationINTEL) gpa(obj, "vkAcquirePerformanceConfigurationINTEL");
    table->ReleasePerformanceConfigurationINTEL = (PFN_vkReleasePerformanceConfigurationINTEL) gpa(obj, "vkReleasePerformanceConfigurationINTEL");
    table->QueueSetPerformanceConfigurationINTEL = (PFN_vkQueueSetPerformanceConfigurationINTEL) gpa(obj, "vkQueueSetPerformanceConfigurationINTEL");
    table->GetPerformanceParameterINTEL = (PFN_vkGetPerformanceParameterINTEL) gpa(obj, "vkGetPerformanceParameterINTEL");
    table->GetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress) gpa(obj, "vkGetDeviceMemoryOpaqueCaptureAddress");
    table->GetDeviceMemoryOpaqueCaptureAddressKHR = (PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR) gpa(obj, "vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    if (table->GetDeviceMemoryOpaqueCaptureAddress && !table->GetDeviceMemoryOpaqueCaptureAddressKHR)
       table->GetDeviceMemoryOpaqueCaptureAddressKHR = (PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR) table->GetDeviceMemoryOpaqueCaptureAddress;
    if (!table->GetDeviceMemoryOpaqueCaptureAddress)
       table->GetDeviceMemoryOpaqueCaptureAddress = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress) table->GetDeviceMemoryOpaqueCaptureAddressKHR;
        table->GetPipelineExecutablePropertiesKHR = (PFN_vkGetPipelineExecutablePropertiesKHR) gpa(obj, "vkGetPipelineExecutablePropertiesKHR");
    table->GetPipelineExecutableStatisticsKHR = (PFN_vkGetPipelineExecutableStatisticsKHR) gpa(obj, "vkGetPipelineExecutableStatisticsKHR");
    table->GetPipelineExecutableInternalRepresentationsKHR = (PFN_vkGetPipelineExecutableInternalRepresentationsKHR) gpa(obj, "vkGetPipelineExecutableInternalRepresentationsKHR");
    table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) gpa(obj, "vkCmdSetLineStipple");
    table->CmdSetLineStippleKHR = (PFN_vkCmdSetLineStippleKHR) gpa(obj, "vkCmdSetLineStippleKHR");
    if (table->CmdSetLineStipple && !table->CmdSetLineStippleKHR)
       table->CmdSetLineStippleKHR = (PFN_vkCmdSetLineStippleKHR) table->CmdSetLineStipple;
    if (!table->CmdSetLineStipple)
       table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) table->CmdSetLineStippleKHR;
    table->CmdSetLineStippleEXT = (PFN_vkCmdSetLineStippleEXT) gpa(obj, "vkCmdSetLineStippleEXT");
    if (table->CmdSetLineStipple && !table->CmdSetLineStippleEXT)
       table->CmdSetLineStippleEXT = (PFN_vkCmdSetLineStippleEXT) table->CmdSetLineStipple;
    if (!table->CmdSetLineStipple)
       table->CmdSetLineStipple = (PFN_vkCmdSetLineStipple) table->CmdSetLineStippleEXT;
            table->CreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR) gpa(obj, "vkCreateAccelerationStructureKHR");
    table->CmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR) gpa(obj, "vkCmdBuildAccelerationStructuresKHR");
    table->CmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR) gpa(obj, "vkCmdBuildAccelerationStructuresIndirectKHR");
    table->BuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR) gpa(obj, "vkBuildAccelerationStructuresKHR");
    table->GetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR) gpa(obj, "vkGetAccelerationStructureDeviceAddressKHR");
    table->CreateDeferredOperationKHR = (PFN_vkCreateDeferredOperationKHR) gpa(obj, "vkCreateDeferredOperationKHR");
    table->DestroyDeferredOperationKHR = (PFN_vkDestroyDeferredOperationKHR) gpa(obj, "vkDestroyDeferredOperationKHR");
    table->GetDeferredOperationMaxConcurrencyKHR = (PFN_vkGetDeferredOperationMaxConcurrencyKHR) gpa(obj, "vkGetDeferredOperationMaxConcurrencyKHR");
    table->GetDeferredOperationResultKHR = (PFN_vkGetDeferredOperationResultKHR) gpa(obj, "vkGetDeferredOperationResultKHR");
    table->DeferredOperationJoinKHR = (PFN_vkDeferredOperationJoinKHR) gpa(obj, "vkDeferredOperationJoinKHR");
    table->GetPipelineIndirectMemoryRequirementsNV = (PFN_vkGetPipelineIndirectMemoryRequirementsNV) gpa(obj, "vkGetPipelineIndirectMemoryRequirementsNV");
    table->GetPipelineIndirectDeviceAddressNV = (PFN_vkGetPipelineIndirectDeviceAddressNV) gpa(obj, "vkGetPipelineIndirectDeviceAddressNV");
    table->AntiLagUpdateAMD = (PFN_vkAntiLagUpdateAMD) gpa(obj, "vkAntiLagUpdateAMD");
    table->CmdSetCullMode = (PFN_vkCmdSetCullMode) gpa(obj, "vkCmdSetCullMode");
    table->CmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT) gpa(obj, "vkCmdSetCullModeEXT");
    if (table->CmdSetCullMode && !table->CmdSetCullModeEXT)
       table->CmdSetCullModeEXT = (PFN_vkCmdSetCullModeEXT) table->CmdSetCullMode;
    if (!table->CmdSetCullMode)
       table->CmdSetCullMode = (PFN_vkCmdSetCullMode) table->CmdSetCullModeEXT;
        table->CmdSetFrontFace = (PFN_vkCmdSetFrontFace) gpa(obj, "vkCmdSetFrontFace");
    table->CmdSetFrontFaceEXT = (PFN_vkCmdSetFrontFaceEXT) gpa(obj, "vkCmdSetFrontFaceEXT");
    if (table->CmdSetFrontFace && !table->CmdSetFrontFaceEXT)
       table->CmdSetFrontFaceEXT = (PFN_vkCmdSetFrontFaceEXT) table->CmdSetFrontFace;
    if (!table->CmdSetFrontFace)
       table->CmdSetFrontFace = (PFN_vkCmdSetFrontFace) table->CmdSetFrontFaceEXT;
        table->CmdSetPrimitiveTopology = (PFN_vkCmdSetPrimitiveTopology) gpa(obj, "vkCmdSetPrimitiveTopology");
    table->CmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT) gpa(obj, "vkCmdSetPrimitiveTopologyEXT");
    if (table->CmdSetPrimitiveTopology && !table->CmdSetPrimitiveTopologyEXT)
       table->CmdSetPrimitiveTopologyEXT = (PFN_vkCmdSetPrimitiveTopologyEXT) table->CmdSetPrimitiveTopology;
    if (!table->CmdSetPrimitiveTopology)
       table->CmdSetPrimitiveTopology = (PFN_vkCmdSetPrimitiveTopology) table->CmdSetPrimitiveTopologyEXT;
        table->CmdSetViewportWithCount = (PFN_vkCmdSetViewportWithCount) gpa(obj, "vkCmdSetViewportWithCount");
    table->CmdSetViewportWithCountEXT = (PFN_vkCmdSetViewportWithCountEXT) gpa(obj, "vkCmdSetViewportWithCountEXT");
    if (table->CmdSetViewportWithCount && !table->CmdSetViewportWithCountEXT)
       table->CmdSetViewportWithCountEXT = (PFN_vkCmdSetViewportWithCountEXT) table->CmdSetViewportWithCount;
    if (!table->CmdSetViewportWithCount)
       table->CmdSetViewportWithCount = (PFN_vkCmdSetViewportWithCount) table->CmdSetViewportWithCountEXT;
        table->CmdSetScissorWithCount = (PFN_vkCmdSetScissorWithCount) gpa(obj, "vkCmdSetScissorWithCount");
    table->CmdSetScissorWithCountEXT = (PFN_vkCmdSetScissorWithCountEXT) gpa(obj, "vkCmdSetScissorWithCountEXT");
    if (table->CmdSetScissorWithCount && !table->CmdSetScissorWithCountEXT)
       table->CmdSetScissorWithCountEXT = (PFN_vkCmdSetScissorWithCountEXT) table->CmdSetScissorWithCount;
    if (!table->CmdSetScissorWithCount)
       table->CmdSetScissorWithCount = (PFN_vkCmdSetScissorWithCount) table->CmdSetScissorWithCountEXT;
        table->CmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2) gpa(obj, "vkCmdBindIndexBuffer2");
    table->CmdBindIndexBuffer2KHR = (PFN_vkCmdBindIndexBuffer2KHR) gpa(obj, "vkCmdBindIndexBuffer2KHR");
    if (table->CmdBindIndexBuffer2 && !table->CmdBindIndexBuffer2KHR)
       table->CmdBindIndexBuffer2KHR = (PFN_vkCmdBindIndexBuffer2KHR) table->CmdBindIndexBuffer2;
    if (!table->CmdBindIndexBuffer2)
       table->CmdBindIndexBuffer2 = (PFN_vkCmdBindIndexBuffer2) table->CmdBindIndexBuffer2KHR;
        table->CmdBindVertexBuffers2 = (PFN_vkCmdBindVertexBuffers2) gpa(obj, "vkCmdBindVertexBuffers2");
    table->CmdBindVertexBuffers2EXT = (PFN_vkCmdBindVertexBuffers2EXT) gpa(obj, "vkCmdBindVertexBuffers2EXT");
    if (table->CmdBindVertexBuffers2 && !table->CmdBindVertexBuffers2EXT)
       table->CmdBindVertexBuffers2EXT = (PFN_vkCmdBindVertexBuffers2EXT) table->CmdBindVertexBuffers2;
    if (!table->CmdBindVertexBuffers2)
       table->CmdBindVertexBuffers2 = (PFN_vkCmdBindVertexBuffers2) table->CmdBindVertexBuffers2EXT;
        table->CmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable) gpa(obj, "vkCmdSetDepthTestEnable");
    table->CmdSetDepthTestEnableEXT = (PFN_vkCmdSetDepthTestEnableEXT) gpa(obj, "vkCmdSetDepthTestEnableEXT");
    if (table->CmdSetDepthTestEnable && !table->CmdSetDepthTestEnableEXT)
       table->CmdSetDepthTestEnableEXT = (PFN_vkCmdSetDepthTestEnableEXT) table->CmdSetDepthTestEnable;
    if (!table->CmdSetDepthTestEnable)
       table->CmdSetDepthTestEnable = (PFN_vkCmdSetDepthTestEnable) table->CmdSetDepthTestEnableEXT;
        table->CmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) gpa(obj, "vkCmdSetDepthWriteEnable");
    table->CmdSetDepthWriteEnableEXT = (PFN_vkCmdSetDepthWriteEnableEXT) gpa(obj, "vkCmdSetDepthWriteEnableEXT");
    if (table->CmdSetDepthWriteEnable && !table->CmdSetDepthWriteEnableEXT)
       table->CmdSetDepthWriteEnableEXT = (PFN_vkCmdSetDepthWriteEnableEXT) table->CmdSetDepthWriteEnable;
    if (!table->CmdSetDepthWriteEnable)
       table->CmdSetDepthWriteEnable = (PFN_vkCmdSetDepthWriteEnable) table->CmdSetDepthWriteEnableEXT;
        table->CmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) gpa(obj, "vkCmdSetDepthCompareOp");
    table->CmdSetDepthCompareOpEXT = (PFN_vkCmdSetDepthCompareOpEXT) gpa(obj, "vkCmdSetDepthCompareOpEXT");
    if (table->CmdSetDepthCompareOp && !table->CmdSetDepthCompareOpEXT)
       table->CmdSetDepthCompareOpEXT = (PFN_vkCmdSetDepthCompareOpEXT) table->CmdSetDepthCompareOp;
    if (!table->CmdSetDepthCompareOp)
       table->CmdSetDepthCompareOp = (PFN_vkCmdSetDepthCompareOp) table->CmdSetDepthCompareOpEXT;
        table->CmdSetDepthBoundsTestEnable = (PFN_vkCmdSetDepthBoundsTestEnable) gpa(obj, "vkCmdSetDepthBoundsTestEnable");
    table->CmdSetDepthBoundsTestEnableEXT = (PFN_vkCmdSetDepthBoundsTestEnableEXT) gpa(obj, "vkCmdSetDepthBoundsTestEnableEXT");
    if (table->CmdSetDepthBoundsTestEnable && !table->CmdSetDepthBoundsTestEnableEXT)
       table->CmdSetDepthBoundsTestEnableEXT = (PFN_vkCmdSetDepthBoundsTestEnableEXT) table->CmdSetDepthBoundsTestEnable;
    if (!table->CmdSetDepthBoundsTestEnable)
       table->CmdSetDepthBoundsTestEnable = (PFN_vkCmdSetDepthBoundsTestEnable) table->CmdSetDepthBoundsTestEnableEXT;
        table->CmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable) gpa(obj, "vkCmdSetStencilTestEnable");
    table->CmdSetStencilTestEnableEXT = (PFN_vkCmdSetStencilTestEnableEXT) gpa(obj, "vkCmdSetStencilTestEnableEXT");
    if (table->CmdSetStencilTestEnable && !table->CmdSetStencilTestEnableEXT)
       table->CmdSetStencilTestEnableEXT = (PFN_vkCmdSetStencilTestEnableEXT) table->CmdSetStencilTestEnable;
    if (!table->CmdSetStencilTestEnable)
       table->CmdSetStencilTestEnable = (PFN_vkCmdSetStencilTestEnable) table->CmdSetStencilTestEnableEXT;
        table->CmdSetStencilOp = (PFN_vkCmdSetStencilOp) gpa(obj, "vkCmdSetStencilOp");
    table->CmdSetStencilOpEXT = (PFN_vkCmdSetStencilOpEXT) gpa(obj, "vkCmdSetStencilOpEXT");
    if (table->CmdSetStencilOp && !table->CmdSetStencilOpEXT)
       table->CmdSetStencilOpEXT = (PFN_vkCmdSetStencilOpEXT) table->CmdSetStencilOp;
    if (!table->CmdSetStencilOp)
       table->CmdSetStencilOp = (PFN_vkCmdSetStencilOp) table->CmdSetStencilOpEXT;
        table->CmdSetPatchControlPointsEXT = (PFN_vkCmdSetPatchControlPointsEXT) gpa(obj, "vkCmdSetPatchControlPointsEXT");
    table->CmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable) gpa(obj, "vkCmdSetRasterizerDiscardEnable");
    table->CmdSetRasterizerDiscardEnableEXT = (PFN_vkCmdSetRasterizerDiscardEnableEXT) gpa(obj, "vkCmdSetRasterizerDiscardEnableEXT");
    if (table->CmdSetRasterizerDiscardEnable && !table->CmdSetRasterizerDiscardEnableEXT)
       table->CmdSetRasterizerDiscardEnableEXT = (PFN_vkCmdSetRasterizerDiscardEnableEXT) table->CmdSetRasterizerDiscardEnable;
    if (!table->CmdSetRasterizerDiscardEnable)
       table->CmdSetRasterizerDiscardEnable = (PFN_vkCmdSetRasterizerDiscardEnable) table->CmdSetRasterizerDiscardEnableEXT;
        table->CmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) gpa(obj, "vkCmdSetDepthBiasEnable");
    table->CmdSetDepthBiasEnableEXT = (PFN_vkCmdSetDepthBiasEnableEXT) gpa(obj, "vkCmdSetDepthBiasEnableEXT");
    if (table->CmdSetDepthBiasEnable && !table->CmdSetDepthBiasEnableEXT)
       table->CmdSetDepthBiasEnableEXT = (PFN_vkCmdSetDepthBiasEnableEXT) table->CmdSetDepthBiasEnable;
    if (!table->CmdSetDepthBiasEnable)
       table->CmdSetDepthBiasEnable = (PFN_vkCmdSetDepthBiasEnable) table->CmdSetDepthBiasEnableEXT;
        table->CmdSetLogicOpEXT = (PFN_vkCmdSetLogicOpEXT) gpa(obj, "vkCmdSetLogicOpEXT");
    table->CmdSetPrimitiveRestartEnable = (PFN_vkCmdSetPrimitiveRestartEnable) gpa(obj, "vkCmdSetPrimitiveRestartEnable");
    table->CmdSetPrimitiveRestartEnableEXT = (PFN_vkCmdSetPrimitiveRestartEnableEXT) gpa(obj, "vkCmdSetPrimitiveRestartEnableEXT");
    if (table->CmdSetPrimitiveRestartEnable && !table->CmdSetPrimitiveRestartEnableEXT)
       table->CmdSetPrimitiveRestartEnableEXT = (PFN_vkCmdSetPrimitiveRestartEnableEXT) table->CmdSetPrimitiveRestartEnable;
    if (!table->CmdSetPrimitiveRestartEnable)
       table->CmdSetPrimitiveRestartEnable = (PFN_vkCmdSetPrimitiveRestartEnable) table->CmdSetPrimitiveRestartEnableEXT;
        table->CmdSetTessellationDomainOriginEXT = (PFN_vkCmdSetTessellationDomainOriginEXT) gpa(obj, "vkCmdSetTessellationDomainOriginEXT");
    table->CmdSetDepthClampEnableEXT = (PFN_vkCmdSetDepthClampEnableEXT) gpa(obj, "vkCmdSetDepthClampEnableEXT");
    table->CmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) gpa(obj, "vkCmdSetPolygonModeEXT");
    table->CmdSetRasterizationSamplesEXT = (PFN_vkCmdSetRasterizationSamplesEXT) gpa(obj, "vkCmdSetRasterizationSamplesEXT");
    table->CmdSetSampleMaskEXT = (PFN_vkCmdSetSampleMaskEXT) gpa(obj, "vkCmdSetSampleMaskEXT");
    table->CmdSetAlphaToCoverageEnableEXT = (PFN_vkCmdSetAlphaToCoverageEnableEXT) gpa(obj, "vkCmdSetAlphaToCoverageEnableEXT");
    table->CmdSetAlphaToOneEnableEXT = (PFN_vkCmdSetAlphaToOneEnableEXT) gpa(obj, "vkCmdSetAlphaToOneEnableEXT");
    table->CmdSetLogicOpEnableEXT = (PFN_vkCmdSetLogicOpEnableEXT) gpa(obj, "vkCmdSetLogicOpEnableEXT");
    table->CmdSetColorBlendEnableEXT = (PFN_vkCmdSetColorBlendEnableEXT) gpa(obj, "vkCmdSetColorBlendEnableEXT");
    table->CmdSetColorBlendEquationEXT = (PFN_vkCmdSetColorBlendEquationEXT) gpa(obj, "vkCmdSetColorBlendEquationEXT");
    table->CmdSetColorWriteMaskEXT = (PFN_vkCmdSetColorWriteMaskEXT) gpa(obj, "vkCmdSetColorWriteMaskEXT");
    table->CmdSetRasterizationStreamEXT = (PFN_vkCmdSetRasterizationStreamEXT) gpa(obj, "vkCmdSetRasterizationStreamEXT");
    table->CmdSetConservativeRasterizationModeEXT = (PFN_vkCmdSetConservativeRasterizationModeEXT) gpa(obj, "vkCmdSetConservativeRasterizationModeEXT");
    table->CmdSetExtraPrimitiveOverestimationSizeEXT = (PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT) gpa(obj, "vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    table->CmdSetDepthClipEnableEXT = (PFN_vkCmdSetDepthClipEnableEXT) gpa(obj, "vkCmdSetDepthClipEnableEXT");
    table->CmdSetSampleLocationsEnableEXT = (PFN_vkCmdSetSampleLocationsEnableEXT) gpa(obj, "vkCmdSetSampleLocationsEnableEXT");
    table->CmdSetColorBlendAdvancedEXT = (PFN_vkCmdSetColorBlendAdvancedEXT) gpa(obj, "vkCmdSetColorBlendAdvancedEXT");
    table->CmdSetProvokingVertexModeEXT = (PFN_vkCmdSetProvokingVertexModeEXT) gpa(obj, "vkCmdSetProvokingVertexModeEXT");
    table->CmdSetLineRasterizationModeEXT = (PFN_vkCmdSetLineRasterizationModeEXT) gpa(obj, "vkCmdSetLineRasterizationModeEXT");
    table->CmdSetLineStippleEnableEXT = (PFN_vkCmdSetLineStippleEnableEXT) gpa(obj, "vkCmdSetLineStippleEnableEXT");
    table->CmdSetDepthClipNegativeOneToOneEXT = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT) gpa(obj, "vkCmdSetDepthClipNegativeOneToOneEXT");
    table->CmdSetViewportWScalingEnableNV = (PFN_vkCmdSetViewportWScalingEnableNV) gpa(obj, "vkCmdSetViewportWScalingEnableNV");
    table->CmdSetViewportSwizzleNV = (PFN_vkCmdSetViewportSwizzleNV) gpa(obj, "vkCmdSetViewportSwizzleNV");
    table->CmdSetCoverageToColorEnableNV = (PFN_vkCmdSetCoverageToColorEnableNV) gpa(obj, "vkCmdSetCoverageToColorEnableNV");
    table->CmdSetCoverageToColorLocationNV = (PFN_vkCmdSetCoverageToColorLocationNV) gpa(obj, "vkCmdSetCoverageToColorLocationNV");
    table->CmdSetCoverageModulationModeNV = (PFN_vkCmdSetCoverageModulationModeNV) gpa(obj, "vkCmdSetCoverageModulationModeNV");
    table->CmdSetCoverageModulationTableEnableNV = (PFN_vkCmdSetCoverageModulationTableEnableNV) gpa(obj, "vkCmdSetCoverageModulationTableEnableNV");
    table->CmdSetCoverageModulationTableNV = (PFN_vkCmdSetCoverageModulationTableNV) gpa(obj, "vkCmdSetCoverageModulationTableNV");
    table->CmdSetShadingRateImageEnableNV = (PFN_vkCmdSetShadingRateImageEnableNV) gpa(obj, "vkCmdSetShadingRateImageEnableNV");
    table->CmdSetCoverageReductionModeNV = (PFN_vkCmdSetCoverageReductionModeNV) gpa(obj, "vkCmdSetCoverageReductionModeNV");
    table->CmdSetRepresentativeFragmentTestEnableNV = (PFN_vkCmdSetRepresentativeFragmentTestEnableNV) gpa(obj, "vkCmdSetRepresentativeFragmentTestEnableNV");
    table->CreatePrivateDataSlot = (PFN_vkCreatePrivateDataSlot) gpa(obj, "vkCreatePrivateDataSlot");
    table->CreatePrivateDataSlotEXT = (PFN_vkCreatePrivateDataSlotEXT) gpa(obj, "vkCreatePrivateDataSlotEXT");
    if (table->CreatePrivateDataSlot && !table->CreatePrivateDataSlotEXT)
       table->CreatePrivateDataSlotEXT = (PFN_vkCreatePrivateDataSlotEXT) table->CreatePrivateDataSlot;
    if (!table->CreatePrivateDataSlot)
       table->CreatePrivateDataSlot = (PFN_vkCreatePrivateDataSlot) table->CreatePrivateDataSlotEXT;
        table->DestroyPrivateDataSlot = (PFN_vkDestroyPrivateDataSlot) gpa(obj, "vkDestroyPrivateDataSlot");
    table->DestroyPrivateDataSlotEXT = (PFN_vkDestroyPrivateDataSlotEXT) gpa(obj, "vkDestroyPrivateDataSlotEXT");
    if (table->DestroyPrivateDataSlot && !table->DestroyPrivateDataSlotEXT)
       table->DestroyPrivateDataSlotEXT = (PFN_vkDestroyPrivateDataSlotEXT) table->DestroyPrivateDataSlot;
    if (!table->DestroyPrivateDataSlot)
       table->DestroyPrivateDataSlot = (PFN_vkDestroyPrivateDataSlot) table->DestroyPrivateDataSlotEXT;
        table->SetPrivateData = (PFN_vkSetPrivateData) gpa(obj, "vkSetPrivateData");
    table->SetPrivateDataEXT = (PFN_vkSetPrivateDataEXT) gpa(obj, "vkSetPrivateDataEXT");
    if (table->SetPrivateData && !table->SetPrivateDataEXT)
       table->SetPrivateDataEXT = (PFN_vkSetPrivateDataEXT) table->SetPrivateData;
    if (!table->SetPrivateData)
       table->SetPrivateData = (PFN_vkSetPrivateData) table->SetPrivateDataEXT;
        table->GetPrivateData = (PFN_vkGetPrivateData) gpa(obj, "vkGetPrivateData");
    table->GetPrivateDataEXT = (PFN_vkGetPrivateDataEXT) gpa(obj, "vkGetPrivateDataEXT");
    if (table->GetPrivateData && !table->GetPrivateDataEXT)
       table->GetPrivateDataEXT = (PFN_vkGetPrivateDataEXT) table->GetPrivateData;
    if (!table->GetPrivateData)
       table->GetPrivateData = (PFN_vkGetPrivateData) table->GetPrivateDataEXT;
        table->CmdCopyBuffer2 = (PFN_vkCmdCopyBuffer2) gpa(obj, "vkCmdCopyBuffer2");
    table->CmdCopyBuffer2KHR = (PFN_vkCmdCopyBuffer2KHR) gpa(obj, "vkCmdCopyBuffer2KHR");
    if (table->CmdCopyBuffer2 && !table->CmdCopyBuffer2KHR)
       table->CmdCopyBuffer2KHR = (PFN_vkCmdCopyBuffer2KHR) table->CmdCopyBuffer2;
    if (!table->CmdCopyBuffer2)
       table->CmdCopyBuffer2 = (PFN_vkCmdCopyBuffer2) table->CmdCopyBuffer2KHR;
        table->CmdCopyImage2 = (PFN_vkCmdCopyImage2) gpa(obj, "vkCmdCopyImage2");
    table->CmdCopyImage2KHR = (PFN_vkCmdCopyImage2KHR) gpa(obj, "vkCmdCopyImage2KHR");
    if (table->CmdCopyImage2 && !table->CmdCopyImage2KHR)
       table->CmdCopyImage2KHR = (PFN_vkCmdCopyImage2KHR) table->CmdCopyImage2;
    if (!table->CmdCopyImage2)
       table->CmdCopyImage2 = (PFN_vkCmdCopyImage2) table->CmdCopyImage2KHR;
        table->CmdBlitImage2 = (PFN_vkCmdBlitImage2) gpa(obj, "vkCmdBlitImage2");
    table->CmdBlitImage2KHR = (PFN_vkCmdBlitImage2KHR) gpa(obj, "vkCmdBlitImage2KHR");
    if (table->CmdBlitImage2 && !table->CmdBlitImage2KHR)
       table->CmdBlitImage2KHR = (PFN_vkCmdBlitImage2KHR) table->CmdBlitImage2;
    if (!table->CmdBlitImage2)
       table->CmdBlitImage2 = (PFN_vkCmdBlitImage2) table->CmdBlitImage2KHR;
        table->CmdCopyBufferToImage2 = (PFN_vkCmdCopyBufferToImage2) gpa(obj, "vkCmdCopyBufferToImage2");
    table->CmdCopyBufferToImage2KHR = (PFN_vkCmdCopyBufferToImage2KHR) gpa(obj, "vkCmdCopyBufferToImage2KHR");
    if (table->CmdCopyBufferToImage2 && !table->CmdCopyBufferToImage2KHR)
       table->CmdCopyBufferToImage2KHR = (PFN_vkCmdCopyBufferToImage2KHR) table->CmdCopyBufferToImage2;
    if (!table->CmdCopyBufferToImage2)
       table->CmdCopyBufferToImage2 = (PFN_vkCmdCopyBufferToImage2) table->CmdCopyBufferToImage2KHR;
        table->CmdCopyImageToBuffer2 = (PFN_vkCmdCopyImageToBuffer2) gpa(obj, "vkCmdCopyImageToBuffer2");
    table->CmdCopyImageToBuffer2KHR = (PFN_vkCmdCopyImageToBuffer2KHR) gpa(obj, "vkCmdCopyImageToBuffer2KHR");
    if (table->CmdCopyImageToBuffer2 && !table->CmdCopyImageToBuffer2KHR)
       table->CmdCopyImageToBuffer2KHR = (PFN_vkCmdCopyImageToBuffer2KHR) table->CmdCopyImageToBuffer2;
    if (!table->CmdCopyImageToBuffer2)
       table->CmdCopyImageToBuffer2 = (PFN_vkCmdCopyImageToBuffer2) table->CmdCopyImageToBuffer2KHR;
        table->CmdResolveImage2 = (PFN_vkCmdResolveImage2) gpa(obj, "vkCmdResolveImage2");
    table->CmdResolveImage2KHR = (PFN_vkCmdResolveImage2KHR) gpa(obj, "vkCmdResolveImage2KHR");
    if (table->CmdResolveImage2 && !table->CmdResolveImage2KHR)
       table->CmdResolveImage2KHR = (PFN_vkCmdResolveImage2KHR) table->CmdResolveImage2;
    if (!table->CmdResolveImage2)
       table->CmdResolveImage2 = (PFN_vkCmdResolveImage2) table->CmdResolveImage2KHR;
        table->CmdSetFragmentShadingRateKHR = (PFN_vkCmdSetFragmentShadingRateKHR) gpa(obj, "vkCmdSetFragmentShadingRateKHR");
    table->CmdSetFragmentShadingRateEnumNV = (PFN_vkCmdSetFragmentShadingRateEnumNV) gpa(obj, "vkCmdSetFragmentShadingRateEnumNV");
    table->GetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR) gpa(obj, "vkGetAccelerationStructureBuildSizesKHR");
    table->CmdSetVertexInputEXT = (PFN_vkCmdSetVertexInputEXT) gpa(obj, "vkCmdSetVertexInputEXT");
    table->CmdSetColorWriteEnableEXT = (PFN_vkCmdSetColorWriteEnableEXT) gpa(obj, "vkCmdSetColorWriteEnableEXT");
    table->CmdSetEvent2 = (PFN_vkCmdSetEvent2) gpa(obj, "vkCmdSetEvent2");
    table->CmdSetEvent2KHR = (PFN_vkCmdSetEvent2KHR) gpa(obj, "vkCmdSetEvent2KHR");
    if (table->CmdSetEvent2 && !table->CmdSetEvent2KHR)
       table->CmdSetEvent2KHR = (PFN_vkCmdSetEvent2KHR) table->CmdSetEvent2;
    if (!table->CmdSetEvent2)
       table->CmdSetEvent2 = (PFN_vkCmdSetEvent2) table->CmdSetEvent2KHR;
        table->CmdResetEvent2 = (PFN_vkCmdResetEvent2) gpa(obj, "vkCmdResetEvent2");
    table->CmdResetEvent2KHR = (PFN_vkCmdResetEvent2KHR) gpa(obj, "vkCmdResetEvent2KHR");
    if (table->CmdResetEvent2 && !table->CmdResetEvent2KHR)
       table->CmdResetEvent2KHR = (PFN_vkCmdResetEvent2KHR) table->CmdResetEvent2;
    if (!table->CmdResetEvent2)
       table->CmdResetEvent2 = (PFN_vkCmdResetEvent2) table->CmdResetEvent2KHR;
        table->CmdWaitEvents2 = (PFN_vkCmdWaitEvents2) gpa(obj, "vkCmdWaitEvents2");
    table->CmdWaitEvents2KHR = (PFN_vkCmdWaitEvents2KHR) gpa(obj, "vkCmdWaitEvents2KHR");
    if (table->CmdWaitEvents2 && !table->CmdWaitEvents2KHR)
       table->CmdWaitEvents2KHR = (PFN_vkCmdWaitEvents2KHR) table->CmdWaitEvents2;
    if (!table->CmdWaitEvents2)
       table->CmdWaitEvents2 = (PFN_vkCmdWaitEvents2) table->CmdWaitEvents2KHR;
        table->CmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) gpa(obj, "vkCmdPipelineBarrier2");
    table->CmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) gpa(obj, "vkCmdPipelineBarrier2KHR");
    if (table->CmdPipelineBarrier2 && !table->CmdPipelineBarrier2KHR)
       table->CmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) table->CmdPipelineBarrier2;
    if (!table->CmdPipelineBarrier2)
       table->CmdPipelineBarrier2 = (PFN_vkCmdPipelineBarrier2) table->CmdPipelineBarrier2KHR;
        table->QueueSubmit2 = (PFN_vkQueueSubmit2) gpa(obj, "vkQueueSubmit2");
    table->QueueSubmit2KHR = (PFN_vkQueueSubmit2KHR) gpa(obj, "vkQueueSubmit2KHR");
    if (table->QueueSubmit2 && !table->QueueSubmit2KHR)
       table->QueueSubmit2KHR = (PFN_vkQueueSubmit2KHR) table->QueueSubmit2;
    if (!table->QueueSubmit2)
       table->QueueSubmit2 = (PFN_vkQueueSubmit2) table->QueueSubmit2KHR;
        table->CmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2) gpa(obj, "vkCmdWriteTimestamp2");
    table->CmdWriteTimestamp2KHR = (PFN_vkCmdWriteTimestamp2KHR) gpa(obj, "vkCmdWriteTimestamp2KHR");
    if (table->CmdWriteTimestamp2 && !table->CmdWriteTimestamp2KHR)
       table->CmdWriteTimestamp2KHR = (PFN_vkCmdWriteTimestamp2KHR) table->CmdWriteTimestamp2;
    if (!table->CmdWriteTimestamp2)
       table->CmdWriteTimestamp2 = (PFN_vkCmdWriteTimestamp2) table->CmdWriteTimestamp2KHR;
        table->CmdWriteBufferMarker2AMD = (PFN_vkCmdWriteBufferMarker2AMD) gpa(obj, "vkCmdWriteBufferMarker2AMD");
    table->GetQueueCheckpointData2NV = (PFN_vkGetQueueCheckpointData2NV) gpa(obj, "vkGetQueueCheckpointData2NV");
    table->CopyMemoryToImage = (PFN_vkCopyMemoryToImage) gpa(obj, "vkCopyMemoryToImage");
    table->CopyMemoryToImageEXT = (PFN_vkCopyMemoryToImageEXT) gpa(obj, "vkCopyMemoryToImageEXT");
    if (table->CopyMemoryToImage && !table->CopyMemoryToImageEXT)
       table->CopyMemoryToImageEXT = (PFN_vkCopyMemoryToImageEXT) table->CopyMemoryToImage;
    if (!table->CopyMemoryToImage)
       table->CopyMemoryToImage = (PFN_vkCopyMemoryToImage) table->CopyMemoryToImageEXT;
        table->CopyImageToMemory = (PFN_vkCopyImageToMemory) gpa(obj, "vkCopyImageToMemory");
    table->CopyImageToMemoryEXT = (PFN_vkCopyImageToMemoryEXT) gpa(obj, "vkCopyImageToMemoryEXT");
    if (table->CopyImageToMemory && !table->CopyImageToMemoryEXT)
       table->CopyImageToMemoryEXT = (PFN_vkCopyImageToMemoryEXT) table->CopyImageToMemory;
    if (!table->CopyImageToMemory)
       table->CopyImageToMemory = (PFN_vkCopyImageToMemory) table->CopyImageToMemoryEXT;
        table->CopyImageToImage = (PFN_vkCopyImageToImage) gpa(obj, "vkCopyImageToImage");
    table->CopyImageToImageEXT = (PFN_vkCopyImageToImageEXT) gpa(obj, "vkCopyImageToImageEXT");
    if (table->CopyImageToImage && !table->CopyImageToImageEXT)
       table->CopyImageToImageEXT = (PFN_vkCopyImageToImageEXT) table->CopyImageToImage;
    if (!table->CopyImageToImage)
       table->CopyImageToImage = (PFN_vkCopyImageToImage) table->CopyImageToImageEXT;
        table->TransitionImageLayout = (PFN_vkTransitionImageLayout) gpa(obj, "vkTransitionImageLayout");
    table->TransitionImageLayoutEXT = (PFN_vkTransitionImageLayoutEXT) gpa(obj, "vkTransitionImageLayoutEXT");
    if (table->TransitionImageLayout && !table->TransitionImageLayoutEXT)
       table->TransitionImageLayoutEXT = (PFN_vkTransitionImageLayoutEXT) table->TransitionImageLayout;
    if (!table->TransitionImageLayout)
       table->TransitionImageLayout = (PFN_vkTransitionImageLayout) table->TransitionImageLayoutEXT;
        table->CreateVideoSessionKHR = (PFN_vkCreateVideoSessionKHR) gpa(obj, "vkCreateVideoSessionKHR");
    table->DestroyVideoSessionKHR = (PFN_vkDestroyVideoSessionKHR) gpa(obj, "vkDestroyVideoSessionKHR");
    table->CreateVideoSessionParametersKHR = (PFN_vkCreateVideoSessionParametersKHR) gpa(obj, "vkCreateVideoSessionParametersKHR");
    table->UpdateVideoSessionParametersKHR = (PFN_vkUpdateVideoSessionParametersKHR) gpa(obj, "vkUpdateVideoSessionParametersKHR");
    table->GetEncodedVideoSessionParametersKHR = (PFN_vkGetEncodedVideoSessionParametersKHR) gpa(obj, "vkGetEncodedVideoSessionParametersKHR");
    table->DestroyVideoSessionParametersKHR = (PFN_vkDestroyVideoSessionParametersKHR) gpa(obj, "vkDestroyVideoSessionParametersKHR");
    table->GetVideoSessionMemoryRequirementsKHR = (PFN_vkGetVideoSessionMemoryRequirementsKHR) gpa(obj, "vkGetVideoSessionMemoryRequirementsKHR");
    table->BindVideoSessionMemoryKHR = (PFN_vkBindVideoSessionMemoryKHR) gpa(obj, "vkBindVideoSessionMemoryKHR");
    table->CmdDecodeVideoKHR = (PFN_vkCmdDecodeVideoKHR) gpa(obj, "vkCmdDecodeVideoKHR");
    table->CmdBeginVideoCodingKHR = (PFN_vkCmdBeginVideoCodingKHR) gpa(obj, "vkCmdBeginVideoCodingKHR");
    table->CmdControlVideoCodingKHR = (PFN_vkCmdControlVideoCodingKHR) gpa(obj, "vkCmdControlVideoCodingKHR");
    table->CmdEndVideoCodingKHR = (PFN_vkCmdEndVideoCodingKHR) gpa(obj, "vkCmdEndVideoCodingKHR");
    table->CmdEncodeVideoKHR = (PFN_vkCmdEncodeVideoKHR) gpa(obj, "vkCmdEncodeVideoKHR");
    table->CmdDecompressMemoryNV = (PFN_vkCmdDecompressMemoryNV) gpa(obj, "vkCmdDecompressMemoryNV");
    table->CmdDecompressMemoryIndirectCountNV = (PFN_vkCmdDecompressMemoryIndirectCountNV) gpa(obj, "vkCmdDecompressMemoryIndirectCountNV");
    table->GetPartitionedAccelerationStructuresBuildSizesNV = (PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV) gpa(obj, "vkGetPartitionedAccelerationStructuresBuildSizesNV");
    table->CmdBuildPartitionedAccelerationStructuresNV = (PFN_vkCmdBuildPartitionedAccelerationStructuresNV) gpa(obj, "vkCmdBuildPartitionedAccelerationStructuresNV");
    table->CmdDecompressMemoryEXT = (PFN_vkCmdDecompressMemoryEXT) gpa(obj, "vkCmdDecompressMemoryEXT");
    table->CmdDecompressMemoryIndirectCountEXT = (PFN_vkCmdDecompressMemoryIndirectCountEXT) gpa(obj, "vkCmdDecompressMemoryIndirectCountEXT");
    table->CreateCuModuleNVX = (PFN_vkCreateCuModuleNVX) gpa(obj, "vkCreateCuModuleNVX");
    table->CreateCuFunctionNVX = (PFN_vkCreateCuFunctionNVX) gpa(obj, "vkCreateCuFunctionNVX");
    table->DestroyCuModuleNVX = (PFN_vkDestroyCuModuleNVX) gpa(obj, "vkDestroyCuModuleNVX");
    table->DestroyCuFunctionNVX = (PFN_vkDestroyCuFunctionNVX) gpa(obj, "vkDestroyCuFunctionNVX");
    table->CmdCuLaunchKernelNVX = (PFN_vkCmdCuLaunchKernelNVX) gpa(obj, "vkCmdCuLaunchKernelNVX");
    table->GetDescriptorSetLayoutSizeEXT = (PFN_vkGetDescriptorSetLayoutSizeEXT) gpa(obj, "vkGetDescriptorSetLayoutSizeEXT");
    table->GetDescriptorSetLayoutBindingOffsetEXT = (PFN_vkGetDescriptorSetLayoutBindingOffsetEXT) gpa(obj, "vkGetDescriptorSetLayoutBindingOffsetEXT");
    table->GetDescriptorEXT = (PFN_vkGetDescriptorEXT) gpa(obj, "vkGetDescriptorEXT");
    table->CmdBindDescriptorBuffersEXT = (PFN_vkCmdBindDescriptorBuffersEXT) gpa(obj, "vkCmdBindDescriptorBuffersEXT");
    table->CmdSetDescriptorBufferOffsetsEXT = (PFN_vkCmdSetDescriptorBufferOffsetsEXT) gpa(obj, "vkCmdSetDescriptorBufferOffsetsEXT");
    table->CmdBindDescriptorBufferEmbeddedSamplersEXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT) gpa(obj, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
    table->GetBufferOpaqueCaptureDescriptorDataEXT = (PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetBufferOpaqueCaptureDescriptorDataEXT");
    table->GetImageOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetImageOpaqueCaptureDescriptorDataEXT");
    table->GetImageViewOpaqueCaptureDescriptorDataEXT = (PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetImageViewOpaqueCaptureDescriptorDataEXT");
    table->GetSamplerOpaqueCaptureDescriptorDataEXT = (PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetSamplerOpaqueCaptureDescriptorDataEXT");
    table->GetAccelerationStructureOpaqueCaptureDescriptorDataEXT = (PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT) gpa(obj, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT");
    table->SetDeviceMemoryPriorityEXT = (PFN_vkSetDeviceMemoryPriorityEXT) gpa(obj, "vkSetDeviceMemoryPriorityEXT");
    table->WaitForPresent2KHR = (PFN_vkWaitForPresent2KHR) gpa(obj, "vkWaitForPresent2KHR");
    table->WaitForPresentKHR = (PFN_vkWaitForPresentKHR) gpa(obj, "vkWaitForPresentKHR");
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->CreateBufferCollectionFUCHSIA = (PFN_vkCreateBufferCollectionFUCHSIA) gpa(obj, "vkCreateBufferCollectionFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->SetBufferCollectionBufferConstraintsFUCHSIA = (PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA) gpa(obj, "vkSetBufferCollectionBufferConstraintsFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->SetBufferCollectionImageConstraintsFUCHSIA = (PFN_vkSetBufferCollectionImageConstraintsFUCHSIA) gpa(obj, "vkSetBufferCollectionImageConstraintsFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->DestroyBufferCollectionFUCHSIA = (PFN_vkDestroyBufferCollectionFUCHSIA) gpa(obj, "vkDestroyBufferCollectionFUCHSIA");
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    table->GetBufferCollectionPropertiesFUCHSIA = (PFN_vkGetBufferCollectionPropertiesFUCHSIA) gpa(obj, "vkGetBufferCollectionPropertiesFUCHSIA");
#endif
    table->CmdBeginRendering = (PFN_vkCmdBeginRendering) gpa(obj, "vkCmdBeginRendering");
    table->CmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) gpa(obj, "vkCmdBeginRenderingKHR");
    if (table->CmdBeginRendering && !table->CmdBeginRenderingKHR)
       table->CmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) table->CmdBeginRendering;
    if (!table->CmdBeginRendering)
       table->CmdBeginRendering = (PFN_vkCmdBeginRendering) table->CmdBeginRenderingKHR;
        table->CmdEndRendering = (PFN_vkCmdEndRendering) gpa(obj, "vkCmdEndRendering");
    table->CmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) gpa(obj, "vkCmdEndRenderingKHR");
    if (table->CmdEndRendering && !table->CmdEndRenderingKHR)
       table->CmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) table->CmdEndRendering;
    if (!table->CmdEndRendering)
       table->CmdEndRendering = (PFN_vkCmdEndRendering) table->CmdEndRenderingKHR;
    table->CmdEndRendering2KHR = (PFN_vkCmdEndRendering2KHR) gpa(obj, "vkCmdEndRendering2KHR");
    table->CmdEndRendering2EXT = (PFN_vkCmdEndRendering2EXT) gpa(obj, "vkCmdEndRendering2EXT");
    if (table->CmdEndRendering2KHR && !table->CmdEndRendering2EXT)
       table->CmdEndRendering2EXT = (PFN_vkCmdEndRendering2EXT) table->CmdEndRendering2KHR;
    if (!table->CmdEndRendering2KHR)
       table->CmdEndRendering2KHR = (PFN_vkCmdEndRendering2KHR) table->CmdEndRendering2EXT;
            table->GetDescriptorSetLayoutHostMappingInfoVALVE = (PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE) gpa(obj, "vkGetDescriptorSetLayoutHostMappingInfoVALVE");
    table->GetDescriptorSetHostMappingVALVE = (PFN_vkGetDescriptorSetHostMappingVALVE) gpa(obj, "vkGetDescriptorSetHostMappingVALVE");
    table->CreateMicromapEXT = (PFN_vkCreateMicromapEXT) gpa(obj, "vkCreateMicromapEXT");
    table->CmdBuildMicromapsEXT = (PFN_vkCmdBuildMicromapsEXT) gpa(obj, "vkCmdBuildMicromapsEXT");
    table->BuildMicromapsEXT = (PFN_vkBuildMicromapsEXT) gpa(obj, "vkBuildMicromapsEXT");
    table->DestroyMicromapEXT = (PFN_vkDestroyMicromapEXT) gpa(obj, "vkDestroyMicromapEXT");
    table->CmdCopyMicromapEXT = (PFN_vkCmdCopyMicromapEXT) gpa(obj, "vkCmdCopyMicromapEXT");
    table->CopyMicromapEXT = (PFN_vkCopyMicromapEXT) gpa(obj, "vkCopyMicromapEXT");
    table->CmdCopyMicromapToMemoryEXT = (PFN_vkCmdCopyMicromapToMemoryEXT) gpa(obj, "vkCmdCopyMicromapToMemoryEXT");
    table->CopyMicromapToMemoryEXT = (PFN_vkCopyMicromapToMemoryEXT) gpa(obj, "vkCopyMicromapToMemoryEXT");
    table->CmdCopyMemoryToMicromapEXT = (PFN_vkCmdCopyMemoryToMicromapEXT) gpa(obj, "vkCmdCopyMemoryToMicromapEXT");
    table->CopyMemoryToMicromapEXT = (PFN_vkCopyMemoryToMicromapEXT) gpa(obj, "vkCopyMemoryToMicromapEXT");
    table->CmdWriteMicromapsPropertiesEXT = (PFN_vkCmdWriteMicromapsPropertiesEXT) gpa(obj, "vkCmdWriteMicromapsPropertiesEXT");
    table->WriteMicromapsPropertiesEXT = (PFN_vkWriteMicromapsPropertiesEXT) gpa(obj, "vkWriteMicromapsPropertiesEXT");
    table->GetDeviceMicromapCompatibilityEXT = (PFN_vkGetDeviceMicromapCompatibilityEXT) gpa(obj, "vkGetDeviceMicromapCompatibilityEXT");
    table->GetMicromapBuildSizesEXT = (PFN_vkGetMicromapBuildSizesEXT) gpa(obj, "vkGetMicromapBuildSizesEXT");
    table->GetShaderModuleIdentifierEXT = (PFN_vkGetShaderModuleIdentifierEXT) gpa(obj, "vkGetShaderModuleIdentifierEXT");
    table->GetShaderModuleCreateInfoIdentifierEXT = (PFN_vkGetShaderModuleCreateInfoIdentifierEXT) gpa(obj, "vkGetShaderModuleCreateInfoIdentifierEXT");
    table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) gpa(obj, "vkGetImageSubresourceLayout2");
    table->GetImageSubresourceLayout2KHR = (PFN_vkGetImageSubresourceLayout2KHR) gpa(obj, "vkGetImageSubresourceLayout2KHR");
    if (table->GetImageSubresourceLayout2 && !table->GetImageSubresourceLayout2KHR)
       table->GetImageSubresourceLayout2KHR = (PFN_vkGetImageSubresourceLayout2KHR) table->GetImageSubresourceLayout2;
    if (!table->GetImageSubresourceLayout2)
       table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) table->GetImageSubresourceLayout2KHR;
    table->GetImageSubresourceLayout2EXT = (PFN_vkGetImageSubresourceLayout2EXT) gpa(obj, "vkGetImageSubresourceLayout2EXT");
    if (table->GetImageSubresourceLayout2 && !table->GetImageSubresourceLayout2EXT)
       table->GetImageSubresourceLayout2EXT = (PFN_vkGetImageSubresourceLayout2EXT) table->GetImageSubresourceLayout2;
    if (!table->GetImageSubresourceLayout2)
       table->GetImageSubresourceLayout2 = (PFN_vkGetImageSubresourceLayout2) table->GetImageSubresourceLayout2EXT;
            table->GetPipelinePropertiesEXT = (PFN_vkGetPipelinePropertiesEXT) gpa(obj, "vkGetPipelinePropertiesEXT");
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->ExportMetalObjectsEXT = (PFN_vkExportMetalObjectsEXT) gpa(obj, "vkExportMetalObjectsEXT");
#endif
    table->CmdBindTileMemoryQCOM = (PFN_vkCmdBindTileMemoryQCOM) gpa(obj, "vkCmdBindTileMemoryQCOM");
    table->GetFramebufferTilePropertiesQCOM = (PFN_vkGetFramebufferTilePropertiesQCOM) gpa(obj, "vkGetFramebufferTilePropertiesQCOM");
    table->GetDynamicRenderingTilePropertiesQCOM = (PFN_vkGetDynamicRenderingTilePropertiesQCOM) gpa(obj, "vkGetDynamicRenderingTilePropertiesQCOM");
    table->CreateOpticalFlowSessionNV = (PFN_vkCreateOpticalFlowSessionNV) gpa(obj, "vkCreateOpticalFlowSessionNV");
    table->DestroyOpticalFlowSessionNV = (PFN_vkDestroyOpticalFlowSessionNV) gpa(obj, "vkDestroyOpticalFlowSessionNV");
    table->BindOpticalFlowSessionImageNV = (PFN_vkBindOpticalFlowSessionImageNV) gpa(obj, "vkBindOpticalFlowSessionImageNV");
    table->CmdOpticalFlowExecuteNV = (PFN_vkCmdOpticalFlowExecuteNV) gpa(obj, "vkCmdOpticalFlowExecuteNV");
    table->GetDeviceFaultInfoEXT = (PFN_vkGetDeviceFaultInfoEXT) gpa(obj, "vkGetDeviceFaultInfoEXT");
    table->GetDeviceFaultReportsKHR = (PFN_vkGetDeviceFaultReportsKHR) gpa(obj, "vkGetDeviceFaultReportsKHR");
    table->GetDeviceFaultDebugInfoKHR = (PFN_vkGetDeviceFaultDebugInfoKHR) gpa(obj, "vkGetDeviceFaultDebugInfoKHR");
    table->CmdSetDepthBias2EXT = (PFN_vkCmdSetDepthBias2EXT) gpa(obj, "vkCmdSetDepthBias2EXT");
    table->ReleaseSwapchainImagesKHR = (PFN_vkReleaseSwapchainImagesKHR) gpa(obj, "vkReleaseSwapchainImagesKHR");
    table->ReleaseSwapchainImagesEXT = (PFN_vkReleaseSwapchainImagesEXT) gpa(obj, "vkReleaseSwapchainImagesEXT");
    if (table->ReleaseSwapchainImagesKHR && !table->ReleaseSwapchainImagesEXT)
       table->ReleaseSwapchainImagesEXT = (PFN_vkReleaseSwapchainImagesEXT) table->ReleaseSwapchainImagesKHR;
    if (!table->ReleaseSwapchainImagesKHR)
       table->ReleaseSwapchainImagesKHR = (PFN_vkReleaseSwapchainImagesKHR) table->ReleaseSwapchainImagesEXT;
        table->GetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout) gpa(obj, "vkGetDeviceImageSubresourceLayout");
    table->GetDeviceImageSubresourceLayoutKHR = (PFN_vkGetDeviceImageSubresourceLayoutKHR) gpa(obj, "vkGetDeviceImageSubresourceLayoutKHR");
    if (table->GetDeviceImageSubresourceLayout && !table->GetDeviceImageSubresourceLayoutKHR)
       table->GetDeviceImageSubresourceLayoutKHR = (PFN_vkGetDeviceImageSubresourceLayoutKHR) table->GetDeviceImageSubresourceLayout;
    if (!table->GetDeviceImageSubresourceLayout)
       table->GetDeviceImageSubresourceLayout = (PFN_vkGetDeviceImageSubresourceLayout) table->GetDeviceImageSubresourceLayoutKHR;
        table->MapMemory2 = (PFN_vkMapMemory2) gpa(obj, "vkMapMemory2");
    table->MapMemory2KHR = (PFN_vkMapMemory2KHR) gpa(obj, "vkMapMemory2KHR");
    if (table->MapMemory2 && !table->MapMemory2KHR)
       table->MapMemory2KHR = (PFN_vkMapMemory2KHR) table->MapMemory2;
    if (!table->MapMemory2)
       table->MapMemory2 = (PFN_vkMapMemory2) table->MapMemory2KHR;
        table->UnmapMemory2 = (PFN_vkUnmapMemory2) gpa(obj, "vkUnmapMemory2");
    table->UnmapMemory2KHR = (PFN_vkUnmapMemory2KHR) gpa(obj, "vkUnmapMemory2KHR");
    if (table->UnmapMemory2 && !table->UnmapMemory2KHR)
       table->UnmapMemory2KHR = (PFN_vkUnmapMemory2KHR) table->UnmapMemory2;
    if (!table->UnmapMemory2)
       table->UnmapMemory2 = (PFN_vkUnmapMemory2) table->UnmapMemory2KHR;
        table->CreateShadersEXT = (PFN_vkCreateShadersEXT) gpa(obj, "vkCreateShadersEXT");
    table->DestroyShaderEXT = (PFN_vkDestroyShaderEXT) gpa(obj, "vkDestroyShaderEXT");
    table->GetShaderBinaryDataEXT = (PFN_vkGetShaderBinaryDataEXT) gpa(obj, "vkGetShaderBinaryDataEXT");
    table->CmdBindShadersEXT = (PFN_vkCmdBindShadersEXT) gpa(obj, "vkCmdBindShadersEXT");
    table->SetSwapchainPresentTimingQueueSizeEXT = (PFN_vkSetSwapchainPresentTimingQueueSizeEXT) gpa(obj, "vkSetSwapchainPresentTimingQueueSizeEXT");
    table->GetSwapchainTimingPropertiesEXT = (PFN_vkGetSwapchainTimingPropertiesEXT) gpa(obj, "vkGetSwapchainTimingPropertiesEXT");
    table->GetSwapchainTimeDomainPropertiesEXT = (PFN_vkGetSwapchainTimeDomainPropertiesEXT) gpa(obj, "vkGetSwapchainTimeDomainPropertiesEXT");
    table->GetPastPresentationTimingEXT = (PFN_vkGetPastPresentationTimingEXT) gpa(obj, "vkGetPastPresentationTimingEXT");
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    table->GetScreenBufferPropertiesQNX = (PFN_vkGetScreenBufferPropertiesQNX) gpa(obj, "vkGetScreenBufferPropertiesQNX");
#endif
    table->CreateGpaSessionAMD = (PFN_vkCreateGpaSessionAMD) gpa(obj, "vkCreateGpaSessionAMD");
    table->DestroyGpaSessionAMD = (PFN_vkDestroyGpaSessionAMD) gpa(obj, "vkDestroyGpaSessionAMD");
    table->SetGpaDeviceClockModeAMD = (PFN_vkSetGpaDeviceClockModeAMD) gpa(obj, "vkSetGpaDeviceClockModeAMD");
    table->GetGpaDeviceClockInfoAMD = (PFN_vkGetGpaDeviceClockInfoAMD) gpa(obj, "vkGetGpaDeviceClockInfoAMD");
    table->CmdBeginGpaSessionAMD = (PFN_vkCmdBeginGpaSessionAMD) gpa(obj, "vkCmdBeginGpaSessionAMD");
    table->CmdEndGpaSessionAMD = (PFN_vkCmdEndGpaSessionAMD) gpa(obj, "vkCmdEndGpaSessionAMD");
    table->CmdBeginGpaSampleAMD = (PFN_vkCmdBeginGpaSampleAMD) gpa(obj, "vkCmdBeginGpaSampleAMD");
    table->CmdEndGpaSampleAMD = (PFN_vkCmdEndGpaSampleAMD) gpa(obj, "vkCmdEndGpaSampleAMD");
    table->GetGpaSessionStatusAMD = (PFN_vkGetGpaSessionStatusAMD) gpa(obj, "vkGetGpaSessionStatusAMD");
    table->GetGpaSessionResultsAMD = (PFN_vkGetGpaSessionResultsAMD) gpa(obj, "vkGetGpaSessionResultsAMD");
    table->ResetGpaSessionAMD = (PFN_vkResetGpaSessionAMD) gpa(obj, "vkResetGpaSessionAMD");
    table->CmdCopyGpaSessionResultsAMD = (PFN_vkCmdCopyGpaSessionResultsAMD) gpa(obj, "vkCmdCopyGpaSessionResultsAMD");
    table->CmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2) gpa(obj, "vkCmdBindDescriptorSets2");
    table->CmdBindDescriptorSets2KHR = (PFN_vkCmdBindDescriptorSets2KHR) gpa(obj, "vkCmdBindDescriptorSets2KHR");
    if (table->CmdBindDescriptorSets2 && !table->CmdBindDescriptorSets2KHR)
       table->CmdBindDescriptorSets2KHR = (PFN_vkCmdBindDescriptorSets2KHR) table->CmdBindDescriptorSets2;
    if (!table->CmdBindDescriptorSets2)
       table->CmdBindDescriptorSets2 = (PFN_vkCmdBindDescriptorSets2) table->CmdBindDescriptorSets2KHR;
        table->CmdPushConstants2 = (PFN_vkCmdPushConstants2) gpa(obj, "vkCmdPushConstants2");
    table->CmdPushConstants2KHR = (PFN_vkCmdPushConstants2KHR) gpa(obj, "vkCmdPushConstants2KHR");
    if (table->CmdPushConstants2 && !table->CmdPushConstants2KHR)
       table->CmdPushConstants2KHR = (PFN_vkCmdPushConstants2KHR) table->CmdPushConstants2;
    if (!table->CmdPushConstants2)
       table->CmdPushConstants2 = (PFN_vkCmdPushConstants2) table->CmdPushConstants2KHR;
        table->CmdPushDescriptorSet2 = (PFN_vkCmdPushDescriptorSet2) gpa(obj, "vkCmdPushDescriptorSet2");
    table->CmdPushDescriptorSet2KHR = (PFN_vkCmdPushDescriptorSet2KHR) gpa(obj, "vkCmdPushDescriptorSet2KHR");
    if (table->CmdPushDescriptorSet2 && !table->CmdPushDescriptorSet2KHR)
       table->CmdPushDescriptorSet2KHR = (PFN_vkCmdPushDescriptorSet2KHR) table->CmdPushDescriptorSet2;
    if (!table->CmdPushDescriptorSet2)
       table->CmdPushDescriptorSet2 = (PFN_vkCmdPushDescriptorSet2) table->CmdPushDescriptorSet2KHR;
        table->CmdPushDescriptorSetWithTemplate2 = (PFN_vkCmdPushDescriptorSetWithTemplate2) gpa(obj, "vkCmdPushDescriptorSetWithTemplate2");
    table->CmdPushDescriptorSetWithTemplate2KHR = (PFN_vkCmdPushDescriptorSetWithTemplate2KHR) gpa(obj, "vkCmdPushDescriptorSetWithTemplate2KHR");
    if (table->CmdPushDescriptorSetWithTemplate2 && !table->CmdPushDescriptorSetWithTemplate2KHR)
       table->CmdPushDescriptorSetWithTemplate2KHR = (PFN_vkCmdPushDescriptorSetWithTemplate2KHR) table->CmdPushDescriptorSetWithTemplate2;
    if (!table->CmdPushDescriptorSetWithTemplate2)
       table->CmdPushDescriptorSetWithTemplate2 = (PFN_vkCmdPushDescriptorSetWithTemplate2) table->CmdPushDescriptorSetWithTemplate2KHR;
        table->CmdSetDescriptorBufferOffsets2EXT = (PFN_vkCmdSetDescriptorBufferOffsets2EXT) gpa(obj, "vkCmdSetDescriptorBufferOffsets2EXT");
    table->CmdBindDescriptorBufferEmbeddedSamplers2EXT = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT) gpa(obj, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT");
    table->SetLatencySleepModeNV = (PFN_vkSetLatencySleepModeNV) gpa(obj, "vkSetLatencySleepModeNV");
    table->LatencySleepNV = (PFN_vkLatencySleepNV) gpa(obj, "vkLatencySleepNV");
    table->SetLatencyMarkerNV = (PFN_vkSetLatencyMarkerNV) gpa(obj, "vkSetLatencyMarkerNV");
    table->GetLatencyTimingsNV = (PFN_vkGetLatencyTimingsNV) gpa(obj, "vkGetLatencyTimingsNV");
    table->QueueNotifyOutOfBandNV = (PFN_vkQueueNotifyOutOfBandNV) gpa(obj, "vkQueueNotifyOutOfBandNV");
    table->CmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations) gpa(obj, "vkCmdSetRenderingAttachmentLocations");
    table->CmdSetRenderingAttachmentLocationsKHR = (PFN_vkCmdSetRenderingAttachmentLocationsKHR) gpa(obj, "vkCmdSetRenderingAttachmentLocationsKHR");
    if (table->CmdSetRenderingAttachmentLocations && !table->CmdSetRenderingAttachmentLocationsKHR)
       table->CmdSetRenderingAttachmentLocationsKHR = (PFN_vkCmdSetRenderingAttachmentLocationsKHR) table->CmdSetRenderingAttachmentLocations;
    if (!table->CmdSetRenderingAttachmentLocations)
       table->CmdSetRenderingAttachmentLocations = (PFN_vkCmdSetRenderingAttachmentLocations) table->CmdSetRenderingAttachmentLocationsKHR;
        table->CmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices) gpa(obj, "vkCmdSetRenderingInputAttachmentIndices");
    table->CmdSetRenderingInputAttachmentIndicesKHR = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR) gpa(obj, "vkCmdSetRenderingInputAttachmentIndicesKHR");
    if (table->CmdSetRenderingInputAttachmentIndices && !table->CmdSetRenderingInputAttachmentIndicesKHR)
       table->CmdSetRenderingInputAttachmentIndicesKHR = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR) table->CmdSetRenderingInputAttachmentIndices;
    if (!table->CmdSetRenderingInputAttachmentIndices)
       table->CmdSetRenderingInputAttachmentIndices = (PFN_vkCmdSetRenderingInputAttachmentIndices) table->CmdSetRenderingInputAttachmentIndicesKHR;
        table->CmdSetDepthClampRangeEXT = (PFN_vkCmdSetDepthClampRangeEXT) gpa(obj, "vkCmdSetDepthClampRangeEXT");
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->GetMemoryMetalHandleEXT = (PFN_vkGetMemoryMetalHandleEXT) gpa(obj, "vkGetMemoryMetalHandleEXT");
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
    table->GetMemoryMetalHandlePropertiesEXT = (PFN_vkGetMemoryMetalHandlePropertiesEXT) gpa(obj, "vkGetMemoryMetalHandlePropertiesEXT");
#endif
    table->ConvertCooperativeVectorMatrixNV = (PFN_vkConvertCooperativeVectorMatrixNV) gpa(obj, "vkConvertCooperativeVectorMatrixNV");
    table->CmdConvertCooperativeVectorMatrixNV = (PFN_vkCmdConvertCooperativeVectorMatrixNV) gpa(obj, "vkCmdConvertCooperativeVectorMatrixNV");
    table->CmdDispatchTileQCOM = (PFN_vkCmdDispatchTileQCOM) gpa(obj, "vkCmdDispatchTileQCOM");
    table->CmdBeginPerTileExecutionQCOM = (PFN_vkCmdBeginPerTileExecutionQCOM) gpa(obj, "vkCmdBeginPerTileExecutionQCOM");
    table->CmdEndPerTileExecutionQCOM = (PFN_vkCmdEndPerTileExecutionQCOM) gpa(obj, "vkCmdEndPerTileExecutionQCOM");
    table->CreateExternalComputeQueueNV = (PFN_vkCreateExternalComputeQueueNV) gpa(obj, "vkCreateExternalComputeQueueNV");
    table->DestroyExternalComputeQueueNV = (PFN_vkDestroyExternalComputeQueueNV) gpa(obj, "vkDestroyExternalComputeQueueNV");
    table->CreateShaderInstrumentationARM = (PFN_vkCreateShaderInstrumentationARM) gpa(obj, "vkCreateShaderInstrumentationARM");
    table->DestroyShaderInstrumentationARM = (PFN_vkDestroyShaderInstrumentationARM) gpa(obj, "vkDestroyShaderInstrumentationARM");
    table->CmdBeginShaderInstrumentationARM = (PFN_vkCmdBeginShaderInstrumentationARM) gpa(obj, "vkCmdBeginShaderInstrumentationARM");
    table->CmdEndShaderInstrumentationARM = (PFN_vkCmdEndShaderInstrumentationARM) gpa(obj, "vkCmdEndShaderInstrumentationARM");
    table->GetShaderInstrumentationValuesARM = (PFN_vkGetShaderInstrumentationValuesARM) gpa(obj, "vkGetShaderInstrumentationValuesARM");
    table->ClearShaderInstrumentationMetricsARM = (PFN_vkClearShaderInstrumentationMetricsARM) gpa(obj, "vkClearShaderInstrumentationMetricsARM");
    table->CreateTensorARM = (PFN_vkCreateTensorARM) gpa(obj, "vkCreateTensorARM");
    table->DestroyTensorARM = (PFN_vkDestroyTensorARM) gpa(obj, "vkDestroyTensorARM");
    table->CreateTensorViewARM = (PFN_vkCreateTensorViewARM) gpa(obj, "vkCreateTensorViewARM");
    table->DestroyTensorViewARM = (PFN_vkDestroyTensorViewARM) gpa(obj, "vkDestroyTensorViewARM");
    table->GetTensorMemoryRequirementsARM = (PFN_vkGetTensorMemoryRequirementsARM) gpa(obj, "vkGetTensorMemoryRequirementsARM");
    table->BindTensorMemoryARM = (PFN_vkBindTensorMemoryARM) gpa(obj, "vkBindTensorMemoryARM");
    table->GetDeviceTensorMemoryRequirementsARM = (PFN_vkGetDeviceTensorMemoryRequirementsARM) gpa(obj, "vkGetDeviceTensorMemoryRequirementsARM");
    table->CmdCopyTensorARM = (PFN_vkCmdCopyTensorARM) gpa(obj, "vkCmdCopyTensorARM");
    table->GetTensorOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorOpaqueCaptureDescriptorDataARM) gpa(obj, "vkGetTensorOpaqueCaptureDescriptorDataARM");
    table->GetTensorViewOpaqueCaptureDescriptorDataARM = (PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM) gpa(obj, "vkGetTensorViewOpaqueCaptureDescriptorDataARM");
    table->CreateDataGraphPipelinesARM = (PFN_vkCreateDataGraphPipelinesARM) gpa(obj, "vkCreateDataGraphPipelinesARM");
    table->CreateDataGraphPipelineSessionARM = (PFN_vkCreateDataGraphPipelineSessionARM) gpa(obj, "vkCreateDataGraphPipelineSessionARM");
    table->GetDataGraphPipelineSessionBindPointRequirementsARM = (PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM) gpa(obj, "vkGetDataGraphPipelineSessionBindPointRequirementsARM");
    table->GetDataGraphPipelineSessionMemoryRequirementsARM = (PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM) gpa(obj, "vkGetDataGraphPipelineSessionMemoryRequirementsARM");
    table->BindDataGraphPipelineSessionMemoryARM = (PFN_vkBindDataGraphPipelineSessionMemoryARM) gpa(obj, "vkBindDataGraphPipelineSessionMemoryARM");
    table->DestroyDataGraphPipelineSessionARM = (PFN_vkDestroyDataGraphPipelineSessionARM) gpa(obj, "vkDestroyDataGraphPipelineSessionARM");
    table->CmdDispatchDataGraphARM = (PFN_vkCmdDispatchDataGraphARM) gpa(obj, "vkCmdDispatchDataGraphARM");
    table->GetDataGraphPipelineAvailablePropertiesARM = (PFN_vkGetDataGraphPipelineAvailablePropertiesARM) gpa(obj, "vkGetDataGraphPipelineAvailablePropertiesARM");
    table->GetDataGraphPipelinePropertiesARM = (PFN_vkGetDataGraphPipelinePropertiesARM) gpa(obj, "vkGetDataGraphPipelinePropertiesARM");
#ifdef VK_USE_PLATFORM_OHOS
    table->GetNativeBufferPropertiesOHOS = (PFN_vkGetNativeBufferPropertiesOHOS) gpa(obj, "vkGetNativeBufferPropertiesOHOS");
#endif
#ifdef VK_USE_PLATFORM_OHOS
    table->GetMemoryNativeBufferOHOS = (PFN_vkGetMemoryNativeBufferOHOS) gpa(obj, "vkGetMemoryNativeBufferOHOS");
#endif
    table->QueueSetPerfHintQCOM = (PFN_vkQueueSetPerfHintQCOM) gpa(obj, "vkQueueSetPerfHintQCOM");
    table->CmdSetComputeOccupancyPriorityNV = (PFN_vkCmdSetComputeOccupancyPriorityNV) gpa(obj, "vkCmdSetComputeOccupancyPriorityNV");
    table->WriteSamplerDescriptorsEXT = (PFN_vkWriteSamplerDescriptorsEXT) gpa(obj, "vkWriteSamplerDescriptorsEXT");
    table->WriteResourceDescriptorsEXT = (PFN_vkWriteResourceDescriptorsEXT) gpa(obj, "vkWriteResourceDescriptorsEXT");
    table->CmdBindSamplerHeapEXT = (PFN_vkCmdBindSamplerHeapEXT) gpa(obj, "vkCmdBindSamplerHeapEXT");
    table->CmdBindResourceHeapEXT = (PFN_vkCmdBindResourceHeapEXT) gpa(obj, "vkCmdBindResourceHeapEXT");
    table->CmdPushDataEXT = (PFN_vkCmdPushDataEXT) gpa(obj, "vkCmdPushDataEXT");
    table->RegisterCustomBorderColorEXT = (PFN_vkRegisterCustomBorderColorEXT) gpa(obj, "vkRegisterCustomBorderColorEXT");
    table->UnregisterCustomBorderColorEXT = (PFN_vkUnregisterCustomBorderColorEXT) gpa(obj, "vkUnregisterCustomBorderColorEXT");
    table->GetImageOpaqueCaptureDataEXT = (PFN_vkGetImageOpaqueCaptureDataEXT) gpa(obj, "vkGetImageOpaqueCaptureDataEXT");
    table->GetTensorOpaqueCaptureDataARM = (PFN_vkGetTensorOpaqueCaptureDataARM) gpa(obj, "vkGetTensorOpaqueCaptureDataARM");
    table->CmdCopyMemoryKHR = (PFN_vkCmdCopyMemoryKHR) gpa(obj, "vkCmdCopyMemoryKHR");
    table->CmdCopyMemoryToImageKHR = (PFN_vkCmdCopyMemoryToImageKHR) gpa(obj, "vkCmdCopyMemoryToImageKHR");
    table->CmdCopyImageToMemoryKHR = (PFN_vkCmdCopyImageToMemoryKHR) gpa(obj, "vkCmdCopyImageToMemoryKHR");
    table->CmdUpdateMemoryKHR = (PFN_vkCmdUpdateMemoryKHR) gpa(obj, "vkCmdUpdateMemoryKHR");
    table->CmdFillMemoryKHR = (PFN_vkCmdFillMemoryKHR) gpa(obj, "vkCmdFillMemoryKHR");
    table->CmdCopyQueryPoolResultsToMemoryKHR = (PFN_vkCmdCopyQueryPoolResultsToMemoryKHR) gpa(obj, "vkCmdCopyQueryPoolResultsToMemoryKHR");
    table->CmdBeginConditionalRendering2EXT = (PFN_vkCmdBeginConditionalRendering2EXT) gpa(obj, "vkCmdBeginConditionalRendering2EXT");
    table->CmdBindTransformFeedbackBuffers2EXT = (PFN_vkCmdBindTransformFeedbackBuffers2EXT) gpa(obj, "vkCmdBindTransformFeedbackBuffers2EXT");
    table->CmdBeginTransformFeedback2EXT = (PFN_vkCmdBeginTransformFeedback2EXT) gpa(obj, "vkCmdBeginTransformFeedback2EXT");
    table->CmdEndTransformFeedback2EXT = (PFN_vkCmdEndTransformFeedback2EXT) gpa(obj, "vkCmdEndTransformFeedback2EXT");
    table->CmdDrawIndirectByteCount2EXT = (PFN_vkCmdDrawIndirectByteCount2EXT) gpa(obj, "vkCmdDrawIndirectByteCount2EXT");
    table->CmdWriteMarkerToMemoryAMD = (PFN_vkCmdWriteMarkerToMemoryAMD) gpa(obj, "vkCmdWriteMarkerToMemoryAMD");
    table->CmdBindIndexBuffer3KHR = (PFN_vkCmdBindIndexBuffer3KHR) gpa(obj, "vkCmdBindIndexBuffer3KHR");
    table->CmdBindVertexBuffers3KHR = (PFN_vkCmdBindVertexBuffers3KHR) gpa(obj, "vkCmdBindVertexBuffers3KHR");
    table->CmdDrawIndirect2KHR = (PFN_vkCmdDrawIndirect2KHR) gpa(obj, "vkCmdDrawIndirect2KHR");
    table->CmdDrawIndexedIndirect2KHR = (PFN_vkCmdDrawIndexedIndirect2KHR) gpa(obj, "vkCmdDrawIndexedIndirect2KHR");
    table->CmdDrawIndirectCount2KHR = (PFN_vkCmdDrawIndirectCount2KHR) gpa(obj, "vkCmdDrawIndirectCount2KHR");
    table->CmdDrawIndexedIndirectCount2KHR = (PFN_vkCmdDrawIndexedIndirectCount2KHR) gpa(obj, "vkCmdDrawIndexedIndirectCount2KHR");
    table->CmdDrawMeshTasksIndirect2EXT = (PFN_vkCmdDrawMeshTasksIndirect2EXT) gpa(obj, "vkCmdDrawMeshTasksIndirect2EXT");
    table->CmdDrawMeshTasksIndirectCount2EXT = (PFN_vkCmdDrawMeshTasksIndirectCount2EXT) gpa(obj, "vkCmdDrawMeshTasksIndirectCount2EXT");
    table->CmdDispatchIndirect2KHR = (PFN_vkCmdDispatchIndirect2KHR) gpa(obj, "vkCmdDispatchIndirect2KHR");
    table->CreateAccelerationStructure2KHR = (PFN_vkCreateAccelerationStructure2KHR) gpa(obj, "vkCreateAccelerationStructure2KHR");
    table->CmdSetDispatchParametersARM = (PFN_vkCmdSetDispatchParametersARM) gpa(obj, "vkCmdSetDispatchParametersARM");
}



struct string_map_entry {
   uint32_t name;
   uint32_t hash;
   uint32_t num;
};

/* We use a big string constant to avoid lots of reloctions from the entry
 * point table to lots of little strings. The entries in the entry point table
 * store the index into this big string.
 */




static const char instance_strings[] =
    "vkCreateAndroidSurfaceKHR\0"
    "vkCreateDebugReportCallbackEXT\0"
    "vkCreateDebugUtilsMessengerEXT\0"
    "vkCreateDirectFBSurfaceEXT\0"
    "vkCreateDisplayPlaneSurfaceKHR\0"
    "vkCreateHeadlessSurfaceEXT\0"
    "vkCreateIOSSurfaceMVK\0"
    "vkCreateImagePipeSurfaceFUCHSIA\0"
    "vkCreateInstance\0"
    "vkCreateMacOSSurfaceMVK\0"
    "vkCreateMetalSurfaceEXT\0"
    "vkCreatePlan9SurfacePLAN9\0"
    "vkCreateScreenSurfaceQNX\0"
    "vkCreateStreamDescriptorSurfaceGGP\0"
    "vkCreateSurfaceOHOS\0"
    "vkCreateUbmSurfaceSEC\0"
    "vkCreateViSurfaceNN\0"
    "vkCreateWaylandSurfaceKHR\0"
    "vkCreateWin32SurfaceKHR\0"
    "vkCreateXcbSurfaceKHR\0"
    "vkCreateXlibSurfaceKHR\0"
    "vkDebugReportMessageEXT\0"
    "vkDestroyDebugReportCallbackEXT\0"
    "vkDestroyDebugUtilsMessengerEXT\0"
    "vkDestroyInstance\0"
    "vkDestroySurfaceKHR\0"
    "vkEnumerateInstanceExtensionProperties\0"
    "vkEnumerateInstanceLayerProperties\0"
    "vkEnumerateInstanceVersion\0"
    "vkEnumeratePhysicalDeviceGroups\0"
    "vkEnumeratePhysicalDeviceGroupsKHR\0"
    "vkEnumeratePhysicalDevices\0"
    "vkGetExternalComputeQueueDataNV\0"
    "vkGetInstanceProcAddr\0"
    "vkSubmitDebugUtilsMessageEXT\0"
;

static const struct string_map_entry instance_string_map_entries[] = {
    { 0, 0x3667f4e, 7 }, /* vkCreateAndroidSurfaceKHR */
    { 26, 0x987ef56, 21 }, /* vkCreateDebugReportCallbackEXT */
    { 57, 0xb674c135, 29 }, /* vkCreateDebugUtilsMessengerEXT */
    { 88, 0x2aff82da, 17 }, /* vkCreateDirectFBSurfaceEXT */
    { 115, 0x7ac4dacb, 9 }, /* vkCreateDisplayPlaneSurfaceKHR */
    { 146, 0x475d2386, 32 }, /* vkCreateHeadlessSurfaceEXT */
    { 173, 0x12f99669, 26 }, /* vkCreateIOSSurfaceMVK */
    { 195, 0xdee614a2, 18 }, /* vkCreateImagePipeSurfaceFUCHSIA */
    { 227, 0x38a581a6, 0 }, /* vkCreateInstance */
    { 244, 0x30feffa7, 27 }, /* vkCreateMacOSSurfaceMVK */
    { 268, 0xa436224a, 28 }, /* vkCreateMetalSurfaceEXT */
    { 292, 0xf694322, 33 }, /* vkCreatePlan9SurfacePLAN9 */
    { 318, 0x6338a483, 20 }, /* vkCreateScreenSurfaceQNX */
    { 343, 0x14361e93, 19 }, /* vkCreateStreamDescriptorSurfaceGGP */
    { 378, 0x8578791, 8 }, /* vkCreateSurfaceOHOS */
    { 398, 0x9f9e8a4b, 13 }, /* vkCreateUbmSurfaceSEC */
    { 420, 0xba0e6b39, 11 }, /* vkCreateViSurfaceNN */
    { 440, 0x2b2a4b79, 12 }, /* vkCreateWaylandSurfaceKHR */
    { 466, 0xfa2ba1e2, 14 }, /* vkCreateWin32SurfaceKHR */
    { 490, 0xc5e5b106, 16 }, /* vkCreateXcbSurfaceKHR */
    { 512, 0xa693bc66, 15 }, /* vkCreateXlibSurfaceKHR */
    { 535, 0xa4e75334, 23 }, /* vkDebugReportMessageEXT */
    { 559, 0x43d4c4e2, 22 }, /* vkDestroyDebugReportCallbackEXT */
    { 591, 0xf0c196c1, 30 }, /* vkDestroyDebugUtilsMessengerEXT */
    { 623, 0x9bd21af2, 1 }, /* vkDestroyInstance */
    { 641, 0xf204ce7d, 10 }, /* vkDestroySurfaceKHR */
    { 661, 0xeb27627e, 6 }, /* vkEnumerateInstanceExtensionProperties */
    { 700, 0x81f69d8, 5 }, /* vkEnumerateInstanceLayerProperties */
    { 735, 0xd0481e5c, 4 }, /* vkEnumerateInstanceVersion */
    { 762, 0x270514f0, 24 }, /* vkEnumeratePhysicalDeviceGroups */
    { 794, 0x549ce595, 25 }, /* vkEnumeratePhysicalDeviceGroupsKHR */
    { 829, 0x5787c327, 2 }, /* vkEnumeratePhysicalDevices */
    { 856, 0xeee25bb8, 34 }, /* vkGetExternalComputeQueueDataNV */
    { 888, 0x3d2ae9ad, 3 }, /* vkGetInstanceProcAddr */
    { 910, 0x92943f0d, 31 }, /* vkSubmitDebugUtilsMessageEXT */
};

/* Hash table stats:
 * size 35 entries
 * collisions entries:
 *     0      27
 *     1      4
 *     2      1
 *     3      2
 *     4      1
 *     5      0
 *     6      0
 *     7      0
 *     8      0
 *     9+     0
 */

#define none 0xffff
static const uint16_t instance_string_map[64] = {
    none,
    0x0017,
    none,
    0x000c,
    none,
    none,
    0x0005,
    none,
    0x000b,
    none,
    0x000a,
    0x0004,
    0x0011,
    0x0022,
    0x0000,
    none,
    none,
    0x000e,
    none,
    0x000d,
    none,
    0x001e,
    0x0001,
    none,
    0x001b,
    0x0013,
    0x0003,
    0x0012,
    0x001c,
    none,
    0x000f,
    0x0014,
    none,
    none,
    0x0007,
    none,
    none,
    none,
    0x0008,
    0x0009,
    none,
    0x0006,
    none,
    none,
    none,
    0x0021,
    0x0016,
    none,
    0x001d,
    none,
    0x0018,
    none,
    0x0015,
    0x0002,
    none,
    none,
    0x0020,
    0x0010,
    0x001f,
    none,
    none,
    0x0019,
    0x001a,
    none,
};

static int
instance_string_map_lookup(const char *str)
{
    static const uint32_t prime_factor = 5024183;
    static const uint32_t prime_step = 19;
    const struct string_map_entry *e;
    uint32_t hash, h;
    uint16_t i;
    const char *p;

    hash = 0;
    for (p = str; *p; p++)
        hash = hash * prime_factor + *p;

    h = hash;
    while (1) {
        i = instance_string_map[h & 63];
        if (i == none)
           return -1;
        e = &instance_string_map_entries[i];
        if (e->hash == hash && strcmp(str, instance_strings + e->name) == 0)
            return e->num;
        h += prime_step;
    }

    return -1;
}


static const char physical_device_strings[] =
    "vkAcquireDrmDisplayEXT\0"
    "vkAcquireWinrtDisplayNV\0"
    "vkAcquireXlibDisplayEXT\0"
    "vkCreateDevice\0"
    "vkCreateDisplayModeKHR\0"
    "vkEnumerateDeviceExtensionProperties\0"
    "vkEnumerateDeviceLayerProperties\0"
    "vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM\0"
    "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR\0"
    "vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM\0"
    "vkGetDisplayModeProperties2KHR\0"
    "vkGetDisplayModePropertiesKHR\0"
    "vkGetDisplayPlaneCapabilities2KHR\0"
    "vkGetDisplayPlaneCapabilitiesKHR\0"
    "vkGetDisplayPlaneSupportedDisplaysKHR\0"
    "vkGetDrmDisplayEXT\0"
    "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT\0"
    "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR\0"
    "vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV\0"
    "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR\0"
    "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV\0"
    "vkGetPhysicalDeviceCooperativeVectorPropertiesNV\0"
    "vkGetPhysicalDeviceDescriptorSizeEXT\0"
    "vkGetPhysicalDeviceDirectFBPresentationSupportEXT\0"
    "vkGetPhysicalDeviceDisplayPlaneProperties2KHR\0"
    "vkGetPhysicalDeviceDisplayPlanePropertiesKHR\0"
    "vkGetPhysicalDeviceDisplayProperties2KHR\0"
    "vkGetPhysicalDeviceDisplayPropertiesKHR\0"
    "vkGetPhysicalDeviceExternalBufferProperties\0"
    "vkGetPhysicalDeviceExternalBufferPropertiesKHR\0"
    "vkGetPhysicalDeviceExternalFenceProperties\0"
    "vkGetPhysicalDeviceExternalFencePropertiesKHR\0"
    "vkGetPhysicalDeviceExternalImageFormatPropertiesNV\0"
    "vkGetPhysicalDeviceExternalSemaphoreProperties\0"
    "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR\0"
    "vkGetPhysicalDeviceExternalTensorPropertiesARM\0"
    "vkGetPhysicalDeviceFeatures\0"
    "vkGetPhysicalDeviceFeatures2\0"
    "vkGetPhysicalDeviceFeatures2KHR\0"
    "vkGetPhysicalDeviceFormatProperties\0"
    "vkGetPhysicalDeviceFormatProperties2\0"
    "vkGetPhysicalDeviceFormatProperties2KHR\0"
    "vkGetPhysicalDeviceFragmentShadingRatesKHR\0"
    "vkGetPhysicalDeviceImageFormatProperties\0"
    "vkGetPhysicalDeviceImageFormatProperties2\0"
    "vkGetPhysicalDeviceImageFormatProperties2KHR\0"
    "vkGetPhysicalDeviceMemoryProperties\0"
    "vkGetPhysicalDeviceMemoryProperties2\0"
    "vkGetPhysicalDeviceMemoryProperties2KHR\0"
    "vkGetPhysicalDeviceMultisamplePropertiesEXT\0"
    "vkGetPhysicalDeviceOpticalFlowImageFormatsNV\0"
    "vkGetPhysicalDevicePresentRectanglesKHR\0"
    "vkGetPhysicalDeviceProperties\0"
    "vkGetPhysicalDeviceProperties2\0"
    "vkGetPhysicalDeviceProperties2KHR\0"
    "vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM\0"
    "vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM\0"
    "vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM\0"
    "vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM\0"
    "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR\0"
    "vkGetPhysicalDeviceQueueFamilyProperties\0"
    "vkGetPhysicalDeviceQueueFamilyProperties2\0"
    "vkGetPhysicalDeviceQueueFamilyProperties2KHR\0"
    "vkGetPhysicalDeviceScreenPresentationSupportQNX\0"
    "vkGetPhysicalDeviceSparseImageFormatProperties\0"
    "vkGetPhysicalDeviceSparseImageFormatProperties2\0"
    "vkGetPhysicalDeviceSparseImageFormatProperties2KHR\0"
    "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV\0"
    "vkGetPhysicalDeviceSurfaceCapabilities2EXT\0"
    "vkGetPhysicalDeviceSurfaceCapabilities2KHR\0"
    "vkGetPhysicalDeviceSurfaceCapabilitiesKHR\0"
    "vkGetPhysicalDeviceSurfaceFormats2KHR\0"
    "vkGetPhysicalDeviceSurfaceFormatsKHR\0"
    "vkGetPhysicalDeviceSurfacePresentModes2EXT\0"
    "vkGetPhysicalDeviceSurfacePresentModesKHR\0"
    "vkGetPhysicalDeviceSurfaceSupportKHR\0"
    "vkGetPhysicalDeviceToolProperties\0"
    "vkGetPhysicalDeviceToolPropertiesEXT\0"
    "vkGetPhysicalDeviceUbmPresentationSupportSEC\0"
    "vkGetPhysicalDeviceVideoCapabilitiesKHR\0"
    "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR\0"
    "vkGetPhysicalDeviceVideoFormatPropertiesKHR\0"
    "vkGetPhysicalDeviceWaylandPresentationSupportKHR\0"
    "vkGetPhysicalDeviceWin32PresentationSupportKHR\0"
    "vkGetPhysicalDeviceXcbPresentationSupportKHR\0"
    "vkGetPhysicalDeviceXlibPresentationSupportKHR\0"
    "vkGetRandROutputDisplayEXT\0"
    "vkGetWinrtDisplayNV\0"
    "vkReleaseDisplayEXT\0"
;

static const struct string_map_entry physical_device_string_map_entries[] = {
    { 0, 0x837a3e0d, 75 }, /* vkAcquireDrmDisplayEXT */
    { 23, 0xdfb5cfcd, 51 }, /* vkAcquireWinrtDisplayNV */
    { 47, 0x60df100d, 49 }, /* vkAcquireXlibDisplayEXT */
    { 71, 0x85ed23f, 6 }, /* vkCreateDevice */
    { 86, 0xcc0bde41, 14 }, /* vkCreateDisplayModeKHR */
    { 109, 0x5fd13eed, 8 }, /* vkEnumerateDeviceExtensionProperties */
    { 146, 0x2f8566e7, 7 }, /* vkEnumerateDeviceLayerProperties */
    { 179, 0xcf95ba43, 85 }, /* vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM */
    { 246, 0x8d3d4995, 66 }, /* vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR */
    { 310, 0xab6c7e06, 81 }, /* vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM */
    { 367, 0x3e613e42, 60 }, /* vkGetDisplayModeProperties2KHR */
    { 398, 0x36b8a8de, 13 }, /* vkGetDisplayModePropertiesKHR */
    { 428, 0xff1655a4, 61 }, /* vkGetDisplayPlaneCapabilities2KHR */
    { 462, 0x4b60d48c, 15 }, /* vkGetDisplayPlaneCapabilitiesKHR */
    { 495, 0xabef4889, 12 }, /* vkGetDisplayPlaneSupportedDisplaysKHR */
    { 533, 0x35c4e65, 76 }, /* vkGetDrmDisplayEXT */
    { 552, 0xea07da1a, 63 }, /* vkGetPhysicalDeviceCalibrateableTimeDomainsEXT */
    { 599, 0x2440038e, 62 }, /* vkGetPhysicalDeviceCalibrateableTimeDomainsKHR */
    { 646, 0xf5c582d1, 79 }, /* vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV */
    { 713, 0xb948444c, 78 }, /* vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR */
    { 763, 0x3c5ac109, 64 }, /* vkGetPhysicalDeviceCooperativeMatrixPropertiesNV */
    { 812, 0xf3d312b, 80 }, /* vkGetPhysicalDeviceCooperativeVectorPropertiesNV */
    { 861, 0x300bb0db, 86 }, /* vkGetPhysicalDeviceDescriptorSizeEXT */
    { 898, 0x8d00453f, 25 }, /* vkGetPhysicalDeviceDirectFBPresentationSupportEXT */
    { 948, 0xb7bc4386, 59 }, /* vkGetPhysicalDeviceDisplayPlaneProperties2KHR */
    { 994, 0xb9b8ddba, 11 }, /* vkGetPhysicalDeviceDisplayPlanePropertiesKHR */
    { 1039, 0x540c0372, 58 }, /* vkGetPhysicalDeviceDisplayProperties2KHR */
    { 1080, 0xfa0cd2e, 10 }, /* vkGetPhysicalDeviceDisplayPropertiesKHR */
    { 1120, 0x944476dc, 42 }, /* vkGetPhysicalDeviceExternalBufferProperties */
    { 1164, 0xee68b389, 43 }, /* vkGetPhysicalDeviceExternalBufferPropertiesKHR */
    { 1211, 0x3bc965eb, 46 }, /* vkGetPhysicalDeviceExternalFenceProperties */
    { 1254, 0x99b35492, 47 }, /* vkGetPhysicalDeviceExternalFencePropertiesKHR */
    { 1300, 0xc8420c4c, 27 }, /* vkGetPhysicalDeviceExternalImageFormatPropertiesNV */
    { 1351, 0xcf251b0e, 44 }, /* vkGetPhysicalDeviceExternalSemaphoreProperties */
    { 1398, 0x984c3fa7, 45 }, /* vkGetPhysicalDeviceExternalSemaphorePropertiesKHR */
    { 1448, 0x69d48b8f, 82 }, /* vkGetPhysicalDeviceExternalTensorPropertiesARM */
    { 1495, 0x113e2f33, 3 }, /* vkGetPhysicalDeviceFeatures */
    { 1523, 0x63c068a7, 28 }, /* vkGetPhysicalDeviceFeatures2 */
    { 1552, 0x6a9a3636, 29 }, /* vkGetPhysicalDeviceFeatures2KHR */
    { 1584, 0x3e54b398, 4 }, /* vkGetPhysicalDeviceFormatProperties */
    { 1620, 0xca3bb9da, 32 }, /* vkGetPhysicalDeviceFormatProperties2 */
    { 1657, 0x9099cbbb, 33 }, /* vkGetPhysicalDeviceFormatProperties2KHR */
    { 1697, 0x6f0a9ed6, 71 }, /* vkGetPhysicalDeviceFragmentShadingRatesKHR */
    { 1740, 0xdd36a867, 5 }, /* vkGetPhysicalDeviceImageFormatProperties */
    { 1781, 0x35d260d3, 34 }, /* vkGetPhysicalDeviceImageFormatProperties2 */
    { 1823, 0x102ff7ea, 35 }, /* vkGetPhysicalDeviceImageFormatProperties2KHR */
    { 1868, 0xa90da4da, 2 }, /* vkGetPhysicalDeviceMemoryProperties */
    { 1904, 0xcb4cc208, 38 }, /* vkGetPhysicalDeviceMemoryProperties2 */
    { 1941, 0xc8c3da3d, 39 }, /* vkGetPhysicalDeviceMemoryProperties2KHR */
    { 1981, 0x219aa0b9, 55 }, /* vkGetPhysicalDeviceMultisamplePropertiesEXT */
    { 2025, 0x521044d1, 77 }, /* vkGetPhysicalDeviceOpticalFlowImageFormatsNV */
    { 2070, 0x100341b4, 54 }, /* vkGetPhysicalDevicePresentRectanglesKHR */
    { 2110, 0x52fe22c9, 0 }, /* vkGetPhysicalDeviceProperties */
    { 2140, 0x6c4d8ee1, 30 }, /* vkGetPhysicalDeviceProperties2 */
    { 2171, 0xcd15838c, 31 }, /* vkGetPhysicalDeviceProperties2KHR */
    { 2205, 0xcf2b0201, 87 }, /* vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM */
    { 2273, 0x458d064a, 88 }, /* vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM */
    { 2339, 0xac76abef, 84 }, /* vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM */
    { 2408, 0xba8b4b64, 83 }, /* vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM */
    { 2461, 0x7c7c9a0f, 67 }, /* vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR */
    { 2517, 0x4e5fc88a, 1 }, /* vkGetPhysicalDeviceQueueFamilyProperties */
    { 2558, 0xcad374d8, 36 }, /* vkGetPhysicalDeviceQueueFamilyProperties2 */
    { 2600, 0x5ceb2bed, 37 }, /* vkGetPhysicalDeviceQueueFamilyProperties2KHR */
    { 2645, 0xb5c7dc78, 26 }, /* vkGetPhysicalDeviceScreenPresentationSupportQNX */
    { 2693, 0x272ef8ef, 9 }, /* vkGetPhysicalDeviceSparseImageFormatProperties */
    { 2740, 0xebddba0b, 40 }, /* vkGetPhysicalDeviceSparseImageFormatProperties2 */
    { 2788, 0x8746ed72, 41 }, /* vkGetPhysicalDeviceSparseImageFormatProperties2KHR */
    { 2839, 0x432ca8f7, 68 }, /* vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV */
    { 2905, 0x5a5fba04, 53 }, /* vkGetPhysicalDeviceSurfaceCapabilities2EXT */
    { 2948, 0x9497e378, 56 }, /* vkGetPhysicalDeviceSurfaceCapabilities2KHR */
    { 2991, 0x77890558, 17 }, /* vkGetPhysicalDeviceSurfaceCapabilitiesKHR */
    { 3033, 0xd00b7188, 57 }, /* vkGetPhysicalDeviceSurfaceFormats2KHR */
    { 3071, 0xe32227c8, 18 }, /* vkGetPhysicalDeviceSurfaceFormatsKHR */
    { 3108, 0x33557b83, 65 }, /* vkGetPhysicalDeviceSurfacePresentModes2EXT */
    { 3151, 0x31c3cbd1, 19 }, /* vkGetPhysicalDeviceSurfacePresentModesKHR */
    { 3193, 0x1a687885, 16 }, /* vkGetPhysicalDeviceSurfaceSupportKHR */
    { 3230, 0x7b5f3fb9, 69 }, /* vkGetPhysicalDeviceToolProperties */
    { 3264, 0xd1685100, 70 }, /* vkGetPhysicalDeviceToolPropertiesEXT */
    { 3301, 0xa6ea33de, 21 }, /* vkGetPhysicalDeviceUbmPresentationSupportSEC */
    { 3346, 0x8ee6bf8a, 72 }, /* vkGetPhysicalDeviceVideoCapabilitiesKHR */
    { 3386, 0x886cb8aa, 74 }, /* vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR */
    { 3442, 0xbb7625d6, 73 }, /* vkGetPhysicalDeviceVideoFormatPropertiesKHR */
    { 3486, 0x84e085ac, 20 }, /* vkGetPhysicalDeviceWaylandPresentationSupportKHR */
    { 3535, 0x80e72505, 22 }, /* vkGetPhysicalDeviceWin32PresentationSupportKHR */
    { 3582, 0x41782cb9, 24 }, /* vkGetPhysicalDeviceXcbPresentationSupportKHR */
    { 3627, 0x34a063ab, 23 }, /* vkGetPhysicalDeviceXlibPresentationSupportKHR */
    { 3673, 0xb87cdd6c, 50 }, /* vkGetRandROutputDisplayEXT */
    { 3700, 0x613996b5, 52 }, /* vkGetWinrtDisplayNV */
    { 3720, 0x4207f4f1, 48 }, /* vkReleaseDisplayEXT */
};

/* Hash table stats:
 * size 89 entries
 * collisions entries:
 *     0      56
 *     1      15
 *     2      8
 *     3      2
 *     4      0
 *     5      2
 *     6      1
 *     7      0
 *     8      0
 *     9+     5
 */

#define none 0xffff
static const uint16_t physical_device_string_map[128] = {
    0x002e,
    0x0037,
    0x0040,
    0x0049,
    0x0044,
    0x0042,
    0x0009,
    none,
    0x002f,
    0x000e,
    0x003c,
    0x0041,
    0x000d,
    0x0000,
    0x0011,
    0x0023,
    none,
    none,
    0x001f,
    0x003e,
    0x0054,
    0x0008,
    none,
    0x0058,
    0x0027,
    0x0018,
    0x0010,
    0x0047,
    0x0014,
    0x0043,
    0x0045,
    0x0036,
    0x0002,
    0x0021,
    0x003b,
    none,
    0x000c,
    none,
    0x004d,
    0x0022,
    none,
    none,
    0x0050,
    0x0015,
    0x0052,
    none,
    0x001b,
    0x001d,
    0x004a,
    0x0057,
    none,
    0x0024,
    0x0033,
    0x0051,
    0x0026,
    none,
    none,
    0x0031,
    0x0019,
    0x0029,
    none,
    0x0030,
    0x004b,
    0x0003,
    none,
    0x0004,
    0x000a,
    0x0007,
    none,
    none,
    none,
    none,
    0x0048,
    0x0034,
    0x0038,
    none,
    0x0013,
    0x0001,
    none,
    none,
    none,
    0x0012,
    0x0017,
    0x002c,
    none,
    none,
    0x002a,
    none,
    0x003d,
    none,
    0x0028,
    0x0016,
    0x001c,
    none,
    0x000b,
    0x0020,
    0x0025,
    0x0035,
    none,
    none,
    0x0032,
    0x000f,
    none,
    0x0006,
    none,
    0x004c,
    0x002d,
    0x001e,
    0x0056,
    0x0005,
    0x0053,
    0x0039,
    none,
    0x004e,
    0x001a,
    0x0055,
    none,
    none,
    none,
    0x003a,
    0x003f,
    none,
    0x002b,
    none,
    0x004f,
    none,
    0x0046,
    none,
};

static int
physical_device_string_map_lookup(const char *str)
{
    static const uint32_t prime_factor = 5024183;
    static const uint32_t prime_step = 19;
    const struct string_map_entry *e;
    uint32_t hash, h;
    uint16_t i;
    const char *p;

    hash = 0;
    for (p = str; *p; p++)
        hash = hash * prime_factor + *p;

    h = hash;
    while (1) {
        i = physical_device_string_map[h & 127];
        if (i == none)
           return -1;
        e = &physical_device_string_map_entries[i];
        if (e->hash == hash && strcmp(str, physical_device_strings + e->name) == 0)
            return e->num;
        h += prime_step;
    }

    return -1;
}


static const char device_strings[] =
    "vkAcquireFullScreenExclusiveModeEXT\0"
    "vkAcquireImageANDROID\0"
    "vkAcquireNextImage2KHR\0"
    "vkAcquireNextImageKHR\0"
    "vkAcquirePerformanceConfigurationINTEL\0"
    "vkAcquireProfilingLockKHR\0"
    "vkAllocateCommandBuffers\0"
    "vkAllocateDescriptorSets\0"
    "vkAllocateMemory\0"
    "vkAntiLagUpdateAMD\0"
    "vkBeginCommandBuffer\0"
    "vkBindAccelerationStructureMemoryNV\0"
    "vkBindBufferMemory\0"
    "vkBindBufferMemory2\0"
    "vkBindBufferMemory2KHR\0"
    "vkBindDataGraphPipelineSessionMemoryARM\0"
    "vkBindImageMemory\0"
    "vkBindImageMemory2\0"
    "vkBindImageMemory2KHR\0"
    "vkBindOpticalFlowSessionImageNV\0"
    "vkBindTensorMemoryARM\0"
    "vkBindVideoSessionMemoryKHR\0"
    "vkBuildAccelerationStructuresKHR\0"
    "vkBuildMicromapsEXT\0"
    "vkClearShaderInstrumentationMetricsARM\0"
    "vkCmdBeginConditionalRendering2EXT\0"
    "vkCmdBeginConditionalRenderingEXT\0"
    "vkCmdBeginCustomResolveEXT\0"
    "vkCmdBeginDebugUtilsLabelEXT\0"
    "vkCmdBeginGpaSampleAMD\0"
    "vkCmdBeginGpaSessionAMD\0"
    "vkCmdBeginPerTileExecutionQCOM\0"
    "vkCmdBeginQuery\0"
    "vkCmdBeginQueryIndexedEXT\0"
    "vkCmdBeginRenderPass\0"
    "vkCmdBeginRenderPass2\0"
    "vkCmdBeginRenderPass2KHR\0"
    "vkCmdBeginRendering\0"
    "vkCmdBeginRenderingKHR\0"
    "vkCmdBeginShaderInstrumentationARM\0"
    "vkCmdBeginTransformFeedback2EXT\0"
    "vkCmdBeginTransformFeedbackEXT\0"
    "vkCmdBeginVideoCodingKHR\0"
    "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT\0"
    "vkCmdBindDescriptorBufferEmbeddedSamplersEXT\0"
    "vkCmdBindDescriptorBuffersEXT\0"
    "vkCmdBindDescriptorSets\0"
    "vkCmdBindDescriptorSets2\0"
    "vkCmdBindDescriptorSets2KHR\0"
    "vkCmdBindIndexBuffer\0"
    "vkCmdBindIndexBuffer2\0"
    "vkCmdBindIndexBuffer2KHR\0"
    "vkCmdBindIndexBuffer3KHR\0"
    "vkCmdBindInvocationMaskHUAWEI\0"
    "vkCmdBindPipeline\0"
    "vkCmdBindPipelineShaderGroupNV\0"
    "vkCmdBindResourceHeapEXT\0"
    "vkCmdBindSamplerHeapEXT\0"
    "vkCmdBindShadersEXT\0"
    "vkCmdBindShadingRateImageNV\0"
    "vkCmdBindTileMemoryQCOM\0"
    "vkCmdBindTransformFeedbackBuffers2EXT\0"
    "vkCmdBindTransformFeedbackBuffersEXT\0"
    "vkCmdBindVertexBuffers\0"
    "vkCmdBindVertexBuffers2\0"
    "vkCmdBindVertexBuffers2EXT\0"
    "vkCmdBindVertexBuffers3KHR\0"
    "vkCmdBlitImage\0"
    "vkCmdBlitImage2\0"
    "vkCmdBlitImage2KHR\0"
    "vkCmdBuildAccelerationStructureNV\0"
    "vkCmdBuildAccelerationStructuresIndirectKHR\0"
    "vkCmdBuildAccelerationStructuresKHR\0"
    "vkCmdBuildClusterAccelerationStructureIndirectNV\0"
    "vkCmdBuildMicromapsEXT\0"
    "vkCmdBuildPartitionedAccelerationStructuresNV\0"
    "vkCmdClearAttachments\0"
    "vkCmdClearColorImage\0"
    "vkCmdClearDepthStencilImage\0"
    "vkCmdControlVideoCodingKHR\0"
    "vkCmdConvertCooperativeVectorMatrixNV\0"
    "vkCmdCopyAccelerationStructureKHR\0"
    "vkCmdCopyAccelerationStructureNV\0"
    "vkCmdCopyAccelerationStructureToMemoryKHR\0"
    "vkCmdCopyBuffer\0"
    "vkCmdCopyBuffer2\0"
    "vkCmdCopyBuffer2KHR\0"
    "vkCmdCopyBufferToImage\0"
    "vkCmdCopyBufferToImage2\0"
    "vkCmdCopyBufferToImage2KHR\0"
    "vkCmdCopyGpaSessionResultsAMD\0"
    "vkCmdCopyImage\0"
    "vkCmdCopyImage2\0"
    "vkCmdCopyImage2KHR\0"
    "vkCmdCopyImageToBuffer\0"
    "vkCmdCopyImageToBuffer2\0"
    "vkCmdCopyImageToBuffer2KHR\0"
    "vkCmdCopyImageToMemoryKHR\0"
    "vkCmdCopyMemoryIndirectKHR\0"
    "vkCmdCopyMemoryIndirectNV\0"
    "vkCmdCopyMemoryKHR\0"
    "vkCmdCopyMemoryToAccelerationStructureKHR\0"
    "vkCmdCopyMemoryToImageIndirectKHR\0"
    "vkCmdCopyMemoryToImageIndirectNV\0"
    "vkCmdCopyMemoryToImageKHR\0"
    "vkCmdCopyMemoryToMicromapEXT\0"
    "vkCmdCopyMicromapEXT\0"
    "vkCmdCopyMicromapToMemoryEXT\0"
    "vkCmdCopyQueryPoolResults\0"
    "vkCmdCopyQueryPoolResultsToMemoryKHR\0"
    "vkCmdCopyTensorARM\0"
    "vkCmdCuLaunchKernelNVX\0"
    "vkCmdDebugMarkerBeginEXT\0"
    "vkCmdDebugMarkerEndEXT\0"
    "vkCmdDebugMarkerInsertEXT\0"
    "vkCmdDecodeVideoKHR\0"
    "vkCmdDecompressMemoryEXT\0"
    "vkCmdDecompressMemoryIndirectCountEXT\0"
    "vkCmdDecompressMemoryIndirectCountNV\0"
    "vkCmdDecompressMemoryNV\0"
    "vkCmdDispatch\0"
    "vkCmdDispatchBase\0"
    "vkCmdDispatchBaseKHR\0"
    "vkCmdDispatchDataGraphARM\0"
    "vkCmdDispatchIndirect\0"
    "vkCmdDispatchIndirect2KHR\0"
    "vkCmdDispatchTileQCOM\0"
    "vkCmdDraw\0"
    "vkCmdDrawClusterHUAWEI\0"
    "vkCmdDrawClusterIndirectHUAWEI\0"
    "vkCmdDrawIndexed\0"
    "vkCmdDrawIndexedIndirect\0"
    "vkCmdDrawIndexedIndirect2KHR\0"
    "vkCmdDrawIndexedIndirectCount\0"
    "vkCmdDrawIndexedIndirectCount2KHR\0"
    "vkCmdDrawIndexedIndirectCountAMD\0"
    "vkCmdDrawIndexedIndirectCountKHR\0"
    "vkCmdDrawIndirect\0"
    "vkCmdDrawIndirect2KHR\0"
    "vkCmdDrawIndirectByteCount2EXT\0"
    "vkCmdDrawIndirectByteCountEXT\0"
    "vkCmdDrawIndirectCount\0"
    "vkCmdDrawIndirectCount2KHR\0"
    "vkCmdDrawIndirectCountAMD\0"
    "vkCmdDrawIndirectCountKHR\0"
    "vkCmdDrawMeshTasksEXT\0"
    "vkCmdDrawMeshTasksIndirect2EXT\0"
    "vkCmdDrawMeshTasksIndirectCount2EXT\0"
    "vkCmdDrawMeshTasksIndirectCountEXT\0"
    "vkCmdDrawMeshTasksIndirectCountNV\0"
    "vkCmdDrawMeshTasksIndirectEXT\0"
    "vkCmdDrawMeshTasksIndirectNV\0"
    "vkCmdDrawMeshTasksNV\0"
    "vkCmdDrawMultiEXT\0"
    "vkCmdDrawMultiIndexedEXT\0"
    "vkCmdEncodeVideoKHR\0"
    "vkCmdEndConditionalRenderingEXT\0"
    "vkCmdEndDebugUtilsLabelEXT\0"
    "vkCmdEndGpaSampleAMD\0"
    "vkCmdEndGpaSessionAMD\0"
    "vkCmdEndPerTileExecutionQCOM\0"
    "vkCmdEndQuery\0"
    "vkCmdEndQueryIndexedEXT\0"
    "vkCmdEndRenderPass\0"
    "vkCmdEndRenderPass2\0"
    "vkCmdEndRenderPass2KHR\0"
    "vkCmdEndRendering\0"
    "vkCmdEndRendering2EXT\0"
    "vkCmdEndRendering2KHR\0"
    "vkCmdEndRenderingKHR\0"
    "vkCmdEndShaderInstrumentationARM\0"
    "vkCmdEndTransformFeedback2EXT\0"
    "vkCmdEndTransformFeedbackEXT\0"
    "vkCmdEndVideoCodingKHR\0"
    "vkCmdExecuteCommands\0"
    "vkCmdExecuteGeneratedCommandsEXT\0"
    "vkCmdExecuteGeneratedCommandsNV\0"
    "vkCmdFillBuffer\0"
    "vkCmdFillMemoryKHR\0"
    "vkCmdInsertDebugUtilsLabelEXT\0"
    "vkCmdNextSubpass\0"
    "vkCmdNextSubpass2\0"
    "vkCmdNextSubpass2KHR\0"
    "vkCmdOpticalFlowExecuteNV\0"
    "vkCmdPipelineBarrier\0"
    "vkCmdPipelineBarrier2\0"
    "vkCmdPipelineBarrier2KHR\0"
    "vkCmdPreprocessGeneratedCommandsEXT\0"
    "vkCmdPreprocessGeneratedCommandsNV\0"
    "vkCmdPushConstants\0"
    "vkCmdPushConstants2\0"
    "vkCmdPushConstants2KHR\0"
    "vkCmdPushDataEXT\0"
    "vkCmdPushDescriptorSet\0"
    "vkCmdPushDescriptorSet2\0"
    "vkCmdPushDescriptorSet2KHR\0"
    "vkCmdPushDescriptorSetKHR\0"
    "vkCmdPushDescriptorSetWithTemplate\0"
    "vkCmdPushDescriptorSetWithTemplate2\0"
    "vkCmdPushDescriptorSetWithTemplate2KHR\0"
    "vkCmdPushDescriptorSetWithTemplateKHR\0"
    "vkCmdResetEvent\0"
    "vkCmdResetEvent2\0"
    "vkCmdResetEvent2KHR\0"
    "vkCmdResetQueryPool\0"
    "vkCmdResolveImage\0"
    "vkCmdResolveImage2\0"
    "vkCmdResolveImage2KHR\0"
    "vkCmdSetAlphaToCoverageEnableEXT\0"
    "vkCmdSetAlphaToOneEnableEXT\0"
    "vkCmdSetAttachmentFeedbackLoopEnableEXT\0"
    "vkCmdSetBlendConstants\0"
    "vkCmdSetCheckpointNV\0"
    "vkCmdSetCoarseSampleOrderNV\0"
    "vkCmdSetColorBlendAdvancedEXT\0"
    "vkCmdSetColorBlendEnableEXT\0"
    "vkCmdSetColorBlendEquationEXT\0"
    "vkCmdSetColorWriteEnableEXT\0"
    "vkCmdSetColorWriteMaskEXT\0"
    "vkCmdSetComputeOccupancyPriorityNV\0"
    "vkCmdSetConservativeRasterizationModeEXT\0"
    "vkCmdSetCoverageModulationModeNV\0"
    "vkCmdSetCoverageModulationTableEnableNV\0"
    "vkCmdSetCoverageModulationTableNV\0"
    "vkCmdSetCoverageReductionModeNV\0"
    "vkCmdSetCoverageToColorEnableNV\0"
    "vkCmdSetCoverageToColorLocationNV\0"
    "vkCmdSetCullMode\0"
    "vkCmdSetCullModeEXT\0"
    "vkCmdSetDepthBias\0"
    "vkCmdSetDepthBias2EXT\0"
    "vkCmdSetDepthBiasEnable\0"
    "vkCmdSetDepthBiasEnableEXT\0"
    "vkCmdSetDepthBounds\0"
    "vkCmdSetDepthBoundsTestEnable\0"
    "vkCmdSetDepthBoundsTestEnableEXT\0"
    "vkCmdSetDepthClampEnableEXT\0"
    "vkCmdSetDepthClampRangeEXT\0"
    "vkCmdSetDepthClipEnableEXT\0"
    "vkCmdSetDepthClipNegativeOneToOneEXT\0"
    "vkCmdSetDepthCompareOp\0"
    "vkCmdSetDepthCompareOpEXT\0"
    "vkCmdSetDepthTestEnable\0"
    "vkCmdSetDepthTestEnableEXT\0"
    "vkCmdSetDepthWriteEnable\0"
    "vkCmdSetDepthWriteEnableEXT\0"
    "vkCmdSetDescriptorBufferOffsets2EXT\0"
    "vkCmdSetDescriptorBufferOffsetsEXT\0"
    "vkCmdSetDeviceMask\0"
    "vkCmdSetDeviceMaskKHR\0"
    "vkCmdSetDiscardRectangleEXT\0"
    "vkCmdSetDiscardRectangleEnableEXT\0"
    "vkCmdSetDiscardRectangleModeEXT\0"
    "vkCmdSetDispatchParametersARM\0"
    "vkCmdSetEvent\0"
    "vkCmdSetEvent2\0"
    "vkCmdSetEvent2KHR\0"
    "vkCmdSetExclusiveScissorEnableNV\0"
    "vkCmdSetExclusiveScissorNV\0"
    "vkCmdSetExtraPrimitiveOverestimationSizeEXT\0"
    "vkCmdSetFragmentShadingRateEnumNV\0"
    "vkCmdSetFragmentShadingRateKHR\0"
    "vkCmdSetFrontFace\0"
    "vkCmdSetFrontFaceEXT\0"
    "vkCmdSetLineRasterizationModeEXT\0"
    "vkCmdSetLineStipple\0"
    "vkCmdSetLineStippleEXT\0"
    "vkCmdSetLineStippleEnableEXT\0"
    "vkCmdSetLineStippleKHR\0"
    "vkCmdSetLineWidth\0"
    "vkCmdSetLogicOpEXT\0"
    "vkCmdSetLogicOpEnableEXT\0"
    "vkCmdSetPatchControlPointsEXT\0"
    "vkCmdSetPerformanceMarkerINTEL\0"
    "vkCmdSetPerformanceOverrideINTEL\0"
    "vkCmdSetPerformanceStreamMarkerINTEL\0"
    "vkCmdSetPolygonModeEXT\0"
    "vkCmdSetPrimitiveRestartEnable\0"
    "vkCmdSetPrimitiveRestartEnableEXT\0"
    "vkCmdSetPrimitiveRestartIndexEXT\0"
    "vkCmdSetPrimitiveTopology\0"
    "vkCmdSetPrimitiveTopologyEXT\0"
    "vkCmdSetProvokingVertexModeEXT\0"
    "vkCmdSetRasterizationSamplesEXT\0"
    "vkCmdSetRasterizationStreamEXT\0"
    "vkCmdSetRasterizerDiscardEnable\0"
    "vkCmdSetRasterizerDiscardEnableEXT\0"
    "vkCmdSetRayTracingPipelineStackSizeKHR\0"
    "vkCmdSetRenderingAttachmentLocations\0"
    "vkCmdSetRenderingAttachmentLocationsKHR\0"
    "vkCmdSetRenderingInputAttachmentIndices\0"
    "vkCmdSetRenderingInputAttachmentIndicesKHR\0"
    "vkCmdSetRepresentativeFragmentTestEnableNV\0"
    "vkCmdSetSampleLocationsEXT\0"
    "vkCmdSetSampleLocationsEnableEXT\0"
    "vkCmdSetSampleMaskEXT\0"
    "vkCmdSetScissor\0"
    "vkCmdSetScissorWithCount\0"
    "vkCmdSetScissorWithCountEXT\0"
    "vkCmdSetShadingRateImageEnableNV\0"
    "vkCmdSetStencilCompareMask\0"
    "vkCmdSetStencilOp\0"
    "vkCmdSetStencilOpEXT\0"
    "vkCmdSetStencilReference\0"
    "vkCmdSetStencilTestEnable\0"
    "vkCmdSetStencilTestEnableEXT\0"
    "vkCmdSetStencilWriteMask\0"
    "vkCmdSetTessellationDomainOriginEXT\0"
    "vkCmdSetVertexInputEXT\0"
    "vkCmdSetViewport\0"
    "vkCmdSetViewportShadingRatePaletteNV\0"
    "vkCmdSetViewportSwizzleNV\0"
    "vkCmdSetViewportWScalingEnableNV\0"
    "vkCmdSetViewportWScalingNV\0"
    "vkCmdSetViewportWithCount\0"
    "vkCmdSetViewportWithCountEXT\0"
    "vkCmdSubpassShadingHUAWEI\0"
    "vkCmdTraceRaysIndirect2KHR\0"
    "vkCmdTraceRaysIndirectKHR\0"
    "vkCmdTraceRaysKHR\0"
    "vkCmdTraceRaysNV\0"
    "vkCmdUpdateBuffer\0"
    "vkCmdUpdateMemoryKHR\0"
    "vkCmdUpdatePipelineIndirectBufferNV\0"
    "vkCmdWaitEvents\0"
    "vkCmdWaitEvents2\0"
    "vkCmdWaitEvents2KHR\0"
    "vkCmdWriteAccelerationStructuresPropertiesKHR\0"
    "vkCmdWriteAccelerationStructuresPropertiesNV\0"
    "vkCmdWriteBufferMarker2AMD\0"
    "vkCmdWriteBufferMarkerAMD\0"
    "vkCmdWriteMarkerToMemoryAMD\0"
    "vkCmdWriteMicromapsPropertiesEXT\0"
    "vkCmdWriteTimestamp\0"
    "vkCmdWriteTimestamp2\0"
    "vkCmdWriteTimestamp2KHR\0"
    "vkCompileDeferredNV\0"
    "vkConvertCooperativeVectorMatrixNV\0"
    "vkCopyAccelerationStructureKHR\0"
    "vkCopyAccelerationStructureToMemoryKHR\0"
    "vkCopyImageToImage\0"
    "vkCopyImageToImageEXT\0"
    "vkCopyImageToMemory\0"
    "vkCopyImageToMemoryEXT\0"
    "vkCopyMemoryToAccelerationStructureKHR\0"
    "vkCopyMemoryToImage\0"
    "vkCopyMemoryToImageEXT\0"
    "vkCopyMemoryToMicromapEXT\0"
    "vkCopyMicromapEXT\0"
    "vkCopyMicromapToMemoryEXT\0"
    "vkCreateAccelerationStructure2KHR\0"
    "vkCreateAccelerationStructureKHR\0"
    "vkCreateAccelerationStructureNV\0"
    "vkCreateBuffer\0"
    "vkCreateBufferCollectionFUCHSIA\0"
    "vkCreateBufferView\0"
    "vkCreateCommandPool\0"
    "vkCreateComputePipelines\0"
    "vkCreateCuFunctionNVX\0"
    "vkCreateCuModuleNVX\0"
    "vkCreateDataGraphPipelineSessionARM\0"
    "vkCreateDataGraphPipelinesARM\0"
    "vkCreateDeferredOperationKHR\0"
    "vkCreateDescriptorPool\0"
    "vkCreateDescriptorSetLayout\0"
    "vkCreateDescriptorUpdateTemplate\0"
    "vkCreateDescriptorUpdateTemplateKHR\0"
    "vkCreateEvent\0"
    "vkCreateExternalComputeQueueNV\0"
    "vkCreateFence\0"
    "vkCreateFramebuffer\0"
    "vkCreateGpaSessionAMD\0"
    "vkCreateGraphicsPipelines\0"
    "vkCreateImage\0"
    "vkCreateImageView\0"
    "vkCreateIndirectCommandsLayoutEXT\0"
    "vkCreateIndirectCommandsLayoutNV\0"
    "vkCreateIndirectExecutionSetEXT\0"
    "vkCreateMicromapEXT\0"
    "vkCreateOpticalFlowSessionNV\0"
    "vkCreatePipelineBinariesKHR\0"
    "vkCreatePipelineCache\0"
    "vkCreatePipelineLayout\0"
    "vkCreatePrivateDataSlot\0"
    "vkCreatePrivateDataSlotEXT\0"
    "vkCreateQueryPool\0"
    "vkCreateRayTracingPipelinesKHR\0"
    "vkCreateRayTracingPipelinesNV\0"
    "vkCreateRenderPass\0"
    "vkCreateRenderPass2\0"
    "vkCreateRenderPass2KHR\0"
    "vkCreateSampler\0"
    "vkCreateSamplerYcbcrConversion\0"
    "vkCreateSamplerYcbcrConversionKHR\0"
    "vkCreateSemaphore\0"
    "vkCreateShaderInstrumentationARM\0"
    "vkCreateShaderModule\0"
    "vkCreateShadersEXT\0"
    "vkCreateSharedSwapchainsKHR\0"
    "vkCreateSwapchainKHR\0"
    "vkCreateTensorARM\0"
    "vkCreateTensorViewARM\0"
    "vkCreateValidationCacheEXT\0"
    "vkCreateVideoSessionKHR\0"
    "vkCreateVideoSessionParametersKHR\0"
    "vkDebugMarkerSetObjectNameEXT\0"
    "vkDebugMarkerSetObjectTagEXT\0"
    "vkDeferredOperationJoinKHR\0"
    "vkDestroyAccelerationStructureKHR\0"
    "vkDestroyAccelerationStructureNV\0"
    "vkDestroyBuffer\0"
    "vkDestroyBufferCollectionFUCHSIA\0"
    "vkDestroyBufferView\0"
    "vkDestroyCommandPool\0"
    "vkDestroyCuFunctionNVX\0"
    "vkDestroyCuModuleNVX\0"
    "vkDestroyDataGraphPipelineSessionARM\0"
    "vkDestroyDeferredOperationKHR\0"
    "vkDestroyDescriptorPool\0"
    "vkDestroyDescriptorSetLayout\0"
    "vkDestroyDescriptorUpdateTemplate\0"
    "vkDestroyDescriptorUpdateTemplateKHR\0"
    "vkDestroyDevice\0"
    "vkDestroyEvent\0"
    "vkDestroyExternalComputeQueueNV\0"
    "vkDestroyFence\0"
    "vkDestroyFramebuffer\0"
    "vkDestroyGpaSessionAMD\0"
    "vkDestroyImage\0"
    "vkDestroyImageView\0"
    "vkDestroyIndirectCommandsLayoutEXT\0"
    "vkDestroyIndirectCommandsLayoutNV\0"
    "vkDestroyIndirectExecutionSetEXT\0"
    "vkDestroyMicromapEXT\0"
    "vkDestroyOpticalFlowSessionNV\0"
    "vkDestroyPipeline\0"
    "vkDestroyPipelineBinaryKHR\0"
    "vkDestroyPipelineCache\0"
    "vkDestroyPipelineLayout\0"
    "vkDestroyPrivateDataSlot\0"
    "vkDestroyPrivateDataSlotEXT\0"
    "vkDestroyQueryPool\0"
    "vkDestroyRenderPass\0"
    "vkDestroySampler\0"
    "vkDestroySamplerYcbcrConversion\0"
    "vkDestroySamplerYcbcrConversionKHR\0"
    "vkDestroySemaphore\0"
    "vkDestroyShaderEXT\0"
    "vkDestroyShaderInstrumentationARM\0"
    "vkDestroyShaderModule\0"
    "vkDestroySwapchainKHR\0"
    "vkDestroyTensorARM\0"
    "vkDestroyTensorViewARM\0"
    "vkDestroyValidationCacheEXT\0"
    "vkDestroyVideoSessionKHR\0"
    "vkDestroyVideoSessionParametersKHR\0"
    "vkDeviceWaitIdle\0"
    "vkDisplayPowerControlEXT\0"
    "vkEndCommandBuffer\0"
    "vkExportMetalObjectsEXT\0"
    "vkFlushMappedMemoryRanges\0"
    "vkFreeCommandBuffers\0"
    "vkFreeDescriptorSets\0"
    "vkFreeMemory\0"
    "vkGetAccelerationStructureBuildSizesKHR\0"
    "vkGetAccelerationStructureDeviceAddressKHR\0"
    "vkGetAccelerationStructureHandleNV\0"
    "vkGetAccelerationStructureMemoryRequirementsNV\0"
    "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT\0"
    "vkGetAndroidHardwareBufferPropertiesANDROID\0"
    "vkGetBufferCollectionPropertiesFUCHSIA\0"
    "vkGetBufferDeviceAddress\0"
    "vkGetBufferDeviceAddressEXT\0"
    "vkGetBufferDeviceAddressKHR\0"
    "vkGetBufferMemoryRequirements\0"
    "vkGetBufferMemoryRequirements2\0"
    "vkGetBufferMemoryRequirements2KHR\0"
    "vkGetBufferOpaqueCaptureAddress\0"
    "vkGetBufferOpaqueCaptureAddressKHR\0"
    "vkGetBufferOpaqueCaptureDescriptorDataEXT\0"
    "vkGetCalibratedTimestampsEXT\0"
    "vkGetCalibratedTimestampsKHR\0"
    "vkGetClusterAccelerationStructureBuildSizesNV\0"
    "vkGetDataGraphPipelineAvailablePropertiesARM\0"
    "vkGetDataGraphPipelinePropertiesARM\0"
    "vkGetDataGraphPipelineSessionBindPointRequirementsARM\0"
    "vkGetDataGraphPipelineSessionMemoryRequirementsARM\0"
    "vkGetDeferredOperationMaxConcurrencyKHR\0"
    "vkGetDeferredOperationResultKHR\0"
    "vkGetDescriptorEXT\0"
    "vkGetDescriptorSetHostMappingVALVE\0"
    "vkGetDescriptorSetLayoutBindingOffsetEXT\0"
    "vkGetDescriptorSetLayoutHostMappingInfoVALVE\0"
    "vkGetDescriptorSetLayoutSizeEXT\0"
    "vkGetDescriptorSetLayoutSupport\0"
    "vkGetDescriptorSetLayoutSupportKHR\0"
    "vkGetDeviceAccelerationStructureCompatibilityKHR\0"
    "vkGetDeviceBufferMemoryRequirements\0"
    "vkGetDeviceBufferMemoryRequirementsKHR\0"
    "vkGetDeviceCombinedImageSamplerIndexNVX\0"
    "vkGetDeviceFaultDebugInfoKHR\0"
    "vkGetDeviceFaultInfoEXT\0"
    "vkGetDeviceFaultReportsKHR\0"
    "vkGetDeviceGroupPeerMemoryFeatures\0"
    "vkGetDeviceGroupPeerMemoryFeaturesKHR\0"
    "vkGetDeviceGroupPresentCapabilitiesKHR\0"
    "vkGetDeviceGroupSurfacePresentModes2EXT\0"
    "vkGetDeviceGroupSurfacePresentModesKHR\0"
    "vkGetDeviceImageMemoryRequirements\0"
    "vkGetDeviceImageMemoryRequirementsKHR\0"
    "vkGetDeviceImageSparseMemoryRequirements\0"
    "vkGetDeviceImageSparseMemoryRequirementsKHR\0"
    "vkGetDeviceImageSubresourceLayout\0"
    "vkGetDeviceImageSubresourceLayoutKHR\0"
    "vkGetDeviceMemoryCommitment\0"
    "vkGetDeviceMemoryOpaqueCaptureAddress\0"
    "vkGetDeviceMemoryOpaqueCaptureAddressKHR\0"
    "vkGetDeviceMicromapCompatibilityEXT\0"
    "vkGetDeviceProcAddr\0"
    "vkGetDeviceQueue\0"
    "vkGetDeviceQueue2\0"
    "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI\0"
    "vkGetDeviceTensorMemoryRequirementsARM\0"
    "vkGetDynamicRenderingTilePropertiesQCOM\0"
    "vkGetEncodedVideoSessionParametersKHR\0"
    "vkGetEventStatus\0"
    "vkGetFenceFdKHR\0"
    "vkGetFenceStatus\0"
    "vkGetFenceWin32HandleKHR\0"
    "vkGetFramebufferTilePropertiesQCOM\0"
    "vkGetGeneratedCommandsMemoryRequirementsEXT\0"
    "vkGetGeneratedCommandsMemoryRequirementsNV\0"
    "vkGetGpaDeviceClockInfoAMD\0"
    "vkGetGpaSessionResultsAMD\0"
    "vkGetGpaSessionStatusAMD\0"
    "vkGetImageDrmFormatModifierPropertiesEXT\0"
    "vkGetImageMemoryRequirements\0"
    "vkGetImageMemoryRequirements2\0"
    "vkGetImageMemoryRequirements2KHR\0"
    "vkGetImageOpaqueCaptureDataEXT\0"
    "vkGetImageOpaqueCaptureDescriptorDataEXT\0"
    "vkGetImageSparseMemoryRequirements\0"
    "vkGetImageSparseMemoryRequirements2\0"
    "vkGetImageSparseMemoryRequirements2KHR\0"
    "vkGetImageSubresourceLayout\0"
    "vkGetImageSubresourceLayout2\0"
    "vkGetImageSubresourceLayout2EXT\0"
    "vkGetImageSubresourceLayout2KHR\0"
    "vkGetImageViewAddressNVX\0"
    "vkGetImageViewHandle64NVX\0"
    "vkGetImageViewHandleNVX\0"
    "vkGetImageViewOpaqueCaptureDescriptorDataEXT\0"
    "vkGetLatencyTimingsNV\0"
    "vkGetMemoryAndroidHardwareBufferANDROID\0"
    "vkGetMemoryFdKHR\0"
    "vkGetMemoryFdPropertiesKHR\0"
    "vkGetMemoryHostPointerPropertiesEXT\0"
    "vkGetMemoryMetalHandleEXT\0"
    "vkGetMemoryMetalHandlePropertiesEXT\0"
    "vkGetMemoryNativeBufferOHOS\0"
    "vkGetMemoryRemoteAddressNV\0"
    "vkGetMemoryWin32HandleKHR\0"
    "vkGetMemoryWin32HandleNV\0"
    "vkGetMemoryWin32HandlePropertiesKHR\0"
    "vkGetMemoryZirconHandleFUCHSIA\0"
    "vkGetMemoryZirconHandlePropertiesFUCHSIA\0"
    "vkGetMicromapBuildSizesEXT\0"
    "vkGetNativeBufferPropertiesOHOS\0"
    "vkGetPartitionedAccelerationStructuresBuildSizesNV\0"
    "vkGetPastPresentationTimingEXT\0"
    "vkGetPastPresentationTimingGOOGLE\0"
    "vkGetPerformanceParameterINTEL\0"
    "vkGetPipelineBinaryDataKHR\0"
    "vkGetPipelineCacheData\0"
    "vkGetPipelineExecutableInternalRepresentationsKHR\0"
    "vkGetPipelineExecutablePropertiesKHR\0"
    "vkGetPipelineExecutableStatisticsKHR\0"
    "vkGetPipelineIndirectDeviceAddressNV\0"
    "vkGetPipelineIndirectMemoryRequirementsNV\0"
    "vkGetPipelineKeyKHR\0"
    "vkGetPipelinePropertiesEXT\0"
    "vkGetPrivateData\0"
    "vkGetPrivateDataEXT\0"
    "vkGetQueryPoolResults\0"
    "vkGetQueueCheckpointData2NV\0"
    "vkGetQueueCheckpointDataNV\0"
    "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR\0"
    "vkGetRayTracingShaderGroupHandlesKHR\0"
    "vkGetRayTracingShaderGroupHandlesNV\0"
    "vkGetRayTracingShaderGroupStackSizeKHR\0"
    "vkGetRefreshCycleDurationGOOGLE\0"
    "vkGetRenderAreaGranularity\0"
    "vkGetRenderingAreaGranularity\0"
    "vkGetRenderingAreaGranularityKHR\0"
    "vkGetSamplerOpaqueCaptureDescriptorDataEXT\0"
    "vkGetScreenBufferPropertiesQNX\0"
    "vkGetSemaphoreCounterValue\0"
    "vkGetSemaphoreCounterValueKHR\0"
    "vkGetSemaphoreFdKHR\0"
    "vkGetSemaphoreWin32HandleKHR\0"
    "vkGetSemaphoreZirconHandleFUCHSIA\0"
    "vkGetShaderBinaryDataEXT\0"
    "vkGetShaderInfoAMD\0"
    "vkGetShaderInstrumentationValuesARM\0"
    "vkGetShaderModuleCreateInfoIdentifierEXT\0"
    "vkGetShaderModuleIdentifierEXT\0"
    "vkGetSwapchainCounterEXT\0"
    "vkGetSwapchainGrallocUsage2ANDROID\0"
    "vkGetSwapchainGrallocUsageANDROID\0"
    "vkGetSwapchainImagesKHR\0"
    "vkGetSwapchainStatusKHR\0"
    "vkGetSwapchainTimeDomainPropertiesEXT\0"
    "vkGetSwapchainTimingPropertiesEXT\0"
    "vkGetTensorMemoryRequirementsARM\0"
    "vkGetTensorOpaqueCaptureDataARM\0"
    "vkGetTensorOpaqueCaptureDescriptorDataARM\0"
    "vkGetTensorViewOpaqueCaptureDescriptorDataARM\0"
    "vkGetValidationCacheDataEXT\0"
    "vkGetVideoSessionMemoryRequirementsKHR\0"
    "vkImportFenceFdKHR\0"
    "vkImportFenceWin32HandleKHR\0"
    "vkImportSemaphoreFdKHR\0"
    "vkImportSemaphoreWin32HandleKHR\0"
    "vkImportSemaphoreZirconHandleFUCHSIA\0"
    "vkInitializePerformanceApiINTEL\0"
    "vkInvalidateMappedMemoryRanges\0"
    "vkLatencySleepNV\0"
    "vkMapMemory\0"
    "vkMapMemory2\0"
    "vkMapMemory2KHR\0"
    "vkMergePipelineCaches\0"
    "vkMergeValidationCachesEXT\0"
    "vkQueueBeginDebugUtilsLabelEXT\0"
    "vkQueueBindSparse\0"
    "vkQueueEndDebugUtilsLabelEXT\0"
    "vkQueueInsertDebugUtilsLabelEXT\0"
    "vkQueueNotifyOutOfBandNV\0"
    "vkQueuePresentKHR\0"
    "vkQueueSetPerfHintQCOM\0"
    "vkQueueSetPerformanceConfigurationINTEL\0"
    "vkQueueSignalReleaseImageANDROID\0"
    "vkQueueSubmit\0"
    "vkQueueSubmit2\0"
    "vkQueueSubmit2KHR\0"
    "vkQueueWaitIdle\0"
    "vkRegisterCustomBorderColorEXT\0"
    "vkRegisterDeviceEventEXT\0"
    "vkRegisterDisplayEventEXT\0"
    "vkReleaseCapturedPipelineDataKHR\0"
    "vkReleaseFullScreenExclusiveModeEXT\0"
    "vkReleasePerformanceConfigurationINTEL\0"
    "vkReleaseProfilingLockKHR\0"
    "vkReleaseSwapchainImagesEXT\0"
    "vkReleaseSwapchainImagesKHR\0"
    "vkResetCommandBuffer\0"
    "vkResetCommandPool\0"
    "vkResetDescriptorPool\0"
    "vkResetEvent\0"
    "vkResetFences\0"
    "vkResetGpaSessionAMD\0"
    "vkResetQueryPool\0"
    "vkResetQueryPoolEXT\0"
    "vkSetBufferCollectionBufferConstraintsFUCHSIA\0"
    "vkSetBufferCollectionImageConstraintsFUCHSIA\0"
    "vkSetDebugUtilsObjectNameEXT\0"
    "vkSetDebugUtilsObjectTagEXT\0"
    "vkSetDeviceMemoryPriorityEXT\0"
    "vkSetEvent\0"
    "vkSetGpaDeviceClockModeAMD\0"
    "vkSetHdrMetadataEXT\0"
    "vkSetLatencyMarkerNV\0"
    "vkSetLatencySleepModeNV\0"
    "vkSetLocalDimmingAMD\0"
    "vkSetPrivateData\0"
    "vkSetPrivateDataEXT\0"
    "vkSetSwapchainPresentTimingQueueSizeEXT\0"
    "vkSignalSemaphore\0"
    "vkSignalSemaphoreKHR\0"
    "vkTransitionImageLayout\0"
    "vkTransitionImageLayoutEXT\0"
    "vkTrimCommandPool\0"
    "vkTrimCommandPoolKHR\0"
    "vkUninitializePerformanceApiINTEL\0"
    "vkUnmapMemory\0"
    "vkUnmapMemory2\0"
    "vkUnmapMemory2KHR\0"
    "vkUnregisterCustomBorderColorEXT\0"
    "vkUpdateDescriptorSetWithTemplate\0"
    "vkUpdateDescriptorSetWithTemplateKHR\0"
    "vkUpdateDescriptorSets\0"
    "vkUpdateIndirectExecutionSetPipelineEXT\0"
    "vkUpdateIndirectExecutionSetShaderEXT\0"
    "vkUpdateVideoSessionParametersKHR\0"
    "vkWaitForFences\0"
    "vkWaitForPresent2KHR\0"
    "vkWaitForPresentKHR\0"
    "vkWaitSemaphores\0"
    "vkWaitSemaphoresKHR\0"
    "vkWriteAccelerationStructuresPropertiesKHR\0"
    "vkWriteMicromapsPropertiesEXT\0"
    "vkWriteResourceDescriptorsEXT\0"
    "vkWriteSamplerDescriptorsEXT\0"
;

static const struct string_map_entry device_string_map_entries[] = {
    { 0, 0x8a43a1cc, 348 }, /* vkAcquireFullScreenExclusiveModeEXT */
    { 36, 0x6bf780dd, 253 }, /* vkAcquireImageANDROID */
    { 58, 0x82860572, 208 }, /* vkAcquireNextImage2KHR */
    { 81, 0xc3fedb2e, 150 }, /* vkAcquireNextImageKHR */
    { 103, 0x33d2767, 363 }, /* vkAcquirePerformanceConfigurationINTEL */
    { 142, 0xaf1d64ad, 350 }, /* vkAcquireProfilingLockKHR */
    { 168, 0x8c0c811a, 82 }, /* vkAllocateCommandBuffers */
    { 193, 0x4c449d3a, 69 }, /* vkAllocateDescriptorSets */
    { 218, 0x522b85d3, 6 }, /* vkAllocateMemory */
    { 235, 0x14f172ac, 387 }, /* vkAntiLagUpdateAMD */
    { 254, 0xc54f7327, 84 }, /* vkBeginCommandBuffer */
    { 275, 0x3ec4e21a, 316 }, /* vkBindAccelerationStructureMemoryNV */
    { 311, 0x6bcbdcb, 14 }, /* vkBindBufferMemory */
    { 330, 0xc27aaf4f, 200 }, /* vkBindBufferMemory2 */
    { 350, 0x6878d3ce, 201 }, /* vkBindBufferMemory2KHR */
    { 373, 0xabc6f019, 661 }, /* vkBindDataGraphPipelineSessionMemoryARM */
    { 413, 0x5caaae4a, 16 }, /* vkBindImageMemory */
    { 431, 0xa9097118, 202 }, /* vkBindImageMemory2 */
    { 450, 0xf18729ad, 203 }, /* vkBindImageMemory2KHR */
    { 472, 0xc3b6afe1, 577 }, /* vkBindOpticalFlowSessionImageNV */
    { 504, 0x88808458, 652 }, /* vkBindTensorMemoryARM */
    { 526, 0x61c0a1e7, 507 }, /* vkBindVideoSessionMemoryKHR */
    { 554, 0xf7d6c55c, 378 }, /* vkBuildAccelerationStructuresKHR */
    { 587, 0x9aa65b87, 553 }, /* vkBuildMicromapsEXT */
    { 607, 0x118db34a, 646 }, /* vkClearShaderInstrumentationMetricsARM */
    { 646, 0xd9a065b1, 685 }, /* vkCmdBeginConditionalRendering2EXT */
    { 681, 0xe561c19f, 135 }, /* vkCmdBeginConditionalRenderingEXT */
    { 715, 0xec7dff1a, 137 }, /* vkCmdBeginCustomResolveEXT */
    { 742, 0x6184193f, 264 }, /* vkCmdBeginDebugUtilsLabelEXT */
    { 771, 0x4ab1675a, 606 }, /* vkCmdBeginGpaSampleAMD */
    { 794, 0x97caaf5e, 604 }, /* vkCmdBeginGpaSessionAMD */
    { 818, 0x5630459, 637 }, /* vkCmdBeginPerTileExecutionQCOM */
    { 849, 0xf5064ea4, 133 }, /* vkCmdBeginQuery */
    { 865, 0x73251a2c, 296 }, /* vkCmdBeginQueryIndexedEXT */
    { 891, 0xcb7a58e3, 142 }, /* vkCmdBeginRenderPass */
    { 912, 0x9c876577, 271 }, /* vkCmdBeginRenderPass2 */
    { 934, 0x8b6b4de6, 272 }, /* vkCmdBeginRenderPass2KHR */
    { 959, 0x385cfdb8, 543 }, /* vkCmdBeginRendering */
    { 979, 0x50d17e0d, 544 }, /* vkCmdBeginRenderingKHR */
    { 1002, 0x68ef89af, 643 }, /* vkCmdBeginShaderInstrumentationARM */
    { 1037, 0xe8d6c7d4, 687 }, /* vkCmdBeginTransformFeedback2EXT */
    { 1069, 0xb217c94, 294 }, /* vkCmdBeginTransformFeedbackEXT */
    { 1100, 0xd941eabc, 509 }, /* vkCmdBeginVideoCodingKHR */
    { 1125, 0x5bacd94d, 621 }, /* vkCmdBindDescriptorBufferEmbeddedSamplers2EXT */
    { 1171, 0xaf295ae3, 529 }, /* vkCmdBindDescriptorBufferEmbeddedSamplersEXT */
    { 1216, 0x64e825f, 527 }, /* vkCmdBindDescriptorBuffersEXT */
    { 1246, 0x28c7a5da, 99 }, /* vkCmdBindDescriptorSets */
    { 1270, 0x2fec7908, 612 }, /* vkCmdBindDescriptorSets2 */
    { 1295, 0x8ba87b3d, 613 }, /* vkCmdBindDescriptorSets2KHR */
    { 1323, 0x4c22d870, 100 }, /* vkCmdBindIndexBuffer */
    { 1344, 0xaf0aa842, 398 }, /* vkCmdBindIndexBuffer2 */
    { 1366, 0xce5c6793, 399 }, /* vkCmdBindIndexBuffer2KHR */
    { 1391, 0x6e6d5fa, 691 }, /* vkCmdBindIndexBuffer3KHR */
    { 1416, 0xa8f55bdd, 312 }, /* vkCmdBindInvocationMaskHUAWEI */
    { 1446, 0x3af9fd84, 87 }, /* vkCmdBindPipeline */
    { 1464, 0x353570d6, 160 }, /* vkCmdBindPipelineShaderGroupNV */
    { 1495, 0x268ef845, 673 }, /* vkCmdBindResourceHeapEXT */
    { 1520, 0x7bd507, 672 }, /* vkCmdBindSamplerHeapEXT */
    { 1544, 0x3e18f3f5, 594 }, /* vkCmdBindShadersEXT */
    { 1564, 0xbae753eb, 301 }, /* vkCmdBindShadingRateImageNV */
    { 1592, 0x563a67a1, 572 }, /* vkCmdBindTileMemoryQCOM */
    { 1616, 0xaff65093, 686 }, /* vkCmdBindTransformFeedbackBuffers2EXT */
    { 1654, 0x98fdb5cd, 293 }, /* vkCmdBindTransformFeedbackBuffersEXT */
    { 1691, 0xa9c83f1d, 101 }, /* vkCmdBindVertexBuffers */
    { 1714, 0x4c6b42ed, 400 }, /* vkCmdBindVertexBuffers2 */
    { 1738, 0x30a5f2ec, 401 }, /* vkCmdBindVertexBuffers2EXT */
    { 1765, 0xa3688ac7, 692 }, /* vkCmdBindVertexBuffers3KHR */
    { 1792, 0x331ebf89, 116 }, /* vkCmdBlitImage */
    { 1807, 0xb4185c21, 465 }, /* vkCmdBlitImage2 */
    { 1823, 0x785f984c, 466 }, /* vkCmdBlitImage2KHR */
    { 1842, 0x8e9d180a, 326 }, /* vkCmdBuildAccelerationStructureNV */
    { 1876, 0xb02d6bee, 377 }, /* vkCmdBuildAccelerationStructuresIndirectKHR */
    { 1920, 0x25a727dc, 376 }, /* vkCmdBuildAccelerationStructuresKHR */
    { 1956, 0x31e9c478, 339 }, /* vkCmdBuildClusterAccelerationStructureIndirectNV */
    { 2005, 0x331d3d07, 552 }, /* vkCmdBuildMicromapsEXT */
    { 2028, 0x119e6a50, 516 }, /* vkCmdBuildPartitionedAccelerationStructuresNV */
    { 2074, 0x93cb5cb8, 127 }, /* vkCmdClearAttachments */
    { 2096, 0xb4bc8d08, 125 }, /* vkCmdClearColorImage */
    { 2117, 0x4f88e4ba, 126 }, /* vkCmdClearDepthStencilImage */
    { 2145, 0x2a5f6f70, 510 }, /* vkCmdControlVideoCodingKHR */
    { 2172, 0xb7acd091, 635 }, /* vkCmdConvertCooperativeVectorMatrixNV */
    { 2210, 0xe2d4fe2c, 318 }, /* vkCmdCopyAccelerationStructureKHR */
    { 2244, 0x84ab5629, 317 }, /* vkCmdCopyAccelerationStructureNV */
    { 2277, 0x46b2a8a0, 320 }, /* vkCmdCopyAccelerationStructureToMemoryKHR */
    { 2319, 0xc939a0da, 114 }, /* vkCmdCopyBuffer */
    { 2335, 0xa419e608, 461 }, /* vkCmdCopyBuffer2 */
    { 2352, 0x90c5563d, 462 }, /* vkCmdCopyBuffer2KHR */
    { 2372, 0x929847e, 117 }, /* vkCmdCopyBufferToImage */
    { 2395, 0x4a8ce444, 467 }, /* vkCmdCopyBufferToImage2 */
    { 2419, 0x1e9f6861, 468 }, /* vkCmdCopyBufferToImage2KHR */
    { 2446, 0xe0e93ae2, 611 }, /* vkCmdCopyGpaSessionResultsAMD */
    { 2476, 0x278effa9, 115 }, /* vkCmdCopyImage */
    { 2491, 0x942b5301, 463 }, /* vkCmdCopyImage2 */
    { 2507, 0xdad52c6c, 464 }, /* vkCmdCopyImage2KHR */
    { 2526, 0x68cddbac, 118 }, /* vkCmdCopyImageToBuffer */
    { 2549, 0x83c9426, 469 }, /* vkCmdCopyImageToBuffer2 */
    { 2573, 0x2db6484f, 470 }, /* vkCmdCopyImageToBuffer2KHR */
    { 2600, 0xf31c0c78, 681 }, /* vkCmdCopyImageToMemoryKHR */
    { 2626, 0x9ceb9f8c, 120 }, /* vkCmdCopyMemoryIndirectKHR */
    { 2653, 0x329f3fc9, 119 }, /* vkCmdCopyMemoryIndirectNV */
    { 2679, 0x67fe25fa, 679 }, /* vkCmdCopyMemoryKHR */
    { 2698, 0xa76c5fd8, 322 }, /* vkCmdCopyMemoryToAccelerationStructureKHR */
    { 2740, 0x353874c2, 122 }, /* vkCmdCopyMemoryToImageIndirectKHR */
    { 2774, 0x42caec43, 121 }, /* vkCmdCopyMemoryToImageIndirectNV */
    { 2807, 0xd06207b0, 680 }, /* vkCmdCopyMemoryToImageKHR */
    { 2833, 0x47d6c41b, 559 }, /* vkCmdCopyMemoryToMicromapEXT */
    { 2862, 0x8c3811e7, 555 }, /* vkCmdCopyMicromapEXT */
    { 2883, 0x715dff1b, 557 }, /* vkCmdCopyMicromapToMemoryEXT */
    { 2912, 0xdee8c6d4, 140 }, /* vkCmdCopyQueryPoolResults */
    { 2938, 0xd39e6105, 684 }, /* vkCmdCopyQueryPoolResultsToMemoryKHR */
    { 2975, 0xbffb80d1, 654 }, /* vkCmdCopyTensorARM */
    { 2994, 0x29000809, 523 }, /* vkCmdCuLaunchKernelNVX */
    { 3017, 0xaec8bb10, 154 }, /* vkCmdDebugMarkerBeginEXT */
    { 3042, 0xd6a1433e, 155 }, /* vkCmdDebugMarkerEndEXT */
    { 3065, 0x173d440, 156 }, /* vkCmdDebugMarkerInsertEXT */
    { 3091, 0x5c38928d, 508 }, /* vkCmdDecodeVideoKHR */
    { 3111, 0x94c80f18, 517 }, /* vkCmdDecompressMemoryEXT */
    { 3136, 0x79cc8c1, 518 }, /* vkCmdDecompressMemoryIndirectCountEXT */
    { 3174, 0xca5aa668, 514 }, /* vkCmdDecompressMemoryIndirectCountNV */
    { 3211, 0xbb136ec9, 513 }, /* vkCmdDecompressMemoryNV */
    { 3235, 0xbd58e867, 108 }, /* vkCmdDispatch */
    { 3249, 0xfb767220, 209 }, /* vkCmdDispatchBase */
    { 3267, 0x402403e5, 210 }, /* vkCmdDispatchBaseKHR */
    { 3288, 0x25e3f65f, 663 }, /* vkCmdDispatchDataGraphARM */
    { 3314, 0xd6353005, 109 }, /* vkCmdDispatchIndirect */
    { 3336, 0xcfa5ae48, 699 }, /* vkCmdDispatchIndirect2KHR */
    { 3362, 0x5179be15, 636 }, /* vkCmdDispatchTileQCOM */
    { 3384, 0x9912c1a1, 102 }, /* vkCmdDraw */
    { 3394, 0x255fa5c8, 111 }, /* vkCmdDrawClusterHUAWEI */
    { 3417, 0x73efb906, 112 }, /* vkCmdDrawClusterIndirectHUAWEI */
    { 3448, 0xbe5a8058, 103 }, /* vkCmdDrawIndexed */
    { 3465, 0x94e7ed36, 107 }, /* vkCmdDrawIndexedIndirect */
    { 3490, 0xc9477419, 694 }, /* vkCmdDrawIndexedIndirect2KHR */
    { 3519, 0xb4acef41, 288 }, /* vkCmdDrawIndexedIndirectCount */
    { 3549, 0xc99cb04, 696 }, /* vkCmdDrawIndexedIndirectCount2KHR */
    { 3583, 0xc86e9287, 290 }, /* vkCmdDrawIndexedIndirectCountAMD */
    { 3616, 0xda9e8a2c, 289 }, /* vkCmdDrawIndexedIndirectCountKHR */
    { 3649, 0xe9ac41bf, 106 }, /* vkCmdDrawIndirect */
    { 3667, 0xc2bfd842, 693 }, /* vkCmdDrawIndirect2KHR */
    { 3689, 0x80d2a8f7, 689 }, /* vkCmdDrawIndirectByteCount2EXT */
    { 3720, 0x80c3b089, 298 }, /* vkCmdDrawIndirectByteCountEXT */
    { 3750, 0x40079990, 285 }, /* vkCmdDrawIndirectCount */
    { 3773, 0xc1011cb3, 695 }, /* vkCmdDrawIndirectCount2KHR */
    { 3800, 0xe5ad0a50, 287 }, /* vkCmdDrawIndirectCountAMD */
    { 3826, 0xf7dd01f5, 286 }, /* vkCmdDrawIndirectCountKHR */
    { 3852, 0xfa045961, 307 }, /* vkCmdDrawMeshTasksEXT */
    { 3874, 0xa70104fd, 697 }, /* vkCmdDrawMeshTasksIndirect2EXT */
    { 3905, 0xeade0f78, 698 }, /* vkCmdDrawMeshTasksIndirectCount2EXT */
    { 3941, 0xac310210, 309 }, /* vkCmdDrawMeshTasksIndirectCountEXT */
    { 3976, 0xf21f391, 306 }, /* vkCmdDrawMeshTasksIndirectCountNV */
    { 4010, 0x6c0d4cb3, 308 }, /* vkCmdDrawMeshTasksIndirectEXT */
    { 4040, 0xaecd0a06, 305 }, /* vkCmdDrawMeshTasksIndirectNV */
    { 4069, 0xfba21ac8, 304 }, /* vkCmdDrawMeshTasksNV */
    { 4090, 0xa6c231d9, 104 }, /* vkCmdDrawMultiEXT */
    { 4108, 0xb6bd0f40, 105 }, /* vkCmdDrawMultiIndexedEXT */
    { 4133, 0x552d044d, 512 }, /* vkCmdEncodeVideoKHR */
    { 4153, 0x18c8217d, 136 }, /* vkCmdEndConditionalRenderingEXT */
    { 4185, 0x29875911, 265 }, /* vkCmdEndDebugUtilsLabelEXT */
    { 4212, 0xfa8be34c, 607 }, /* vkCmdEndGpaSampleAMD */
    { 4233, 0x54a40b5c, 605 }, /* vkCmdEndGpaSessionAMD */
    { 4255, 0xea4a5ecb, 638 }, /* vkCmdEndPerTileExecutionQCOM */
    { 4284, 0xd556fd22, 134 }, /* vkCmdEndQuery */
    { 4298, 0xd5c2f48a, 297 }, /* vkCmdEndQueryIndexedEXT */
    { 4322, 0xdcdb0235, 144 }, /* vkCmdEndRenderPass */
    { 4341, 0x1cbf9115, 275 }, /* vkCmdEndRenderPass2 */
    { 4361, 0x57eebe78, 276 }, /* vkCmdEndRenderPass2KHR */
    { 4384, 0x22c5e6f6, 545 }, /* vkCmdEndRendering */
    { 4402, 0x1772dc65, 547 }, /* vkCmdEndRendering2EXT */
    { 4424, 0x51ab05d9, 546 }, /* vkCmdEndRendering2KHR */
    { 4446, 0xabf9ff, 548 }, /* vkCmdEndRenderingKHR */
    { 4467, 0x324f9f61, 644 }, /* vkCmdEndShaderInstrumentationARM */
    { 4500, 0x1ec5b152, 688 }, /* vkCmdEndTransformFeedback2EXT */
    { 4530, 0xf008d706, 295 }, /* vkCmdEndTransformFeedbackEXT */
    { 4559, 0xa5c55b4e, 511 }, /* vkCmdEndVideoCodingKHR */
    { 4582, 0x9eaabe40, 145 }, /* vkCmdExecuteCommands */
    { 4603, 0xda10331a, 164 }, /* vkCmdExecuteGeneratedCommandsEXT */
    { 4636, 0xe02372d7, 158 }, /* vkCmdExecuteGeneratedCommandsNV */
    { 4668, 0x5bdd2ae0, 124 }, /* vkCmdFillBuffer */
    { 4684, 0x64554264, 683 }, /* vkCmdFillMemoryKHR */
    { 4703, 0xce6aa7d1, 266 }, /* vkCmdInsertDebugUtilsLabelEXT */
    { 4733, 0x2eeec2f9, 143 }, /* vkCmdNextSubpass */
    { 4750, 0xd4fc131, 273 }, /* vkCmdNextSubpass2 */
    { 4768, 0x25b621bc, 274 }, /* vkCmdNextSubpass2KHR */
    { 4789, 0x9fa9b32c, 578 }, /* vkCmdOpticalFlowExecuteNV */
    { 4815, 0x97fccfe8, 132 }, /* vkCmdPipelineBarrier */
    { 4836, 0x43d8c70a, 484 }, /* vkCmdPipelineBarrier2 */
    { 4858, 0x9654ba0b, 485 }, /* vkCmdPipelineBarrier2KHR */
    { 4883, 0x2e93edd3, 165 }, /* vkCmdPreprocessGeneratedCommandsEXT */
    { 4919, 0x26eff1e6, 159 }, /* vkCmdPreprocessGeneratedCommandsNV */
    { 4954, 0xb1c6b468, 141 }, /* vkCmdPushConstants */
    { 4973, 0xd1039e8a, 614 }, /* vkCmdPushConstants2 */
    { 4993, 0xf62a6e8b, 615 }, /* vkCmdPushConstants2KHR */
    { 5016, 0x69b08588, 674 }, /* vkCmdPushDataEXT */
    { 5033, 0xe924e004, 173 }, /* vkCmdPushDescriptorSet */
    { 5056, 0x826ec70e, 616 }, /* vkCmdPushDescriptorSet2 */
    { 5080, 0x1ad873a7, 617 }, /* vkCmdPushDescriptorSet2KHR */
    { 5107, 0xf17232a1, 174 }, /* vkCmdPushDescriptorSetKHR */
    { 5133, 0xf173c624, 217 }, /* vkCmdPushDescriptorSetWithTemplate */
    { 5168, 0xda4067ee, 618 }, /* vkCmdPushDescriptorSetWithTemplate2 */
    { 5204, 0x3c106dc7, 619 }, /* vkCmdPushDescriptorSetWithTemplate2KHR */
    { 5243, 0x3d528981, 218 }, /* vkCmdPushDescriptorSetWithTemplateKHR */
    { 5281, 0x4fccce28, 130 }, /* vkCmdResetEvent */
    { 5297, 0x735fc6ca, 480 }, /* vkCmdResetEvent2 */
    { 5314, 0x950a204b, 481 }, /* vkCmdResetEvent2KHR */
    { 5334, 0x2f614082, 138 }, /* vkCmdResetQueryPool */
    { 5354, 0x671bb594, 128 }, /* vkCmdResolveImage */
    { 5372, 0xe79d80fe, 471 }, /* vkCmdResolveImage2 */
    { 5391, 0x9fea6337, 472 }, /* vkCmdResolveImage2KHR */
    { 5413, 0x8344384a, 427 }, /* vkCmdSetAlphaToCoverageEnableEXT */
    { 5446, 0x246d31e4, 428 }, /* vkCmdSetAlphaToOneEnableEXT */
    { 5474, 0xd6cf1e45, 89 }, /* vkCmdSetAttachmentFeedbackLoopEnableEXT */
    { 5514, 0x1c989dfb, 94 }, /* vkCmdSetBlendConstants */
    { 5537, 0x4331556d, 291 }, /* vkCmdSetCheckpointNV */
    { 5558, 0xcd76e1c0, 303 }, /* vkCmdSetCoarseSampleOrderNV */
    { 5586, 0x67adf3f4, 438 }, /* vkCmdSetColorBlendAdvancedEXT */
    { 5616, 0x7242b4bb, 430 }, /* vkCmdSetColorBlendEnableEXT */
    { 5644, 0x6828ae82, 431 }, /* vkCmdSetColorBlendEquationEXT */
    { 5674, 0x5402d31d, 477 }, /* vkCmdSetColorWriteEnableEXT */
    { 5702, 0x80a5014c, 432 }, /* vkCmdSetColorWriteMaskEXT */
    { 5728, 0x47c44cb7, 669 }, /* vkCmdSetComputeOccupancyPriorityNV */
    { 5763, 0xbd4e36a9, 434 }, /* vkCmdSetConservativeRasterizationModeEXT */
    { 5804, 0xd1185034, 447 }, /* vkCmdSetCoverageModulationModeNV */
    { 5837, 0x58921b18, 448 }, /* vkCmdSetCoverageModulationTableEnableNV */
    { 5877, 0xc32a1ef5, 449 }, /* vkCmdSetCoverageModulationTableNV */
    { 5911, 0x64509d31, 451 }, /* vkCmdSetCoverageReductionModeNV */
    { 5943, 0xb956bfce, 445 }, /* vkCmdSetCoverageToColorEnableNV */
    { 5975, 0xf35aaa00, 446 }, /* vkCmdSetCoverageToColorLocationNV */
    { 6009, 0xcf691c42, 388 }, /* vkCmdSetCullMode */
    { 6026, 0xb7fcea1f, 389 }, /* vkCmdSetCullModeEXT */
    { 6046, 0x30f14d07, 93 }, /* vkCmdSetDepthBias */
    { 6064, 0xc3139716, 582 }, /* vkCmdSetDepthBias2EXT */
    { 6086, 0xc07e1b1a, 417 }, /* vkCmdSetDepthBiasEnable */
    { 6110, 0x5d604307, 418 }, /* vkCmdSetDepthBiasEnableEXT */
    { 6137, 0x7b3a8a63, 95 }, /* vkCmdSetDepthBounds */
    { 6157, 0xe72cce0, 408 }, /* vkCmdSetDepthBoundsTestEnable */
    { 6187, 0x3f2ddb1, 409 }, /* vkCmdSetDepthBoundsTestEnableEXT */
    { 6220, 0x6a365461, 423 }, /* vkCmdSetDepthClampEnableEXT */
    { 6248, 0x1163e9a9, 631 }, /* vkCmdSetDepthClampRangeEXT */
    { 6275, 0x88cb8180, 436 }, /* vkCmdSetDepthClipEnableEXT */
    { 6302, 0x69601111, 442 }, /* vkCmdSetDepthClipNegativeOneToOneEXT */
    { 6339, 0xda98add0, 406 }, /* vkCmdSetDepthCompareOp */
    { 6362, 0x2f377e41, 407 }, /* vkCmdSetDepthCompareOpEXT */
    { 6388, 0x68666de3, 402 }, /* vkCmdSetDepthTestEnable */
    { 6412, 0x57c5efe6, 403 }, /* vkCmdSetDepthTestEnableEXT */
    { 6439, 0x373bda6c, 404 }, /* vkCmdSetDepthWriteEnable */
    { 6464, 0xbe217905, 405 }, /* vkCmdSetDepthWriteEnableEXT */
    { 6492, 0x1791e0b3, 620 }, /* vkCmdSetDescriptorBufferOffsets2EXT */
    { 6528, 0x6cef26ad, 528 }, /* vkCmdSetDescriptorBufferOffsetsEXT */
    { 6563, 0xaecdae87, 204 }, /* vkCmdSetDeviceMask */
    { 6582, 0xfbb79356, 205 }, /* vkCmdSetDeviceMaskKHR */
    { 6604, 0x64df188b, 224 }, /* vkCmdSetDiscardRectangleEXT */
    { 6632, 0xb7bee320, 225 }, /* vkCmdSetDiscardRectangleEnableEXT */
    { 6666, 0xc1898148, 226 }, /* vkCmdSetDiscardRectangleModeEXT */
    { 6698, 0x1385e783, 701 }, /* vkCmdSetDispatchParametersARM */
    { 6728, 0xe257f075, 129 }, /* vkCmdSetEvent */
    { 6742, 0x3d5620d5, 478 }, /* vkCmdSetEvent2 */
    { 6757, 0xa3c714b8, 479 }, /* vkCmdSetEvent2KHR */
    { 6775, 0x938290a6, 300 }, /* vkCmdSetExclusiveScissorEnableNV */
    { 6808, 0xb2537e63, 299 }, /* vkCmdSetExclusiveScissorNV */
    { 6835, 0x22d38855, 435 }, /* vkCmdSetExtraPrimitiveOverestimationSizeEXT */
    { 6879, 0x7670296e, 474 }, /* vkCmdSetFragmentShadingRateEnumNV */
    { 6913, 0x4c696cd8, 473 }, /* vkCmdSetFragmentShadingRateKHR */
    { 6944, 0x4cd999a9, 390 }, /* vkCmdSetFrontFace */
    { 6962, 0xa7a7a090, 391 }, /* vkCmdSetFrontFaceEXT */
    { 6983, 0x82fb3a20, 440 }, /* vkCmdSetLineRasterizationModeEXT */
    { 7016, 0xd72bc058, 372 }, /* vkCmdSetLineStipple */
    { 7036, 0xbdaa62f9, 374 }, /* vkCmdSetLineStippleEXT */
    { 7059, 0x36f713ae, 441 }, /* vkCmdSetLineStippleEnableEXT */
    { 7088, 0xf7e28c6d, 373 }, /* vkCmdSetLineStippleKHR */
    { 7111, 0x32282165, 92 }, /* vkCmdSetLineWidth */
    { 7129, 0x7689581f, 419 }, /* vkCmdSetLogicOpEXT */
    { 7148, 0xc1fbf774, 429 }, /* vkCmdSetLogicOpEnableEXT */
    { 7173, 0x58604abc, 414 }, /* vkCmdSetPatchControlPointsEXT */
    { 7203, 0x4eb21af9, 360 }, /* vkCmdSetPerformanceMarkerINTEL */
    { 7234, 0x30d793c7, 362 }, /* vkCmdSetPerformanceOverrideINTEL */
    { 7267, 0xc50b03a9, 361 }, /* vkCmdSetPerformanceStreamMarkerINTEL */
    { 7304, 0x966edf9, 424 }, /* vkCmdSetPolygonModeEXT */
    { 7327, 0x2b3504c0, 420 }, /* vkCmdSetPrimitiveRestartEnable */
    { 7358, 0x28d998d1, 421 }, /* vkCmdSetPrimitiveRestartEnableEXT */
    { 7392, 0xb5ad63f4, 88 }, /* vkCmdSetPrimitiveRestartIndexEXT */
    { 7425, 0xb9524b01, 392 }, /* vkCmdSetPrimitiveTopology */
    { 7451, 0x1dacaf8, 393 }, /* vkCmdSetPrimitiveTopologyEXT */
    { 7480, 0x710ab2e2, 439 }, /* vkCmdSetProvokingVertexModeEXT */
    { 7511, 0xbe15d782, 425 }, /* vkCmdSetRasterizationSamplesEXT */
    { 7543, 0xc1c028f, 433 }, /* vkCmdSetRasterizationStreamEXT */
    { 7574, 0x81319b79, 415 }, /* vkCmdSetRasterizerDiscardEnable */
    { 7606, 0x1f7bb40, 416 }, /* vkCmdSetRasterizerDiscardEnableEXT */
    { 7641, 0xd056ef9b, 342 }, /* vkCmdSetRayTracingPipelineStackSizeKHR */
    { 7680, 0x6704b8dc, 627 }, /* vkCmdSetRenderingAttachmentLocations */
    { 7717, 0xea1f4189, 628 }, /* vkCmdSetRenderingAttachmentLocationsKHR */
    { 7757, 0x8a2407b1, 629 }, /* vkCmdSetRenderingInputAttachmentIndices */
    { 7797, 0x96697f3c, 630 }, /* vkCmdSetRenderingInputAttachmentIndicesKHR */
    { 7840, 0xb8eb12ff, 452 }, /* vkCmdSetRepresentativeFragmentTestEnableNV */
    { 7883, 0xa9e2c72, 227 }, /* vkCmdSetSampleLocationsEXT */
    { 7910, 0x3b21c717, 437 }, /* vkCmdSetSampleLocationsEnableEXT */
    { 7943, 0xd997f166, 426 }, /* vkCmdSetSampleMaskEXT */
    { 7965, 0x48f28c7f, 91 }, /* vkCmdSetScissor */
    { 7981, 0x159097b2, 396 }, /* vkCmdSetScissorWithCount */
    { 8006, 0xf349b42f, 397 }, /* vkCmdSetScissorWithCountEXT */
    { 8034, 0x9300b169, 450 }, /* vkCmdSetShadingRateImageEnableNV */
    { 8067, 0xa8f534e2, 96 }, /* vkCmdSetStencilCompareMask */
    { 8094, 0x43020f38, 412 }, /* vkCmdSetStencilOp */
    { 8112, 0xbb885f19, 413 }, /* vkCmdSetStencilOpEXT */
    { 8133, 0x83e2b024, 98 }, /* vkCmdSetStencilReference */
    { 8158, 0x63fedc5c, 410 }, /* vkCmdSetStencilTestEnable */
    { 8184, 0x16cc6095, 411 }, /* vkCmdSetStencilTestEnableEXT */
    { 8213, 0xe7c4b134, 97 }, /* vkCmdSetStencilWriteMask */
    { 8238, 0xf2d7ac79, 422 }, /* vkCmdSetTessellationDomainOriginEXT */
    { 8274, 0x9dd954c8, 476 }, /* vkCmdSetVertexInputEXT */
    { 8297, 0x53d6c2b, 90 }, /* vkCmdSetViewport */
    { 8314, 0x54d063a4, 302 }, /* vkCmdSetViewportShadingRatePaletteNV */
    { 8351, 0x5118219b, 444 }, /* vkCmdSetViewportSwizzleNV */
    { 8377, 0xad68ff96, 443 }, /* vkCmdSetViewportWScalingEnableNV */
    { 8410, 0x60ee2453, 223 }, /* vkCmdSetViewportWScalingNV */
    { 8437, 0xbdea58a6, 394 }, /* vkCmdSetViewportWithCount */
    { 8463, 0xa3d72e5b, 395 }, /* vkCmdSetViewportWithCountEXT */
    { 8492, 0xf2c7909d, 110 }, /* vkCmdSubpassShadingHUAWEI */
    { 8518, 0x519b0602, 337 }, /* vkCmdTraceRaysIndirect2KHR */
    { 8545, 0xaf8c1f1e, 336 }, /* vkCmdTraceRaysIndirectKHR */
    { 8571, 0x5eb65f0c, 328 }, /* vkCmdTraceRaysKHR */
    { 8589, 0xe8687c49, 329 }, /* vkCmdTraceRaysNV */
    { 8606, 0xd2986b5e, 123 }, /* vkCmdUpdateBuffer */
    { 8624, 0xf34e5916, 682 }, /* vkCmdUpdateMemoryKHR */
    { 8645, 0xa195186, 113 }, /* vkCmdUpdatePipelineIndirectBufferNV */
    { 8681, 0x3b9346b3, 131 }, /* vkCmdWaitEvents */
    { 8697, 0xcd17b527, 482 }, /* vkCmdWaitEvents2 */
    { 8714, 0x8c98fdb6, 483 }, /* vkCmdWaitEvents2KHR */
    { 8734, 0x9ecb3888, 324 }, /* vkCmdWriteAccelerationStructuresPropertiesKHR */
    { 8780, 0xd2925ead, 325 }, /* vkCmdWriteAccelerationStructuresPropertiesNV */
    { 8825, 0xe277f952, 490 }, /* vkCmdWriteBufferMarker2AMD */
    { 8852, 0x447be82c, 268 }, /* vkCmdWriteBufferMarkerAMD */
    { 8878, 0x806f96f8, 690 }, /* vkCmdWriteMarkerToMemoryAMD */
    { 8906, 0xa6efed5d, 561 }, /* vkCmdWriteMicromapsPropertiesEXT */
    { 8939, 0xec4d324c, 139 }, /* vkCmdWriteTimestamp */
    { 8959, 0xcff32086, 488 }, /* vkCmdWriteTimestamp2 */
    { 8980, 0xa43a02ef, 489 }, /* vkCmdWriteTimestamp2KHR */
    { 9004, 0xbad693ed, 310 }, /* vkCompileDeferredNV */
    { 9024, 0x38bc6611, 634 }, /* vkConvertCooperativeVectorMatrixNV */
    { 9059, 0x45e623ac, 319 }, /* vkCopyAccelerationStructureKHR */
    { 9090, 0x9726ae20, 321 }, /* vkCopyAccelerationStructureToMemoryKHR */
    { 9129, 0x24e0b8e7, 496 }, /* vkCopyImageToImage */
    { 9148, 0x95dd682, 497 }, /* vkCopyImageToImageEXT */
    { 9170, 0xa0243195, 494 }, /* vkCopyImageToMemory */
    { 9190, 0xbd2a2884, 495 }, /* vkCopyImageToMemoryEXT */
    { 9213, 0xf7e06558, 323 }, /* vkCopyMemoryToAccelerationStructureKHR */
    { 9252, 0xf178699d, 492 }, /* vkCopyMemoryToImage */
    { 9272, 0x9a7023bc, 493 }, /* vkCopyMemoryToImageEXT */
    { 9295, 0xa8f9ba9b, 560 }, /* vkCopyMemoryToMicromapEXT */
    { 9321, 0x5aa2e867, 556 }, /* vkCopyMicromapEXT */
    { 9339, 0xd280f59b, 558 }, /* vkCopyMicromapToMemoryEXT */
    { 9365, 0x8b63383d, 700 }, /* vkCreateAccelerationStructure2KHR */
    { 9399, 0x30dceabb, 375 }, /* vkCreateAccelerationStructureKHR */
    { 9432, 0x9dc98a12, 311 }, /* vkCreateAccelerationStructureNV */
    { 9464, 0x7d4282b9, 36 }, /* vkCreateBuffer */
    { 9479, 0xaba0b50, 538 }, /* vkCreateBufferCollectionFUCHSIA */
    { 9511, 0x925bd256, 38 }, /* vkCreateBufferView */
    { 9530, 0x820fe476, 79 }, /* vkCreateCommandPool */
    { 9550, 0xf70c85eb, 57 }, /* vkCreateComputePipelines */
    { 9575, 0x9bff3a5d, 520 }, /* vkCreateCuFunctionNVX */
    { 9597, 0xf3c09939, 519 }, /* vkCreateCuModuleNVX */
    { 9617, 0x2bf0baab, 658 }, /* vkCreateDataGraphPipelineSessionARM */
    { 9653, 0xc931175e, 657 }, /* vkCreateDataGraphPipelinesARM */
    { 9683, 0x3f5d1a36, 380 }, /* vkCreateDeferredOperationKHR */
    { 9712, 0xfb95a8a4, 66 }, /* vkCreateDescriptorPool */
    { 9735, 0x3c14cc74, 64 }, /* vkCreateDescriptorSetLayout */
    { 9763, 0xad3ce733, 211 }, /* vkCreateDescriptorUpdateTemplate */
    { 9796, 0x5189488a, 212 }, /* vkCreateDescriptorUpdateTemplateKHR */
    { 9832, 0xe7188731, 26 }, /* vkCreateEvent */
    { 9846, 0x9e9dc9ee, 639 }, /* vkCreateExternalComputeQueueNV */
    { 9877, 0x958af968, 19 }, /* vkCreateFence */
    { 9891, 0x887a38c4, 72 }, /* vkCreateFramebuffer */
    { 9911, 0x17387fb1, 600 }, /* vkCreateGpaSessionAMD */
    { 9933, 0x4b59f96d, 56 }, /* vkCreateGraphicsPipelines */
    { 9959, 0x652128c2, 40 }, /* vkCreateImage */
    { 9973, 0xdce077ff, 43 }, /* vkCreateImageView */
    { 9991, 0x82e3e90, 167 }, /* vkCreateIndirectCommandsLayoutEXT */
    { 10025, 0x3bc09b11, 162 }, /* vkCreateIndirectCommandsLayoutNV */
    { 10058, 0x70e416c8, 169 }, /* vkCreateIndirectExecutionSetEXT */
    { 10090, 0x4dea1af0, 551 }, /* vkCreateMicromapEXT */
    { 10110, 0x7a80ce69, 575 }, /* vkCreateOpticalFlowSessionNV */
    { 10139, 0xbb50e82b, 51 }, /* vkCreatePipelineBinariesKHR */
    { 10167, 0xcbf6489f, 47 }, /* vkCreatePipelineCache */
    { 10189, 0x451ef1ed, 60 }, /* vkCreatePipelineLayout */
    { 10212, 0xd1450d02, 453 }, /* vkCreatePrivateDataSlot */
    { 10236, 0xc06d475f, 454 }, /* vkCreatePrivateDataSlotEXT */
    { 10263, 0x5edcd92b, 31 }, /* vkCreateQueryPool */
    { 10281, 0x28847755, 335 }, /* vkCreateRayTracingPipelinesKHR */
    { 10312, 0x12bca48, 334 }, /* vkCreateRayTracingPipelinesNV */
    { 10342, 0x109a9c18, 74 }, /* vkCreateRenderPass */
    { 10361, 0x46b16d5a, 269 }, /* vkCreateRenderPass2 */
    { 10381, 0xfa16043b, 270 }, /* vkCreateRenderPass2KHR */
    { 10404, 0x13cf03f, 62 }, /* vkCreateSampler */
    { 10420, 0xe6a58c26, 240 }, /* vkCreateSamplerYcbcrConversion */
    { 10451, 0x7482104f, 241 }, /* vkCreateSamplerYcbcrConversionKHR */
    { 10485, 0xf2065e5b, 24 }, /* vkCreateSemaphore */
    { 10503, 0x42c0db54, 641 }, /* vkCreateShaderInstrumentationARM */
    { 10536, 0xa0d3cea2, 45 }, /* vkCreateShaderModule */
    { 10557, 0x5d0a2b04, 591 }, /* vkCreateShadersEXT */
    { 10576, 0x47655c4a, 146 }, /* vkCreateSharedSwapchainsKHR */
    { 10604, 0xcdefcaa8, 147 }, /* vkCreateSwapchainKHR */
    { 10625, 0x8ad2338a, 647 }, /* vkCreateTensorARM */
    { 10643, 0xccdbfb15, 649 }, /* vkCreateTensorViewARM */
    { 10665, 0x591d7ed9, 245 }, /* vkCreateValidationCacheEXT */
    { 10692, 0xcddb2969, 500 }, /* vkCreateVideoSessionKHR */
    { 10716, 0x83987bd7, 502 }, /* vkCreateVideoSessionParametersKHR */
    { 10750, 0xe206fb25, 152 }, /* vkDebugMarkerSetObjectNameEXT */
    { 10780, 0x30799448, 153 }, /* vkDebugMarkerSetObjectTagEXT */
    { 10809, 0x8c8648b8, 384 }, /* vkDeferredOperationJoinKHR */
    { 10836, 0x3eccc207, 313 }, /* vkDestroyAccelerationStructureKHR */
    { 10870, 0x693f9d26, 314 }, /* vkDestroyAccelerationStructureNV */
    { 10903, 0x94a07a45, 37 }, /* vkDestroyBuffer */
    { 10919, 0xd6301e64, 541 }, /* vkDestroyBufferCollectionFUCHSIA */
    { 10952, 0x98b27962, 39 }, /* vkDestroyBufferView */
    { 10972, 0xd5d83a0a, 80 }, /* vkDestroyCommandPool */
    { 10993, 0x111b9831, 522 }, /* vkDestroyCuFunctionNVX */
    { 11016, 0x4788eecd, 521 }, /* vkDestroyCuModuleNVX */
    { 11037, 0x84eb023f, 662 }, /* vkDestroyDataGraphPipelineSessionARM */
    { 11074, 0x7d549a02, 381 }, /* vkDestroyDeferredOperationKHR */
    { 11104, 0x47bdaf30, 67 }, /* vkDestroyDescriptorPool */
    { 11128, 0xa4227b08, 65 }, /* vkDestroyDescriptorSetLayout */
    { 11157, 0xbb2cbe7f, 213 }, /* vkDestroyDescriptorUpdateTemplate */
    { 11191, 0xaa83901e, 214 }, /* vkDestroyDescriptorUpdateTemplateKHR */
    { 11228, 0x1fbcc9cb, 1 }, /* vkDestroyDevice */
    { 11244, 0x4df27c05, 27 }, /* vkDestroyEvent */
    { 11259, 0xd8ea9f7a, 640 }, /* vkDestroyExternalComputeQueueNV */
    { 11291, 0xfc64ee3c, 20 }, /* vkDestroyFence */
    { 11306, 0xdc428e58, 73 }, /* vkDestroyFramebuffer */
    { 11327, 0x8c54dd85, 601 }, /* vkDestroyGpaSessionAMD */
    { 11350, 0xcbfb1d96, 41 }, /* vkDestroyImage */
    { 11365, 0xb5853953, 44 }, /* vkDestroyImageView */
    { 11384, 0x3e5251e4, 168 }, /* vkDestroyIndirectCommandsLayoutEXT */
    { 11419, 0x49b0725d, 163 }, /* vkDestroyIndirectCommandsLayoutNV */
    { 11453, 0x3c5a29dc, 170 }, /* vkDestroyIndirectExecutionSetEXT */
    { 11486, 0xa1b27084, 554 }, /* vkDestroyMicromapEXT */
    { 11507, 0xb8784e35, 576 }, /* vkDestroyOpticalFlowSessionNV */
    { 11537, 0x6aac68af, 59 }, /* vkDestroyPipeline */
    { 11555, 0x8a979065, 52 }, /* vkDestroyPipelineBinaryKHR */
    { 11582, 0x4112a673, 48 }, /* vkDestroyPipelineCache */
    { 11605, 0x9146f879, 61 }, /* vkDestroyPipelineLayout */
    { 11629, 0x9fc42716, 455 }, /* vkDestroyPrivateDataSlot */
    { 11654, 0xe18d5d6b, 456 }, /* vkDestroyPrivateDataSlotEXT */
    { 11682, 0x37819a7f, 32 }, /* vkDestroyQueryPool */
    { 11701, 0x16f14324, 75 }, /* vkDestroyRenderPass */
    { 11721, 0x3b645153, 63 }, /* vkDestroySampler */
    { 11738, 0x20f261b2, 242 }, /* vkDestroySamplerYcbcrConversion */
    { 11770, 0xaaa623a3, 243 }, /* vkDestroySamplerYcbcrConversionKHR */
    { 11805, 0xcaab1faf, 25 }, /* vkDestroySemaphore */
    { 11824, 0x9def5f27, 592 }, /* vkDestroyShaderEXT */
    { 11843, 0x50b0b2a0, 642 }, /* vkDestroyShaderInstrumentationARM */
    { 11877, 0x2d77af6e, 46 }, /* vkDestroyShaderModule */
    { 11899, 0x5a93ab74, 148 }, /* vkDestroySwapchainKHR */
    { 11921, 0x6376f4de, 648 }, /* vkDestroyTensorARM */
    { 11940, 0x41f858e9, 650 }, /* vkDestroyTensorViewARM */
    { 11963, 0x7a3d94e5, 246 }, /* vkDestroyValidationCacheEXT */
    { 11991, 0x9c5a437d, 501 }, /* vkDestroyVideoSessionKHR */
    { 12016, 0xb9bc8f2b, 505 }, /* vkDestroyVideoSessionParametersKHR */
    { 12051, 0xd46c5f24, 5 }, /* vkDeviceWaitIdle */
    { 12068, 0xdbb064, 194 }, /* vkDisplayPowerControlEXT */
    { 12093, 0xaffb5725, 85 }, /* vkEndCommandBuffer */
    { 12112, 0xa59ad883, 571 }, /* vkExportMetalObjectsEXT */
    { 12136, 0xff52f051, 10 }, /* vkFlushMappedMemoryRanges */
    { 12162, 0xb9db2b91, 83 }, /* vkFreeCommandBuffers */
    { 12183, 0x7a1347b1, 70 }, /* vkFreeDescriptorSets */
    { 12204, 0x8f6f838a, 7 }, /* vkFreeMemory */
    { 12217, 0x4dbe8d2f, 475 }, /* vkGetAccelerationStructureBuildSizesKHR */
    { 12257, 0x1a50de81, 379 }, /* vkGetAccelerationStructureDeviceAddressKHR */
    { 12300, 0xd26f255a, 333 }, /* vkGetAccelerationStructureHandleNV */
    { 12335, 0x5d79203, 315 }, /* vkGetAccelerationStructureMemoryRequirementsNV */
    { 12382, 0x2c8ad5, 534 }, /* vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT */
    { 12439, 0xb891b5e, 283 }, /* vkGetAndroidHardwareBufferPropertiesANDROID */
    { 12483, 0x14b1e93d, 542 }, /* vkGetBufferCollectionPropertiesFUCHSIA */
    { 12522, 0x7022f0cd, 355 }, /* vkGetBufferDeviceAddress */
    { 12547, 0x3703280c, 357 }, /* vkGetBufferDeviceAddressEXT */
    { 12575, 0x713b5180, 356 }, /* vkGetBufferDeviceAddressKHR */
    { 12603, 0xab98422a, 13 }, /* vkGetBufferMemoryRequirements */
    { 12633, 0xd1fd0638, 228 }, /* vkGetBufferMemoryRequirements2 */
    { 12664, 0x78dbe98d, 229 }, /* vkGetBufferMemoryRequirements2KHR */
    { 12698, 0x2a5545a0, 353 }, /* vkGetBufferOpaqueCaptureAddress */
    { 12730, 0xddac1c65, 354 }, /* vkGetBufferOpaqueCaptureAddressKHR */
    { 12765, 0x82935b24, 530 }, /* vkGetBufferOpaqueCaptureDescriptorDataEXT */
    { 12807, 0xcf3070fe, 258 }, /* vkGetCalibratedTimestampsEXT */
    { 12836, 0x9689a72, 257 }, /* vkGetCalibratedTimestampsKHR */
    { 12865, 0xcebf7abe, 338 }, /* vkGetClusterAccelerationStructureBuildSizesNV */
    { 12911, 0xc91148f5, 664 }, /* vkGetDataGraphPipelineAvailablePropertiesARM */
    { 12956, 0x34bd8e54, 665 }, /* vkGetDataGraphPipelinePropertiesARM */
    { 12992, 0x7f95d15a, 659 }, /* vkGetDataGraphPipelineSessionBindPointRequirementsARM */
    { 13046, 0xce931622, 660 }, /* vkGetDataGraphPipelineSessionMemoryRequirementsARM */
    { 13097, 0x7d902967, 382 }, /* vkGetDeferredOperationMaxConcurrencyKHR */
    { 13137, 0xf2144be9, 383 }, /* vkGetDeferredOperationResultKHR */
    { 13169, 0x1e9d8271, 526 }, /* vkGetDescriptorEXT */
    { 13188, 0x19faddac, 550 }, /* vkGetDescriptorSetHostMappingVALVE */
    { 13223, 0xbb130e35, 525 }, /* vkGetDescriptorSetLayoutBindingOffsetEXT */
    { 13264, 0x5485f810, 549 }, /* vkGetDescriptorSetLayoutHostMappingInfoVALVE */
    { 13309, 0x9ea2493c, 524 }, /* vkGetDescriptorSetLayoutSizeEXT */
    { 13341, 0xfeac9573, 249 }, /* vkGetDescriptorSetLayoutSupport */
    { 13373, 0xd7e44a, 250 }, /* vkGetDescriptorSetLayoutSupportKHR */
    { 13408, 0xe86334c9, 340 }, /* vkGetDeviceAccelerationStructureCompatibilityKHR */
    { 13457, 0xec1a1918, 234 }, /* vkGetDeviceBufferMemoryRequirements */
    { 13493, 0xa56ac1ad, 235 }, /* vkGetDeviceBufferMemoryRequirementsKHR */
    { 13532, 0x796e896f, 346 }, /* vkGetDeviceCombinedImageSamplerIndexNVX */
    { 13572, 0xe40c9a47, 581 }, /* vkGetDeviceFaultDebugInfoKHR */
    { 13601, 0x8d21a400, 579 }, /* vkGetDeviceFaultInfoEXT */
    { 13625, 0x4d9711d1, 580 }, /* vkGetDeviceFaultReportsKHR */
    { 13652, 0x2e218c10, 198 }, /* vkGetDeviceGroupPeerMemoryFeatures */
    { 13687, 0xa3809375, 199 }, /* vkGetDeviceGroupPeerMemoryFeaturesKHR */
    { 13725, 0xf72c87d4, 206 }, /* vkGetDeviceGroupPresentCapabilitiesKHR */
    { 13764, 0x41b28e81, 347 }, /* vkGetDeviceGroupSurfacePresentModes2EXT */
    { 13804, 0x6b9448c3, 207 }, /* vkGetDeviceGroupSurfacePresentModesKHR */
    { 13843, 0x5f18b6e5, 236 }, /* vkGetDeviceImageMemoryRequirements */
    { 13878, 0x3a2c5528, 237 }, /* vkGetDeviceImageMemoryRequirementsKHR */
    { 13916, 0x1ac18abd, 238 }, /* vkGetDeviceImageSparseMemoryRequirements */
    { 13957, 0xb8906110, 239 }, /* vkGetDeviceImageSparseMemoryRequirementsKHR */
    { 14001, 0x6eafea14, 585 }, /* vkGetDeviceImageSubresourceLayout */
    { 14035, 0x57561f11, 586 }, /* vkGetDeviceImageSubresourceLayoutKHR */
    { 14072, 0x46e38db5, 12 }, /* vkGetDeviceMemoryCommitment */
    { 14100, 0x9a0fe777, 367 }, /* vkGetDeviceMemoryOpaqueCaptureAddress */
    { 14138, 0x49339be6, 368 }, /* vkGetDeviceMemoryOpaqueCaptureAddressKHR */
    { 14179, 0x31e1f1f0, 563 }, /* vkGetDeviceMicromapCompatibilityEXT */
    { 14215, 0xba013486, 0 }, /* vkGetDeviceProcAddr */
    { 14235, 0xcc920d9a, 2 }, /* vkGetDeviceQueue */
    { 14252, 0xb11a6348, 244 }, /* vkGetDeviceQueue2 */
    { 14270, 0x9d280cca, 58 }, /* vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI */
    { 14318, 0xdb4a3473, 653 }, /* vkGetDeviceTensorMemoryRequirementsARM */
    { 14357, 0x4711995f, 574 }, /* vkGetDynamicRenderingTilePropertiesQCOM */
    { 14397, 0x56ffada3, 504 }, /* vkGetEncodedVideoSessionParametersKHR */
    { 14435, 0x96d834b, 28 }, /* vkGetEventStatus */
    { 14452, 0x69a5d6af, 192 }, /* vkGetFenceFdKHR */
    { 14468, 0x5f391892, 22 }, /* vkGetFenceStatus */
    { 14485, 0x8963de2, 190 }, /* vkGetFenceWin32HandleKHR */
    { 14510, 0xb332dbcd, 573 }, /* vkGetFramebufferTilePropertiesQCOM */
    { 14545, 0xa4c5682, 166 }, /* vkGetGeneratedCommandsMemoryRequirementsEXT */
    { 14589, 0xac420aaf, 161 }, /* vkGetGeneratedCommandsMemoryRequirementsNV */
    { 14632, 0xdaeeb271, 603 }, /* vkGetGpaDeviceClockInfoAMD */
    { 14659, 0xab32221, 609 }, /* vkGetGpaSessionResultsAMD */
    { 14685, 0x72d47ae7, 608 }, /* vkGetGpaSessionStatusAMD */
    { 14710, 0x12fa78a3, 352 }, /* vkGetImageDrmFormatModifierPropertiesEXT */
    { 14751, 0x916f1e63, 15 }, /* vkGetImageMemoryRequirements */
    { 14780, 0x56e213f7, 230 }, /* vkGetImageMemoryRequirements2 */
    { 14810, 0x8de28366, 231 }, /* vkGetImageMemoryRequirements2KHR */
    { 14843, 0x217c86e4, 677 }, /* vkGetImageOpaqueCaptureDataEXT */
    { 14874, 0x881c3c3d, 531 }, /* vkGetImageOpaqueCaptureDescriptorDataEXT */
    { 14915, 0x15855f5b, 17 }, /* vkGetImageSparseMemoryRequirements */
    { 14950, 0xbd4e3d3f, 232 }, /* vkGetImageSparseMemoryRequirements2 */
    { 14986, 0x3df40f5e, 233 }, /* vkGetImageSparseMemoryRequirements2KHR */
    { 15025, 0x9163b686, 42 }, /* vkGetImageSubresourceLayout */
    { 15053, 0xf18deffc, 567 }, /* vkGetImageSubresourceLayout2 */
    { 15082, 0x1a0c05f5, 569 }, /* vkGetImageSubresourceLayout2EXT */
    { 15114, 0x54442f69, 568 }, /* vkGetImageSubresourceLayout2KHR */
    { 15146, 0xed8f1d33, 345 }, /* vkGetImageViewAddressNVX */
    { 15171, 0x791e5b93, 344 }, /* vkGetImageViewHandle64NVX */
    { 15197, 0x20caa1e1, 343 }, /* vkGetImageViewHandleNVX */
    { 15221, 0xe2c45ea, 532 }, /* vkGetImageViewOpaqueCaptureDescriptorDataEXT */
    { 15266, 0x1595f564, 625 }, /* vkGetLatencyTimingsNV */
    { 15288, 0x71220e82, 284 }, /* vkGetMemoryAndroidHardwareBufferANDROID */
    { 15328, 0x503c14c5, 179 }, /* vkGetMemoryFdKHR */
    { 15345, 0xb028a792, 180 }, /* vkGetMemoryFdPropertiesKHR */
    { 15372, 0x7030ee5b, 267 }, /* vkGetMemoryHostPointerPropertiesEXT */
    { 15408, 0xcbc069ac, 632 }, /* vkGetMemoryMetalHandleEXT */
    { 15434, 0x63d24c29, 633 }, /* vkGetMemoryMetalHandlePropertiesEXT */
    { 15470, 0xfe84229e, 667 }, /* vkGetMemoryNativeBufferOHOS */
    { 15498, 0x93d6c7a4, 183 }, /* vkGetMemoryRemoteAddressNV */
    { 15525, 0x45fc7e1c, 177 }, /* vkGetMemoryWin32HandleKHR */
    { 15551, 0xc8795b9, 157 }, /* vkGetMemoryWin32HandleNV */
    { 15576, 0xb8f59859, 178 }, /* vkGetMemoryWin32HandlePropertiesKHR */
    { 15612, 0x4540b38e, 181 }, /* vkGetMemoryZirconHandleFUCHSIA */
    { 15643, 0x5a4149eb, 182 }, /* vkGetMemoryZirconHandlePropertiesFUCHSIA */
    { 15684, 0x715aea54, 564 }, /* vkGetMicromapBuildSizesEXT */
    { 15711, 0x87a09e58, 666 }, /* vkGetNativeBufferPropertiesOHOS */
    { 15743, 0xb67756e4, 515 }, /* vkGetPartitionedAccelerationStructuresBuildSizesNV */
    { 15794, 0xe201028a, 598 }, /* vkGetPastPresentationTimingEXT */
    { 15825, 0x19616a98, 222 }, /* vkGetPastPresentationTimingGOOGLE */
    { 15859, 0x1ec6c4ec, 366 }, /* vkGetPerformanceParameterINTEL */
    { 15890, 0x7b55e5b7, 54 }, /* vkGetPipelineBinaryDataKHR */
    { 15917, 0x2092a349, 49 }, /* vkGetPipelineCacheData */
    { 15940, 0x8b20fc09, 371 }, /* vkGetPipelineExecutableInternalRepresentationsKHR */
    { 15990, 0x748dd8cd, 369 }, /* vkGetPipelineExecutablePropertiesKHR */
    { 16027, 0x5c4d6435, 370 }, /* vkGetPipelineExecutableStatisticsKHR */
    { 16064, 0x4b0a7b6d, 386 }, /* vkGetPipelineIndirectDeviceAddressNV */
    { 16101, 0x617eabaa, 385 }, /* vkGetPipelineIndirectMemoryRequirementsNV */
    { 16143, 0x92b5eeb9, 53 }, /* vkGetPipelineKeyKHR */
    { 16163, 0xd93861f3, 570 }, /* vkGetPipelinePropertiesEXT */
    { 16190, 0xa60eca94, 459 }, /* vkGetPrivateData */
    { 16207, 0x2dc1491d, 460 }, /* vkGetPrivateDataEXT */
    { 16227, 0xbf3f2cb3, 33 }, /* vkGetQueryPoolResults */
    { 16249, 0xf6cef020, 491 }, /* vkGetQueueCheckpointData2NV */
    { 16277, 0x428d4692, 292 }, /* vkGetQueueCheckpointDataNV */
    { 16304, 0x4b32ff8, 332 }, /* vkGetRayTracingCaptureReplayShaderGroupHandlesKHR */
    { 16354, 0x4693e853, 330 }, /* vkGetRayTracingShaderGroupHandlesKHR */
    { 16391, 0x3b54d93a, 331 }, /* vkGetRayTracingShaderGroupHandlesNV */
    { 16427, 0x2f709815, 341 }, /* vkGetRayTracingShaderGroupStackSizeKHR */
    { 16466, 0x85a9d101, 221 }, /* vkGetRefreshCycleDurationGOOGLE */
    { 16498, 0xa9820d22, 76 }, /* vkGetRenderAreaGranularity */
    { 16525, 0x695b7926, 77 }, /* vkGetRenderingAreaGranularity */
    { 16555, 0xf98a6b4f, 78 }, /* vkGetRenderingAreaGranularityKHR */
    { 16588, 0xf13b1f2a, 533 }, /* vkGetSamplerOpaqueCaptureDescriptorDataEXT */
    { 16631, 0xc3009b1b, 599 }, /* vkGetScreenBufferPropertiesQNX */
    { 16662, 0xd05a61a0, 277 }, /* vkGetSemaphoreCounterValue */
    { 16689, 0xf3c26065, 278 }, /* vkGetSemaphoreCounterValueKHR */
    { 16719, 0x3e0e9884, 186 }, /* vkGetSemaphoreFdKHR */
    { 16739, 0xd04be5e5, 184 }, /* vkGetSemaphoreWin32HandleKHR */
    { 16768, 0x37c0989d, 188 }, /* vkGetSemaphoreZirconHandleFUCHSIA */
    { 16802, 0x42987180, 593 }, /* vkGetShaderBinaryDataEXT */
    { 16827, 0x5330743c, 255 }, /* vkGetShaderInfoAMD */
    { 16846, 0xfb50be1a, 645 }, /* vkGetShaderInstrumentationValuesARM */
    { 16882, 0x81d5d7e4, 566 }, /* vkGetShaderModuleCreateInfoIdentifierEXT */
    { 16923, 0xb7334436, 565 }, /* vkGetShaderModuleIdentifierEXT */
    { 16954, 0xa4aeb5a, 197 }, /* vkGetSwapchainCounterEXT */
    { 16979, 0x219d929, 252 }, /* vkGetSwapchainGrallocUsage2ANDROID */
    { 17014, 0x4979c9a3, 251 }, /* vkGetSwapchainGrallocUsageANDROID */
    { 17048, 0x57695f28, 149 }, /* vkGetSwapchainImagesKHR */
    { 17072, 0x66ae725e, 220 }, /* vkGetSwapchainStatusKHR */
    { 17096, 0xd5839310, 597 }, /* vkGetSwapchainTimeDomainPropertiesEXT */
    { 17134, 0x74de978f, 596 }, /* vkGetSwapchainTimingPropertiesEXT */
    { 17168, 0x306676b1, 651 }, /* vkGetTensorMemoryRequirementsARM */
    { 17201, 0x946c8f6f, 678 }, /* vkGetTensorOpaqueCaptureDataARM */
    { 17233, 0xcfd466c8, 655 }, /* vkGetTensorOpaqueCaptureDescriptorDataARM */
    { 17275, 0xcb3ac475, 656 }, /* vkGetTensorViewOpaqueCaptureDescriptorDataARM */
    { 17321, 0xbbc9f99f, 247 }, /* vkGetValidationCacheDataEXT */
    { 17349, 0xd8960270, 506 }, /* vkGetVideoSessionMemoryRequirementsKHR */
    { 17388, 0x51df0390, 193 }, /* vkImportFenceFdKHR */
    { 17407, 0x1bcbb079, 191 }, /* vkImportFenceWin32HandleKHR */
    { 17435, 0x36337c05, 187 }, /* vkImportSemaphoreFdKHR */
    { 17458, 0x7e2cfcdc, 185 }, /* vkImportSemaphoreWin32HandleKHR */
    { 17490, 0x4d1996ce, 189 }, /* vkImportSemaphoreZirconHandleFUCHSIA */
    { 17527, 0x65a01d77, 358 }, /* vkInitializePerformanceApiINTEL */
    { 17559, 0x1e115cca, 11 }, /* vkInvalidateMappedMemoryRanges */
    { 17590, 0x2b780b06, 623 }, /* vkLatencySleepNV */
    { 17607, 0xcb977bd8, 8 }, /* vkMapMemory */
    { 17619, 0x4e2b1f9a, 587 }, /* vkMapMemory2 */
    { 17632, 0x5d243bfb, 588 }, /* vkMapMemory2KHR */
    { 17648, 0xc3499606, 50 }, /* vkMergePipelineCaches */
    { 17670, 0xe8fe1154, 248 }, /* vkMergeValidationCachesEXT */
    { 17697, 0xcb7dc88, 261 }, /* vkQueueBeginDebugUtilsLabelEXT */
    { 17728, 0xc3628a09, 18 }, /* vkQueueBindSparse */
    { 17746, 0xf130b20a, 262 }, /* vkQueueEndDebugUtilsLabelEXT */
    { 17775, 0x56027200, 263 }, /* vkQueueInsertDebugUtilsLabelEXT */
    { 17807, 0x19ae9eed, 626 }, /* vkQueueNotifyOutOfBandNV */
    { 17832, 0xfc5fb6ce, 151 }, /* vkQueuePresentKHR */
    { 17850, 0x400cf596, 668 }, /* vkQueueSetPerfHintQCOM */
    { 17873, 0xf8499f82, 365 }, /* vkQueueSetPerformanceConfigurationINTEL */
    { 17913, 0xa0313eef, 254 }, /* vkQueueSignalReleaseImageANDROID */
    { 17946, 0xfa4713ec, 3 }, /* vkQueueSubmit */
    { 17960, 0xbf0609e6, 486 }, /* vkQueueSubmit2 */
    { 17975, 0xc2dd288f, 487 }, /* vkQueueSubmit2KHR */
    { 17993, 0x6f8fc2a5, 4 }, /* vkQueueWaitIdle */
    { 18009, 0x7aa819b3, 675 }, /* vkRegisterCustomBorderColorEXT */
    { 18040, 0x26cc78f5, 195 }, /* vkRegisterDeviceEventEXT */
    { 18065, 0x4a0bd849, 196 }, /* vkRegisterDisplayEventEXT */
    { 18091, 0xa633d2d1, 55 }, /* vkReleaseCapturedPipelineDataKHR */
    { 18124, 0x13814325, 349 }, /* vkReleaseFullScreenExclusiveModeEXT */
    { 18160, 0x28575036, 364 }, /* vkReleasePerformanceConfigurationINTEL */
    { 18199, 0x8bdecb76, 351 }, /* vkReleaseProfilingLockKHR */
    { 18225, 0xf217e5, 584 }, /* vkReleaseSwapchainImagesEXT */
    { 18253, 0x3b2a4159, 583 }, /* vkReleaseSwapchainImagesKHR */
    { 18281, 0x847dc731, 86 }, /* vkResetCommandBuffer */
    { 18302, 0x6da9f7fd, 81 }, /* vkResetCommandPool */
    { 18321, 0x9bd85f5, 68 }, /* vkResetDescriptorPool */
    { 18343, 0x6d373ba8, 30 }, /* vkResetEvent */
    { 18356, 0x684781dc, 21 }, /* vkResetFences */
    { 18370, 0xed2050e8, 610 }, /* vkResetGpaSessionAMD */
    { 18391, 0x4e671e02, 34 }, /* vkResetQueryPool */
    { 18408, 0xe6701e5f, 35 }, /* vkResetQueryPoolEXT */
    { 18428, 0x3c97f0dc, 539 }, /* vkSetBufferCollectionBufferConstraintsFUCHSIA */
    { 18474, 0xcae02471, 540 }, /* vkSetBufferCollectionImageConstraintsFUCHSIA */
    { 18519, 0x180cec44, 259 }, /* vkSetDebugUtilsObjectNameEXT */
    { 18548, 0x15942821, 260 }, /* vkSetDebugUtilsObjectTagEXT */
    { 18576, 0xa42f1309, 535 }, /* vkSetDeviceMemoryPriorityEXT */
    { 18605, 0x592ae5f5, 29 }, /* vkSetEvent */
    { 18616, 0x8f79058, 602 }, /* vkSetGpaDeviceClockModeAMD */
    { 18643, 0xa20f1ea9, 219 }, /* vkSetHdrMetadataEXT */
    { 18663, 0xfba7b4a3, 624 }, /* vkSetLatencyMarkerNV */
    { 18684, 0x4adbd599, 622 }, /* vkSetLatencySleepModeNV */
    { 18708, 0xbd1cd781, 256 }, /* vkSetLocalDimmingAMD */
    { 18729, 0x5cf49ca8, 457 }, /* vkSetPrivateData */
    { 18746, 0x23456729, 458 }, /* vkSetPrivateDataEXT */
    { 18766, 0x26a54a1b, 595 }, /* vkSetSwapchainPresentTimingQueueSizeEXT */
    { 18806, 0xcd347297, 281 }, /* vkSignalSemaphore */
    { 18824, 0x8fef55c6, 282 }, /* vkSignalSemaphoreKHR */
    { 18845, 0xdff5343, 498 }, /* vkTransitionImageLayout */
    { 18869, 0x50997986, 499 }, /* vkTransitionImageLayoutEXT */
    { 18896, 0xfef2fb38, 175 }, /* vkTrimCommandPool */
    { 18914, 0x51177c8d, 176 }, /* vkTrimCommandPoolKHR */
    { 18935, 0x408975ae, 359 }, /* vkUninitializePerformanceApiINTEL */
    { 18969, 0x1a1a0e2f, 9 }, /* vkUnmapMemory */
    { 18983, 0x11f12acb, 589 }, /* vkUnmapMemory2 */
    { 18998, 0xfb0bcab2, 590 }, /* vkUnmapMemory2KHR */
    { 19016, 0x2a6adf34, 676 }, /* vkUnregisterCustomBorderColorEXT */
    { 19049, 0x5349c9d, 215 }, /* vkUpdateDescriptorSetWithTemplate */
    { 19083, 0x214ad230, 216 }, /* vkUpdateDescriptorSetWithTemplateKHR */
    { 19120, 0xbfd090ae, 71 }, /* vkUpdateDescriptorSets */
    { 19143, 0x14206639, 171 }, /* vkUpdateIndirectExecutionSetPipelineEXT */
    { 19183, 0x948e3936, 172 }, /* vkUpdateIndirectExecutionSetShaderEXT */
    { 19221, 0x8e570a3a, 503 }, /* vkUpdateVideoSessionParametersKHR */
    { 19255, 0x19d64c81, 23 }, /* vkWaitForFences */
    { 19271, 0x49c2eb17, 536 }, /* vkWaitForPresent2KHR */
    { 19292, 0x19c136b1, 537 }, /* vkWaitForPresentKHR */
    { 19312, 0x74368ad9, 279 }, /* vkWaitSemaphores */
    { 19329, 0x2bc77454, 280 }, /* vkWaitSemaphoresKHR */
    { 19349, 0x8bc9ae08, 327 }, /* vkWriteAccelerationStructuresPropertiesKHR */
    { 19392, 0xc43df3dd, 562 }, /* vkWriteMicromapsPropertiesEXT */
    { 19422, 0xaa8e887d, 671 }, /* vkWriteResourceDescriptorsEXT */
    { 19452, 0x3b2f890b, 670 }, /* vkWriteSamplerDescriptorsEXT */
};

/* Hash table stats:
 * size 702 entries
 * collisions entries:
 *     0      465
 *     1      103
 *     2      57
 *     3      23
 *     4      11
 *     5      11
 *     6      6
 *     7      7
 *     8      1
 *     9+     18
 */

#define none 0xffff
static const uint16_t device_string_map[1024] = {
    0x01f5,
    0x00c6,
    none,
    none,
    0x00c1,
    0x007c,
    none,
    0x00e1,
    0x0201,
    0x006f,
    0x0046,
    0x028c,
    0x01d8,
    none,
    none,
    none,
    0x01ec,
    none,
    none,
    none,
    none,
    0x024d,
    none,
    none,
    0x0184,
    0x000f,
    0x0117,
    0x0069,
    0x023e,
    none,
    0x01a5,
    0x010e,
    0x0248,
    0x0044,
    none,
    0x01f7,
    0x012f,
    none,
    0x005f,
    none,
    none,
    0x022e,
    none,
    0x0135,
    0x0084,
    none,
    none,
    0x012a,
    none,
    0x019e,
    none,
    none,
    0x00dd,
    0x0240,
    0x025d,
    none,
    none,
    0x0188,
    none,
    0x0186,
    0x025a,
    0x015e,
    0x017c,
    0x014a,
    0x0072,
    none,
    0x0032,
    0x0067,
    0x0058,
    0x0038,
    none,
    0x0299,
    0x0196,
    0x0140,
    0x00d0,
    0x00cb,
    0x0045,
    0x009b,
    none,
    0x0060,
    0x021c,
    0x01a7,
    0x0187,
    0x0139,
    0x02b9,
    0x008a,
    none,
    0x0298,
    0x0014,
    0x001f,
    none,
    none,
    0x0130,
    0x018e,
    none,
    none,
    none,
    0x0059,
    0x0189,
    none,
    0x01c9,
    0x00a7,
    0x024b,
    0x0078,
    0x00bd,
    none,
    none,
    0x0082,
    0x005d,
    0x010c,
    none,
    0x0287,
    0x0031,
    0x0297,
    0x0125,
    0x020a,
    0x00ec,
    0x00fe,
    0x0164,
    0x01cc,
    0x0049,
    0x0133,
    0x015c,
    0x00e3,
    none,
    0x02bc,
    0x0057,
    0x0128,
    none,
    0x02b5,
    0x00cc,
    0x01cb,
    0x0157,
    none,
    0x014e,
    0x016c,
    0x0147,
    0x008c,
    0x00a2,
    0x0061,
    0x01b6,
    0x02a9,
    0x0103,
    0x0283,
    0x0107,
    0x0050,
    0x020f,
    0x003d,
    0x0029,
    0x0131,
    0x01ef,
    0x01b1,
    none,
    0x0206,
    none,
    0x0268,
    none,
    0x013c,
    0x00fa,
    0x017d,
    0x0053,
    0x02b8,
    none,
    0x0217,
    0x0109,
    0x0233,
    0x0101,
    none,
    0x028a,
    none,
    0x0256,
    none,
    none,
    0x0005,
    0x0278,
    0x01b3,
    0x016e,
    0x01b4,
    0x026c,
    0x008e,
    0x02ba,
    none,
    0x029e,
    0x00db,
    0x004c,
    0x013a,
    0x004e,
    0x00d7,
    none,
    none,
    none,
    none,
    0x0115,
    0x0075,
    0x0066,
    0x01fb,
    0x0172,
    0x022a,
    0x0096,
    none,
    0x0134,
    0x01f0,
    0x016b,
    0x0100,
    none,
    0x01d7,
    0x02a1,
    none,
    none,
    0x006e,
    none,
    none,
    0x02b1,
    0x00ff,
    0x0037,
    0x01de,
    0x0105,
    0x00f6,
    0x0054,
    none,
    0x0120,
    0x0001,
    0x0197,
    none,
    0x00ea,
    none,
    0x012c,
    0x0022,
    0x0116,
    0x01c5,
    none,
    0x0154,
    0x0175,
    0x01c4,
    0x0255,
    none,
    0x023b,
    none,
    none,
    0x026e,
    0x0209,
    0x01c3,
    none,
    0x023f,
    none,
    0x01e3,
    none,
    0x008b,
    none,
    none,
    none,
    0x0293,
    0x0258,
    0x0092,
    0x00ce,
    0x0247,
    none,
    0x024e,
    0x017f,
    0x0271,
    none,
    0x006d,
    0x0081,
    0x0039,
    0x002f,
    none,
    none,
    0x02bd,
    none,
    none,
    none,
    0x026d,
    0x01ff,
    0x009d,
    none,
    none,
    none,
    0x00a4,
    0x0142,
    none,
    0x0011,
    none,
    0x0006,
    0x004d,
    none,
    0x0246,
    none,
    none,
    none,
    none,
    0x00a1,
    none,
    0x00ef,
    none,
    0x0199,
    0x0145,
    0x01fd,
    0x025f,
    none,
    0x00f5,
    none,
    0x004a,
    0x0286,
    0x01d0,
    none,
    0x00b5,
    none,
    0x0224,
    0x0132,
    0x024f,
    0x0083,
    0x01e0,
    none,
    0x0167,
    0x0007,
    0x0296,
    0x01ed,
    0x01d6,
    0x0181,
    0x001c,
    0x00e5,
    none,
    none,
    none,
    0x00e0,
    none,
    none,
    none,
    0x00fc,
    0x02b3,
    none,
    none,
    0x00da,
    0x002b,
    none,
    0x029c,
    none,
    0x01f1,
    0x00ab,
    0x01ad,
    0x0277,
    none,
    none,
    none,
    0x0158,
    0x028d,
    0x0185,
    0x02af,
    0x0016,
    0x014c,
    none,
    0x020b,
    0x024c,
    0x0091,
    0x019c,
    none,
    0x0228,
    0x010d,
    0x0127,
    0x01e7,
    0x0171,
    0x012b,
    none,
    0x01b8,
    none,
    0x00d4,
    0x0104,
    0x01f3,
    none,
    none,
    0x0002,
    0x01ee,
    none,
    none,
    none,
    0x0023,
    0x0149,
    0x01bb,
    none,
    none,
    0x0193,
    0x009c,
    none,
    none,
    0x00ee,
    0x00c8,
    none,
    none,
    0x0036,
    0x01ab,
    0x0143,
    none,
    0x00c0,
    0x0121,
    0x0270,
    0x021e,
    none,
    0x01dc,
    none,
    none,
    0x008d,
    none,
    none,
    0x0174,
    0x00cd,
    0x0156,
    0x01ac,
    none,
    0x0250,
    0x029f,
    0x0207,
    0x0137,
    none,
    0x0159,
    none,
    0x001a,
    0x01dd,
    0x007f,
    none,
    0x020c,
    none,
    none,
    0x01d2,
    none,
    none,
    0x00ed,
    none,
    none,
    0x01ea,
    0x0012,
    0x015d,
    0x0027,
    none,
    0x0019,
    0x01bc,
    0x0254,
    none,
    0x0202,
    0x0146,
    0x023c,
    0x0025,
    0x01d9,
    none,
    none,
    0x00b6,
    none,
    none,
    0x0089,
    0x00d5,
    0x02aa,
    none,
    none,
    0x00eb,
    0x0269,
    0x0285,
    0x00c7,
    0x0080,
    0x0260,
    none,
    0x000c,
    0x0000,
    0x003e,
    none,
    0x0106,
    0x00f0,
    0x01f6,
    0x022d,
    0x0008,
    none,
    none,
    none,
    none,
    none,
    0x0099,
    0x002e,
    none,
    0x01b0,
    none,
    0x01a6,
    0x01e5,
    none,
    0x0226,
    0x0210,
    0x00f2,
    0x00d1,
    0x0257,
    0x0024,
    0x0015,
    none,
    none,
    0x0227,
    0x0165,
    0x00a8,
    0x017e,
    0x0170,
    0x0292,
    0x0205,
    none,
    0x0232,
    0x0244,
    none,
    0x0090,
    none,
    0x01ae,
    0x02a7,
    0x00bb,
    0x0034,
    0x00d3,
    none,
    none,
    0x0235,
    0x00a9,
    0x00e2,
    none,
    0x013d,
    0x01d3,
    none,
    0x0259,
    0x0097,
    0x0198,
    0x0055,
    0x0279,
    0x019d,
    0x00ba,
    0x00bc,
    0x0026,
    none,
    none,
    0x0094,
    0x0151,
    0x0160,
    0x027b,
    0x0200,
    0x007e,
    none,
    none,
    none,
    0x0276,
    0x000b,
    0x0222,
    0x0231,
    0x027a,
    none,
    0x00e4,
    0x0064,
    0x0215,
    0x01e6,
    none,
    0x00c5,
    0x02a5,
    none,
    none,
    0x00c9,
    0x0052,
    0x01da,
    none,
    0x0021,
    0x025b,
    0x027e,
    0x02ab,
    0x02b0,
    none,
    0x0114,
    0x006a,
    none,
    0x00a3,
    0x016a,
    none,
    0x01db,
    0x02b2,
    0x02b4,
    0x01a1,
    0x01a9,
    0x0056,
    none,
    0x0051,
    0x00ae,
    0x00f1,
    none,
    none,
    none,
    0x00d2,
    0x0079,
    0x01f4,
    0x007d,
    none,
    0x0010,
    none,
    0x014d,
    none,
    0x0294,
    none,
    0x004b,
    none,
    0x0088,
    none,
    0x01e4,
    none,
    0x0163,
    none,
    0x019a,
    0x0108,
    none,
    0x013b,
    none,
    0x0166,
    0x0262,
    0x002d,
    none,
    none,
    none,
    0x008f,
    0x00b2,
    0x01a0,
    none,
    0x0236,
    0x0076,
    0x017b,
    none,
    0x01aa,
    0x00f4,
    none,
    0x0183,
    none,
    0x01af,
    0x01e9,
    0x007b,
    0x01b5,
    none,
    none,
    0x00e9,
    0x019b,
    0x00a5,
    none,
    0x0290,
    none,
    none,
    none,
    0x01f2,
    0x0153,
    none,
    0x018a,
    0x00d8,
    0x026a,
    0x0214,
    0x01e1,
    0x0220,
    0x0087,
    none,
    0x0102,
    0x00be,
    0x00bf,
    none,
    0x0073,
    none,
    0x011c,
    0x0177,
    0x0237,
    0x01a4,
    none,
    0x01b2,
    0x0155,
    none,
    0x02a4,
    0x023a,
    none,
    0x00f8,
    0x015b,
    0x0218,
    0x0239,
    0x022f,
    none,
    0x01c0,
    0x00c4,
    0x018c,
    none,
    0x0020,
    0x01b9,
    none,
    0x01d1,
    0x018f,
    0x00dc,
    none,
    0x0168,
    0x0009,
    0x00f7,
    none,
    0x020e,
    none,
    0x0265,
    0x02ad,
    0x0144,
    none,
    none,
    none,
    0x0282,
    0x0249,
    0x0161,
    0x01eb,
    0x015f,
    0x002a,
    0x01fe,
    0x01e2,
    none,
    0x0148,
    none,
    0x0213,
    none,
    0x02b7,
    none,
    0x029b,
    0x0042,
    0x0098,
    0x0077,
    0x00ca,
    0x00a0,
    0x0243,
    0x019f,
    0x0212,
    0x0110,
    none,
    0x0288,
    none,
    none,
    0x006c,
    0x01d4,
    none,
    0x00b0,
    none,
    0x0192,
    none,
    0x0179,
    none,
    none,
    0x0284,
    none,
    0x00b1,
    0x0229,
    0x005a,
    0x002c,
    0x021b,
    0x01fc,
    none,
    0x0216,
    none,
    none,
    none,
    none,
    0x0041,
    0x0040,
    0x0267,
    0x014f,
    0x017a,
    0x02ac,
    none,
    0x01fa,
    0x026f,
    0x00df,
    0x00a6,
    0x0238,
    0x0119,
    0x00b4,
    none,
    none,
    none,
    none,
    none,
    0x0124,
    0x027c,
    0x005c,
    0x0280,
    none,
    0x0086,
    none,
    0x00ac,
    0x00e8,
    0x011a,
    0x029a,
    0x00b9,
    0x014b,
    0x010a,
    none,
    0x00c2,
    none,
    0x0070,
    0x0178,
    0x02a3,
    none,
    0x0118,
    0x0191,
    0x00e6,
    0x0126,
    0x0074,
    0x012e,
    0x001b,
    0x006b,
    none,
    0x003f,
    0x013e,
    0x0111,
    0x00fb,
    none,
    none,
    0x0263,
    0x01ba,
    0x0195,
    none,
    0x000a,
    0x0261,
    0x01b7,
    0x018d,
    0x00de,
    0x00b7,
    0x00af,
    0x0003,
    none,
    0x00d9,
    0x016f,
    0x013f,
    0x016d,
    0x02ae,
    none,
    none,
    0x00cf,
    0x012d,
    none,
    0x01bf,
    none,
    0x0123,
    0x0030,
    0x0071,
    0x022c,
    0x009a,
    0x0085,
    none,
    0x01a2,
    0x028e,
    none,
    none,
    none,
    0x0208,
    0x023d,
    0x0018,
    0x01ca,
    0x009e,
    none,
    0x00ad,
    0x000d,
    0x0162,
    0x01c7,
    0x0245,
    0x00e7,
    0x018b,
    0x0182,
    0x00f9,
    none,
    none,
    none,
    0x001d,
    0x021d,
    0x009f,
    0x01c8,
    0x001e,
    0x0180,
    none,
    0x00aa,
    0x0251,
    0x0252,
    none,
    0x0272,
    0x011e,
    0x0004,
    none,
    0x0223,
    none,
    none,
    none,
    0x0241,
    0x01c1,
    0x0266,
    0x004f,
    0x0141,
    none,
    none,
    0x010f,
    0x01f8,
    0x028b,
    0x0203,
    0x0093,
    0x011d,
    0x01a3,
    none,
    0x0295,
    0x01c6,
    none,
    none,
    0x025e,
    0x02a0,
    0x011b,
    0x00fd,
    0x0169,
    none,
    none,
    0x0017,
    0x02a2,
    0x0043,
    0x0190,
    none,
    0x0062,
    0x01a8,
    0x0234,
    0x0264,
    0x026b,
    0x0095,
    0x022b,
    0x0033,
    none,
    0x027f,
    0x0138,
    0x01d5,
    none,
    none,
    0x01c2,
    0x011f,
    0x02b6,
    0x01cf,
    none,
    0x021a,
    0x0253,
    0x003c,
    0x02a6,
    0x01bd,
    0x0136,
    none,
    0x0225,
    0x00c3,
    0x0291,
    0x005b,
    0x020d,
    none,
    0x005e,
    0x0274,
    0x010b,
    0x01be,
    0x0068,
    0x0122,
    0x0129,
    0x029d,
    none,
    none,
    none,
    0x01cd,
    none,
    none,
    none,
    none,
    0x0113,
    0x021f,
    none,
    0x0152,
    none,
    none,
    none,
    none,
    0x0173,
    0x027d,
    none,
    0x0112,
    none,
    0x0063,
    0x0230,
    none,
    none,
    0x0211,
    0x000e,
    0x015a,
    0x0242,
    0x00b3,
    none,
    none,
    0x0028,
    none,
    none,
    0x0194,
    0x0065,
    none,
    none,
    none,
    0x0048,
    0x0035,
    none,
    none,
    none,
    0x0013,
    0x01df,
    0x0289,
    0x025c,
    0x007a,
    0x00f3,
    0x01f9,
    0x00b8,
    0x01e8,
    0x01ce,
    0x003b,
    0x0281,
    0x0150,
    0x0047,
    none,
    0x02bb,
    none,
    none,
    none,
    0x00d6,
    0x003a,
    0x02a8,
    0x0219,
    0x024a,
    0x0204,
    none,
    0x0275,
    0x0221,
    0x028f,
    0x0273,
    0x0176,
};

static int
device_string_map_lookup(const char *str)
{
    static const uint32_t prime_factor = 5024183;
    static const uint32_t prime_step = 19;
    const struct string_map_entry *e;
    uint32_t hash, h;
    uint16_t i;
    const char *p;

    hash = 0;
    for (p = str; *p; p++)
        hash = hash * prime_factor + *p;

    h = hash;
    while (1) {
        i = device_string_map[h & 1023];
        if (i == none)
           return -1;
        e = &device_string_map_entries[i];
        if (e->hash == hash && strcmp(str, device_strings + e->name) == 0)
            return e->num;
        h += prime_step;
    }

    return -1;
}



static const uint8_t instance_compaction_table[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
};


static const uint8_t physical_device_compaction_table[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    28,
    29,
    29,
    30,
    30,
    31,
    31,
    32,
    32,
    33,
    33,
    34,
    34,
    35,
    35,
    36,
    36,
    37,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    52,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
};


static const uint16_t device_compaction_table[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    91,
    92,
    93,
    94,
    95,
    96,
    97,
    98,
    99,
    100,
    101,
    102,
    103,
    104,
    105,
    106,
    107,
    108,
    109,
    110,
    111,
    112,
    113,
    114,
    115,
    116,
    117,
    118,
    119,
    120,
    121,
    122,
    123,
    124,
    125,
    126,
    127,
    128,
    129,
    130,
    131,
    132,
    133,
    134,
    135,
    136,
    137,
    138,
    139,
    140,
    141,
    142,
    143,
    144,
    145,
    146,
    147,
    148,
    149,
    150,
    151,
    152,
    153,
    154,
    155,
    156,
    157,
    158,
    159,
    160,
    161,
    162,
    163,
    164,
    165,
    166,
    167,
    168,
    169,
    170,
    171,
    171,
    172,
    172,
    173,
    174,
    175,
    176,
    177,
    178,
    179,
    180,
    181,
    182,
    183,
    184,
    185,
    186,
    187,
    188,
    189,
    190,
    191,
    192,
    193,
    194,
    194,
    195,
    195,
    196,
    196,
    197,
    197,
    198,
    199,
    200,
    201,
    201,
    202,
    202,
    203,
    203,
    204,
    204,
    205,
    205,
    206,
    207,
    208,
    209,
    210,
    211,
    212,
    213,
    214,
    215,
    215,
    216,
    216,
    217,
    217,
    218,
    218,
    219,
    219,
    220,
    220,
    221,
    221,
    222,
    222,
    223,
    224,
    225,
    226,
    227,
    228,
    228,
    229,
    230,
    231,
    232,
    233,
    234,
    235,
    235,
    236,
    237,
    238,
    239,
    240,
    241,
    242,
    243,
    244,
    245,
    246,
    246,
    247,
    247,
    248,
    248,
    249,
    249,
    250,
    250,
    251,
    251,
    252,
    252,
    253,
    254,
    255,
    255,
    255,
    256,
    256,
    256,
    257,
    258,
    259,
    260,
    261,
    262,
    263,
    264,
    265,
    266,
    267,
    268,
    269,
    270,
    271,
    272,
    273,
    274,
    275,
    276,
    277,
    278,
    279,
    280,
    281,
    282,
    283,
    284,
    285,
    286,
    287,
    288,
    289,
    290,
    291,
    292,
    293,
    294,
    295,
    296,
    296,
    297,
    298,
    299,
    300,
    301,
    302,
    303,
    304,
    305,
    306,
    307,
    308,
    309,
    310,
    311,
    312,
    313,
    314,
    315,
    316,
    317,
    318,
    318,
    319,
    319,
    319,
    320,
    321,
    322,
    323,
    324,
    325,
    326,
    327,
    328,
    329,
    329,
    330,
    331,
    332,
    333,
    333,
    333,
    334,
    335,
    336,
    337,
    338,
    339,
    340,
    341,
    342,
    343,
    344,
    345,
    346,
    347,
    347,
    348,
    348,
    349,
    349,
    350,
    350,
    351,
    351,
    352,
    352,
    353,
    353,
    354,
    354,
    355,
    355,
    356,
    356,
    357,
    357,
    358,
    358,
    359,
    359,
    360,
    361,
    361,
    362,
    362,
    363,
    364,
    364,
    365,
    366,
    367,
    368,
    369,
    370,
    371,
    372,
    373,
    374,
    375,
    376,
    377,
    378,
    379,
    380,
    381,
    382,
    383,
    384,
    385,
    386,
    387,
    388,
    389,
    390,
    391,
    392,
    393,
    394,
    395,
    396,
    396,
    397,
    397,
    398,
    398,
    399,
    399,
    400,
    400,
    401,
    401,
    402,
    402,
    403,
    403,
    404,
    404,
    405,
    405,
    406,
    407,
    408,
    409,
    410,
    411,
    411,
    412,
    412,
    413,
    413,
    414,
    414,
    415,
    415,
    416,
    416,
    417,
    418,
    419,
    419,
    420,
    420,
    421,
    421,
    422,
    422,
    423,
    424,
    425,
    426,
    427,
    428,
    429,
    430,
    431,
    432,
    433,
    434,
    435,
    436,
    437,
    438,
    439,
    440,
    441,
    442,
    443,
    444,
    445,
    446,
    447,
    448,
    449,
    450,
    451,
    452,
    453,
    454,
    455,
    456,
    457,
    458,
    459,
    460,
    461,
    462,
    463,
    464,
    465,
    466,
    466,
    467,
    468,
    468,
    467,
    469,
    470,
    471,
    472,
    473,
    474,
    475,
    476,
    477,
    478,
    479,
    480,
    481,
    482,
    483,
    484,
    485,
    486,
    487,
    487,
    487,
    488,
    489,
    490,
    491,
    492,
    493,
    494,
    495,
    496,
    497,
    498,
    499,
    500,
    501,
    501,
    502,
    502,
    503,
    503,
    504,
    504,
    505,
    506,
    507,
    508,
    509,
    510,
    511,
    512,
    513,
    514,
    515,
    516,
    517,
    518,
    519,
    520,
    521,
    522,
    523,
    524,
    525,
    526,
    526,
    527,
    527,
    528,
    528,
    529,
    529,
    530,
    531,
    532,
    533,
    534,
    535,
    536,
    537,
    537,
    538,
    538,
    539,
    540,
    541,
    542,
    543,
    544,
    545,
    546,
    547,
    548,
    549,
    550,
    551,
    552,
    553,
    554,
    555,
    556,
    557,
    558,
    559,
    560,
    561,
    562,
    563,
    564,
    565,
    566,
    567,
    568,
    569,
    570,
    571,
    572,
    573,
    574,
    575,
    576,
    577,
    578,
    579,
    580,
    581,
    582,
    583,
    584,
    585,
    586,
    587,
    588,
    589,
    590,
    591,
    592,
    593,
    594,
    595,
    596,
    597,
    598,
    599,
    600,
    601,
    602,
    603,
    604,
    605,
    606,
    607,
    608,
    609,
};

static bool
vk_instance_entrypoint_is_enabled(int index, uint32_t core_version,
                                  const struct vk_instance_extension_table *instance)
{
   switch (index) {
   case 0:
      /* CreateInstance */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 1:
      /* DestroyInstance */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 2:
      /* EnumeratePhysicalDevices */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 3:
      /* GetInstanceProcAddr */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 4:
      /* EnumerateInstanceVersion */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 5:
      /* EnumerateInstanceLayerProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 6:
      /* EnumerateInstanceExtensionProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 7:
      /* CreateAndroidSurfaceKHR */
      if (instance->KHR_android_surface) return true;
      return false;
   case 8:
      /* CreateSurfaceOHOS */
      if (instance->OHOS_surface) return true;
      return false;
   case 9:
      /* CreateDisplayPlaneSurfaceKHR */
      if (instance->KHR_display) return true;
      return false;
   case 10:
      /* DestroySurfaceKHR */
      if (instance->KHR_surface) return true;
      return false;
   case 11:
      /* CreateViSurfaceNN */
      if (instance->NN_vi_surface) return true;
      return false;
   case 12:
      /* CreateWaylandSurfaceKHR */
      if (instance->KHR_wayland_surface) return true;
      return false;
   case 13:
      /* CreateUbmSurfaceSEC */
      if (instance->SEC_ubm_surface) return true;
      return false;
   case 14:
      /* CreateWin32SurfaceKHR */
      if (instance->KHR_win32_surface) return true;
      return false;
   case 15:
      /* CreateXlibSurfaceKHR */
      if (instance->KHR_xlib_surface) return true;
      return false;
   case 16:
      /* CreateXcbSurfaceKHR */
      if (instance->KHR_xcb_surface) return true;
      return false;
   case 17:
      /* CreateDirectFBSurfaceEXT */
      if (instance->EXT_directfb_surface) return true;
      return false;
   case 18:
      /* CreateImagePipeSurfaceFUCHSIA */
      if (instance->FUCHSIA_imagepipe_surface) return true;
      return false;
   case 19:
      /* CreateStreamDescriptorSurfaceGGP */
      if (instance->GGP_stream_descriptor_surface) return true;
      return false;
   case 20:
      /* CreateScreenSurfaceQNX */
      if (instance->QNX_screen_surface) return true;
      return false;
   case 21:
      /* CreateDebugReportCallbackEXT */
      if (instance->EXT_debug_report) return true;
      return false;
   case 22:
      /* DestroyDebugReportCallbackEXT */
      if (instance->EXT_debug_report) return true;
      return false;
   case 23:
      /* DebugReportMessageEXT */
      if (instance->EXT_debug_report) return true;
      return false;
   case 24:
      /* EnumeratePhysicalDeviceGroups */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 25:
      /* EnumeratePhysicalDeviceGroupsKHR */
      if (instance->KHR_device_group_creation) return true;
      return false;
   case 26:
      /* CreateIOSSurfaceMVK */
      if (instance->MVK_ios_surface) return true;
      return false;
   case 27:
      /* CreateMacOSSurfaceMVK */
      if (instance->MVK_macos_surface) return true;
      return false;
   case 28:
      /* CreateMetalSurfaceEXT */
      if (instance->EXT_metal_surface) return true;
      return false;
   case 29:
      /* CreateDebugUtilsMessengerEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 30:
      /* DestroyDebugUtilsMessengerEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 31:
      /* SubmitDebugUtilsMessageEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 32:
      /* CreateHeadlessSurfaceEXT */
      if (instance->EXT_headless_surface) return true;
      return false;
   case 33:
      /* CreatePlan9SurfacePLAN9 */
      if (instance->PLAN9_surface) return true;
      return false;
   case 34:
      /* GetExternalComputeQueueDataNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   default:
      return false;
   }
}

/** Return true if the core version or extension in which the given entrypoint
 * is defined is enabled.
 *
 * If device is NULL, all device extensions are considered enabled.
 */
static bool
vk_physical_device_entrypoint_is_enabled(int index, uint32_t core_version,
                                         const struct vk_instance_extension_table *instance)
{
   switch (index) {
   case 0:
      /* GetPhysicalDeviceProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 1:
      /* GetPhysicalDeviceQueueFamilyProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 2:
      /* GetPhysicalDeviceMemoryProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 3:
      /* GetPhysicalDeviceFeatures */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 4:
      /* GetPhysicalDeviceFormatProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 5:
      /* GetPhysicalDeviceImageFormatProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 6:
      /* CreateDevice */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 7:
      /* EnumerateDeviceLayerProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 8:
      /* EnumerateDeviceExtensionProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 9:
      /* GetPhysicalDeviceSparseImageFormatProperties */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 10:
      /* GetPhysicalDeviceDisplayPropertiesKHR */
      if (instance->KHR_display) return true;
      return false;
   case 11:
      /* GetPhysicalDeviceDisplayPlanePropertiesKHR */
      if (instance->KHR_display) return true;
      return false;
   case 12:
      /* GetDisplayPlaneSupportedDisplaysKHR */
      if (instance->KHR_display) return true;
      return false;
   case 13:
      /* GetDisplayModePropertiesKHR */
      if (instance->KHR_display) return true;
      return false;
   case 14:
      /* CreateDisplayModeKHR */
      if (instance->KHR_display) return true;
      return false;
   case 15:
      /* GetDisplayPlaneCapabilitiesKHR */
      if (instance->KHR_display) return true;
      return false;
   case 16:
      /* GetPhysicalDeviceSurfaceSupportKHR */
      if (instance->KHR_surface) return true;
      return false;
   case 17:
      /* GetPhysicalDeviceSurfaceCapabilitiesKHR */
      if (instance->KHR_surface) return true;
      return false;
   case 18:
      /* GetPhysicalDeviceSurfaceFormatsKHR */
      if (instance->KHR_surface) return true;
      return false;
   case 19:
      /* GetPhysicalDeviceSurfacePresentModesKHR */
      if (instance->KHR_surface) return true;
      return false;
   case 20:
      /* GetPhysicalDeviceWaylandPresentationSupportKHR */
      if (instance->KHR_wayland_surface) return true;
      return false;
   case 21:
      /* GetPhysicalDeviceUbmPresentationSupportSEC */
      if (instance->SEC_ubm_surface) return true;
      return false;
   case 22:
      /* GetPhysicalDeviceWin32PresentationSupportKHR */
      if (instance->KHR_win32_surface) return true;
      return false;
   case 23:
      /* GetPhysicalDeviceXlibPresentationSupportKHR */
      if (instance->KHR_xlib_surface) return true;
      return false;
   case 24:
      /* GetPhysicalDeviceXcbPresentationSupportKHR */
      if (instance->KHR_xcb_surface) return true;
      return false;
   case 25:
      /* GetPhysicalDeviceDirectFBPresentationSupportEXT */
      if (instance->EXT_directfb_surface) return true;
      return false;
   case 26:
      /* GetPhysicalDeviceScreenPresentationSupportQNX */
      if (instance->QNX_screen_surface) return true;
      return false;
   case 27:
      /* GetPhysicalDeviceExternalImageFormatPropertiesNV */
      if (instance->NV_external_memory_capabilities) return true;
      return false;
   case 28:
      /* GetPhysicalDeviceFeatures2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 29:
      /* GetPhysicalDeviceFeatures2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 30:
      /* GetPhysicalDeviceProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 31:
      /* GetPhysicalDeviceProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 32:
      /* GetPhysicalDeviceFormatProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 33:
      /* GetPhysicalDeviceFormatProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 34:
      /* GetPhysicalDeviceImageFormatProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 35:
      /* GetPhysicalDeviceImageFormatProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 36:
      /* GetPhysicalDeviceQueueFamilyProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 37:
      /* GetPhysicalDeviceQueueFamilyProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 38:
      /* GetPhysicalDeviceMemoryProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 39:
      /* GetPhysicalDeviceMemoryProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 40:
      /* GetPhysicalDeviceSparseImageFormatProperties2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 41:
      /* GetPhysicalDeviceSparseImageFormatProperties2KHR */
      if (instance->KHR_get_physical_device_properties2) return true;
      return false;
   case 42:
      /* GetPhysicalDeviceExternalBufferProperties */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 43:
      /* GetPhysicalDeviceExternalBufferPropertiesKHR */
      if (instance->KHR_external_memory_capabilities) return true;
      return false;
   case 44:
      /* GetPhysicalDeviceExternalSemaphoreProperties */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 45:
      /* GetPhysicalDeviceExternalSemaphorePropertiesKHR */
      if (instance->KHR_external_semaphore_capabilities) return true;
      return false;
   case 46:
      /* GetPhysicalDeviceExternalFenceProperties */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 47:
      /* GetPhysicalDeviceExternalFencePropertiesKHR */
      if (instance->KHR_external_fence_capabilities) return true;
      return false;
   case 48:
      /* ReleaseDisplayEXT */
      if (instance->EXT_direct_mode_display) return true;
      return false;
   case 49:
      /* AcquireXlibDisplayEXT */
      if (instance->EXT_acquire_xlib_display) return true;
      return false;
   case 50:
      /* GetRandROutputDisplayEXT */
      if (instance->EXT_acquire_xlib_display) return true;
      return false;
   case 51:
      /* AcquireWinrtDisplayNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 52:
      /* GetWinrtDisplayNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 53:
      /* GetPhysicalDeviceSurfaceCapabilities2EXT */
      if (instance->EXT_display_surface_counter) return true;
      return false;
   case 54:
      /* GetPhysicalDevicePresentRectanglesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 55:
      /* GetPhysicalDeviceMultisamplePropertiesEXT */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 56:
      /* GetPhysicalDeviceSurfaceCapabilities2KHR */
      if (instance->KHR_get_surface_capabilities2) return true;
      return false;
   case 57:
      /* GetPhysicalDeviceSurfaceFormats2KHR */
      if (instance->KHR_get_surface_capabilities2) return true;
      return false;
   case 58:
      /* GetPhysicalDeviceDisplayProperties2KHR */
      if (instance->KHR_get_display_properties2) return true;
      return false;
   case 59:
      /* GetPhysicalDeviceDisplayPlaneProperties2KHR */
      if (instance->KHR_get_display_properties2) return true;
      return false;
   case 60:
      /* GetDisplayModeProperties2KHR */
      if (instance->KHR_get_display_properties2) return true;
      return false;
   case 61:
      /* GetDisplayPlaneCapabilities2KHR */
      if (instance->KHR_get_display_properties2) return true;
      return false;
   case 62:
      /* GetPhysicalDeviceCalibrateableTimeDomainsKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 63:
      /* GetPhysicalDeviceCalibrateableTimeDomainsEXT */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 64:
      /* GetPhysicalDeviceCooperativeMatrixPropertiesNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 65:
      /* GetPhysicalDeviceSurfacePresentModes2EXT */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 66:
      /* EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 67:
      /* GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 68:
      /* GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 69:
      /* GetPhysicalDeviceToolProperties */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 70:
      /* GetPhysicalDeviceToolPropertiesEXT */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 71:
      /* GetPhysicalDeviceFragmentShadingRatesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 72:
      /* GetPhysicalDeviceVideoCapabilitiesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 73:
      /* GetPhysicalDeviceVideoFormatPropertiesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 74:
      /* GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 75:
      /* AcquireDrmDisplayEXT */
      if (instance->EXT_acquire_drm_display) return true;
      return false;
   case 76:
      /* GetDrmDisplayEXT */
      if (instance->EXT_acquire_drm_display) return true;
      return false;
   case 77:
      /* GetPhysicalDeviceOpticalFlowImageFormatsNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 78:
      /* GetPhysicalDeviceCooperativeMatrixPropertiesKHR */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 79:
      /* GetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 80:
      /* GetPhysicalDeviceCooperativeVectorPropertiesNV */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 81:
      /* EnumeratePhysicalDeviceShaderInstrumentationMetricsARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 82:
      /* GetPhysicalDeviceExternalTensorPropertiesARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 83:
      /* GetPhysicalDeviceQueueFamilyDataGraphPropertiesARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 84:
      /* GetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 85:
      /* EnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 86:
      /* GetPhysicalDeviceDescriptorSizeEXT */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 87:
      /* GetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   case 88:
      /* GetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM */
      /* All device extensions are considered enabled at the instance level */
      return true;
      return false;
   default:
      return false;
   }
}

/** Return true if the core version or extension in which the given entrypoint
 * is defined is enabled.
 *
 * If device is NULL, all device extensions are considered enabled.
 */
static bool
vk_device_entrypoint_is_enabled(int index, uint32_t core_version,
                                const struct vk_instance_extension_table *instance,
                                const struct vk_device_extension_table *device)
{
   switch (index) {
   case 0:
      /* GetDeviceProcAddr */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 1:
      /* DestroyDevice */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 2:
      /* GetDeviceQueue */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 3:
      /* QueueSubmit */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 4:
      /* QueueWaitIdle */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 5:
      /* DeviceWaitIdle */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 6:
      /* AllocateMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 7:
      /* FreeMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 8:
      /* MapMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 9:
      /* UnmapMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 10:
      /* FlushMappedMemoryRanges */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 11:
      /* InvalidateMappedMemoryRanges */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 12:
      /* GetDeviceMemoryCommitment */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 13:
      /* GetBufferMemoryRequirements */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 14:
      /* BindBufferMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 15:
      /* GetImageMemoryRequirements */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 16:
      /* BindImageMemory */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 17:
      /* GetImageSparseMemoryRequirements */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 18:
      /* QueueBindSparse */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 19:
      /* CreateFence */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 20:
      /* DestroyFence */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 21:
      /* ResetFences */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 22:
      /* GetFenceStatus */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 23:
      /* WaitForFences */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 24:
      /* CreateSemaphore */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 25:
      /* DestroySemaphore */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 26:
      /* CreateEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 27:
      /* DestroyEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 28:
      /* GetEventStatus */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 29:
      /* SetEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 30:
      /* ResetEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 31:
      /* CreateQueryPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 32:
      /* DestroyQueryPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 33:
      /* GetQueryPoolResults */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 34:
      /* ResetQueryPool */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 35:
      /* ResetQueryPoolEXT */
      if (!device || device->EXT_host_query_reset) return true;
      return false;
   case 36:
      /* CreateBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 37:
      /* DestroyBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 38:
      /* CreateBufferView */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 39:
      /* DestroyBufferView */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 40:
      /* CreateImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 41:
      /* DestroyImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 42:
      /* GetImageSubresourceLayout */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 43:
      /* CreateImageView */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 44:
      /* DestroyImageView */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 45:
      /* CreateShaderModule */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 46:
      /* DestroyShaderModule */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 47:
      /* CreatePipelineCache */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 48:
      /* DestroyPipelineCache */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 49:
      /* GetPipelineCacheData */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 50:
      /* MergePipelineCaches */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 51:
      /* CreatePipelineBinariesKHR */
      if (!device || device->KHR_pipeline_binary) return true;
      return false;
   case 52:
      /* DestroyPipelineBinaryKHR */
      if (!device || device->KHR_pipeline_binary) return true;
      return false;
   case 53:
      /* GetPipelineKeyKHR */
      if (!device || device->KHR_pipeline_binary) return true;
      return false;
   case 54:
      /* GetPipelineBinaryDataKHR */
      if (!device || device->KHR_pipeline_binary) return true;
      return false;
   case 55:
      /* ReleaseCapturedPipelineDataKHR */
      if (!device || device->KHR_pipeline_binary) return true;
      return false;
   case 56:
      /* CreateGraphicsPipelines */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 57:
      /* CreateComputePipelines */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 58:
      /* GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI */
      if (!device || device->HUAWEI_subpass_shading) return true;
      return false;
   case 59:
      /* DestroyPipeline */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 60:
      /* CreatePipelineLayout */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 61:
      /* DestroyPipelineLayout */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 62:
      /* CreateSampler */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 63:
      /* DestroySampler */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 64:
      /* CreateDescriptorSetLayout */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 65:
      /* DestroyDescriptorSetLayout */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 66:
      /* CreateDescriptorPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 67:
      /* DestroyDescriptorPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 68:
      /* ResetDescriptorPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 69:
      /* AllocateDescriptorSets */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 70:
      /* FreeDescriptorSets */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 71:
      /* UpdateDescriptorSets */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 72:
      /* CreateFramebuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 73:
      /* DestroyFramebuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 74:
      /* CreateRenderPass */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 75:
      /* DestroyRenderPass */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 76:
      /* GetRenderAreaGranularity */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 77:
      /* GetRenderingAreaGranularity */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 78:
      /* GetRenderingAreaGranularityKHR */
      if (!device || device->KHR_maintenance5) return true;
      return false;
   case 79:
      /* CreateCommandPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 80:
      /* DestroyCommandPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 81:
      /* ResetCommandPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 82:
      /* AllocateCommandBuffers */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 83:
      /* FreeCommandBuffers */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 84:
      /* BeginCommandBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 85:
      /* EndCommandBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 86:
      /* ResetCommandBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 87:
      /* CmdBindPipeline */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 88:
      /* CmdSetPrimitiveRestartIndexEXT */
      if (!device || device->EXT_primitive_restart_index) return true;
      return false;
   case 89:
      /* CmdSetAttachmentFeedbackLoopEnableEXT */
      if (!device || device->EXT_attachment_feedback_loop_dynamic_state) return true;
      return false;
   case 90:
      /* CmdSetViewport */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 91:
      /* CmdSetScissor */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 92:
      /* CmdSetLineWidth */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 93:
      /* CmdSetDepthBias */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 94:
      /* CmdSetBlendConstants */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 95:
      /* CmdSetDepthBounds */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 96:
      /* CmdSetStencilCompareMask */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 97:
      /* CmdSetStencilWriteMask */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 98:
      /* CmdSetStencilReference */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 99:
      /* CmdBindDescriptorSets */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 100:
      /* CmdBindIndexBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 101:
      /* CmdBindVertexBuffers */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 102:
      /* CmdDraw */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 103:
      /* CmdDrawIndexed */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 104:
      /* CmdDrawMultiEXT */
      if (!device || device->EXT_multi_draw) return true;
      return false;
   case 105:
      /* CmdDrawMultiIndexedEXT */
      if (!device || device->EXT_multi_draw) return true;
      return false;
   case 106:
      /* CmdDrawIndirect */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 107:
      /* CmdDrawIndexedIndirect */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 108:
      /* CmdDispatch */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 109:
      /* CmdDispatchIndirect */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 110:
      /* CmdSubpassShadingHUAWEI */
      if (!device || device->HUAWEI_subpass_shading) return true;
      return false;
   case 111:
      /* CmdDrawClusterHUAWEI */
      if (!device || device->HUAWEI_cluster_culling_shader) return true;
      return false;
   case 112:
      /* CmdDrawClusterIndirectHUAWEI */
      if (!device || device->HUAWEI_cluster_culling_shader) return true;
      return false;
   case 113:
      /* CmdUpdatePipelineIndirectBufferNV */
      if (!device || device->NV_device_generated_commands_compute) return true;
      return false;
   case 114:
      /* CmdCopyBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 115:
      /* CmdCopyImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 116:
      /* CmdBlitImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 117:
      /* CmdCopyBufferToImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 118:
      /* CmdCopyImageToBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 119:
      /* CmdCopyMemoryIndirectNV */
      if (!device || device->NV_copy_memory_indirect) return true;
      return false;
   case 120:
      /* CmdCopyMemoryIndirectKHR */
      if (!device || device->KHR_copy_memory_indirect) return true;
      return false;
   case 121:
      /* CmdCopyMemoryToImageIndirectNV */
      if (!device || device->NV_copy_memory_indirect) return true;
      return false;
   case 122:
      /* CmdCopyMemoryToImageIndirectKHR */
      if (!device || device->KHR_copy_memory_indirect) return true;
      return false;
   case 123:
      /* CmdUpdateBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 124:
      /* CmdFillBuffer */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 125:
      /* CmdClearColorImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 126:
      /* CmdClearDepthStencilImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 127:
      /* CmdClearAttachments */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 128:
      /* CmdResolveImage */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 129:
      /* CmdSetEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 130:
      /* CmdResetEvent */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 131:
      /* CmdWaitEvents */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 132:
      /* CmdPipelineBarrier */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 133:
      /* CmdBeginQuery */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 134:
      /* CmdEndQuery */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 135:
      /* CmdBeginConditionalRenderingEXT */
      if (!device || device->EXT_conditional_rendering) return true;
      return false;
   case 136:
      /* CmdEndConditionalRenderingEXT */
      if (!device || device->EXT_conditional_rendering) return true;
      return false;
   case 137:
      /* CmdBeginCustomResolveEXT */
      if (!device || device->EXT_custom_resolve) return true;
      return false;
   case 138:
      /* CmdResetQueryPool */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 139:
      /* CmdWriteTimestamp */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 140:
      /* CmdCopyQueryPoolResults */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 141:
      /* CmdPushConstants */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 142:
      /* CmdBeginRenderPass */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 143:
      /* CmdNextSubpass */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 144:
      /* CmdEndRenderPass */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 145:
      /* CmdExecuteCommands */
      return VK_MAKE_VERSION(1, 0, 0) <= core_version;
   case 146:
      /* CreateSharedSwapchainsKHR */
      if (!device || device->KHR_display_swapchain) return true;
      return false;
   case 147:
      /* CreateSwapchainKHR */
      if (!device || device->KHR_swapchain) return true;
      return false;
   case 148:
      /* DestroySwapchainKHR */
      if (!device || device->KHR_swapchain) return true;
      return false;
   case 149:
      /* GetSwapchainImagesKHR */
      if (!device || device->KHR_swapchain) return true;
      return false;
   case 150:
      /* AcquireNextImageKHR */
      if (!device || device->KHR_swapchain) return true;
      return false;
   case 151:
      /* QueuePresentKHR */
      if (!device || device->KHR_swapchain) return true;
      return false;
   case 152:
      /* DebugMarkerSetObjectNameEXT */
      if (!device || device->EXT_debug_marker) return true;
      return false;
   case 153:
      /* DebugMarkerSetObjectTagEXT */
      if (!device || device->EXT_debug_marker) return true;
      return false;
   case 154:
      /* CmdDebugMarkerBeginEXT */
      if (!device || device->EXT_debug_marker) return true;
      return false;
   case 155:
      /* CmdDebugMarkerEndEXT */
      if (!device || device->EXT_debug_marker) return true;
      return false;
   case 156:
      /* CmdDebugMarkerInsertEXT */
      if (!device || device->EXT_debug_marker) return true;
      return false;
   case 157:
      /* GetMemoryWin32HandleNV */
      if (!device || device->NV_external_memory_win32) return true;
      return false;
   case 158:
      /* CmdExecuteGeneratedCommandsNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 159:
      /* CmdPreprocessGeneratedCommandsNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 160:
      /* CmdBindPipelineShaderGroupNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 161:
      /* GetGeneratedCommandsMemoryRequirementsNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 162:
      /* CreateIndirectCommandsLayoutNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 163:
      /* DestroyIndirectCommandsLayoutNV */
      if (!device || device->NV_device_generated_commands) return true;
      return false;
   case 164:
      /* CmdExecuteGeneratedCommandsEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 165:
      /* CmdPreprocessGeneratedCommandsEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 166:
      /* GetGeneratedCommandsMemoryRequirementsEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 167:
      /* CreateIndirectCommandsLayoutEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 168:
      /* DestroyIndirectCommandsLayoutEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 169:
      /* CreateIndirectExecutionSetEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 170:
      /* DestroyIndirectExecutionSetEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 171:
      /* UpdateIndirectExecutionSetPipelineEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 172:
      /* UpdateIndirectExecutionSetShaderEXT */
      if (!device || device->EXT_device_generated_commands) return true;
      return false;
   case 173:
      /* CmdPushDescriptorSet */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 174:
      /* CmdPushDescriptorSetKHR */
      if (!device || device->KHR_push_descriptor) return true;
      return false;
   case 175:
      /* TrimCommandPool */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 176:
      /* TrimCommandPoolKHR */
      if (!device || device->KHR_maintenance1) return true;
      return false;
   case 177:
      /* GetMemoryWin32HandleKHR */
      if (!device || device->KHR_external_memory_win32) return true;
      return false;
   case 178:
      /* GetMemoryWin32HandlePropertiesKHR */
      if (!device || device->KHR_external_memory_win32) return true;
      return false;
   case 179:
      /* GetMemoryFdKHR */
      if (!device || device->KHR_external_memory_fd) return true;
      return false;
   case 180:
      /* GetMemoryFdPropertiesKHR */
      if (!device || device->KHR_external_memory_fd) return true;
      return false;
   case 181:
      /* GetMemoryZirconHandleFUCHSIA */
      if (!device || device->FUCHSIA_external_memory) return true;
      return false;
   case 182:
      /* GetMemoryZirconHandlePropertiesFUCHSIA */
      if (!device || device->FUCHSIA_external_memory) return true;
      return false;
   case 183:
      /* GetMemoryRemoteAddressNV */
      if (!device || device->NV_external_memory_rdma) return true;
      return false;
   case 184:
      /* GetSemaphoreWin32HandleKHR */
      if (!device || device->KHR_external_semaphore_win32) return true;
      return false;
   case 185:
      /* ImportSemaphoreWin32HandleKHR */
      if (!device || device->KHR_external_semaphore_win32) return true;
      return false;
   case 186:
      /* GetSemaphoreFdKHR */
      if (!device || device->KHR_external_semaphore_fd) return true;
      return false;
   case 187:
      /* ImportSemaphoreFdKHR */
      if (!device || device->KHR_external_semaphore_fd) return true;
      return false;
   case 188:
      /* GetSemaphoreZirconHandleFUCHSIA */
      if (!device || device->FUCHSIA_external_semaphore) return true;
      return false;
   case 189:
      /* ImportSemaphoreZirconHandleFUCHSIA */
      if (!device || device->FUCHSIA_external_semaphore) return true;
      return false;
   case 190:
      /* GetFenceWin32HandleKHR */
      if (!device || device->KHR_external_fence_win32) return true;
      return false;
   case 191:
      /* ImportFenceWin32HandleKHR */
      if (!device || device->KHR_external_fence_win32) return true;
      return false;
   case 192:
      /* GetFenceFdKHR */
      if (!device || device->KHR_external_fence_fd) return true;
      return false;
   case 193:
      /* ImportFenceFdKHR */
      if (!device || device->KHR_external_fence_fd) return true;
      return false;
   case 194:
      /* DisplayPowerControlEXT */
      if (!device || device->EXT_display_control) return true;
      return false;
   case 195:
      /* RegisterDeviceEventEXT */
      if (!device || device->EXT_display_control) return true;
      return false;
   case 196:
      /* RegisterDisplayEventEXT */
      if (!device || device->EXT_display_control) return true;
      return false;
   case 197:
      /* GetSwapchainCounterEXT */
      if (!device || device->EXT_display_control) return true;
      return false;
   case 198:
      /* GetDeviceGroupPeerMemoryFeatures */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 199:
      /* GetDeviceGroupPeerMemoryFeaturesKHR */
      if (!device || device->KHR_device_group) return true;
      return false;
   case 200:
      /* BindBufferMemory2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 201:
      /* BindBufferMemory2KHR */
      if (!device || device->KHR_bind_memory2) return true;
      return false;
   case 202:
      /* BindImageMemory2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 203:
      /* BindImageMemory2KHR */
      if (!device || device->KHR_bind_memory2) return true;
      return false;
   case 204:
      /* CmdSetDeviceMask */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 205:
      /* CmdSetDeviceMaskKHR */
      if (!device || device->KHR_device_group) return true;
      return false;
   case 206:
      /* GetDeviceGroupPresentCapabilitiesKHR */
      if (!device || device->KHR_swapchain) return true;
      if (!device || device->KHR_device_group) return true;
      return false;
   case 207:
      /* GetDeviceGroupSurfacePresentModesKHR */
      if (!device || device->KHR_swapchain) return true;
      if (!device || device->KHR_device_group) return true;
      return false;
   case 208:
      /* AcquireNextImage2KHR */
      if (!device || device->KHR_swapchain) return true;
      if (!device || device->KHR_device_group) return true;
      return false;
   case 209:
      /* CmdDispatchBase */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 210:
      /* CmdDispatchBaseKHR */
      if (!device || device->KHR_device_group) return true;
      return false;
   case 211:
      /* CreateDescriptorUpdateTemplate */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 212:
      /* CreateDescriptorUpdateTemplateKHR */
      if (!device || device->KHR_descriptor_update_template) return true;
      return false;
   case 213:
      /* DestroyDescriptorUpdateTemplate */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 214:
      /* DestroyDescriptorUpdateTemplateKHR */
      if (!device || device->KHR_descriptor_update_template) return true;
      return false;
   case 215:
      /* UpdateDescriptorSetWithTemplate */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 216:
      /* UpdateDescriptorSetWithTemplateKHR */
      if (!device || device->KHR_descriptor_update_template) return true;
      return false;
   case 217:
      /* CmdPushDescriptorSetWithTemplate */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 218:
      /* CmdPushDescriptorSetWithTemplateKHR */
      if (!device || device->KHR_push_descriptor) return true;
      if (!device || device->KHR_descriptor_update_template) return true;
      return false;
   case 219:
      /* SetHdrMetadataEXT */
      if (!device || device->EXT_hdr_metadata) return true;
      return false;
   case 220:
      /* GetSwapchainStatusKHR */
      if (!device || device->KHR_shared_presentable_image) return true;
      return false;
   case 221:
      /* GetRefreshCycleDurationGOOGLE */
      if (!device || device->GOOGLE_display_timing) return true;
      return false;
   case 222:
      /* GetPastPresentationTimingGOOGLE */
      if (!device || device->GOOGLE_display_timing) return true;
      return false;
   case 223:
      /* CmdSetViewportWScalingNV */
      if (!device || device->NV_clip_space_w_scaling) return true;
      return false;
   case 224:
      /* CmdSetDiscardRectangleEXT */
      if (!device || device->EXT_discard_rectangles) return true;
      return false;
   case 225:
      /* CmdSetDiscardRectangleEnableEXT */
      if (!device || device->EXT_discard_rectangles) return true;
      return false;
   case 226:
      /* CmdSetDiscardRectangleModeEXT */
      if (!device || device->EXT_discard_rectangles) return true;
      return false;
   case 227:
      /* CmdSetSampleLocationsEXT */
      if (!device || device->EXT_sample_locations) return true;
      return false;
   case 228:
      /* GetBufferMemoryRequirements2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 229:
      /* GetBufferMemoryRequirements2KHR */
      if (!device || device->KHR_get_memory_requirements2) return true;
      return false;
   case 230:
      /* GetImageMemoryRequirements2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 231:
      /* GetImageMemoryRequirements2KHR */
      if (!device || device->KHR_get_memory_requirements2) return true;
      return false;
   case 232:
      /* GetImageSparseMemoryRequirements2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 233:
      /* GetImageSparseMemoryRequirements2KHR */
      if (!device || device->KHR_get_memory_requirements2) return true;
      return false;
   case 234:
      /* GetDeviceBufferMemoryRequirements */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 235:
      /* GetDeviceBufferMemoryRequirementsKHR */
      if (!device || device->KHR_maintenance4) return true;
      return false;
   case 236:
      /* GetDeviceImageMemoryRequirements */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 237:
      /* GetDeviceImageMemoryRequirementsKHR */
      if (!device || device->KHR_maintenance4) return true;
      return false;
   case 238:
      /* GetDeviceImageSparseMemoryRequirements */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 239:
      /* GetDeviceImageSparseMemoryRequirementsKHR */
      if (!device || device->KHR_maintenance4) return true;
      return false;
   case 240:
      /* CreateSamplerYcbcrConversion */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 241:
      /* CreateSamplerYcbcrConversionKHR */
      if (!device || device->KHR_sampler_ycbcr_conversion) return true;
      return false;
   case 242:
      /* DestroySamplerYcbcrConversion */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 243:
      /* DestroySamplerYcbcrConversionKHR */
      if (!device || device->KHR_sampler_ycbcr_conversion) return true;
      return false;
   case 244:
      /* GetDeviceQueue2 */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 245:
      /* CreateValidationCacheEXT */
      if (!device || device->EXT_validation_cache) return true;
      return false;
   case 246:
      /* DestroyValidationCacheEXT */
      if (!device || device->EXT_validation_cache) return true;
      return false;
   case 247:
      /* GetValidationCacheDataEXT */
      if (!device || device->EXT_validation_cache) return true;
      return false;
   case 248:
      /* MergeValidationCachesEXT */
      if (!device || device->EXT_validation_cache) return true;
      return false;
   case 249:
      /* GetDescriptorSetLayoutSupport */
      return VK_MAKE_VERSION(1, 1, 0) <= core_version;
   case 250:
      /* GetDescriptorSetLayoutSupportKHR */
      if (!device || device->KHR_maintenance3) return true;
      return false;
   case 251:
      /* GetSwapchainGrallocUsageANDROID */
      if (!device || device->ANDROID_native_buffer) return true;
      return false;
   case 252:
      /* GetSwapchainGrallocUsage2ANDROID */
      if (!device || device->ANDROID_native_buffer) return true;
      return false;
   case 253:
      /* AcquireImageANDROID */
      if (!device || device->ANDROID_native_buffer) return true;
      return false;
   case 254:
      /* QueueSignalReleaseImageANDROID */
      if (!device || device->ANDROID_native_buffer) return true;
      return false;
   case 255:
      /* GetShaderInfoAMD */
      if (!device || device->AMD_shader_info) return true;
      return false;
   case 256:
      /* SetLocalDimmingAMD */
      if (!device || device->AMD_display_native_hdr) return true;
      return false;
   case 257:
      /* GetCalibratedTimestampsKHR */
      if (!device || device->KHR_calibrated_timestamps) return true;
      return false;
   case 258:
      /* GetCalibratedTimestampsEXT */
      if (!device || device->EXT_calibrated_timestamps) return true;
      return false;
   case 259:
      /* SetDebugUtilsObjectNameEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 260:
      /* SetDebugUtilsObjectTagEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 261:
      /* QueueBeginDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 262:
      /* QueueEndDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 263:
      /* QueueInsertDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 264:
      /* CmdBeginDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 265:
      /* CmdEndDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 266:
      /* CmdInsertDebugUtilsLabelEXT */
      if (instance->EXT_debug_utils) return true;
      return false;
   case 267:
      /* GetMemoryHostPointerPropertiesEXT */
      if (!device || device->EXT_external_memory_host) return true;
      return false;
   case 268:
      /* CmdWriteBufferMarkerAMD */
      if (!device || device->AMD_buffer_marker) return true;
      return false;
   case 269:
      /* CreateRenderPass2 */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 270:
      /* CreateRenderPass2KHR */
      if (!device || device->KHR_create_renderpass2) return true;
      return false;
   case 271:
      /* CmdBeginRenderPass2 */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 272:
      /* CmdBeginRenderPass2KHR */
      if (!device || device->KHR_create_renderpass2) return true;
      return false;
   case 273:
      /* CmdNextSubpass2 */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 274:
      /* CmdNextSubpass2KHR */
      if (!device || device->KHR_create_renderpass2) return true;
      return false;
   case 275:
      /* CmdEndRenderPass2 */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 276:
      /* CmdEndRenderPass2KHR */
      if (!device || device->KHR_create_renderpass2) return true;
      return false;
   case 277:
      /* GetSemaphoreCounterValue */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 278:
      /* GetSemaphoreCounterValueKHR */
      if (!device || device->KHR_timeline_semaphore) return true;
      return false;
   case 279:
      /* WaitSemaphores */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 280:
      /* WaitSemaphoresKHR */
      if (!device || device->KHR_timeline_semaphore) return true;
      return false;
   case 281:
      /* SignalSemaphore */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 282:
      /* SignalSemaphoreKHR */
      if (!device || device->KHR_timeline_semaphore) return true;
      return false;
   case 283:
      /* GetAndroidHardwareBufferPropertiesANDROID */
      if (!device || device->ANDROID_external_memory_android_hardware_buffer) return true;
      return false;
   case 284:
      /* GetMemoryAndroidHardwareBufferANDROID */
      if (!device || device->ANDROID_external_memory_android_hardware_buffer) return true;
      return false;
   case 285:
      /* CmdDrawIndirectCount */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 286:
      /* CmdDrawIndirectCountKHR */
      if (!device || device->KHR_draw_indirect_count) return true;
      return false;
   case 287:
      /* CmdDrawIndirectCountAMD */
      if (!device || device->AMD_draw_indirect_count) return true;
      return false;
   case 288:
      /* CmdDrawIndexedIndirectCount */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 289:
      /* CmdDrawIndexedIndirectCountKHR */
      if (!device || device->KHR_draw_indirect_count) return true;
      return false;
   case 290:
      /* CmdDrawIndexedIndirectCountAMD */
      if (!device || device->AMD_draw_indirect_count) return true;
      return false;
   case 291:
      /* CmdSetCheckpointNV */
      if (!device || device->NV_device_diagnostic_checkpoints) return true;
      return false;
   case 292:
      /* GetQueueCheckpointDataNV */
      if (!device || device->NV_device_diagnostic_checkpoints) return true;
      return false;
   case 293:
      /* CmdBindTransformFeedbackBuffersEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 294:
      /* CmdBeginTransformFeedbackEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 295:
      /* CmdEndTransformFeedbackEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 296:
      /* CmdBeginQueryIndexedEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 297:
      /* CmdEndQueryIndexedEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 298:
      /* CmdDrawIndirectByteCountEXT */
      if (!device || device->EXT_transform_feedback) return true;
      return false;
   case 299:
      /* CmdSetExclusiveScissorNV */
      if (!device || device->NV_scissor_exclusive) return true;
      return false;
   case 300:
      /* CmdSetExclusiveScissorEnableNV */
      if (!device || device->NV_scissor_exclusive) return true;
      return false;
   case 301:
      /* CmdBindShadingRateImageNV */
      if (!device || device->NV_shading_rate_image) return true;
      return false;
   case 302:
      /* CmdSetViewportShadingRatePaletteNV */
      if (!device || device->NV_shading_rate_image) return true;
      return false;
   case 303:
      /* CmdSetCoarseSampleOrderNV */
      if (!device || device->NV_shading_rate_image) return true;
      return false;
   case 304:
      /* CmdDrawMeshTasksNV */
      if (!device || device->NV_mesh_shader) return true;
      return false;
   case 305:
      /* CmdDrawMeshTasksIndirectNV */
      if (!device || device->NV_mesh_shader) return true;
      return false;
   case 306:
      /* CmdDrawMeshTasksIndirectCountNV */
      if (!device || device->NV_mesh_shader) return true;
      return false;
   case 307:
      /* CmdDrawMeshTasksEXT */
      if (!device || device->EXT_mesh_shader) return true;
      return false;
   case 308:
      /* CmdDrawMeshTasksIndirectEXT */
      if (!device || device->EXT_mesh_shader) return true;
      return false;
   case 309:
      /* CmdDrawMeshTasksIndirectCountEXT */
      if (!device || device->EXT_mesh_shader) return true;
      return false;
   case 310:
      /* CompileDeferredNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 311:
      /* CreateAccelerationStructureNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 312:
      /* CmdBindInvocationMaskHUAWEI */
      if (!device || device->HUAWEI_invocation_mask) return true;
      return false;
   case 313:
      /* DestroyAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 314:
      /* DestroyAccelerationStructureNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 315:
      /* GetAccelerationStructureMemoryRequirementsNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 316:
      /* BindAccelerationStructureMemoryNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 317:
      /* CmdCopyAccelerationStructureNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 318:
      /* CmdCopyAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 319:
      /* CopyAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 320:
      /* CmdCopyAccelerationStructureToMemoryKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 321:
      /* CopyAccelerationStructureToMemoryKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 322:
      /* CmdCopyMemoryToAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 323:
      /* CopyMemoryToAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 324:
      /* CmdWriteAccelerationStructuresPropertiesKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 325:
      /* CmdWriteAccelerationStructuresPropertiesNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 326:
      /* CmdBuildAccelerationStructureNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 327:
      /* WriteAccelerationStructuresPropertiesKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 328:
      /* CmdTraceRaysKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 329:
      /* CmdTraceRaysNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 330:
      /* GetRayTracingShaderGroupHandlesKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 331:
      /* GetRayTracingShaderGroupHandlesNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 332:
      /* GetRayTracingCaptureReplayShaderGroupHandlesKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 333:
      /* GetAccelerationStructureHandleNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 334:
      /* CreateRayTracingPipelinesNV */
      if (!device || device->NV_ray_tracing) return true;
      return false;
   case 335:
      /* CreateRayTracingPipelinesKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 336:
      /* CmdTraceRaysIndirectKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 337:
      /* CmdTraceRaysIndirect2KHR */
      if (!device || device->KHR_ray_tracing_maintenance1) return true;
      return false;
   case 338:
      /* GetClusterAccelerationStructureBuildSizesNV */
      if (!device || device->NV_cluster_acceleration_structure) return true;
      return false;
   case 339:
      /* CmdBuildClusterAccelerationStructureIndirectNV */
      if (!device || device->NV_cluster_acceleration_structure) return true;
      return false;
   case 340:
      /* GetDeviceAccelerationStructureCompatibilityKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 341:
      /* GetRayTracingShaderGroupStackSizeKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 342:
      /* CmdSetRayTracingPipelineStackSizeKHR */
      if (!device || device->KHR_ray_tracing_pipeline) return true;
      return false;
   case 343:
      /* GetImageViewHandleNVX */
      if (!device || device->NVX_image_view_handle) return true;
      return false;
   case 344:
      /* GetImageViewHandle64NVX */
      if (!device || device->NVX_image_view_handle) return true;
      return false;
   case 345:
      /* GetImageViewAddressNVX */
      if (!device || device->NVX_image_view_handle) return true;
      return false;
   case 346:
      /* GetDeviceCombinedImageSamplerIndexNVX */
      if (!device || device->NVX_image_view_handle) return true;
      return false;
   case 347:
      /* GetDeviceGroupSurfacePresentModes2EXT */
      if (!device || device->EXT_full_screen_exclusive) return true;
      return false;
   case 348:
      /* AcquireFullScreenExclusiveModeEXT */
      if (!device || device->EXT_full_screen_exclusive) return true;
      return false;
   case 349:
      /* ReleaseFullScreenExclusiveModeEXT */
      if (!device || device->EXT_full_screen_exclusive) return true;
      return false;
   case 350:
      /* AcquireProfilingLockKHR */
      if (!device || device->KHR_performance_query) return true;
      return false;
   case 351:
      /* ReleaseProfilingLockKHR */
      if (!device || device->KHR_performance_query) return true;
      return false;
   case 352:
      /* GetImageDrmFormatModifierPropertiesEXT */
      if (!device || device->EXT_image_drm_format_modifier) return true;
      return false;
   case 353:
      /* GetBufferOpaqueCaptureAddress */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 354:
      /* GetBufferOpaqueCaptureAddressKHR */
      if (!device || device->KHR_buffer_device_address) return true;
      return false;
   case 355:
      /* GetBufferDeviceAddress */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 356:
      /* GetBufferDeviceAddressKHR */
      if (!device || device->KHR_buffer_device_address) return true;
      return false;
   case 357:
      /* GetBufferDeviceAddressEXT */
      if (!device || device->EXT_buffer_device_address) return true;
      return false;
   case 358:
      /* InitializePerformanceApiINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 359:
      /* UninitializePerformanceApiINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 360:
      /* CmdSetPerformanceMarkerINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 361:
      /* CmdSetPerformanceStreamMarkerINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 362:
      /* CmdSetPerformanceOverrideINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 363:
      /* AcquirePerformanceConfigurationINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 364:
      /* ReleasePerformanceConfigurationINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 365:
      /* QueueSetPerformanceConfigurationINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 366:
      /* GetPerformanceParameterINTEL */
      if (!device || device->INTEL_performance_query) return true;
      return false;
   case 367:
      /* GetDeviceMemoryOpaqueCaptureAddress */
      return VK_MAKE_VERSION(1, 2, 0) <= core_version;
   case 368:
      /* GetDeviceMemoryOpaqueCaptureAddressKHR */
      if (!device || device->KHR_buffer_device_address) return true;
      return false;
   case 369:
      /* GetPipelineExecutablePropertiesKHR */
      if (!device || device->KHR_pipeline_executable_properties) return true;
      return false;
   case 370:
      /* GetPipelineExecutableStatisticsKHR */
      if (!device || device->KHR_pipeline_executable_properties) return true;
      return false;
   case 371:
      /* GetPipelineExecutableInternalRepresentationsKHR */
      if (!device || device->KHR_pipeline_executable_properties) return true;
      return false;
   case 372:
      /* CmdSetLineStipple */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 373:
      /* CmdSetLineStippleKHR */
      if (!device || device->KHR_line_rasterization) return true;
      return false;
   case 374:
      /* CmdSetLineStippleEXT */
      if (!device || device->EXT_line_rasterization) return true;
      return false;
   case 375:
      /* CreateAccelerationStructureKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 376:
      /* CmdBuildAccelerationStructuresKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 377:
      /* CmdBuildAccelerationStructuresIndirectKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 378:
      /* BuildAccelerationStructuresKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 379:
      /* GetAccelerationStructureDeviceAddressKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 380:
      /* CreateDeferredOperationKHR */
      if (!device || device->KHR_deferred_host_operations) return true;
      return false;
   case 381:
      /* DestroyDeferredOperationKHR */
      if (!device || device->KHR_deferred_host_operations) return true;
      return false;
   case 382:
      /* GetDeferredOperationMaxConcurrencyKHR */
      if (!device || device->KHR_deferred_host_operations) return true;
      return false;
   case 383:
      /* GetDeferredOperationResultKHR */
      if (!device || device->KHR_deferred_host_operations) return true;
      return false;
   case 384:
      /* DeferredOperationJoinKHR */
      if (!device || device->KHR_deferred_host_operations) return true;
      return false;
   case 385:
      /* GetPipelineIndirectMemoryRequirementsNV */
      if (!device || device->NV_device_generated_commands_compute) return true;
      return false;
   case 386:
      /* GetPipelineIndirectDeviceAddressNV */
      if (!device || device->NV_device_generated_commands_compute) return true;
      return false;
   case 387:
      /* AntiLagUpdateAMD */
      if (!device || device->AMD_anti_lag) return true;
      return false;
   case 388:
      /* CmdSetCullMode */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 389:
      /* CmdSetCullModeEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 390:
      /* CmdSetFrontFace */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 391:
      /* CmdSetFrontFaceEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 392:
      /* CmdSetPrimitiveTopology */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 393:
      /* CmdSetPrimitiveTopologyEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 394:
      /* CmdSetViewportWithCount */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 395:
      /* CmdSetViewportWithCountEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 396:
      /* CmdSetScissorWithCount */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 397:
      /* CmdSetScissorWithCountEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 398:
      /* CmdBindIndexBuffer2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 399:
      /* CmdBindIndexBuffer2KHR */
      if (!device || device->KHR_maintenance5) return true;
      return false;
   case 400:
      /* CmdBindVertexBuffers2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 401:
      /* CmdBindVertexBuffers2EXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 402:
      /* CmdSetDepthTestEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 403:
      /* CmdSetDepthTestEnableEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 404:
      /* CmdSetDepthWriteEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 405:
      /* CmdSetDepthWriteEnableEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 406:
      /* CmdSetDepthCompareOp */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 407:
      /* CmdSetDepthCompareOpEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 408:
      /* CmdSetDepthBoundsTestEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 409:
      /* CmdSetDepthBoundsTestEnableEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 410:
      /* CmdSetStencilTestEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 411:
      /* CmdSetStencilTestEnableEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 412:
      /* CmdSetStencilOp */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 413:
      /* CmdSetStencilOpEXT */
      if (!device || device->EXT_extended_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 414:
      /* CmdSetPatchControlPointsEXT */
      if (!device || device->EXT_extended_dynamic_state2) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 415:
      /* CmdSetRasterizerDiscardEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 416:
      /* CmdSetRasterizerDiscardEnableEXT */
      if (!device || device->EXT_extended_dynamic_state2) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 417:
      /* CmdSetDepthBiasEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 418:
      /* CmdSetDepthBiasEnableEXT */
      if (!device || device->EXT_extended_dynamic_state2) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 419:
      /* CmdSetLogicOpEXT */
      if (!device || device->EXT_extended_dynamic_state2) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 420:
      /* CmdSetPrimitiveRestartEnable */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 421:
      /* CmdSetPrimitiveRestartEnableEXT */
      if (!device || device->EXT_extended_dynamic_state2) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 422:
      /* CmdSetTessellationDomainOriginEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 423:
      /* CmdSetDepthClampEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 424:
      /* CmdSetPolygonModeEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 425:
      /* CmdSetRasterizationSamplesEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 426:
      /* CmdSetSampleMaskEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 427:
      /* CmdSetAlphaToCoverageEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 428:
      /* CmdSetAlphaToOneEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 429:
      /* CmdSetLogicOpEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 430:
      /* CmdSetColorBlendEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 431:
      /* CmdSetColorBlendEquationEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 432:
      /* CmdSetColorWriteMaskEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 433:
      /* CmdSetRasterizationStreamEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 434:
      /* CmdSetConservativeRasterizationModeEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 435:
      /* CmdSetExtraPrimitiveOverestimationSizeEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 436:
      /* CmdSetDepthClipEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 437:
      /* CmdSetSampleLocationsEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 438:
      /* CmdSetColorBlendAdvancedEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 439:
      /* CmdSetProvokingVertexModeEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 440:
      /* CmdSetLineRasterizationModeEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 441:
      /* CmdSetLineStippleEnableEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 442:
      /* CmdSetDepthClipNegativeOneToOneEXT */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 443:
      /* CmdSetViewportWScalingEnableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 444:
      /* CmdSetViewportSwizzleNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 445:
      /* CmdSetCoverageToColorEnableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 446:
      /* CmdSetCoverageToColorLocationNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 447:
      /* CmdSetCoverageModulationModeNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 448:
      /* CmdSetCoverageModulationTableEnableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 449:
      /* CmdSetCoverageModulationTableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 450:
      /* CmdSetShadingRateImageEnableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 451:
      /* CmdSetCoverageReductionModeNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 452:
      /* CmdSetRepresentativeFragmentTestEnableNV */
      if (!device || device->EXT_extended_dynamic_state3) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 453:
      /* CreatePrivateDataSlot */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 454:
      /* CreatePrivateDataSlotEXT */
      if (!device || device->EXT_private_data) return true;
      return false;
   case 455:
      /* DestroyPrivateDataSlot */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 456:
      /* DestroyPrivateDataSlotEXT */
      if (!device || device->EXT_private_data) return true;
      return false;
   case 457:
      /* SetPrivateData */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 458:
      /* SetPrivateDataEXT */
      if (!device || device->EXT_private_data) return true;
      return false;
   case 459:
      /* GetPrivateData */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 460:
      /* GetPrivateDataEXT */
      if (!device || device->EXT_private_data) return true;
      return false;
   case 461:
      /* CmdCopyBuffer2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 462:
      /* CmdCopyBuffer2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 463:
      /* CmdCopyImage2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 464:
      /* CmdCopyImage2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 465:
      /* CmdBlitImage2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 466:
      /* CmdBlitImage2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 467:
      /* CmdCopyBufferToImage2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 468:
      /* CmdCopyBufferToImage2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 469:
      /* CmdCopyImageToBuffer2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 470:
      /* CmdCopyImageToBuffer2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 471:
      /* CmdResolveImage2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 472:
      /* CmdResolveImage2KHR */
      if (!device || device->KHR_copy_commands2) return true;
      return false;
   case 473:
      /* CmdSetFragmentShadingRateKHR */
      if (!device || device->KHR_fragment_shading_rate) return true;
      return false;
   case 474:
      /* CmdSetFragmentShadingRateEnumNV */
      if (!device || device->NV_fragment_shading_rate_enums) return true;
      return false;
   case 475:
      /* GetAccelerationStructureBuildSizesKHR */
      if (!device || device->KHR_acceleration_structure) return true;
      return false;
   case 476:
      /* CmdSetVertexInputEXT */
      if (!device || device->EXT_vertex_input_dynamic_state) return true;
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 477:
      /* CmdSetColorWriteEnableEXT */
      if (!device || device->EXT_color_write_enable) return true;
      return false;
   case 478:
      /* CmdSetEvent2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 479:
      /* CmdSetEvent2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 480:
      /* CmdResetEvent2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 481:
      /* CmdResetEvent2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 482:
      /* CmdWaitEvents2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 483:
      /* CmdWaitEvents2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 484:
      /* CmdPipelineBarrier2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 485:
      /* CmdPipelineBarrier2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 486:
      /* QueueSubmit2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 487:
      /* QueueSubmit2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 488:
      /* CmdWriteTimestamp2 */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 489:
      /* CmdWriteTimestamp2KHR */
      if (!device || device->KHR_synchronization2) return true;
      return false;
   case 490:
      /* CmdWriteBufferMarker2AMD */
      if (!device || device->AMD_buffer_marker) return true;
      return false;
   case 491:
      /* GetQueueCheckpointData2NV */
      if (!device || device->NV_device_diagnostic_checkpoints) return true;
      return false;
   case 492:
      /* CopyMemoryToImage */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 493:
      /* CopyMemoryToImageEXT */
      if (!device || device->EXT_host_image_copy) return true;
      return false;
   case 494:
      /* CopyImageToMemory */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 495:
      /* CopyImageToMemoryEXT */
      if (!device || device->EXT_host_image_copy) return true;
      return false;
   case 496:
      /* CopyImageToImage */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 497:
      /* CopyImageToImageEXT */
      if (!device || device->EXT_host_image_copy) return true;
      return false;
   case 498:
      /* TransitionImageLayout */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 499:
      /* TransitionImageLayoutEXT */
      if (!device || device->EXT_host_image_copy) return true;
      return false;
   case 500:
      /* CreateVideoSessionKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 501:
      /* DestroyVideoSessionKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 502:
      /* CreateVideoSessionParametersKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 503:
      /* UpdateVideoSessionParametersKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 504:
      /* GetEncodedVideoSessionParametersKHR */
      if (!device || device->KHR_video_encode_queue) return true;
      return false;
   case 505:
      /* DestroyVideoSessionParametersKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 506:
      /* GetVideoSessionMemoryRequirementsKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 507:
      /* BindVideoSessionMemoryKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 508:
      /* CmdDecodeVideoKHR */
      if (!device || device->KHR_video_decode_queue) return true;
      return false;
   case 509:
      /* CmdBeginVideoCodingKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 510:
      /* CmdControlVideoCodingKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 511:
      /* CmdEndVideoCodingKHR */
      if (!device || device->KHR_video_queue) return true;
      return false;
   case 512:
      /* CmdEncodeVideoKHR */
      if (!device || device->KHR_video_encode_queue) return true;
      return false;
   case 513:
      /* CmdDecompressMemoryNV */
      if (!device || device->NV_memory_decompression) return true;
      return false;
   case 514:
      /* CmdDecompressMemoryIndirectCountNV */
      if (!device || device->NV_memory_decompression) return true;
      return false;
   case 515:
      /* GetPartitionedAccelerationStructuresBuildSizesNV */
      if (!device || device->NV_partitioned_acceleration_structure) return true;
      return false;
   case 516:
      /* CmdBuildPartitionedAccelerationStructuresNV */
      if (!device || device->NV_partitioned_acceleration_structure) return true;
      return false;
   case 517:
      /* CmdDecompressMemoryEXT */
      if (!device || device->EXT_memory_decompression) return true;
      return false;
   case 518:
      /* CmdDecompressMemoryIndirectCountEXT */
      if (!device || device->EXT_memory_decompression) return true;
      return false;
   case 519:
      /* CreateCuModuleNVX */
      if (!device || device->NVX_binary_import) return true;
      return false;
   case 520:
      /* CreateCuFunctionNVX */
      if (!device || device->NVX_binary_import) return true;
      return false;
   case 521:
      /* DestroyCuModuleNVX */
      if (!device || device->NVX_binary_import) return true;
      return false;
   case 522:
      /* DestroyCuFunctionNVX */
      if (!device || device->NVX_binary_import) return true;
      return false;
   case 523:
      /* CmdCuLaunchKernelNVX */
      if (!device || device->NVX_binary_import) return true;
      return false;
   case 524:
      /* GetDescriptorSetLayoutSizeEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 525:
      /* GetDescriptorSetLayoutBindingOffsetEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 526:
      /* GetDescriptorEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 527:
      /* CmdBindDescriptorBuffersEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 528:
      /* CmdSetDescriptorBufferOffsetsEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 529:
      /* CmdBindDescriptorBufferEmbeddedSamplersEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 530:
      /* GetBufferOpaqueCaptureDescriptorDataEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 531:
      /* GetImageOpaqueCaptureDescriptorDataEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 532:
      /* GetImageViewOpaqueCaptureDescriptorDataEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 533:
      /* GetSamplerOpaqueCaptureDescriptorDataEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 534:
      /* GetAccelerationStructureOpaqueCaptureDescriptorDataEXT */
      if (!device || device->EXT_descriptor_buffer) return true;
      return false;
   case 535:
      /* SetDeviceMemoryPriorityEXT */
      if (!device || device->EXT_pageable_device_local_memory) return true;
      return false;
   case 536:
      /* WaitForPresent2KHR */
      if (!device || device->KHR_present_wait2) return true;
      return false;
   case 537:
      /* WaitForPresentKHR */
      if (!device || device->KHR_present_wait) return true;
      return false;
   case 538:
      /* CreateBufferCollectionFUCHSIA */
      if (!device || device->FUCHSIA_buffer_collection) return true;
      return false;
   case 539:
      /* SetBufferCollectionBufferConstraintsFUCHSIA */
      if (!device || device->FUCHSIA_buffer_collection) return true;
      return false;
   case 540:
      /* SetBufferCollectionImageConstraintsFUCHSIA */
      if (!device || device->FUCHSIA_buffer_collection) return true;
      return false;
   case 541:
      /* DestroyBufferCollectionFUCHSIA */
      if (!device || device->FUCHSIA_buffer_collection) return true;
      return false;
   case 542:
      /* GetBufferCollectionPropertiesFUCHSIA */
      if (!device || device->FUCHSIA_buffer_collection) return true;
      return false;
   case 543:
      /* CmdBeginRendering */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 544:
      /* CmdBeginRenderingKHR */
      if (!device || device->KHR_dynamic_rendering) return true;
      return false;
   case 545:
      /* CmdEndRendering */
      return VK_MAKE_VERSION(1, 3, 0) <= core_version;
   case 546:
      /* CmdEndRendering2KHR */
      if (!device || device->KHR_maintenance10) return true;
      return false;
   case 547:
      /* CmdEndRendering2EXT */
      if (!device || device->EXT_fragment_density_map_offset) return true;
      return false;
   case 548:
      /* CmdEndRenderingKHR */
      if (!device || device->KHR_dynamic_rendering) return true;
      return false;
   case 549:
      /* GetDescriptorSetLayoutHostMappingInfoVALVE */
      if (!device || device->VALVE_descriptor_set_host_mapping) return true;
      return false;
   case 550:
      /* GetDescriptorSetHostMappingVALVE */
      if (!device || device->VALVE_descriptor_set_host_mapping) return true;
      return false;
   case 551:
      /* CreateMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 552:
      /* CmdBuildMicromapsEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 553:
      /* BuildMicromapsEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 554:
      /* DestroyMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 555:
      /* CmdCopyMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 556:
      /* CopyMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 557:
      /* CmdCopyMicromapToMemoryEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 558:
      /* CopyMicromapToMemoryEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 559:
      /* CmdCopyMemoryToMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 560:
      /* CopyMemoryToMicromapEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 561:
      /* CmdWriteMicromapsPropertiesEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 562:
      /* WriteMicromapsPropertiesEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 563:
      /* GetDeviceMicromapCompatibilityEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 564:
      /* GetMicromapBuildSizesEXT */
      if (!device || device->EXT_opacity_micromap) return true;
      return false;
   case 565:
      /* GetShaderModuleIdentifierEXT */
      if (!device || device->EXT_shader_module_identifier) return true;
      return false;
   case 566:
      /* GetShaderModuleCreateInfoIdentifierEXT */
      if (!device || device->EXT_shader_module_identifier) return true;
      return false;
   case 567:
      /* GetImageSubresourceLayout2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 568:
      /* GetImageSubresourceLayout2KHR */
      if (!device || device->KHR_maintenance5) return true;
      return false;
   case 569:
      /* GetImageSubresourceLayout2EXT */
      if (!device || device->EXT_host_image_copy) return true;
      if (!device || device->EXT_image_compression_control) return true;
      return false;
   case 570:
      /* GetPipelinePropertiesEXT */
      if (!device || device->EXT_pipeline_properties) return true;
      return false;
   case 571:
      /* ExportMetalObjectsEXT */
      if (!device || device->EXT_metal_objects) return true;
      return false;
   case 572:
      /* CmdBindTileMemoryQCOM */
      if (!device || device->QCOM_tile_memory_heap) return true;
      return false;
   case 573:
      /* GetFramebufferTilePropertiesQCOM */
      if (!device || device->QCOM_tile_properties) return true;
      return false;
   case 574:
      /* GetDynamicRenderingTilePropertiesQCOM */
      if (!device || device->QCOM_tile_properties) return true;
      return false;
   case 575:
      /* CreateOpticalFlowSessionNV */
      if (!device || device->NV_optical_flow) return true;
      return false;
   case 576:
      /* DestroyOpticalFlowSessionNV */
      if (!device || device->NV_optical_flow) return true;
      return false;
   case 577:
      /* BindOpticalFlowSessionImageNV */
      if (!device || device->NV_optical_flow) return true;
      return false;
   case 578:
      /* CmdOpticalFlowExecuteNV */
      if (!device || device->NV_optical_flow) return true;
      return false;
   case 579:
      /* GetDeviceFaultInfoEXT */
      if (!device || device->EXT_device_fault) return true;
      return false;
   case 580:
      /* GetDeviceFaultReportsKHR */
      if (!device || device->KHR_device_fault) return true;
      return false;
   case 581:
      /* GetDeviceFaultDebugInfoKHR */
      if (!device || device->KHR_device_fault) return true;
      return false;
   case 582:
      /* CmdSetDepthBias2EXT */
      if (!device || device->EXT_depth_bias_control) return true;
      return false;
   case 583:
      /* ReleaseSwapchainImagesKHR */
      if (!device || device->KHR_swapchain_maintenance1) return true;
      return false;
   case 584:
      /* ReleaseSwapchainImagesEXT */
      if (!device || device->EXT_swapchain_maintenance1) return true;
      return false;
   case 585:
      /* GetDeviceImageSubresourceLayout */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 586:
      /* GetDeviceImageSubresourceLayoutKHR */
      if (!device || device->KHR_maintenance5) return true;
      return false;
   case 587:
      /* MapMemory2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 588:
      /* MapMemory2KHR */
      if (!device || device->KHR_map_memory2) return true;
      return false;
   case 589:
      /* UnmapMemory2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 590:
      /* UnmapMemory2KHR */
      if (!device || device->KHR_map_memory2) return true;
      return false;
   case 591:
      /* CreateShadersEXT */
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 592:
      /* DestroyShaderEXT */
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 593:
      /* GetShaderBinaryDataEXT */
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 594:
      /* CmdBindShadersEXT */
      if (!device || device->EXT_shader_object) return true;
      return false;
   case 595:
      /* SetSwapchainPresentTimingQueueSizeEXT */
      if (!device || device->EXT_present_timing) return true;
      return false;
   case 596:
      /* GetSwapchainTimingPropertiesEXT */
      if (!device || device->EXT_present_timing) return true;
      return false;
   case 597:
      /* GetSwapchainTimeDomainPropertiesEXT */
      if (!device || device->EXT_present_timing) return true;
      return false;
   case 598:
      /* GetPastPresentationTimingEXT */
      if (!device || device->EXT_present_timing) return true;
      return false;
   case 599:
      /* GetScreenBufferPropertiesQNX */
      if (!device || device->QNX_external_memory_screen_buffer) return true;
      return false;
   case 600:
      /* CreateGpaSessionAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 601:
      /* DestroyGpaSessionAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 602:
      /* SetGpaDeviceClockModeAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 603:
      /* GetGpaDeviceClockInfoAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 604:
      /* CmdBeginGpaSessionAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 605:
      /* CmdEndGpaSessionAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 606:
      /* CmdBeginGpaSampleAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 607:
      /* CmdEndGpaSampleAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 608:
      /* GetGpaSessionStatusAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 609:
      /* GetGpaSessionResultsAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 610:
      /* ResetGpaSessionAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 611:
      /* CmdCopyGpaSessionResultsAMD */
      if (!device || device->AMD_gpa_interface) return true;
      return false;
   case 612:
      /* CmdBindDescriptorSets2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 613:
      /* CmdBindDescriptorSets2KHR */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 614:
      /* CmdPushConstants2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 615:
      /* CmdPushConstants2KHR */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 616:
      /* CmdPushDescriptorSet2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 617:
      /* CmdPushDescriptorSet2KHR */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 618:
      /* CmdPushDescriptorSetWithTemplate2 */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 619:
      /* CmdPushDescriptorSetWithTemplate2KHR */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 620:
      /* CmdSetDescriptorBufferOffsets2EXT */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 621:
      /* CmdBindDescriptorBufferEmbeddedSamplers2EXT */
      if (!device || device->KHR_maintenance6) return true;
      return false;
   case 622:
      /* SetLatencySleepModeNV */
      if (!device || device->NV_low_latency2) return true;
      return false;
   case 623:
      /* LatencySleepNV */
      if (!device || device->NV_low_latency2) return true;
      return false;
   case 624:
      /* SetLatencyMarkerNV */
      if (!device || device->NV_low_latency2) return true;
      return false;
   case 625:
      /* GetLatencyTimingsNV */
      if (!device || device->NV_low_latency2) return true;
      return false;
   case 626:
      /* QueueNotifyOutOfBandNV */
      if (!device || device->NV_low_latency2) return true;
      return false;
   case 627:
      /* CmdSetRenderingAttachmentLocations */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 628:
      /* CmdSetRenderingAttachmentLocationsKHR */
      if (!device || device->KHR_dynamic_rendering_local_read) return true;
      return false;
   case 629:
      /* CmdSetRenderingInputAttachmentIndices */
      return VK_MAKE_VERSION(1, 4, 0) <= core_version;
   case 630:
      /* CmdSetRenderingInputAttachmentIndicesKHR */
      if (!device || device->KHR_dynamic_rendering_local_read) return true;
      return false;
   case 631:
      /* CmdSetDepthClampRangeEXT */
      if (!device || device->EXT_shader_object) return true;
      if (!device || device->EXT_depth_clamp_control) return true;
      return false;
   case 632:
      /* GetMemoryMetalHandleEXT */
      if (!device || device->EXT_external_memory_metal) return true;
      return false;
   case 633:
      /* GetMemoryMetalHandlePropertiesEXT */
      if (!device || device->EXT_external_memory_metal) return true;
      return false;
   case 634:
      /* ConvertCooperativeVectorMatrixNV */
      if (!device || device->NV_cooperative_vector) return true;
      return false;
   case 635:
      /* CmdConvertCooperativeVectorMatrixNV */
      if (!device || device->NV_cooperative_vector) return true;
      return false;
   case 636:
      /* CmdDispatchTileQCOM */
      if (!device || device->QCOM_tile_shading) return true;
      return false;
   case 637:
      /* CmdBeginPerTileExecutionQCOM */
      if (!device || device->QCOM_tile_shading) return true;
      return false;
   case 638:
      /* CmdEndPerTileExecutionQCOM */
      if (!device || device->QCOM_tile_shading) return true;
      return false;
   case 639:
      /* CreateExternalComputeQueueNV */
      if (!device || device->NV_external_compute_queue) return true;
      return false;
   case 640:
      /* DestroyExternalComputeQueueNV */
      if (!device || device->NV_external_compute_queue) return true;
      return false;
   case 641:
      /* CreateShaderInstrumentationARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 642:
      /* DestroyShaderInstrumentationARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 643:
      /* CmdBeginShaderInstrumentationARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 644:
      /* CmdEndShaderInstrumentationARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 645:
      /* GetShaderInstrumentationValuesARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 646:
      /* ClearShaderInstrumentationMetricsARM */
      if (!device || device->ARM_shader_instrumentation) return true;
      return false;
   case 647:
      /* CreateTensorARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 648:
      /* DestroyTensorARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 649:
      /* CreateTensorViewARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 650:
      /* DestroyTensorViewARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 651:
      /* GetTensorMemoryRequirementsARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 652:
      /* BindTensorMemoryARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 653:
      /* GetDeviceTensorMemoryRequirementsARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 654:
      /* CmdCopyTensorARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 655:
      /* GetTensorOpaqueCaptureDescriptorDataARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 656:
      /* GetTensorViewOpaqueCaptureDescriptorDataARM */
      if (!device || device->ARM_tensors) return true;
      return false;
   case 657:
      /* CreateDataGraphPipelinesARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 658:
      /* CreateDataGraphPipelineSessionARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 659:
      /* GetDataGraphPipelineSessionBindPointRequirementsARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 660:
      /* GetDataGraphPipelineSessionMemoryRequirementsARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 661:
      /* BindDataGraphPipelineSessionMemoryARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 662:
      /* DestroyDataGraphPipelineSessionARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 663:
      /* CmdDispatchDataGraphARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 664:
      /* GetDataGraphPipelineAvailablePropertiesARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 665:
      /* GetDataGraphPipelinePropertiesARM */
      if (!device || device->ARM_data_graph) return true;
      return false;
   case 666:
      /* GetNativeBufferPropertiesOHOS */
      if (!device || device->OHOS_external_memory) return true;
      return false;
   case 667:
      /* GetMemoryNativeBufferOHOS */
      if (!device || device->OHOS_external_memory) return true;
      return false;
   case 668:
      /* QueueSetPerfHintQCOM */
      if (!device || device->QCOM_queue_perf_hint) return true;
      return false;
   case 669:
      /* CmdSetComputeOccupancyPriorityNV */
      if (!device || device->NV_compute_occupancy_priority) return true;
      return false;
   case 670:
      /* WriteSamplerDescriptorsEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 671:
      /* WriteResourceDescriptorsEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 672:
      /* CmdBindSamplerHeapEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 673:
      /* CmdBindResourceHeapEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 674:
      /* CmdPushDataEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 675:
      /* RegisterCustomBorderColorEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 676:
      /* UnregisterCustomBorderColorEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 677:
      /* GetImageOpaqueCaptureDataEXT */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 678:
      /* GetTensorOpaqueCaptureDataARM */
      if (!device || device->EXT_descriptor_heap) return true;
      return false;
   case 679:
      /* CmdCopyMemoryKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 680:
      /* CmdCopyMemoryToImageKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 681:
      /* CmdCopyImageToMemoryKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 682:
      /* CmdUpdateMemoryKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 683:
      /* CmdFillMemoryKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 684:
      /* CmdCopyQueryPoolResultsToMemoryKHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 685:
      /* CmdBeginConditionalRendering2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 686:
      /* CmdBindTransformFeedbackBuffers2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 687:
      /* CmdBeginTransformFeedback2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 688:
      /* CmdEndTransformFeedback2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 689:
      /* CmdDrawIndirectByteCount2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 690:
      /* CmdWriteMarkerToMemoryAMD */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 691:
      /* CmdBindIndexBuffer3KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 692:
      /* CmdBindVertexBuffers3KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 693:
      /* CmdDrawIndirect2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 694:
      /* CmdDrawIndexedIndirect2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 695:
      /* CmdDrawIndirectCount2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 696:
      /* CmdDrawIndexedIndirectCount2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 697:
      /* CmdDrawMeshTasksIndirect2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 698:
      /* CmdDrawMeshTasksIndirectCount2EXT */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 699:
      /* CmdDispatchIndirect2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 700:
      /* CreateAccelerationStructure2KHR */
      if (!device || device->KHR_device_address_commands) return true;
      return false;
   case 701:
      /* CmdSetDispatchParametersARM */
      if (!device || device->ARM_scheduling_controls) return true;
      return false;
   default:
      return false;
   }
}

#ifdef _MSC_VER
VKAPI_ATTR void VKAPI_CALL vk_entrypoint_stub(void)
{
   UNREACHABLE("Entrypoint not implemented");
}

static const void *get_function_target(const void *func)
{
   const uint8_t *address = func;
#ifdef _M_X64
   /* Incremental linking may indirect through relative jump */
   if (*address == 0xE9)
   {
      /* Compute JMP target if the first byte is opcode 0xE9 */
      uint32_t offset;
      memcpy(&offset, address + 1, 4);
      address += offset + 5;
   }
#else
   /* Add other platforms here if necessary */
#endif
   return address;
}

static bool vk_function_is_stub(PFN_vkVoidFunction func)
{
   return (func == vk_entrypoint_stub) || (get_function_target(func) == get_function_target(vk_entrypoint_stub));
}
#endif




void vk_instance_dispatch_table_from_entrypoints(
    struct vk_instance_dispatch_table *dispatch_table,
    const struct vk_instance_entrypoint_table *entrypoint_table,
    bool overwrite)
{
    PFN_vkVoidFunction *disp = (PFN_vkVoidFunction *)dispatch_table;
    PFN_vkVoidFunction *entry = (PFN_vkVoidFunction *)entrypoint_table;

    if (overwrite) {
        memset(dispatch_table, 0, sizeof(*dispatch_table));
        for (unsigned i = 0; i < ARRAY_SIZE(instance_compaction_table); i++) {
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (vk_function_is_stub(entry[i]))
#else
            if (entry[i] == NULL)
#endif
                continue;
            unsigned disp_index = instance_compaction_table[i];
            assert(disp[disp_index] == NULL);
            disp[disp_index] = entry[i];
        }
    } else {
        for (unsigned i = 0; i < ARRAY_SIZE(instance_compaction_table); i++) {
            unsigned disp_index = instance_compaction_table[i];
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (disp[disp_index] == NULL && !vk_function_is_stub(entry[i]))
#else
            if (disp[disp_index] == NULL)
#endif
                disp[disp_index] = entry[i];
        }
    }
}


void vk_physical_device_dispatch_table_from_entrypoints(
    struct vk_physical_device_dispatch_table *dispatch_table,
    const struct vk_physical_device_entrypoint_table *entrypoint_table,
    bool overwrite)
{
    PFN_vkVoidFunction *disp = (PFN_vkVoidFunction *)dispatch_table;
    PFN_vkVoidFunction *entry = (PFN_vkVoidFunction *)entrypoint_table;

    if (overwrite) {
        memset(dispatch_table, 0, sizeof(*dispatch_table));
        for (unsigned i = 0; i < ARRAY_SIZE(physical_device_compaction_table); i++) {
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (vk_function_is_stub(entry[i]))
#else
            if (entry[i] == NULL)
#endif
                continue;
            unsigned disp_index = physical_device_compaction_table[i];
            assert(disp[disp_index] == NULL);
            disp[disp_index] = entry[i];
        }
    } else {
        for (unsigned i = 0; i < ARRAY_SIZE(physical_device_compaction_table); i++) {
            unsigned disp_index = physical_device_compaction_table[i];
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (disp[disp_index] == NULL && !vk_function_is_stub(entry[i]))
#else
            if (disp[disp_index] == NULL)
#endif
                disp[disp_index] = entry[i];
        }
    }
}


void vk_device_dispatch_table_from_entrypoints(
    struct vk_device_dispatch_table *dispatch_table,
    const struct vk_device_entrypoint_table *entrypoint_table,
    bool overwrite)
{
    PFN_vkVoidFunction *disp = (PFN_vkVoidFunction *)dispatch_table;
    PFN_vkVoidFunction *entry = (PFN_vkVoidFunction *)entrypoint_table;

    if (overwrite) {
        memset(dispatch_table, 0, sizeof(*dispatch_table));
        for (unsigned i = 0; i < ARRAY_SIZE(device_compaction_table); i++) {
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (vk_function_is_stub(entry[i]))
#else
            if (entry[i] == NULL)
#endif
                continue;
            unsigned disp_index = device_compaction_table[i];
            assert(disp[disp_index] == NULL);
            disp[disp_index] = entry[i];
        }
    } else {
        for (unsigned i = 0; i < ARRAY_SIZE(device_compaction_table); i++) {
            unsigned disp_index = device_compaction_table[i];
#ifdef _MSC_VER
            assert(entry[i] != NULL);
            if (disp[disp_index] == NULL && !vk_function_is_stub(entry[i]))
#else
            if (disp[disp_index] == NULL)
#endif
                disp[disp_index] = entry[i];
        }
    }
}





static PFN_vkVoidFunction
vk_instance_dispatch_table_get_for_entry_index(
    const struct vk_instance_dispatch_table *table, int entry_index)
{
    assert(entry_index < ARRAY_SIZE(instance_compaction_table));
    int disp_index = instance_compaction_table[entry_index];
    return ((PFN_vkVoidFunction *)table)[disp_index];
}

PFN_vkVoidFunction
vk_instance_dispatch_table_get(
    const struct vk_instance_dispatch_table *table, const char *name)
{
    int entry_index = instance_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    return vk_instance_dispatch_table_get_for_entry_index(table, entry_index);
}


static PFN_vkVoidFunction
vk_physical_device_dispatch_table_get_for_entry_index(
    const struct vk_physical_device_dispatch_table *table, int entry_index)
{
    assert(entry_index < ARRAY_SIZE(physical_device_compaction_table));
    int disp_index = physical_device_compaction_table[entry_index];
    return ((PFN_vkVoidFunction *)table)[disp_index];
}

PFN_vkVoidFunction
vk_physical_device_dispatch_table_get(
    const struct vk_physical_device_dispatch_table *table, const char *name)
{
    int entry_index = physical_device_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    return vk_physical_device_dispatch_table_get_for_entry_index(table, entry_index);
}


static PFN_vkVoidFunction
vk_device_dispatch_table_get_for_entry_index(
    const struct vk_device_dispatch_table *table, int entry_index)
{
    assert(entry_index < ARRAY_SIZE(device_compaction_table));
    int disp_index = device_compaction_table[entry_index];
    return ((PFN_vkVoidFunction *)table)[disp_index];
}

PFN_vkVoidFunction
vk_device_dispatch_table_get(
    const struct vk_device_dispatch_table *table, const char *name)
{
    int entry_index = device_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    return vk_device_dispatch_table_get_for_entry_index(table, entry_index);
}


PFN_vkVoidFunction
vk_instance_dispatch_table_get_if_supported(
    const struct vk_instance_dispatch_table *table,
    const char *name,
    uint32_t core_version,
    const struct vk_instance_extension_table *instance_exts)
{
    int entry_index = instance_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    if (!vk_instance_entrypoint_is_enabled(entry_index, core_version,
                                           instance_exts))
        return NULL;

    return vk_instance_dispatch_table_get_for_entry_index(table, entry_index);
}

PFN_vkVoidFunction
vk_physical_device_dispatch_table_get_if_supported(
    const struct vk_physical_device_dispatch_table *table,
    const char *name,
    uint32_t core_version,
    const struct vk_instance_extension_table *instance_exts)
{
    int entry_index = physical_device_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    if (!vk_physical_device_entrypoint_is_enabled(entry_index, core_version,
                                                  instance_exts))
        return NULL;

    return vk_physical_device_dispatch_table_get_for_entry_index(table, entry_index);
}

PFN_vkVoidFunction
vk_device_dispatch_table_get_if_supported(
    const struct vk_device_dispatch_table *table,
    const char *name,
    uint32_t core_version,
    const struct vk_instance_extension_table *instance_exts,
    const struct vk_device_extension_table *device_exts)
{
    int entry_index = device_string_map_lookup(name);
    if (entry_index < 0)
        return NULL;

    if (!vk_device_entrypoint_is_enabled(entry_index, core_version,
                                         instance_exts, device_exts))
        return NULL;

    return vk_device_dispatch_table_get_for_entry_index(table, entry_index);
}
