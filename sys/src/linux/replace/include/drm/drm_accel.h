/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/drm_accel.h (ours: Linux's is GPL) */
#ifndef _KPI_DRM_DRM_ACCEL_H
#define _KPI_DRM_DRM_ACCEL_H

#include <linux/errno.h>

/* no compute accelerators (CONFIG_DRM_ACCEL off) */
#define accel_minor_alloc()		(-EOPNOTSUPP)
#define drm_accel_minor_alloc()		(-EOPNOTSUPP)
#define accel_minor_replace(m, i)	do { } while (0)
#define accel_minor_remove(i)		do { } while (0)
#define drm_accel_core_init()		0
#define drm_accel_core_exit()		do { } while (0)
#define accel_core_exit()		do { } while (0)
#define accel_core_init()		0
#define accel_debugfs_init(m, i)	do { } while (0)
#define drm_accel_debugfs_init(d)	do { } while (0)
#define drm_accel_debugfs_register(d)	do { } while (0)
#define drm_accel_pm_resume_test(d)	0

#define ACCEL_MAX_MINORS	256	/* 9front: Linux's limit, none made here */

#define accel_debugfs_register(d)	do { } while (0)

#endif
