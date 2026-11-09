/*
 * SPDX-License-Identifier: MIT
 *
 * wl_seat: a pointer and a keyboard.  The pointer is wsys' mouse in the
 * window that has the focus (m events: while over it, a button held, once
 * when it left); buttons 1, 2 and 4 are Linux's left, middle and right, 8
 * and 16 the wheel.  The keyboard is the focused window's kbd file: kbd(3)'s
 * k and K messages say which keys are held, their difference is Wayland's
 * key events; the keymap is kbdfs' (keymap.c), in a memfd the clients map.
 */
#include "dat.h"
#include <sys/mman.h>

enum {
	Btnleft = 0x110,
	Btnright = 0x111,
	Btnmiddle = 0x112,
};

static struct wl_list pointers, keyboards;	/* their resources */
static Win *pwin;		/* the window the pointer is in */
static Surface *psurf;		/* and the surface of its tree under it */
static int pwx, pwy;		/* where in the window, last seen */
static int px, py, pbuttons;
static Win *kwin;		/* the one the keyboard is in */
static uint32_t held[Maxkeys];	/* Linux codes */
static int nheld;
static uint32_t mods;
static int keymapfd = -1;
static int keymaplen;
static Surface *cursurf;	/* the cursor surface set, nil: hidden */
static int curset;		/* set_cursor was called for curwin */
static int curhx, curhy;	/* its hot spot */
static Win *curwin;		/* the window it was set for */

static struct wl_client*
winclient(Win *w)
{
	if(w == NULL || w->s == NULL)
		return NULL;
	return wl_resource_get_client(w->s->res);
}

#define eachres(r, list, w) \
	wl_resource_for_each(r, list) \
		if(wl_resource_get_client(r) == winclient(w))

static void
pframe(struct wl_resource *r)
{
	if(wl_resource_get_version(r) >= WL_POINTER_FRAME_SINCE_VERSION)
		wl_pointer_send_frame(r);
}

static void
pleave(void)
{
	struct wl_resource *r;
	uint32_t serial;

	if(pwin == NULL)
		return;
	serial = wl_display_next_serial(display);
	if(psurf != NULL)
		eachres(r, &pointers, pwin){
			wl_pointer_send_leave(r, serial, psurf->res);
			pframe(r);
		}
	pwin = NULL;
	psurf = NULL;
	pbuttons = 0;
}

static void
penter(Win *w, Surface *s, int x, int y)
{
	struct wl_resource *r;
	uint32_t serial;

	pleave();
	pwin = w;
	psurf = s;
	px = x;
	py = y;
	pbuttons = 0;
	serial = wl_display_next_serial(display);
	eachres(r, &pointers, w){
		wl_pointer_send_enter(r, serial, s->res, wl_fixed_from_int(x), wl_fixed_from_int(y));
		pframe(r);
	}
}

static void
pbutton(Win *w, uint32_t ms, uint32_t code, int down)
{
	struct wl_resource *r;
	uint32_t serial;

	serial = wl_display_next_serial(display);
	eachres(r, &pointers, w)
		wl_pointer_send_button(r, serial, ms, code,
			down ? WL_POINTER_BUTTON_STATE_PRESSED : WL_POINTER_BUTTON_STATE_RELEASED);
}

static void
pwheel(Win *w, uint32_t ms, int dir)
{
	struct wl_resource *r;
	int v;

	eachres(r, &pointers, w){
		v = wl_resource_get_version(r);
		if(v >= WL_POINTER_AXIS_SOURCE_SINCE_VERSION)
			wl_pointer_send_axis_source(r, WL_POINTER_AXIS_SOURCE_WHEEL);
		if(v >= WL_POINTER_AXIS_DISCRETE_SINCE_VERSION && v < 8)
			wl_pointer_send_axis_discrete(r, WL_POINTER_AXIS_VERTICAL_SCROLL, dir);
		wl_pointer_send_axis(r, ms, WL_POINTER_AXIS_VERTICAL_SCROLL, wl_fixed_from_int(10*dir));
	}
}

