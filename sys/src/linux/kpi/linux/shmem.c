// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: shmem files, the memory behind GEM objects.  A
 * file's pages come from the page pool when first read and stay until
 * the file goes; nothing is swapped out.
 */
#include <linux/shmem_fs.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/xarray.h>
#include <linux/slab.h>
#include <linux/mm.h>

struct shmem {
	struct inode	inode;
	struct address_space	mapping;
	struct xarray	pages;
};

static int
shmem_release(struct inode *inode, struct file *f)
{
	struct shmem *s = container_of(inode, struct shmem, inode);
	unsigned long i;
	struct page *p;

	xa_for_each(&s->pages, i, p)
		put_page(p);
	xa_destroy(&s->pages);
	kfree(s);
	return 0;
}

static const struct file_operations shmem_fops = {
	.release = shmem_release,
};

struct file *
shmem_file_setup(const char *name, loff_t size, vma_flags_t flags)
{
	struct shmem *s;
	struct file *f;

	if (size < 0)
		return ERR_PTR(-EINVAL);
	s = kzalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return ERR_PTR(-ENOMEM);
	xa_init(&s->pages);
	s->inode.i_mapping = &s->mapping;
	s->inode.i_size = size;
	s->mapping.host = &s->inode;
	f = kpi_alloc_file(&s->inode, &shmem_fops, NULL, O_RDWR);
	if (IS_ERR(f))
		kfree(s);
	return f;
}

struct file *
shmem_file_setup_with_mnt(struct vfsmount *mnt, const char *name, loff_t size, vma_flags_t flags)
{
	return shmem_file_setup(name, size, flags);
}

/* the page at index, made (zeroed) if need be, with a reference for the caller */
struct page *
shmem_read_mapping_page_gfp(struct address_space *m, pgoff_t index, gfp_t gfp)
{
	struct shmem *s = container_of(m, struct shmem, mapping);
	struct page *p, *old;

	if ((loff_t)index << PAGE_SHIFT >= s->inode.i_size)
		return ERR_PTR(-EINVAL);
	p = xa_load(&s->pages, index);
	if (!p) {
		p = alloc_page(gfp | __GFP_ZERO);
		if (!p)
			return ERR_PTR(-ENOMEM);
		p->mapping = m;
		p->index = index;
		old = xa_cmpxchg(&s->pages, index, NULL, p, GFP_KERNEL);
		if (xa_is_err(old)) {
			put_page(p);
			return ERR_PTR(xa_err(old));
		}
		if (old) {
			put_page(p);
			p = old;
		}
	}
	get_page(p);
	return p;
}

struct folio *
shmem_read_folio_gfp(struct address_space *m, pgoff_t index, gfp_t gfp)
{
	struct page *p = shmem_read_mapping_page_gfp(m, index, gfp);

	if (IS_ERR(p))
		return ERR_CAST(p);
	return page_folio(p);
}

/*
 * reading and writing a shmem file's contents from the kernel (i915's
 * objects made from data): page by page, the pages made as reading
 * them makes them; other files are not ours to write
 */
static ssize_t
shmem_rw(struct file *f, void *buf, size_t count, loff_t *pos, int write)
{
	struct address_space *m;
	size_t done = 0, n, off;
	struct page *p;

	if (f->f_op != &shmem_fops)
		return -EINVAL;
	m = f->f_mapping ? f->f_mapping : f->f_inode->i_mapping;
	while (done < count) {
		if (*pos >= f->f_inode->i_size)
			break;
		off = *pos & (PAGE_SIZE - 1);
		n = min(count - done, (size_t)(PAGE_SIZE - off));
		n = min(n, (size_t)(f->f_inode->i_size - *pos));
		p = shmem_read_mapping_page_gfp(m, *pos >> PAGE_SHIFT, GFP_KERNEL);
		if (IS_ERR(p))
			return done ? (ssize_t)done : PTR_ERR(p);
		if (write)
			memcpy((char *)page_address(p) + off, (char *)buf + done, n);
		else
			memcpy((char *)buf + done, (char *)page_address(p) + off, n);
		put_page(p);
		done += n;
		*pos += n;
	}
	return done;
}

ssize_t
kernel_write(struct file *f, const void *buf, size_t count, loff_t *pos)
{
	return shmem_rw(f, (void *)buf, count, pos, 1);
}

ssize_t
kernel_read(struct file *f, void *buf, size_t count, loff_t *pos)
{
	return shmem_rw(f, buf, count, pos, 0);
}
