/* Public domain. */

#ifndef _LINUX_AGP_BACKEND_H
#define _LINUX_AGP_BACKEND_H

#include <sys/param.h> /* for sparc64 bus.h */
#include <machine/bus.h>

#if defined(__amd64__) || defined(__i386__)
#define AGP_USER_MEMORY			0
#define AGP_USER_CACHED_MEMORY		BUS_DMA_COHERENT
#endif

struct agp_bridge_data;

/* 9front: what DRM's legacy header (drm_legacy.h) keeps of AGP; there is no AGP here */
struct pci_dev;
struct agp_kern_info {
	struct pci_dev	*device;
	uint32_t	mode;
	unsigned long	aper_base;
	size_t		aper_size;
	int		max_memory;
	int		current_memory;
	bool		cant_use_aperture;
	unsigned long	page_mask;
	const void	*vm_ops;
};

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/list.h>
struct agp_memory { struct agp_memory *next; struct agp_memory *prev; struct agp_bridge_data *bridge; struct page **pages; size_t page_count; int key; int num_scratch_pages; off_t pg_start; u32 type; u32 physical; bool is_bound; bool is_flushed; struct list_head mapped_list; struct scatterlist *sg_list; int num_sg; };

#endif
