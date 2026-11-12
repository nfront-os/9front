/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pfn_t.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PFN_T_H
#define _KPI_LINUX_PFN_T_H

#include <linux/types.h>

typedef struct { u64 val; } pfn_t;
#define PFN_DEV		(1ULL << 61)
#define PFN_MAP		(1ULL << 60)
#define __pfn_to_pfn_t(p, f)	((pfn_t){ (p) | (f) })
#define pfn_t_to_pfn(p)	((unsigned long)((p).val & ~(PFN_DEV | PFN_MAP)))

#endif
