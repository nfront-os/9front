/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pm.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PM_H
#define _KPI_LINUX_PM_H

#include <linux/kernel.h>
struct device;

typedef struct pm_message { int event; } pm_message_t;
#define PM_EVENT_SUSPEND	0x0002
#define PM_EVENT_FREEZE		0x0001
#define PM_EVENT_HIBERNATE	0x0004
#define PMSG_SUSPEND		((struct pm_message){ .event = PM_EVENT_SUSPEND })
#define PMSG_FREEZE		((struct pm_message){ .event = PM_EVENT_FREEZE })
#define PMSG_HIBERNATE		((struct pm_message){ .event = PM_EVENT_HIBERNATE })
#define DPM_FLAG_NO_DIRECT_COMPLETE	0x1
int	pm_generic_suspend(struct device *d);
int	pm_generic_resume(struct device *d);
int	pm_generic_freeze(struct device *d);
int	pm_generic_thaw(struct device *d);
int	pm_generic_poweroff(struct device *d);
int	pm_generic_restore(struct device *d);
int	pm_generic_runtime_suspend(struct device *d);
int	pm_generic_runtime_resume(struct device *d);
#define pm_suspend_via_firmware()	0
#define pm_resume_via_firmware()	0
#include <linux/suspend.h>	/* suspend_state_t, pm_suspend_target_state */
extern const char power_group_name[];


#ifndef pm_sleep_ptr
#define pm_sleep_ptr(p)	(p)	/* 9front: CONFIG_PM_SLEEP is there */
#endif

#ifndef pm_ptr
#define pm_ptr(p)	(p)	/* 9front: CONFIG_PM is there */
#endif

#endif
