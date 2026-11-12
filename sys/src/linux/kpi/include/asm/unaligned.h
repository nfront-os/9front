/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: asm/unaligned.h (ours, not Linux's) */
#ifndef _KPI_ASM_UNALIGNED_H
#define _KPI_ASM_UNALIGNED_H

#include <linux/types.h>
#include <linux/swab.h>

#define __kpi_get_unaligned(t, p)	({ struct { t x; } __attribute__((packed)) *__p = (void *)(p); __p->x; })
#define __kpi_put_unaligned(t, v, p)	do { struct { t x; } __attribute__((packed)) *__p = (void *)(p); __p->x = (v); } while (0)
#define get_unaligned(p)		__kpi_get_unaligned(typeof(*(p)), p)
#define put_unaligned(v, p)		__kpi_put_unaligned(typeof(*(p)), v, p)
#define get_unaligned_le16(p)		__kpi_get_unaligned(u16, p)
#define get_unaligned_le32(p)		__kpi_get_unaligned(u32, p)
#define get_unaligned_le64(p)		__kpi_get_unaligned(u64, p)
#define get_unaligned_be16(p)		swab16(__kpi_get_unaligned(u16, p))
#define get_unaligned_be32(p)		swab32(__kpi_get_unaligned(u32, p))
#define get_unaligned_be64(p)		swab64(__kpi_get_unaligned(u64, p))
#define put_unaligned_le16(v, p)	__kpi_put_unaligned(u16, v, p)
#define put_unaligned_le32(v, p)	__kpi_put_unaligned(u32, v, p)
#define put_unaligned_le64(v, p)	__kpi_put_unaligned(u64, v, p)
#define put_unaligned_be16(v, p)	__kpi_put_unaligned(u16, swab16(v), p)
#define put_unaligned_be32(v, p)	__kpi_put_unaligned(u32, swab32(v), p)
#define put_unaligned_be64(v, p)	__kpi_put_unaligned(u64, swab64(v), p)
static inline u32 get_unaligned_be24(const void *p) { const u8 *b = p; return b[0] << 16 | b[1] << 8 | b[2]; }
static inline u32 get_unaligned_le24(const void *p) { const u8 *b = p; return b[0] | b[1] << 8 | b[2] << 16; }

#endif
