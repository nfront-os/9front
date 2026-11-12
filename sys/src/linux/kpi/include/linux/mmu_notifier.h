/* Public domain. */

#ifndef _LINUX_MMU_NOTIFIER_H
#define _LINUX_MMU_NOTIFIER_H

struct mmu_interval_notifier {
};

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/mm_types.h>
#include <linux/errno.h>
struct mmu_notifier { int unused; };
struct mmu_notifier_range { unsigned long start, end; int event; struct mm_struct *mm; };
struct mmu_interval_notifier_ops { bool (*invalidate)(struct mmu_interval_notifier *n, const struct mmu_notifier_range *r, unsigned long cur_seq); };
#define mmu_notifier_range_blockable(r)		1
#define mmu_interval_notifier_insert(n, mm, s, l, o)	(-ENODEV)
#define mmu_interval_notifier_remove(n)		do { } while (0)
#define mmu_interval_read_begin(n)		0UL
#define mmu_interval_read_retry(n, s)		0
#define mmu_interval_set_seq(n, s)		do { } while (0)
#define mmu_notifier_synchronize()		do { } while (0)	/* none is ever inserted */

#endif
