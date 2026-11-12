/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/debugfs.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DEBUGFS_H
#define _KPI_LINUX_DEBUGFS_H

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/fs.h>

/* 9front: debugfs is off: no dentry is ever made (debugfs_create_* fail) */
struct dentry { struct inode *d_inode; };
static inline struct inode *d_inode(const struct dentry *d) { return d->d_inode; }

/* no debugfs (CONFIG_DEBUG_FS off): the file server's files instead */
struct dentry;
struct debugfs_reg32 { char *name; unsigned long offset; };
struct debugfs_regset32 { const struct debugfs_reg32 *regs; int nregs; void __iomem *base; struct device *dev; };
#define debugfs_create_dir(n, p)		((struct dentry *)ERR_PTR(-ENODEV))
#define debugfs_create_file(n, m, p, d, f)	((struct dentry *)ERR_PTR(-ENODEV))
#define debugfs_create_file_unsafe(n, m, p, d, f)	((struct dentry *)ERR_PTR(-ENODEV))
#define debugfs_create_bool(n, m, p, v)		do { } while (0)
#define debugfs_create_u32(n, m, p, v)		do { } while (0)
#define debugfs_create_u64(n, m, p, v)		do { } while (0)
#define debugfs_create_x32(n, m, p, v)		do { } while (0)
#define debugfs_create_atomic_t(n, m, p, v)	do { } while (0)
#define debugfs_create_ulong(n, m, p, v)	do { } while (0)
#define debugfs_create_regset32(n, m, p, r)	do { } while (0)
#define debugfs_remove(d)			do { } while (0)
#define debugfs_remove_recursive(d)		do { } while (0)
#define debugfs_lookup(n, p)			((struct dentry *)NULL)
#define debugfs_lookup_and_remove(n, p)		do { } while (0)
#define debugfs_initialized()			0

struct debugfs_blob_wrapper { void *data; unsigned long size; };

#endif
