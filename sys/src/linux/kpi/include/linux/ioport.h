/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/ioport.h (ours, not Linux's) */
#ifndef _KPI_LINUX_IOPORT_H
#define _KPI_LINUX_IOPORT_H

#include <linux/kernel.h>

struct resource { resource_size_t start; resource_size_t end; const char *name; unsigned long flags; unsigned long desc; struct resource *parent, *sibling, *child; };
enum { IORES_DESC_NONE = 0, IORES_DESC_RESERVED = 7 };
#define IORESOURCE_IO		0x00000100
#define IORESOURCE_IRQ		0x00000400
#define IORESOURCE_MEM		0x00000200
#define IORESOURCE_PREFETCH	0x00002000
#define IORESOURCE_MEM_64	0x00100000
#define IORESOURCE_UNSET	0x20000000
#define IORESOURCE_BUSY		0x80000000
#define IORESOURCE_ROM_SHADOW	(1 << 1)
#define DEFINE_RES_NAMED(s, l, n, f)	((struct resource){ .start = (s), .end = (s) + (l) - 1, .name = (n), .flags = (f) })
#define DEFINE_RES_MEM(s, l)		DEFINE_RES_NAMED(s, l, NULL, IORESOURCE_MEM)
static inline resource_size_t resource_size(const struct resource *r) { return r->end - r->start + 1; }
static inline bool resource_contains(struct resource *a, struct resource *b) { return a->start <= b->start && a->end >= b->end; }
static inline bool resource_overlaps(struct resource *a, struct resource *b) { return a->start <= b->end && b->start <= a->end; }
extern struct resource iomem_resource;
extern struct resource intel_graphics_stolen_res;
#define request_resource(r, n)		0
#define release_resource(r)		0
#define insert_resource(r, n)		0
#define request_mem_region(s, n, name)	((struct resource *)1)
#define release_mem_region(s, n)	do { } while (0)
#define __request_region(p, s, n, nm, f)	((struct resource *)1)
#define devm_request_mem_region(d, s, n, nm)	((struct resource *)1)

#endif
