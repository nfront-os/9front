/*
 * SPDX-License-Identifier: MIT
 *
 * EGL's plan9 platform (9front): GL through zink, whose kopper presents
 * a window surface's frames by Vulkan's VK_PLAN9_surface (Mesa's plan9
 * WSI: copied into a draw(3) window, or to the display directly when
 * the window is the screen).  The native window is a window image's
 * name (a char *), or "": the program's own window (/dev/winname, read
 * again at each frame: rio gives a resized window a new name; EGL takes
 * no NULL window).  The
 * native display is EGL_DEFAULT_DISPLAY.  Pbuffers as on surfaceless.
 */

#include <stdlib.h>
#include <string.h>

#include "egl_dri2.h"
#include "kopper_interface.h"
#include "dri_util.h"
#include "plan9win.h"
#include "util/format/u_formats.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_plan9.h>

static_assert(sizeof(struct kopper_vk_surface_create_storage) >=
                 sizeof(VkPlan9SurfaceCreateInfoPLAN9),
              "");

static void
plan9_set_surface_create_info(void *draw, struct kopper_loader_info *ci)
{
   struct dri2_egl_surface *dri2_surf = draw;
   VkPlan9SurfaceCreateInfoPLAN9 *p9 = (VkPlan9SurfaceCreateInfoPLAN9 *)&ci->bos;

   p9->sType = VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9;
   p9->pNext = NULL;
   p9->flags = 0;
   p9->pWindowName = dri2_surf->plan9_name;
   ci->has_alpha = 0;		/* draw(3) windows are opaque */
   ci->present_opaque = true;
   ci->native = dri2_surf;	/* a NULL name is no identity */
}

static void
plan9_get_drawable_info(struct dri_drawable *draw, int *w, int *h, void *loaderPrivate)
{
   struct dri2_egl_surface *dri2_surf = loaderPrivate;

   if (dri2_surf->base.Type != EGL_WINDOW_BIT) {
      kopper_pbuffer_loader_extension.GetDrawableInfo(draw, w, h, loaderPrivate);
      return;
   }
   if (plan9win_size(dri2_surf->plan9_name, w, h) < 0)
      *w = *h = 0;
}

static const __DRIkopperLoaderExtension plan9_kopper_loader_extension = {
   .base = {__DRI_KOPPER_LOADER, 1},
   .SetSurfaceCreateInfo = plan9_set_surface_create_info,
   .GetDrawableInfo = plan9_get_drawable_info,
};

static const __DRIextension *plan9_loader_extensions[] = {
   &plan9_kopper_loader_extension.base,
   &image_lookup_extension.base,
   NULL,
};

static _EGLSurface *
plan9_create_surface(_EGLDisplay *disp, EGLint type, _EGLConfig *conf,
                     void *native_window, const EGLint *attrib_list)
{
   struct dri2_egl_display *dri2_dpy = dri2_egl_display(disp);
   struct dri2_egl_config *dri2_conf = dri2_egl_config(conf);
   struct dri2_egl_surface *dri2_surf;
   const struct dri_config *config;
   int w, h;

   dri2_surf = calloc(1, sizeof *dri2_surf);
   if (!dri2_surf) {
      _eglError(EGL_BAD_ALLOC, "plan9_create_surface");
      return NULL;
   }
   if (!dri2_init_surface(&dri2_surf->base, disp, type, conf, attrib_list,
                          false, native_window))
      goto fail;

   if (type == EGL_WINDOW_BIT) {
      if (native_window != NULL && *(const char *)native_window != '\0')
         dri2_surf->plan9_name = strdup(native_window);
      if (plan9win_size(dri2_surf->plan9_name, &w, &h) < 0) {
         _eglError(EGL_BAD_NATIVE_WINDOW, "no such window (draw(3))");
         goto fail;
      }
      dri2_surf->base.Width = w;
      dri2_surf->base.Height = h;
   }

   config = dri2_get_dri_config(dri2_conf, type, dri2_surf->base.GLColorspace);
   if (!config) {
      _eglError(EGL_BAD_MATCH, "Unsupported surfacetype/colorspace configuration");
      goto fail;
   }
   if (type != EGL_WINDOW_BIT) {
      dri2_surf->visual = dri2_image_format_for_pbuffer_config(dri2_dpy, config);
      if (dri2_surf->visual == PIPE_FORMAT_NONE)
         goto fail;
   }
   if (!dri2_create_drawable(dri2_dpy, config, dri2_surf, dri2_surf))
      goto fail;
   if (type == EGL_WINDOW_BIT) {
      dri2_surf->base.SwapInterval = 1;
      kopperSetSwapInterval(dri2_surf->dri_drawable, dri2_dpy->default_swap_interval);
   }
   return &dri2_surf->base;

fail:
   free(dri2_surf->plan9_name);
   free(dri2_surf);
   return NULL;
}

static _EGLSurface *
plan9_create_window_surface(_EGLDisplay *disp, _EGLConfig *conf,
                            void *native_window, const EGLint *attrib_list)
{
   return plan9_create_surface(disp, EGL_WINDOW_BIT, conf, native_window, attrib_list);
}

static _EGLSurface *
plan9_create_pbuffer_surface(_EGLDisplay *disp, _EGLConfig *conf,
                             const EGLint *attrib_list)
{
   return plan9_create_surface(disp, EGL_PBUFFER_BIT, conf, NULL, attrib_list);
}

