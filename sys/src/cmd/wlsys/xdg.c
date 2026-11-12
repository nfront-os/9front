/*
 * SPDX-License-Identifier: MIT
 *
 * xdg_wm_base: toplevels are windows of wsys, which draws their borders and
 * moves and sweeps them (as rio); a toplevel's size is its buffer's, or the
 * one swept in wsys (a configure).  Maximizing, fullscreen and minimizing
 * are not offered (wm_capabilities: none); a request for them is answered
 * with the size as it is.  A popup is a window of wsys without a border
 * (ctl's popup), where its positioner puts it by its parent (flipped and
 * slid onto the screen as the rules allow); one that grabs (a menu) takes
 * the focus, a tooltip leaves it.  Once the focus settled, a popup not on
 * the way to it (nor a tooltip of one that is) is done: a click elsewhere
 * closes a menu, as a grab's.
 */
#include "dat.h"

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/* ---- xdg_positioner: a popup's rules, copied when it is made ---- */

static Pos*
pos(struct wl_resource *r)
{
	return wl_resource_get_user_data(r);
}

static void
posdestroyres(struct wl_resource *r)
{
	free(pos(r));
}

static void
possize(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h)
{
	(void)c;
	if(w <= 0 || h <= 0){
		wl_resource_post_error(r, XDG_POSITIONER_ERROR_INVALID_INPUT, "size %d %d", w, h);
		return;
	}
	pos(r)->w = w;
	pos(r)->h = h;
}

static void
posanchorrect(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h)
{
	Pos *p;

	(void)c;
	if(w < 0 || h < 0){
		wl_resource_post_error(r, XDG_POSITIONER_ERROR_INVALID_INPUT, "anchor rectangle %d %d", w, h);
		return;
	}
	p = pos(r);
	p->ax = x;
	p->ay = y;
	p->aw = w;
	p->ah = h;
}

static void posanchor(struct wl_client *c, struct wl_resource *r, uint32_t a) { (void)c; pos(r)->anchor = a; }
static void posgravity(struct wl_client *c, struct wl_resource *r, uint32_t g) { (void)c; pos(r)->gravity = g; }
static void posadjust(struct wl_client *c, struct wl_resource *r, uint32_t a) { (void)c; pos(r)->adjust = a; }

static void
posoffset(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y)
{
	(void)c;
	pos(r)->ox = x;
	pos(r)->oy = y;
}

/* reactive, the parent's size and configure: wsys' windows do not move under a popup */
static void posnone(struct wl_client *c, struct wl_resource *r) { (void)c; (void)r; }
static void posint2(struct wl_client *c, struct wl_resource *r, int32_t a, int32_t b) { (void)c; (void)r; (void)a; (void)b; }
static void posuint(struct wl_client *c, struct wl_resource *r, uint32_t a) { (void)c; (void)r; (void)a; }

static const struct xdg_positioner_interface positionerimpl = {
	resdestroy,
	possize,
	posanchorrect,
	posanchor,
	posgravity,
	posadjust,
	posoffset,
	posnone,	/* set_reactive */
	posint2,	/* set_parent_size */
	posuint,	/* set_parent_configure */
};

/* ---- placing a popup ---- */

/* the sides of an anchor or a gravity (the enums are the same) */
static int isleft(uint32_t a) { return a == XDG_POSITIONER_ANCHOR_LEFT || a == XDG_POSITIONER_ANCHOR_TOP_LEFT || a == XDG_POSITIONER_ANCHOR_BOTTOM_LEFT; }
static int isright(uint32_t a) { return a == XDG_POSITIONER_ANCHOR_RIGHT || a == XDG_POSITIONER_ANCHOR_TOP_RIGHT || a == XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT; }
static int istop(uint32_t a) { return a == XDG_POSITIONER_ANCHOR_TOP || a == XDG_POSITIONER_ANCHOR_TOP_LEFT || a == XDG_POSITIONER_ANCHOR_TOP_RIGHT; }
static int isbottom(uint32_t a) { return a == XDG_POSITIONER_ANCHOR_BOTTOM || a == XDG_POSITIONER_ANCHOR_BOTTOM_LEFT || a == XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT; }

