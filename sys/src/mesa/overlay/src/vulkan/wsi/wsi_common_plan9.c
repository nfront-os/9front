/*
 * Copyright 2026 The 9front gpu port
 * Copyright 2021 Red Hat, Inc. (wsi_common_headless.c, the model)
 * SPDX-License-Identifier: MIT
 */

/** VK_PLAN9_surface: present to a draw(3) window on 9front
 *
 * The images are the device's; at present the common code blits them
 * into a host visible buffer (WSI_IMAGE_TYPE_CPU), and queue_present
 * waits for that blit and copies the pixels to the window through its
 * own /dev/draw connection (plan9win.h).  The window is found by name
 * every time: rio gives a window a new image when it is resized.
 *
 * In a window of wsys(4) the copy is wsys's: the host visible buffers
 * are exported (a token each, gpu(4)), wsys imports them (ctl
 * gpubuffer) and reads a frame when it is presented (ctl present); the
 * write of present returns once wsys has it, and the buffer is free.
 * Without the screen to itself or wsys, or if either refuses, the copy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util/u_cpu_detect.h"
#include "util/streaming-load-memcpy.h"

#include "util/macros.h"
#include "util/timespec.h"
#include "util/os_time.h"
#include "c11/threads.h"
#include "util/u_atomic.h"
#include <pthread.h>
#include "vk_util.h"
#include "vk_instance.h"
#include "vk_physical_device.h"
#include "wsi_common_entrypoints.h"
#include "wsi_common_private.h"

#include "plan9win.h"

extern int plan9_dmabuf_token(int fd, char *buf, int n);	/* plan9/lib/dmabuf.c */

enum { Wsysbufs = 8 };	/* wsys' buffers a window */
/*
 * a swapchain's buffers in wsys: numbers 0-3 or 4-7, the other half from the
 * swapchain before (an app makes the new one before it destroys the old,
 * whose unbuffers must not hit the new one's)
 */
static int wsys_half;

struct wsi_plan9 {
   struct wsi_interface base;
};

static uint32_t wsi_plan9_select_scanout_memory_type(const struct wsi_device *wsi, uint32_t type_bits);
static uint32_t wsi_plan9_select_overlay_memory_type(const struct wsi_device *wsi, uint32_t type_bits);
static VkResult wsi_plan9_create_overlay_mem(const struct wsi_swapchain *chain, const struct wsi_image_info *info,
                                            struct wsi_image *image);
static VkResult wsi_plan9_create_scanout_mem(const struct wsi_swapchain *chain, const struct wsi_image_info *info,
                                             struct wsi_image *image);

static VkResult
wsi_plan9_surface_get_support(VkIcdSurfaceBase *surface,
                              struct wsi_device *wsi_device,
                              uint32_t queueFamilyIndex,
                              VkBool32 *pSupported)
{
   VkIcdSurfacePlan9 *s = (VkIcdSurfacePlan9 *)surface;
   int w, h;

   *pSupported = plan9win_size(s->windowName, &w, &h) == 0;
   return VK_SUCCESS;
}

static const VkPresentModeKHR present_modes[] = {
   VK_PRESENT_MODE_FIFO_KHR,
   VK_PRESENT_MODE_IMMEDIATE_KHR,
};

/*
 * the whole screen without rio: the frames go to the display (gpu(4)
 * scanout) unless WSI_PLAN9_COPY=1 (tests read them back through draw(3))
 */
static bool
plan9_scanout(VkIcdSurfacePlan9 *s, struct wsi_device *wsi_device)
{
   const char *copy = getenv("WSI_PLAN9_COPY");

   return wsi_device->plan9_scanout != NULL && !(copy && atoi(copy)) &&
          plan9win_isscreen(s->windowName);
}

/*
 * The images the WSI holds itself while the app acquires: the one being
 * presented (venus presents on a thread of its own, the image is the
 * WSI's until the present is done) and, with scanout, the one on the
 * display.  minImageCount is one more, so that an app holding all the
 * spec lets it (imageCount - minImageCount) still finds one free: venus
 * holds the swapchain's lock while acquire waits, the present that would
 * free one cannot run meanwhile (Zink's kopper hung so, glwin).
 */
static uint32_t
plan9_min_images(VkIcdSurfacePlan9 *s, struct wsi_device *wsi_device)
{
   return plan9_scanout(s, wsi_device) ? 3 : 2;
}

