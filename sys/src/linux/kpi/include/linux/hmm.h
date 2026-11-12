/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/hmm.h (ours: Linux's is GPL).  Heterogeneous
 * memory management mirrors a process's pages to a device (amdgpu's
 * userptr objects); 9front's drivers serve other processes and reach no
 * memory of theirs, so the range is there for the structures that embed
 * it (amdgpu_hmm_range, Linux 6.18 on) and faulting it in fails.
 */
#ifndef _KPI_LINUX_HMM_H
#define _KPI_LINUX_HMM_H

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/bits.h>
#include <linux/mm.h>

struct mmu_interval_notifier;

/*
 * an hmm_pfns entry: the frame number below, above it the map order
 * (5 bits from bit BITS_PER_LONG - 8) and, at the top, valid, writable,
 * error; the request bits are the same
 */
#define __HMM_TOP(n)	BIT(BITS_PER_LONG - (n))
enum hmm_pfn_flags {
	HMM_PFN_VALID = __HMM_TOP(1),
	HMM_PFN_WRITE = __HMM_TOP(2),
	HMM_PFN_ERROR = __HMM_TOP(3),
	HMM_PFN_ORDER_SHIFT = BITS_PER_LONG - 8,
	HMM_PFN_FLAGS = GENMASK(BITS_PER_LONG - 1, BITS_PER_LONG - 8),
	HMM_PFN_REQ_FAULT = HMM_PFN_VALID,
	HMM_PFN_REQ_WRITE = HMM_PFN_WRITE,
};

/* what amdgpu's userptr objects hold (filled by hmm_range_fault) */
struct hmm_range {
	unsigned long	start;
	unsigned long	end;
	unsigned long	*hmm_pfns;
	unsigned long	default_flags;
	unsigned long	pfn_flags_mask;
	struct mmu_interval_notifier	*notifier;
	unsigned long	notifier_seq;
	void	*dev_private_owner;
};

static inline unsigned long
__hmm_pfn(unsigned long entry)
{
	return entry & ~(unsigned long)HMM_PFN_FLAGS;
}

static inline struct page *
hmm_pfn_to_page(unsigned long hmm_pfn)
{
	return pfn_to_page(__hmm_pfn(hmm_pfn));
}

static inline phys_addr_t
hmm_pfn_to_phys(unsigned long hmm_pfn)
{
	return (phys_addr_t)__hmm_pfn(hmm_pfn) << PAGE_SHIFT;
}

static inline unsigned int
hmm_pfn_to_map_order(unsigned long hmm_pfn)
{
	return (unsigned int)(hmm_pfn >> HMM_PFN_ORDER_SHIFT) % 32;
}

/* no process memory to mirror */
static inline int
hmm_range_fault(struct hmm_range *range)
{
	return -EFAULT;
}

#endif
