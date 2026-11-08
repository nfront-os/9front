// SPDX-License-Identifier: MIT
/*
 * aux/i915: the display, the Linux side.  The outputs, the screen and
 * the clients' pictures are kmsout.c's (shared with aux/amdgpu); here
 * what is i915's: the primary node opened, the screen object (stolen
 * memory, else shmem; pinned in the mappable part of the GGTT, where
 * the processor writes it through the aperture, BAR 2: the Plan 9 side,
 * screen.c, copies into it what devdraw changes in #G's framebuffer), a
 * panel's backlight (intel_backlight), the screen for a client's map
 * (gpu(4) map plane).
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/backlight.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_connector.h>
#include <drm/drm_mode.h>
#include "i915_drv.h"
#include "i915_driver.h"
#include "gem/i915_gem_object.h"
#include "gem/i915_gem_stolen.h"
#include "gem/i915_gem_domain.h"
#include "i915_vma.h"
#include "display/intel_display_types.h"
#include "../i915.h"
#include "../../kmsout.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);

static struct file *kms;		/* the primary node, master */

/* the screen objects pinned (the old one goes after the new one is lit) */
static struct {
	u32	handle;
	struct i915_vma	*vma;
} pinned[2];

/* Linux 7.3: drm_open_helper wants the flag DRM's own GEM fops have */
static const struct file_operations kms_fops = { .fop_flags = FOP_UNSIGNED_OFFSET };

static struct drm_i915_private *
i915dev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);

	return p ? pci_get_drvdata(p) : NULL;
}

/*
 * the device as Linux leaves it at a reboot (i915_pci_shutdown): the
 * display off, interrupts off, the GT idle, so that the next system (a
 * warm reboot into another one) finds it as after the firmware; for the
 * end of aux/i915 (fshalt's note, a fatal fault)
 */
void
i915_shutdown(void)
{
	struct drm_i915_private *i915 = i915dev();

	if (i915)
		i915_driver_shutdown(i915);
}

static int
kioctl(struct file *f, unsigned int cmd, void *arg)
{
	return drm_ioctl(f, cmd, (unsigned long)arg);
}

/*
 * the screen object: its cache level the display's (uncached, write-
 * through with eDRAM) before it is pinned, so that the display's pin
 * keeps our binding; at an alignment every display version's linear
 * surfaces take (256 KB)
 */
static int
screen_alloc(struct file *f, u32 w, u32 h, u32 *handle, u32 *pitch, unsigned long long *baroff)
{
	struct drm_i915_private *i915 = i915dev();
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	u64 size;
	int r, i;

	for (i = 0; i < ARRAY_SIZE(pinned) && pinned[i].vma; i++)
		;
	if (i == ARRAY_SIZE(pinned))
		return -EBUSY;
	*pitch = ALIGN(w * 4, 64);
	size = PAGE_ALIGN((u64)*pitch * h);
	obj = i915_gem_object_create_stolen(i915, size);
	if (IS_ERR(obj))
		obj = i915_gem_object_create_shmem(i915, size);
	if (IS_ERR(obj))
		return PTR_ERR(obj);
	r = i915_gem_object_lock(obj, NULL);
	if (r == 0) {
		r = i915_gem_object_set_cache_level(obj, HAS_WT(i915) ? I915_CACHE_WT : I915_CACHE_NONE);
		i915_gem_object_unlock(obj);
	}
	if (r)
		goto put;
	vma = i915_gem_object_ggtt_pin(obj, NULL, 0, SZ_256K, PIN_MAPPABLE);
	if (IS_ERR(vma)) {
		r = PTR_ERR(vma);
		goto put;
	}
	r = drm_gem_handle_create(f->private_data, &obj->base, handle);
	if (r) {
		i915_vma_unpin(vma);
		goto put;
	}
	pinned[i].handle = *handle;
	pinned[i].vma = vma;
	*baroff = i915_ggtt_offset(vma);
	i915_gem_object_put(obj);	/* the handle holds it */
	return 0;
put:
	i915_gem_object_put(obj);
	return r;
}

static void
screen_free(struct file *f, u32 handle)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pinned); i++)
		if (pinned[i].vma && pinned[i].handle == handle) {
			i915_vma_unpin(pinned[i].vma);
			pinned[i].vma = NULL;
		}
	drm_gem_handle_delete(f->private_data, handle);
}

/* the panel's backlight (intel_backlight_device_register) */
static struct backlight_device *
backlight(struct drm_connector *c)
{
	if (c->connector_type == DRM_MODE_CONNECTOR_WRITEBACK)
		return NULL;
	return to_intel_connector(c)->panel.backlight.device;
}

static const struct kmsout_ops ops = {
	.screen_alloc = screen_alloc,
	.screen_free = screen_free,
	.backlight = backlight,
};

int
i915_kms_start(void)
{
	struct drm_i915_private *i915 = i915dev();
	struct drm_device *dev = i915 ? &i915->drm : NULL;
	int r;

	if (!dev || !dev->primary)
		return -ENODEV;
	kms = kpi_alloc_file(NULL, &kms_fops, NULL, O_RDWR);
	if (IS_ERR(kms)) {
		r = PTR_ERR(kms);
		kms = NULL;
		return r;
	}
	r = drm_open_helper(kms, dev->primary);
	if (r) {
		fput(kms);
		kms = NULL;
		return r;
	}
	return kmsout_init(kms, &ops);
}

/* the screen: its object as a handle of file to, its geometry (gpu(4) map plane) */
int
kms_plane(struct file *to, u32 *handle, u32 *w, u32 *h, u32 *stride, u32 *bpp)
{
	struct drm_mode_fb_cmd2 fb = { };
	struct drm_gem_close cl = { };
	int r;

	if (!kms)
		return -ENODEV;
	r = kmsout_screenfb(&fb.fb_id, w, h, stride);
	if (r)
		return r;
	*bpp = 32;
	r = kioctl(kms, DRM_IOCTL_MODE_GETFB2, &fb);
	if (r == 0 && !fb.handles[0])
		r = -ENOENT;
	if (r == 0) {
		r = kmsout_prime(kms, fb.handles[0], to, handle);
		cl.handle = fb.handles[0];
		kioctl(kms, DRM_IOCTL_GEM_CLOSE, &cl);
	}
	return r;
}
