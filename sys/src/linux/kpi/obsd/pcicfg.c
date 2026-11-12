/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD's PCI configuration space access
 * (pci_conf_read(9)) through #G and #$/pci (p9_pcicfgat), capability
 * lists, and bus_space_map(9) of a device's memory (an ioremap of the
 * BAR segment, kpi/linux/io.c).
 */
#include "obsd.h"
#include <dev/pci/pcivar.h>
#include <machine/bus.h>
#include <linux/io.h>

pcireg_t
pci_conf_read(pci_chipset_tag_t pc, pcitag_t tag, int reg)
{
	uint32_t v;

	if (p9_pcicfgat(PCITAG_BUS(tag), PCITAG_DEV(tag), PCITAG_FUN(tag), 0, reg & ~3, &v, 4) < 0)
		return 0xffffffff;
	return v;
}

void
pci_conf_write(pci_chipset_tag_t pc, pcitag_t tag, int reg, pcireg_t v)
{
	p9_pcicfgat(PCITAG_BUS(tag), PCITAG_DEV(tag), PCITAG_FUN(tag), 1, reg & ~3, &v, 4);
}

int
pci_get_capability(pci_chipset_tag_t pc, pcitag_t tag, int capid, int *offset, pcireg_t *value)
{
	pcireg_t reg;
	unsigned int off;
	int n;

	reg = pci_conf_read(pc, tag, PCI_COMMAND_STATUS_REG);
	if (!(reg & PCI_STATUS_CAPLIST_SUPPORT))
		return 0;
	off = pci_conf_read(pc, tag, PCI_CAPLISTPTR_REG) & 0xfc;
	for (n = 0; off != 0 && n < 48; n++) {
		reg = pci_conf_read(pc, tag, off);
		if ((reg & 0xff) == (pcireg_t)capid) {
			if (offset != NULL)
				*offset = off;
			if (value != NULL)
				*value = reg;
			return 1;
		}
		off = (reg >> 8) & 0xfc;
	}
	return 0;
}

int
pci_get_ext_capability(pci_chipset_tag_t pc, pcitag_t tag, int capid, int *offset, pcireg_t *value)
{
	pcireg_t reg;
	unsigned int off = PCI_PCIE_ECAP;
	int n;

	for (n = 0; off != 0 && n < 64; n++) {
		reg = pci_conf_read(pc, tag, off);
		if (reg == 0 || reg == 0xffffffff)
			return 0;
		if (PCI_PCIE_ECAP_ID(reg) == (pcireg_t)capid) {
			if (offset != NULL)
				*offset = off;
			if (value != NULL)
				*value = reg;
			return 1;
		}
		off = PCI_PCIE_ECAP_NEXT(reg);
	}
	return 0;
}

int
bus_space_map(bus_space_tag_t t, bus_addr_t addr, bus_size_t size, int flags, bus_space_handle_t *hp)
{
	void *va;

	if (t != X86_BUS_SPACE_MEM)
		return EINVAL;	/* port I/O goes through inb/outb (#P) */
	if (flags & BUS_SPACE_MAP_PREFETCHABLE)
		va = ioremap_wc(addr, size);
	else
		va = ioremap(addr, size);
	if (va == NULL)
		return ENOMEM;
	*hp = (bus_space_handle_t)va;
	return 0;
}

void
bus_space_unmap(bus_space_tag_t t, bus_space_handle_t h, bus_size_t size)
{
	iounmap((void *)h);
}
