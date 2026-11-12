// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: GEM objects that are TTM buffer objects: their
 * object functions for mappings go to TTM's (Linux's file is GPL; this
 * one is ours, after what the functions are for).
 */
#include <linux/iosys-map.h>
#include <linux/mm.h>
#include <drm/drm_gem_ttm_helper.h>
#include <drm/ttm/ttm_bo.h>

/*
 * a mapping of the object in a process: TTM's fault handler; TTM holds
 * the object for the mapping itself, so the reference the GEM mmap took
 * (drm_gem_mmap_obj) goes
 */
int
drm_gem_ttm_mmap(struct drm_gem_object *gem, struct vm_area_struct *vma)
{
	int r;

	r = ttm_bo_mmap_obj(vma, drm_gem_ttm_of_gem(gem));
	if (r < 0)
		return r;
	drm_gem_object_put(gem);
	return 0;
}

/* the object in the driver's address space */
int
drm_gem_ttm_vmap(struct drm_gem_object *gem, struct iosys_map *map)
{
	return ttm_bo_vmap(drm_gem_ttm_of_gem(gem), map);
}

void
drm_gem_ttm_vunmap(struct drm_gem_object *gem, struct iosys_map *map)
{
	ttm_bo_vunmap(drm_gem_ttm_of_gem(gem), map);
}