static VkResult
wsi_plan9_surface_get_capabilities(VkIcdSurfaceBase *surface,
                                   struct wsi_device *wsi_device,
                                   VkSurfaceCapabilities2KHR *caps)
{
   VkIcdSurfacePlan9 *s = (VkIcdSurfacePlan9 *)surface;
   int w, h;

   if (plan9win_size(s->windowName, &w, &h) < 0)
      return VK_ERROR_SURFACE_LOST_KHR;

   caps->surfaceCapabilities.minImageCount = plan9_min_images(s, wsi_device);
   caps->surfaceCapabilities.maxImageCount = 0;

   /* the window's size: the swapchain must have it */
   caps->surfaceCapabilities.currentExtent = (VkExtent2D) { w, h };
   caps->surfaceCapabilities.minImageExtent = (VkExtent2D) { 1, 1 };
   caps->surfaceCapabilities.maxImageExtent = (VkExtent2D) {
      wsi_device->maxImageDimension2D,
      wsi_device->maxImageDimension2D,
   };

   caps->surfaceCapabilities.supportedTransforms = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
   caps->surfaceCapabilities.currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
   caps->surfaceCapabilities.maxImageArrayLayers = 1;
   caps->surfaceCapabilities.supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

   VkImageUsageFlags image_usage = wsi_caps_get_image_usage();
   VkImageUsageFlags2CreateInfoKHR *usage2 =
      vk_find_struct(caps->pNext, IMAGE_USAGE_FLAGS_2_CREATE_INFO_KHR);
   if (usage2)
      usage2->usage = image_usage;
   else
      caps->surfaceCapabilities.supportedUsageFlags = image_usage;

   return VK_SUCCESS;
}

static VkResult
wsi_plan9_surface_get_capabilities2(VkIcdSurfaceBase *surface,
                                    struct wsi_device *wsi_device,
                                    const void *info_next,
                                    VkSurfaceCapabilities2KHR *caps)
{
   assert(caps->sType == VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR);

   const VkSurfacePresentModeKHR *present_mode =
      vk_find_struct_const(info_next, SURFACE_PRESENT_MODE_EXT);

   VkResult result = wsi_plan9_surface_get_capabilities(surface, wsi_device, caps);

   vk_foreach_struct(ext, caps->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR: {
         VkSurfaceProtectedCapabilitiesKHR *protected = (void *)ext;
         protected->supportsProtected = VK_FALSE;
         break;
      }
      case VK_STRUCTURE_TYPE_SURFACE_PRESENT_SCALING_CAPABILITIES_KHR: {
         VkSurfacePresentScalingCapabilitiesKHR *scaling = (void *)ext;
         scaling->supportedPresentScaling = 0;
         scaling->supportedPresentGravityX = 0;
         scaling->supportedPresentGravityY = 0;
         scaling->minScaledImageExtent = caps->surfaceCapabilities.minImageExtent;
         scaling->maxScaledImageExtent = caps->surfaceCapabilities.maxImageExtent;
         break;
      }
      case VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_KHR: {
         VkSurfacePresentModeCompatibilityKHR *compat = (void *)ext;
         if (compat->pPresentModes == NULL) {
            compat->presentModeCount = 1;
         } else if (compat->presentModeCount) {
            compat->presentModeCount = 1;
            compat->pPresentModes[0] =
               present_mode ? present_mode->presentMode : VK_PRESENT_MODE_FIFO_KHR;
         }
         break;
      }
      case VK_STRUCTURE_TYPE_PRESENT_TIMING_SURFACE_CAPABILITIES_EXT: {
         VkPresentTimingSurfaceCapabilitiesEXT *wait = (void *)ext;
         wait->presentStageQueries = 0;
         wait->presentTimingSupported = VK_FALSE;
         wait->presentAtAbsoluteTimeSupported = VK_FALSE;
         wait->presentAtRelativeTimeSupported = VK_FALSE;
         break;
      }
      default:
         break;
      }
   }

   return result;
}

/* draw(3)'s XRGB32 is B, G, R, X in memory */
static const VkFormat formats[] = {
   VK_FORMAT_B8G8R8A8_SRGB,
   VK_FORMAT_B8G8R8A8_UNORM,
};

static VkResult
wsi_plan9_surface_get_formats(VkIcdSurfaceBase *icd_surface,
                              struct wsi_device *wsi_device,
                              uint32_t *pSurfaceFormatCount,
                              VkSurfaceFormatKHR *pSurfaceFormats)
{
   VK_OUTARRAY_MAKE_TYPED(VkSurfaceFormatKHR, out, pSurfaceFormats, pSurfaceFormatCount);

   for (unsigned i = 0; i < ARRAY_SIZE(formats); i++) {
      vk_outarray_append_typed(VkSurfaceFormatKHR, &out, out_fmt) {
         out_fmt->format = formats[i];
         out_fmt->colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      }
   }

   return vk_outarray_status(&out);
}

static VkResult
wsi_plan9_surface_get_formats2(VkIcdSurfaceBase *icd_surface,
                               struct wsi_device *wsi_device,
                               const void *info_next,
                               uint32_t *pSurfaceFormatCount,
                               VkSurfaceFormat2KHR *pSurfaceFormats)
{
   VK_OUTARRAY_MAKE_TYPED(VkSurfaceFormat2KHR, out, pSurfaceFormats, pSurfaceFormatCount);

   for (unsigned i = 0; i < ARRAY_SIZE(formats); i++) {
      vk_outarray_append_typed(VkSurfaceFormat2KHR, &out, out_fmt) {
         out_fmt->surfaceFormat.format = formats[i];
         out_fmt->surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      }
   }

   return vk_outarray_status(&out);
}

