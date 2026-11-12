/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: dma-bufs and sync files on our files
 * (kpi/linux/file.c).  Laid out as OpenBSD's (drm_linux.c): a dma-buf
 * is an allocation with a file whose last fput releases it through the
 * exporter; dma_buf_get takes the file of a descriptor, dma_buf_fd
 * gives one.  The attachment calls go to the exporter's operations
 * under the buffer's reservation lock, the _unlocked ones take it.
 */
#include <linux/dma-buf.h>
#include <linux/dma-resv.h>
#include <linux/sync_file.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/fs.h>

/* the file of a dma-buf */
static int
dmabuf_release(struct inode *inode, struct file *fp)
{
	struct dma_buf *db = fp->private_data;
	struct dma_resv *own = (struct dma_resv *)(db + 1);

	db->ops->release(db);
	/* the reservation export made after the struct, if it did */
	if (db->resv == own)
		dma_resv_fini(own);
	kfree(db);
	return 0;
}

static const struct file_operations dmabuf_fops = {
	.release = dmabuf_release,
};

struct dma_buf *
dma_buf_export(const struct dma_buf_export_info *info)
{
	struct dma_resv *resv = info->resv;
	struct dma_buf *db;
	struct file *fp;

	/* without a reservation of the exporter's, one of its own after the struct */
	db = kzalloc(sizeof(*db) + (resv ? 0 : sizeof(struct dma_resv)), GFP_KERNEL);
	if (db == NULL)
		return ERR_PTR(-ENOMEM);
	if (resv == NULL) {
		resv = (struct dma_resv *)&db[1];
		dma_resv_init(resv);
	}
	db->priv = info->priv;
	db->ops = info->ops;
	db->size = info->size;
	db->exp_name = info->exp_name;
	db->owner = info->owner;
	db->resv = resv;
	INIT_LIST_HEAD(&db->attachments);

	fp = kpi_alloc_file(NULL, &dmabuf_fops, db, O_RDWR);
	if (IS_ERR(fp)) {
		if (resv == (struct dma_resv *)&db[1])
			dma_resv_fini(resv);
		kfree(db);
		return ERR_CAST(fp);
	}
	db->file = fp;
	return db;
}

struct dma_buf *
dma_buf_get(int fd)
{
	struct file *fp;

	fp = fget(fd);
	if (fp == NULL)
		return ERR_PTR(-EBADF);
	if (fp->f_op != &dmabuf_fops) {
		fput(fp);
		return ERR_PTR(-EINVAL);
	}
	return fp->private_data;
}

void
dma_buf_put(struct dma_buf *db)
{
	KASSERT(db);
	KASSERT(db->file);
	fput(db->file);
}

/* the buffer's file at fd, which the caller reserved (Linux 7: drm_prime) */
void
dma_buf_fd_install(struct dma_buf *db, int fd)
{
	fd_install(fd, db->file);
}

int
dma_buf_fd(struct dma_buf *db, int flags)
{
	int fd;

	if (db == NULL || db->file == NULL)
		return -EINVAL;
	fd = get_unused_fd_flags(flags);
	if (fd < 0)
		return fd;
	fd_install(fd, db->file);
	return fd;
}

struct dma_buf_attachment *
dma_buf_dynamic_attach(struct dma_buf *db, struct device *dev,
    const struct dma_buf_attach_ops *iops, void *ipriv)
{
	struct dma_buf_attachment *dba;
	int ret;

	if (db == NULL || dev == NULL)
		return ERR_PTR(-EINVAL);
	dba = kzalloc(sizeof(*dba), GFP_KERNEL);
	if (dba == NULL)
		return ERR_PTR(-ENOMEM);
	dba->dev = dev;
	dba->dmabuf = db;
	dba->importer_ops = iops;
	dba->importer_priv = ipriv;
	if (iops != NULL)
		dba->peer2peer = iops->allow_peer2peer;
	if (db->ops->attach != NULL) {
		ret = db->ops->attach(db, dba);
		if (ret) {
			kfree(dba);
			return ERR_PTR(ret);
		}
	}
	dma_resv_lock(db->resv, NULL);
	list_add(&dba->node, &db->attachments);
	dma_resv_unlock(db->resv);
	return dba;
}

