/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/dma-buf.h (ours, not Linux's) */
#ifndef _KPI_LINUX_DMA_BUF_H
#define _KPI_LINUX_DMA_BUF_H

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/dma-direction.h>
#include <linux/scatterlist.h>
#include <linux/iosys-map.h>
#include <linux/fs.h>
#include <linux/dma-fence.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>

/* dma-bufs: buffers shared between devices and processes (replace/dma-buf.c) */
struct dma_buf;
struct dma_buf_attachment;
struct dma_buf_ops {
	bool	cache_sgt_mapping;
	int	(*attach)(struct dma_buf *b, struct dma_buf_attachment *a);
	void	(*detach)(struct dma_buf *b, struct dma_buf_attachment *a);
	int	(*pin)(struct dma_buf_attachment *a);
	void	(*unpin)(struct dma_buf_attachment *a);
	struct sg_table	*(*map_dma_buf)(struct dma_buf_attachment *a, enum dma_data_direction dir);
	void	(*unmap_dma_buf)(struct dma_buf_attachment *a, struct sg_table *t, enum dma_data_direction dir);
	void	(*release)(struct dma_buf *b);
	int	(*begin_cpu_access)(struct dma_buf *b, enum dma_data_direction dir);
	int	(*end_cpu_access)(struct dma_buf *b, enum dma_data_direction dir);
	int	(*mmap)(struct dma_buf *b, struct vm_area_struct *v);
	int	(*vmap)(struct dma_buf *b, struct iosys_map *m);
	void	(*vunmap)(struct dma_buf *b, struct iosys_map *m);
};
struct dma_buf {
	size_t	size;
	struct file	*file;
	struct list_head	attachments;
	const struct dma_buf_ops	*ops;
	unsigned	vmapping_counter;
	struct iosys_map	vmap_ptr;
	const char	*exp_name;
	const char	*name;
	struct module	*owner;
	void	*priv;
	struct dma_resv	*resv;
	struct dma_resv	*kpi_resv;
};
struct dma_buf_attach_ops { bool allow_peer2peer; void (*invalidate_mappings)(struct dma_buf_attachment *a); };	/* 9front: Linux 7's name of move_notify */
struct dma_buf_attachment {
	struct dma_buf	*dmabuf;
	struct device	*dev;
	struct list_head	node;
	struct sg_table	*sgt;
	enum dma_data_direction	dir;
	bool	peer2peer;
	const struct dma_buf_attach_ops	*importer_ops;
	void	*importer_priv;
	void	*priv;
};
struct dma_buf_export_info {
	const char	*exp_name;
	struct module	*owner;
	const struct dma_buf_ops	*ops;
	size_t	size;
	int	flags;
	struct dma_resv	*resv;
	void	*priv;
};
#define DEFINE_DMA_BUF_EXPORT_INFO(n)	struct dma_buf_export_info n = { .exp_name = KBUILD_MODNAME, .owner = THIS_MODULE }
struct dma_buf	*dma_buf_export(const struct dma_buf_export_info *info);
int	dma_buf_fd(struct dma_buf *b, int flags);
struct dma_buf	*dma_buf_get(int fd);
void	dma_buf_put(struct dma_buf *b);
struct dma_buf_attachment	*dma_buf_attach(struct dma_buf *b, struct device *d);
struct dma_buf_attachment	*dma_buf_dynamic_attach(struct dma_buf *b, struct device *d, const struct dma_buf_attach_ops *ops, void *priv);
void	dma_buf_detach(struct dma_buf *b, struct dma_buf_attachment *a);
int	dma_buf_pin(struct dma_buf_attachment *a);
void	dma_buf_unpin(struct dma_buf_attachment *a);
struct sg_table	*dma_buf_map_attachment(struct dma_buf_attachment *a, enum dma_data_direction dir);
void	dma_buf_unmap_attachment(struct dma_buf_attachment *a, struct sg_table *t, enum dma_data_direction dir);
struct sg_table	*dma_buf_map_attachment_unlocked(struct dma_buf_attachment *a, enum dma_data_direction dir);
void	dma_buf_unmap_attachment_unlocked(struct dma_buf_attachment *a, struct sg_table *t, enum dma_data_direction dir);
int	dma_buf_begin_cpu_access(struct dma_buf *b, enum dma_data_direction dir);
int	dma_buf_end_cpu_access(struct dma_buf *b, enum dma_data_direction dir);
int	dma_buf_vmap(struct dma_buf *b, struct iosys_map *m);
void	dma_buf_vunmap(struct dma_buf *b, struct iosys_map *m);
int	dma_buf_vmap_unlocked(struct dma_buf *b, struct iosys_map *m);
void	dma_buf_vunmap_unlocked(struct dma_buf *b, struct iosys_map *m);
int	dma_buf_mmap(struct dma_buf *b, struct vm_area_struct *v, unsigned long pgoff);
void	dma_buf_invalidate_mappings(struct dma_buf *b);
void	dma_buf_fd_install(struct dma_buf *b, int fd);
static inline void get_dma_buf(struct dma_buf *b) { get_file(b->file); }
#define dma_buf_attachment_is_dynamic(a)	((a)->importer_ops != NULL)
#define dma_buf_is_dynamic(b)			((b)->ops->pin != NULL)

#endif
