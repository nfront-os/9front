// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: I/O memory and ports.  ioremap finds the BAR of
 * the device (#G) that holds the range: the BAR is a segment attached
 * to the file server, uncached, or write-combining for ioremap_wc.
 * Port I/O goes through #P.
 */
#include <linux/io.h>
#include <linux/io-mapping.h>
#include <linux/ioport.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/pci.h>
#include "kpi.h"

struct resource iomem_resource = { .name = "PCI mem", .start = 0, .end = ~(resource_size_t)0, .flags = IORESOURCE_MEM };

static void __iomem *
barmap(resource_size_t pa, unsigned long size, bool wc)
{
	unsigned long long bpa, bsize;
	int i, io;
	u8 *va;

	for (i = 0; i < PCI_STD_NUM_BARS; i++) {
		if (p9_barinfo(i, &bpa, &bsize, &io) < 0 || io)
			continue;
		if (pa < bpa || pa + size > bpa + bsize)
			continue;
		va = wc ? p9_barwc(i) : NULL;
		if (!va)
			va = p9_bar(i, NULL, NULL);
		if (!va)
			return NULL;
		return (void __iomem *)(va + (pa - bpa));
	}
	pr_warn("ioremap %#llx+%#lx: not in a BAR of the device\n", (unsigned long long)pa, size);
	return NULL;
}

void __iomem *ioremap(resource_size_t pa, unsigned long size) { return barmap(pa, size, false); }
void __iomem *ioremap_wc(resource_size_t pa, unsigned long size) { return barmap(pa, size, true); }
void iounmap(volatile void __iomem *a) { }

/*
 * memory outside the device: what #G offers of it (an Intel GPU's
 * OpRegion with the VBT), else nothing
 */
void *
memremap(phys_addr_t pa, size_t size, int flags)
{
	void *va = p9_memmap(pa, size);

	if (!va)
		pr_warn("memremap %#llx+%#zx: not offered by #G\n", (unsigned long long)pa, size);
	return va;
}

void memunmap(void *a) { }

/* the BARs' segments have their memory type already */
int arch_phys_wc_add(unsigned long base, unsigned long size) { return 0; }
void arch_phys_wc_del(int handle) { }

u8 inb(unsigned long port) { return p9_in(1, port); }
u16 inw(unsigned long port) { return p9_in(2, port); }
u32 inl(unsigned long port) { return p9_in(4, port); }
void outb(u8 v, unsigned long port) { p9_out(1, port, v); }
void outw(u16 v, unsigned long port) { p9_out(2, port, v); }
void outl(u32 v, unsigned long port) { p9_out(4, port, v); }

struct io_mapping *
io_mapping_init_wc(struct io_mapping *m, resource_size_t base, unsigned long size)
{
	m->iomem = ioremap_wc(base, size);
	if (!m->iomem)
		return NULL;
	m->base = base;
	m->size = size;
	m->prot = pgprot_writecombine(PAGE_KERNEL);
	return m;
}

void io_mapping_fini(struct io_mapping *m) { iounmap(m->iomem); }

struct io_mapping *
io_mapping_create_wc(resource_size_t base, unsigned long size)
{
	struct io_mapping *m = kmalloc(sizeof(*m), GFP_KERNEL);

	if (m && !io_mapping_init_wc(m, base, size)) {
		kfree(m);
		m = NULL;
	}
	return m;
}

void
io_mapping_free(struct io_mapping *m)
{
	io_mapping_fini(m);
	kfree(m);
}

/*
 * vmap_pfn: frames of the page pool, or of a BAR, that follow each
 * other where the server sees them
 */
void *
vmap_pfn(unsigned long *pfns, unsigned int count, pgprot_t prot)
{
	struct page *p;
	u8 *va = NULL, *v;
	unsigned int i;

	for (i = 0; i < count; i++) {
		p = pfn_to_page(pfns[i]);
		if (p)
			v = page_address(p);
		else
			v = (u8 *)barmap(PFN_PHYS(pfns[i]), PAGE_SIZE, pgprot_val(prot) == pgprot_val(pgprot_writecombine(PAGE_KERNEL)));
		if (!v)
			return NULL;
		if (i == 0)
			va = v;
		else if (v != va + i * PAGE_SIZE) {
			pr_warn("vmap_pfn: %u frames not contiguous\n", count);
			return NULL;
		}
	}
	return va;
}
