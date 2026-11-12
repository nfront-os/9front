/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/sysctl.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SYSCTL_H
#define _KPI_LINUX_SYSCTL_H

#include <linux/kernel.h>

struct ctl_table { const char *procname; void *data; int maxlen; umode_t mode; void *proc_handler; void *extra1; void *extra2; };
struct ctl_table_header;
#define register_sysctl(p, t)	((struct ctl_table_header *)NULL)
#define unregister_sysctl_table(h)	do { } while (0)
#define SYSCTL_ZERO	((void *)NULL)
#define SYSCTL_ONE	((void *)NULL)
int	proc_dointvec_minmax(struct ctl_table *t, int w, void *b, size_t *l, loff_t *p);

#endif
