// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: files, file descriptors, anonymous inodes and
 * seq_file.  The descriptors are the file server's own (one table):
 * the server hands what a driver makes (dma-buf, sync_file) to its
 * clients as files of its own and keeps the numbers to itself.
 */
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/anon_inodes.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/mount.h>

#define NFD	4096
#define RESERVED	((struct file *)1)

static DEFINE_SPINLOCK(fdlock);
static struct file *fds[NFD];
static struct inode anon_inode = { .i_size = 0 };

int
get_unused_fd_flags(unsigned int flags)
{
	int i;

	spin_lock(&fdlock);
	for (i = 0; i < NFD; i++)
		if (!fds[i]) {
			fds[i] = RESERVED;
			spin_unlock(&fdlock);
			return i;
		}
	spin_unlock(&fdlock);
	return -EMFILE;
}

void
put_unused_fd(unsigned int fd)
{
	spin_lock(&fdlock);
	if (fd < NFD && fds[fd] == RESERVED)
		fds[fd] = NULL;
	spin_unlock(&fdlock);
}

void
fd_install(unsigned int fd, struct file *f)
{
	spin_lock(&fdlock);
	if (fd < NFD && fds[fd] == RESERVED)
		fds[fd] = f;
	spin_unlock(&fdlock);
}

struct file *
fget(unsigned int fd)
{
	struct file *f = NULL;

	spin_lock(&fdlock);
	if (fd < NFD && fds[fd] && fds[fd] != RESERVED)
		f = get_file(fds[fd]);
	spin_unlock(&fdlock);
	return f;
}

/* the server's close of a descriptor */
int
close_fd(unsigned int fd)
{
	struct file *f = NULL;

	spin_lock(&fdlock);
	if (fd < NFD && fds[fd] != RESERVED) {
		f = fds[fd];
		fds[fd] = NULL;
	}
	spin_unlock(&fdlock);
	if (!f)
		return -EBADF;
	fput(f);
	return 0;
}

struct file *
kpi_alloc_file(struct inode *inode, const struct file_operations *fops, void *priv, int flags)
{
	struct file *f = kzalloc(sizeof(*f), GFP_KERNEL);

	if (!f)
		return ERR_PTR(-ENOMEM);
	f->f_op = fops;
	f->private_data = priv;
	f->f_inode = inode;
	f->f_mapping = inode ? inode->i_mapping : NULL;
	f->f_flags = flags & (O_ACCMODE | O_NONBLOCK);
	f->f_mode = ((flags & O_ACCMODE) != O_WRONLY ? FMODE_READ : 0) | ((flags & O_ACCMODE) != O_RDONLY ? FMODE_WRITE : 0);
	atomic_long_set(&f->f_count, 1);
	return f;
}

void
fput(struct file *f)
{
	if (!f || !atomic_long_dec_and_test(&f->f_count))
		return;
	if (f->f_op && f->f_op->release)
		f->f_op->release(f->f_inode, f);
	kfree(f);
}

/* a close of the file (its descriptor): the driver's flush, then the reference goes */
int
filp_close(struct file *f, fl_owner_t id)
{
	int r = 0;

	if (f->f_op && f->f_op->flush)
		r = f->f_op->flush(f, id);
	fput(f);
	return r;
}

struct file *
anon_inode_getfile(const char *name, const struct file_operations *fops, void *priv, int flags)
{
	return kpi_alloc_file(&anon_inode, fops, priv, flags);
}

int
anon_inode_getfd(const char *name, const struct file_operations *fops, void *priv, int flags)
{
	struct file *f;
	int fd;

	fd = get_unused_fd_flags(flags);
	if (fd < 0)
		return fd;
	f = anon_inode_getfile(name, fops, priv, flags);
	if (IS_ERR(f)) {
		put_unused_fd(fd);
		return PTR_ERR(f);
	}
	fd_install(fd, f);
	return fd;
}

/*
 * DRM's pseudo filesystem: nothing is mounted; its inodes carry an
 * address space each (drm_device's anon_inode)
 */
static struct vfsmount pseudo_mnt;

