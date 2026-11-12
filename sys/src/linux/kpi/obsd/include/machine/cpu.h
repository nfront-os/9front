/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, machine/cpu.h: struct
 * cpu_info, one per proc here (curcpu() is the proc's: its "atomic"
 * depth belongs to the proc, not to a processor); the CPUID bits are
 * the machine's (kpi/obsd/cpu.c reads them once).
 */
#ifndef _OBSD_MACHINE_CPU_H
#define _OBSD_MACHINE_CPU_H

#include <sys/types.h>
#include <machine/specialreg.h>

struct schedstate_percpu {
	int	spc_schedflags;
};
#define SPCF_SHOULDYIELD	0x0002

struct cpu_info {
	unsigned int	ci_cpuid;	/* the processor the proc last ran on, for smp_processor_id */
	int	ci_inatomic;
	int	ci_idepth;
	struct schedstate_percpu ci_schedstate;
	uint32_t	ci_feature_flags;	/* CPUID 1 edx */
	uint32_t	ci_feature_eflags;
	uint32_t	ci_feature_sefflags_ebx;	/* CPUID 7 ebx */
	uint32_t	ci_feature_sefflags_ecx;
	uint32_t	ci_feature_sefflags_edx;
	unsigned int	ci_cflushsz;	/* clflush line size, 0: none */
	struct kpi_percpu	*ci_percpu;	/* the proc's per-CPU variables (kpi/linux/percpu.c) */
};

struct cpu_info	*kpi_curcpu(void);
#define curcpu()	(kpi_curcpu())
#define cpu_number()	(curcpu()->ci_cpuid)
#define CPU_IS_PRIMARY(ci)	((ci)->ci_cpuid == 0)
#define CPU_INFO_ITERATOR	int
#define CPU_INFO_FOREACH(cii, ci)	for ((cii) = 0, (ci) = curcpu(); (cii) == 0; (cii)++)

extern uint32_t	cpu_ecxfeature;		/* CPUID 1 ecx */
extern uint32_t	cpu_feature;		/* CPUID 1 edx */
extern uint32_t	cpu_sefflags_ebx;	/* CPUID 7 ebx */
void	kpi_cpu_init(void);
extern int	cpu_vendor_amd;

/* a file server has no interrupts to turn off */
static inline unsigned long intr_disable(void) { return 0; }
static inline void intr_restore(unsigned long s) { (void)s; }
static inline void intr_enable(void) { }

#define CPU_BUSY_CYCLE()	__asm volatile("pause" ::: "memory")

#endif
