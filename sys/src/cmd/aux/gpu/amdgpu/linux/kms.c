// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: the display, the Linux side, as aux/i915's (its kms.c):
 * the file server holds the primary node open (a DRM master, as a
 * display server on Linux) and works through the KMS ioctls: the
 * outputs and their modes, a client object shown with ADDFB2 (the
 * object comes over by PRIME, a dma-buf, from the client's render node
 * file).  Unlike i915 behind the firmware's plane, nothing may be lit
 * when the driver starts (no fbdev here): the first picture sets the
 * mode on a connected output (SETCRTC), the next ones are page flips;
 * scanout 0 turns the display off again (or back to a plane that was
 * lit).  A display engine of a discrete Radeon scans out VRAM only: the
 * objects are VRAM ones (bo size vram).
 *
 * The screen of draw(3) (amdgpu_screen_start): a VRAM object of the
 * mode's size, in one piece where the processor sees it through BAR 0,
 * on a connected output; the Plan 9 side (screen.c) copies into it what
 * devdraw changes in #G's framebuffer.  It is the plane scanout 0 goes
 * back to.
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
#include "amdgpu.h"
#include "amdgpu_object.h"
#include "amdgpu_gem.h"
#include "amdgpu_res_cursor.h"
#include "../amdgpu.h"

struct pci_dev	*kpi_pci_gpu(const char *gdir);
int	kms_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride);

static struct file *kms;		/* the primary node, master */
static DEFINE_MUTEX(kmslock);
static u32 crtc_id, orig_fb;		/* a plane lit before us: what scanout 0 shows */
static u32 orig_w, orig_h, orig_stride, orig_bpp;
static u32 lit_crtc, lit_conn;		/* where we set a mode: what scanout 0 turns off */
static u32 lit_w, lit_h;
static u32 shown_fb;			/* a client's, on the display */
static struct file *shown_by;		/* the client's file */

static const struct file_operations kms_fops = { };

static struct drm_device *
drmdev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);

	return p ? pci_get_drvdata(p) : NULL;
}

static int
kioctl(struct file *f, unsigned int cmd, void *arg)
{
	return drm_ioctl(f, cmd, (unsigned long)arg);
}

/* the first active CRTC and its framebuffer, if something is lit */
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
	findplane();
	return 0;
}

/*
 * outputs: their names, EDID, modes (probed), what each shows; read
 * from the connectors themselves, under the mode configuration's locks
 */
int
amdgpu_outputs(char **names, int max)
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
amdgpu_output_edid(char *name, u8 *buf, int n)
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
amdgpu_output_modes(char *name)
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
amdgpu_output_mode(char *name)
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