static VkResult
wsi_plan9_surface_get_present_modes(VkIcdSurfaceBase *surface,
                                    struct wsi_device *wsi_device,
                                    uint32_t *pPresentModeCount,
                                    VkPresentModeKHR *pPresentModes)
{
   if (pPresentModes == NULL) {
      *pPresentModeCount = ARRAY_SIZE(present_modes);
      return VK_SUCCESS;
   }

   *pPresentModeCount = MIN2(*pPresentModeCount, ARRAY_SIZE(present_modes));
   typed_memcpy(pPresentModes, present_modes, *pPresentModeCount);

   return *pPresentModeCount < ARRAY_SIZE(present_modes) ? VK_INCOMPLETE : VK_SUCCESS;
}

static VkResult
wsi_plan9_surface_get_present_rectangles(VkIcdSurfaceBase *surface,
                                         struct wsi_device *wsi_device,
                                         uint32_t *pRectCount,
                                         VkRect2D *pRects)
{
   VkIcdSurfacePlan9 *s = (VkIcdSurfacePlan9 *)surface;
   VK_OUTARRAY_MAKE_TYPED(VkRect2D, out, pRects, pRectCount);
   int w, h;

   if (plan9win_size(s->windowName, &w, &h) < 0)
      return VK_ERROR_SURFACE_LOST_KHR;

   vk_outarray_append_typed(VkRect2D, &out, rect) {
      *rect = (VkRect2D) {
         .offset = { 0, 0 },
         .extent = { w, h },
      };
   }

   return vk_outarray_status(&out);
}

struct wsi_plan9_image {
   struct wsi_image base;
   bool busy;  /* acquired by the app, being presented or on the display (lock) */
};

struct wsi_plan9_swapchain {
   struct wsi_swapchain base;
   const char *window_name;
   void *pixels;  /* one frame, read once from the host visible buffer */
   VkExtent2D extent;
   VkResult status;  /* VK_SUBOPTIMAL_KHR once the window changed size */
   bool scanout;     /* the frames go to the display directly */
   int wsys;         /* the frames go to wsys(4) as GPU buffers: its ctl; -1 not */
   int shown;        /* the image on the display, kept busy; -1 none */
   bool overlay;     /* in wsys, the images in VRAM for its overlay plane: no copy, no view */
   uint32_t bufbase; /* its buffers' numbers in wsys: bufbase + image */
   uint32_t ignored; /* presents since VK_SUBOPTIMAL_KHR off the overlay plane */
   uint32_t next;
   mtx_t lock;       /* busy, next */
   cnd_t freed;      /* an image is no longer busy */
   /* WSI_PLAN9_STATS=1: where a present's time goes, every 100 frames */
   bool stats;
   uint32_t nstat;
   int64_t tstat[4];  /* the GPU's blit, the copy, draw(3) or scanout, between presents */
   int64_t last;
   struct wsi_plan9_image images[0];
};

static void
plan9_stats(struct wsi_plan9_swapchain *chain, const int64_t t[4])
{
   int64_t now = os_time_get_nano();
   int i;

   for (i = 0; i < 3; i++)
      chain->tstat[i] += t[i+1] - t[i];
   if (chain->last != 0)
      chain->tstat[3] += now - chain->last;
   chain->last = now;
   if (++chain->nstat < 100)
      return;
   fprintf(stderr, "wsi plan9 %ux%u %s: per frame %.2f ms: blit wait %.2f, copy %.2f, %s %.2f\n",
           chain->extent.width, chain->extent.height,
           chain->scanout ? "scanout" : chain->wsys >= 0 ? "wsys" : "copy",
           chain->tstat[3] / 1e6 / (chain->nstat - 1),
           chain->tstat[0] / 1e6 / chain->nstat, chain->tstat[1] / 1e6 / chain->nstat,
           chain->scanout ? "scanout" : chain->wsys >= 0 ? "wsys" : "draw", chain->tstat[2] / 1e6 / chain->nstat);
   chain->nstat = 0;
   chain->last = 0;
   memset(chain->tstat, 0, sizeof chain->tstat);
}
VK_DEFINE_NONDISP_HANDLE_CASTS(wsi_plan9_swapchain, base.base, VkSwapchainKHR,
                               VK_OBJECT_TYPE_SWAPCHAIN_KHR)

static struct wsi_image *
wsi_plan9_swapchain_get_wsi_image(struct wsi_swapchain *wsi_chain,
                                  uint32_t image_index)
{
   struct wsi_plan9_swapchain *chain = (struct wsi_plan9_swapchain *)wsi_chain;
   return &chain->images[image_index].base;
}

/* image i no longer busy: an acquire waiting for one goes on */
static void
plan9_free(struct wsi_plan9_swapchain *chain, uint32_t i)
{
   mtx_lock(&chain->lock);
   chain->images[i].busy = false;
   cnd_broadcast(&chain->freed);
   mtx_unlock(&chain->lock);
}

