// SPDX-License-Identifier: MIT
/*
 * aux/i915, aux/amdgpu: the outputs and the screen, the Linux side,
 * shared (kmsout.h); each driver builds it into its library
 * (linux/kmsout.c includes this file) and gives the screen object.
 *
 * The file server holds the primary node open (a DRM master, as a
 * display server on Linux) and works through the KMS ioctls.  The
 * screen of draw(3) is one object, the bounding box of the outputs
 * that are on; each output shows its part, a CRTC of its own set to the
 * object at the output's x y (SETCRTC), as an extended desktop; two at
 * the same place in the same mode show the same.  At the start every
 * connected output is on, in its preferred mode, side by side in the
 * connectors' order.  A client's picture (gpu(4) scanout) of the
 * screen's size goes to every output (a page flip each, the CRTCs keep
 * their x y); one of an output's size to the first output with a mode
 * of that size (SETCRTC at 0 0, then flips); scanout 0 gives the
 * outputs the screen again.  A monitor connected later (DRM's hotplug
 * event, to an in-kernel client of ours) goes on right of the others,
 * one no longer connected off (not the last one); one turned off by
 * hand stays off while it is connected.  The mouse cursor (vga hwgc
 * gpu) is on every output's cursor plane, a 64 x 64 ARGB object.
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/backlight.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_connector.h>
#include <drm/drm_crtc.h>
#include <drm/drm_device.h>
#include <drm/drm_modes.h>
#include <drm/drm_mode_config.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>
#include <drm/drm_client.h>
#include <drm/drm_gem.h>
#include <drm/drm_plane.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_modeset_lock.h>
#include <linux/iosys-map.h>
#include "kmsout.h"

enum {
	Nout = 16,
	Ncrtc = 8,
	Nmode = 64,
	Nenc = 8,
};

struct out {
	u32	conn;		/* the connector's id */
	char	name[32];
	bool	on;
	struct drm_mode_modeinfo	mode;
	int	x, y;		/* its part of the screen */
	u32	crtc;		/* the CRTC it is lit on, 0 */
	bool	byhand;		/* turned off by hand: hotplug leaves it off while connected */
	bool	connected;	/* when the layout was made */
};

static struct file *kms;
static const struct kmsout_ops *ops;
static DEFINE_MUTEX(kmslock);
static struct out outs[Nout];
static int nout;
static u32 crtcs[Ncrtc];		/* GETRESOURCES' order: possible_crtcs' bits */
static int ncrtc;
static u32 scr_handle, scr_fb, scr_w, scr_h, scr_pitch;
static unsigned long long scr_off;
static u32 shown_fb;			/* a client's picture on the display */
static struct file *shown_by;		/* the client's file */
static int shown_out;			/* the output that shows it, -1: every one */
static struct drm_mode_modeinfo shown_mode;	/* its mode there */
/*
 * an overlay plane per CRTC (GETRESOURCES' order), XRGB8888; a client's
 * window on one of them: two objects of ours in VRAM, the driver copies
 * the client's picture into the one not shown (kmsout_overlay_back),
 * then it is shown (kmsout_overlay_show)
 */
static u32 ovplane[Ncrtc];
static struct file *ov_by;		/* the client's file, NULL: none */
static u32 ov_handle[2], ov_fb[2];	/* ours, in VRAM */
static u32 ov_w, ov_h, ov_pitch;
static int ov_back;			/* the one not shown */
static u32 ov_plane, ov_crtc;		/* where it is, 0: not shown yet */
static int ov_x, ov_y;
static void	ovfree(void);
static char	*ovstate(void);
static struct drm_client_dev client;	/* for DRM's hotplug events */

enum { Curw = 64 };			/* the cursor object's side: every display has 64 */
static DEFINE_MUTEX(curlock);		/* the cursor; never with kmslock after it */
static u32 cur_handle;			/* a dumb buffer, ARGB8888 */
static bool cur_on, cur_loaded;
static int cur_x, cur_y, cur_ox, cur_oy;
static void	curset(void);
static DECLARE_WAIT_QUEUE_HEAD(hotplugq);
static atomic_t hotplugs;

/* a connector's state may have changed (DRM's hotplug event): wake kmsout_hotplug_wait */
static int
clienthotplug(struct drm_client_dev *c)
{
	atomic_inc(&hotplugs);
	wake_up_all(&hotplugq);
	return 0;
}

static const struct drm_client_funcs clientfuncs = {
	.owner = THIS_MODULE,
	.hotplug = clienthotplug,
};

/* the screen as it is (also after a request refused) */
static void
geom(unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff)
{
	*w = scr_w;
	*h = scr_h;
	*pitch = scr_pitch;
	*baroff = scr_off;
}

static int
kioctl(struct file *f, unsigned int cmd, void *arg)
{
	return drm_ioctl(f, cmd, (unsigned long)arg);
}

static struct drm_device *
kmsdev(void)
{
	struct drm_file *fp = kms ? kms->private_data : NULL;

	return fp ? fp->minor->dev : NULL;
}

static struct out *
findout(const char *name)
{
	int i;

	for (i = 0; i < nout; i++)
		if (strcmp(outs[i].name, name) == 0)
			return &outs[i];
	return NULL;
}