/* "conn connected|disconnected" a line, for status; kmalloc'd */
char *
amdgpu_outputs_status(void)
{
	struct drm_device *dev = drmdev();
	struct drm_connector_list_iter it;
	struct drm_connector *c;
	char *s, *t;

	if (!dev)
		return NULL;
	s = kstrdup("", GFP_KERNEL);
	mutex_lock(&dev->mode_config.mutex);
	drm_connector_list_iter_begin(dev, &it);
	drm_for_each_connector_iter(c, &it) {
		drm_helper_probe_single_connector_modes(c, dev->mode_config.max_width, dev->mode_config.max_height);
		t = kasprintf(GFP_KERNEL, "%soutput %s %s\n", s ? s : "", c->name,
		    c->status == connector_status_connected ? "connected" :
		    c->status == connector_status_disconnected ? "disconnected" : "unknown");
		kfree(s);
		s = t;
	}
	drm_connector_list_iter_end(&it);
	mutex_unlock(&dev->mode_config.mutex);
	return s;
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

/* flip crtc to fb and wait until the display shows it (the event) */
static int
flip(u32 crtc, u32 fb)
{
	struct drm_mode_crtc_page_flip f = { .crtc_id = crtc, .fb_id = fb, .flags = DRM_MODE_PAGE_FLIP_EVENT };
	char ev[256];
	loff_t pos = 0;
	int r;

	r = kioctl(kms, DRM_IOCTL_MODE_PAGE_FLIP, &f);
	if (r)
		return r;
	r = drm_read(kms, (char __user *)ev, sizeof(ev), &pos);
	return r < 0 ? r : 0;
}

/*
 * light a connected output with fb, w x h: its mode of that size (the
 * preferred one first), a CRTC its encoder can drive; SETCRTC returns
 * when the display shows it
 */
static int
light(u32 fb, u32 w, u32 h)
{
	struct drm_mode_card_res res = { };
	struct drm_mode_get_connector gc;
	struct drm_mode_get_encoder enc;
	struct drm_mode_modeinfo modes[64], *m;
	struct drm_mode_crtc set;
	u32 conns[16], crtcs[8], encs[8];
	unsigned int i, j;
	int r;

	res.connector_id_ptr = (u64)(uintptr_t)conns;
	res.count_connectors = ARRAY_SIZE(conns);
	res.crtc_id_ptr = (u64)(uintptr_t)crtcs;
	res.count_crtcs = ARRAY_SIZE(crtcs);
	r = kioctl(kms, DRM_IOCTL_MODE_GETRESOURCES, &res);
	if (r)
		return r;
	for (i = 0; i < res.count_connectors && i < ARRAY_SIZE(conns); i++) {
		memset(&gc, 0, sizeof(gc));
		gc.connector_id = conns[i];
		gc.modes_ptr = (u64)(uintptr_t)modes;
		gc.count_modes = ARRAY_SIZE(modes);
		gc.encoders_ptr = (u64)(uintptr_t)encs;
		gc.count_encoders = ARRAY_SIZE(encs);
		if (kioctl(kms, DRM_IOCTL_MODE_GETCONNECTOR, &gc) || gc.connection != connector_status_connected)	/* the uapi value is the enum's: 1 */
			continue;
		/* its mode of the picture's size, the preferred one if that is it */
		m = NULL;
		for (j = 0; j < gc.count_modes && j < ARRAY_SIZE(modes); j++)
			if (modes[j].hdisplay == w && modes[j].vdisplay == h &&
			    (!m || (modes[j].type & DRM_MODE_TYPE_PREFERRED)))
				m = &modes[j];
		if (!m)
			continue;
		/* a CRTC one of its encoders can drive */
		for (j = 0; j < gc.count_encoders && j < ARRAY_SIZE(encs); j++) {
			unsigned int k;

			memset(&enc, 0, sizeof(enc));
			enc.encoder_id = encs[j];
			if (kioctl(kms, DRM_IOCTL_MODE_GETENCODER, &enc))
				continue;
			for (k = 0; k < res.count_crtcs && k < ARRAY_SIZE(crtcs); k++) {
				if (!(enc.possible_crtcs & (1U << k)))
					continue;
				memset(&set, 0, sizeof(set));
				set.crtc_id = crtcs[k];
				set.fb_id = fb;
				set.set_connectors_ptr = (u64)(uintptr_t)&conns[i];
				set.count_connectors = 1;
				set.mode = *m;
				set.mode_valid = 1;
				r = kioctl(kms, DRM_IOCTL_MODE_SETCRTC, &set);
				if (r == 0) {
					lit_crtc = crtcs[k];
					lit_conn = conns[i];
					lit_w = w;
					lit_h = h;
				}
				return r;
			}
		}
	}
	return -ENODEV;	/* no connected output has a mode of that size */
}

/* the output we lit off again */
static int
unlight(void)
{
	struct drm_mode_crtc set = { .crtc_id = lit_crtc };
	int r;

	r = kioctl(kms, DRM_IOCTL_MODE_SETCRTC, &set);
	lit_crtc = lit_conn = 0;
	return r;
}

/*
 * show the object handle of the client's file from (x8r8g8b8, w x h,
 * stride bytes a row); handle 0: the display as it was (off, or the
 * plane lit before us)
 */
int
kms_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride)
{
	struct drm_mode_fb_cmd2 fb = { };
	struct drm_gem_close cl = { };
	u32 old;
	int r;

	if (!kms)
		return -ENODEV;
	mutex_lock(&kmslock);
	old = shown_fb;
	if (handle == 0) {
		if (from && from != shown_by) {
			r = 0;		/* another's on the display */
			goto out;
		}
		r = 0;
		if (old && orig_fb && lit_crtc) {
			/* a mode of the client's: the screen's again */
			r = light(orig_fb, orig_w, orig_h);
			if (r == 0) {
				crtc_id = lit_crtc;
				lit_crtc = 0;
			}
		} else if (old && orig_fb)
			r = flip(crtc_id, orig_fb);
		else if (old && lit_crtc)
			r = unlight();
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
	if (lit_crtc && lit_w == w && lit_h == h)
		r = flip(lit_crtc, fb.fb_id);
	else if (!lit_crtc && orig_fb && orig_w == w && orig_h == h)
		r = flip(crtc_id, fb.fb_id);
	else
		r = light(fb.fb_id, w, h);	/* a mode of that size */
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

/*
 * the screen: a VRAM object w x h (x8r8g8b8) on a connected output;
 * where it is in BAR 0 and its pitch, for the copies
 */
int
amdgpu_screen_start(unsigned int w, unsigned int h, unsigned long long *baroff, unsigned int *pitch)
{
	struct drm_device *dev = drmdev();
	struct amdgpu_device *adev;
	struct drm_gem_object *gobj;
	struct drm_mode_fb_cmd2 fb = { };
	struct amdgpu_res_cursor cur;
	struct amdgpu_bo *abo;
	u64 size, next;
	u32 handle;
	int r;

	if (!dev || !kms)
		return -ENODEV;
	adev = drm_to_adev(dev);
	*pitch = ALIGN(w * 4, 256);	/* DCN: pitches of 256 bytes */
	size = PAGE_ALIGN((u64)*pitch * h);
	r = amdgpu_gem_object_create(adev, size, PAGE_SIZE, AMDGPU_GEM_DOMAIN_VRAM,
	    AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED | AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS,
	    ttm_bo_type_device, NULL, &gobj, 0);
	if (r)
		return r;
	r = drm_gem_handle_create(kms->private_data, gobj, &handle);
	abo = gem_to_amdgpu_bo(gobj);
	if (r)
		goto put;
	r = amdgpu_bo_reserve(abo, false);
	if (r)
		goto put;
	r = amdgpu_bo_pin(abo, AMDGPU_GEM_DOMAIN_VRAM);	/* for good: the screen stays */
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
		goto put;
	}
	fb.width = w;
	fb.height = h;
	fb.pixel_format = DRM_FORMAT_XRGB8888;
	fb.pitches[0] = *pitch;
	fb.handles[0] = handle;
	mutex_lock(&kmslock);
	r = kioctl(kms, DRM_IOCTL_MODE_ADDFB2, &fb);
	if (r == 0 && (r = light(fb.fb_id, w, h)) == 0) {
		/* the plane scanout 0 comes back to */
		crtc_id = lit_crtc;
		orig_fb = fb.fb_id;
		orig_w = w;
		orig_h = h;
		orig_stride = *pitch;
		orig_bpp = 32;
		lit_crtc = 0;
	}
	mutex_unlock(&kmslock);
put:
	drm_gem_object_put(gobj);	/* the handle (and the framebuffer) hold it */
	return r;
}

/* "plane ..." for the device's ctl */
char *
amdgpu_plane(void)
{
	if (lit_crtc)
		return kasprintf(GFP_KERNEL, "plane %ux%u (a context's on the display)\n", lit_w, lit_h);
	if (!orig_fb)
		return kstrdup("plane none\n", GFP_KERNEL);
	return kasprintf(GFP_KERNEL, "plane A %#x %u %u %u %u%s\n", 0, orig_stride, orig_bpp, orig_w, orig_h,
		shown_fb ? " (a context's on the display)" : "");
}
