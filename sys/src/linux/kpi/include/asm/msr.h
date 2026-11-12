/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: asm/msr.h (ours, not Linux's): model specific registers through #P/msr */
#ifndef _KPI_ASM_MSR_H
#define _KPI_ASM_MSR_H
#include <linux/types.h>
/* the registers' numbers (Intel SDM, volume 4) */
#define MSR_RAPL_POWER_UNIT	0x00000606
#define MSR_PKG_ENERGY_STATUS	0x00000611
#define MSR_PP0_ENERGY_STATUS	0x00000639
#define MSR_PP1_ENERGY_STATUS	0x00000641
#define MSR_DRAM_ENERGY_STATUS	0x00000619
int	kpi_rdmsr(u32 msr, u64 *v);	/* 0 or -EIO */
#define rdmsrl_safe(msr, p)	kpi_rdmsr(msr, p)
#define rdmsrl(msr, v)		do { u64 __v = 0; kpi_rdmsr(msr, &__v); (v) = __v; } while (0)
/* 9front: Linux 6.16's names (q: 64 bits) */
#define rdmsrq_safe(msr, p)	rdmsrl_safe(msr, p)

#endif
