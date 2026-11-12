/* Public domain. */

#ifndef _LINUX_MM_H
#define _LINUX_MM_H

#include <sys/types.h>
#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/stdint.h>
#include <sys/atomic.h>
#include <machine/cpu.h>
#include <uvm/uvm_extern.h>
#include <uvm/uvm_glue.h>
#include <lib/libkern/libkern.h> /* for flsl */
#include <linux/shrinker.h>
#include <linux/overflow.h>
#include <linux/pgtable.h>
#include <linux/sysctl.h>	/* 9front: as our earlier mm.h */
#include <linux/huge_mm.h>
#include <linux/mm_types.h>

#define PageHighMem(x)	0

#define page_to_phys(page)	(VM_PAGE_TO_PHYS(page))
#define page_to_pfn(pp)		(VM_PAGE_TO_PHYS(pp) / PAGE_SIZE)
#define pfn_to_page(pfn)	(PHYS_TO_VM_PAGE(ptoa(pfn)))
/*
 * 9front: the struct pages are arrays of 8 MB chunks, not a memmap of all
 * memory: the n'th page after page (by frame) is page + n only inside its
 * chunk, as Linux's nth_page without SPARSEMEM_VMEMMAP (pfn_to_page of
 * the frame number + n): i915 joins frames that follow each other into
 * one scatterlist entry, also across chunks (page.c)
 */
#define nth_page(page, n)	kpi_nth_page((page), (n))
/* 9front: PAGE_MASK is Linux's ~(PAGE_SIZE - 1) here (machine/param.h) */
#define offset_in_page(off)	((vaddr_t)(off) & (PAGE_SIZE - 1))
#define set_page_dirty(page)	atomic_clearbits_int(&page->pg_flags, PG_CLEAN)

#define PAGE_ALIGN(addr)	(((addr) + PAGE_SIZE - 1) & PAGE_MASK)

#define PFN_UP(x)		(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)		((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)		((x) << PAGE_SHIFT)

bool is_vmalloc_addr(const void *);

static inline void *
kvmalloc(size_t size, gfp_t flags)
{
	return kpi_malloc(size, M_DRM, flags);
}

static inline void *
kvmalloc_array(size_t n, size_t size, int flags)
{
	if (n != 0 && SIZE_MAX / n < size)
		return NULL;
	return kpi_malloc(n * size, M_DRM, flags);
}

static inline struct page *
vmalloc_to_page(const void *va)
{
	return uvm_atopg((vaddr_t)va);
}

static inline struct page *
virt_to_page(const void *va)
{
	return uvm_atopg((vaddr_t)va);
}

/* 9front: where the file server sees a page (its segment) */
static inline void *
page_address(const struct page *pg)
{
	return pg->virtual;
}
#define page_to_virt(p)		page_address(p)

static inline void *
kvcalloc(size_t n, size_t size, int flags)
{
	return kvmalloc_array(n, size, flags | M_ZERO);
}

static inline void *
kvzalloc(size_t size, int flags)
{
	return kpi_malloc(size, M_DRM, flags | M_ZERO);
}

static inline void
kvfree(const void *objp)
{
	kpi_free((void *)objp, M_DRM, 0);
}

static inline long
si_mem_available(void)
{
	return uvmexp.free;
}

static inline unsigned int
get_order(size_t size)
{
	return flsl((size - 1) >> PAGE_SHIFT);
}

static inline long
totalram_pages(void)
{
	return uvmexp.npages;
}

