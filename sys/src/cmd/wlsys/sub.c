/*
 * SPDX-License-Identifier: MIT
 *
 * wl_subcompositor: a window with subsurfaces shows its tree as wsys'
 * layers (ctl's layers): every surface's buffer goes to wsys as it is, the
 * tree's order and places in one message; wsys composes them (on the GPU,
 * a layer with alpha blended over those under it, or with draw(3)) and
 * keeps each buffer until no frame shows it.  wlsys copies no pixels.
 * Synchronized subsurfaces (the default) keep a commit until their
 * parent's, as the protocol says; desynchronized ones show theirs at once.
 * The pointer goes to the topmost surface under it.
 */
#include "dat.h"

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/* a new surface: a tree of its own */
void
subnew(Surface *s)
{
	wl_list_init(&s->stack);
	wl_list_insert(&s->stack, &s->self);
	wl_list_init(&s->slink);
}

int
hassubs(Surface *s)
{
	return s->stack.next != &s->self || s->stack.prev != &s->self;
}

static Surface*
treeroot(Surface *s)
{
	while(s->sparent != NULL)
		s = s->sparent;
	return s;
}

/* s or a parent of it synchronized: its commits wait for the parent's */
static int
synced(Surface *s)
{
	for(; s != NULL && s->sub != NULL; s = s->sparent)
		if(!s->desync)
			return 1;
	return 0;
}

/* the window of the tree shown again (composed) */
static void
repaint(Surface *s)
{
	Surface *r;

	r = treeroot(s);
	if(r->buf != NULL && (r->toplevel != NULL || (r->popup != NULL && !r->dismissed)))
		show(r);
}

/*
 * a commit of a subsurface: kept for the parent's commit if synchronized
 * (1: nothing more to do), else its state applied by the caller (0)
 */
int
subcommit(Surface *s)
{
	if(!synced(s))
		return 0;
	if(s->pattached){
		s->cbuf = s->pbuf;
		s->cattached = 1;
	}
	s->pbuf = NULL;
	s->pattached = 0;
	s->cached = 1;
	return 1;
}

/*
 * s committed (its state applied): its subsurfaces' places, their
 * kept commits (and theirs) applied
 */
void
subapply(Surface *s)
{
	struct wl_list *l;
	Surface *c;

	for(l = s->stack.next; l != &s->stack; l = l->next){
		if(l == &s->self)
			continue;
		c = wl_container_of(l, c, slink);
		c->sx = c->psx;
		c->sy = c->psy;
		if(c->cached){
			c->cached = 0;
			if(c->cattached){
				c->buf = c->cbuf;
				c->cbuf = NULL;
				c->cattached = 0;
				if(c->buf != NULL)
					c->buf->freed = 0;
				c->lnew = 1;
			}
			subapply(c);
		}
	}
}

/*
 * after a surface's own state was applied by a commit (attached: with a
 * buffer): its subsurfaces' kept commits applied; a desynchronized
 * subsurface's tree shown again
 */
void
subcommitted(Surface *s, int attached)
{
	if(attached)
		s->lnew = 1;
	subapply(s);
	if(s->sub != NULL)
		repaint(s);
}

/*
 * the layers of root's tree, bottom to top: each surface with a buffer
 * (its parents with theirs), where it is in the window; their number
 */
static int
layers1(Surface *s, int x, int y, Surface **v, int *xs, int *ys, int n, int max)
{
	struct wl_list *l;
	Surface *c;

	for(l = s->stack.next; l != &s->stack; l = l->next){
		if(l != &s->self){
			c = wl_container_of(l, c, slink);
			if(c->buf != NULL)
				n = layers1(c, x + c->sx, y + c->sy, v, xs, ys, n, max);
			continue;
		}
		if(s->buf != NULL && n < max){
			v[n] = s;
			xs[n] = x;
			ys[n] = y;
			n++;
		}
	}
	return n;
}

int
treelayers(Surface *root, Surface **v, int *xs, int *ys, int max)
{
	return layers1(root, 0, 0, v, xs, ys, 0, max);
}

/* ---- the pointer ---- */

/* the topmost surface of s's tree (s at x, y) with a picture under x, y; its point in *px, *py */
static Surface*
pick1(Surface *s, int sx, int sy, int x, int y, int *px, int *py)
{
	struct wl_list *l;
	Surface *c, *f;

	for(l = s->stack.prev; l != &s->stack; l = l->prev){
		if(l != &s->self){
			c = wl_container_of(l, c, slink);
			if(c->buf != NULL && (f = pick1(c, sx + c->sx, sy + c->sy, x, y, px, py)) != NULL)
				return f;
			continue;
		}
		if(s->buf != NULL && x >= sx && y >= sy && x < sx + s->buf->w && y < sy + s->buf->h){
			*px = x - sx;
			*py = y - sy;
			return s;
		}
	}
	return NULL;
}

/* the surface of root's tree under x, y of the window (the root if none) */
Surface*
pick(Surface *root, int x, int y, int *px, int *py)
{
	Surface *s;

	if((s = pick1(root, 0, 0, x, y, px, py)) != NULL)
		return s;
	*px = x;
	*py = y;
	return root;
}

/* where s is in its tree's window (its root's 0 0) */
void
suborigin(Surface *s, int *x, int *y)
{
	*x = *y = 0;
	for(; s->sparent != NULL; s = s->sparent){
		*x += s->sx;
		*y += s->sy;
	}
}

