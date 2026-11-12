/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/fs.h (ours, not Linux's) */
#ifndef _KPI_LINUX_FS_H
#define _KPI_LINUX_FS_H

#include <linux/kernel.h>
#include <linux/mm_types.h>
#include <linux/atomic.h>
#include <linux/seq_file.h>
#include <linux/kdev_t.h>
#include <linux/stat.h>
#include <linux/err.h>

struct poll_table_struct;
struct fs_context;
struct super_block { int unused; };
struct file_system_type { const char *name; int (*init_fs_context)(struct fs_context *fc); void (*kill_sb)(struct super_block *sb); struct module *owner; int fs_flags; };
#define unregister_chrdev(m, n)		do { } while (0)
#define register_chrdev(m, n, f)	0	/* the file server is the device */
#define get_fs_type(n)		((struct file_system_type *)NULL)
#define put_filesystem(t)	do { } while (0)
#define kill_anon_super		NULL
/* DRM's pseudo filesystem: inodes for their address spaces (kpi/linux/file.c) */
struct vfsmount;
struct super_block;
int	kpi_simple_pin_fs(struct vfsmount **m, int *count);
void	kpi_simple_release_fs(struct vfsmount **m, int *count);
#define simple_pin_fs(t, m, c)	kpi_simple_pin_fs(m, c)
#define simple_release_fs(m, c)	kpi_simple_release_fs(m, c)
struct inode	*alloc_anon_inode(struct super_block *sb);
void	iput(struct inode *i);
struct inode { struct address_space *i_mapping; void *i_private; loff_t i_size; };
struct file;
typedef void *fl_owner_t;
struct kiocb;
struct iov_iter;
struct file_operations { struct module *owner; unsigned int fop_flags; int (*flush)(struct file *f, fl_owner_t id); long (*unlocked_ioctl)(struct file *f, unsigned int cmd, unsigned long arg);
	long (*compat_ioctl)(struct file *f, unsigned int cmd, unsigned long arg); int (*mmap)(struct file *f, struct vm_area_struct *v);
	int (*open)(struct inode *i, struct file *f); int (*release)(struct inode *i, struct file *f);
	ssize_t (*read)(struct file *f, char __user *b, size_t n, loff_t *o); ssize_t (*write)(struct file *f, const char __user *b, size_t n, loff_t *o);
	ssize_t (*write_iter)(struct kiocb *k, struct iov_iter *from);	/* 9front: none here (i915's shmem pwrite falls back) */
	unsigned int (*poll)(struct file *f, struct poll_table_struct *w); loff_t (*llseek)(struct file *f, loff_t o, int w);
	void (*show_fdinfo)(struct seq_file *m, struct file *f); unsigned long (*get_unmapped_area)(struct file *f, unsigned long a, unsigned long l, unsigned long p, unsigned long fl); };
struct file {
	void	*private_data;
	const struct file_operations	*f_op;
	struct inode	*f_inode;
	struct address_space	*f_mapping;
	fmode_t	f_mode;
	unsigned int	f_flags;
	loff_t	f_pos;
	atomic_long_t	f_count;
	void	*kpi;	/* kpi/file.c: the object it stands for */
};
struct poll_table_struct;
struct writeback_control;
struct address_space_operations { int (*writepage)(struct page *p, struct writeback_control *w); int (*dirty_folio)(struct address_space *m, struct folio *f);
	int (*write_begin)(struct file *f, struct address_space *m, loff_t pos, unsigned len, struct page **pagep, void **fsdata);
	int (*write_end)(struct file *f, struct address_space *m, loff_t pos, unsigned len, unsigned copied, struct page *page, void *fsdata); };
struct address_space { struct inode *host; const struct address_space_operations *a_ops; void *private_data; };
#define FMODE_READ		0x1
#define FMODE_WRITE		0x2
#define FMODE_UNSIGNED_OFFSET	0x2000
#define MAX_LFS_FILESIZE	((loff_t)LLONG_MAX)
#define O_ACCMODE		03
#define O_RDONLY		00
#define O_WRONLY		01
#define O_RDWR			02
#define O_CLOEXEC		02000000
#define O_NONBLOCK		04000
#define O_EXCL			0200
#define O_CREAT			0100
#define O_TRUNC			01000
#define mapping_gfp_mask(m)			GFP_HIGHUSER
#define clear_page_dirty_for_io(p)	0
#define find_lock_page(m, i)		((struct page *)NULL)
#define mapping_gfp_constraint(m, g)		(mapping_gfp_mask(m) & (g))
#define mapping_set_gfp_mask(m, g)		do { } while (0)
#define check_move_unevictable_folios(fb)	do { } while (0)
#define mapping_set_unevictable(m)	do { } while (0)
#define mapping_clear_unevictable(m)	do { } while (0)
#define iminor(i)			0
#define imajor(i)			0
#define fops_get(f)			(f)
#define fops_put(f)			do { } while (0)
#define replace_fops(f, fops)	do { (f)->f_op = (fops); } while (0)
#define file_inode(f)		((f)->f_inode)
#define get_file_rcu(f)		atomic_long_inc_not_zero(&(f)->f_count)
#define get_file(f)		({ atomic_long_inc(&(f)->f_count); (f); })
void	fput(struct file *f);
int	filp_close(struct file *f, fl_owner_t id);	/* flush, then fput */
#define noop_llseek		NULL
#define no_llseek		NULL
#define default_llseek		NULL
#define nonseekable_open(i, f)	0
#define stream_open(i, f)	0
#define simple_read_from_buffer(t, c, p, f, a)	((ssize_t)0)
#include <linux/file.h>

/* 9front: file_operations' fop_flags (Linux 6.11) */
#define FOP_BUFFER_RASYNC	(1U << 0)
#define FOP_BUFFER_WASYNC	(1U << 1)
#define FOP_MMAP_SYNC		(1U << 2)
#define FOP_DIO_PARALLEL_WRITE	(1U << 3)
#define FOP_HUGE_PAGES		(1U << 4)
#define FOP_UNSIGNED_OFFSET	(1U << 5)

/* 9front: a reference to *pf's file if it is still alive (Linux 6.8) */
#define get_file_active(pf)	({ struct file *__f = READ_ONCE(*(pf)); (__f && get_file_rcu(__f)) ? __f : NULL; })

/* 9front: an I/O control block and a user buffer, for write_iter (which no file here has) */
struct kiocb { struct file *ki_filp; loff_t ki_pos; };
#define init_sync_kiocb(k, f)	do { (k)->ki_filp = (f); (k)->ki_pos = 0; } while (0)
enum { ITER_DEST = 0, ITER_SOURCE = 1 };
struct iov_iter { unsigned int data_source; void __user *ubuf; size_t count; };
#define iov_iter_ubuf(i, dir, buf, n)	do { (i)->data_source = (dir); (i)->ubuf = (buf); (i)->count = (n); } while (0)

#define MAX_RW_COUNT	(INT_MAX & ~(PAGE_SIZE - 1))	/* Linux's largest single read or write */

#define AOP_WRITEPAGE_ACTIVATE	0x80000	/* a writepage's: keep the page active */
#define force_o_largefile()	true	/* 64 bits: files are large */

#ifndef O_LARGEFILE
#define O_LARGEFILE	0	/* 9front: every file is large */
#endif

ssize_t	kernel_write(struct file *f, const void *buf, size_t count, loff_t *pos);	/* 9front: shmem files only */
ssize_t	kernel_read(struct file *f, void *buf, size_t count, loff_t *pos);

#endif
