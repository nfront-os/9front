/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: list_lru (linux/list_lru.h), one list per lru.
 */
#include <linux/list_lru.h>

int
__list_lru_init(struct list_lru *lru, bool memcg_aware, struct shrinker *shrinker)
{
	spin_lock_init(&lru->lock);
	INIT_LIST_HEAD(&lru->one.list);
	lru->one.nr_items = 0;
	return 0;
}

void
list_lru_destroy(struct list_lru *lru)
{
	/* the items are the owner's: they must be gone (as on Linux) */
	WARN_ON(lru->one.nr_items != 0);
}

/* add item if it is on no list: true if added */
bool
list_lru_add(struct list_lru *lru, struct list_head *item, int nid, struct mem_cgroup *memcg)
{
	bool r = false;

	spin_lock(&lru->lock);
	if (list_empty(item)) {
		list_add_tail(item, &lru->one.list);
		lru->one.nr_items++;
		r = true;
	}
	spin_unlock(&lru->lock);
	return r;
}

bool
list_lru_add_obj(struct list_lru *lru, struct list_head *item)
{
	return list_lru_add(lru, item, 0, NULL);
}

/* take item off if it is on the list: true if taken */
bool
list_lru_del(struct list_lru *lru, struct list_head *item, int nid, struct mem_cgroup *memcg)
{
	bool r = false;

	spin_lock(&lru->lock);
	if (!list_empty(item)) {
		list_del_init(item);
		lru->one.nr_items--;
		r = true;
	}
	spin_unlock(&lru->lock);
	return r;
}

bool
list_lru_del_obj(struct list_lru *lru, struct list_head *item)
{
	return list_lru_del(lru, item, 0, NULL);
}

unsigned long
list_lru_count_one(struct list_lru *lru, int nid, struct mem_cgroup *memcg)
{
	return READ_ONCE(lru->one.nr_items);
}

unsigned long
list_lru_count_node(struct list_lru *lru, int nid)
{
	return READ_ONCE(lru->one.nr_items);
}

void
list_lru_isolate(struct list_lru_one *list, struct list_head *item)
{
	list_del_init(item);
	list->nr_items--;
}

void
list_lru_isolate_move(struct list_lru_one *list, struct list_head *item, struct list_head *head)
{
	list_move(item, head);
	list->nr_items--;
}

/*
 * up to *nr_to_walk items through isolate, under the lock; the number
 * taken off.  A callback that dropped the lock (_RETRY) makes the walk
 * start over, as the list may have changed.
 */
unsigned long
list_lru_walk_one(struct list_lru *lru, int nid, struct mem_cgroup *memcg,
	list_lru_walk_cb isolate, void *cb_arg, unsigned long *nr_to_walk)
{
	struct list_head *item, *n;
	unsigned long isolated = 0;

	spin_lock(&lru->lock);
restart:
	list_for_each_safe(item, n, &lru->one.list) {
		if (*nr_to_walk == 0)
			break;
		--*nr_to_walk;
		switch (isolate(item, &lru->one, cb_arg)) {
		case LRU_REMOVED:
			isolated++;
			break;
		case LRU_REMOVED_RETRY:
			isolated++;
			goto restart;
		case LRU_ROTATE:
			list_move_tail(item, &lru->one.list);
			break;
		case LRU_SKIP:
			break;
		case LRU_RETRY:
			goto restart;
		case LRU_STOP:
			goto out;
		}
	}
out:
	spin_unlock(&lru->lock);
	return isolated;
}

unsigned long
list_lru_walk_node(struct list_lru *lru, int nid,
	list_lru_walk_cb isolate, void *cb_arg, unsigned long *nr_to_walk)
{
	return list_lru_walk_one(lru, nid, NULL, isolate, cb_arg, nr_to_walk);
}
