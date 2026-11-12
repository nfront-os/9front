/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/oom.h (ours, not Linux's) */
#ifndef _KPI_LINUX_OOM_H
#define _KPI_LINUX_OOM_H

#include <linux/kernel.h>
#include <linux/notifier.h>

#define register_oom_notifier(n)	0
#define unregister_oom_notifier(n)	0

#endif
