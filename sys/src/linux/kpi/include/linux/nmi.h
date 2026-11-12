/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/nmi.h (ours, not Linux's) */
#ifndef _KPI_LINUX_NMI_H
#define _KPI_LINUX_NMI_H

#include <linux/kernel.h>

#define touch_nmi_watchdog()	do { } while (0)

#endif
