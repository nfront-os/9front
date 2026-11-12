/* Public domain. */

#ifndef _LINUX_SMP_H
#define _LINUX_SMP_H

#include <linux/cpumask.h>
#include <linux/llist.h> /* via smp_types.h */

/* sparc64 cpu.h needs time.h and siginfo.h (indirect via param.h) */
#include <sys/param.h>
#include <machine/cpu.h>

#define smp_processor_id()	(curcpu()->ci_cpuid)

#include <linux/percpu.h>	/* 9front: as Linux's smp.h brings it */
#define get_cpu()		cpu_number()
#define put_cpu()

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <asm/processor.h>
#include <linux/mm.h>
#define raw_smp_processor_id()	0
/*
 * 9front: the machine's processors (procs run on all of them: tests that
 * race two kthreads need it); the per-CPU data is one copy, so
 * num_possible_cpus and the CPU loops stay at one
 */
extern int kpi_ncpus;
#define num_online_cpus()	kpi_ncpus
#define num_possible_cpus()	1
#define for_each_online_cpu(c)	for ((c) = 0; (c) < 1; (c)++)
#define for_each_possible_cpu(c)	for ((c) = 0; (c) < 1; (c)++)
#define on_each_cpu(f, i, w)	f(i)
#define wbinvd_on_all_cpus()	kpi_wbinvd()
int	kpi_wbinvd(void);

#endif
