/*
 * SPDX-License-Identifier: MIT
 *
 * wl_subcompositor: a window with subsurfaces is composed by wlsys.  Each
 * surface of the tree keeps a copy of its buffer's pixels (read from the
 * segment's data file when it is committed; the buffer is released at
 * once), and a commit of the tree blends the copies, bottom to top, each
 * at its place, into one of the window's own buffers (a memfd of wlsys'
 * as a pool, three of them: wsys holds the one shown); the window is the
 * root surface's size, a subsurface beyond it is cut off.  A window
 * without subsurfaces stays as it was: its client's buffers go to wsys as
 * they are, nothing copied.  Synchronized subsurfaces (the default) keep a
 * commit until their parent's, as the protocol says; desynchronized ones
 * show theirs at once.  A dma-buf in a tree is not read: it is left out.
 * The pointer goes to the topmost surface under it.
 */
#include "dat.h"
#include <sys/mman.h>

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

/* the buffer's pixels copied (a dma-buf's: none), the buffer released */
static void
snap(Surface *s)
{
	Buffer *b;
	unsigned char *px;
	int y;

	free(s->pix);
	s->pix = NULL;
	if((b = s->buf) == NULL)
		return;
	if((px = pixels(b)) != NULL && (s->pix = malloc((size_t)b->w*b->h*4)) != NULL){
		for(y = 0; y < b->h; y++)
			memcpy(s->pix + (size_t)y*b->w, px + (size_t)y*b->stride, 4*b->w);
		s->pixw = b->w;
		s->pixh = b->h;
		s->pixalpha = b->format == WL_SHM_FORMAT_ARGB8888;
	}
	free(px);
	relbuf(b);
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
 * kept commits (and theirs) applied, its pixels copied if it is in a tree
 * wlsys composes; a desynchronized subsurface's window shown again
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
				snap(c);
			}
			subapply(c);
		}
	}
}

/*
 * after a surface's own state was applied by a commit (attached: with a
 * buffer): copied if it is part of a tree, a root without subsurfaces any
 * more back to showing its client's buffers; a desynchronized
 * subsurface's tree shown again
 */
void
subcommitted(Surface *s, int attached)
{
	if(attached){
		if(s->sub != NULL || hassubs(s))
			snap(s);
		else{
			free(s->pix);
			s->pix = NULL;
		}
	}
	subapply(s);
	if(s->sub != NULL)
		repaint(s);
}

/* ---- composing ---- */

/* n pixels of src over dst (premultiplied; no alpha: a copy) */
static void
blend(uint32_t *dst, uint32_t *src, int n, int alpha)
{
	uint32_t s, d, a, rb, g;
	int i;

	if(!alpha){
		for(i = 0; i < n; i++)
			dst[i] = src[i] | 0xff000000u;
		return;
	}
	for(i = 0; i < n; i++){
		s = src[i];
		a = s >> 24;
		if(a == 255){
			dst[i] = s;
			continue;
		}
		if(a == 0)
			continue;
		d = dst[i];
		rb = ((d & 0xff00ff) * (255 - a) / 255) & 0xff00ff;
		g = ((d & 0x00ff00) * (255 - a) / 255) & 0x00ff00;
		dst[i] = (s & 0xffffff) + rb + g;	/* premultiplied: no carry */
		dst[i] |= 0xff000000u;
	}
}

/* surface s's copy at x, y of the w by h picture px, then its subsurfaces, in their order */
static void
draw(Surface *s, int x, int y, uint32_t *px, int w, int h)
{
	struct wl_list *l;
	Surface *c;
	int x0, y0, x1, y1, row;

	for(l = s->stack.next; l != &s->stack; l = l->next){
		if(l != &s->self){
			c = wl_container_of(l, c, slink);
			if(c->buf != NULL)
				draw(c, x + c->sx, y + c->sy, px, w, h);
			continue;
		}
		if(s->pix == NULL)
			continue;
		x0 = x < 0 ? 0 : x;
		y0 = y < 0 ? 0 : y;
		x1 = x + s->pixw > w ? w : x + s->pixw;
		y1 = y + s->pixh > h ? h : y + s->pixh;
		for(row = y0; row < y1 && x0 < x1; row++)
			blend(px + (size_t)row*w + x0, s->pix + (size_t)(row - y)*s->pixw + (x0 - x), x1 - x0, s->pixalpha);
	}
}

/* the window's own buffers let go of */
void
compfree(Win *w)
{
	int i;

	for(i = 0; i < Ncomp; i++){
		if(w->cbuf[i] == NULL)
			continue;
		unbuffer(w->cbuf[i]);
		free(w->cbuf[i]);
		w->cbuf[i] = NULL;
	}
	if(w->cpx != NULL)
		munmap(w->cpx, (size_t)Ncomp*w->cw*w->ch*4);
	w->cpx = NULL;
	if(w->cpool != NULL)
		poolput(w->cpool);
	w->cpool = NULL;
	w->cw = w->ch = 0;
}

/* Ncomp buffers of w by h in a memfd of wlsys' */
static int
compmake(Win *w, int bw, int bh)
{
	Pool *p;
	Buffer *b;
	size_t n;
	int fd, i;

	compfree(w);
	n = (size_t)Ncomp*bw*bh*4;
	if((p = calloc(1, sizeof *p)) == NULL)
		return -1;
	if((fd = memfd_create("wlsys-compose", MFD_CLOEXEC)) < 0 || ftruncate(fd, n) < 0 ||
	   segname(fd, p->seg, sizeof p->seg) < 0 ||
	   (w->cpx = mmap(NULL, n, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		fprintf(stderr, "wlsys: window %d: buffers to compose in: %s\n", w->id, strerror(errno));
		if(fd >= 0)
			close(fd);
		free(p);
		w->cpx = NULL;
		return -1;
	}
	p->fd = fd;
	p->size = n;
	p->ref = 1;
	w->cpool = p;
	w->cw = bw;
	w->ch = bh;
	for(i = 0; i < Ncomp; i++){
		if((b = calloc(1, sizeof *b)) == NULL){
			compfree(w);
			return -1;
		}
		b->pool = p;
		b->off = i*bw*bh*4;
		b->w = bw;
		b->h = bh;
		b->stride = 4*bw;
		b->format = WL_SHM_FORMAT_XRGB8888;
		b->slot = -1;
		w->cbuf[i] = b;
	}
	dprint("window %d: composes its subsurfaces, %dx%d\n", w->id, bw, bh);
	return 0;
}

/*
 * the tree of root composed into one of window w's own buffers, the root's
 * size; nil if all of them are wsys' (the picture is skipped)
 */
Buffer*
compose(Surface *root, Win *w)
{
	Buffer *b;
	uint32_t *px;
	int i, bw, bh;

	if(root->pix == NULL && root->buf != NULL)
		snap(root);	/* subsurfaces came without a new buffer of the root */
	bw = root->buf->w;
	bh = root->buf->h;
	if((w->cw != bw || w->ch != bh) && compmake(w, bw, bh) < 0)
		return NULL;
	b = NULL;
	for(i = 0; i < Ncomp; i++)
		if(w->cbuf[i]->win != w || !w->busy[w->cbuf[i]->slot]){
			b = w->cbuf[i];
			break;
		}
	if(b == NULL)
		return NULL;
	px = w->cpx + (size_t)i*bw*bh;
	for(i = 0; i < bw*bh; i++)
		px[i] = 0xff000000u;
	draw(root, 0, 0, px, bw, bh);
	return b;
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
			snap(s);
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
	free(s->pix);
	s->pix = NULL;
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
		snap(s);
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
