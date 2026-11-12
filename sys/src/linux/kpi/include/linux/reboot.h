/* Public domain. */

#ifndef _LINUX_REBOOT_H
#define _LINUX_REBOOT_H

#include <sys/reboot.h>

struct notifier_block;

#define SYS_RESTART 0

static inline void
register_reboot_notifier(struct notifier_block *nb)
{
}

static inline void
unregister_reboot_notifier(struct notifier_block *nb)
{
}

/* 9front: the machine goes down as 9front takes it down (reboot, kpi/obsd/systm.c) */
static inline void
emergency_restart(void)
{
	reboot(RB_AUTOBOOT | RB_NOSYNC);
}

static inline void
orderly_poweroff(bool force)
{
	if (force)
		reboot(RB_HALT | RB_POWERDOWN | RB_NOSYNC);
	else
		reboot(RB_HALT | RB_POWERDOWN);
}

#endif
