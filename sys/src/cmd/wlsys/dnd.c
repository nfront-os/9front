/*
 * SPDX-License-Identifier: MIT
 *
 * Drag and drop (wl_data_device.start_drag): while the button that started
 * it is held, wsys keeps giving the mouse to the window of origin, also
 * outside it; each motion's point on the screen finds the topmost window of
 * wsys there (every window's ctl: its rectangle, z and hidden), and if it is
 * a Wayland window of wlsys' the surface of its tree under the point.  That
 * client's data devices get an offer of the source's types with enter,
 * motion and leave; the button released over a target that accepted a type
 * (and an action) drops, else the source is cancelled.  The target's
 * receive goes to the source as it is (its pipe: no copy here); its finish
 * ends the drag.  The icon is a window of wsys without a border and without
 * the focus, at the pointer.  No drag reaches a Plan 9 program, nor comes
 * from one.
 */
#include "dat.h"
#include <dirent.h>

enum {
	Noffer = 8,	/* the target client's data devices */
};

static struct {
	int	on;
	struct wl_client	*client;	/* the drag's */
	Source	*src;		/* nil: a drag within its client, with no data */
	Win	*from;		/* the window it started in: its mouse is the drag's */
	Surface	*icon;
	int	sx, sy;		/* the pointer on the screen */
	Surface	*over;		/* the target surface, nil if none */
	struct wl_resource	*offers[Noffer];	/* to its client's devices */
	int	noffer;
	int	accepted;	/* a type accepted by the target */
	uint32_t	dactions, dpref;	/* the target's actions, its preferred */
	uint32_t	action;	/* the one chosen */
	int	dropped;	/* waiting for the target's finish */
} drag;

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static uint32_t
srcactions(void)
{
	if(drag.src == NULL)
		return WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY;
	if(wl_resource_get_version(drag.src->res) < WL_DATA_SOURCE_ACTION_SINCE_VERSION)
		return WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY;
	return drag.src->actions;
}

/* the action both sides take: the target's preferred, else copy, move, ask */
static void
chooseaction(void)
{
	static uint32_t order[] = {
		WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY,
		WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE,
		WL_DATA_DEVICE_MANAGER_DND_ACTION_ASK,
	};
	uint32_t both, a;
	int i;

	both = srcactions() & drag.dactions;
	a = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
	if(drag.dpref & both)
		a = drag.dpref & both;
	else
		for(i = 0; i < 3; i++)
			if(both & order[i]){
				a = order[i];
				break;
			}
	if(a == drag.action)
		return;
	drag.action = a;
	for(i = 0; i < drag.noffer; i++)
		if(drag.offers[i] != NULL && wl_resource_get_version(drag.offers[i]) >= WL_DATA_OFFER_ACTION_SINCE_VERSION)
			wl_data_offer_send_action(drag.offers[i], a);
	if(drag.src != NULL && wl_resource_get_version(drag.src->res) >= WL_DATA_SOURCE_ACTION_SINCE_VERSION)
		wl_data_source_send_action(drag.src->res, a);
}

/* ---- the target's offers ---- */

static int
myoffer(struct wl_resource *r)
{
	int i;

	for(i = 0; i < drag.noffer; i++)
		if(drag.offers[i] == r)
			return 1;
	return 0;
}

static void
offeraccept(struct wl_client *c, struct wl_resource *r, uint32_t serial, const char *mime)
{
	(void)c; (void)serial;
	if(!drag.on || !myoffer(r))
		return;
	drag.accepted = mime != NULL;
	if(drag.src != NULL)
		wl_data_source_send_target(drag.src->res, mime);
}

/* the data: the target's fd to the source, which writes it there */
static void
offerreceive(struct wl_client *c, struct wl_resource *r, const char *mime, int32_t fd)
{
	(void)c;
	if(drag.on && myoffer(r) && drag.src != NULL)
		wl_data_source_send_send(drag.src->res, mime, fd);
	close(fd);
}

static void dragend(void);

static void
offerfinish(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	if(!drag.on || !myoffer(r) || !drag.dropped)
		return;
	if(drag.src != NULL && wl_resource_get_version(drag.src->res) >= WL_DATA_SOURCE_DND_FINISHED_SINCE_VERSION)
		wl_data_source_send_dnd_finished(drag.src->res);
	dragend();
}

static void
offeractions(struct wl_client *c, struct wl_resource *r, uint32_t actions, uint32_t pref)
{
	(void)c;
	if(!drag.on || !myoffer(r))
		return;
	drag.dactions = actions;
	drag.dpref = pref;
	chooseaction();
}

static void
offerdestroyres(struct wl_resource *r)
{
	int i;

	for(i = 0; i < drag.noffer; i++)
		if(drag.offers[i] == r)
			drag.offers[i] = NULL;
}