/* an m event of window w: x, y in its coordinates */
void
pointer(Win *w, int x, int y, int b, uint32_t ms)
{
	static struct { int bit; uint32_t code; } btn[] = {
		{1, Btnleft}, {2, Btnmiddle}, {4, Btnright},
	};
	struct wl_resource *r;
	Surface *s;
	int inside, i, old, ox, oy;

	if(w->s == NULL)
		return;
	/* a drag from this window takes the pointer (dnd.c) */
	if(dragpointer(w, x, y, b, ms)){
		pbuttons = b;
		return;
	}
	pwx = x;
	pwy = y;
	inside = x >= 0 && y >= 0 && x < w->w && y < w->h;
	/*
	 * the surface of the window's tree under it, in its coordinates;
	 * while a button is held, the one it was pressed on
	 */
	if(pwin == w && pbuttons != 0 && psurf != NULL){
		s = psurf;
		suborigin(s, &ox, &oy);
		x -= ox;
		y -= oy;
	}else
		s = pick(w->s, x, y, &x, &y);
	if(pwin != w || psurf != s){
		if(!inside && pwin != w)
			return;
		penter(w, s, x, y);
	}
	if(x != px || y != py){
		px = x;
		py = y;
		eachres(r, &pointers, w)
			wl_pointer_send_motion(r, ms, wl_fixed_from_int(x), wl_fixed_from_int(y));
	}
	old = pbuttons;
	for(i = 0; i < 3; i++)
		if((old ^ b) & btn[i].bit)
			pbutton(w, ms, btn[i].code, b & btn[i].bit);
	/* the wheel: a press is a step, up (8) or down (16) */
	if(b & 8 & ~old)
		pwheel(w, ms, -1);
	if(b & 16 & ~old)
		pwheel(w, ms, 1);
	pbuttons = b;
	eachres(r, &pointers, w)
		pframe(r);
	if(!inside && (b & 7) == 0)
		pleave();
}

static void
kenter(Win *w)
{
	struct wl_resource *r;
	struct wl_array keys;
	uint32_t serial;

	kwin = w;
	nheld = 0;
	mods = 0;
	wl_array_init(&keys);
	serial = wl_display_next_serial(display);
	eachres(r, &keyboards, w){
		wl_keyboard_send_enter(r, serial, w->s->res, &keys);
		wl_keyboard_send_modifiers(r, serial, 0, 0, 0, 0);
	}
	wl_array_release(&keys);
	selectionfocus(w);
}

static void
kleave(void)
{
	struct wl_resource *r;
	uint32_t serial;

	if(kwin == NULL)
		return;
	serial = wl_display_next_serial(display);
	eachres(r, &keyboards, kwin)
		wl_keyboard_send_leave(r, serial, kwin->s->res);
	kwin = NULL;
	nheld = 0;
	mods = 0;
}

/* an f event: window w got the focus (1) or lost it (0) */
void
focus(Win *w, int f)
{
	if(w->focus == f)
		return;
	w->focus = f;
	if(f){
		if(kwin != w){
			kleave();
			kenter(w);
		}
	}else if(kwin == w)
		kleave();
	focuschanged();	/* activated, or not; popups done */
}

/*
 * the window the pointer is in, if c's and a button is held there (a drag
 * may start); the point in it in *x, *y
 */
Win*
pointerheld(struct wl_client *c, int *x, int *y)
{
	if(pwin == NULL || pbuttons == 0 || winclient(pwin) != c)
		return NULL;
	*x = pwx;
	*y = pwy;
	return pwin;
}

/* the pointer leaves the surface it is in (a drag takes it) */
void
pointerleave(void)
{
	pleave();
}

/* surface s goes: the pointer is in none of its window's (no leave: it is gone) */
void
pointergone(Surface *s)
{
	if(psurf == s){
		psurf = NULL;
		pwin = NULL;
		pbuttons = 0;
	}
}

