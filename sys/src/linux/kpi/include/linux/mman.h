/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/mman.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MMAN_H
#define _KPI_LINUX_MMAN_H

#include <linux/types.h>

#define PROT_NONE	0x0
#define PROT_READ	0x1
#define PROT_WRITE	0x2
#define PROT_EXEC	0x4
#define MAP_SHARED	0x01
#define MAP_PRIVATE	0x02

#endif
