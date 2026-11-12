/*
 * SPDX-License-Identifier: MIT
 *
 * HiDPI (../9front-ui PLAN stage 8): wsys' scale (its theme's "scale"),
 * what clients are told of it (wl_output's integer scale,
 * wp_fractional_scale_v1's preferred scale) and wp_viewporter's
 * destination size.  wsys shows a buffer at its pixels (it does not
 * resample), so a window is as big as its buffer; a surface's tree has the
 * scale of its root: buffer pixels per surface unit (the buffer over the
 * viewport's destination, else buffer_scale, else 1).  By it wlsys turns
 * surface coordinates into the window's pixels (positions, regions,
 * damage) and back (the pointer, configure).  A client that knows no scale
 * draws at 1: sharp, but small (as draw(3) programs with a bitmap font).
 */
#include "dat.h"
#include <math.h>
#include <fractional-scale-v1-server-protocol.h>
#include <viewporter-server-protocol.h>

double wscale = 1;
static struct wl_list fracs;	/* the wp_fractional_scale_v1s, told when it changes */

/* wsys' scale: its theme's line "scale [auto] s"; 1 without one */
double
readscale(void)
{
	char buf[4096], *l, *e, *p;
	int fd, n;
	double s, v;

	s = 1;
	if((fd = open("/mnt/wsys/theme", O_RDONLY)) < 0)
		return s;
	n = read(fd, buf, sizeof buf - 1);
	close(fd);
	if(n <= 0)
		return s;
	buf[n] = 0;
	for(l = buf; l != NULL && *l != 0; l = e){
		if((e = strchr(l, '\n')) != NULL)
			*e++ = 0;
		if(strncmp(l, "scale ", 6) != 0 || (p = strrchr(l, ' ')) == NULL)
			continue;
		v = strtod(p + 1, NULL);
		if(v >= 0.5 && v <= 8)
			s = v;
	}
	return s;
}

/* wl_output's scale: wsys' rounded, at least 1 */
int
outscale(void)
{
	int s;

	s = (int)floor(wscale + 0.5);
	return s < 1 ? 1 : s;
}

static Surface*
treeroot(Surface *s)
{
	while(s->sparent != NULL)
		s = s->sparent;
	return s;
}

/* a surface's own scale, for buffer b: b over the viewport's destination, else buffer_scale */
static double
ownscale(Surface *s, Buffer *b, int vw, int bscale)
{
	if(b != NULL && vw > 0)
		return (double)b->w / vw;
	return bscale > 0 ? bscale : 1;
}

/* the window's pixels per surface unit of s's tree */
double
scaleof(Surface *s)
{
	s = treeroot(s);
	return ownscale(s, s->buf, s->vw, s->bscale);
}

/* the scale a toplevel's client draws at next (its configure's): wsys' if it takes fractional ones */
double
wantscale(Surface *s)
{
	s = treeroot(s);
	if(s->frac != NULL)
		return wscale;
	return s->bscale > 0 ? s->bscale : 1;
}

/* v surface units of s's tree as the window's pixels */
int
topx(Surface *s, double v)
{
	return (int)floor(v * scaleof(s) + 0.5);
}

/*
 * a commit's scale: buffer_scale and the viewport's destination applied,
 * damage given in surface units made the buffer's
 */
void
scalecommit(Surface *s)
{
	Buffer *b;
	double sc;
	Rect d;

	s->bscale = s->pscale;
	s->vw = s->pvw;
	s->vh = s->pvh;
	d = s->psdamage;
	s->psdamage = (Rect){0, 0, 0, 0};
	if(d.x0 >= d.x1 || d.y0 >= d.y1)
		return;
	b = s->pattached ? s->pbuf : s->buf;
	sc = ownscale(s, b, s->vw, s->bscale);
	rectadd(&s->pdamage, (int)floor(d.x0 * sc), (int)floor(d.y0 * sc), (int)ceil(d.x1 * sc), (int)ceil(d.y1 * sc));
}

/* the surface is going: its viewport's and fractional scale's objects stay inert */
void
scalegone(Surface *s)
{
	if(s->viewport != NULL)
		wl_resource_set_user_data(s->viewport, NULL);
	if(s->frac != NULL){
		wl_resource_set_user_data(s->frac, NULL);
		wl_list_remove(wl_resource_get_link(s->frac));
		wl_list_init(wl_resource_get_link(s->frac));
	}
	s->viewport = s->frac = NULL;
}

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/* ---- wp_fractional_scale_v1 ---- */

static const struct wp_fractional_scale_v1_interface fracimpl = {
	resdestroy,
};