/* the window with the keyboard, nil if none */
Win*
keywin(void)
{
	return kwin;
}

/* the window goes: the pointer and the keyboard leave it */
void
seatforget(Win *w)
{
	if(curwin == w){
		curwin = NULL;
		curset = 0;
	}
	if(pwin == w)
		pleave();
	if(kwin == w)
		kleave();
}

static int
inset(uint32_t c, uint32_t *s, int n)
{
	int i;

	for(i = 0; i < n; i++)
		if(s[i] == c)
			return 1;
	return 0;
}

/* a rune of UTF-8 at p (n bytes there); its length, 0 if none */
static int
rune(char *p, int n, uint32_t *r)
{
	unsigned char *u;
	int l, i;

	u = (unsigned char*)p;
	if(n < 1)
		return 0;
	if(u[0] < 0x80){
		*r = u[0];
		return 1;
	}
	if((u[0] & 0xe0) == 0xc0){
		l = 2;
		*r = u[0] & 0x1f;
	}else if((u[0] & 0xf0) == 0xe0){
		l = 3;
		*r = u[0] & 0x0f;
	}else if((u[0] & 0xf8) == 0xf0){
		l = 4;
		*r = u[0] & 0x07;
	}else
		return 0;
	if(n < l)
		return 0;
	for(i = 1; i < l; i++){
		if((u[i] & 0xc0) != 0x80)
			return 0;
		*r = *r << 6 | (u[i] & 0x3f);
	}
	return l;
}

static void
key(Win *w, uint32_t code, int down)
{
	struct wl_resource *r;
	uint32_t serial;

	serial = wl_display_next_serial(display);
	eachres(r, &keyboards, w)
		wl_keyboard_send_key(r, serial, msec(), code,
			down ? WL_KEYBOARD_KEY_STATE_PRESSED : WL_KEYBOARD_KEY_STATE_RELEASED);
}

/* a kbd(3) message of window w: k or K and the runes of the keys held */
void
keys(Win *w, char *m, int n)
{
	struct wl_resource *r;
	uint32_t now[Maxkeys], c, rr, m2;
	int nnow, i, l;

	if(n < 1 || (m[0] != 'k' && m[0] != 'K'))
		return;	/* c: a rune composed or typed, not a key */
	if(kwin != w){
		kleave();
		kenter(w);
	}
	nnow = 0;
	for(i = 1; i < n; i += l){
		if((l = rune(m+i, n-i, &rr)) == 0)
			break;
		c = runekey(rr);
		if((int)c >= 0 && nnow < Maxkeys && !inset(c, now, nnow))
			now[nnow++] = c;
	}
	for(i = 0; i < nheld; i++)
		if(!inset(held[i], now, nnow))
			key(w, held[i], 0);
	for(i = 0; i < nnow; i++)
		if(!inset(now[i], held, nheld))
			key(w, now[i], 1);
	memcpy(held, now, nnow * sizeof now[0]);
	nheld = nnow;
	m2 = keymods(held, nheld);
	if(m2 != mods){
		mods = m2;
		eachres(r, &keyboards, w)
			wl_keyboard_send_modifiers(r, wl_display_next_serial(display), mods, 0, 0, 0);
	}
}

/* ---- the resources ---- */

static void
reslink(struct wl_resource *r)
{
	wl_list_remove(wl_resource_get_link(r));
}

