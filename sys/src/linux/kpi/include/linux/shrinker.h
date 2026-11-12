/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/shrinker.h (ours, not Linux's) */
#ifndef _KPI_LINUX_SHRINKER_H
#define _KPI_LINUX_SHRINKER_H

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/gfp.h>

/*
 * shrinkers: what can give memory back when it gets short (kpi/slab.c
 * calls them when an allocation fails)
 */
struct shrink_control { gfp_t gfp_mask; int nid; unsigned long nr_to_scan; unsigned long nr_scanned; struct mem_cgroup *memcg; };
#define SHRINK_STOP		(~0UL)
#define SHRINK_EMPTY		(~0UL - 1)
#define DEFAULT_SEEKS		2
#define SHRINK_BATCH		128
#define SHRINKER_NUMA_AWARE	(1 << 0)
#define SHRINKER_MEMCG_AWARE	(1 << 1)
struct shrinker {
	unsigned long	(*count_objects)(struct shrinker *s, struct shrink_control *sc);
	unsigned long	(*scan_objects)(struct shrinker *s, struct shrink_control *sc);
	long	batch;
	int	seeks;
	unsigned	flags;
	void	*private_data;
	struct list_head	list;
};
int	register_shrinker(struct shrinker *s, const char *fmt, ...) __printf(2, 3);
void	unregister_shrinker(struct shrinker *s);
struct shrinker	*shrinker_alloc(unsigned int flags, const char *fmt, ...) __printf(2, 3);
void	shrinker_register(struct shrinker *s);
void	shrinker_free(struct shrinker *s);
void	synchronize_shrinkers(void);

#endif