static VkResult
wsi_plan9_swapchain_release_images(struct wsi_swapchain *wsi_chain,
                                   uint32_t count, const uint32_t *indices)
{
   struct wsi_plan9_swapchain *chain = (struct wsi_plan9_swapchain *)wsi_chain;

   for (uint32_t i = 0; i < count; i++) {
      assert(indices[i] < chain->base.image_count);
      plan9_free(chain, indices[i]);
   }
   return VK_SUCCESS;
}

static VkResult
wsi_plan9_swapchain_acquire_next_image(struct wsi_swapchain *wsi_chain,
                                       const VkAcquireNextImageInfoKHR *info,
                                       uint32_t *image_index)
{
   struct wsi_plan9_swapchain *chain = (struct wsi_plan9_swapchain *)wsi_chain;
   struct timespec start_time, end_time, rel_timeout;

   if (chain->status == VK_ERROR_OUT_OF_DATE_KHR)
      return chain->status;

   timespec_from_nsec(&rel_timeout, info->timeout);
   clock_gettime(CLOCK_MONOTONIC, &start_time);
   timespec_add(&end_time, &rel_timeout, &start_time);

   mtx_lock(&chain->lock);
   for (;;) {
      /* round robin: the image presented longest ago comes first */
      for (uint32_t n = 0; n < chain->base.image_count; n++) {
         uint32_t i = (chain->next + n) % chain->base.image_count;
         if (!chain->images[i].busy) {
            chain->images[i].busy = true;
            chain->next = (i + 1) % chain->base.image_count;
            mtx_unlock(&chain->lock);
            *image_index = i;
            return chain->status;
         }
      }

      /* none free: until a present or a release frees one */
      if (info->timeout == UINT64_MAX) {
         cnd_wait(&chain->freed, &chain->lock);
         continue;
      }
      struct timespec current_time;
      clock_gettime(CLOCK_MONOTONIC, &current_time);
      if (timespec_after(&current_time, &end_time)) {
         mtx_unlock(&chain->lock);
         return info->timeout ? VK_TIMEOUT : VK_NOT_READY;
      }
      /* a finite wait: in steps (cnd_timedwait's clock is the wall clock) */
      mtx_unlock(&chain->lock);
      os_time_sleep(100);
      mtx_lock(&chain->lock);
   }
}

/*
 * wsys' word for this process's window (wsys(4) event "overlay 0|1"):
 * whether its frames may go on the display's overlay plane, -1 not heard
 * yet.  A thread reads the window's events; a swapchain made then has its
 * images in VRAM (1, variant b: the display shows them as they are) or
 * where wsys reads them (0); when the word changes, present says
 * VK_SUBOPTIMAL_KHR and the app makes a new swapchain.
 */
static int wsys_overlay = -1;
static pthread_once_t wsys_events_once = PTHREAD_ONCE_INIT;

static void *
wsys_events(void *arg)
{
   char buf[1024];
   int fd, n;

   (void)arg;
   if ((fd = plan9win_wsysevents()) < 0)
      return NULL;
   while ((n = read(fd, buf, sizeof buf - 1)) > 0) {
      buf[n] = 0;
      for (char *l = buf; l != NULL && *l; l = strchr(l, '\n') ? strchr(l, '\n') + 1 : NULL) {
         if (strncmp(l, "overlay ", 8) == 0)
            p_atomic_set(&wsys_overlay, atoi(l + 8) != 0);
      }
   }
   close(fd);
   return NULL;
}

static void
wsys_events_start(void)
{
   pthread_t t;

   if (pthread_create(&t, NULL, wsys_events, NULL) == 0)
      pthread_detach(t);
}

/* wsys lets go of the buffers; frames by copy from now on */
static void
wsys_off(struct wsi_plan9_swapchain *chain)
{
   char msg[32];

   if (chain->wsys < 0)
      return;
   for (uint32_t i = 0; i < chain->base.image_count; i++) {
      snprintf(msg, sizeof msg, "unbuffer %u", chain->bufbase + i);
      plan9win_wsysctl(chain->wsys, msg);
   }
   close(chain->wsys);
   chain->wsys = -1;
}

/* each image's host visible buffer to wsys by its token; false: none */
static bool
wsys_buffers(struct wsi_plan9_swapchain *chain)
{
   const struct wsi_device *wsi = chain->base.wsi;
   char token[64], msg[160];

   for (uint32_t i = 0; i < chain->base.image_count; i++) {
      struct wsi_image *image = &chain->images[i].base;
      VkMemoryGetFdInfoKHR info = {
         .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
         .memory = image->blit.memory != VK_NULL_HANDLE ? image->blit.memory : image->memory,
         .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
      };
      int fd, r;

      if (wsi->GetMemoryFdKHR == NULL ||
          wsi->GetMemoryFdKHR(chain->base.device, &info, &fd) != VK_SUCCESS)
         return false;
      r = plan9_dmabuf_token(fd, token, sizeof token);
      close(fd);	/* the token stays while the memory does */
      if (r < 0)
         return false;
      snprintf(msg, sizeof msg, "gpubuffer %u %s %u %u %u", chain->bufbase + i, token,
               chain->extent.width, chain->extent.height, image->row_pitches[0]);
      if (plan9win_wsysctl(chain->wsys, msg) < 0) {
         if (chain->stats)
            fprintf(stderr, "wsi plan9: wsys: %s refused\n", msg);
         return false;
      }
   }
   return true;
}

