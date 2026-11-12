/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: asm/processor.h: boot_cpu_data, the CPU's
 * family and cache line (kpi/obsd/cpu.c, from CPUID); the feature
 * tests are OpenBSD's asm/cpufeature.h.
 */
#ifndef _KPI_ASM_PROCESSOR_H
#define _KPI_ASM_PROCESSOR_H

#include <linux/types.h>
#include <asm/cpufeature.h>

struct cpuinfo_x86 {
	uint8_t	x86;		/* family */
	uint8_t	x86_vendor;
	uint8_t	x86_model;
	uint8_t	x86_stepping;
	int	x86_clflush_size;
	int	x86_cache_alignment;
	uint16_t	x86_max_cores;	/* cores of the package */
};
extern struct cpuinfo_x86	boot_cpu_data;
#define cpu_data(cpu)	boot_cpu_data	/* the CPUs are alike */

#define X86_VENDOR_INTEL	0
#define X86_VENDOR_AMD		2


/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <asm/fpu/api.h>
extern struct cpuinfo_x86 boot_cpu_data;
#define X86_FEATURE_PAE			(0*32 + 6)
#define _PAGE_CACHE_MASK	(_PAGE_PWT | _PAGE_PCD | _PAGE_PAT)
#define clflush(p)		__builtin_ia32_clflush(p)

#endif
