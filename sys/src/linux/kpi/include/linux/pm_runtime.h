/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pm_runtime.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PM_RUNTIME_H
#define _KPI_LINUX_PM_RUNTIME_H

#include <linux/device.h>

/* no runtime power management: the device stays awake */
#define pm_runtime_get_sync(d)			1
#define pm_runtime_get_noresume(d)		do { } while (0)
#define pm_runtime_get_if_in_use(d)		1
#define pm_runtime_get_if_active(d)		1	/* 9front: Linux 6.9 dropped ign_usage_count */
#define pm_runtime_resume_and_get(d)		0
#define pm_runtime_put(d)			0
#define pm_runtime_put_sync(d)			0
#define pm_runtime_put_noidle(d)		do { } while (0)
#define pm_runtime_put_autosuspend(d)		0
#define pm_runtime_autosuspend(d)		0	/* no runtime suspend here */
#define pm_runtime_autosuspend_expiration(d)	0UL
#define pm_runtime_mark_last_busy(d)		do { } while (0)
#define pm_runtime_use_autosuspend(d)		do { } while (0)
#define pm_runtime_dont_use_autosuspend(d)	do { } while (0)
#define pm_runtime_set_autosuspend_delay(d, m)	do { } while (0)
#define pm_runtime_set_active(d)		0
#define pm_runtime_allow(d)			do { } while (0)
#define pm_runtime_forbid(d)			do { } while (0)
#define pm_runtime_enable(d)			do { } while (0)
#define pm_runtime_disable(d)			do { } while (0)
#define pm_runtime_suspended(d)			0
#define pm_runtime_active(d)			1
#define pm_runtime_status_suspended(d)		0
#define pm_runtime_enabled(d)			0
#define pm_runtime_resume(d)			0
#define pm_runtime_idle(d)			0
#define pm_runtime_suspend(d)			0
#define pm_runtime_set_suspended(d)		0
#define pm_runtime_no_callbacks(d)		do { } while (0)
#define pm_runtime_irq_safe(d)			do { } while (0)
#define pm_runtime_barrier(d)			do { } while (0)
#define pm_runtime_get_noresume_unless(d)	1
#define pm_suspend_ignore_children(d, e)	do { } while (0)
#define pm_runtime_get_if_in_use_or_active(d)	1
#define pm_runtime_suspended_if_enabled(d)	0

#endif