static VkResult
wsi_plan9_swapchain_queue_present(struct wsi_swapchain *wsi_chain,
                                  uint32_t image_index,
                                  uint64_t present_id,
                                  const VkPresentRegionKHR *damage)
{
   struct wsi_plan9_swapchain *chain = (struct wsi_plan9_swapchain *)wsi_chain;
   struct wsi_plan9_image *image = &chain->images[image_index];
   const struct wsi_device *wsi = chain->base.wsi;
   int w, h;

   assert(image_index < chain->base.image_count);

   int64_t t[4];
   t[0] = os_time_get_nano();
   /* the blit into the host visible buffer must be done */
   if (!wsi->sw) {
      wsi->WaitForFences(chain->base.device, 1,
                         &chain->base.fences[image_index], true, UINT64_MAX);
   }

   /*
    * The screen without rio: the buffer itself goes on the display
    * (gpu(4) scanout), no copy; it stays busy until another one is
    * shown.  If the driver cannot, copy from now on.
    */
   if (chain->scanout) {
      VkDeviceMemory mem = image->base.blit.memory != VK_NULL_HANDLE ?
                           image->base.blit.memory : image->base.memory;
      t[1] = t[2] = os_time_get_nano();
      if (wsi->plan9_scanout(chain->base.device, mem, chain->extent.width,
                             chain->extent.height,
                             image->base.row_pitches[0]) == VK_SUCCESS) {
         if (chain->shown >= 0 && chain->shown != (int)image_index)
            plan9_free(chain, chain->shown);
         chain->shown = image_index;
         goto Shown;
      }
      if (chain->stats)
         fprintf(stderr, "wsi plan9: scanout refused, copying from now on\n");
      chain->scanout = false;
      chain->shown = -1;
   }

   /* in wsys: it reads the buffer itself */
   if (chain->wsys >= 0) {
      char msg[32];

      t[1] = t[2] = os_time_get_nano();
      snprintf(msg, sizeof msg, "present %u", chain->bufbase + image_index);
      if (plan9win_wsysctl(chain->wsys, msg) == 0) {
         plan9_free(chain, image_index);
         /* wsys' word changed: another kind of images (VRAM for the plane, or not) */
         int ov = p_atomic_read(&wsys_overlay);
         if (ov >= 0 && (ov == 1) != chain->overlay && chain->status == VK_SUCCESS) {
            if (chain->stats)
               fprintf(stderr, "wsi plan9: wsys says overlay %d: a new swapchain\n", ov);
            chain->status = VK_SUBOPTIMAL_KHR;
            chain->ignored = 0;
         } else if (ov == 0 && chain->overlay && chain->status == VK_SUBOPTIMAL_KHR && ++chain->ignored > 2) {
            /*
             * images in VRAM off the plane: wsys cannot show them, the window
             * stands still; an app that does not make a new swapchain on
             * VK_SUBOPTIMAL_KHR must on VK_ERROR_OUT_OF_DATE_KHR (as on a resize)
             */
            if (chain->stats)
               fprintf(stderr, "wsi plan9: suboptimal ignored off the overlay plane: out of date\n");
            chain->status = VK_ERROR_OUT_OF_DATE_KHR;
         }
         goto Shown;
      }
      if (chain->overlay) {
         /* images in VRAM, no view: no copy either; the app makes new ones */
         plan9_free(chain, image_index);
         chain->status = VK_ERROR_OUT_OF_DATE_KHR;
         return chain->status;
      }
      if (chain->stats)
         fprintf(stderr, "wsi plan9: wsys refused present, copying from now on\n");
      wsys_off(chain);
   }

   /*
    * The host visible buffer can be write-combined on the host (venus:
    * the device's memory through a dma-buf) and is slow to read: read it
    * once, with streaming loads where there are any, then hand draw(3)
    * the copy.
    */
   t[1] = os_time_get_nano();
   size_t size = (size_t)image->base.row_pitches[0] * chain->extent.height;
#if defined(USE_SSE41)
   if (util_get_cpu_caps()->has_sse4_1)
      util_streaming_load_memcpy(chain->pixels, image->base.cpu_map, size);
   else
#endif
      memcpy(chain->pixels, image->base.cpu_map, size);
   t[2] = os_time_get_nano();

   if (plan9win_put(chain->window_name, chain->pixels,
                    chain->extent.width, chain->extent.height,
                    image->base.row_pitches[0]) < 0) {
      plan9_free(chain, image_index);
      chain->status = VK_ERROR_SURFACE_LOST_KHR;
      return chain->status;
   }
   plan9_free(chain, image_index);

Shown:
   if (chain->stats) {
      t[3] = os_time_get_nano();
      plan9_stats(chain, t);
   }
   /* resized: tell the app, which makes a new swapchain */
   if (plan9win_size(chain->window_name, &w, &h) == 0 &&
       ((uint32_t)w != chain->extent.width || (uint32_t)h != chain->extent.height))
      chain->status = VK_ERROR_OUT_OF_DATE_KHR;

   return chain->status;
}

