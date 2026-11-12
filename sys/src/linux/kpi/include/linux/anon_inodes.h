/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/anon_inodes.h (ours, not Linux's) */
#ifndef _KPI_LINUX_ANON_INODES_H
#define _KPI_LINUX_ANON_INODES_H

#include <linux/fs.h>

struct file	*anon_inode_getfile(const char *name, const struct file_operations *fops, void *priv, int flags);
int	anon_inode_getfd(const char *name, const struct file_operations *fops, void *priv, int flags);

#endif
