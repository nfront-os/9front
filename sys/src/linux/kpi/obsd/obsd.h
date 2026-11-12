/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: what the files of kpi/obsd share.  They are the
 * OpenBSD kernel API (kpi/obsd/include) on 9front, through kpi/p9.h.
 */
#ifndef _KPI_OBSD_H
#define _KPI_OBSD_H

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mutex.h>
#include <sys/rwlock.h>
#include <sys/atomic.h>
#include <kpi/p9.h>

struct page;
struct page	*kpi_virt_to_page(const void *);	/* kpi/linux/page.c */
struct page	*kpi_pfn_to_page(unsigned long);

/* a proc made by the KPI: runs fn(arg) with its own struct task_struct */
struct task_struct	*kpi_newproc(const char *name, void (*fn)(void *), void *arg);
/* the proc's own sleep: until woken or ms (< 0: no limit); 1 if woken */
int	kpi_procsleep(struct task_struct *p, long ms);
void	kpi_procwake(struct task_struct *p);

#endif