static VkResult
wsi_plan9_swapchain_wait_for_present(struct wsi_swapchain *wsi_chain,
                                     uint64_t waitValue,
                                     uint64_t timeout)
{
   return wsi_swapchain_wait_for_present_semaphore(wsi_chain, waitValue, timeout);
}

static VkResult
wsi_plan9_swapchain_destroy(struct wsi_swapchain *wsi_chain,
                            const VkAllocationCallbacks *pAllocator)
{
   struct wsi_plan9_swapchain *chain = (struct wsi_plan9_swapchain *)wsi_chain;

   /* the draw(3) screen back on the display before the buffers go */
   if (chain->shown >= 0)
      chain->base.wsi->plan9_scanout(chain->base.device, VK_NULL_HANDLE, 0, 0, 0);
   wsys_off(chain);
   for (uint32_t i = 0; i < chain->base.image_count; i++) {
      if (chain->images[i].base.image != VK_NULL_HANDLE)
         wsi_destroy_image(&chain->base, &chain->images[i].base);
   }
   wsi_swapchain_finish(&chain->base);
   cnd_destroy(&chain->freed);
   mtx_destroy(&chain->lock);
   free(chain->pixels);
   vk_free(pAllocator, chain);

   return VK_SUCCESS;
}

static VkResult
wsi_plan9_surface_create_swapchain(VkIcdSurfaceBase *icd_surface,
                                   VkDevice device,
                                   struct wsi_device *wsi_device,
                                   const VkSwapchainCreateInfoKHR *pCreateInfo,
                                   const VkAllocationCallbacks *pAllocator,
                                   struct wsi_swapchain **swapchain_out)
{
   VkIcdSurfacePlan9 *surface = (VkIcdSurfacePlan9 *)icd_surface;
   struct wsi_plan9_swapchain *chain;
   VkResult result;

   assert(pCreateInfo->sType == VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR);