struct dma_buf_attachment *
dma_buf_attach(struct dma_buf *db, struct device *dev)
{
	return dma_buf_dynamic_attach(db, dev, NULL, NULL);
}

void
dma_buf_detach(struct dma_buf *db, struct dma_buf_attachment *dba)
{
	if (dba == NULL)
		return;
	dma_resv_lock(db->resv, NULL);
	if (dba->sgt != NULL) {
		db->ops->unmap_dma_buf(dba, dba->sgt, dba->dir);
		if (dma_buf_is_dynamic(db))
			db->ops->unpin(dba);
	}
	list_del(&dba->node);
	dma_resv_unlock(db->resv);
	if (db->ops->detach != NULL)
		db->ops->detach(db, dba);
	kfree(dba);
}

int
dma_buf_pin(struct dma_buf_attachment *dba)
{
	struct dma_buf *db = dba->dmabuf;

	dma_resv_assert_held(db->resv);
	return db->ops->pin != NULL ? db->ops->pin(dba) : 0;
}

void
dma_buf_unpin(struct dma_buf_attachment *dba)
{
	struct dma_buf *db = dba->dmabuf;

	dma_resv_assert_held(db->resv);
	if (db->ops->unpin != NULL)
		db->ops->unpin(dba);
}

/* under the reservation lock; an exporter that caches keeps the first table */
struct sg_table *
dma_buf_map_attachment(struct dma_buf_attachment *dba, enum dma_data_direction dir)
{
	struct dma_buf *db = dba->dmabuf;
	struct sg_table *sgt;
	int ret;

	dma_resv_assert_held(db->resv);
	if (dba->sgt != NULL)
		return dba->dir == dir || dba->dir == DMA_BIDIRECTIONAL ?
		    dba->sgt : ERR_PTR(-EBUSY);
	if (dma_buf_is_dynamic(db) && !dma_buf_attachment_is_dynamic(dba)) {
		ret = db->ops->pin(dba);
		if (ret)
			return ERR_PTR(ret);
	}
	sgt = db->ops->map_dma_buf(dba, dir);
	if (sgt == NULL)
		sgt = ERR_PTR(-ENOMEM);
	if (IS_ERR(sgt)) {
		if (dma_buf_is_dynamic(db) && !dma_buf_attachment_is_dynamic(dba))
			db->ops->unpin(dba);
		return sgt;
	}
	if (db->ops->cache_sgt_mapping) {
		dba->sgt = sgt;
		dba->dir = dir;
	}
	return sgt;
}

void
dma_buf_unmap_attachment(struct dma_buf_attachment *dba, struct sg_table *sgt,
    enum dma_data_direction dir)
{
	struct dma_buf *db = dba->dmabuf;

	dma_resv_assert_held(db->resv);
	if (dba->sgt == sgt)
		return;		/* cached: until the detach */
	db->ops->unmap_dma_buf(dba, sgt, dir);
	if (dma_buf_is_dynamic(db) && !dma_buf_attachment_is_dynamic(dba))
		db->ops->unpin(dba);
}

struct sg_table *
dma_buf_map_attachment_unlocked(struct dma_buf_attachment *dba, enum dma_data_direction dir)
{
	struct sg_table *sgt;

	dma_resv_lock(dba->dmabuf->resv, NULL);
	sgt = dma_buf_map_attachment(dba, dir);
	dma_resv_unlock(dba->dmabuf->resv);
	return sgt;
}

void
dma_buf_unmap_attachment_unlocked(struct dma_buf_attachment *dba, struct sg_table *sgt,
    enum dma_data_direction dir)
{
	dma_resv_lock(dba->dmabuf->resv, NULL);
	dma_buf_unmap_attachment(dba, sgt, dir);
	dma_resv_unlock(dba->dmabuf->resv);
}

/* the exporter moved the buffer: tell the dynamic importers (Linux 7's name of move_notify) */
void
dma_buf_invalidate_mappings(struct dma_buf *db)
{
	struct dma_buf_attachment *dba;

	dma_resv_assert_held(db->resv);
	list_for_each_entry(dba, &db->attachments, node)
		if (dba->importer_ops != NULL)
			dba->importer_ops->invalidate_mappings(dba);
}

