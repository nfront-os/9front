/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/relay.h (ours, not Linux's) */
#ifndef _KPI_LINUX_RELAY_H
#define _KPI_LINUX_RELAY_H

#include <linux/kernel.h>
#include <linux/fs.h>

/* no relay channels: the GuC's log is read through the file server instead */
struct rchan_callbacks;
struct rchan { u32 version; size_t subbuf_size; size_t n_subbufs; size_t alloc_size; const struct rchan_callbacks *cb; void *private_data; struct rchan_buf **buf; };
struct rchan_buf { void *start; void *data; size_t offset; size_t subbufs_produced; size_t subbufs_consumed; struct rchan *chan; };
struct rchan_callbacks {
	int	(*subbuf_start)(struct rchan_buf *buf, void *subbuf, void *prev_subbuf);	/* 9front: Linux 6.17 dropped prev_padding */
	struct dentry	*(*create_buf_file)(const char *filename, struct dentry *parent, umode_t mode, struct rchan_buf *buf, int *is_global);
	int	(*remove_buf_file)(struct dentry *dentry);
};
#define relay_open(n, p, s, c, cb, pd)	((struct rchan *)NULL)
#define relay_close(c)			do { } while (0)
#define relay_flush(c)			do { } while (0)
#define relay_reserve(c, l)		((void *)NULL)
#define relay_write(c, d, l)		do { } while (0)
#define relay_buf_full(b)		0
#define relay_file_operations		(*(const struct file_operations *)NULL)

#endif
