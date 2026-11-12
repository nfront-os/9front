/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/iosys-map.h (ours, not Linux's) */
#ifndef _KPI_LINUX_IOSYS_MAP_H
#define _KPI_LINUX_IOSYS_MAP_H

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/io.h>

/* a pointer to memory or I/O memory (ours: Linux's is GPL) */
struct iosys_map { union { void __iomem *vaddr_iomem; void *vaddr; }; bool is_iomem; };
#define IOSYS_MAP_INIT_VADDR(v)		{ .vaddr = (v), .is_iomem = false }
#define IOSYS_MAP_INIT_VADDR_IOMEM(v)	{ .vaddr_iomem = (v), .is_iomem = true }
#define IOSYS_MAP_INIT_OFFSET(m, o)	({ struct iosys_map __m = *(m); iosys_map_incr(&__m, o); __m; })
static inline void iosys_map_set_vaddr(struct iosys_map *m, void *v) { m->vaddr = v; m->is_iomem = false; }
static inline void iosys_map_set_vaddr_iomem(struct iosys_map *m, void __iomem *v) { m->vaddr_iomem = v; m->is_iomem = true; }
static inline bool iosys_map_is_null(const struct iosys_map *m) { return !m->vaddr; }
static inline bool iosys_map_is_set(const struct iosys_map *m) { return !iosys_map_is_null(m); }
static inline bool iosys_map_is_equal(const struct iosys_map *a, const struct iosys_map *b) { return a->is_iomem == b->is_iomem && a->vaddr == b->vaddr; }
static inline void iosys_map_clear(struct iosys_map *m) { m->vaddr = NULL; m->is_iomem = false; }
static inline void iosys_map_incr(struct iosys_map *m, size_t n) { m->vaddr = (char *)m->vaddr + n; }
static inline void iosys_map_memcpy_to(struct iosys_map *d, size_t o, const void *s, size_t n) { memcpy((char *)d->vaddr + o, s, n); }
static inline void iosys_map_memcpy_from(void *d, const struct iosys_map *s, size_t o, size_t n) { memcpy(d, (char *)s->vaddr + o, n); }
static inline void iosys_map_memset(struct iosys_map *d, size_t o, int v, size_t n) { memset((char *)d->vaddr + o, v, n); }
#define iosys_map_rd(m, o, t)		({ t __v; iosys_map_memcpy_from(&__v, m, o, sizeof(t)); __v; })
#define iosys_map_wr(m, o, t, v)	({ t __v = (v); iosys_map_memcpy_to(m, o, &__v, sizeof(t)); })
#define iosys_map_rd_field(m, so, st, f)	({ st *__s; iosys_map_rd(m, (so) + offsetof(st, f), typeof(__s->f)); })
#define iosys_map_wr_field(m, so, st, f, v)	({ st *__s; iosys_map_wr(m, (so) + offsetof(st, f), typeof(__s->f), v); })

#endif
