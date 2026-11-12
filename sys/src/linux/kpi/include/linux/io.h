/* Public domain. */

#ifndef _LINUX_IO_H
#define _LINUX_IO_H

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/memrange.h> /* for MDF_WRITECOMBINE */

#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/compiler.h>
#include <linux/vmalloc.h> /* via asm/io.h */
#include <linux/sizes.h>

#define memcpy_toio(d, s, n)	memcpy(d, s, n)
#define memcpy_fromio(d, s, n)	memcpy(d, s, n)
#define memset_io(d, b, n)	memset(d, b, n)

#ifdef __powerpc__
#define iobarrier()		mb()
#else
#define iobarrier()		barrier()
#endif

static inline u8
ioread8(const volatile void __iomem *addr)
{
	uint8_t val;

	iobarrier();
	val = *(volatile uint8_t *)addr;
	rmb();
	return val;
}

static inline void
iowrite8(u8 val, volatile void __iomem *addr)
{
	wmb();
	*(volatile uint8_t *)addr = val;
}

#ifdef __sparc64__

/*
 * On sparc64, bus_space_vaddr(9) returns a virtual address that has
 * been mapped little-endian, so we should just use the CPU byte
 * order.
 */

static inline u16
ioread16(const volatile void __iomem *addr)
{
	uint16_t val;

	iobarrier();
	val = *(volatile uint16_t *)addr;
	rmb();
	return val;
}

static inline u32
ioread32(const volatile void __iomem *addr)
{
	uint32_t val;

	iobarrier();
	val = *(volatile uint32_t *)addr;
	rmb();
	return val;
}

static inline u64
ioread64(const volatile void __iomem *addr)
{
	uint64_t val;

	iobarrier();
	val = *(volatile uint64_t *)addr;
	rmb();
	return val;
}

static inline void
iowrite16(u16 val, volatile void __iomem *addr)
{
	wmb();
	*(volatile uint16_t *)addr = val;
}

static inline void
iowrite32(u32 val, volatile void __iomem *addr)
{
	wmb();
	*(volatile uint32_t *)addr = val;
}

static inline void
iowrite64(u64 val, volatile void __iomem *addr)
{
	wmb();
	*(volatile uint64_t *)addr = val;
}

#else

static inline u16
ioread16(const volatile void __iomem *addr)
{
	uint16_t val;

	iobarrier();
	val = lemtoh16(addr);
	rmb();
	return val;
}

static inline u32
ioread32(const volatile void __iomem *addr)
{
	uint32_t val;
	
	iobarrier();
	val = lemtoh32(addr);
	rmb();
	return val;
}

static inline u64
ioread64(const volatile void __iomem *addr)
{
	uint64_t val;

	iobarrier();
	val = lemtoh64(addr);
	rmb();
	return val;
}

static inline void
iowrite16(u16 val, volatile void __iomem *addr)
{
	wmb();
	htolem16(addr, val);
}

static inline void
iowrite32(u32 val, volatile void __iomem *addr)
{
	wmb();
	htolem32(addr, val);
}

static inline void
iowrite64(u64 val, volatile void __iomem *addr)
{
	wmb();
	htolem64(addr, val);
}

#endif

#define readb(p) ioread8(p)
#define writeb(v, p) iowrite8(v, p)
#define readw(p) ioread16(p)
#define writew(v, p) iowrite16(v, p)
#define readl(p) ioread32(p)
#define writel(v, p) iowrite32(v, p)
#define readq(p) ioread64(p)
#define writeq(v, p) iowrite64(v, p)

#define readl_relaxed(p) readl(p)
#define writel_relaxed(v, p) writel(v, p)

int	drm_mtrr_add(unsigned long, size_t, int);
int	drm_mtrr_del(int, unsigned long, size_t, int);

#define DRM_MTRR_WC	MDF_WRITECOMBINE

static inline void *
IOMEM_ERR_PTR(long error)
{
	return (void *) error;
}

#define MEMREMAP_WB	(1 << 0)

void	*memremap(phys_addr_t, size_t, int);
void	memunmap(void *);

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/page.h>
#include <linux/err.h>
#define __kpi_rd(t, a)		(*(const volatile t *)(a))
#define __kpi_wr(t, v, a)	(*(volatile t *)(a) = (v))
#define readb_relaxed readb
#define readw_relaxed readw
#define readq_relaxed readq
#define writeb_relaxed writeb
#define writew_relaxed writew
#define writeq_relaxed writeq
#define __raw_readl readl
#define __raw_writel writel
#define __raw_readq readq
#define __raw_writeq writeq
void __iomem	*ioremap(resource_size_t pa, unsigned long size);
void __iomem	*ioremap_wc(resource_size_t pa, unsigned long size);
void	iounmap(volatile void __iomem *a);
u8	inb(unsigned long port);
u16	inw(unsigned long port);
u32	inl(unsigned long port);
void	outb(u8 v, unsigned long port);
void	outw(u16 v, unsigned long port);
void	outl(u32 v, unsigned long port);
#define inb_p inb
#define outb_p outb
#define ioremap_uc(p, s)		ioremap(p, s)
#define ioremap_cache(p, s)		ioremap(p, s)
#define ioremap_np(p, s)		ioremap(p, s)
#define devm_ioremap(d, p, s)		ioremap(p, s)
#define devm_ioremap_wc(d, p, s)	ioremap_wc(p, s)
#define MEMREMAP_WT		2
#define MEMREMAP_WC		4
int	arch_phys_wc_add(unsigned long base, unsigned long size);
void	arch_phys_wc_del(int handle);
#define arch_io_reserve_memtype_wc(b, s)	0
#define arch_io_free_memtype_wc(b, s)		do { } while (0)
#define phys_to_virt(p)			((void *)0)
#define virt_to_bus(p)			virt_to_phys(p)

#endif