/* ---- wl_subsurface ---- */

static Surface*
subsurf(struct wl_resource *r)
{
	return wl_resource_get_user_data(r);
}

/* s out of its parent's tree (unmapped) */
static void
subunlink(Surface *s)
{
	Surface *p;

	if((p = s->sparent) == NULL)
		return;
	wl_list_remove(&s->slink);
	wl_list_init(&s->slink);
	s->sparent = NULL;
	repaint(p);
}

static void
subdestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = subsurf(r)) == NULL)
		return;
	s->sub = NULL;
	s->cached = 0;
	s->cbuf = NULL;
	s->cattached = 0;
	subunlink(s);
}

static void
subposition(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y)
{
	Surface *s;

	(void)c;
	if((s = subsurf(r)) == NULL)
		return;
	s->psx = x;
	s->psy = y;
}

/* the entry of sibling in s's parent's stack (the parent's own, or another subsurface's); nil if not one */
static struct wl_list*
entry(Surface *s, struct wl_resource *sibling)
{
	Surface *t;

	if(sibling == NULL || (t = wl_resource_get_user_data(sibling)) == NULL || t == s)
		return NULL;
	if(t == s->sparent)
		return &t->self;
	if(t->sparent == s->sparent)
		return &t->slink;
	return NULL;
}

static void
subplace(struct wl_resource *r, struct wl_resource *sibling, int above)
{
	Surface *s;
	struct wl_list *e;

	if((s = subsurf(r)) == NULL || s->sparent == NULL)
		return;
	if((e = entry(s, sibling)) == NULL){
		wl_resource_post_error(r, WL_SUBSURFACE_ERROR_BAD_SURFACE, "not a sibling nor the parent");
		return;
	}
	wl_list_remove(&s->slink);
	wl_list_insert(above ? e : e->prev, &s->slink);
}

static void subabove(struct wl_client *c, struct wl_resource *r, struct wl_resource *sib) { (void)c; subplace(r, sib, 1); }
static void subbelow(struct wl_client *c, struct wl_resource *r, struct wl_resource *sib) { (void)c; subplace(r, sib, 0); }

static void
subsync(struct wl_client *c, struct wl_resource *r)
{
	Surface *s;

	(void)c;
	if((s = subsurf(r)) != NULL)
		s->desync = 0;
}

/* desynchronized: a commit kept is applied now */
static void
subdesync(struct wl_client *c, struct wl_resource *r)
{
	Surface *s;

	(void)c;
	if((s = subsurf(r)) == NULL || s->desync)
		return;
	s->desync = 1;
	if(s->cached && !synced(s)){
		s->cached = 0;
		if(s->cattached){
			s->buf = s->cbuf;
			s->cbuf = NULL;
			s->cattached = 0;
			if(s->buf != NULL)
				s->buf->freed = 0;
			s->lnew = 1;
		}
		subapply(s);
		repaint(s);
	}
}

static const struct wl_subsurface_interface subimpl = {
	resdestroy,
	subposition,
	subabove,
	subbelow,
	subsync,
	subdesync,
};

/* the surface goes: out of its parent's tree, its subsurfaces unmapped */
void
subgone(Surface *s)
{
	struct wl_list *l, *n;
	Surface *c;

	if(s->sub != NULL)
		wl_resource_set_user_data(s->sub, NULL);
	subunlink(s);
	for(l = s->stack.next; l != &s->stack; l = n){
		n = l->next;
		if(l == &s->self)
			continue;
		c = wl_container_of(l, c, slink);
		wl_list_remove(&c->slink);
		wl_list_init(&c->slink);
		c->sparent = NULL;
	}
}

/* ---- wl_subcompositor ---- */

static void
getsub(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *surf, struct wl_resource *parent)
{
	Surface *s, *p, *q;
	struct wl_resource *u;

	s = wl_resource_get_user_data(surf);
	p = wl_resource_get_user_data(parent);
	if(s->role != Rnone && s->role != Rsub){
		wl_resource_post_error(r, WL_SUBCOMPOSITOR_ERROR_BAD_SURFACE, "the surface has another role");
		return;
	}
	if(s->sub != NULL){
		wl_resource_post_error(r, WL_SUBCOMPOSITOR_ERROR_BAD_SURFACE, "a subsurface already");
		return;
	}
	for(q = p; q != NULL; q = q->sparent)
		if(q == s){
			wl_resource_post_error(r, WL_SUBCOMPOSITOR_ERROR_BAD_PARENT, "the surface is its own parent");
			return;
		}
	if((u = wl_resource_create(c, &wl_subsurface_interface, wl_resource_get_version(r), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(u, &subimpl, s, subdestroyres);
	s->role = Rsub;
	s->sub = u;
	s->sparent = p;
	s->desync = 0;
	s->sx = s->sy = s->psx = s->psy = 0;
	/* on top of its siblings */
	wl_list_insert(p->stack.prev, &s->slink);
	if(s->buf != NULL)
		s->lnew = 1;
}

static const struct wl_subcompositor_interface subcompimpl = {
	resdestroy,
	getsub,
};

static void
subbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_subcompositor_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &subcompimpl, NULL, NULL);
}

void
subinit(void)
{
	wl_global_create(display, &wl_subcompositor_interface, 1, NULL, subbind);
}
