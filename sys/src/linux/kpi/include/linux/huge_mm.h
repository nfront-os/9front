/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/huge_mm.h (ours, not Linux's): no transparent huge pages */
#ifndef _KPI_LINUX_HUGE_MM_H
#define _KPI_LINUX_HUGE_MM_H
#define has_transparent_hugepage()	0
/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define has_transparent_hugepage()	0

#endif
