/* Public domain. */

#ifndef _LINUX_IOMMU_H
#define _LINUX_IOMMU_H

struct bus_type;
struct sg_table;
struct device;	/* 9front: before the prototypes */

struct iommu_domain {
	bus_dma_tag_t dmat;
	unsigned int type;	/* 9front: Linux's IOMMU_DOMAIN_* */
};
#define IOMMU_DOMAIN_IDENTITY	(1U << 0)	/* 9front: DMA addresses are physical */

#define IOMMU_READ	0x0001
#define IOMMU_WRITE	0x0002

ssize_t	iommu_map_sgtable(struct iommu_domain *, u_long,
	    struct sg_table *, int);
size_t	iommu_unmap(struct iommu_domain *, u_long, size_t);


/* 9front: the driver sees no IOMMU (the device's addresses are physical, linux/dma-mapping.h) */
static inline struct iommu_domain *iommu_get_domain_for_dev(struct device *d) { return NULL; }
phys_addr_t iommu_iova_to_phys(struct iommu_domain *, dma_addr_t);

struct iommu_domain *iommu_domain_alloc(struct bus_type *);
#define iommu_domain_free(a)
int	iommu_attach_device(struct iommu_domain *, struct device *);
#define iommu_detach_device(a, b)

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
struct dev_iommu { void *priv; };

#endif
