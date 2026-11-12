/* Public domain. */

#ifndef _LINUX_SUSPEND_H
#define _LINUX_SUSPEND_H

typedef int suspend_state_t;

#define PM_SUSPEND_ON		0
#define PM_SUSPEND_MEM		1
#define PM_SUSPEND_TO_IDLE	2

extern suspend_state_t pm_suspend_target_state;

/* 9front: the file systems are not ours to sync */
#define ksys_sync_helper()	do { } while (0)

#define pm_hibernate_is_recovering()	false	/* 9front: no hibernation */

#define pm_hibernation_mode_is_suspend()	false	/* 9front: no hibernation */

/* 9front: no system sleep notifications */
#define PM_HIBERNATION_PREPARE	1
#define PM_POST_HIBERNATION	2
#define PM_SUSPEND_PREPARE	3
#define PM_POST_SUSPEND		4
#define register_pm_notifier(nb)	0
#define unregister_pm_notifier(nb)	0

#endif
