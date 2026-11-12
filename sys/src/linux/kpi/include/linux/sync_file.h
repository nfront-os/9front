/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/sync_file.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SYNC_FILE_H
#define _KPI_LINUX_SYNC_FILE_H

#include <linux/dma-fence.h>
#include <linux/dma-fence-array.h>
#include <linux/fs.h>

/* sync files: a fence handed to a process (replace/sync_file.c) */
struct sync_file { struct file *file; char user_name[32]; struct dma_fence *fence; };
struct sync_file	*sync_file_create(struct dma_fence *f);
struct dma_fence	*sync_file_get_fence(int fd);
char	*sync_file_get_name(struct sync_file *s, char *buf, int len);

#endif