static uint32_t
flipx(uint32_t a)
{
	static uint32_t f[] = {0, 1, 2, 4, 3, 7, 8, 5, 6};

	return a < 9 ? f[a] : a;
}

static uint32_t
flipy(uint32_t a)
{
	static uint32_t f[] = {0, 2, 1, 3, 4, 6, 5, 8, 7};

	return a < 9 ? f[a] : a;
}

/* where p puts the popup in one axis: the anchor's point, then the gravity's side */
static int
place1(int a0, int alen, int lo, int hi, int len, int off)
{
	int x;

	x = lo ? a0 : hi ? a0 + alen : a0 + alen/2;
	return x + off;
}

static int
grav1(int x, int lo, int hi, int len)
{
	return lo ? x - len : hi ? x : x - len/2;
}

static void
place(Pos *p, uint32_t anchor, uint32_t gravity, int dx, int dy, int *x, int *y)
{
	*x = grav1(place1(p->ax, p->aw, isleft(anchor), isright(anchor), p->w, dx), isleft(gravity), isright(gravity), p->w);
	*y = grav1(place1(p->ay, p->ah, istop(anchor), isbottom(anchor), p->h, dy), istop(gravity), isbottom(gravity), p->h);
}

/* the parent's window geometry's origin on the screen; -1 if it is not shown */
static int
parentorigin(Surface *s, int *x, int *y)
{
	Rect r;

	if(s->parent == NULL || s->parent->win == NULL || winrect(s->parent->win, &r) < 0)
		return -1;
	*x = r.x0 + s->parent->gx;
	*y = r.y0 + s->parent->gy;
	return 0;
}

static int
outside(int v, int len, int lo, int hi)
{
	return v < lo || v + len > hi;
}

/*
 * the popup's place by its rules (in the parent's window geometry): flipped
 * if it would leave the screen and may be (and fits better then), then slid
 * onto it if it may be
 */
static void
popupplace(Surface *s)
{
	Pos *p, q;
	double sc;
	int x, y, fx, fy, ox, oy, sx0, sy0, sx1, sy1;

	/* the rules in the window's pixels: the parent's surface units by its scale (scale.c) */
	q = s->pos;
	if(s->parent != NULL){
		sc = scaleof(s->parent);
		q.w = q.w * sc + 0.5; q.h = q.h * sc + 0.5;
		q.ax = q.ax * sc + 0.5; q.ay = q.ay * sc + 0.5;
		q.aw = q.aw * sc + 0.5; q.ah = q.ah * sc + 0.5;
		q.ox = q.ox * sc + 0.5; q.oy = q.oy * sc + 0.5;
	}
	p = &q;
	place(p, p->anchor, p->gravity, p->ox, p->oy, &x, &y);
	if(parentorigin(s, &ox, &oy) < 0){
		s->popx = x;
		s->popy = y;
		return;
	}
	sx0 = screenx - ox;
	sy0 = screeny - oy;
	sx1 = screenx + screenw - ox;
	sy1 = screeny + screenh - oy;
	if(outside(x, p->w, sx0, sx1) && (p->adjust & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X)){
		place(p, flipx(p->anchor), flipx(p->gravity), -p->ox, p->oy, &fx, &fy);
		if(!outside(fx, p->w, sx0, sx1))
			x = fx;
	}
	if(outside(y, p->h, sy0, sy1) && (p->adjust & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y)){
		place(p, flipy(p->anchor), flipy(p->gravity), p->ox, -p->oy, &fx, &fy);
		if(!outside(fy, p->h, sy0, sy1))
			y = fy;
	}
	if(p->adjust & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X){
		if(x + p->w > sx1)
			x = sx1 - p->w;
		if(x < sx0)
			x = sx0;
	}
	if(p->adjust & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y){
		if(y + p->h > sy1)
			y = sy1 - p->h;
		if(y < sy0)
			y = sy0;
	}
	s->popx = x;
	s->popy = y;
}

/*
 * the popup's window on the screen for a buffer of w by h: where it was
 * configured, its own window geometry's origin there; -1 if its parent is
 * not shown
 */
