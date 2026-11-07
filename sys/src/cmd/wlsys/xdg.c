/*
 * SPDX-License-Identifier: MIT
 *
 * xdg_wm_base: toplevels are windows of wsys, which draws their borders and
 * moves and sweeps them (as rio); a toplevel's size is its buffer's, or the
 * one swept in wsys (a configure).  Maximizing, fullscreen and minimizing
 * are not offered (wm_capabilities: none); a request for them is answered
 * with the size as it is.  Popups are dismissed at once (popup_done): wsys
 * has no windows without the focus yet.
 */
#include "dat.h"

/* ---- xdg_positioner: kept for nobody ---- */

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static void posint2(struct wl_client *c, struct wl_resource *r, int32_t a, int32_t b) { (void)c; (void)r; (void)a; (void)b; }
static void posrect(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h) { (void)c; (void)r; (void)x; (void)y; (void)w; (void)h; }
static void posuint(struct wl_client *c, struct wl_resource *r, uint32_t a) { (void)c; (void)r; (void)a; }
static void posnone(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }

static const struct xdg_positioner_interface positionerimpl = {
	resdestroy,
	posint2,	/* set_size */
	posrect,	/* set_anchor_rect */
	posuint,	/* set_anchor */
	posuint,	/* set_gravity */
	posuint,	/* set_constraint_adjustment */
	posint2,	/* set_offset */
	posnone,	/* set_reactive */
	posint2,	/* set_parent_size */
	posuint,	/* set_parent_configure */
};

/* ---- xdg_popup ---- */

static void
popupgrab(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t serial)
{
	(void)c; (void)r; (void)seat; (void)serial;
}

static void
popupreposition(struct wl_client *c, struct wl_resource *r, struct wl_resource *pos, uint32_t token)
{
	(void)c; (void)r; (void)pos; (void)token;
}

static const struct xdg_popup_interface popupimpl = {
	resdestroy,
	popupgrab,
	popupreposition,
};

/* ---- xdg_toplevel ---- */

static Surface*
tsurface(struct wl_resource *r)
{
	return wl_resource_get_user_data(r);
}

/* the toplevel's size and states, then the xdg_surface's serial */
void
configure(Surface *s)
{
	struct wl_array states;
	uint32_t *st;
	int w, h;

	if(s->toplevel == NULL || s->xdgsurf == NULL)
		return;
	wl_array_init(&states);
	w = h = 0;	/* the client's choice */
	if(s->win != NULL){
		w = s->win->w;
		h = s->win->h;
		if(s->win->focus && (st = wl_array_add(&states, sizeof *st)) != NULL)
			*st = XDG_TOPLEVEL_STATE_ACTIVATED;
	}
	xdg_toplevel_send_configure(s->toplevel, w, h, &states);
	wl_array_release(&states);
	s->serial = wl_display_next_serial(display);
	xdg_surface_send_configure(s->xdgsurf, s->serial);
	dprint("configure %d %d serial %u\n", w, h, s->serial);
}

void
toplevelclose(Surface *s)
{
	if(s->toplevel != NULL)
		xdg_toplevel_send_close(s->toplevel);
}

static void
topleveldestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = tsurface(r)) == NULL)
		return;
	unshow(s);
	s->toplevel = NULL;
	s->committed = 0;
}

static void
toplevelparent(struct wl_client *c, struct wl_resource *r, struct wl_resource *parent)
{
	(void)c; (void)r; (void)parent;
}

static void
toplevelsettitle(struct wl_client *c, struct wl_resource *r, const char *title)
{
	Surface *s;

	(void)c;
	if((s = tsurface(r)) == NULL)
		return;
	free(s->title);
	s->title = strdup(title);
	if(s->win != NULL)
		winlabel(s->win, s->title);
}

static void
toplevelsetappid(struct wl_client *c, struct wl_resource *r, const char *id)
{
	(void)c; (void)r; (void)id;
}

static void
toplevelmenu(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y)
{
	(void)c; (void)r; (void)seat; (void)serial; (void)x; (void)y;
}

static void
toplevelmove(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t serial)
{
	/* wsys moves windows by their borders */
	(void)c; (void)r; (void)seat; (void)serial;
}

static void
toplevelresize(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t serial, uint32_t edges)
{
	(void)c; (void)r; (void)seat; (void)serial; (void)edges;
}

