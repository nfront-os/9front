// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: where an Intel integrated GPU's stolen memory
 * lies (Linux finds it in arch/x86/kernel/early-quirks.c, GPL; this is
 * our own from the registers' documentation): its base in the GPU's
 * configuration space (BDSM), its size in the graphics control register
 * (GGC) of the host bridge's copy at 0:2.0, per generation.  The result
 * is intel_graphics_stolen_res, as on Linux.
 */
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/sizes.h>
#include <drm/intel/i915_drm.h>
#include <drm/intel/pciids.h>

struct resource intel_graphics_stolen_res = DEFINE_RES_MEM(0, 0);

struct stolen {
	resource_size_t	(*base)(struct pci_dev *d);
	resource_size_t	(*size)(struct pci_dev *d);
};

#define GGC	0x50	/* word */
#define BDSM	0x5c	/* dword: base above 1 MB */
#define BDSM64	0xc0	/* qword, gen11 on */

static resource_size_t
bdsm(struct pci_dev *d)
{
	u32 v = 0;

	pci_read_config_dword(d, BDSM, &v);
	return v & ~(SZ_1M - 1);
}

static resource_size_t
bdsm64(struct pci_dev *d)
{
	u32 lo = 0, hi = 0;

	pci_read_config_dword(d, BDSM64, &lo);
	pci_read_config_dword(d, BDSM64 + 4, &hi);
	return (((u64)hi << 32) | lo) & ~(u64)(SZ_1M - 1);
}

static u16
ggc(struct pci_dev *d)
{
	u16 v = 0;

	pci_read_config_word(d, GGC, &v);
	return v;
}

/* Sandy Bridge to Haswell, Valleyview: GMS in bits 7:3, 32 MB each */
static resource_size_t gen6_size(struct pci_dev *d) { return (resource_size_t)((ggc(d) >> 3) & 0x1f) * SZ_32M; }

/* Broadwell: GMS in bits 15:8, 32 MB each */
static resource_size_t gen8_size(struct pci_dev *d) { return (resource_size_t)((ggc(d) >> 8) & 0xff) * SZ_32M; }

/* Cherryview: 32 MB steps, then 4 MB steps from 8 MB, then from 36 MB */
static resource_size_t
chv_size(struct pci_dev *d)
{
	unsigned int gms = (ggc(d) >> 8) & 0xff;

	if (gms < 0x11)
		return (resource_size_t)gms * SZ_32M;
	if (gms < 0x17)
		return (resource_size_t)(gms - 0x11) * SZ_4M + SZ_8M;
	return (resource_size_t)(gms - 0x17) * SZ_4M + SZ_8M * 4 + SZ_4M;
}

/* Skylake on: 32 MB steps below 0xf0, then 4 MB steps from 4 MB */
static resource_size_t
gen9_size(struct pci_dev *d)
{
	unsigned int gms = (ggc(d) >> 8) & 0xff;

	if (gms < 0xf0)
		return (resource_size_t)gms * SZ_32M;
	return (resource_size_t)(gms - 0xf0) * SZ_4M + SZ_4M;
}

static const struct stolen gen6 = { bdsm, gen6_size };
static const struct stolen gen8 = { bdsm, gen8_size };
static const struct stolen chv = { bdsm, chv_size };
static const struct stolen gen9 = { bdsm, gen9_size };
static const struct stolen gen11 = { bdsm64, gen9_size };

static const struct pci_device_id ids[] = {
	INTEL_SNB_D_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_SNB_M_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_IVB_M_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_IVB_D_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_HSW_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_VLV_IDS(INTEL_VGA_DEVICE, &gen6),
	INTEL_BDW_IDS(INTEL_VGA_DEVICE, &gen8),
	INTEL_CHV_IDS(INTEL_VGA_DEVICE, &chv),
	INTEL_SKL_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_BXT_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_KBL_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_CFL_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_GLK_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_CNL_IDS(INTEL_VGA_DEVICE, &gen9),
	INTEL_ICL_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_EHL_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_JSL_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_TGL_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_RKL_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_ADLS_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_ADLP_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_ADLN_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_RPLS_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_RPLU_IDS(INTEL_VGA_DEVICE, &gen11),
	INTEL_RPLP_IDS(INTEL_VGA_DEVICE, &gen11),
	{ 0 }
};

/* called with the device of #G before its driver probes */
void
kpi_intel_graphics_quirks(struct pci_dev *d)
{
	const struct pci_device_id *id;
	const struct stolen *s;
	resource_size_t base, size;

	if (d->vendor != PCI_VENDOR_ID_INTEL || d->bus->number != 0 || d->devfn != PCI_DEVFN(2, 0))
		return;
	id = pci_match_id(ids, d);
	if (!id)
		return;
	s = (const struct stolen *)id->driver_data;
	size = s->size(d);
	if (!size)
		return;
	base = s->base(d);
	intel_graphics_stolen_res = (struct resource)DEFINE_RES_MEM(base, size);
	pr_info("Intel graphics stolen memory: %pR\n", &intel_graphics_stolen_res);
}
