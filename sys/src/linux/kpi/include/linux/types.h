/* Public domain. */

#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <sys/types.h>
#include <sys/stdint.h>
/*
 * sparc64 bus.h needs _null.h (indirect via param.h)
 * sparc64 busop.h needs machine/ctlreg.h (indirect via param.h)
 */
#include <sys/param.h>
#include <machine/bus.h>

typedef int8_t   __s8;
typedef uint8_t  __u8;
typedef int16_t  __s16;
typedef uint16_t __u16;
typedef int32_t  __s32;
typedef uint32_t __u32;
typedef int64_t  __s64;
typedef uint64_t __u64;

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;

typedef uint16_t __le16; 
typedef uint16_t __be16; 
typedef uint32_t __le32; 
typedef uint32_t __be32;
typedef uint64_t __le64; 
typedef uint64_t __be64; 

/* 9front: the uapi types the uapi headers (vendor/include/uapi) use */
typedef unsigned long	__kernel_size_t;
typedef long		__kernel_ssize_t;
typedef long		__kernel_long_t;
typedef unsigned long	__kernel_ulong_t;
typedef long long	__kernel_loff_t;
typedef int		__kernel_pid_t;
typedef unsigned int	__kernel_uid32_t;
typedef unsigned int	__kernel_gid32_t;
typedef long long	__kernel_time64_t;
typedef unsigned int	__poll_t;
typedef uint16_t	__sum16;
typedef uint32_t	__wsum;
typedef uint64_t	__aligned_u64 __attribute__((__aligned__(8)));
typedef int64_t		__aligned_s64 __attribute__((__aligned__(8)));

typedef uint64_t dma_addr_t;
typedef paddr_t phys_addr_t;
typedef paddr_t resource_size_t;

typedef off_t loff_t;

typedef __ptrdiff_t ptrdiff_t;

typedef unsigned int umode_t;
typedef unsigned int gfp_t;
typedef unsigned int fmode_t;
typedef unsigned int vm_fault_t;	/* 9front: Linux's */	/* 9front: Linux's file modes (FMODE_) */

typedef unsigned long pgoff_t;
typedef int pgprot_t;

/* 9front: Linux's atomic types, structs with a counter (linux/atomic.h) */
typedef struct { volatile int counter; } atomic_t;
typedef struct { volatile int64_t counter; } __attribute__((__aligned__(8))) atomic64_t;
typedef struct { volatile long counter; } atomic_long_t;

typedef int64_t ktime_t;

struct list_head {
	struct list_head *next, *prev;
};

/* 9front: Linux's member name pprev (the Linux code is unchanged) */
struct hlist_node {
	struct hlist_node *next, **pprev;
};

struct hlist_head {
	struct hlist_node *first;
};

#define DECLARE_BITMAP(x, y)	unsigned long x[BITS_TO_LONGS(y)];

#define LINUX_PAGE_MASK		PAGE_MASK	/* 9front: PAGE_MASK is Linux's already */

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/compiler_types.h>
#include <linux/stddef.h>
typedef s64			time64_t;
typedef long			clock_t;
typedef struct { u64 pte; } pte_t;
typedef unsigned int		slab_flags_t;
typedef u64			sector_t;
typedef u64			blkcnt_t;
typedef unsigned long		irq_hw_number_t;

#endif /* _LINUX_TYPES_H_ */
