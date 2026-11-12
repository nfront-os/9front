// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: mappings into a process.  A client of the file
 * server maps an object by attaching its segment (gpu(4)), not through
 * the driver's mmap: these calls of the driver's own mmap paths fail.
 */
#include <linux/mm.h>

struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr) { return NULL; }

unsigned long
vm_mmap(struct file *f, unsigned long addr, unsigned long len, unsigned long prot, unsigned long flag, unsigned long off)
{
	return -ENODEV;
}

vm_fault_t vmf_insert_pfn_prot(struct vm_area_struct *v, unsigned long addr, unsigned long pfn, pgprot_t p) { return VM_FAULT_SIGBUS; }
vm_fault_t vmf_insert_pfn(struct vm_area_struct *v, unsigned long addr, unsigned long pfn) { return VM_FAULT_SIGBUS; }
vm_fault_t vmf_insert_mixed(struct vm_area_struct *v, unsigned long addr, unsigned long pfn) { return VM_FAULT_SIGBUS; }
int remap_pfn_range(struct vm_area_struct *v, unsigned long addr, unsigned long pfn, unsigned long size, pgprot_t p) { return -ENODEV; }

int
apply_to_page_range(struct mm_struct *mm, unsigned long addr, unsigned long size,
	int (*fn)(pte_t *pte, unsigned long addr, void *data), void *data)
{
	return -ENODEV;
}

/* userptr objects: memory of the server itself is not pinned for the device */
int pin_user_pages_fast(unsigned long start, int nr, unsigned int flags, struct page **pages) { return -EFAULT; }
void unpin_user_pages(struct page **pages, unsigned long n) { }
void unpin_user_pages_dirty_lock(struct page **pages, unsigned long n, bool dirty) { }

/* "nomodeset" on the kernel's command line: not on 9front */
bool video_firmware_drivers_only(void) { return false; }

/* nothing the driver mapped into a process (vm_mmap refuses): nothing to unmap */
int vm_munmap(unsigned long start, size_t len) { return -EINVAL; }

/* no process memory reached from the driver: nothing read or written */
int
access_process_vm(struct task_struct *t, unsigned long addr, void *buf, int len, unsigned int flags)
{
	return 0;
}
