/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/file.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FILE_H
#define _KPI_LINUX_FILE_H

#include <linux/fs.h>

int	get_unused_fd_flags(unsigned int flags);
void	put_unused_fd(unsigned int fd);
void	fd_install(unsigned int fd, struct file *f);
struct file	*fget(unsigned int fd);
int	close_fd(unsigned int fd);
/* kpi/linux/file.c: a file of fops for priv (anon_inode_getfile, shmem) */
struct file	*kpi_alloc_file(struct inode *inode, const struct file_operations *fops, void *priv, int flags);
struct fd { struct file *file; unsigned int flags; };
static inline struct fd fdget(unsigned int fd) { struct fd f = { fget(fd), 0 }; return f; }
static inline void fdput(struct fd f) { if (f.file) fput(f.file); }
#define fd_file(f)	((f).file)
#define fd_empty(f)	(!(f).file)

/* 9front: CLASS(fd, f)(fd) (linux/cleanup.h) */
#include <linux/cleanup.h>
DEFINE_CLASS(fd, struct fd, fdput(_T), fdget(fd), int fd)

#endif
