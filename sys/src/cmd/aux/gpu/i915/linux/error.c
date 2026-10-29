// SPDX-License-Identifier: MIT
/*
 * aux/i915: the GPU's error state (gpu(4)'s error file), as Linux's
 * /sys/class/drm/card0/error (i915_sysfs.c): i915 captures it when the
 * GPU hangs (CONFIG_DRM_I915_CAPTURE_ERROR); reading gives its text,
 * which Mesa's aubinator_error_decode reads, writing clears it.
 */
#include <linux/kernel.h>
#include <linux/pci.h>
#include "i915_drv.h"
#include "i915_gpu_error.h"
#include "../i915.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);

static struct drm_i915_private *
i915(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);

	return p ? pci_get_drvdata(p) : NULL;
}

/* n bytes of the error state's text at off; their number, or -errno */
long
i915_error_read(char *buf, long n, long long off)
{
	static const char none[] = "No error state collected\n";
	struct drm_i915_private *i = i915();
	struct i915_gpu_coredump *gpu;
	long r;

	if (!i)
		return -ENODEV;
	gpu = i915_first_error_state(i);
	if (IS_ERR(gpu))
		return PTR_ERR(gpu);
	if (gpu) {
		r = i915_gpu_coredump_copy_to_buffer(gpu, buf, off, n);
		i915_gpu_coredump_put(gpu);
		return r;
	}
	if (off >= (long long)sizeof(none) - 1)
		return 0;
	r = min_t(long, n, sizeof(none) - 1 - off);
	memcpy(buf, none + off, r);
	return r;
}

void
i915_error_clear(void)
{
	struct drm_i915_private *i = i915();

	if (i)
		i915_reset_error_state(i);
}
