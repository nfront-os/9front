/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/cache.h (ours, not Linux's) */
#ifndef _KPI_LINUX_CACHE_H
#define _KPI_LINUX_CACHE_H

#include <linux/types.h>


#define L1_CACHE_BYTES	64
#define SMP_CACHE_BYTES	64
#define cache_line_size()	64

#endif
