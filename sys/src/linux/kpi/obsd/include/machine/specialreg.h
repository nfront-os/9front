/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, machine/specialreg.h: the CPUID bits used (Intel's SDM names) */
#ifndef _OBSD_MACHINE_SPECIALREG_H
#define _OBSD_MACHINE_SPECIALREG_H
#define CPUID_CFLUSH	0x00080000	/* 1 edx 19 */
#define CPUID_PAT	0x00010000	/* 1 edx 16 */
#define CPUID_SSE2	0x04000000	/* 1 edx 26 */
#define CPUIDECX_SSE41	0x00080000	/* 1 ecx 19 */
#define CPUIDECX_HV	0x80000000	/* 1 ecx 31: under a hypervisor */
#define SEFF0EBX_CLFLUSHOPT	0x00800000	/* 7 ebx 23 */
#endif