int
dma_buf_begin_cpu_access(struct dma_buf *db, enum dma_data_direction dir)
{
	return db->ops->begin_cpu_access != NULL ?
	    db->ops->begin_cpu_access(db, dir) : 0;
}

int
dma_buf_end_cpu_access(struct dma_buf *db, enum dma_data_direction dir)
{
	return db->ops->end_cpu_access != NULL ?
	    db->ops->end_cpu_access(db, dir) : 0;
}

int
dma_buf_mmap(struct dma_buf *db, struct vm_area_struct *vma, unsigned long pgoff)
{
	if (db->ops->mmap == NULL)
		return -EINVAL;
	vma->vm_pgoff = pgoff;
	return db->ops->mmap(db, vma);
}

/* one mapping for all who ask, counted, under the reservation lock */
int
dma_buf_vmap(struct dma_buf *db, struct iosys_map *map)
{
	struct iosys_map ptr;
	int ret;

	iosys_map_clear(map);
	if (db->ops->vmap == NULL)
		return -EINVAL;
	dma_resv_assert_held(db->resv);
	if (db->vmapping_counter) {
		db->vmapping_counter++;
		*map = db->vmap_ptr;
		return 0;
	}
	ret = db->ops->vmap(db, &ptr);
	if (ret)
		return ret;
	db->vmap_ptr = ptr;
	db->vmapping_counter = 1;
	*map = ptr;
	return 0;
}

void
dma_buf_vunmap(struct dma_buf *db, struct iosys_map *map)
{
	dma_resv_assert_held(db->resv);
	if (db->vmapping_counter == 0)
		return;
	if (--db->vmapping_counter == 0) {
		if (db->ops->vunmap != NULL)
			db->ops->vunmap(db, map);
		iosys_map_clear(&db->vmap_ptr);
	}
}

int
dma_buf_vmap_unlocked(struct dma_buf *db, struct iosys_map *map)
{
	int ret;

	dma_resv_lock(db->resv, NULL);
	ret = dma_buf_vmap(db, map);
	dma_resv_unlock(db->resv);
	return ret;
}

void
dma_buf_vunmap_unlocked(struct dma_buf *db, struct iosys_map *map)
{
	dma_resv_lock(db->resv, NULL);
	dma_buf_vunmap(db, map);
	dma_resv_unlock(db->resv);
}

/* sync files: a fence in a file, as OpenBSD's */
static int
syncfile_release(struct inode *inode, struct file *fp)
{
	struct sync_file *sf = fp->private_data;

	dma_fence_put(sf->fence);
	kfree(sf);
	return 0;
}

static const struct file_operations syncfile_fops = {
	.release = syncfile_release,
};

struct sync_file *
sync_file_create(struct dma_fence *fence)
{
	struct sync_file *sf;
	struct file *fp;

	sf = kzalloc(sizeof(*sf), GFP_KERNEL);
	if (sf == NULL)
		return NULL;
	fp = kpi_alloc_file(NULL, &syncfile_fops, sf, O_RDWR);
	if (IS_ERR(fp)) {
		kfree(sf);
		return NULL;
	}
	sf->file = fp;
	sf->fence = dma_fence_get(fence);
	return sf;
}

struct dma_fence *
sync_file_get_fence(int fd)
{
	struct sync_file *sf;
	struct dma_fence *f;
	struct file *fp;

	fp = fget(fd);
	if (fp == NULL)
		return NULL;
	if (fp->f_op != &syncfile_fops) {
		fput(fp);
		return NULL;
	}
	sf = fp->private_data;
	f = dma_fence_get(sf->fence);
	fput(fp);
	return f;
}

/* a sync file's name when it has none of its own: the fence's, as Linux gives it */
static const char syncfile_namefmt[] = "%s-%s%llu-%lld";

char *
sync_file_get_name(struct sync_file *sf, char *buf, int len)
{
	if (sf->user_name[0])
		strscpy(buf, sf->user_name, len);
	else
		snprintf(buf, len, syncfile_namefmt,
		    dma_fence_driver_name(sf->fence),
		    dma_fence_timeline_name(sf->fence),
		    (unsigned long long)sf->fence->context,
		    (long long)sf->fence->seqno);
	return buf;
}
