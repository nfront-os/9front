/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/swab.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SWAB_H
#define _KPI_LINUX_SWAB_H

#include <linux/types.h>

#define swab16(x)	__builtin_bswap16(x)
#define swab32(x)	__builtin_bswap32(x)
#define swab64(x)	__builtin_bswap64(x)
#define __swab16(x)	swab16(x)
#define __swab32(x)	swab32(x)
#define __swab64(x)	swab64(x)

#endif