static void
toplevelsize(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h)
{
	(void)c; (void)r; (void)w; (void)h;
}

/* maximize, fullscreen, minimize: not offered; the size stays */
static void
toplevelstate(struct wl_client *c, struct wl_resource *r)
{
	Surface *s;

	(void)c;
	if((s = tsurface(r)) != NULL && s->committed)
		configure(s);
}

static void
toplevelfullscreen(struct wl_client *c, struct wl_resource *r, struct wl_resource *output)
{
	(void)output;
	toplevelstate(c, r);
}

static void
toplevelminimize(struct wl_client *c, struct wl_resource *r)
{
	(void)c; (void)r;
}

static const struct xdg_toplevel_interface toplevelimpl = {
	resdestroy,
	toplevelparent,
	toplevelsettitle,
	toplevelsetappid,
	toplevelmenu,
	toplevelmove,
	toplevelresize,
	toplevelsize,	/* set_max_size */
	toplevelsize,	/* set_min_size */
	toplevelstate,	/* set_maximized */
	toplevelstate,	/* unset_maximized */
	toplevelfullscreen,
	toplevelstate,	/* unset_fullscreen */
	toplevelminimize,
};

/* ---- xdg_surface ---- */

static void
xdgsurfdestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = wl_resource_get_user_data(r)) != NULL)
		s->xdgsurf = NULL;
}

static void
xdggettoplevel(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	Surface *s;
	struct wl_resource *t;
	struct wl_array caps;

	s = wl_resource_get_user_data(r);
	if(s == NULL)
		return;
	if(s->toplevel != NULL){
		wl_resource_post_error(r, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "a toplevel already");
		return;
	}
	t = wl_resource_create(c, &xdg_toplevel_interface, wl_resource_get_version(r), id);
	if(t == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(t, &toplevelimpl, s, topleveldestroyres);
	s->toplevel = t;
	s->committed = 0;
	if(wl_resource_get_version(t) >= XDG_TOPLEVEL_WM_CAPABILITIES_SINCE_VERSION){
		wl_array_init(&caps);
		xdg_toplevel_send_wm_capabilities(t, &caps);
		wl_array_release(&caps);
	}
}

static void
xdggetpopup(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *parent, struct wl_resource *pos)
{
	struct wl_resource *p;

	(void)parent;
	(void)pos;
	p = wl_resource_create(c, &xdg_popup_interface, wl_resource_get_version(r), id);
	if(p == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p, &popupimpl, NULL, NULL);
	xdg_popup_send_popup_done(p);
}

static void
xdggeometry(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h)
{
	(void)c; (void)r; (void)x; (void)y; (void)w; (void)h;
}

static void
xdgack(struct wl_client *c, struct wl_resource *r, uint32_t serial)
{
	Surface *s;

	(void)c;
	if((s = wl_resource_get_user_data(r)) != NULL)
		s->acked = serial;
}

static const struct xdg_surface_interface xdgsurfimpl = {
	resdestroy,
	xdggettoplevel,
	xdggetpopup,
	xdggeometry,
	xdgack,
};

/* ---- xdg_wm_base ---- */

static void
wmcreatepositioner(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	struct wl_resource *p;

	p = wl_resource_create(c, &xdg_positioner_interface, wl_resource_get_version(r), id);
	if(p == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p, &positionerimpl, NULL, NULL);
}

static void
wmgetxdgsurface(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf)
{
	Surface *s;
	struct wl_resource *x;

	s = wl_resource_get_user_data(surf);
	if(s->xdgsurf != NULL){
		wl_resource_post_error(r, XDG_WM_BASE_ERROR_ROLE, "the surface has an xdg_surface");
		return;
	}
	x = wl_resource_create(c, &xdg_surface_interface, wl_resource_get_version(r), id);
	if(x == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(x, &xdgsurfimpl, s, xdgsurfdestroyres);
	s->xdgsurf = x;
}

static void
wmpong(struct wl_client *c, struct wl_resource *r, uint32_t serial)
{
	(void)c; (void)r; (void)serial;
}

static const struct xdg_wm_base_interface wmimpl = {
	resdestroy,
	wmcreatepositioner,
	wmgetxdgsurface,
	wmpong,
};

static void
wmbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &xdg_wm_base_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &wmimpl, NULL, NULL);
}

void
xdginit(void)
{
	wl_global_create(display, &xdg_wm_base_interface, 5, NULL, wmbind);
}
