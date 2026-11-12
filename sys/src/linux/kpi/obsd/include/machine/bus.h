/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, bus_space(9) and bus_dma(9),
 * as far as OpenBSD's LinuxKPI headers use them.  A bus space handle is
 * the virtual address the file server has the registers at (a BAR
 * segment, kpi/obsd/bus.c); there is no IOMMU: a DMA address is the
 * physical address.
 */
#ifndef _OBSD_MACHINE_BUS_H
#define _OBSD_MACHINE_BUS_H

#include <sys/types.h>

typedef unsigned long	bus_addr_t;
typedef unsigned long	bus_size_t;
typedef int		bus_space_tag_t;	/* X86_BUS_SPACE_MEM or _IO */
typedef unsigned long	bus_space_handle_t;

#define X86_BUS_SPACE_IO	0
#define X86_BUS_SPACE_MEM	1

#define BUS_SPACE_MAP_CACHEABLE		0x0001
#define BUS_SPACE_MAP_LINEAR		0x0002
#define BUS_SPACE_MAP_PREFETCHABLE	0x0008

int	bus_space_map(bus_space_tag_t, bus_addr_t, bus_size_t, int, bus_space_handle_t *);
void	bus_space_unmap(bus_space_tag_t, bus_space_handle_t, bus_size_t);
#define bus_space_vaddr(t, h)	((void *)(h))

#define _OBSD_BUS_RW(n, type)						\
static inline type							\
bus_space_read_##n(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o) \
{									\
	return *(volatile type *)(h + o);				\
}									\
static inline void							\
bus_space_write_##n(bus_space_tag_t t, bus_space_handle_t h, bus_size_t o, type v) \
{									\
	*(volatile type *)(h + o) = v;					\
}
_OBSD_BUS_RW(1, uint8_t)
_OBSD_BUS_RW(2, uint16_t)
_OBSD_BUS_RW(4, uint32_t)
_OBSD_BUS_RW(8, uint64_t)
#undef _OBSD_BUS_RW

/* DMA: physical addresses */
typedef struct bus_dma_tag	*bus_dma_tag_t;

typedef struct bus_dma_segment {
	bus_addr_t	ds_addr;
	bus_size_t	ds_len;
} bus_dma_segment_t;

struct bus_dmamap {
	bus_size_t	dm_mapsize;
	int		dm_nsegs;
	bus_dma_segment_t	dm_segs[1];
};
typedef struct bus_dmamap	*bus_dmamap_t;

#define BUS_DMA_WAITOK		0x0000
#define BUS_DMA_NOWAIT		0x0001
#define BUS_DMA_ALLOCNOW	0x0002
#define BUS_DMA_COHERENT	0x0004
#define BUS_DMA_NOCACHE		0x0800
#define BUS_DMA_FIXED		0x0010
#define BUS_DMA_ZERO		0x1000
#define BUS_DMASYNC_PREREAD	0x01
#define BUS_DMASYNC_POSTREAD	0x02
#define BUS_DMASYNC_PREWRITE	0x04
#define BUS_DMASYNC_POSTWRITE	0x08

#endif