static const struct wl_data_offer_interface offerimpl = {
	offeraccept,
	offerreceive,
	resdestroy,
	offerfinish,
	offeractions,
};

/* ---- entering and leaving targets ---- */

/* the target's devices told the pointer left (not after a drop) */
static void
leave(void)
{
	struct wl_resource *d;

	if(drag.over == NULL)
		return;
	wl_resource_for_each(d, datadevices())
		if(wl_resource_get_client(d) == wl_resource_get_client(drag.over->res))
			wl_data_device_send_leave(d);
	drag.over = NULL;
	drag.noffer = 0;
	drag.accepted = 0;
	drag.dactions = drag.dpref = 0;
	drag.action = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
}

/* surface s at x, y the target: an offer to each of its client's devices, enter */
static void
enter(Surface *s, int x, int y)
{
	struct wl_resource *d, *o;
	struct wl_client *c;
	uint32_t serial;
	int i;

	leave();
	drag.over = s;
	c = wl_resource_get_client(s->res);
	/* a drag with no source goes to its own client only */
	if(drag.src == NULL && c != drag.client)
		return;
	serial = wl_display_next_serial(display);
	wl_resource_for_each(d, datadevices()){
		if(wl_resource_get_client(d) != c || drag.noffer >= Noffer)
			continue;
		o = NULL;
		if(drag.src != NULL){
			if((o = wl_resource_create(c, &wl_data_offer_interface, wl_resource_get_version(d), 0)) == NULL){
				wl_client_post_no_memory(c);
				continue;
			}
			wl_resource_set_implementation(o, &offerimpl, NULL, offerdestroyres);
			wl_data_device_send_data_offer(d, o);
			for(i = 0; i < drag.src->nmime; i++)
				wl_data_offer_send_offer(o, drag.src->mime[i]);
			if(wl_resource_get_version(o) >= WL_DATA_OFFER_SOURCE_ACTIONS_SINCE_VERSION)
				wl_data_offer_send_source_actions(o, srcactions());
		}
		drag.offers[drag.noffer++] = o;
		wl_data_device_send_enter(d, serial, s->res, wl_fixed_from_int(x), wl_fixed_from_int(y), o);
	}
	dprint("drag: over a surface at %d %d\n", x, y);
}

/*
 * the topmost window of wsys at sx, sy (not the icon's), from every
 * window's ctl: id x0 y0 x1 y1 w h base size focus z hidden; -1 if none
 */
static int
topwin(int sx, int sy, Rect *wr)
{
	DIR *dir;
	struct dirent *e;
	char path[64], buf[256];
	int fd, n, id, best, bestz, z, hidden, skip;
	Rect r;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	skip = drag.icon != NULL && drag.icon->win != NULL ? drag.icon->win->id : -1;
	best = -1;
	bestz = -1;
	while((e = readdir(dir)) != NULL){
		if((id = atoi(e->d_name)) <= 0 || id == skip)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", id);
		if((fd = open(path, O_RDONLY)) < 0)
			continue;
		n = read(fd, buf, sizeof buf - 1);
		close(fd);
		if(n <= 0)
			continue;
		buf[n] = 0;
		if(sscanf(buf, "%*d %d %d %d %d %*d %*d %*s %*s %*d %d %d", &r.x0, &r.y0, &r.x1, &r.y1, &z, &hidden) != 6)
			continue;
		if(hidden || sx < r.x0 || sy < r.y0 || sx >= r.x1 || sy >= r.y1 || z <= bestz)
			continue;
		best = id;
		bestz = z;
		*wr = r;
	}
	closedir(dir);
	return best;
}

/* the target at the pointer: the surface of a Wayland window's tree there */
static void
target(void)
{
	Surface *s, *t;
	Rect r;
	int id, x, y;

	t = NULL;
	x = y = 0;
	if((id = topwin(drag.sx, drag.sy, &r)) >= 0)
		wl_list_for_each(s, &surfaces, link)
			if(s->win != NULL && !s->win->gone && s->win->id == id && s->sparent == NULL && s->role != Rdndicon){
				t = pick(s, drag.sx - r.x0, drag.sy - r.y0, &x, &y);
				break;
			}
	if(t != drag.over){
		if(t != NULL)
			enter(t, x, y);
		else
			leave();
		return;
	}
	if(t != NULL){
		struct wl_resource *d;

		wl_resource_for_each(d, datadevices())
			if(wl_resource_get_client(d) == wl_resource_get_client(t->res))
				wl_data_device_send_motion(d, msec(), wl_fixed_from_int(x), wl_fixed_from_int(y));
	}
}

/* ---- the icon: a window of wsys at the pointer ---- */

