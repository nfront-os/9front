// SPDX-License-Identifier: MIT
/*
 * aux/i915: the Linux side, where Linux's names are seen: i915's
 * module_init, and what the DRM core asks of the file server.
 */
#include <linux/kernel.h>
#include <drm/drm_device.h>
#include <drm/drm_connector.h>
#include <drm/drm_print.h>
#include "i915_params.h"
#include "i915_selftest.h"
#include <linux/workqueue.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include "../i915.h"

int	kpi_init_i915_init(void);
int	kpi_init_st_init(void);	/* drivers/dma-buf/selftest.c */
extern int kpi_loglevel;

void
i915_options(int nodisplay, unsigned long drmdebug, int loglevel)
{
	i915_modparams.disable_display = nodisplay;
	__drm_debug = drmdebug;
	kpi_loglevel = loglevel;
}

int
i915_start(void)
{
	return kpi_init_i915_init();
}

/*
 * -T: what the laptop hit first: a page of a pool chunk attached after
 * the workers forked, read by a worker (p9_fault attaches it there)
 */
struct segtest {
	struct work_struct	work;
	u32	*page;
	u32	seen;
};

static void
segtest_work(struct work_struct *w)
{
	struct segtest *t = container_of(w, struct segtest, work);

	t->seen = READ_ONCE(t->page[100]);
}

/* vmap of pages apart: an alias segment (segment(3)) shows the same memory */
static int
vmap_selftest(void)
{
	struct page *p[3], *v[2];
	struct segtest t;
	u32 *a;
	int ok;

	p[0] = alloc_page(GFP_KERNEL | __GFP_ZERO);
	p[1] = alloc_page(GFP_KERNEL | __GFP_ZERO);
	p[2] = alloc_page(GFP_KERNEL | __GFP_ZERO);
	if (!p[0] || !p[1] || !p[2])
		return -ENOMEM;
	v[0] = p[2];
	v[1] = p[0];
	a = vmap(v, 2, 0, PAGE_KERNEL);
	if (!a) {
		pr_info("selftest: vmap of 2 pages apart: FAIL (no mapping)\n");
		return -EIO;
	}
	a[1] = 0x1111;
	a[1024 + 2] = 0x2222;
	((u32 *)page_address(p[2]))[3] = 0x3333;
	ok = ((u32 *)page_address(p[2]))[1] == 0x1111 && ((u32 *)page_address(p[0]))[2] == 0x2222 && a[3] == 0x3333;
	/* a worker forked before the alias existed reads through it */
	t.page = a + 1024;
	t.page[100] = 0x915916;
	t.seen = 0;
	INIT_WORK(&t.work, segtest_work);
	queue_work(system_unbound_wq, &t.work);
	flush_work(&t.work);
	ok = ok && t.seen == 0x915916;
	pr_info("selftest: vmap of 2 pages apart (alias segment at %p): %s\n", a, ok ? "ok" : "FAIL");
	vunmap(a);
	put_page(p[0]);
	put_page(p[1]);
	put_page(p[2]);
	return ok ? 0 : -EIO;
}

int
i915_kpitest(void)
{
	struct segtest t;
	struct page *p;
	int r;

	p = alloc_page(GFP_KERNEL | __GFP_ZERO);
	if (!p)
		return -ENOMEM;
	t.page = page_address(p);
	t.page[100] = 0x915915;
	t.seen = 0;
	INIT_WORK(&t.work, segtest_work);
	queue_work(system_unbound_wq, &t.work);
	flush_work(&t.work);
	pr_info("selftest: a worker read %#x from a page of a later chunk: %s\n", t.seen,
		t.seen == 0x915915 ? "ok" : "FAIL");
	put_page(p);
	if (t.seen != 0x915915)
		return -EIO;
	r = vmap_selftest();
	if (r)
		return r;
	/* Linux's selftests of fences, chains, arrays, reservation objects */
	r = kpi_init_st_init();
	pr_info("selftest: Linux's dma-buf selftests: %s (%d)\n", r ? "FAIL" : "ok", r);
	return r;
}

/*
 * -M: i915's mock selftests (no hardware: a mock device), as Linux runs
 * them with i915.mock_selftests=-1: i915_init runs its init functions,
 * the module's caches first, the selftests among them, then stops
 * there; filter as i915.st_filter (NULL: keep it, as -p set it).  0, or the first
 * failure's error
 */
int
i915_mock(char *filter)
{
	int r;

	i915_selftest.mock = -1;
	if (filter)
		i915_selftest.filter = filter;
	r = kpi_init_i915_init();
	if (i915_selftest.mock == -ENOTTY)
		return 0;	/* all ran and passed */
	if (i915_selftest.mock < 0)
		return i915_selftest.mock;
	return r < 0 ? r : -EIO;
}

/*
 * -L: the live selftests ran in the probe (live_selftests=-1); their
 * outcome, as i915_selftest.live has it: 0 all passed, or the error.
 * Still -1: they did not run (no device, the probe failed before
 * them): the probe's error.
 */
int
i915_live_result(int probe)
{
	if (i915_selftest.live == -ENOTTY)
		return 0;
	if (i915_selftest.live == -1) {
		/* i915_init succeeds without a device, as in Linux */
		if (probe < 0)
			pr_err("i915: live selftests did not run: error %d\n", probe);
		else
			pr_err("i915: live selftests did not run: no device probed\n");
		return probe < 0 ? probe : -ENODEV;
	}
	return i915_selftest.live < 0 ? i915_selftest.live : -EIO;
}

/* drm_sysfs: a connector's state changed (the file server will tell its clients) */
void
kpi_drm_hotplug(struct drm_device *dev, struct drm_connector *connector)
{
	if (connector)
		pr_info("i915: hotplug %s\n", connector->name);
	else
		pr_info("i915: hotplug\n");
}
