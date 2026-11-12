/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, machine/param.h (amd64).
 * PAGE_MASK is Linux's ~(PAGE_SIZE-1), not OpenBSD's PAGE_SIZE-1: the
 * Linux code is unchanged here (OpenBSD changes it to LINUX_PAGE_MASK).
 */
#ifndef _OBSD_MACHINE_PARAM_H
#define _OBSD_MACHINE_PARAM_H

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE - 1))
#define PGSHIFT		PAGE_SHIFT
#define NBPG		PAGE_SIZE
#define PGOFSET		(PAGE_SIZE - 1)
#define ALIGNBYTES	15
#define CACHELINESIZE	64

#define atop(x)		((unsigned long)(x) >> PAGE_SHIFT)
#define ptoa(x)		((paddr_t)(x) << PAGE_SHIFT)
#define trunc_page(x)	((unsigned long)(x) & ~PGOFSET)
#define round_page(x)	(((unsigned long)(x) + PGOFSET) & ~PGOFSET)

/* as OpenBSD's for the kernel */
#include <machine/cpu.h>

#endif