static inline bool
want_init_on_free(void)
{
	return false;
}

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <asm/processor.h>
#include <linux/mm_types.h>
#include <asm/page.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/err.h>
#include <linux/pfn.h>
#include <linux/mmzone.h>
#include <linux/rbtree.h>
#include <linux/pagevec.h>
#include <linux/uaccess.h>
#include <linux/mman.h>
#include <linux/highmem.h>
#define VM_READ			0x00000001
#define VM_WRITE		0x00000002
#define VM_EXEC			0x00000004
#define VM_ACCESS_FLAGS		(VM_READ | VM_WRITE | VM_EXEC)
#define VM_SHARED		0x00000008
#define VM_MAYREAD		0x00000010
#define VM_MAYWRITE		0x00000020
#define VM_MAYSHARE		0x00000080
#define VM_PFNMAP		0x00000400
#define VM_IO			0x00004000
#define VM_DONTCOPY		0x00020000
#define VM_DONTEXPAND		0x00040000
#define VM_DONTDUMP		0x04000000
#define VM_NORESERVE		0x00200000
#define VM_MIXEDMAP		0x10000000
#define VM_FAULT_HWPOISON	0x0010
#define VM_FAULT_ERROR		(VM_FAULT_OOM | VM_FAULT_SIGBUS | VM_FAULT_SIGSEGV | VM_FAULT_HWPOISON)
#define VM_FAULT_SIGSEGV	0x0040
#define FAULT_FLAG_WRITE	0x01
#define FAULT_FLAG_ALLOW_RETRY	0x04
#define FAULT_FLAG_RETRY_NOWAIT	0x08
#define FAULT_FLAG_INTERRUPTIBLE	0x200
#define FOLL_WRITE		0x01
#define FOLL_LONGTERM		0x100
#define vm_flags_set(v, f)	((v)->vm_flags |= (f))
#define vm_flags_clear(v, f)	((v)->vm_flags &= ~(f))
#define vm_flags_mod(v, s, c)	((v)->vm_flags = ((v)->vm_flags | (s)) & ~(c))
void	unmap_mapping_range(struct address_space *m, loff_t start, loff_t len, int even_cows);	/* kpi_unmap_range */
struct sysinfo { unsigned long totalram; unsigned long freeram; unsigned long totalhigh; unsigned long freehigh; unsigned int mem_unit; };
void	si_meminfo(struct sysinfo *i);
#define is_cow_mapping(f)	0
#define fault_flag_allow_retry_first(f)	0
#define zap_vma_ptes(v, a, s)		do { } while (0)
#define call_mmap(f, v)			((f)->f_op->mmap(f, v))
struct vm_area_struct	*find_vma(struct mm_struct *mm, unsigned long addr);
#define vma_lookup(mm, addr)		find_vma(mm, addr)
int	vm_munmap(unsigned long start, size_t len);
int	access_process_vm(struct task_struct *t, unsigned long addr, void *buf, int len, unsigned int flags);
static inline pte_t ptep_get(pte_t *p) { return *p; }
#define pte_present(p)		0
#define pte_none(p)		1
#define vma_set_file(v, f)		do { (v)->vm_file = (f); } while (0)
struct file;
unsigned long	vm_mmap(struct file *f, unsigned long addr, unsigned long len, unsigned long prot, unsigned long flag, unsigned long off);
#define flush_cache_range(v, s, e)	do { } while (0)
struct vma_iterator { unsigned long addr; struct mm_struct *mm; };
#define VMA_ITERATOR(n, m, a)		struct vma_iterator n = { (a), (m) }
#define for_each_vma_range(i, v, e)	for ((v) = NULL; (v); )
#define for_each_vma(i, v)		for ((v) = NULL; (v); )
struct writeback_control { long nr_to_write; long pages_skipped; loff_t range_start; loff_t range_end; int sync_mode; unsigned int for_reclaim:1; };
#define WB_SYNC_NONE			0
#define vma_pages(v)		(((v)->vm_end - (v)->vm_start) >> PAGE_SHIFT)
#define mmap_read_lock(m)	do { } while (0)
#define mmap_read_unlock(m)	do { } while (0)
#define mmap_write_lock(m)	do { } while (0)
#define mmap_write_unlock(m)	do { } while (0)
#define mmap_write_lock_killable(m)	0
#define mmap_read_lock_killable(m)	0
#define untagged_addr(a)	(a)
#define kvmalloc_array_noprof	kvmalloc_array
vm_fault_t	vmf_insert_pfn(struct vm_area_struct *v, unsigned long addr, unsigned long pfn);
vm_fault_t	vmf_insert_pfn_prot(struct vm_area_struct *v, unsigned long addr, unsigned long pfn, pgprot_t p);
vm_fault_t	vmf_insert_mixed(struct vm_area_struct *v, unsigned long addr, unsigned long pfn);
int	remap_pfn_range(struct vm_area_struct *v, unsigned long addr, unsigned long pfn, unsigned long size, pgprot_t p);
int	apply_to_page_range(struct mm_struct *mm, unsigned long addr, unsigned long size, int (*fn)(pte_t *pte, unsigned long addr, void *data), void *data);
int	pin_user_pages_fast(unsigned long start, int nr, unsigned int flags, struct page **pages);
void	unpin_user_pages(struct page **pages, unsigned long n);
void	unpin_user_pages_dirty_lock(struct page **pages, unsigned long n, bool dirty);
#define pte_mkspecial(p)	(p)
#define pfn_pte(n, p)		((pte_t){ (n) })
#define set_pte_at(m, a, p, v)	do { } while (0)

#define get_nr_swap_pages()		((long)0)	/* 9front: no swap */
#define current_is_kswapd()		0

/* 9front: Linux 7's mm helpers, for what 9front maps (no process pages) */
#define vma_get_page_prot(f)	vm_get_page_prot((f).__vma_flags)
#define vma_is_cow_mapping(f)	is_cow_mapping((f).__vma_flags)
#define zap_special_vma_range(v, a, s)	zap_vma_ptes(v, a, s)
#define linear_page_index(v, a)	((pgoff_t)(((a) - (v)->vm_start) >> PAGE_SHIFT) + (v)->vm_pgoff)
#define vfs_mmap(f, v)	((f)->f_op->mmap ? (f)->f_op->mmap((f), (v)) : -ENODEV)

#define vma_end_pgoff(v)	((v)->vm_pgoff + (((v)->vm_end - (v)->vm_start) >> PAGE_SHIFT))
/* no page cache to write back (shmem here is memory) */
#define writeback_iter(mapping, wbc, folio, error)	((struct folio *)NULL)
/* no memory statistics per node */
#define mod_lruvec_page_state(p, idx, n)	do { } while (0)
#define NR_GPU_ACTIVE	0
#define NR_GPU_RECLAIM	0
#define folio_next_index(f)	((f)->page.index + folio_nr_pages(f))

/* no process memory here: no mm to take, nothing mapped into a process */
#define get_task_mm(t)	((struct mm_struct *)NULL)
#define mm_get_unmapped_area(f, a, l, p, fl)	(-ENOMEM)
#define folio_mapped(f)	false
#define folio_lock(f)	do { } while (0)
#define folio_unlock(f)	do { } while (0)
void	split_page(struct page *p, unsigned int order);

#define folio_clear_dirty_for_io(f)	false	/* 9front: shmem is memory, never dirty for io */
#define folio_redirty_for_writepage(wbc, f)	false

#define folio_set_reclaim(f)	do { } while (0)
#define shmem_write_folio(f)	0	/* 9front: nothing to write back */
#define mmput(mm)	do { } while (0)
#define mmput_async(mm)	do { } while (0)

#define folio_test_writeback(f)	false

#define folio_clear_reclaim(f)	do { } while (0)

#endif
