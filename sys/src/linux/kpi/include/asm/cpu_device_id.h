/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: asm/cpu_device_id.h (ours: Linux's is GPL).  Tables
 * of processors by vendor, family and model (amdgpu's quirks for PCIe
 * dynamic switching), matched against the boot processor (kpi/cpu.c).
 */
#ifndef _KPI_ASM_CPU_DEVICE_ID_H
#define _KPI_ASM_CPU_DEVICE_ID_H

#include <linux/types.h>
#include <asm/processor.h>

#define X86_FAMILY_ANY	0xffff
#define X86_MODEL_ANY	0xffff

struct x86_cpu_id {
	u16	vendor;
	u16	family;
	u16	model;
	u16	flags;	/* the table's end: 0 */
	void	*driver_data;
};

#define X86_MATCH_VENDOR_FAM_MODEL(v, f, m, data)	\
	{ .vendor = X86_VENDOR_##v, .family = (f), .model = (m), .flags = 1, .driver_data = (void *)(data) }
#define X86_MATCH_VENDOR_FAM(v, f, data)	X86_MATCH_VENDOR_FAM_MODEL(v, f, X86_MODEL_ANY, data)

static inline const struct x86_cpu_id *
x86_match_cpu(const struct x86_cpu_id *m)
{
	for (; m->flags; m++)
		if (m->vendor == boot_cpu_data.x86_vendor &&
		    (m->family == X86_FAMILY_ANY || m->family == boot_cpu_data.x86) &&
		    (m->model == X86_MODEL_ANY || m->model == boot_cpu_data.x86_model))
			return m;
	return NULL;
}

#define X86_MATCH_VFM(vfm, data)	\
	{ .vendor = VFM_VENDOR(vfm), .family = VFM_FAMILY(vfm), .model = VFM_MODEL(vfm), .flags = 1, .driver_data = (void *)(data) }

#endif
