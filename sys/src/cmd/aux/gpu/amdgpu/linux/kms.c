// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: the display, the Linux side.  The outputs, the screen and
 * the clients' pictures are kmsout.c's (shared with aux/i915); here what
 * is amdgpu's: the primary node opened, the screen object (VRAM, in one
 * piece where the processor sees it through BAR 0: the Plan 9 side,
 * screen.c, copies into it what devdraw changes in #G's framebuffer; a
 * display engine of a discrete Radeon scans out VRAM only), an eDP
 * panel's backlight (DC's, one per eDP link).
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/pci.h>
#include <linux/backlight.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_connector.h>
#include "amdgpu.h"
#include "amdgpu_object.h"
#include "amdgpu_gem.h"
#include "amdgpu_res_cursor.h"
#include "amdgpu_dm.h"
#include "../amdgpu.h"
#include "../../kmsout.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);

static struct file *kms;		/* the primary node, master */

/* Linux 7.3: drm_open_helper wants the flag DRM's own GEM fops have */
static const struct file_operations kms_fops = { .fop_flags = FOP_UNSIGNED_OFFSET };

static struct drm_device *
drmdev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);

	return p ? pci_get_drvdata(p) : NULL;
}

/*
 * the screen object: VRAM, contiguous, pinned, in the part of VRAM that
 * BAR 0 shows; DCN wants pitches of 256 bytes
 */
static int
screen_alloc(struct file *f, u32 w, u32 h, u32 *handle, u32 *pitch, unsigned long long *baroff)
{
	struct drm_device *dev = drmdev();
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct drm_gem_object *gobj;
	struct amdgpu_res_cursor cur;
	struct amdgpu_bo *abo;
	u64 size, next;
	int r;

	*pitch = ALIGN(w * 4, 256);
	size = PAGE_ALIGN((u64)*pitch * h);
	r = amdgpu_gem_object_create(adev, size, PAGE_SIZE, AMDGPU_GEM_DOMAIN_VRAM,
	    AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED | AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS,
	    ttm_bo_type_device, NULL, &gobj, 0);
	if (r)
		return r;
	abo = gem_to_amdgpu_bo(gobj);
	r = amdgpu_bo_reserve(abo, false);
	if (r)
		goto put;
	r = amdgpu_bo_pin(abo, AMDGPU_GEM_DOMAIN_VRAM);
	amdgpu_bo_unreserve(abo);
	if (r)
		goto put;
	amdgpu_res_first(abo->tbo.resource, 0, size, &cur);
	*baroff = cur.start;
	for (next = cur.start; cur.remaining; amdgpu_res_next(&cur, cur.size)) {
		if (cur.start != next)
			break;
		next = cur.start + cur.size;
	}
	if (cur.remaining || *baroff + size > adev->gmc.visible_vram_size) {
		r = -ENOSPC;
		goto unpin;
	}
	r = drm_gem_handle_create(f->private_data, gobj, handle);
	if (r)
		goto unpin;
	drm_gem_object_put(gobj);	/* the handle holds it */
	return 0;
unpin:
	if (amdgpu_bo_reserve(abo, false) == 0) {
		amdgpu_bo_unpin(abo);
		amdgpu_bo_unreserve(abo);
	}
put:
	drm_gem_object_put(gobj);
	return r;
}

static void
screen_free(struct file *f, u32 handle)
{
	struct drm_gem_object *gobj = drm_gem_object_lookup(f->private_data, handle);
	struct amdgpu_bo *abo;

	if (gobj) {
		abo = gem_to_amdgpu_bo(gobj);
		if (amdgpu_bo_reserve(abo, false) == 0) {
			amdgpu_bo_unpin(abo);
			amdgpu_bo_unreserve(abo);
		}
		drm_gem_object_put(gobj);
	}
	drm_gem_handle_delete(f->private_data, handle);
}

/* DC's backlight of an eDP link (amdgpu_dm_register_backlight_device) */
static struct backlight_device *
backlight(struct drm_connector *c)
{
	struct amdgpu_device *adev = drm_to_adev(c->dev);
	struct amdgpu_dm_connector *ac;

	if (c->connector_type != DRM_MODE_CONNECTOR_eDP && c->connector_type != DRM_MODE_CONNECTOR_LVDS)
		return NULL;
	ac = to_amdgpu_dm_connector(c);
	if (ac->bl_idx < 0 || ac->bl_idx >= AMDGPU_DM_MAX_NUM_EDP)
		return NULL;
	return adev->dm.backlight_dev[ac->bl_idx];
}

static const struct kmsout_ops ops = {
	.screen_alloc = screen_alloc,
	.screen_free = screen_free,
	.backlight = backlight,
};

int
amdgpu_kms_start(void)
{
	struct drm_device *dev = drmdev();
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
