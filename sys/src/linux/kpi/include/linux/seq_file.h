/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/seq_file.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SEQ_FILE_H
#define _KPI_LINUX_SEQ_FILE_H

#include <linux/kernel.h>
struct file;
struct inode;

struct seq_file { char *buf; size_t size; size_t count; void *private; };
void	seq_printf(struct seq_file *m, const char *fmt, ...) __printf(2, 3);
void	seq_vprintf(struct seq_file *m, const char *fmt, va_list args);
void	seq_puts(struct seq_file *m, const char *s);
void	seq_putc(struct seq_file *m, char c);
void	seq_write(struct seq_file *m, const void *data, size_t len);
static inline bool seq_has_overflowed(struct seq_file *m) { return m->count == m->size; }
int	single_open(struct file *f, int (*show)(struct seq_file *m, void *v), void *data);
int	single_open_size(struct file *f, int (*show)(struct seq_file *m, void *v), void *data, size_t size);
int	single_release(struct inode *i, struct file *f);
ssize_t	seq_read(struct file *f, char __user *b, size_t n, loff_t *o);
loff_t	seq_lseek(struct file *f, loff_t o, int w);
#define seq_puts_safe(m, s)	seq_puts(m, s)
#define DEFINE_SHOW_ATTRIBUTE(n)	static const struct file_operations n##_fops __maybe_unused = { .owner = THIS_MODULE }
/* debugfs is off: the attributes are file operations nobody opens */
#define DEFINE_SIMPLE_ATTRIBUTE(n, g, s, f)	static const struct file_operations n __maybe_unused = { .owner = THIS_MODULE }
#define DEFINE_DEBUGFS_ATTRIBUTE(n, g, s, f)	DEFINE_SIMPLE_ATTRIBUTE(n, g, s, f)
#include <linux/fs.h>
#include <linux/module.h>

#define DEFINE_SHOW_STORE_ATTRIBUTE(n)	DEFINE_SHOW_ATTRIBUTE(n)	/* 9front: no debugfs */

#endif
