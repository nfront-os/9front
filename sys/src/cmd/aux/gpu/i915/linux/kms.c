// SPDX-License-Identifier: MIT
/*
 * aux/i915: the display, the Linux side.  The file server holds the
 * primary node open (a DRM master, as a display server on Linux) and
 * works through the KMS ioctls: the outputs and their modes, a client
 * object shown with ADDFB2 and a page flip (the object comes over by
 * PRIME, a dma-buf, from the client's render node file), the plane
 * the firmware set up given back afterwards.
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_connector.h>
#include <drm/drm_crtc.h>
#include <drm/drm_edid.h>
#include <drm/drm_modes.h>
#include <drm/drm_modeset_lock.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>
#include "i915_drv.h"
#include "../i915.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);

static struct file *kms;		/* the primary node, master */
static DEFINE_MUTEX(kmslock);
static u32 crtc_id, orig_fb;		/* the firmware's plane: what scanout 0 shows */
static u32 orig_w, orig_h, orig_stride, orig_bpp;
static u32 shown_fb;			/* a client's, on the display */
static struct file *shown_by;		/* the client's file */

/* Linux 7.3: drm_open_helper wants the flag DRM's own GEM fops have */
static const struct file_operations kms_fops = { .fop_flags = FOP_UNSIGNED_OFFSET };

static struct drm_device *
drmdev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);
	struct drm_i915_private *i915 = p ? pci_get_drvdata(p) : NULL;

	return i915 ? &i915->drm : NULL;
}

static int
kioctl(struct file *f, unsigned int cmd, void *arg)
{
	return drm_ioctl(f, cmd, (unsigned long)arg);
}

/* the first active CRTC and its framebuffer */
static void
findplane(void)
{
	struct drm_mode_card_res res = { };
	struct drm_mode_crtc c;
	struct drm_mode_fb_cmd2 fb;
	u32 ids[8];
	unsigned int i;

	res.crtc_id_ptr = (u64)(uintptr_t)ids;
	res.count_crtcs = ARRAY_SIZE(ids);
	if (kioctl(kms, DRM_IOCTL_MODE_GETRESOURCES, &res))
		return;
	for (i = 0; i < res.count_crtcs && i < ARRAY_SIZE(ids); i++) {
		memset(&c, 0, sizeof(c));
		c.crtc_id = ids[i];
		if (kioctl(kms, DRM_IOCTL_MODE_GETCRTC, &c) || !c.mode_valid || !c.fb_id)
			continue;
		crtc_id = c.crtc_id;
		orig_fb = c.fb_id;
		memset(&fb, 0, sizeof(fb));
		fb.fb_id = orig_fb;
		if (kioctl(kms, DRM_IOCTL_MODE_GETFB2, &fb) == 0) {
			orig_w = fb.width;
			orig_h = fb.height;
			orig_stride = fb.pitches[0];
			orig_bpp = fb.pixel_format == DRM_FORMAT_RGB565 ? 16 : 32;
			if (fb.handles[0]) {
				struct drm_gem_close cl = { .handle = fb.handles[0] };

				kioctl(kms, DRM_IOCTL_GEM_CLOSE, &cl);
			}
		}
		return;
	}
}

int
i915_kms_start(void)
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
	findplane();
	return 0;
}

/*
 * outputs: their names, EDID, modes (probed), what each shows; read
 * from the connectors themselves, under the mode configuration's locks
 */
int
i915_outputs(char **names, int max)
{
	struct drm_device *dev = drmdev();
	struct drm_connector_list_iter it;
	struct drm_connector *c;
	int n = 0;

	if (!dev)
		return 0;
	drm_connector_list_iter_begin(dev, &it);
	drm_for_each_connector_iter(c, &it)
		if (n < max)
			names[n++] = kstrdup(c->name, GFP_KERNEL);
	drm_connector_list_iter_end(&it);
	return n;
}

static struct drm_connector *
findconn(struct drm_device *dev, const char *name)
{
	struct drm_connector_list_iter it;
	struct drm_connector *c, *found = NULL;

	drm_connector_list_iter_begin(dev, &it);
	drm_for_each_connector_iter(c, &it)
		if (strcmp(c->name, name) == 0) {
			drm_connector_get(c);
			found = c;
			break;
		}
	drm_connector_list_iter_end(&it);
	return found;
}