int
popuprect(Surface *s, int w, int h, Rect *r)
{
	int ox, oy;

	if(parentorigin(s, &ox, &oy) < 0)
		return -1;
	r->x0 = ox + s->popx - s->gx;
	r->y0 = oy + s->popy - s->gy;
	r->x1 = r->x0 + w;
	r->y1 = r->y0 + h;
	return 0;
}

/* ---- xdg_popup ---- */

static Surface*
psurface(struct wl_resource *r)
{
	return wl_resource_get_user_data(r);
}

/* popup s is dismissed: popup_done, its window gone */
static void
dismiss(Surface *s)
{
	if(s->popup == NULL || s->dismissed)
		return;
	s->dismissed = 1;
	dprint("popup dismissed\n");
	xdg_popup_send_popup_done(s->popup);
	unshow(s);
}

static Surface*
root(Surface *s)
{
	while(s != NULL && s->popup != NULL)
		s = s->parent;
	return s;
}

/* s is f, or one of its parents */
static int
under(Surface *s, Surface *f)
{
	for(; f != NULL; f = f->popup != NULL ? f->parent : NULL)
		if(f == s)
			return 1;
	return 0;
}

/* a popup stays while it has the keyboard or a popup of it has, or a tooltip of one that stays */
static int
stays(Surface *s, Surface *f)
{
	if(under(s, f))
		return 1;
	if(s->popup == NULL || s->grab || s->parent == NULL)
		return 0;
	return stays(s->parent, f);
}

static struct wl_event_source *settle;

/*
 * the focus settled (wsys tells the window that lost it and the one that
 * got it, in two files): popups not on the focus' way dismissed, the
 * toplevels' activated state sent where it changed
 */
static int
onsettle(void *data)
{
	Surface *s, *f;
	Win *w;
	int a, again;

	(void)data;
	f = (w = keywin()) != NULL ? w->s : NULL;
	do{
		again = 0;
		wl_list_for_each(s, &surfaces, link)
			if(s->popup != NULL && !s->dismissed && s->win != NULL && !stays(s, f)){
				dismiss(s);
				again = 1;	/* unshow may change the list's surfaces' windows */
				break;
			}
	}while(again);
	wl_list_for_each(s, &surfaces, link){
		if(s->toplevel == NULL || !s->committed)
			continue;
		a = f != NULL && root(f) == s;
		if(a != s->activated){
			s->activated = a;
			configure(s);
		}
	}
	return 0;
}

/* the focus changed: looked at once it settled */
void
focuschanged(void)
{
	if(settle == NULL)
		settle = wl_event_loop_add_timer(loop, onsettle, NULL);
	if(settle != NULL)
		wl_event_source_timer_update(settle, 20);
}

/* surface s goes: its popups are dismissed, they have no parent */
void
popupforget(Surface *s)
{
	Surface *p;

	wl_list_for_each(p, &surfaces, link)
		if(p->parent == s){
			dismiss(p);
			p->parent = NULL;
		}
}

/* every popup dismissed (wsys gone) */
void
popupsdone(void)
{
	Surface *s;

	wl_list_for_each(s, &surfaces, link)
		if(s->popup != NULL && !s->dismissed){
			s->dismissed = 1;
			xdg_popup_send_popup_done(s->popup);
		}
}

static void
popupdestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = psurface(r)) == NULL)
		return;
	popupforget(s);
	unshow(s);
	s->popup = NULL;
	s->parent = NULL;
	s->committed = 0;
	s->dismissed = 0;
}

static void
popupgrab(struct wl_client *c, struct wl_resource *r, struct wl_resource *seat, uint32_t serial)
{
	Surface *s;

	(void)c; (void)seat; (void)serial;
	if((s = psurface(r)) == NULL)
		return;
	if(s->win != NULL){
		wl_resource_post_error(r, XDG_POPUP_ERROR_INVALID_GRAB, "grab after the popup was mapped");
		return;
	}
	s->grab = 1;
}

