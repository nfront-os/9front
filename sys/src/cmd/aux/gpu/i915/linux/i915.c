// SPDX-License-Identifier: MIT
/*
 * aux/i915: the Linux side, where Linux's names are seen: i915's
 * module_init, and what the DRM core asks of the file server.
 */
#include <linux/kernel.h>
#include <drm/drm_device.h>
#include <drm/drm_connector.h>
#include "../i915.h"

int	kpi_init_i915_init(void);

int
i915_start(void)
{
	return kpi_init_i915_init();
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
