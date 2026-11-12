/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: asm/hypervisor.h (ours, not Linux's) */
#ifndef _KPI_ASM_HYPERVISOR_H
#define _KPI_ASM_HYPERVISOR_H

#include <linux/kernel.h>
#include <asm/processor.h>

enum x86_hypervisor_type { X86_HYPER_NATIVE = 0, X86_HYPER_VMWARE, X86_HYPER_MS_HYPERV, X86_HYPER_XEN_PV, X86_HYPER_XEN_HVM, X86_HYPER_KVM, X86_HYPER_JAILHOUSE, X86_HYPER_ACRN };
#define hypervisor_is_type(t)		((t) == X86_HYPER_NATIVE)

#endif