static void
popupreposition(struct wl_client *c, struct wl_resource *r, struct wl_resource *p, uint32_t token)
{
	Surface *s;

	(void)c;
	if((s = psurface(r)) == NULL || s->dismissed)
		return;
	s->pos = *pos(p);
	popupplace(s);
	if(wl_resource_get_version(r) >= XDG_POPUP_REPOSITIONED_SINCE_VERSION)
		xdg_popup_send_repositioned(r, token);
	configure(s);
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

/* the toplevel's size and states (a popup's place), then the xdg_surface's serial */
void
configure(Surface *s)
{
	struct wl_array states;
	uint32_t *st;
	double sc;
	int w, h;

	if(s->xdgsurf == NULL)
		return;
	if(s->popup != NULL){
		if(s->dismissed)
			return;
		/* its place in the parent's surface units (popx, popy: the window's pixels) */
		sc = s->parent != NULL ? scaleof(s->parent) : 1;
		xdg_popup_send_configure(s->popup, s->popx / sc, s->popy / sc, s->pos.w, s->pos.h);
		s->serial = wl_display_next_serial(display);
		xdg_surface_send_configure(s->xdgsurf, s->serial);
		dprint("popup configure %d %d %d %d serial %u\n", s->popx, s->popy, s->pos.w, s->pos.h, s->serial);
		return;
	}
	if(s->toplevel == NULL)
		return;
	wl_array_init(&states);
	w = h = 0;	/* the client's choice */
	if(s->win != NULL){
		/* the window's pixels in the units of the scale it draws at next (scale.c) */
		sc = wantscale(s);
		w = s->win->w / sc + 0.5;
		h = s->win->h / sc + 0.5;
		if(s->activated && (st = wl_array_add(&states, sizeof *st)) != NULL)
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
	if(s->toplevel != NULL || s->popup != NULL){
		wl_resource_post_error(r, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "a role already");
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

/*
 * a popup of parent's (an xdg_surface; none: set later by another
 * protocol, not offered), placed by the positioner's rules; its window has
 * no border, takes the focus if it grabs (a menu), else leaves it (a
 * tooltip)
 */
static void
xdggetpopup(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *parent, struct wl_resource *positioner)
{
	struct wl_resource *p;
	Surface *s, *ps;
	Pos *q;

	if((s = wl_resource_get_user_data(r)) == NULL)
		return;
	if(s->toplevel != NULL || s->popup != NULL){
		wl_resource_post_error(r, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "a role already");
		return;
	}
	q = pos(positioner);
	if(q->w <= 0 || q->h <= 0){
		wl_resource_post_error(r, XDG_WM_BASE_ERROR_INVALID_POSITIONER, "a positioner without a size");
		return;
	}
	p = wl_resource_create(c, &xdg_popup_interface, wl_resource_get_version(r), id);
	if(p == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p, &popupimpl, s, popupdestroyres);
	s->popup = p;
	s->pos = *q;
	s->grab = 0;
	s->dismissed = 0;
	s->committed = 0;
	ps = parent != NULL ? wl_resource_get_user_data(parent) : NULL;
	s->parent = ps;
	popupplace(s);
	if(ps == NULL)
		dismiss(s);	/* no parent: nowhere to be */
}

/* the part of the surface that is the window (without a client's shadows) */
static void
xdggeometry(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h)
{
	Surface *s;

	(void)c; (void)w; (void)h;
	if((s = wl_resource_get_user_data(r)) == NULL)
		return;
	/* in the window's pixels (scale.c) */
	s->gx = topx(s, x);
	s->gy = topx(s, y);
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
	Pos *q;

	p = wl_resource_create(c, &xdg_positioner_interface, wl_resource_get_version(r), id);
	if(p == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	if((q = calloc(1, sizeof *q)) == NULL){
		wl_resource_destroy(p);
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p, &positionerimpl, q, posdestroyres);
}

static void
wmgetxdgsurface(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf)
{
	Surface *s;
	struct wl_resource *x;

	s = wl_resource_get_user_data(surf);
	if(s->xdgsurf != NULL || (s->role != Rnone && s->role != Rxdg)){
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
	s->role = Rxdg;
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