static EGLBoolean
plan9_destroy_surface(_EGLDisplay *disp, _EGLSurface *surf)
{
   struct dri2_egl_surface *dri2_surf = dri2_egl_surface(surf);

   driDestroyDrawable(dri2_surf->dri_drawable);
   free(dri2_surf->plan9_name);
   dri2_fini_surface(surf);
   free(dri2_surf);
   return EGL_TRUE;
}

static EGLBoolean
plan9_swap_buffers_with_damage(_EGLDisplay *disp, _EGLSurface *draw,
                               const EGLint *rects, EGLint nrects)
{
   struct dri2_egl_surface *dri2_surf = dri2_egl_surface(draw);

   if (draw->Type != EGL_WINDOW_BIT)
      return EGL_TRUE;
   if (nrects)
      kopperSwapBuffersWithDamage(dri2_surf->dri_drawable,
                                  __DRI2_FLUSH_CONTEXT | __DRI2_FLUSH_INVALIDATE_ANCILLARY,
                                  nrects, rects);
   else
      kopperSwapBuffers(dri2_surf->dri_drawable,
                        __DRI2_FLUSH_CONTEXT | __DRI2_FLUSH_INVALIDATE_ANCILLARY);
   /* a resized window: kopper made a swapchain of the new size */
   kopperQuerySurfaceSize(dri2_surf->dri_drawable, &dri2_surf->base.Width,
                          &dri2_surf->base.Height);
   return EGL_TRUE;
}

static EGLBoolean
plan9_swap_buffers(_EGLDisplay *disp, _EGLSurface *draw)
{
   return plan9_swap_buffers_with_damage(disp, draw, NULL, 0);
}

static EGLBoolean
plan9_swap_interval(_EGLDisplay *disp, _EGLSurface *surf, EGLint interval)
{
   struct dri2_egl_surface *dri2_surf = dri2_egl_surface(surf);

   kopperSetSwapInterval(dri2_surf->dri_drawable, interval);
   return EGL_TRUE;
}

static EGLint
plan9_query_buffer_age(_EGLDisplay *disp, _EGLSurface *surf)
{
   struct dri2_egl_surface *dri2_surf = dri2_egl_surface(surf);

   return kopperQueryBufferAge(dri2_surf->dri_drawable);
}

static const struct dri2_egl_display_vtbl plan9_display_vtbl = {
   .create_window_surface = plan9_create_window_surface,
   .create_pbuffer_surface = plan9_create_pbuffer_surface,
   .destroy_surface = plan9_destroy_surface,
   .create_image = dri2_create_image_khr,
   .swap_interval = plan9_swap_interval,
   .swap_buffers = plan9_swap_buffers,
   .swap_buffers_with_damage = plan9_swap_buffers_with_damage,
   .query_buffer_age = plan9_query_buffer_age,
   .get_dri_drawable = dri2_surface_get_dri_drawable,
};

/*
 * the configs: every pbuffer one; a window takes those the WSI's
 * swapchains have (B8G8R8A8, unorm or sRGB: plan9 WSI)
 */
static void
plan9_add_configs(_EGLDisplay *disp)
{
   struct dri2_egl_display *dri2_dpy = dri2_egl_display(disp);

   for (unsigned i = 0; dri2_dpy->driver_configs[i] != NULL; i++) {
      const struct gl_config *gl_config = (const struct gl_config *)dri2_dpy->driver_configs[i];
      EGLint types = EGL_PBUFFER_BIT;

      switch (gl_config->color_format) {
      case PIPE_FORMAT_BGRA8888_UNORM:
      case PIPE_FORMAT_BGRX8888_UNORM:
      case PIPE_FORMAT_BGRA8888_SRGB:
      case PIPE_FORMAT_BGRX8888_SRGB:
         types |= EGL_WINDOW_BIT;
         break;
      default:
         break;
      }
      dri2_add_config(disp, dri2_dpy->driver_configs[i], types, NULL);
   }
}

EGLBoolean
dri2_initialize_plan9(_EGLDisplay *disp)
{
   struct dri2_egl_display *dri2_dpy = dri2_egl_display(disp);

   /* zink, on the Vulkan devices it finds itself (no DRM node to open) */
   dri2_dpy->fd_render_gpu = -1;
   dri2_dpy->fd_display_gpu = -1;
   disp->Device = _eglFindDevice(-1, true);
   dri2_dpy->driver_name = strdup("zink");
   if (!dri2_dpy->driver_name)
      return _eglError(EGL_BAD_ALLOC, "eglInitialize");
   dri2_detect_swrast_kopper(disp);
   dri2_dpy->loader_extensions = plan9_loader_extensions;
   if (!dri2_create_screen(disp)) {
      free(dri2_dpy->driver_name);
      dri2_dpy->driver_name = NULL;
      return _eglError(EGL_NOT_INITIALIZED, "plan9: zink did not start");
   }
   dri2_setup_screen(disp);
   plan9_add_configs(disp);
   dri2_dpy->min_swap_interval = 0;
   dri2_dpy->max_swap_interval = 1;
   dri2_dpy->default_swap_interval = 1;
   dri2_dpy->vtbl = &plan9_display_vtbl;
   return EGL_TRUE;
}
