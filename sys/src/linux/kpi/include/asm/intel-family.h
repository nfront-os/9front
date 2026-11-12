/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: asm/intel-family.h: the Intel CPU models (family
 * 6) the drivers ask for: CPUID's model number (boot_cpu_data.x86_model)
 */
#ifndef _KPI_ASM_INTEL_FAMILY_H
#define _KPI_ASM_INTEL_FAMILY_H

#define INTEL_FAM6_ALDERLAKE	0x97	/* Alder Lake-S */

/* 9front: Linux 6.10's vendor-family-model numbers (VFM) of the models i915 matches */
#define VFM_MAKE(v, f, m)	(((v) << 16) | ((f) << 8) | (m))
#define VFM_VENDOR(vfm)		(((vfm) >> 16) & 0xff)
#define VFM_FAMILY(vfm)		(((vfm) >> 8) & 0xff)
#define VFM_MODEL(vfm)		((vfm) & 0xff)
#define INTEL_KABYLAKE_L	VFM_MAKE(X86_VENDOR_INTEL, 6, 0x8e)
#define INTEL_KABYLAKE		VFM_MAKE(X86_VENDOR_INTEL, 6, 0x9e)
#define INTEL_ALDERLAKE		VFM_MAKE(X86_VENDOR_INTEL, 6, 0x97)
#define INTEL_ALDERLAKE_L	VFM_MAKE(X86_VENDOR_INTEL, 6, 0x9a)
#define INTEL_COMETLAKE		VFM_MAKE(X86_VENDOR_INTEL, 6, 0xa5)
#define INTEL_ROCKETLAKE	VFM_MAKE(X86_VENDOR_INTEL, 6, 0xa7)
#define INTEL_RAPTORLAKE	VFM_MAKE(X86_VENDOR_INTEL, 6, 0xb7)
#define INTEL_RAPTORLAKE_P	VFM_MAKE(X86_VENDOR_INTEL, 6, 0xba)
#define INTEL_RAPTORLAKE_S	VFM_MAKE(X86_VENDOR_INTEL, 6, 0xbf)

#define INTEL_TIGERLAKE_L	VFM_MAKE(X86_VENDOR_INTEL, 6, 0x8c)
#define INTEL_TIGERLAKE		VFM_MAKE(X86_VENDOR_INTEL, 6, 0x8d)

#endif
