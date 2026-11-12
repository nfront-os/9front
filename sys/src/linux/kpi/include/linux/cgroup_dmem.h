/* Public domain. */
/*
 * 9front: the memory cgroup of device memory, as Linux 7.3 calls it,
 * without cgroups (Linux without CONFIG_CGROUP_DMEM): no region to
 * account in, every charge granted, nothing protected.  (OpenBSD's
 * stubs of 6.18, brought to 7.3's interface.)
 */

#ifndef _LINUX_CGROUP_DMEM_H
#define _LINUX_CGROUP_DMEM_H

#include <linux/types.h>

struct dmem_cgroup_pool_state;
struct dmem_cgroup_region;

/* what a driver gives for its region: its size, how to reclaim from it */
struct dmem_cgroup_ops {
	int	(*reclaim)(struct dmem_cgroup_pool_state *pool, u64 target_bytes, void *priv);
};

struct dmem_cgroup_init {
	u64	size;
	const struct dmem_cgroup_ops	*ops;
	void	*reclaim_priv;
};

static inline struct dmem_cgroup_region *
dmem_cgroup_register_region(const struct dmem_cgroup_init *init, const char *fmt, ...)
{
	return NULL;
}

static inline void
dmem_cgroup_unregister_region(struct dmem_cgroup_region *r)
{
}

static inline int
dmem_cgroup_try_charge(struct dmem_cgroup_region *r, u64 size,
    struct dmem_cgroup_pool_state **pool, struct dmem_cgroup_pool_state **limit)
{
	*pool = NULL;
	if (limit)
		*limit = NULL;
	return 0;
}

static inline void
dmem_cgroup_uncharge(struct dmem_cgroup_pool_state *pool, u64 size)
{
}

static inline bool
dmem_cgroup_state_evict_valuable(struct dmem_cgroup_pool_state *limit,
    struct dmem_cgroup_pool_state *test, bool ignore_low, bool *hit_low)
{
	return true;
}

static inline bool
dmem_cgroup_below_min(struct dmem_cgroup_pool_state *root, struct dmem_cgroup_pool_state *test)
{
	return false;
}

static inline bool
dmem_cgroup_below_low(struct dmem_cgroup_pool_state *root, struct dmem_cgroup_pool_state *test)
{
	return false;
}

static inline struct dmem_cgroup_pool_state *
dmem_cgroup_get_common_ancestor(struct dmem_cgroup_pool_state *a, struct dmem_cgroup_pool_state *b)
{
	return NULL;
}

static inline void
dmem_cgroup_pool_state_put(struct dmem_cgroup_pool_state *pool)
{
}

#endif