static void
fracdestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = wl_resource_get_user_data(r)) != NULL)
		s->frac = NULL;
	wl_list_remove(wl_resource_get_link(r));
}

static void
fracget(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf)
{
	struct wl_resource *f;
	Surface *s;

	s = wl_resource_get_user_data(surf);
	if(s->frac != NULL){
		wl_resource_post_error(r, WP_FRACTIONAL_SCALE_MANAGER_V1_ERROR_FRACTIONAL_SCALE_EXISTS, "the surface has a fractional scale");
		return;
	}
	if((f = wl_resource_create(c, &wp_fractional_scale_v1_interface, wl_resource_get_version(r), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(f, &fracimpl, s, fracdestroyres);
	wl_list_insert(&fracs, wl_resource_get_link(f));
	s->frac = f;
	wp_fractional_scale_v1_send_preferred_scale(f, (uint32_t)floor(wscale * 120 + 0.5));
}

static const struct wp_fractional_scale_manager_v1_interface fracmgrimpl = {
	resdestroy,
	fracget,
};

static void
fracbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wp_fractional_scale_manager_v1_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &fracmgrimpl, NULL, NULL);
}

/* ---- wp_viewporter ---- */

static void
viewdestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = wl_resource_get_user_data(r)) != NULL){
		/* the next commit has no destination */
		s->pvw = s->pvh = -1;
		s->viewport = NULL;
	}
}

/*
 * the source rectangle: wsys shows the whole buffer (no cropping, no
 * resampling), so only none or all of it is taken
 */
static void
viewsource(struct wl_client *c, struct wl_resource *r, wl_fixed_t x, wl_fixed_t y, wl_fixed_t w, wl_fixed_t h)
{
	(void)c;
	if(wl_resource_get_user_data(r) == NULL){
		wl_resource_post_error(r, WP_VIEWPORT_ERROR_NO_SURFACE, "the surface is gone");
		return;
	}
	if(!(x == wl_fixed_from_int(-1) && y == x && w == x && h == x) && (x < 0 || y < 0 || w <= 0 || h <= 0))
		wl_resource_post_error(r, WP_VIEWPORT_ERROR_BAD_VALUE, "bad source rectangle");
}

static void
viewdest(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h)
{
	Surface *s;

	(void)c;
	if((s = wl_resource_get_user_data(r)) == NULL){
		wl_resource_post_error(r, WP_VIEWPORT_ERROR_NO_SURFACE, "the surface is gone");
		return;
	}
	if(!(w == -1 && h == -1) && (w <= 0 || h <= 0)){
		wl_resource_post_error(r, WP_VIEWPORT_ERROR_BAD_VALUE, "bad destination size");
		return;
	}
	s->pvw = w;
	s->pvh = h;
}

static const struct wp_viewport_interface viewimpl = {
	resdestroy,
	viewsource,
	viewdest,
};

static void
viewget(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf)
{
	struct wl_resource *v;
	Surface *s;

	s = wl_resource_get_user_data(surf);
	if(s->viewport != NULL){
		wl_resource_post_error(r, WP_VIEWPORTER_ERROR_VIEWPORT_EXISTS, "the surface has a viewport");
		return;
	}
	if((v = wl_resource_create(c, &wp_viewport_interface, wl_resource_get_version(r), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(v, &viewimpl, s, viewdestroyres);
	s->viewport = v;
}

static const struct wp_viewporter_interface viewporterimpl = {
	resdestroy,
	viewget,
};

static void
viewbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wp_viewporter_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &viewporterimpl, NULL, NULL);
}

/*
 * wsys' scale again (a window said it changed: its theme may have): if it
 * did, the clients are told (the outputs' scale, the preferred fractional
 * scales) and the toplevels configured at the size their new scale makes
 */
void
scalecheck(void)
{
	struct wl_resource *r;
	Surface *s;
	double sc;

	sc = readscale();
	if(sc == wscale)
		return;
	dprint("scale %g, was %g\n", sc, wscale);
	wscale = sc;
	outputscale();
	wl_resource_for_each(r, &fracs)
		wp_fractional_scale_v1_send_preferred_scale(r, (uint32_t)floor(wscale * 120 + 0.5));
	wl_list_for_each(s, &surfaces, link)
		if(s->toplevel != NULL && s->frac != NULL)
			configure(s);
}

void
scaleinit(void)
{
	wl_list_init(&fracs);
	wscale = readscale();
	wl_global_create(display, &wp_fractional_scale_manager_v1_interface, 1, NULL, fracbind);
	wl_global_create(display, &wp_viewporter_interface, 1, NULL, viewbind);
}