/* the output's EDID into buf; its length */
int
i915_output_edid(char *name, u8 *buf, int n)
{
	struct drm_device *dev = drmdev();
	struct drm_connector *c;
	int len = 0;

	if (!dev || !(c = findconn(dev, name)))
		return -ENOENT;
	mutex_lock(&dev->mode_config.mutex);
	drm_helper_probe_single_connector_modes(c, dev->mode_config.max_width, dev->mode_config.max_height);
	if (c->edid_blob_ptr) {
		len = min_t(int, n, c->edid_blob_ptr->length);
		memcpy(buf, c->edid_blob_ptr->data, len);
	}
	mutex_unlock(&dev->mode_config.mutex);
	drm_connector_put(c);
	return len;
}

/* "WxH Hz" a line, the preferred first; kmalloc'd */
char *
i915_output_modes(char *name)
{
	struct drm_device *dev = drmdev();
	struct drm_display_mode *m;
	struct drm_connector *c;
	char *s, *t;
	int pass;

	if (!dev || !(c = findconn(dev, name)))
		return NULL;
	s = kstrdup("", GFP_KERNEL);
	mutex_lock(&dev->mode_config.mutex);
	drm_helper_probe_single_connector_modes(c, dev->mode_config.max_width, dev->mode_config.max_height);
	for (pass = 0; pass < 2 && s; pass++)
		list_for_each_entry(m, &c->modes, head) {
			if (!!(m->type & DRM_MODE_TYPE_PREFERRED) != (pass == 0))
				continue;
			t = kasprintf(GFP_KERNEL, "%s%dx%d %d\n", s, m->hdisplay, m->vdisplay, drm_mode_vrefresh(m));
			kfree(s);
			s = t;
			if (!s)
				break;
		}
	mutex_unlock(&dev->mode_config.mutex);
	drm_connector_put(c);
	return s;
}

/* what the output shows: "WxH Hz", or "off"; kmalloc'd */
char *
i915_output_mode(char *name)
{
	struct drm_device *dev = drmdev();
	struct drm_connector *c;
	struct drm_crtc *crtc;
	char *s = NULL;

	if (!dev || !(c = findconn(dev, name)))
		return NULL;
	drm_modeset_lock_all(dev);
	crtc = c->state ? c->state->crtc : NULL;
	if (crtc && crtc->state && crtc->state->active)
		s = kasprintf(GFP_KERNEL, "%dx%d %d\n", crtc->state->mode.hdisplay, crtc->state->mode.vdisplay,
			drm_mode_vrefresh(&crtc->state->mode));
	drm_modeset_unlock_all(dev);
	drm_connector_put(c);
	return s ? s : kstrdup("off\n", GFP_KERNEL);
}

/* a handle of one DRM file made a handle of another (PRIME, a dma-buf between) */
int
kms_prime(struct file *from, u32 handle, struct file *to, u32 *out)
{
	struct drm_prime_handle p = { .handle = handle, .flags = DRM_CLOEXEC | DRM_RDWR };
	int r;

	r = kioctl(from, DRM_IOCTL_PRIME_HANDLE_TO_FD, &p);
	if (r)
		return r;
	p.handle = 0;
	r = kioctl(to, DRM_IOCTL_PRIME_FD_TO_HANDLE, &p);
	close_fd(p.fd);
	if (r)
		return r;
	*out = p.handle;
	return 0;
}

/*
 * fb on the CRTC in its present mode, on the connectors it drives, as a
 * full modeset (SETCRTC, returns when the display shows it): the state
 * i915 read from the firmware's mode does not always allow a page flip
 * ("requires full modeset", the flip's atomic check then -EINVAL); on
 * Linux the display server or fbdev sets the mode once at the start
 */
static bool relit;

static int
relight(u32 fb)
{
	struct drm_mode_card_res res = { };
	struct drm_mode_get_connector gc;
	struct drm_mode_get_encoder enc;
	struct drm_mode_crtc c = { .crtc_id = crtc_id };
	u32 conns[16], use[16];
	unsigned int i, n;
	int r;

	r = kioctl(kms, DRM_IOCTL_MODE_GETCRTC, &c);
	if (r)
		return r;
	if (!c.mode_valid)
		return -EINVAL;
	res.connector_id_ptr = (u64)(uintptr_t)conns;
	res.count_connectors = ARRAY_SIZE(conns);
	r = kioctl(kms, DRM_IOCTL_MODE_GETRESOURCES, &res);
	if (r)
		return r;
	n = 0;
	for (i = 0; i < res.count_connectors && i < ARRAY_SIZE(conns); i++) {
		memset(&gc, 0, sizeof(gc));
		gc.connector_id = conns[i];
		if (kioctl(kms, DRM_IOCTL_MODE_GETCONNECTOR, &gc) || !gc.encoder_id)
			continue;
		memset(&enc, 0, sizeof(enc));
		enc.encoder_id = gc.encoder_id;
		if (kioctl(kms, DRM_IOCTL_MODE_GETENCODER, &enc) || enc.crtc_id != crtc_id)
			continue;
		use[n++] = conns[i];
	}
	if (n == 0)
		return -ENODEV;
	c.fb_id = fb;
	c.x = c.y = 0;
	c.set_connectors_ptr = (u64)(uintptr_t)use;
	c.count_connectors = n;
	r = kioctl(kms, DRM_IOCTL_MODE_SETCRTC, &c);
	if (r == 0)
		relit = true;
	return r;
}

