/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/mount.h (ours, not Linux's) */
#ifndef _KPI_LINUX_MOUNT_H
#define _KPI_LINUX_MOUNT_H

#include <linux/kernel.h>

struct super_block;
struct vfsmount { struct super_block *mnt_sb; };
#define vfs_kern_mount(t, f, n, d)	((struct vfsmount *)ERR_PTR(-ENODEV))
#define kern_mount(t)			((struct vfsmount *)ERR_PTR(-ENODEV))
#define kern_unmount(m)	do { } while (0)
#include <linux/err.h>

#endif
