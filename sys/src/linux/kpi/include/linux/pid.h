/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pid.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PID_H
#define _KPI_LINUX_PID_H

#include <linux/sched.h>



/* 9front: no pid namespaces to look into */
struct pid;
#define find_get_pid(n)	((struct pid *)NULL)
#define put_pid(p)	do { } while (0)

#endif
