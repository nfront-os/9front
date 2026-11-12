/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/crc32.h (ours, not Linux's) */
#ifndef _KPI_LINUX_CRC32_H
#define _KPI_LINUX_CRC32_H
#include <linux/types.h>
/* CRC-32 (IEEE 802.3), least significant bit first, as Linux's crc32_le (kpi/linux/lib.c) */
u32	crc32_le(u32 crc, const void *p, size_t len);
#define crc32(seed, p, len)	crc32_le(seed, p, len)
#endif
