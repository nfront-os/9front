/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/shmem_fs.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SHMEM_FS_H
#define _KPI_LINUX_SHMEM_FS_H

#include <linux/fs.h>
#include <linux/mm.h>

struct vfsmount;

/* shmem files: the backing of GEM objects, a #g segment each (kpi/shmem.c) */
struct file	*shmem_file_setup(const char *name, loff_t size, vma_flags_t flags);	/* 9front: Linux 7's vma_flags_t */
struct file	*shmem_file_setup_with_mnt(struct vfsmount *mnt, const char *name, loff_t size, vma_flags_t flags);
struct page	*shmem_read_mapping_page_gfp(struct address_space *m, pgoff_t index, gfp_t gfp);
struct folio	*shmem_read_folio_gfp(struct address_space *m, pgoff_t index, gfp_t gfp);
#define shmem_read_mapping_page(m, i)	shmem_read_mapping_page_gfp(m, i, GFP_KERNEL)
#define shmem_truncate_range(i, s, e)	do { } while (0)

#endif