static struct drm_connector *
findconn(const char *name)
{
	struct drm_device *dev = kmsdev();
	struct drm_connector_list_iter it;
	struct drm_connector *c, *found = NULL;

	if (!dev)
		return NULL;
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

/*
 * the connector's state, modes and encoders; probe: the output asked
 * first (GETCONNECTOR with no room for modes probes it, the EDID read
 * again; we are the master)
 */
static int
getconn(u32 id, struct drm_mode_get_connector *gc, struct drm_mode_modeinfo *modes, u32 *encs, bool probe)
{
	int r;

	if (probe) {
		memset(gc, 0, sizeof(*gc));
		gc->connector_id = id;
		r = kioctl(kms, DRM_IOCTL_MODE_GETCONNECTOR, gc);
		if (r)
			return r;
	}
	memset(gc, 0, sizeof(*gc));
	gc->connector_id = id;
	gc->modes_ptr = (u64)(uintptr_t)modes;
	gc->count_modes = Nmode;
	gc->encoders_ptr = (u64)(uintptr_t)encs;
	gc->count_encoders = Nenc;
	r = kioctl(kms, DRM_IOCTL_MODE_GETCONNECTOR, gc);
	if (gc->count_modes > Nmode)
		gc->count_modes = Nmode;
	if (gc->count_encoders > Nenc)
		gc->count_encoders = Nenc;
	return r;
}

/* the CRTCs the output's encoders can drive, as possible_crtcs' bits */
static u32
possible(struct out *o)
{
	struct drm_mode_get_connector gc;
	struct drm_mode_get_encoder enc;
	struct drm_mode_modeinfo *modes;
	u32 encs[Nenc], bits = 0;
	unsigned int j;

	modes = kmalloc_array(Nmode, sizeof(*modes), GFP_KERNEL);
	if (!modes)
		return 0;
	if (getconn(o->conn, &gc, modes, encs, false) == 0)
		for (j = 0; j < gc.count_encoders; j++) {
			memset(&enc, 0, sizeof(enc));
			enc.encoder_id = encs[j];
			if (kioctl(kms, DRM_IOCTL_MODE_GETENCODER, &enc) == 0)
				bits |= enc.possible_crtcs;
		}
	kfree(modes);
	return bits;
}

/*
 * the output's mode w x h, at hz if not 0 (else the preferred one of
 * that size, or the fastest); w 0: the preferred one; -ENOENT if none
 */
static int
findmode(struct out *o, u32 w, u32 h, u32 hz, struct drm_mode_modeinfo *m, bool *connected, bool probe)
{
	struct drm_mode_get_connector gc;
	struct drm_mode_modeinfo *modes, *best = NULL;
	u32 encs[Nenc];
	unsigned int j;
	int r;

	modes = kmalloc_array(Nmode, sizeof(*modes), GFP_KERNEL);
	if (!modes)
		return -ENOMEM;
	r = getconn(o->conn, &gc, modes, encs, probe);
	if (connected)
		*connected = r == 0 && gc.connection == connector_status_connected;	/* the uapi value is the enum's */
	if (r == 0)
		for (j = 0; j < gc.count_modes; j++) {
			struct drm_mode_modeinfo *x = &modes[j];
			bool pref = x->type & DRM_MODE_TYPE_PREFERRED;

			if (w == 0) {
				if (pref || !best)
					best = x;
				if (pref)
					break;
				continue;
			}
			if (x->hdisplay != w || x->vdisplay != h)
				continue;
			if (hz) {
				if (x->vrefresh == hz) {
					best = x;
					break;
				}
				continue;
			}
			if (!best || pref || (!(best->type & DRM_MODE_TYPE_PREFERRED) && x->vrefresh > best->vrefresh))
				best = x;
		}
	r = best ? 0 : -ENOENT;
	if (best)
		*m = *best;
	kfree(modes);
	return r;
}

/*
 * the overlay planes (GETPLANERESOURCES lists only them: we did not ask
 * for universal planes), the first for each CRTC that takes XRGB8888
 */
static void
findplanes(void)
{
	struct drm_mode_get_plane_res pr = { };
	struct drm_mode_get_plane gp;
	u32 ids[16], fmts[64];
	unsigned int i, j, k;

	memset(ovplane, 0, sizeof(ovplane));
	pr.plane_id_ptr = (u64)(uintptr_t)ids;
	pr.count_planes = ARRAY_SIZE(ids);
	if (kioctl(kms, DRM_IOCTL_MODE_GETPLANERESOURCES, &pr))
		return;
	for (i = 0; i < min_t(u32, pr.count_planes, ARRAY_SIZE(ids)); i++) {
		memset(&gp, 0, sizeof(gp));
		gp.plane_id = ids[i];
		gp.format_type_ptr = (u64)(uintptr_t)fmts;
		gp.count_format_types = ARRAY_SIZE(fmts);
		if (kioctl(kms, DRM_IOCTL_MODE_GETPLANE, &gp))
			continue;
		for (j = 0; j < min_t(u32, gp.count_format_types, ARRAY_SIZE(fmts)); j++)
			if (fmts[j] == DRM_FORMAT_XRGB8888)
				break;
		if (j == min_t(u32, gp.count_format_types, ARRAY_SIZE(fmts)))
			continue;
		for (k = 0; k < (unsigned)ncrtc; k++)
			if ((gp.possible_crtcs & (1U << k)) && ovplane[k] == 0)
				ovplane[k] = ids[i];
	}
}

int
kmsout_init(struct file *f, const struct kmsout_ops *o)
{
	struct drm_mode_card_res res = { };
	struct drm_connector_list_iter it;
	struct drm_connector *c;
	struct drm_device *dev;
	int r;

	kms = f;
	ops = o;
	shown_out = -1;
	res.crtc_id_ptr = (u64)(uintptr_t)crtcs;
	res.count_crtcs = Ncrtc;
	r = kioctl(kms, DRM_IOCTL_MODE_GETRESOURCES, &res);
	if (r)
		return r;
	ncrtc = min_t(int, res.count_crtcs, Ncrtc);
	findplanes();
	dev = kmsdev();
	nout = 0;
	drm_connector_list_iter_begin(dev, &it);
	drm_for_each_connector_iter(c, &it)
		if (nout < Nout) {
			outs[nout].conn = c->base.id;
			strscpy(outs[nout].name, c->name, sizeof(outs[nout].name));
			nout++;
		}
	drm_connector_list_iter_end(&it);
	r = drm_client_init(dev, &client, "kmsout", &clientfuncs);
	if (r == 0)
		drm_client_register(&client);
	else
		pr_warn("kmsout: no hotplug events: %d\n", r);
	return 0;
}

/* how many outputs can be on at once: the CRTCs, at most the outputs */
int
kmsout_npipes(void)
{
	return min(ncrtc, nout);
}

void
kmsout_free(char *s)
{
	kfree(s);
}

int
kmsout_names(char **names, int max)
{
	int i, n = 0;

	for (i = 0; i < nout && n < max; i++)
		names[n++] = kstrdup(outs[i].name, GFP_KERNEL);
	return n;
}

/* the output's EDID into buf; its length */
int
kmsout_edid(char *name, unsigned char *buf, int n)
{
	struct drm_device *dev = kmsdev();
	struct drm_connector *c;
	int len = 0;

	if (!dev || !(c = findconn(name)))
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

/* "w x h hz" a line, the preferred first; kmalloc'd */
char *
kmsout_modes(char *name)
{
	struct drm_mode_get_connector gc;
	struct drm_mode_modeinfo *modes;
	struct out *o = findout(name);
	u32 encs[Nenc];
	char *s, *t;
	unsigned int j;
	int pass;

	if (!o || !(modes = kmalloc_array(Nmode, sizeof(*modes), GFP_KERNEL)))
		return NULL;
	s = kstrdup("", GFP_KERNEL);
	if (getconn(o->conn, &gc, modes, encs, true) == 0)
		for (pass = 0; pass < 2 && s; pass++)
			for (j = 0; j < gc.count_modes && s; j++) {
				if (!!(modes[j].type & DRM_MODE_TYPE_PREFERRED) != (pass == 0))
					continue;
				t = kasprintf(GFP_KERNEL, "%s%dx%d %d\n", s, modes[j].hdisplay, modes[j].vdisplay, modes[j].vrefresh);
				kfree(s);
				s = t;
			}
	kfree(modes);
	return s;
}

/* what the output shows: "w x h hz at x y", or "off"; kmalloc'd */
char *
kmsout_mode(char *name)
{
	struct out *o = findout(name);
	char *s;

	if (!o)
		return NULL;
	mutex_lock(&kmslock);
	if (o->on && o->crtc)
		s = kasprintf(GFP_KERNEL, "%dx%d %d at %d %d\n", o->mode.hdisplay, o->mode.vdisplay, o->mode.vrefresh, o->x, o->y);
	else
		s = kstrdup("off\n", GFP_KERNEL);
	mutex_unlock(&kmslock);
	return s;
}

/*
 * what the display's cursor plane of the output's CRTC holds (DRM's
 * state, what the driver programmed): " cursor fb N at x y", " cursor
 * off"; kmalloc'd
 */
static char *
curstate(struct out *o)
{
	struct drm_device *dev = kmsdev();
	struct drm_crtc *crtc;
	struct drm_plane_state *ps;
	char *s = NULL;

	if (!o->on || !o->crtc || !(crtc = drm_crtc_find(dev, NULL, o->crtc)) || !crtc->cursor)
		return kstrdup("", GFP_KERNEL);
	drm_modeset_lock(&crtc->cursor->mutex, NULL);
	ps = crtc->cursor->state;
	if (ps && ps->fb && ps->crtc == crtc)
		s = kasprintf(GFP_KERNEL, " cursor fb %u %ux%u at %d %d", ps->fb->base.id, ps->crtc_w, ps->crtc_h, ps->crtc_x, ps->crtc_y);
	else
		s = kstrdup(" cursor off", GFP_KERNEL);
	drm_modeset_unlock(&crtc->cursor->mutex);
	return s;
}

/* "output conn connected|disconnected|unknown [cursor ...]" a line; kmalloc'd */
char *
kmsout_status(void)
{
	struct drm_mode_get_connector gc;
	char *s, *t, *c;
	int i;

	s = kstrdup("", GFP_KERNEL);
	for (i = 0; i < nout && s && kms; i++) {
		memset(&gc, 0, sizeof(gc));
		gc.connector_id = outs[i].conn;
		if (kioctl(kms, DRM_IOCTL_MODE_GETCONNECTOR, &gc))	/* probes */
			continue;
		c = curstate(&outs[i]);
		t = kasprintf(GFP_KERNEL, "%soutput %s %s%s\n", s, outs[i].name,
		    gc.connection == connector_status_connected ? "connected" :
		    gc.connection == connector_status_disconnected ? "disconnected" : "unknown", c ? c : "");
		kfree(c);
		kfree(s);
		s = t;
	}
	if (s && kms) {
		mutex_lock(&kmslock);
		c = ovstate();
		mutex_unlock(&kmslock);
		t = kasprintf(GFP_KERNEL, "%s%s", s, c ? c : "");
		kfree(c);
		kfree(s);
		s = t;
	}
	return s;
}

/* a handle of one DRM file made a handle of another (PRIME, a dma-buf between) */
int
kmsout_prime(struct file *from, u32 handle, struct file *to, u32 *out)
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

/* crtc on fb at x y in mode m for connector conn; fb 0: off */
static int
setcrtc(u32 crtc, u32 fb, int x, int y, struct drm_mode_modeinfo *m, u32 conn)
{
	struct drm_mode_crtc c = { .crtc_id = crtc };

	if (fb) {
		c.fb_id = fb;
		c.x = x;
		c.y = y;
		c.mode = *m;
		c.mode_valid = 1;
		c.set_connectors_ptr = (u64)(uintptr_t)&conn;
		c.count_connectors = 1;
	}
	return kioctl(kms, DRM_IOCTL_MODE_SETCRTC, &c);
}

/*
 * a CRTC each for the outputs that are on: the one they have if they
 * may keep it, else a free one their encoders can drive; -ENOSPC if
 * there are too few
 */
static int
assign(u32 *want)
{
	u32 used = 0, bits;
	int i, k;

	for (i = 0; i < nout; i++) {
		want[i] = 0;
		if (!outs[i].on)
			continue;
		bits = possible(&outs[i]);
		for (k = 0; k < ncrtc; k++)
			if (crtcs[k] == outs[i].crtc && (bits & (1U << k)) && !(used & (1U << k))) {
				want[i] = crtcs[k];
				used |= 1U << k;
			}
	}
	for (i = 0; i < nout; i++) {
		if (!outs[i].on || want[i])
			continue;
		bits = possible(&outs[i]);
		for (k = 0; k < ncrtc && !want[i]; k++)
			if ((bits & (1U << k)) && !(used & (1U << k))) {
				want[i] = crtcs[k];
				used |= 1U << k;
			}
		if (!want[i])
			return -ENOSPC;
	}
	return 0;
}

/* the screen's size of the outputs that are on, moved to start at 0 0 */
static int
bbox(u32 *w, u32 *h)
{
	int i, x0 = INT_MAX, y0 = INT_MAX, x1 = 0, y1 = 0;

	for (i = 0; i < nout; i++)
		if (outs[i].on) {
			x0 = min(x0, outs[i].x);
			y0 = min(y0, outs[i].y);
		}
	if (x0 == INT_MAX)
		return -ENODEV;
	for (i = 0; i < nout; i++)
		if (outs[i].on) {
			outs[i].x -= x0;
			outs[i].y -= y0;
			x1 = max(x1, outs[i].x + outs[i].mode.hdisplay);
			y1 = max(y1, outs[i].y + outs[i].mode.vdisplay);
		}
	*w = x1;
	*h = y1;
	return 0;
}

static void
screenfree(u32 fb, u32 handle)
{
	if (fb)
		kioctl(kms, DRM_IOCTL_MODE_RMFB, &fb);
	if (handle)
		ops->screen_free(kms, handle);
}

/* a screen object w x h and its framebuffer */
static int
screenalloc(u32 w, u32 h, u32 *handle, u32 *fbid, u32 *pitch, unsigned long long *off)
{
	struct drm_mode_fb_cmd2 fb = { };
	int r;

	r = ops->screen_alloc(kms, w, h, handle, pitch, off);
	if (r)
		return r;
	fb.width = w;
	fb.height = h;
	fb.pixel_format = DRM_FORMAT_XRGB8888;
	fb.pitches[0] = *pitch;
	fb.handles[0] = *handle;
	r = kioctl(kms, DRM_IOCTL_MODE_ADDFB2, &fb);
	if (r) {
		ops->screen_free(kms, *handle);
		return r;
	}
	*fbid = fb.fb_id;
	return 0;
}

/*
 * the layout in outs[] onto the display: the screen object of its size
 * (a new one if the size changed: the old goes after, or first if there
 * is no room for both), the CRTCs; the outputs as they were if it fails
 */
static int
apply(unsigned long long maxbytes)
{
	struct drm_device *dev = kmsdev();
	u32 want[Nout], w, h, handle, fb, pitch, ofb = 0, ohandle = 0;
	unsigned long long off;
	int i, k, r;

	r = bbox(&w, &h);
	if (r)
		return r;
	ovfree();	/* a new layout: the client's overlay comes again with its next picture */
	if (w > dev->mode_config.max_width || h > dev->mode_config.max_height || (unsigned long long)w * h * 4 > maxbytes)
		return -E2BIG;
	r = assign(want);
	if (r)
		return r;
	if (!scr_fb || w != scr_w || h != scr_h) {
		r = screenalloc(w, h, &handle, &fb, &pitch, &off);
		if (r == -ENOSPC || r == -ENOMEM) {
			/* VRAM for one: the old screen off and away first */
			for (i = 0; i < nout; i++)
				if (outs[i].crtc)
					setcrtc(outs[i].crtc, 0, 0, 0, NULL, 0);
			screenfree(scr_fb, scr_handle);
			scr_fb = scr_handle = 0;
			for (i = 0; i < nout; i++)
				outs[i].crtc = 0;
			r = screenalloc(w, h, &handle, &fb, &pitch, &off);
		}
		if (r)
			return r;
		ofb = scr_fb;
		ohandle = scr_handle;
		scr_fb = fb;
		scr_handle = handle;
		scr_w = w;
		scr_h = h;
		scr_pitch = pitch;
		scr_off = off;
	}
	/* first off what goes elsewhere or off, then the outputs on */
	for (i = 0; i < nout; i++)
		if (outs[i].crtc && outs[i].crtc != want[i]) {
			setcrtc(outs[i].crtc, 0, 0, 0, NULL, 0);
			outs[i].crtc = 0;
		}
	for (k = 0; k < ncrtc; k++) {
		bool ours = false;

		for (i = 0; i < nout; i++)
			if (want[i] == crtcs[k])
				ours = true;
		if (!ours)
			setcrtc(crtcs[k], 0, 0, 0, NULL, 0);	/* lit before us (firmware, fbdev) */
	}
	for (i = 0; i < nout; i++) {
		if (!outs[i].on)
			continue;
		r = setcrtc(want[i], scr_fb, outs[i].x, outs[i].y, &outs[i].mode, outs[i].conn);
		if (r) {
			pr_err("kmsout: %s: %dx%d at %d %d: %d\n", outs[i].name, outs[i].mode.hdisplay, outs[i].mode.vdisplay,
			    outs[i].x, outs[i].y, r);
			outs[i].on = false;
			continue;
		}
		outs[i].crtc = want[i];
	}
	screenfree(ofb, ohandle);
	/* the cursor on the outputs as they are now */
	mutex_lock(&curlock);
	curset();
	mutex_unlock(&curlock);
	return 0;
}

int
kmsout_screen(unsigned long long maxbytes, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff)
{
	bool conn;
	int i, x = 0, r;

	if (!kms)
		return -ENODEV;
	mutex_lock(&kmslock);
	for (i = 0; i < nout; i++) {
		outs[i].on = false;
		outs[i].byhand = false;
		outs[i].connected = findmode(&outs[i], 0, 0, 0, &outs[i].mode, &conn, true) == 0 && conn;
		if (outs[i].connected) {
			outs[i].on = true;
			outs[i].x = x;
			outs[i].y = 0;
			x += outs[i].mode.hdisplay;
		}
	}
	r = apply(maxbytes);
	if (r == -E2BIG) {
		/* too big for the framebuffer: the first output alone */
		for (i = 0; i < nout; i++)
			if (outs[i].on) {
				for (i++; i < nout; i++)
					outs[i].on = false;
				break;
			}
		r = apply(maxbytes);
	}
	geom(w, h, pitch, baroff);
	mutex_unlock(&kmslock);
	return r;
}

static const char *
errstr(int r)
{
	switch (r) {
	case -ENOENT:	return "no such mode";
	case -E2BIG:	return "screen bigger than its framebuffer or the display engine";
	case -ENOSPC:	return "no display pipe (CRTC) left for the output";
	case -ENODEV:	return "the screen needs an output that is on";
	case -EBUSY:	return "a context's picture is on the display";
	case -EINVAL:	return "the display refuses the mode";
	}
	return "display error";
}

/*
 * "w x h [hz] [at x y]" or "off": the output's mode, its place (without
 * one where it was, or right of the others)
 */
const char *
kmsout_set(char *name, char *spec, unsigned long long maxbytes, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff)
{
	struct out *o = findout(name), save[Nout];
	struct drm_mode_modeinfo m;
	char *s = spec, *e;
	unsigned long mw, mh, hz = 0;
	long x = -1, y = -1;
	int i, r;

	geom(w, h, pitch, baroff);
	if (!o)
		return "no such output";
	while (*s == ' ' || *s == '\t')
		s++;
	if (strncmp(s, "off", 3) == 0) {
		mw = 0;
	} else {
		mw = simple_strtoul(s, &e, 10);
		if (e == s || *e != 'x')
			return "usage: w x h [hz] [at x y], or off";
		s = e + 1;
		mh = simple_strtoul(s, &e, 10);
		if (e == s || mw == 0 || mh == 0)
			return "usage: w x h [hz] [at x y], or off";
		s = e;
		while (*s == ' ' || *s == '\t')
			s++;
		if (*s >= '0' && *s <= '9') {
			hz = simple_strtoul(s, &e, 10);
			s = e;
			while (*s == ' ' || *s == '\t')
				s++;
		}
		if (strncmp(s, "at", 2) == 0) {
			x = simple_strtol(s + 2, &e, 10);
			s = e;
			y = simple_strtol(s, &e, 10);
			if (e == s || x < 0 || y < 0)
				return "usage: w x h [hz] [at x y], or off";
		}
	}
	mutex_lock(&kmslock);
	if (shown_fb) {
		mutex_unlock(&kmslock);
		return errstr(-EBUSY);
	}
	memcpy(save, outs, sizeof(save));
	if (mw == 0) {
		o->on = false;
		o->byhand = true;
	} else {
		o->byhand = false;
		r = findmode(o, mw, mh, hz, &m, NULL, true);
		if (r) {
			mutex_unlock(&kmslock);
			return errstr(r);
		}
		if (x < 0) {
			if (o->on) {
				x = o->x;
				y = o->y;
			} else {
				x = y = 0;
				for (i = 0; i < nout; i++)
					if (outs[i].on && &outs[i] != o)
						x = max_t(long, x, outs[i].x + outs[i].mode.hdisplay);
			}
		}
		o->mode = m;
		o->x = x;
		o->y = y;
		o->on = true;
	}
	r = apply(maxbytes);
	if (r) {
		/* as it was: the CRTCs that went off come on again */
		for (i = 0; i < nout; i++) {
			save[i].crtc = outs[i].crtc;
			if (!outs[i].on)
				save[i].crtc = 0;
		}
		memcpy(outs, save, sizeof(save));
		apply(maxbytes);
	}
	geom(w, h, pitch, baroff);
	mutex_unlock(&kmslock);
	return r ? errstr(r) : NULL;
}

/* the cursor where it is on every output that is on (relative to its part); curlock held */
static void
curmove(void)
{
	struct drm_mode_cursor c;
	int i;

	for (i = 0; i < nout; i++) {
		struct out *o = &outs[i];

		if (!o->on || !o->crtc)
			continue;
		memset(&c, 0, sizeof(c));
		c.flags = DRM_MODE_CURSOR_MOVE;
		c.crtc_id = o->crtc;
		c.x = cur_x + cur_ox - o->x;
		c.y = cur_y + cur_oy - o->y;
		kioctl(kms, DRM_IOCTL_MODE_CURSOR, &c);
	}
}

/* the cursor's object on every output that is on (or off), then where it is; curlock held */
static void
curset(void)
{
	struct drm_mode_cursor c;
	int i, r;

	for (i = 0; i < nout; i++) {
		struct out *o = &outs[i];

		if (!o->on || !o->crtc)
			continue;
		memset(&c, 0, sizeof(c));
		c.flags = DRM_MODE_CURSOR_BO;
		c.crtc_id = o->crtc;
		if (cur_on && cur_loaded) {
			c.handle = cur_handle;
			c.width = Curw;
			c.height = Curw;
		}
		r = kioctl(kms, DRM_IOCTL_MODE_CURSOR, &c);
		if (r)
			pr_warn("kmsout: %s: cursor: %d\n", o->name, r);
	}
	if (cur_on && cur_loaded)
		curmove();
}

/*
 * the cursor's image: Plan 9's (cursor.h: 16 x 16, a bit each of clr,
 * white, and set, black, over it), offset (ox oy) from the mouse's
 * point; 0 or the error
 */
int
kmsout_cursor_image(unsigned char *clr, unsigned char *set, int ox, int oy)
{
	struct drm_mode_create_dumb cd = { .width = Curw, .height = Curw, .bpp = 32 };
	struct drm_gem_object *obj;
	struct iosys_map map;
	u32 *px;
	int x, y, r, b;

	if (!kms)
		return -ENODEV;
	mutex_lock(&curlock);
	if (!cur_handle) {
		r = kioctl(kms, DRM_IOCTL_MODE_CREATE_DUMB, &cd);
		if (r)
			goto out;
		if (cd.pitch != Curw * 4) {
			pr_warn("kmsout: the cursor's pitch is %u\n", cd.pitch);
			r = -EINVAL;
			goto out;
		}
		cur_handle = cd.handle;
	}
	px = kcalloc(Curw * Curw, sizeof(*px), GFP_KERNEL);
	if (!px) {
		r = -ENOMEM;
		goto out;
	}
	for (y = 0; y < 16; y++)
		for (x = 0; x < 16; x++) {
			b = 0x80 >> (x & 7);
			if (set[y * 2 + x / 8] & b)
				px[y * Curw + x] = 0xff000000;
			else if (clr[y * 2 + x / 8] & b)
				px[y * Curw + x] = 0xffffffff;
		}
	obj = drm_gem_object_lookup(kms->private_data, cur_handle);
	r = obj ? drm_gem_vmap(obj, &map) : -ENOENT;
	if (r == 0) {
		iosys_map_memcpy_to(&map, 0, px, Curw * Curw * sizeof(*px));
		drm_gem_vunmap(obj, &map);
	}
	if (obj)
		drm_gem_object_put(obj);
	kfree(px);
	if (r)
		goto out;
	cur_ox = ox;
	cur_oy = oy;
	cur_loaded = true;
	curset();	/* the display reads it again */
out:
	mutex_unlock(&curlock);
	return r;
}

/* the cursor at x y on the screen, shown or not */
void
kmsout_cursor_move(int x, int y, int on)
{
	if (!kms)
		return;
	mutex_lock(&curlock);
	cur_x = x;
	cur_y = y;
	if (!!on != cur_on) {
		cur_on = on;
		curset();
	} else if (cur_on && cur_loaded)
		curmove();
	mutex_unlock(&curlock);
}

/* until a hotplug event after the one *seen; 0, or -EINTR (a note) */
int
kmsout_hotplug_wait(unsigned int *seen)
{
	int r;

	r = wait_event_interruptible(hotplugq, (unsigned int)atomic_read(&hotplugs) != *seen);
	*seen = atomic_read(&hotplugs);
	return r;
}

/*
 * after a hotplug event: an output newly connected on (its preferred
 * mode, right of the others) unless it was turned off by hand, one no
 * longer connected off (not the last one on); nil, or the error (busy:
 * a client's picture is on the display, try again later); *changed if
 * the layout did
 */
const char *
kmsout_relayout(unsigned long long maxbytes, int *changed, unsigned int *w, unsigned int *h, unsigned int *pitch, unsigned long long *baroff)
{
	struct out save[Nout];
	struct drm_mode_modeinfo m;
	bool conn, any;
	int i, j, x, r = 0;

	*changed = 0;
	geom(w, h, pitch, baroff);
	if (!kms || !scr_fb)
		return "no screen";
	mutex_lock(&kmslock);
	if (shown_fb) {
		mutex_unlock(&kmslock);
		return errstr(-EBUSY);
	}
	memcpy(save, outs, sizeof(save));
	for (i = 0; i < nout; i++) {
		struct out *o = &outs[i];

		if (findmode(o, 0, 0, 0, &m, &conn, true) != 0)
			conn = false;
		if (conn == o->connected)
			continue;
		o->connected = conn;
		if (!conn) {
			o->byhand = false;
			if (!o->on)
				continue;
			any = false;
			for (j = 0; j < nout; j++)
				if (j != i && outs[j].on)
					any = true;
			if (!any)
				continue;	/* the last one stays */
			o->on = false;
			pr_info("kmsout: %s disconnected: off\n", o->name);
			*changed = 1;
		} else if (!o->on && !o->byhand) {
			x = 0;
			for (j = 0; j < nout; j++)
				if (outs[j].on)
					x = max(x, outs[j].x + outs[j].mode.hdisplay);
			o->mode = m;
			o->x = x;
			o->y = 0;
			o->on = true;
			pr_info("kmsout: %s connected: %dx%d at %d 0\n", o->name, m.hdisplay, m.vdisplay, x);
			*changed = 1;
		}
	}
	if (*changed) {
		r = apply(maxbytes);
		if (r) {
			/* as it was, but knowing what is connected now */
			for (i = 0; i < nout; i++) {
				bool c = outs[i].connected;

				save[i].crtc = outs[i].on ? outs[i].crtc : 0;
				outs[i] = save[i];
				outs[i].connected = c;
			}
			apply(maxbytes);
			*changed = 0;
		}
	}
	geom(w, h, pitch, baroff);
	mutex_unlock(&kmslock);
	return r ? errstr(r) : NULL;
}

int
kmsout_screenfb(u32 *fb, u32 *w, u32 *h, u32 *pitch)
{
	if (!scr_fb)
		return -ENODEV;
	*fb = scr_fb;
	*w = scr_w;
	*h = scr_h;
	*pitch = scr_pitch;
	return 0;
}

/*
 * page flips of the outputs that are on (or of output only) to fb, and
 * the wait until the display shows them (an event each, a second at
 * most: kmslock is held); a flip the CRTC's state refuses ("requires
 * full modeset", -EINVAL) is a SETCRTC instead
 */
static int
flip(int only, u32 fb)
{
	struct drm_file *fp = kms->private_data;
	struct drm_mode_crtc_page_flip f;
	char ev[1024];
	loff_t pos;
	long t;
	int i, n = 0, r = 0, k, rr;

	for (i = 0; i < nout; i++) {
		struct out *o = &outs[i];

		if (!o->on || !o->crtc || (only >= 0 && i != only))
			continue;
		memset(&f, 0, sizeof(f));
		f.crtc_id = o->crtc;
		f.fb_id = fb;
		f.flags = DRM_MODE_PAGE_FLIP_EVENT;
		rr = kioctl(kms, DRM_IOCTL_MODE_PAGE_FLIP, &f);
		if (rr == -EINVAL)
			rr = only >= 0 ? setcrtc(o->crtc, fb, 0, 0, &shown_mode, o->conn)
			    : setcrtc(o->crtc, fb, o->x, o->y, &o->mode, o->conn);
		else if (rr == 0)
			n++;
		if (rr && !r)
			r = rr;
	}
	while (n > 0) {
		t = wait_event_interruptible_timeout(fp->event_wait, !list_empty(&fp->event_list), HZ);
		if (t <= 0) {
			pr_err("kmsout: no flip event for fb %u within a second (%ld)\n", fb, t);
			return t < 0 ? t : -ETIMEDOUT;
		}
		pos = 0;
		rr = drm_read(kms, (char __user *)ev, sizeof(ev), &pos);
		if (rr < 0)
			return rr;
		for (k = 0; k + (int)sizeof(struct drm_event) <= rr; ) {
			struct drm_event *e = (struct drm_event *)(ev + k);

			if (e->length < sizeof(*e))
				break;
			if (e->type == DRM_EVENT_FLIP_COMPLETE)
				n--;
			k += e->length;
		}
	}
	return r;
}

/* the output with a mode w x h for a client's picture, its mode; -1 */
static int
outfor(u32 w, u32 h, struct drm_mode_modeinfo *m)
{
	int i;

	if (shown_out >= 0 && outs[shown_out].on &&
	    findmode(&outs[shown_out], w, h, 0, m, NULL, false) == 0)
		return shown_out;
	for (i = 0; i < nout; i++)
		if (outs[i].on && outs[i].crtc && outs[i].mode.hdisplay == w && outs[i].mode.vdisplay == h) {
			*m = outs[i].mode;
			return i;
		}
	for (i = 0; i < nout; i++)
		if (outs[i].on && outs[i].crtc && findmode(&outs[i], w, h, 0, m, NULL, false) == 0)
			return i;
	return -1;
}

/* the screen on the output that showed a client's picture again */
static int
back(void)
{
	struct out *o;

	if (shown_out < 0)
		return flip(-1, scr_fb);
	o = &outs[shown_out];
	return setcrtc(o->crtc, scr_fb, o->x, o->y, &o->mode, o->conn);
}

/*
 * show the object handle of the client's file from (x8r8g8b8, w x h,
 * stride bytes a row); handle 0: the screen again
 */
int
kmsout_scanout(struct file *from, u32 handle, u32 w, u32 h, u32 stride)
{
	struct drm_mode_fb_cmd2 fb = { };
	struct drm_gem_close cl = { };
	struct drm_mode_modeinfo m;
	u32 old;
	int r, o;

	if (!kms || !scr_fb)
		return -ENODEV;
	mutex_lock(&kmslock);
	old = shown_fb;
	if (handle == 0) {
		if (from && from != shown_by) {
			r = 0;		/* another's on the display */
			goto out;
		}
		r = old ? back() : 0;
		shown_fb = 0;
		shown_by = NULL;
		shown_out = -1;
		goto rm;
	}
	if (w == scr_w && h == scr_h)
		o = -1;
	else if ((o = outfor(w, h, &m)) < 0) {
		r = -EINVAL;	/* no output has a mode of that size: the client copies */
		goto out;
	}
	r = kmsout_prime(from, handle, kms, &fb.handles[0]);
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
	if (old && shown_out != o)
		back();		/* the last picture was elsewhere */
	if (o < 0)
		r = flip(-1, fb.fb_id);
	else if (old && shown_out == o)
		r = flip(o, fb.fb_id);	/* the output shows the client's at 0 0 already */
	else
		r = setcrtc(outs[o].crtc, fb.fb_id, 0, 0, &m, outs[o].conn);
	if (r) {
		kioctl(kms, DRM_IOCTL_MODE_RMFB, &fb.fb_id);
		if (old) {
			back();
			kioctl(kms, DRM_IOCTL_MODE_RMFB, &old);
		}
		shown_fb = 0;
		shown_by = NULL;
		shown_out = -1;
		goto out;
	}
	shown_fb = fb.fb_id;
	shown_by = from;
	shown_out = o;
	if (o >= 0)
		shown_mode = m;
rm:
	/* the picture before is no longer on the display */
	if (old && old != shown_fb)
		kioctl(kms, DRM_IOCTL_MODE_RMFB, &old);
out:
	mutex_unlock(&kmslock);
	return r;
}

/* the overlay off the display and its objects freed; kmslock held */
static void
ovfree(void)
{
	struct drm_mode_set_plane sp = { };
	int i;

	if (ov_plane) {
		sp.plane_id = ov_plane;
		sp.crtc_id = ov_crtc;
		kioctl(kms, DRM_IOCTL_MODE_SETPLANE, &sp);	/* fb 0: off */
	}
	for (i = 0; i < 2; i++) {
		if (ov_fb[i])
			kioctl(kms, DRM_IOCTL_MODE_RMFB, &ov_fb[i]);
		if (ov_handle[i])
			ops->screen_free(kms, ov_handle[i]);
		ov_fb[i] = ov_handle[i] = 0;
	}
	ov_by = NULL;
	ov_plane = ov_crtc = 0;
	ov_w = ov_h = ov_pitch = 0;
}

/*
 * the object the client's picture (w x h, stride bytes a row, XRGB8888)
 * goes into next: one of ours in VRAM, made (two) when the size changes;
 * the master file and its handle there for the copy.  Another client's
 * overlay stays: -EBUSY.
 */
int
kmsout_overlay_back(struct file *from, u32 w, u32 h, u32 stride, struct file **kmsf, u32 *handle)
{
	struct drm_mode_fb_cmd2 fb;
	unsigned long long off;
	u32 pitch;
	int i, r;

	if (!kms || !scr_fb)
		return -ENODEV;
	if (w == 0 || h == 0 || stride < w * 4 || stride % 256)
		return -EINVAL;	/* the copy is one piece: our pitch is the client's */
	mutex_lock(&kmslock);
	r = 0;
	if (ov_by && ov_by != from) {
		r = -EBUSY;
		goto out;
	}
	if (ov_by != from || ov_w != w || ov_h != h || ov_pitch != stride) {
		ovfree();
		for (i = 0; i < 2; i++) {
			/* screen_alloc's pitch is ALIGN(w*4, 256): the client's stride */
			r = ops->screen_alloc(kms, stride / 4, h, &ov_handle[i], &pitch, &off);
			if (r == 0 && pitch != stride)
				r = -EINVAL;
			if (r == 0) {
				memset(&fb, 0, sizeof(fb));
				fb.width = w;
				fb.height = h;
				fb.pixel_format = DRM_FORMAT_XRGB8888;
				fb.handles[0] = ov_handle[i];
				fb.pitches[0] = stride;
				r = kioctl(kms, DRM_IOCTL_MODE_ADDFB2, &fb);
				ov_fb[i] = fb.fb_id;
			}
			if (r) {
				ovfree();
				goto out;
			}
		}
		ov_by = from;
		ov_w = w;
		ov_h = h;
		ov_pitch = stride;
		ov_back = 0;
	}
	*kmsf = kms;
	*handle = ov_handle[ov_back];
out:
	mutex_unlock(&kmslock);
	return r;
}

/*
 * the back object on the overlay plane of the output whose part of the
 * screen has the window (x y on the screen) wholly, then the other one
 * is the back; returns when the display shows it (the plane's update
 * waits for the vblank).  -ERANGE: no output has it all, or no overlay
 * plane on its CRTC (the client copies).
 */
int
kmsout_overlay_show(struct file *from, int x, int y)
{
	struct drm_mode_set_plane sp = { };
	struct out *o = NULL;
	int i, k, r;

	mutex_lock(&kmslock);
	if (ov_by != from || !ov_fb[ov_back]) {
		r = -EINVAL;
		goto out;
	}
	for (i = 0; i < nout; i++) {
		struct out *p = &outs[i];

		if (p->on && p->crtc && x >= p->x && y >= p->y &&
		    x + (int)ov_w <= p->x + p->mode.hdisplay && y + (int)ov_h <= p->y + p->mode.vdisplay) {
			o = p;
			break;
		}
	}
	for (k = 0; o && k < ncrtc; k++)
		if (crtcs[k] == o->crtc)
			break;
	if (!o || k == ncrtc || !ovplane[k] || (shown_fb && (shown_out < 0 || &outs[shown_out] == o))) {
		r = -ERANGE;
		goto out;
	}
	if (ov_plane && (ov_plane != ovplane[k] || ov_crtc != o->crtc)) {
		/* the window went to another output: off on the old one */
		sp.plane_id = ov_plane;
		sp.crtc_id = ov_crtc;
		kioctl(kms, DRM_IOCTL_MODE_SETPLANE, &sp);
		memset(&sp, 0, sizeof(sp));
	}
	sp.plane_id = ovplane[k];
	sp.crtc_id = o->crtc;
	sp.fb_id = ov_fb[ov_back];
	sp.crtc_x = x - o->x;
	sp.crtc_y = y - o->y;
	sp.crtc_w = ov_w;
	sp.crtc_h = ov_h;
	sp.src_w = ov_w << 16;
	sp.src_h = ov_h << 16;
	r = kioctl(kms, DRM_IOCTL_MODE_SETPLANE, &sp);
	if (r == 0) {
		ov_plane = ovplane[k];
		ov_crtc = o->crtc;
		ov_x = x;
		ov_y = y;
		ov_back ^= 1;
	}
out:
	mutex_unlock(&kmslock);
	return r;
}

/* the client's overlay off (from NULL: whoever's); its objects freed */
void
kmsout_overlay_off(struct file *from)
{
	mutex_lock(&kmslock);
	if (ov_by && (from == NULL || ov_by == from))
		ovfree();
	mutex_unlock(&kmslock);
}

/* "overlay ..." for the device's status */
static char *
ovstate(void)
{
	int k, n = 0;

	for (k = 0; k < ncrtc; k++)
		if (ovplane[k])
			n++;
	if (!ov_plane)
		return kasprintf(GFP_KERNEL, "overlay planes %d, none shown\n", n);
	return kasprintf(GFP_KERNEL, "overlay planes %d, plane %u crtc %u: %ux%u at %d %d\n",
	    n, ov_plane, ov_crtc, ov_w, ov_h, ov_x, ov_y);
}

/* "plane ..." for the device's ctl: the screen */
char *
kmsout_plane(void)
{
	if (!scr_fb)
		return kstrdup("plane none\n", GFP_KERNEL);
	return kasprintf(GFP_KERNEL, "plane A %#x %u %u %u %u%s\n", 0, scr_pitch, 32, scr_w, scr_h,
		shown_fb ? " (a context's on the display)" : "");
}

static struct backlight_device *
findbl(char *name)
{
	struct drm_connector *c;
	struct backlight_device *b;

	if (!ops || !ops->backlight || !(c = findconn(name)))
		return NULL;
	b = ops->backlight(c);
	drm_connector_put(c);
	return b;
}

int
kmsout_hasbacklight(char *name)
{
	return findbl(name) != NULL;
}

/* "brightness max" of the output's backlight; kmalloc'd */
char *
kmsout_backlight(char *name)
{
	struct backlight_device *b = findbl(name);
	int v;

	if (!b)
		return NULL;
	mutex_lock(&b->ops_lock);
	v = b->props.brightness;
	if (b->ops && b->ops->get_brightness)
		v = b->ops->get_brightness(b);
	mutex_unlock(&b->ops_lock);
	return kasprintf(GFP_KERNEL, "%d %d\n", v, b->props.max_brightness);
}

/* "n" (of max) or "n%": the output's brightness; nil or the error */
const char *
kmsout_setbacklight(char *name, char *spec)
{
	struct backlight_device *b = findbl(name);
	unsigned long v;
	char *e;

	if (!b)
		return "no backlight";
	v = simple_strtoul(spec, &e, 10);
	if (e == spec)
		return "usage: brightness, or percent%";
	if (*e == '%') {
		if (v > 100)
			return "more than 100%";
		v = DIV_ROUND_CLOSEST(v * b->props.max_brightness, 100);
	}
	if (v > b->props.max_brightness)
		return "brighter than its maximum";
	return backlight_device_set_brightness(b, v) ? "the backlight refuses" : NULL;
}
