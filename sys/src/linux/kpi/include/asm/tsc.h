/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: asm/tsc.h (ours, not Linux's) */
#ifndef _KPI_ASM_TSC_H
#define _KPI_ASM_TSC_H

#include <linux/types.h>

static inline u64 rdtsc(void) { return __builtin_ia32_rdtsc(); }
#define rdtsc_ordered()	rdtsc()
extern unsigned int tsc_khz;

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
extern unsigned int tsc_khz;

#endif
