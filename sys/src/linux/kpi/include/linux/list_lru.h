/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/list_lru.h (ours: Linux's is GPL).  A list
 * of reclaimable objects (TTM's page pools since Linux 6.15): here one
 * list per lru, as 9front has one memory node and no memory cgroups;
 * nid and memcg are taken and ignored.  The walks hold the lru's lock
 * around each callback, as Linux's do.
 */
#ifndef _KPI_LINUX_LIST_LRU_H
#define _KPI_LINUX_LIST_LRU_H

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/nodemask.h>

struct mem_cgroup;
struct shrinker;
struct shrink_control;

/* what a walk's callback did with an item */
enum lru_status {
	LRU_REMOVED,		/* taken off the list (list_lru_isolate) */
	LRU_REMOVED_RETRY,	/* taken off, the lock dropped and retaken: walk again */
	LRU_ROTATE,		/* to the tail: another pass later */
	LRU_SKIP,		/* left where it is */
	LRU_RETRY,		/* not taken, the lock dropped: walk again */
	LRU_STOP,		/* stop the walk */
};

/* the items of one list (Linux: per node and memory cgroup) */
struct list_lru_one {
	struct list_head	list;
	long	nr_items;
};

struct list_lru {
	spinlock_t	lock;
	struct list_lru_one	one;
};

/* what a walk does with an item: the item, the list it is on, the walker's argument */
typedef enum lru_status (*list_lru_walk_cb)(struct list_head *, struct list_lru_one *, void *);

int	__list_lru_init(struct list_lru *lru, bool memcg_aware, struct shrinker *shrinker);
#define list_lru_init(lru)	__list_lru_init((lru), false, NULL)
#define list_lru_init_memcg(lru, shrinker)	__list_lru_init((lru), true, (shrinker))
void	list_lru_destroy(struct list_lru *lru);

bool	list_lru_add(struct list_lru *lru, struct list_head *item, int nid, struct mem_cgroup *memcg);
bool	list_lru_add_obj(struct list_lru *lru, struct list_head *item);
bool	list_lru_del(struct list_lru *lru, struct list_head *item, int nid, struct mem_cgroup *memcg);
bool	list_lru_del_obj(struct list_lru *lru, struct list_head *item);

unsigned long	list_lru_count_one(struct list_lru *lru, int nid, struct mem_cgroup *memcg);
unsigned long	list_lru_count_node(struct list_lru *lru, int nid);

static inline unsigned long
list_lru_count(struct list_lru *lru)
{
	return list_lru_count_node(lru, 0);
}

/* in a walk's callback: the item off the list (onto head) */
void	list_lru_isolate(struct list_lru_one *list, struct list_head *item);
void	list_lru_isolate_move(struct list_lru_one *list, struct list_head *item, struct list_head *head);

unsigned long	list_lru_walk_one(struct list_lru *lru, int nid, struct mem_cgroup *memcg,
	list_lru_walk_cb isolate, void *cb_arg, unsigned long *nr_to_walk);
unsigned long	list_lru_walk_node(struct list_lru *l, int node,
	list_lru_walk_cb f, void *arg, unsigned long *left);

static inline unsigned long
list_lru_walk(struct list_lru *lru, list_lru_walk_cb isolate, void *cb_arg, unsigned long nr_to_walk)
{
	return list_lru_walk_node(lru, 0, isolate, cb_arg, &nr_to_walk);
}

#endif
