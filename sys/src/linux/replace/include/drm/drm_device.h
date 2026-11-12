/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/drm_device.h (ours: Linux's is GPL or has no notice) */
#ifndef _KPI_DRM_DRM_DEVICE_H
#define _KPI_DRM_DRM_DEVICE_H

#include <linux/kref.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/idr.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <drm/drm_mode_config.h>

/* how a wedged device can recover (drm_dev_wedged_event) */
#define DRM_WEDGE_RECOVERY_NONE		BIT(0)
#define DRM_WEDGE_RECOVERY_REBIND	BIT(1)
#define DRM_WEDGE_RECOVERY_BUS_RESET	BIT(2)
#define DRM_WEDGE_RECOVERY_VENDOR	BIT(3)

/* the task a wedge is blamed on */
struct drm_wedge_task_info {
	pid_t	pid;
	char	comm[TASK_COMM_LEN];
};

struct dentry;
struct drm_driver;
struct drm_fb_helper;
struct drm_master;
struct drm_minor;
struct drm_vblank_crtc;
struct drm_vma_offset_manager;
struct inode;
struct pci_dev;

/* vga_switcheroo's states of a device */
enum switch_power_state {
	DRM_SWITCH_POWER_ON = 0,
	DRM_SWITCH_POWER_OFF = 1,
	DRM_SWITCH_POWER_CHANGING = 2,
	DRM_SWITCH_POWER_DYNAMIC_OFF = 3,
};

/*
 * a DRM device: the one the file server drives.  Only the members the
 * drivers, the DRM core and the LinuxKPI use, by what they are for.
 */
struct drm_device {
	/* the device and its driver */
	const struct drm_driver	*driver;
	u32	driver_features;
	struct device	*dev;
	struct pci_dev	*pdev;		/* the device, for the drivers that ask */
	struct device	*dma_dev;	/* for DMA, if not dev (drm_dev_dma_dev) */
	char	*unique;
	int	if_version;
	struct kref	ref;
	bool	registered;
	bool	unplugged;
	enum switch_power_state	switch_power_state;
	struct dentry	*debugfs_root;

	/* drmm_*: what is freed with the device */
	struct {
		spinlock_t	lock;
		struct list_head	resources;
		void	*final_kfree;
	} managed;

	/* the minors, their files, the master and the in-kernel clients */
	struct drm_minor	*primary;
	struct drm_minor	*render;
	struct drm_minor	*accel;
	struct inode	*anon_inode;
	atomic_t	open_count;
	struct mutex	filelist_mutex;
	struct list_head	filelist;
	struct list_head	filelist_internal;
	struct mutex	master_mutex;
	struct drm_master	*master;
	struct mutex	clientlist_mutex;
	struct list_head	clientlist;
	struct list_head	client_sysrq_list;
	struct drm_fb_helper	*fb_helper;

	/* GEM: names, mappings, the LRU */
	struct mutex	object_name_lock;
	struct idr	object_name_idr;
	struct drm_vma_offset_manager	*vma_offset_manager;
	struct mutex	gem_lru_mutex;

	/* modes, vertical blanks and their events */
	struct drm_mode_config	mode_config;
	unsigned int	num_crtcs;
	struct drm_vblank_crtc	*vblank;
	u32	max_vblank_count;
	bool	vblank_disable_immediate;
	spinlock_t	vbl_lock;
	spinlock_t	vblank_time_lock;
	spinlock_t	event_lock;
	struct list_head	vblank_event_list;
};

void	drm_dev_set_dma_dev(struct drm_device *dev, struct device *dma_dev);

/* the device DMA goes through */
static inline struct device *
drm_dev_dma_dev(struct drm_device *dev)
{
	return dev->dma_dev != NULL ? dev->dma_dev : dev->dev;
}

#endif
