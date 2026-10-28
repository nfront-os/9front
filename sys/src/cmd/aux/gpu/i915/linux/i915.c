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
#include <linux/workqueue.h>
#include <linux/mm.h>
#include "../i915.h"

int	kpi_init_i915_init(void);
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

int
i915_selftest(void)
{
	struct segtest t;
	struct page *p;

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
	return t.seen == 0x915915 ? 0 : -EIO;
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
