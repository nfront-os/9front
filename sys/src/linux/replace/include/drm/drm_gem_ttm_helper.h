/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: drm/drm_gem_ttm_helper.h (ours: Linux's is GPL):
 * GEM objects that are TTM buffer objects (replace/drivers/gpu/drm/
 * drm_gem_ttm_helper.c)
 */
#ifndef _KPI_DRM_GEM_TTM_HELPER_H
#define _KPI_DRM_GEM_TTM_HELPER_H

#include <linux/kernel.h>
#include <drm/drm_gem.h>
#include <drm/drm_device.h>
#include <drm/ttm/ttm_bo.h>

struct iosys_map;
struct vm_area_struct;

#define drm_gem_ttm_of_gem(gem)	container_of(gem, struct ttm_buffer_object, base)

int	drm_gem_ttm_mmap(struct drm_gem_object *gem, struct vm_area_struct *vma);
int	drm_gem_ttm_vmap(struct drm_gem_object *gem, struct iosys_map *map);
void	drm_gem_ttm_vunmap(struct drm_gem_object *gem, struct iosys_map *map);

#endif