int
kpi_simple_pin_fs(struct vfsmount **m, int *count)
{
	*m = &pseudo_mnt;
	(*count)++;
	return 0;
}

void
kpi_simple_release_fs(struct vfsmount **m, int *count)
{
	if (--*count == 0)
		*m = NULL;
}

struct anon_inode {
	struct inode	inode;
	struct address_space	mapping;
};

struct inode *
alloc_anon_inode(struct super_block *sb)
{
	struct anon_inode *a = kzalloc(sizeof(*a), GFP_KERNEL);

	if (!a)
		return ERR_PTR(-ENOMEM);
	a->inode.i_mapping = &a->mapping;
	a->mapping.host = &a->inode;
	return &a->inode;
}

/* the last reference to an inode of alloc_anon_inode (DRM's only use) */
void
iput(struct inode *i)
{
	struct anon_inode *a;

	if (!i)
		return;
	a = container_of(i, struct anon_inode, inode);
	if (i->i_mapping == &a->mapping)
		kfree(a);
}

/*
 * seq_file: single_open's show writes the whole text into a buffer,
 * twice as big each time it does not fit
 */
struct single {
	struct seq_file	m;
	int	(*show)(struct seq_file *m, void *v);
	bool	done;
};

void
seq_vprintf(struct seq_file *m, const char *fmt, va_list ap)
{
	int n;

	if (m->count < m->size) {
		n = vsnprintf(m->buf + m->count, m->size - m->count, fmt, ap);
		if (m->count + n < m->size) {
			m->count += n;
			return;
		}
	}
	m->count = m->size;
}

void
seq_printf(struct seq_file *m, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	seq_vprintf(m, fmt, ap);
	va_end(ap);
}

void
seq_write(struct seq_file *m, const void *data, size_t len)
{
	if (m->count + len < m->size) {
		memcpy(m->buf + m->count, data, len);
		m->count += len;
	} else
		m->count = m->size;
}

void seq_puts(struct seq_file *m, const char *s) { seq_write(m, s, strlen(s)); }
void seq_putc(struct seq_file *m, char c) { seq_write(m, &c, 1); }

int
single_open_size(struct file *f, int (*show)(struct seq_file *m, void *v), void *data, size_t size)
{
	struct single *s = kzalloc(sizeof(*s), GFP_KERNEL);

	if (!s)
		return -ENOMEM;
	s->show = show;
	s->m.private = data;
	s->m.size = max_t(size_t, size, PAGE_SIZE);
	f->private_data = &s->m;
	return 0;
}

int single_open(struct file *f, int (*show)(struct seq_file *m, void *v), void *data) { return single_open_size(f, show, data, PAGE_SIZE); }

int
single_release(struct inode *i, struct file *f)
{
	struct single *s = container_of((struct seq_file *)f->private_data, struct single, m);

	kvfree(s->m.buf);
	kfree(s);
	return 0;
}

static int
fill(struct single *s)
{
	int r;

	for (;;) {
		if (!s->m.buf) {
			s->m.buf = kvmalloc(s->m.size, GFP_KERNEL);
			if (!s->m.buf)
				return -ENOMEM;
		}
		s->m.count = 0;
		r = s->show(&s->m, NULL);
		if (r < 0)
			return r;
		if (!seq_has_overflowed(&s->m))
			break;
		kvfree(s->m.buf);
		s->m.buf = NULL;
		s->m.size *= 2;
	}
	s->done = true;
	return 0;
}

ssize_t
seq_read(struct file *f, char __user *b, size_t n, loff_t *off)
{
	struct single *s = container_of((struct seq_file *)f->private_data, struct single, m);
	int r;

	if (!s->done && (r = fill(s)) < 0)
		return r;
	if (*off >= (loff_t)s->m.count)
		return 0;
	n = min_t(size_t, n, s->m.count - *off);
	memcpy((void *)b, s->m.buf + *off, n);
	*off += n;
	return n;
}

loff_t
seq_lseek(struct file *f, loff_t o, int whence)
{
	if (whence != 0 || o < 0)
		return -EINVAL;
	f->f_pos = o;
	return o;
}
