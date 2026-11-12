/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: per-CPU variables.  Each proc running the Linux
 * code is a CPU of its own here (its own curcpu(), its own FPU state),
 * so a per-CPU variable is one per proc: made when the proc first asks
 * for it, a copy of the variable as defined (kpi/linux/percpu.c).
 */
#ifndef _KPI_LINUX_PERCPU_H
#define _KPI_LINUX_PERCPU_H

#include <linux/types.h>

void	*kpi_percpu(void *var, size_t size);

#define DEFINE_PER_CPU(type, name)	__typeof__(type) name
#define DECLARE_PER_CPU(type, name)	extern __typeof__(type) name
#define this_cpu_ptr(p)			((__typeof__(p))kpi_percpu((p), sizeof(*(p))))
#define get_cpu_ptr(p)			this_cpu_ptr(p)
#define put_cpu_ptr(p)			do { (void)(p); } while (0)
#define raw_cpu_ptr(p)			this_cpu_ptr(p)
#define this_cpu_read(v)		(*this_cpu_ptr(&(v)))
#define this_cpu_write(v, x)		(*this_cpu_ptr(&(v)) = (x))
#define get_cpu_var(v)			(*this_cpu_ptr(&(v)))
#define put_cpu_var(v)			do { } while (0)

#define this_cpu_inc_return(v)	(++(*this_cpu_ptr(&(v))))	/* 9front */
#define this_cpu_dec_return(v)	(--(*this_cpu_ptr(&(v))))

#endif
