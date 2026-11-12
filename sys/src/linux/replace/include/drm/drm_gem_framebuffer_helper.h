/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: drm/drm_gem_framebuffer_helper.h (ours, Linux's
 * has no notice): framebuffers whose planes are GEM objects
 * (replace/drivers/gpu/drm/drm_gem_framebuffer_helper.c, OpenBSD's)
 */
#ifndef _KPI_DRM_GEM_FRAMEBUFFER_HELPER_H
#define _KPI_DRM_GEM_FRAMEBUFFER_HELPER_H

struct drm_device;
struct drm_file;
struct drm_framebuffer;
struct drm_gem_object;
struct drm_mode_fb_cmd2;

struct drm_gem_object	*drm_gem_fb_get_obj(struct drm_framebuffer *fb, unsigned int plane);
void	drm_gem_fb_destroy(struct drm_framebuffer *fb);
int	drm_gem_fb_create_handle(struct drm_framebuffer *fb, struct drm_file *file, unsigned int *handle);
struct drm_framebuffer	*drm_gem_fb_create(struct drm_device *dev, struct drm_file *file,
	const struct drm_format_info *info, const struct drm_mode_fb_cmd2 *cmd);

#endif
