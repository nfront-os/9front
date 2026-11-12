// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: drm_lease.c (ours: Linux's is GPL).
 *
 * Leases hand part of the display to another process as a Linux file
 * descriptor.  The file server here has no leases: every master owns
 * everything, and the lease ioctls fail.
 */
#include <linux/errno.h>
#include <drm/drm_auth.h>
#include <drm/drm_file.h>
#include <drm/drm_lease.h>

#include "drm_internal.h"

struct drm_master *drm_lease_owner(struct drm_master *master)
{
	return master;
}

bool _drm_lease_held(struct drm_file *file_priv, int id)
{
	return true;
}

bool drm_lease_held(struct drm_file *file_priv, int id)
{
	return true;
}

uint32_t drm_lease_filter_crtcs(struct drm_file *file_priv, uint32_t crtcs_in)
{
	return crtcs_in;
}

void drm_lease_destroy(struct drm_master *master)
{
}

void drm_lease_revoke(struct drm_master *top)
{
}

int drm_mode_create_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	return -EOPNOTSUPP;
}

int drm_mode_list_lessees_ioctl(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	return -EOPNOTSUPP;
}

int drm_mode_get_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	return -EOPNOTSUPP;
}

int drm_mode_revoke_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	return -EOPNOTSUPP;
}