/*
 * flip to fb and wait until the display shows it (the event); once, if
 * the CRTC needs a full modeset first, set the mode with fb instead
 */
static int
flip(u32 fb)
{
	struct drm_mode_crtc_page_flip f = { .crtc_id = crtc_id, .fb_id = fb, .flags = DRM_MODE_PAGE_FLIP_EVENT };
	char ev[256];
	loff_t pos = 0;
	int r;

	r = kioctl(kms, DRM_IOCTL_MODE_PAGE_FLIP, &f);
	if (r == -EINVAL && !relit)
		return relight(fb);
	if (r)
		return r;
	r = drm_read(kms, (char __user *)ev, sizeof(ev), &pos);
	return r < 0 ? r : 0;
}

/*
 * show the object handle of the client's file from (x8r8g8b8, w x h,
 * stride bytes a row); handle 0: the firmware's plane again
 */
int
kms_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride)
{
	struct drm_mode_fb_cmd2 fb = { };
	struct drm_gem_close cl = { };
	u32 old;
	int r;

	if (!kms || !crtc_id)
		return -ENODEV;
	mutex_lock(&kmslock);
	old = shown_fb;
	if (handle == 0) {
		if (from && from != shown_by) {
			r = 0;		/* another's on the display */
			goto out;
		}
		r = old ? flip(orig_fb) : 0;
		shown_fb = 0;
		shown_by = NULL;
		goto rm;
	}
	r = kms_prime(from, handle, kms, &fb.handles[0]);
	if (r)
		goto out;
	fb.width = w;
	fb.height = h;
	fb.pixel_format = DRM_FORMAT_XRGB8888;
	fb.pitches[0] = stride;
	r = kioctl(kms, DRM_IOCTL_MODE_ADDFB2, &fb);
	cl.handle = fb.handles[0];
	kioctl(kms, DRM_IOCTL_GEM_CLOSE, &cl);	/* the framebuffer keeps the object */
	if (r)
		goto out;
	r = flip(fb.fb_id);
	if (r) {
		kioctl(kms, DRM_IOCTL_MODE_RMFB, &fb.fb_id);
		goto out;
	}
	shown_fb = fb.fb_id;
	shown_by = from;
rm:
	/* the picture before is no longer on the display */
	if (old && old != shown_fb)
		kioctl(kms, DRM_IOCTL_MODE_RMFB, &old);
out:
	mutex_unlock(&kmslock);
	return r;
}

/* the firmware's plane: its object as a handle of file to, its geometry */
int
kms_plane(struct file *to, u32 *handle, u32 *w, u32 *h, u32 *stride, u32 *bpp)
{
	struct drm_mode_fb_cmd2 fb = { .fb_id = orig_fb };
	struct drm_gem_close cl = { };
	int r;

	if (!kms || !orig_fb)
		return -ENODEV;
	mutex_lock(&kmslock);
	r = kioctl(kms, DRM_IOCTL_MODE_GETFB2, &fb);
	if (r == 0 && !fb.handles[0])
		r = -ENOENT;
	if (r == 0) {
		r = kms_prime(kms, fb.handles[0], to, handle);
		cl.handle = fb.handles[0];
		kioctl(kms, DRM_IOCTL_GEM_CLOSE, &cl);
	}
	mutex_unlock(&kmslock);
	*w = orig_w;
	*h = orig_h;
	*stride = orig_stride;
	*bpp = orig_bpp;
	return r;
}

/* "plane ..." for the device's ctl */
char *
i915_plane(void)
{
	if (!orig_fb)
		return kstrdup("plane none\n", GFP_KERNEL);
	return kasprintf(GFP_KERNEL, "plane A %#x %u %u %u %u%s\n", 0, orig_stride, orig_bpp, orig_w, orig_h,
		shown_fb ? " (a context's on the display)" : "");
}
