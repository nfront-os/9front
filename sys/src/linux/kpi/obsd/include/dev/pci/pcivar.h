/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, dev/pci/pcivar.h: PCI
 * configuration space through #G (the device's) and #$/pci (the
 * others'), kpi/obsd/pci.c.  A tag is bus, device and function.
 */
#ifndef _OBSD_DEV_PCI_PCIVAR_H
#define _OBSD_DEV_PCI_PCIVAR_H

#include <sys/types.h>
#include <dev/pci/pcireg.h>

typedef void		*pci_chipset_tag_t;
typedef uint32_t	pcitag_t;
typedef uint32_t	pcireg_t;
typedef struct { int unused; } pci_intr_handle_t;

#define PCITAG_BUS(t)	(((t) >> 16) & 0xff)
#define PCITAG_DEV(t)	(((t) >> 11) & 0x1f)
#define PCITAG_FUN(t)	(((t) >> 8) & 0x07)

static inline pcitag_t
pci_make_tag(pci_chipset_tag_t pc, int bus, int dev, int fn)
{
	return (bus << 16) | (dev << 11) | (fn << 8);
}

static inline void
pci_decompose_tag(pci_chipset_tag_t pc, pcitag_t t, int *bus, int *dev, int *fn)
{
	if (bus != NULL)
		*bus = PCITAG_BUS(t);
	if (dev != NULL)
		*dev = PCITAG_DEV(t);
	if (fn != NULL)
		*fn = PCITAG_FUN(t);
}

pcireg_t	pci_conf_read(pci_chipset_tag_t, pcitag_t, int);
void	pci_conf_write(pci_chipset_tag_t, pcitag_t, int, pcireg_t);
/* 1 and the capability's offset (and first word) if the device has it, else 0 */
int	pci_get_capability(pci_chipset_tag_t, pcitag_t, int, int *, pcireg_t *);
int	pci_get_ext_capability(pci_chipset_tag_t, pcitag_t, int, int *, pcireg_t *);

#endif