   /* WSI_PLAN9_COPY=1: always through draw(3) (tests read it back there) */
   const char *copy = getenv("WSI_PLAN9_COPY");
   const bool scanout = plan9_scanout(surface, wsi_device);
   /* at least what the caps say (plan9_min_images: the WSI's own and one) */
   uint32_t num_images = MAX2(pCreateInfo->minImageCount, plan9_min_images(surface, wsi_device));
   /* in a window of wsys the buffers go there: exportable (dma-buf) */
   int wsys = -1;
   if (!scanout && !(copy && atoi(copy)) && !wsi_device->sw && num_images <= Wsysbufs / 2)
      wsys = plan9win_wsys(surface->windowName);
   size_t size = sizeof(*chain) + num_images * sizeof(chain->images[0]);
   chain = vk_zalloc(pAllocator, size, 8, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (chain == NULL) {
      if (wsys >= 0)
         close(wsys);
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }
   chain->wsys = wsys;
   if (wsys >= 0)
      chain->bufbase = __atomic_xor_fetch(&wsys_half, Wsysbufs / 2, __ATOMIC_SEQ_CST);

   /* rendered by the device, blitted into host memory for draw(3) */
   struct wsi_cpu_image_params cpu_params = {
      .base.image_type = WSI_IMAGE_TYPE_CPU,
   };

   result = wsi_swapchain_init(wsi_device, &chain->base, device,
                               pCreateInfo, &cpu_params.base, pAllocator);
   if (result != VK_SUCCESS)
      goto fail_free_chain;

   /*
    * Buffers the display may show (scanout; in wsys, an overlay plane:
    * gpu(4) overlay): rows a multiple of 256 bytes, the display's pitch
    * (AMD's DCN: 64 pixels), so that the driver copies or shows them in
    * one piece; the blit and gpubuffer take the stride from here.
    */
   if ((scanout || wsys >= 0) && chain->base.blit.type == WSI_SWAPCHAIN_BUFFER_BLIT) {
      chain->base.image_info.linear_stride = align(chain->base.image_info.linear_stride, 256);
      chain->base.image_info.linear_size = (uint64_t)chain->base.image_info.linear_stride *
                                           pCreateInfo->imageExtent.height;
   }

   /* the display reads VRAM only: the copy the display shows there, an
    * object the display may take (exportable, not one of the VM's own) */
   if (scanout && wsi_device->plan9_scanout_device_local) {
      chain->base.image_info.select_blit_dst_memory_type = wsi_plan9_select_scanout_memory_type;
      chain->base.image_info.create_mem = wsi_plan9_create_scanout_mem;
   }
   /* the images as without wsys, if the driver cannot make them exportable */
   VkResult (*plain_mem)(const struct wsi_swapchain *, const struct wsi_image_info *,
                         struct wsi_image *) = chain->base.image_info.create_mem;
   uint32_t (*plain_type)(const struct wsi_device *, uint32_t) = chain->base.image_info.select_blit_dst_memory_type;
   if (wsys >= 0) {
      pthread_once(&wsys_events_once, wsys_events_start);
      if (p_atomic_read(&wsys_overlay) == 1 && wsi_device->plan9_scanout_device_local) {
         /* on wsys' overlay plane: in VRAM, exportable, the display shows them as they are */
         chain->overlay = true;
         chain->base.image_info.select_blit_dst_memory_type = wsi_plan9_select_overlay_memory_type;
         chain->base.image_info.create_mem = wsi_plan9_create_overlay_mem;
      } else
         chain->base.image_info.create_mem = wsi_plan9_create_scanout_mem;	/* exportable, mapped */
   }

   chain->base.destroy = wsi_plan9_swapchain_destroy;
   chain->base.get_wsi_image = wsi_plan9_swapchain_get_wsi_image;
   chain->base.acquire_next_image = wsi_plan9_swapchain_acquire_next_image;
   chain->base.release_images = wsi_plan9_swapchain_release_images;
   chain->base.queue_present = wsi_plan9_swapchain_queue_present;
   chain->base.wait_for_present = wsi_plan9_swapchain_wait_for_present;
   chain->base.present_mode = wsi_swapchain_get_present_mode(wsi_device, pCreateInfo);
   chain->base.image_count = num_images;
   chain->window_name = surface->windowName;
   chain->extent = pCreateInfo->imageExtent;
   chain->status = VK_SUCCESS;
   chain->scanout = scanout;
   chain->shown = -1;
   mtx_init(&chain->lock, mtx_plain);
   cnd_init(&chain->freed);
   const char *stats = getenv("WSI_PLAN9_STATS");
   chain->stats = stats != NULL && atoi(stats) != 0;
   if (chain->stats)
      fprintf(stderr, "wsi plan9: %s: %s\n", surface->windowName ? surface->windowName : "(window)",
              scanout ? "scanout" : wsys >= 0 ? "wsys" :
              wsi_device->plan9_scanout == NULL ? "copy: the driver has no scanout" :
              (copy && atoi(copy)) ? "copy: WSI_PLAN9_COPY" : "copy: not the whole screen");

   uint32_t image = 0;
   for (; image < chain->base.image_count; image++) {
      result = wsi_create_image(&chain->base, &chain->base.image_info,
                                &chain->images[image].base);
      if (result != VK_SUCCESS && chain->wsys >= 0) {
         /* not exportable here (a driver without dma-bufs): the copy */
         if (chain->stats)
            fprintf(stderr, "wsi plan9: no exportable images (%d), copying\n", result);
         for (uint32_t i = 0; i < image; i++)
            wsi_destroy_image(&chain->base, &chain->images[i].base);
         wsys_off(chain);
         chain->overlay = false;
         chain->base.image_info.select_blit_dst_memory_type = plain_type;
         chain->base.image_info.create_mem = plain_mem;
         image = 0;
         result = wsi_create_image(&chain->base, &chain->base.image_info,
                                   &chain->images[image].base);
      }
      if (result != VK_SUCCESS)
         goto fail_destroy_images;
      if (chain->images[image].base.cpu_map == NULL && !chain->overlay) {
         result = VK_ERROR_OUT_OF_HOST_MEMORY;
         image++;
         goto fail_destroy_images;
      }
   }

   /* wsys takes the buffers, or the copy after all */
   if (chain->wsys >= 0 && !wsys_buffers(chain))
      wsys_off(chain);
   if (chain->stats && wsys >= 0)
      fprintf(stderr, "wsi plan9: %s\n", chain->wsys >= 0 ? chain->overlay ? "wsys has the buffers, in VRAM for its overlay plane" :
              "wsys has the buffers" : "wsys refused, copying");

   /* 64-byte aligned for the streaming loads */
   size_t frame = (size_t)chain->images[0].base.row_pitches[0] * chain->extent.height;
   chain->pixels = aligned_alloc(64, (frame + 63) & ~(size_t)63);
   if (chain->pixels == NULL) {
      result = VK_ERROR_OUT_OF_HOST_MEMORY;
      goto fail_destroy_images;
   }

   *swapchain_out = &chain->base;
   return VK_SUCCESS;

fail_destroy_images:
   wsys_off(chain);
   for (uint32_t i = 0; i < image; i++)
      wsi_destroy_image(&chain->base, &chain->images[i].base);
   wsi_swapchain_finish(&chain->base);
   cnd_destroy(&chain->freed);
   mtx_destroy(&chain->lock);
fail_free_chain:
   if (chain->wsys >= 0)
      close(chain->wsys);
   vk_free(pAllocator, chain);
   return result;
}

/* as wsi_create_cpu_buffer_image_mem, the copy exportable (a dma-buf:
 * the display driver imports it) */
static VkResult
wsi_plan9_create_scanout_mem(const struct wsi_swapchain *chain, const struct wsi_image_info *info,
                             struct wsi_image *image)
{
   VkResult result;

