/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: what the files of kpi/linux share.  The procs,
 * their sleep and the OpenBSD kernel API under OpenBSD's LinuxKPI
 * headers are kpi/obsd's (../obsd/obsd.h).
 */
#ifndef _KPI_LINUX_KPI_H
#define _KPI_LINUX_KPI_H

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <kpi/p9.h>
#include "../obsd/obsd.h"

void	kpi_init(void);
extern int kpi_started;

#endif
