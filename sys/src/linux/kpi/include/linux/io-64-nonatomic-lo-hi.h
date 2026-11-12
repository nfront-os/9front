/* Public domain. */

#ifndef _LINUX_IO_64_NONATOMIC_LO_HI_H
#define _LINUX_IO_64_NONATOMIC_LO_HI_H

#include <linux/io.h>

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
static inline u64 lo_hi_readq(const volatile void __iomem *a) { const volatile u32 __iomem *p = a; u32 lo = readl(p), hi = readl(p + 1); return lo + ((u64)hi << 32); }
static inline void lo_hi_writeq(u64 v, volatile void __iomem *a) { volatile u32 __iomem *p = a; writel(v, p); writel(v >> 32, p + 1); }
#define lo_hi_readq_relaxed lo_hi_readq
#define lo_hi_writeq_relaxed lo_hi_writeq

#endif
