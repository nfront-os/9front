/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: our own declarations of selftests/mock_gem_device.c
 * (MIT; Linux's header is GPL): an i915 device without hardware for the
 * mock selftests.
 */
#ifndef __MOCK_GEM_DEVICE_H__
#define __MOCK_GEM_DEVICE_H__

struct drm_i915_private;

struct drm_i915_private	*mock_gem_device(void);
void	mock_device_flush(struct drm_i915_private *i915);
void	mock_destroy_device(struct drm_i915_private *i915);

#endif