/* the icon's window for a buffer of w by h: its surface's origin at the pointer */
int
iconrect(Surface *s, int w, int h, Rect *r)
{
	if(!drag.on || s != drag.icon)
		return -1;
	r->x0 = drag.sx + s->ix;
	r->y0 = drag.sy + s->iy;
	r->x1 = r->x0 + w;
	r->y1 = r->y0 + h;
	return 0;
}

static void
iconmove(void)
{
	Rect r;

	if(drag.icon != NULL && drag.icon->win != NULL && drag.icon->buf != NULL &&
	   iconrect(drag.icon, drag.icon->buf->w, drag.icon->buf->h, &r) == 0)
		ctlmsg(drag.icon->win, "rect %d %d %d %d", r.x0, r.y0, r.x1, r.y1);
}

/* ---- the drag ---- */

static void
dragend(void)
{
	Surface *icon;

	icon = drag.icon;
	memset(&drag, 0, sizeof drag);
	if(icon != NULL)
		unshow(icon);
	dprint("drag: over\n");
}

/* the drag cancelled: the target left, the source told */
static void
cancel(void)
{
	if(!drag.dropped)
		leave();
	if(drag.src != NULL)
		wl_data_source_send_cancelled(drag.src->res);
	dragend();
}

/*
 * start_drag of data device r: src's types (none: within its client), from
 * surface origin of the window with the button held, icon at the pointer
 */
void
dragstart(struct wl_resource *r, Source *src, Surface *origin, Surface *icon)
{
	struct wl_client *c;
	Win *w;
	Rect wr;
	int x, y;

	c = wl_resource_get_client(r);
	if(drag.on || origin == NULL || (w = pointerheld(c, &x, &y)) == NULL || winrect(w, &wr) < 0){
		if(src != NULL)
			wl_data_source_send_cancelled(src->res);
		return;
	}
	if(icon != NULL){
		if(icon->role != Rnone && icon->role != Rdndicon){
			wl_resource_post_error(r, WL_DATA_DEVICE_ERROR_ROLE, "the icon has another role");
			return;
		}
		icon->role = Rdndicon;
	}
	memset(&drag, 0, sizeof drag);
	drag.on = 1;
	drag.client = c;
	drag.src = src;
	drag.from = w;
	drag.icon = icon;
	drag.sx = wr.x0 + x;
	drag.sy = wr.y0 + y;
	dprint("drag: from window %d at %d %d\n", w->id, drag.sx, drag.sy);
	/* the pointer is the drag's now */
	pointerleave();
	if(icon != NULL && icon->buf != NULL)
		show(icon);
	target();
}

/* a mouse event of window w (x, y in it): the drag's, if it is from w (1) */
int
dragpointer(Win *w, int x, int y, int b, uint32_t ms)
{
	Rect wr;

	(void)ms;
	if(!drag.on || drag.dropped || w != drag.from)
		return 0;
	if(winrect(w, &wr) == 0){
		drag.sx = wr.x0 + x;
		drag.sy = wr.y0 + y;
	}
	if((b & 7) != 0){
		iconmove();
		target();
		return 1;
	}
	/* released: dropped on a target that takes it, else cancelled */
	if(drag.over == NULL || (drag.src != NULL && (!drag.accepted ||
	   (wl_resource_get_version(drag.src->res) >= WL_DATA_SOURCE_ACTION_SINCE_VERSION && drag.action == WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE)))){
		dprint("drag: released, not taken\n");
		cancel();
		return 1;
	}
	{
		struct wl_resource *d;

		wl_resource_for_each(d, datadevices())
			if(wl_resource_get_client(d) == wl_resource_get_client(drag.over->res))
				wl_data_device_send_drop(d);
	}
	if(drag.src != NULL && wl_resource_get_version(drag.src->res) >= WL_DATA_SOURCE_DND_DROP_PERFORMED_SINCE_VERSION)
		wl_data_source_send_dnd_drop_performed(drag.src->res);
	dprint("drag: dropped\n");
	if(drag.src == NULL){
		dragend();
		return 1;
	}
	/* the icon goes; the drag waits for the target's finish */
	drag.dropped = 1;
	if(drag.icon != NULL){
		unshow(drag.icon);
		drag.icon = NULL;
	}
	return 1;
}

void
dragsourcegone(Source *s)
{
	if(drag.on && drag.src == s){
		drag.src = NULL;
		if(!drag.dropped)
			leave();
		dragend();
	}
}

void
dragsurfacegone(Surface *s)
{
	if(!drag.on)
		return;
	if(drag.icon == s)
		drag.icon = NULL;
	if(drag.over == s){
		drag.over = NULL;	/* gone: no leave to send */
		drag.noffer = 0;
	}
}

void
dragwingone(Win *w)
{
	if(drag.on && drag.from == w)
		cancel();
}
