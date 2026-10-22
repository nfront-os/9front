#ifndef VULKAN_PLAN9_H_
#define VULKAN_PLAN9_H_ 1

/*
** Copyright 2026 The 9front gpu port
**
** SPDX-License-Identifier: Apache-2.0 OR MIT
*/

/*
** VK_PLAN9_surface: present to a draw(3) window on 9front.
** Written in the form of the Khronos platform headers (vulkan_wayland.h);
** the registry entry is in src/vulkan/registry/vk.xml.
*/


#ifdef __cplusplus
extern "C" {
#endif



// VK_PLAN9_surface is a preprocessor guard. Do not pass it to API calls.
#define VK_PLAN9_surface 1
#define VK_PLAN9_SURFACE_SPEC_VERSION     1
#define VK_PLAN9_SURFACE_EXTENSION_NAME   "VK_PLAN9_surface"
typedef VkFlags VkPlan9SurfaceCreateFlagsPLAN9;
typedef struct VkPlan9SurfaceCreateInfoPLAN9 {
    VkStructureType                   sType;
    const void*                       pNext;
    VkPlan9SurfaceCreateFlagsPLAN9    flags;
    const char*                       pWindowName;	/* NULL: /dev/winname, else the screen */
} VkPlan9SurfaceCreateInfoPLAN9;

typedef VkResult (VKAPI_PTR *PFN_vkCreatePlan9SurfacePLAN9)(VkInstance instance, const VkPlan9SurfaceCreateInfoPLAN9* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);

#ifndef VK_NO_PROTOTYPES
#ifndef VK_ONLY_EXPORTED_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkCreatePlan9SurfacePLAN9(
    VkInstance                                  instance,
    const VkPlan9SurfaceCreateInfoPLAN9*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
