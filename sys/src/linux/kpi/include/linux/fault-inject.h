/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/fault-inject.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FAULT_INJECT_H
#define _KPI_LINUX_FAULT_INJECT_H

#include <linux/kernel.h>

/*
 * fault injection for the selftests (kpi/linux/lib.c, ours): after
 * space bytes, every interval-th call, with probability percent, times
 * times (-1: always)
 */
struct fault_attr {
	unsigned long	probability;
	unsigned long	interval;
	atomic_t	times;
	atomic_t	space;
	unsigned long	verbose;
	unsigned long	count;
};
#define FAULT_ATTR_INITIALIZER		{ .interval = 1, .times = ATOMIC_INIT(1), .verbose = 2 }
#define DECLARE_FAULT_ATTR(n)		struct fault_attr n = FAULT_ATTR_INITIALIZER
bool	should_fail(struct fault_attr *a, ssize_t size);
#define fault_create_debugfs_attr(n, p, a)	((struct dentry *)NULL)

#endif
