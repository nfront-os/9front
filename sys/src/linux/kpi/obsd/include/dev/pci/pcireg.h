/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, dev/pci/pcireg.h: the PCI
 * configuration space under OpenBSD's names, the values the PCI and
 * PCI Express specifications give.
 */
#ifndef _OBSD_DEV_PCI_PCIREG_H
#define _OBSD_DEV_PCI_PCIREG_H

#define PCI_ID_REG			0x00
#define PCI_VENDOR(id)			((id) & 0xffff)
#define PCI_PRODUCT(id)			(((id) >> 16) & 0xffff)
#define PCI_COMMAND_STATUS_REG		0x04
#define PCI_COMMAND_IO_ENABLE		0x00000001
#define PCI_COMMAND_MEM_ENABLE		0x00000002
#define PCI_COMMAND_MASTER_ENABLE	0x00000004
#define PCI_STATUS_CAPLIST_SUPPORT	0x00100000
#define PCI_CLASS_REG			0x08
#define PCI_CLASS(cr)			(((cr) >> 24) & 0xff)
#define PCI_SUBCLASS(cr)		(((cr) >> 16) & 0xff)
#define PCI_REVISION(cr)		((cr) & 0xff)
#define PCI_BHLC_REG			0x0c
#define PCI_HDRTYPE_TYPE(bhlcr)		(((bhlcr) >> 16) & 0x7f)
#define PCI_MAPREG_START		0x10
#define PCI_SUBSYS_ID_REG		0x2c
#define PCI_CAPLISTPTR_REG		0x34
#define PCI_INTERRUPT_REG		0x3c
#define PCI_PRIBUS_1			0x18	/* bridges: primary bus number */

#define PCI_CLASS_DISPLAY		0x03
#define PCI_SUBCLASS_DISPLAY_VGA	0x00
#define PCI_SUBCLASS_DISPLAY_XGA	0x01
#define PCI_SUBCLASS_DISPLAY_3D		0x02
#define PCI_SUBCLASS_DISPLAY_MISC	0x80
#define PCI_CLASS_BRIDGE		0x06
#define PCI_CLASS_ACCELERATOR		0x12

/* capabilities */
#define PCI_CAP_PWRMGMT			0x01
#define PCI_CAP_AGP			0x02
#define PCI_CAP_VPD			0x03
#define PCI_CAP_MSI			0x05
#define PCI_CAP_VENDSPEC		0x09
#define PCI_CAP_PCIEXPRESS		0x10
#define PCI_CAP_MSIX			0x11

/* PCI Express capability, offsets in it */
#define PCI_PCIE_XCAP			0x00
#define PCI_PCIE_XCAP_VER(x)		(((x) >> 16) & 0x0f)
#define PCI_PCIE_DCAP			0x04
#define PCI_PCIE_DCSR			0x08
#define PCI_PCIE_DCSR_MPS		0x000000e0
#define PCI_PCIE_LCAP			0x0c
#define PCI_PCIE_LCSR			0x10
#define PCI_PCIE_LCSR_ASPM_L0S		0x00000001
#define PCI_PCIE_LCSR_ASPM_L1		0x00000002
#define PCI_PCIE_LCAP2			0x2c
#define PCI_PCIE_LCSR2			0x30
#define PCI_PCIE_LCSR2_TLS		0x0000000f
#define PCI_PCIE_LCSR2_TLS_2_5		0x00000001
#define PCI_PCIE_LCSR2_TLS_5		0x00000002
#define PCI_PCIE_LCSR2_TLS_8		0x00000003

/* extended capabilities, from 0x100 */
#define PCI_PCIE_ECAP			0x100
#define PCI_PCIE_ECAP_ID(x)		((x) & 0xffff)
#define PCI_PCIE_ECAP_VER(x)		(((x) >> 16) & 0x0f)
#define PCI_PCIE_ECAP_NEXT(x)		(((x) >> 20) & 0xffc)
#define PCI_PCIE_ECAP_LAST		0x000

#endif