   result = wsi_create_buffer_blit_context(chain, info, image, VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
   if (result != VK_SUCCESS)
      return result;
   return chain->wsi->MapMemory(chain->device, image->blit.memory, 0, VK_WHOLE_SIZE, 0, &image->cpu_map);
}

/* device local and host visible (VRAM the processor sees: it copies from
 * there if the display refuses), else what any type of type_bits is
 */
static uint32_t
wsi_plan9_select_scanout_memory_type(const struct wsi_device *wsi, uint32_t type_bits)
{
   const VkMemoryPropertyFlags want = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

   u_foreach_bit(t, type_bits) {
      if ((wsi->memory_props.memoryTypes[t].propertyFlags & want) == want)
         return t;
   }
   u_foreach_bit(t, type_bits) {
      if (wsi->memory_props.memoryTypes[t].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
         return t;
   }
   return ffs(type_bits) - 1;
}

VkResult
wsi_plan9_init_wsi(struct wsi_device *wsi_device,
                   const VkAllocationCallbacks *alloc,
                   VkPhysicalDevice physical_device)
{
   struct wsi_plan9 *wsi;

   wsi = vk_alloc(alloc, sizeof(*wsi), 8, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (!wsi) {
      wsi_device->wsi[VK_ICD_WSI_PLATFORM_PLAN9] = NULL;
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   wsi->base.get_support = wsi_plan9_surface_get_support;
   wsi->base.get_capabilities2 = wsi_plan9_surface_get_capabilities2;
   wsi->base.get_formats = wsi_plan9_surface_get_formats;
   wsi->base.get_formats2 = wsi_plan9_surface_get_formats2;
   wsi->base.get_present_modes = wsi_plan9_surface_get_present_modes;
   wsi->base.get_present_rectangles = wsi_plan9_surface_get_present_rectangles;
   wsi->base.create_swapchain = wsi_plan9_surface_create_swapchain;

   wsi_device->wsi[VK_ICD_WSI_PLATFORM_PLAN9] = &wsi->base;

   return VK_SUCCESS;
}

void
wsi_plan9_finish_wsi(struct wsi_device *wsi_device,
                     const VkAllocationCallbacks *alloc)
{
   struct wsi_plan9 *wsi =
      (struct wsi_plan9 *)wsi_device->wsi[VK_ICD_WSI_PLATFORM_PLAN9];
   if (!wsi)
      return;

   vk_free(alloc, wsi);
}

VKAPI_ATTR VkResult VKAPI_CALL
wsi_CreatePlan9SurfacePLAN9(VkInstance _instance,
                            const VkPlan9SurfaceCreateInfoPLAN9 *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkSurfaceKHR *pSurface)
{
   VK_FROM_HANDLE(vk_instance, instance, _instance);
   VkIcdSurfacePlan9 *surface;
   const char *name = pCreateInfo->pWindowName;
   size_t n = name ? strlen(name) + 1 : 0;

   assert(pCreateInfo->sType == VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9);

   /* the name in the same allocation: vkDestroySurfaceKHR frees one */
   surface = vk_alloc2(&instance->alloc, pAllocator, sizeof(*surface) + n, 8,
                       VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (surface == NULL)
      return VK_ERROR_OUT_OF_HOST_MEMORY;

   surface->base.platform = VK_ICD_WSI_PLATFORM_PLAN9;
   surface->windowName = NULL;
   if (name) {
      surface->windowName = (char *)(surface + 1);
      memcpy(surface->windowName, name, n);
   }

   *pSurface = VkIcdSurfaceBase_to_handle(&surface->base);
   return VK_SUCCESS;
}

/* the overlay plane's images (variant b): exportable, in VRAM, no view for the processor */
static VkResult
wsi_plan9_create_overlay_mem(const struct wsi_swapchain *chain, const struct wsi_image_info *info,
                             struct wsi_image *image)
{
   return wsi_create_buffer_blit_context(chain, info, image, VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
}

/* device local, rather one the processor does not see (the part of VRAM BAR 0 does not show) */
static uint32_t
wsi_plan9_select_overlay_memory_type(const struct wsi_device *wsi, uint32_t type_bits)
{
   u_foreach_bit(t, type_bits) {
      VkMemoryPropertyFlags f = wsi->memory_props.memoryTypes[t].propertyFlags;
      if ((f & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) && !(f & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
         return t;
   }
   u_foreach_bit(t, type_bits) {
      if (wsi->memory_props.memoryTypes[t].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
         return t;
   }
   return ffs(type_bits) - 1;
}
