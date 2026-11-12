/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, sys/reboot.h: reboot(2)'s
 * flags, for orderly_poweroff (linux/reboot.h).  reboot runs fshalt,
 * 9front's way down (kpi/obsd/systm.c).
 */
#ifndef _SYS_REBOOT_H_
#define _SYS_REBOOT_H_

#define RB_AUTOBOOT	0
#define RB_HALT		0x0008
#define RB_NOSYNC	0x0004
#define RB_POWERDOWN	0x1000

__dead void	reboot(int);

#endif