static void
resrelease(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/*
 * ---- the pointer's picture: a cursor surface's buffer as cursor(3)'s
 * (16 by 16, each pixel black, white or clear) in the window's cursor file;
 * wsys shows it while the window has the focus (where the pointer's events
 * go) ----
 */

static void
putlong(unsigned char *p, int v)
{
	p[0] = v;
	p[1] = v>>8;
	p[2] = v>>16;
	p[3] = v>>24;
}

/*
 * cursor(3)'s clr and set for surface s from its buffer: a pixel at least
 * half opaque is black if dark, else white; a picture bigger than 16 by 16
 * is shrunk by a whole factor, a dark pixel of a block first (an outline
 * stays); s->cshrink is the factor, 0 if the pixels cannot be read (the
 * default cursor then); no buffer: nothing shown
 */
static void
mkcursor(Surface *s)
{
	Buffer *b;
	unsigned char *px, *p, *clr, *set;
	uint32_t v, a, l;
	int f, x, y, i, j, dark, light;

	memset(s->cbits, 0, sizeof s->cbits);
	s->cshrink = 1;
	if((b = s->buf) == NULL)
		return;
	if((px = pixels(b)) == NULL){
		s->cshrink = 0;
		return;
	}
	f = ((b->w > b->h ? b->w : b->h) + 15) / 16;
	clr = s->cbits;
	set = s->cbits + 32;
	for(y = 0; y < 16; y++)
		for(x = 0; x < 16; x++){
			dark = light = 0;
			for(j = 0; j < f; j++)
				for(i = 0; i < f; i++){
					if(x*f + i >= b->w || y*f + j >= b->h)
						continue;
					p = px + (y*f + j)*b->stride + 4*(x*f + i);
					v = p[0] | p[1]<<8 | p[2]<<16 | (uint32_t)p[3]<<24;
					a = b->format == WL_SHM_FORMAT_ARGB8888 ? v>>24 : 255;
					if(a < 128)
						continue;
					/* premultiplied: the colour's lightness is l/a */
					l = (299*(v>>16 & 255) + 587*(v>>8 & 255) + 114*(v & 255)) / 1000;
					if(l*255 < 128*a)
						dark = 1;
					else
						light = 1;
				}
			if(dark)
				set[2*y + x/8] |= 0x80 >> (x&7);
			else if(light)
				clr[2*y + x/8] |= 0x80 >> (x&7);
		}
	free(px);
	s->cshrink = f;
}

/* the cursor set for curwin to its cursor file (no bytes: the default) */
static void
cursorshow(void)
{
	unsigned char c[72];
	char path[64];
	int fd, n;

	if(curwin == NULL || curwin->gone || !curset)
		return;
	memset(c, 0, sizeof c);
	n = 72;
	if(cursurf != NULL){
		if(cursurf->cshrink == 0)
			n = 0;
		else{
			putlong(c, -curhx/cursurf->cshrink);
			putlong(c+4, -curhy/cursurf->cshrink);
			memcpy(c+8, cursurf->cbits, 64);
		}
	}
	snprintf(path, sizeof path, "/mnt/wsys/%d/cursor", curwin->id);
	if((fd = open(path, O_WRONLY)) < 0)
		return;
	dprint("cursor %d: %d bytes, hot spot %d %d\n", curwin->id, n, curhx, curhy);
	if(write(fd, c, n) != n)
		fprintf(stderr, "wlsys: %s: %s\n", path, strerror(errno));
	close(fd);
}

static void
setcursor(struct wl_client *c, struct wl_resource *r, uint32_t serial, struct wl_resource *surf, int32_t x, int32_t y)
{
	Surface *s;

	(void)serial;
	/* for the window the pointer is in, the client's (serial: its enter) */
	if(pwin == NULL || winclient(pwin) != c)
		return;
	s = surf != NULL ? wl_resource_get_user_data(surf) : NULL;
	if(s != NULL && s->role != Rnone && s->role != Rcursor){
		wl_resource_post_error(r, WL_POINTER_ERROR_ROLE, "the surface has another role");
		return;
	}
	if(s != NULL && s->role == Rnone){
		s->role = Rcursor;
		mkcursor(s);	/* a buffer committed before it had the role */
		if(s->buf != NULL)
			relbuf(s->buf);
	}
	cursurf = s;
	curhx = x;
	curhy = y;
	curwin = pwin;
	curset = 1;
	cursorshow();
}

/*
 * a commit of a cursor surface (attached: a buffer with it): its picture
 * read and the buffer released at once; the hot spot moved by -dx, -dy
 */
void
cursorcommit(Surface *s, int attached, int dx, int dy)
{
	if(attached){
		mkcursor(s);
		if(s->buf != NULL)
			relbuf(s->buf);
	}
	if(s != cursurf)
		return;
	curhx -= dx;
	curhy -= dy;
	cursorshow();
}

void
cursorgone(Surface *s)
{
	if(cursurf == s)
		cursurf = NULL;
}

static const struct wl_pointer_interface pointerimpl = {
	setcursor,
	resrelease,
};

static const struct wl_keyboard_interface keyboardimpl = {
	resrelease,
};

static const struct wl_touch_interface touchimpl = {
	resrelease,
};

static void
getpointer(struct wl_client *c, struct wl_resource *seat, uint32_t id)
{
	struct wl_resource *r;

	if((r = wl_resource_create(c, &wl_pointer_interface, wl_resource_get_version(seat), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &pointerimpl, NULL, reslink);
	wl_list_insert(&pointers, wl_resource_get_link(r));
}

static void
getkeyboard(struct wl_client *c, struct wl_resource *seat, uint32_t id)
{
	struct wl_resource *r;
	struct wl_array none;
	int fd;

	if((r = wl_resource_create(c, &wl_keyboard_interface, wl_resource_get_version(seat), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &keyboardimpl, NULL, reslink);
	wl_list_insert(&keyboards, wl_resource_get_link(r));
	if(keymapfd >= 0)
		wl_keyboard_send_keymap(r, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keymapfd, keymaplen);
	else if((fd = open("/dev/null", O_RDONLY)) >= 0){
		wl_keyboard_send_keymap(r, WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP, fd, 0);
		close(fd);
	}
	if(wl_resource_get_version(r) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION)
		wl_keyboard_send_repeat_info(r, 25, 600);
	if(kwin != NULL && winclient(kwin) == c){
		wl_array_init(&none);
		wl_keyboard_send_enter(r, wl_display_next_serial(display), kwin->s->res, &none);
		wl_keyboard_send_modifiers(r, wl_display_next_serial(display), mods, 0, 0, 0);
		wl_array_release(&none);
	}
}

static void
gettouch(struct wl_client *c, struct wl_resource *seat, uint32_t id)
{
	struct wl_resource *r;

	if((r = wl_resource_create(c, &wl_touch_interface, wl_resource_get_version(seat), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &touchimpl, NULL, NULL);
}

static const struct wl_seat_interface seatimpl = {
	getpointer,
	getkeyboard,
	gettouch,
	resrelease,
};

static void
seatbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_seat_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &seatimpl, NULL, NULL);
	wl_seat_send_capabilities(r, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
	if(version >= WL_SEAT_NAME_SINCE_VERSION)
		wl_seat_send_name(r, "seat0");
}

/* the keymap in a memfd: the clients map it (MAP_PRIVATE: a copy) */
static void
keymapinit(void)
{
	char *text, *p;
	int n, fd;

	if((n = mkkeymap("/dev/kbmap", &text)) < 0){
		fprintf(stderr, "wlsys: /dev/kbmap: no keyboard's tables; no keymap\n");
		return;
	}
	if((fd = memfd_create("wlsys-keymap", MFD_CLOEXEC)) < 0 || ftruncate(fd, n) < 0){
		fprintf(stderr, "wlsys: keymap: memfd: %s\n", strerror(errno));
		free(text);
		return;
	}
	p = mmap(NULL, n, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED){
		fprintf(stderr, "wlsys: keymap: mmap: %s\n", strerror(errno));
		close(fd);
		free(text);
		return;
	}
	memcpy(p, text, n);
	munmap(p, n);
	free(text);
	keymapfd = fd;
	keymaplen = n;
	dprint("keymap: %d bytes\n", n);
}

void
seatinit(void)
{
	wl_list_init(&pointers);
	wl_list_init(&keyboards);
	keymapinit();
	wl_global_create(display, &wl_seat_interface, 7, NULL, seatbind);
}
