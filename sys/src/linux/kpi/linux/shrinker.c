// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: shrinkers.  When the page pool cannot grow, it
 * asks them to give back what they can (kpi_shrink), as Linux's reclaim
 * does.
 */
#include <linux/shrinker.h>
#include <linux/slab.h>
#include <linux/rwsem.h>

static DECLARE_RWSEM(shrinkers_lock);
static LIST_HEAD(shrinkers);

int
register_shrinker(struct shrinker *s, const char *fmt, ...)
{
	if (!s->batch)
		s->batch = SHRINK_BATCH;
	down_write(&shrinkers_lock);
	list_add_tail(&s->list, &shrinkers);
	up_write(&shrinkers_lock);
	return 0;
}

void
unregister_shrinker(struct shrinker *s)
{
	if (!s->list.next)
		return;
	down_write(&shrinkers_lock);
	list_del(&s->list);
	up_write(&shrinkers_lock);
	s->list.next = NULL;
}

struct shrinker *
shrinker_alloc(unsigned int flags, const char *fmt, ...)
{
	struct shrinker *s = kzalloc(sizeof(*s), GFP_KERNEL);

	if (s)
		s->flags = flags;
	return s;
}

void shrinker_register(struct shrinker *s) { register_shrinker(s, "%s", ""); }

void
shrinker_free(struct shrinker *s)
{
	if (!s)
		return;
	unregister_shrinker(s);
	kfree(s);
}

/* a running shrinker is done when it lets go of the lock */
void
synchronize_shrinkers(void)
{
	down_write(&shrinkers_lock);
	up_write(&shrinkers_lock);
}

/* ask all shrinkers for up to want objects; what they freed */
unsigned long
kpi_shrink(gfp_t gfp, unsigned long want)
{
	struct shrink_control sc = { .gfp_mask = gfp };
	struct shrinker *s;
	unsigned long n, freed = 0, r;

	if (!down_read_trylock(&shrinkers_lock))
		return 0;
	list_for_each_entry(s, &shrinkers, list) {
		n = s->count_objects(s, &sc);
		if (n == 0 || n == SHRINK_EMPTY)
			continue;
		while (n > 0 && freed < want) {
			sc.nr_to_scan = min_t(unsigned long, n, s->batch);
			sc.nr_scanned = sc.nr_to_scan;
			r = s->scan_objects(s, &sc);
			if (r == SHRINK_STOP)
				break;
			freed += r;
			n -= min(n, sc.nr_scanned);
		}
		if (freed >= want)
			break;
	}
	up_read(&shrinkers_lock);
	return freed;
}
