/*
  SPDX-License-Identifier: Zlib

  SDL's plan9 video driver: Vulkan.  9front has no dlopen: the loader is
  the static libvulkan.a (Mesa's, /$objtype/lib.gcc/ape), named here by a
  weak reference, so that a program without Vulkan does not carry it; a
  program that wants SDL's Vulkan links -lvulkan (and, if it calls no
  Vulkan function itself, -Wl,-u,vkGetInstanceProcAddr).  The surface is
  VK_PLAN9_surface: the process's window (/dev/winname) or the screen.
*/
#include "SDL_internal.h"

#if defined(SDL_VIDEO_DRIVER_PLAN9) && defined(SDL_VIDEO_VULKAN)

#include "SDL_plan9vulkan.h"

/*
 * VK_PLAN9_surface (9front's vulkan/vulkan_plan9.h): SDL's own Khronos
 * headers do not have it, the two vulkan_core.h do not go together
 */
#define VK_PLAN9_SURFACE_EXTENSION_NAME "VK_PLAN9_surface"
#define VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9 ((VkStructureType)1008999000)
typedef struct VkPlan9SurfaceCreateInfoPLAN9 {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    const char *pWindowName;    /* NULL: /dev/winname, else the screen */
} VkPlan9SurfaceCreateInfoPLAN9;
typedef VkResult (VKAPI_PTR *PFN_vkCreatePlan9SurfacePLAN9)(VkInstance instance, const VkPlan9SurfaceCreateInfoPLAN9 *pCreateInfo,
                                                            const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface);

extern VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *name)
    __attribute__((weak));

bool PLAN9_Vulkan_LoadLibrary(SDL_VideoDevice *_this, const char *path)
{
    (void)path;
    if (_this->vulkan_config.vkGetInstanceProcAddr) {
        return SDL_SetError("Vulkan already loaded");
    }
    if (!vkGetInstanceProcAddr) {
        return SDL_SetError("plan9: no Vulkan in the program (link -lvulkan)");
    }
    _this->vulkan_config.vkGetInstanceProcAddr = (SDL_FunctionPointer)vkGetInstanceProcAddr;
    _this->vulkan_config.vkEnumerateInstanceExtensionProperties =
        (SDL_FunctionPointer)vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");
    if (!_this->vulkan_config.vkEnumerateInstanceExtensionProperties) {
        _this->vulkan_config.vkGetInstanceProcAddr = NULL;
        return SDL_SetError("plan9: no vkEnumerateInstanceExtensionProperties");
    }
    SDL_strlcpy(_this->vulkan_config.loader_path, "libvulkan.a", SDL_arraysize(_this->vulkan_config.loader_path));
    return true;
}

void PLAN9_Vulkan_UnloadLibrary(SDL_VideoDevice *_this)
{
    _this->vulkan_config.vkGetInstanceProcAddr = NULL;
    _this->vulkan_config.vkEnumerateInstanceExtensionProperties = NULL;
}

char const * const *PLAN9_Vulkan_GetInstanceExtensions(SDL_VideoDevice *_this, Uint32 *count)
{
    static const char *const extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_PLAN9_SURFACE_EXTENSION_NAME,
    };

    (void)_this;
    if (count) {
        *count = SDL_arraysize(extensions);
    }
    return extensions;
}

bool PLAN9_Vulkan_CreateSurface(SDL_VideoDevice *_this, SDL_Window *window, VkInstance instance,
                                const struct VkAllocationCallbacks *allocator, VkSurfaceKHR *surface)
{
    PFN_vkGetInstanceProcAddr gipa = (PFN_vkGetInstanceProcAddr)_this->vulkan_config.vkGetInstanceProcAddr;
    PFN_vkCreatePlan9SurfacePLAN9 create;
    VkPlan9SurfaceCreateInfoPLAN9 info;
    VkResult r;

    (void)window;
    if (!gipa) {
        return SDL_SetError("Vulkan is not loaded");
    }
    create = (PFN_vkCreatePlan9SurfacePLAN9)gipa(instance, "vkCreatePlan9SurfacePLAN9");
    if (!create) {
        return SDL_SetError(VK_PLAN9_SURFACE_EXTENSION_NAME " extension is not enabled in the Vulkan instance");
    }
    SDL_zero(info);
    info.sType = VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9;
    info.pWindowName = NULL;    /* the process's window (/dev/winname), else the screen */
    r = create(instance, &info, allocator, surface);
    if (r != VK_SUCCESS) {
        return SDL_SetError("vkCreatePlan9SurfacePLAN9 failed: %s", SDL_Vulkan_GetResultString(r));
    }
    return true;
}

void PLAN9_Vulkan_DestroySurface(SDL_VideoDevice *_this, VkInstance instance, VkSurfaceKHR surface,
                                 const struct VkAllocationCallbacks *allocator)
{
    if (_this->vulkan_config.vkGetInstanceProcAddr) {
        SDL_Vulkan_DestroySurface_Internal(_this->vulkan_config.vkGetInstanceProcAddr, instance, surface, allocator);
    }
}

#endif
