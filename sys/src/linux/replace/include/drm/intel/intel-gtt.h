/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/intel-gtt.h (ours: Linux's is GPL) */
#ifndef _KPI_DRM_INTEL_GTT_H
#define _KPI_DRM_INTEL_GTT_H

#include <linux/types.h>

/* the GTT of Intel's chipsets before gen 6 (drivers/char/agp): not driven here */
struct agp_bridge_data;
struct pci_dev;
struct sg_table;
#define AGP_DCACHE_MEMORY	1
#define AGP_PHYS_MEMORY		2
#define AGP_USER_MEMORY		1
#define AGP_USER_CACHED_MEMORY	2
#define intel_gmch_probe(b, g, br)		0
#define intel_gmch_gtt_get(gs, ms, mb)		do { *(gs) = 0; *(ms) = 0; *(mb) = 0; } while (0)
#define intel_gmch_enable_gtt()			0
#define intel_gmch_gtt_flush()			do { } while (0)
#define intel_gmch_remove()			do { } while (0)
#define intel_gmch_gtt_insert_page(a, p, f)	do { } while (0)
#define intel_gmch_gtt_insert_sg_entries(s, p, f)	do { } while (0)
#define intel_gmch_gtt_clear_range(f, n)	do { } while (0)

/* no GMCH GTT here: no entry is present */
#define intel_gmch_gtt_read_entry(pg, present, local)	({ *(present) = false; *(local) = false; (dma_addr_t)0; })

#endif
