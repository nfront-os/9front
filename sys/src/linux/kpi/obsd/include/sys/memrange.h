/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/memrange.h: MTRRs (none: write-combining comes with the BAR segment) */
#ifndef _OBSD_SYS_MEMRANGE_H
#define _OBSD_SYS_MEMRANGE_H
#define MDF_UNCACHEABLE		(1 << 0)
#define MDF_WRITECOMBINE	(1 << 1)
#define MDF_WRITETHROUGH	(1 << 2)
#define MDF_WRITEBACK		(1 << 3)
#endif
