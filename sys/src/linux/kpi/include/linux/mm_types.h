/* Public domain. */

#ifndef _LINUX_MM_TYPES_H
#define _LINUX_MM_TYPES_H

#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/rwsem.h>
#include <linux/seqlock.h>

#include <uvm/uvm_extern.h>


#define VM_FAULT_NOPAGE		1
#define VM_FAULT_SIGBUS		2
#define VM_FAULT_RETRY		3
#define VM_FAULT_OOM		4

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/args.h>
#include <linux/list.h>
#include <linux/atomic.h>
typedef unsigned int vm_fault_t;
struct folio { struct page page; };
struct mm_struct { int unused; };
struct vm_area_struct {
	unsigned long	vm_start, vm_end;
	unsigned long	vm_flags;
	pgprot_t	vm_page_prot;
	unsigned long	vm_pgoff;
	struct file	*vm_file;
	void	*vm_private_data;
	const struct vm_operations_struct	*vm_ops;
	struct mm_struct	*vm_mm;
};
struct vm_fault { struct vm_area_struct *vma; unsigned long address; pgoff_t pgoff; unsigned int flags; struct page *page; };
struct vm_operations_struct {
	void	(*open)(struct vm_area_struct *a);
	void	(*close)(struct vm_area_struct *a);
	vm_fault_t	(*fault)(struct vm_fault *f);
	int	(*access)(struct vm_area_struct *a, unsigned long addr, void *buf, int len, int write);
};
#define __pgprot(v)		((pgprot_t){ (v) })
#define vm_get_page_prot(f)	__pgprot(0)

/*
 * 9front: Linux 7's vma_flags_t, a bitmap of the VMA_*_BIT; one word
 * here (shmem_file_setup takes it and has no use for it)
 */
typedef struct { unsigned long __vma_flags; } vma_flags_t;
#define EMPTY_VMA_FLAGS		((vma_flags_t){ 0 })
enum { VMA_NORESERVE_BIT = 21 };	/* VM_NORESERVE's */
#define __kpi_vma_or1(a)	(1UL << (a))
#define __kpi_vma_or2(a, b)	(__kpi_vma_or1(a) | (1UL << (b)))
#define __kpi_vma_or3(a, b, c)	(__kpi_vma_or2(a, b) | (1UL << (c)))
#define mk_vma_flags(...)	((vma_flags_t){ CONCATENATE(__kpi_vma_or, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__) })

#endif
