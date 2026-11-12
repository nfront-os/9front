/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/pseudo_fs.h (ours, not Linux's) */
#ifndef _KPI_LINUX_PSEUDO_FS_H
#define _KPI_LINUX_PSEUDO_FS_H

#include <linux/kernel.h>
#include <linux/fs.h>

struct pseudo_fs_context { const void *ops; const void *xattr; const void *dops; unsigned long magic; };
#define init_pseudo(fc, m)		((struct pseudo_fs_context *)NULL)



#endif
