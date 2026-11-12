/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/drm_lease.h (ours: Linux's is GPL or has no notice) */
#ifndef _KPI_DRM_DRM_LEASE_H
#define _KPI_DRM_DRM_LEASE_H

#include <linux/types.h>

/* no leases: a lease is a Linux file descriptor; the master holds all */
struct drm_file;
struct drm_device;
struct drm_master;
void	drm_lease_destroy(struct drm_master *lessee);
bool	drm_lease_held(struct drm_file *f, int id);
bool	_drm_lease_held(struct drm_file *f, int id);
void	drm_lease_revoke(struct drm_master *master);
uint32_t	drm_lease_filter_crtcs(struct drm_file *f, uint32_t crtcs);
struct drm_master	*drm_lease_owner(struct drm_master *master);
int	drm_mode_create_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *f);
int	drm_mode_list_lessees_ioctl(struct drm_device *dev, void *data, struct drm_file *f);
int	drm_mode_get_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *f);
int	drm_mode_revoke_lease_ioctl(struct drm_device *dev, void *data, struct drm_file *f);

#endif
