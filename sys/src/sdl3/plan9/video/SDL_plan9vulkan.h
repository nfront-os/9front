/*
  SPDX-License-Identifier: Zlib

  SDL's plan9 video driver: Vulkan over VK_PLAN9_surface (Mesa's plan9 WSI)
*/
#ifndef SDL_plan9vulkan_h_
#define SDL_plan9vulkan_h_

#include "SDL_internal.h"

#if defined(SDL_VIDEO_DRIVER_PLAN9) && defined(SDL_VIDEO_VULKAN)

#include "video/SDL_sysvideo.h"
#include "video/SDL_vulkan_internal.h"

extern bool PLAN9_Vulkan_LoadLibrary(SDL_VideoDevice *_this, const char *path);
extern void PLAN9_Vulkan_UnloadLibrary(SDL_VideoDevice *_this);
extern char const * const *PLAN9_Vulkan_GetInstanceExtensions(SDL_VideoDevice *_this, Uint32 *count);
extern bool PLAN9_Vulkan_CreateSurface(SDL_VideoDevice *_this, SDL_Window *window, VkInstance instance,
                                       const struct VkAllocationCallbacks *allocator, VkSurfaceKHR *surface);
extern void PLAN9_Vulkan_DestroySurface(SDL_VideoDevice *_this, VkInstance instance, VkSurfaceKHR surface,
                                        const struct VkAllocationCallbacks *allocator);

#endif

#endif
