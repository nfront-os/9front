/*
 * SPDX-License-Identifier: MIT
 *
 * wlclient: a Wayland client of wlsys (../9front-ui PLAN §12, stage 6.5),
 * run by wlsys in wsys -x -i (wlsys.rc).  An xdg toplevel with two buffers
 * in one pool (a memfd: a segment), red then blue, read back from wsys'
 * screen file in the window labelled with its title; a click
 * (/dev/mousein) as the pointer's enter, button press and release, its
 * leave; the keymap (mapped, saved as /tmp/keymap.xkb for the host's
 * xkbcommon) and keys (/dev/kbdin: x down and up, shift held: its
 * modifier); the window swept bigger by its ctl (a configure, a buffer of
 * that size, green); a cursor surface's picture in the window's cursor
 * file (16 by 16, 32 by 32 shrunk, none); the clipboard (wsys' snarf
 * buffer offered, a selection of ours snarfed, the older source
 * cancelled); the decoration (wsys': server side); a popup that grabs
 * (where its positioner put it, no border, the keyboard, done by a click
 * on its parent) and a tooltip (flipped onto the screen, the keyboard
 * stays); subsurfaces (composed by wlsys: synchronized until the parent's
 * commit, below the parent, blended, desynchronized, the pointer's enter
 * on one, an input region letting it through, gone again); drag and drop
 * from our toplevel into a second one (wlclient-b: enter, the icon at the
 * pointer, accept and the action, drop, the data through the source,
 * finish; a drag released over nothing cancelled); deleted in wsys: the
 * toplevel's close.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>
#include <xdg-decoration-unstable-v1-client-protocol.h>
#include <signal.h>

enum {
	W = 200, H = 150,
	Red = 0xff0000, Green = 0x00ff00, Blue = 0x0000ff, Magenta = 0xff00ff,
	Btnleft = 0x110,
	Keyx = 45,	/* x's Linux code (scancode 0x2d) */
};

static int nfail;
static struct wl_display *d;
static struct wl_compositor *comp;
static struct wl_shm *shm;
static struct xdg_wm_base *wm;
static struct wl_seat *seat;
static struct wl_output *output;
static int outw, outh;

static struct wl_surface *surf;
static struct xdg_surface *xsurf;
static struct xdg_toplevel *top;
static int confw = -1, confh = -1, activated;
static uint32_t confserial;
static int configured, closed;

static int released[4];
static int framedone;

static int entered, enterx, entery, left, pressed, unpressed;
static int keymapfd = -1, keymapsize, nkeymaps;
static int keydown, keyup, keymods, shifted;
static uint32_t enterserial, keyserial;
static struct wl_surface *kfocus;	/* the surface with the keyboard */
static int deactivated;			/* configures without activated */

static struct zxdg_decoration_manager_v1 *decomgr;
static uint32_t decomode;
static int decoconfigured;

static struct wl_data_device_manager *ddm;
static struct wl_data_offer *seloffer;
static int selections, offertext, sent, cancelled;

static struct wl_subcompositor *subcomp;
static struct wl_surface *penters;	/* the surface the pointer entered last */
static int penterx, pentery;
static int pmx, pmy;		/* the pointer's last motion */
static struct wl_surface *dndsurf;	/* a drag's enter: where */
static struct wl_data_offer *dndoffer;
static uint32_t dndserial;
static int dndenters, dndmotions, dnddrops, nsrcdropped, nsrcfinished;
static uint32_t srcact;
static int popconf, popdone, popx, popy, popw, poph;
static uint32_t popserial;

static void
check(const char *what, int ok, const char *fmt, ...)
{
	va_list a;

	if(ok)
		printf("ok %s\n", what);
	else{
		printf("FAIL %s", what);
		if(fmt != NULL){
			printf(": ");
			va_start(a, fmt);
			vprintf(fmt, a);
			va_end(a);
		}
		printf("\n");
		nfail++;
	}
	fflush(stdout);
}

/* dispatch until *flag or about 5 s went by */
static int
waitfor(int *flag)
{
	int i;

	for(i = 0; i < 100 && !*flag; i++){
		wl_display_flush(d);
		if(wl_display_roundtrip(d) < 0)
			return 0;
		if(!*flag)
			usleep(50*1000);
	}
	return *flag;
}

/* ---- the listeners ---- */

static void
xdgping(void *data, struct xdg_wm_base *b, uint32_t serial)
{
	xdg_wm_base_pong(b, serial);
}
static const struct xdg_wm_base_listener wmlistener = { xdgping };

static void
xsconfigure(void *data, struct xdg_surface *x, uint32_t serial)
{
	confserial = serial;
	configured++;
}
static const struct xdg_surface_listener xslistener = { xsconfigure };

static void
topconfigure(void *data, struct xdg_toplevel *t, int32_t w, int32_t h, struct wl_array *states)
{
	uint32_t *s;

	confw = w;
	confh = h;
	activated = 0;
	wl_array_for_each(s, states)
		if(*s == XDG_TOPLEVEL_STATE_ACTIVATED)
			activated = 1;
	if(!activated)
		deactivated++;
}
static void topclose(void *data, struct xdg_toplevel *t) { closed = 1; }
static void topbounds(void *data, struct xdg_toplevel *t, int32_t w, int32_t h) {}
static void topcaps(void *data, struct xdg_toplevel *t, struct wl_array *caps) {}
static const struct xdg_toplevel_listener toplistener = { topconfigure, topclose, topbounds, topcaps };

static void
bufrelease(void *data, struct wl_buffer *b)
{
	released[(intptr_t)data]++;
}
static const struct wl_buffer_listener buflistener = { bufrelease };

static void
done(void *data, struct wl_callback *cb, uint32_t ms)
{
	framedone = 1;
	wl_callback_destroy(cb);
}
static const struct wl_callback_listener framelistener = { done };

static void
outgeometry(void *data, struct wl_output *o, int32_t x, int32_t y, int32_t pw, int32_t ph, int32_t sub, const char *make, const char *model, int32_t t) {}
static void
outmode(void *data, struct wl_output *o, uint32_t flags, int32_t w, int32_t h, int32_t refresh)
{
	if(flags & WL_OUTPUT_MODE_CURRENT){
		outw = w;
		outh = h;
	}
}
static void outdone(void *data, struct wl_output *o) {}
static void outscale(void *data, struct wl_output *o, int32_t s) {}
static void outname(void *data, struct wl_output *o, const char *s) {}
static void outdesc(void *data, struct wl_output *o, const char *s) {}
static const struct wl_output_listener outlistener = { outgeometry, outmode, outdone, outscale, outname, outdesc };

static void
penter(void *data, struct wl_pointer *p, uint32_t serial, struct wl_surface *s, wl_fixed_t x, wl_fixed_t y)
{
	enterserial = serial;
	penters = s;
	penterx = wl_fixed_to_int(x);
	pentery = wl_fixed_to_int(y);
	if(s == surf){
		entered++;
		enterx = wl_fixed_to_int(x);
		entery = wl_fixed_to_int(y);
	}
}
static void pleave(void *data, struct wl_pointer *p, uint32_t serial, struct wl_surface *s) { if(s == surf) left++; }
static void pmotion(void *data, struct wl_pointer *p, uint32_t ms, wl_fixed_t x, wl_fixed_t y) { pmx = wl_fixed_to_int(x); pmy = wl_fixed_to_int(y); }
static void
pbutton(void *data, struct wl_pointer *p, uint32_t serial, uint32_t ms, uint32_t b, uint32_t st)
{
	keyserial = serial;
	if(b == Btnleft && st == WL_POINTER_BUTTON_STATE_PRESSED)
		pressed++;
	if(b == Btnleft && st == WL_POINTER_BUTTON_STATE_RELEASED)
		unpressed++;
}
static void paxis(void *data, struct wl_pointer *p, uint32_t ms, uint32_t a, wl_fixed_t v) {}
static void pframe(void *data, struct wl_pointer *p) {}
static void paxissource(void *data, struct wl_pointer *p, uint32_t s) {}
static void paxisstop(void *data, struct wl_pointer *p, uint32_t ms, uint32_t a) {}
static void paxisdiscrete(void *data, struct wl_pointer *p, uint32_t a, int32_t v) {}
static void paxis120(void *data, struct wl_pointer *p, uint32_t a, int32_t v) {}
static void paxisrel(void *data, struct wl_pointer *p, uint32_t a, uint32_t dir) {}
static const struct wl_pointer_listener plistener = {
	penter, pleave, pmotion, pbutton, paxis, pframe, paxissource, paxisstop, paxisdiscrete, paxis120, paxisrel,
};

static void
kkeymap(void *data, struct wl_keyboard *k, uint32_t format, int32_t fd, uint32_t size)
{
	if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1){
		close(fd);
		return;
	}
	if(keymapfd >= 0)
		close(keymapfd);
	keymapfd = fd;
	keymapsize = size;
	nkeymaps++;
}
static void kenter(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s, struct wl_array *keys) { kfocus = s; }
static void kleave(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s) { if(kfocus == s) kfocus = NULL; }
static void
kkey(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t ms, uint32_t key, uint32_t st)
{
	keyserial = serial;
	if(key == Keyx && st == WL_KEYBOARD_KEY_STATE_PRESSED)
		keydown++;
	if(key == Keyx && st == WL_KEYBOARD_KEY_STATE_RELEASED)
		keyup++;
}
static void
kmods(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group)
{
	keymods = dep;
	if(dep & 1)
		shifted = 1;
}
static void krepeat(void *data, struct wl_keyboard *k, int32_t rate, int32_t delay) {}
static const struct wl_keyboard_listener klistener = { kkeymap, kenter, kleave, kkey, kmods, krepeat };

/* ---- the decoration, the clipboard, popups ---- */

static void
decoconfigure(void *data, struct zxdg_toplevel_decoration_v1 *t, uint32_t mode)
{
	decomode = mode;
	decoconfigured++;
}
static const struct zxdg_toplevel_decoration_v1_listener decolistener = { decoconfigure };

static void
offeroffer(void *data, struct wl_data_offer *o, const char *mime)
{
	if(strcmp(mime, "text/plain;charset=utf-8") == 0)
		offertext++;
}
static void offeractions(void *data, struct wl_data_offer *o, uint32_t a) {}
static void offeraction(void *data, struct wl_data_offer *o, uint32_t a) {}
static const struct wl_data_offer_listener offerlistener = { offeroffer, offeractions, offeraction };

static void
devoffer(void *data, struct wl_data_device *dd, struct wl_data_offer *o)
{
	offertext = 0;
	wl_data_offer_add_listener(o, &offerlistener, NULL);
}
static void
deventer(void *data, struct wl_data_device *dd, uint32_t serial, struct wl_surface *s, wl_fixed_t x, wl_fixed_t y, struct wl_data_offer *o)
{
	dndsurf = s;
	dndoffer = o;
	dndserial = serial;
	dndenters++;
}
static void devleave(void *data, struct wl_data_device *dd) { dndsurf = NULL; }
static void devmotion(void *data, struct wl_data_device *dd, uint32_t ms, wl_fixed_t x, wl_fixed_t y) { dndmotions++; }
static void devdrop(void *data, struct wl_data_device *dd) { dnddrops++; }
static void
devselection(void *data, struct wl_data_device *dd, struct wl_data_offer *o)
{
	if(seloffer != NULL && seloffer != o)
		wl_data_offer_destroy(seloffer);
	seloffer = o;
	selections++;
}
static const struct wl_data_device_listener devlistener = { devoffer, deventer, devleave, devmotion, devdrop, devselection };

static void srctarget(void *data, struct wl_data_source *s, const char *mime) {}
static void
srcsend(void *data, struct wl_data_source *s, const char *mime, int32_t fd)
{
	char *text = data;

	write(fd, text, strlen(text));
	close(fd);
	sent++;
}
static void srccancelled(void *data, struct wl_data_source *s) { cancelled++; }
static void srcdropped(void *data, struct wl_data_source *s) { nsrcdropped++; }
static void srcfinished(void *data, struct wl_data_source *s) { nsrcfinished++; }
static void srcaction(void *data, struct wl_data_source *s, uint32_t a) { srcact = a; }
static const struct wl_data_source_listener srclistener = { srctarget, srcsend, srccancelled, srcdropped, srcfinished, srcaction };

static void
popxsconfigure(void *data, struct xdg_surface *x, uint32_t serial)
{
	popserial = serial;
	popconf++;
}
static const struct xdg_surface_listener popxslistener = { popxsconfigure };

static void
popconfigure(void *data, struct xdg_popup *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
	popx = x;
	popy = y;
	popw = w;
	poph = h;
}
static void popupdone(void *data, struct xdg_popup *p) { popdone++; }
static void poprepositioned(void *data, struct xdg_popup *p, uint32_t token) {}
static const struct xdg_popup_listener poplistener = { popconfigure, popupdone, poprepositioned };

static void
global(void *data, struct wl_registry *reg, uint32_t name, const char *iface, uint32_t version)
{
	if(strcmp(iface, wl_compositor_interface.name) == 0)
		comp = wl_registry_bind(reg, name, &wl_compositor_interface, 4);
	else if(strcmp(iface, wl_shm_interface.name) == 0)
		shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
	else if(strcmp(iface, xdg_wm_base_interface.name) == 0){
		wm = wl_registry_bind(reg, name, &xdg_wm_base_interface, 2);
		xdg_wm_base_add_listener(wm, &wmlistener, NULL);
	}else if(strcmp(iface, zxdg_decoration_manager_v1_interface.name) == 0)
		decomgr = wl_registry_bind(reg, name, &zxdg_decoration_manager_v1_interface, 1);
	else if(strcmp(iface, wl_subcompositor_interface.name) == 0)
		subcomp = wl_registry_bind(reg, name, &wl_subcompositor_interface, 1);
	else if(strcmp(iface, wl_data_device_manager_interface.name) == 0)
		ddm = wl_registry_bind(reg, name, &wl_data_device_manager_interface, 3);
	else if(strcmp(iface, wl_seat_interface.name) == 0)
		seat = wl_registry_bind(reg, name, &wl_seat_interface, 5);
	else if(strcmp(iface, wl_output_interface.name) == 0){
		output = wl_registry_bind(reg, name, &wl_output_interface, 2);
		wl_output_add_listener(output, &outlistener, NULL);
	}
}
static void global_remove(void *data, struct wl_registry *reg, uint32_t name) {}
static const struct wl_registry_listener reglistener = { global, global_remove };

/* ---- wsys, from the outside: its window of ours, its screen ---- */

static int winid = -1;
static int wx0, wy0, wx1, wy1;	/* its content, on the screen */

static int
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, O_RDONLY)) < 0)
		return -1;
	m = read(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return -1;
	buf[m] = 0;
	return m;
}

static int
writefile(char *path, char *msg, int n)
{
	int fd, m;

	if((fd = open(path, O_WRONLY)) < 0)
		return -1;
	m = write(fd, msg, n);
	close(fd);
	return m == n ? 0 : -1;
}

/* the window of wsys labelled "wlclient", and where it is */
static int
findwin(void)
{
	DIR *dir;
	struct dirent *e;
	char path[128], buf[256];
	int id;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	winid = -1;
	while((e = readdir(dir)) != NULL){
		if((id = atoi(e->d_name)) <= 0)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/label", id);
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, "wlclient") == 0)
			winid = id;
	}
	closedir(dir);
	if(winid < 0)
		return -1;
	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", winid);
	if(readfile(path, buf, sizeof buf) < 0 ||
	   sscanf(buf, "%d %d %d %d %d", &id, &wx0, &wy0, &wx1, &wy1) != 5)
		return -1;
	return 0;
}

static int
winctl(char *msg)
{
	char path[64];

	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", winid);
	return writefile(path, msg, strlen(msg));
}

/* the top of wsys' screen (in a window of another, not 0): its header's y0 */
static int
screentop(void)
{
	char hdr[61];
	int fd, x0, y0;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return 0;
	if(read(fd, hdr, 60) != 60){
		close(fd);
		return 0;
	}
	close(fd);
	hdr[60] = 0;
	if(sscanf(hdr, "%*s %d %d", &x0, &y0) != 2)
		return 0;
	return y0;
}

/* the screen's pixel at x, y (screen coordinates) as 0xRRGGBB; -1 if not read */
static long
peek(int x, int y)
{
	static char *img;
	char hdr[61], chan[16];
	int fd, sx0, sy0, sx1, sy1, depth, n, total, m;
	unsigned char *p;
	long off;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return -1;
	if(read(fd, hdr, 60) != 60){
		close(fd);
		return -1;
	}
	hdr[60] = 0;
	if(sscanf(hdr, "%15s %d %d %d %d", chan, &sx0, &sy0, &sx1, &sy1) != 5){
		close(fd);
		return -1;
	}
	if(strcmp(chan, "x8r8g8b8") == 0 || strcmp(chan, "a8r8g8b8") == 0)
		depth = 32;
	else if(strcmp(chan, "r8g8b8") == 0)
		depth = 24;
	else if(strcmp(chan, "r5g6b5") == 0)
		depth = 16;
	else{
		fprintf(stderr, "wlclient: screen chan %s\n", chan);
		close(fd);
		return -1;
	}
	total = (sx1 - sx0) * (sy1 - sy0) * (depth/8);
	free(img);
	if((img = malloc(total)) == NULL){
		close(fd);
		return -1;
	}
	for(n = 0; n < total; n += m)
		if((m = read(fd, img + n, total - n)) <= 0)
			break;
	close(fd);
	if(n < total || x < sx0 || y < sy0 || x >= sx1 || y >= sy1)
		return -1;
	off = ((long)(y - sy0) * (sx1 - sx0) + (x - sx0)) * (depth/8);
	p = (unsigned char*)img + off;
	if(depth == 16){
		unsigned v = p[0] | p[1]<<8, r = v>>11 & 31, g = v>>5 & 63, b = v & 31;
		return (long)(r<<3 | r>>2)<<16 | (g<<2 | g>>4)<<8 | (b<<3 | b>>2);
	}
	return (long)p[2]<<16 | p[1]<<8 | p[0];
}

static int
near(long a, long b)
{
	int i, x;

	if(a < 0)
		return 0;
	for(i = 0; i < 24; i += 8){
		x = (int)(a>>i & 0xff) - (int)(b>>i & 0xff);
		if(x < -8 || x > 8)
			return 0;
	}
	return 1;
}

/* the screen at window coordinates x, y is rgb, within a second */
static void
want(int x, int y, long rgb, char *what)
{
	char buf[128];
	long v;
	int i;

	v = -1;
	for(i = 0; i < 50; i++){
		if(near(v = peek(wx0 + x, wy0 + y), rgb))
			break;
		usleep(20*1000);
	}
	snprintf(buf, sizeof buf, "the window at %d,%d: %s", x, y, what);
	check(buf, near(v, rgb), "%06lx, want %06lx", v, rgb);
}

/* the kernel keeps the buttons of each open of mousein: one for all */
static void
mouse(int x, int y, int b)
{
	static int fd = -1;
	char msg[64];
	int n;

	if(fd < 0 && (fd = open("/dev/mousein", O_WRONLY)) < 0){
		check("open /dev/mousein", 0, "%s", strerror(errno));
		return;
	}
	n = snprintf(msg, sizeof msg, "A %d %d %d", x, y, b);
	if(write(fd, msg, n) != n)
		check("write /dev/mousein", 0, "%s", strerror(errno));
}

/* a kbd(3) message to kbdfs: r (down) or R (up) and a rune in UTF-8 */
static void
kbdin(int c, char *rune)
{
	char msg[16];
	int n;

	n = snprintf(msg, sizeof msg, "%c%s", c, rune) + 1;
	if(writefile("/dev/kbdin", msg, n) < 0)
		check("write /dev/kbdin", 0, "%s", strerror(errno));
}

/* ---- buffers ---- */

static struct wl_buffer*
mkbuffer(int fd, struct wl_shm_pool *pool, int off, int w, int h, intptr_t i)
{
	struct wl_buffer *b;

	b = wl_shm_pool_create_buffer(pool, off, w, h, w*4, WL_SHM_FORMAT_XRGB8888);
	wl_buffer_add_listener(b, &buflistener, (void*)i);
	return b;
}

static void
fill(uint32_t *px, int n, uint32_t rgb)
{
	int i;

	for(i = 0; i < n; i++)
		px[i] = 0xff000000u | rgb;
}

static void
present(struct wl_buffer *b, int w, int h)
{
	struct wl_callback *cb;

	framedone = 0;
	wl_surface_attach(surf, b, 0, 0);
	wl_surface_damage_buffer(surf, 0, 0, w, h);
	cb = wl_surface_frame(surf);
	wl_callback_add_listener(cb, &framelistener, NULL);
	wl_surface_commit(surf);
}

/* the window of wsys labelled l: its id and place on the screen; -1 if none */
static int
labelled(char *l, int *id, int *x0, int *y0, int *x1, int *y1)
{
	DIR *dir;
	struct dirent *e;
	char path[128], buf[256];
	int n, i;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	n = -1;
	while((e = readdir(dir)) != NULL){
		if((i = atoi(e->d_name)) <= 0)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/label", i);
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, l) == 0)
			n = i;
	}
	closedir(dir);
	if(n < 0)
		return -1;
	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", n);
	if(readfile(path, buf, sizeof buf) < 0 || sscanf(buf, "%d %d %d %d %d", id, x0, y0, x1, y1) != 5)
		return -1;
	return 0;
}

/* dispatch until *flag changes from was, about 2 s */
static int
waitchange(int *flag, int was)
{
	int i;

	for(i = 0; i < 40 && *flag == was; i++){
		wl_display_roundtrip(d);
		usleep(50*1000);
	}
	return *flag != was;
}

/* a pool of n bytes (a memfd), mapped; nil if not */
static uint32_t*
mkpool(int n, struct wl_shm_pool **pool)
{
	uint32_t *px;
	int fd;

	fd = memfd_create("wlclientpool", MFD_CLOEXEC);
	if(fd < 0 || ftruncate(fd, n) < 0 ||
	   (px = mmap(NULL, n, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		check("a pool: memfd_create, ftruncate, mmap", 0, "%s", strerror(errno));
		return NULL;
	}
	*pool = wl_shm_create_pool(shm, fd, n);
	return px;
}

/* the 72 bytes in our window's cursor file (cursor(3)); how many */
static int
readcursor(unsigned char *c)
{
	char path[64];
	int fd, n;

	memset(c, 0xee, 72);
	snprintf(path, sizeof path, "/mnt/wsys/%d/cursor", winid);
	if((fd = open(path, O_RDONLY)) < 0)
		return -1;
	n = read(fd, c, 72);
	close(fd);
	return n;
}

static int
getlong(unsigned char *p)
{
	return (int)(p[0] | p[1]<<8 | p[2]<<16 | (uint32_t)p[3]<<24);
}

/* the cursor's offset and its 16 rows of set and clr bits, as wanted */
static int
iscursor(unsigned char *c, int ox, int oy, uint16_t *set, uint16_t *clr)
{
	int y;

	if(getlong(c) != ox || getlong(c+4) != oy)
		return 0;
	for(y = 0; y < 16; y++)
		if((c[8+2*y]<<8 | c[9+2*y]) != clr[y] || (c[40+2*y]<<8 | c[41+2*y]) != set[y])
			return 0;
	return 1;
}

static char*
snarfpath(void)
{
	static char path[64];

	snprintf(path, sizeof path, "/mnt/wsys/%d/snarf", winid);
	return path;
}

static void onalarm(int sig) { (void)sig; }

/* what comes through fd until its end, in 5 s */
static int
readall(int fd, char *buf, int n)
{
	int m, k;

	signal(SIGALRM, onalarm);
	alarm(5);
	for(m = 0; m < n - 1; m += k)
		if((k = read(fd, buf + m, n - 1 - m)) <= 0)
			break;
	alarm(0);
	buf[m] = 0;
	return m;
}

/* the keymap's line of key <K45> (x's) has sym first */
static int
k45is(char *sym)
{
	char *km, *l, *e, want[16];
	int ok;

	if(keymapfd < 0 || (km = mmap(NULL, keymapsize, PROT_READ, MAP_PRIVATE, keymapfd, 0)) == MAP_FAILED)
		return 0;
	ok = 0;
	snprintf(want, sizeof want, "[ %s,", sym);
	if((l = strstr(km, "key <K45>")) != NULL && (e = strchr(l, '\n')) != NULL){
		*e = 0;	/* MAP_PRIVATE: our copy */
		ok = strstr(l, want) != NULL;
	}
	munmap(km, keymapsize);
	return ok;
}

/* a popup of our toplevel by p's rules, configured; its xdg_surface in *xs */
static struct xdg_popup*
mkpopup(struct wl_surface *ps, struct xdg_positioner *p, struct xdg_surface **xs, int grab)
{
	struct xdg_popup *pop;

	*xs = xdg_wm_base_get_xdg_surface(wm, ps);
	xdg_surface_add_listener(*xs, &popxslistener, NULL);
	pop = xdg_surface_get_popup(*xs, xsurf, p);
	xdg_popup_add_listener(pop, &poplistener, NULL);
	if(grab)
		xdg_popup_grab(pop, seat, keyserial);
	popconf = popdone = 0;
	wl_surface_commit(ps);
	return pop;
}

int
main(void)
{
	struct wl_registry *reg;
	struct wl_shm_pool *pool, *pool2;
	struct wl_buffer *b0, *b1, *b2;
	struct wl_pointer *ptr;
	struct wl_keyboard *kbd;
	struct wl_surface *cursurf, *ps;
	struct wl_shm_pool *cpool, *ppool;
	struct wl_buffer *c16, *c32, *pb;
	struct zxdg_toplevel_decoration_v1 *deco;
	struct wl_data_device *dd;
	struct wl_data_source *src, *src2;
	struct xdg_positioner *pos;
	struct xdg_surface *pxs;
	struct xdg_popup *pop;
	struct wl_subsurface *sub;
	struct wl_surface *ss;
	struct wl_shm_pool *spool;
	struct wl_buffer *sb, *sw, *sg, *bb, *ib;
	struct wl_region *rg;
	struct wl_surface *bs, *is;
	struct xdg_surface *bxs;
	struct xdg_toplevel *btop;
	struct wl_shm_pool *bpool;
	uint32_t *spx, *bpx;
	int bid, bx0, by0, bx1, by1;
	unsigned char cur[72];
	uint16_t set[16], clr[16];
	uint32_t *px, *px2, *cpx, *ppx;
	char *km, msg[64], buf[256];
	int fd, fd2, size, ok, i, j, p[2], y;

	if((d = wl_display_connect(NULL)) == NULL){
		check("wl_display_connect $WAYLAND_DISPLAY", 0, "%s", strerror(errno));
		return 1;
	}
	check("connected to $WAYLAND_DISPLAY", 1, NULL);
	reg = wl_display_get_registry(d);
	wl_registry_add_listener(reg, &reglistener, NULL);
	wl_display_roundtrip(d);
	wl_display_roundtrip(d);
	check("globals: wl_compositor wl_shm xdg_wm_base wl_seat wl_output",
		comp && shm && wm && seat && output, NULL);
	if(!(comp && shm && wm && seat && output))
		return 1;
	check("the output: wsys' screen", outw > 0 && outh > 0, "%dx%d", outw, outh);
	ptr = wl_seat_get_pointer(seat);
	wl_pointer_add_listener(ptr, &plistener, NULL);
	kbd = wl_seat_get_keyboard(seat);
	wl_keyboard_add_listener(kbd, &klistener, NULL);

	/* a toplevel: its first commit has no buffer, a configure answers */
	surf = wl_compositor_create_surface(comp);
	xsurf = xdg_wm_base_get_xdg_surface(wm, surf);
	xdg_surface_add_listener(xsurf, &xslistener, NULL);
	top = xdg_surface_get_toplevel(xsurf);
	xdg_toplevel_add_listener(top, &toplistener, NULL);
	xdg_toplevel_set_title(top, "wlclient");
	/* we would draw our own decoration: wsys draws them (server side) */
	deco = NULL;
	if(decomgr != NULL){
		deco = zxdg_decoration_manager_v1_get_toplevel_decoration(decomgr, top);
		zxdg_toplevel_decoration_v1_add_listener(deco, &decolistener, NULL);
		zxdg_toplevel_decoration_v1_set_mode(deco, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
	}
	wl_surface_commit(surf);
	check("the first configure", waitfor(&configured), NULL);
	check("its size: ours to choose (0 0)", confw == 0 && confh == 0, "%d %d", confw, confh);
	check("a decoration manager", decomgr != NULL, NULL);
	check("the decoration: wsys' (server side)", decoconfigured && decomode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE,
		"%d configures, mode %u", decoconfigured, decomode);
	xdg_surface_ack_configure(xsurf, confserial);

	/* two buffers in a pool: a memfd, a segment */
	size = 2*W*H*4;
	fd = memfd_create("wlclient", MFD_CLOEXEC);
	if(fd < 0 || ftruncate(fd, size) < 0 ||
	   (px = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		check("a pool: memfd_create, ftruncate, mmap", 0, "%s", strerror(errno));
		return 1;
	}
	fill(px, W*H, Red);
	fill(px + W*H, W*H, Blue);
	pool = wl_shm_create_pool(shm, fd, size);
	b0 = mkbuffer(fd, pool, 0, W, H, 0);
	b1 = mkbuffer(fd, pool, W*H*4, W, H, 1);

	present(b0, W, H);
	check("buffer 0 presented: its frame done", waitfor(&framedone), NULL);
	ok = findwin() == 0;
	check("a window of wsys labelled with the title", ok, NULL);
	if(!ok)
		return 1;
	check("its size: the buffer's", wx1 - wx0 == W && wy1 - wy0 == H, "%dx%d", wx1 - wx0, wy1 - wy0);
	want(10, 10, Red, "buffer 0, red (the pool's first)");
	want(W-1, H-1, Red, "buffer 0's far corner");
	present(b1, W, H);
	check("buffer 1 presented: its frame done", waitfor(&framedone), NULL);
	want(10, 10, Blue, "buffer 1, blue (the pool's second, at an offset)");
	check("both buffers released", waitfor(&released[1]) && released[0] >= 1, "%d %d", released[0], released[1]);

	/* the pointer: a click in the window, then the mouse away */
	mouse(wx0 + 50, wy0 + 40, 1);
	usleep(50*1000);
	mouse(wx0 + 50, wy0 + 40, 0);
	check("the pointer entered", waitfor(&entered), NULL);
	check("at the click", enterx == 50 && entery == 40, "%d,%d", enterx, entery);
	check("button 1: BTN_LEFT pressed", waitfor(&pressed), NULL);
	check("and released", waitfor(&unpressed), NULL);

	/*
	 * the pointer's picture: 16x16, the top left quarter black, the top
	 * right white, the rest clear, its hot spot at 3 4; then 32x32, its
	 * top left quarter black (shrunk by 2: 8x8); then none
	 */
	cursurf = wl_compositor_create_surface(comp);
	if((cpx = mkpool(16*16*4 + 32*32*4, &cpool)) == NULL)
		return 1;
	for(y = 0; y < 16; y++)
		for(i = 0; i < 16; i++)
			cpx[16*y + i] = y >= 8 ? 0 : i < 8 ? 0xff000000u : 0xffffffffu;
	for(y = 0; y < 32; y++)
		for(i = 0; i < 32; i++)
			cpx[256 + 32*y + i] = y < 16 && i < 16 ? 0xff000000u : 0;
	c16 = wl_shm_pool_create_buffer(cpool, 0, 16, 16, 64, WL_SHM_FORMAT_ARGB8888);
	wl_buffer_add_listener(c16, &buflistener, (void*)3);
	c32 = wl_shm_pool_create_buffer(cpool, 256*4, 32, 32, 128, WL_SHM_FORMAT_ARGB8888);
	wl_surface_attach(cursurf, c16, 0, 0);
	wl_surface_damage_buffer(cursurf, 0, 0, 16, 16);
	wl_surface_commit(cursurf);
	wl_pointer_set_cursor(ptr, enterserial, cursurf, 3, 4);
	wl_display_roundtrip(d);
	for(y = 0; y < 16; y++){
		set[y] = y < 8 ? 0xff00 : 0;
		clr[y] = y < 8 ? 0x00ff : 0;
	}
	ok = readcursor(cur) == 72 && iscursor(cur, -3, -4, set, clr);
	check("the cursor in the window's cursor file (16x16, hot spot 3 4)", ok, "offset %d %d, row 0 clr %02x%02x set %02x%02x",
		getlong(cur), getlong(cur+4), cur[8], cur[9], cur[40], cur[41]);
	check("its buffer released at once (read)", waitfor(&released[3]), NULL);
	wl_surface_attach(cursurf, c32, 0, 0);
	wl_surface_damage_buffer(cursurf, 0, 0, 32, 32);
	wl_surface_commit(cursurf);
	wl_pointer_set_cursor(ptr, enterserial, cursurf, 10, 6);
	wl_display_roundtrip(d);
	for(y = 0; y < 16; y++){
		set[y] = y < 8 ? 0xff00 : 0;
		clr[y] = 0;
	}
	ok = readcursor(cur) == 72 && iscursor(cur, -5, -3, set, clr);
	check("a 32x32 cursor shrunk by 2 (hot spot 10 6: 5 3)", ok, "offset %d %d, row 0 set %02x%02x",
		getlong(cur), getlong(cur+4), cur[40], cur[41]);
	wl_pointer_set_cursor(ptr, enterserial, NULL, 0, 0);
	wl_display_roundtrip(d);
	memset(set, 0, sizeof set);
	memset(clr, 0, sizeof clr);
	ok = readcursor(cur) == 72 && iscursor(cur, 0, 0, set, clr);
	check("no cursor surface: none shown (all clear)", ok, NULL);
	wl_surface_destroy(cursurf);
	wl_buffer_destroy(c16);
	wl_buffer_destroy(c32);
	wl_shm_pool_destroy(cpool);

	mouse(wx0 - 30 > 0 ? wx0 - 30 : wx1 + 30, wy0 + 40, 0);
	check("the pointer left", waitfor(&left), NULL);

	/* the keyboard: the keymap, x down and up, shift held */
	check("a keymap (XKB)", waitfor(&keymapsize) && keymapfd >= 0, NULL);
	km = mmap(NULL, keymapsize, PROT_READ, MAP_PRIVATE, keymapfd, 0);
	ok = km != MAP_FAILED && strncmp(km, "xkb_keymap {", 12) == 0 && strstr(km, "<K45>") != NULL;
	check("mapped (MAP_PRIVATE): an xkb_keymap with x's key", ok, NULL);
	if(km != MAP_FAILED){
		if((fd2 = open("/tmp/keymap.xkb", O_WRONLY|O_CREAT|O_TRUNC, 0666)) >= 0){
			write(fd2, km, strnlen(km, keymapsize));
			close(fd2);
		}
		munmap(km, keymapsize);
	}
	kbdin('r', "x");
	kbdin('R', "x");
	check("x pressed (its Linux code)", waitfor(&keydown), NULL);
	check("and released", waitfor(&keyup), NULL);
	kbdin('r', "\xef\x80\x96");	/* Kshift */
	check("shift held: the modifier Shift", waitfor(&shifted), "%#x", keymods);
	kbdin('R', "\xef\x80\x96");

	/*
	 * the clipboard: wsys' snarf buffer offered (a Plan 9 program's), a
	 * selection of ours read into it, the older source cancelled by a newer
	 */
	check("a data device manager", ddm != NULL, NULL);
	if(ddm == NULL)
		return 1;
	ok = writefile(snarfpath(), "from plan9", 10) == 0;
	check("the snarf buffer written (as a Plan 9 program)", ok, "%s", strerror(errno));
	dd = wl_data_device_manager_get_data_device(ddm, seat);
	wl_data_device_add_listener(dd, &devlistener, NULL);
	check("the selection offered (we have the keyboard)", waitfor(&selections) && seloffer != NULL, NULL);
	check("as text/plain;charset=utf-8", offertext, NULL);
	buf[0] = 0;
	if(seloffer != NULL && pipe(p) == 0){
		wl_data_offer_receive(seloffer, "text/plain;charset=utf-8", p[1]);
		close(p[1]);
		wl_display_flush(d);
		readall(p[0], buf, sizeof buf);
		close(p[0]);
	}
	check("received: the snarf buffer", strcmp(buf, "from plan9") == 0, "%s", buf);
	src = wl_data_device_manager_create_data_source(ddm);
	wl_data_source_add_listener(src, &srclistener, "from wayland");
	wl_data_source_offer(src, "text/plain");
	wl_data_source_offer(src, "text/plain;charset=utf-8");
	wl_data_device_set_selection(dd, src, keyserial);
	check("our selection read by wlsys", waitfor(&sent), NULL);
	for(i = 0; i < 50; i++){
		if(readfile(snarfpath(), buf, sizeof buf) >= 0 && strcmp(buf, "from wayland") == 0)
			break;
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("our selection in the snarf buffer", strcmp(buf, "from wayland") == 0, "%s", buf);
	src2 = wl_data_device_manager_create_data_source(ddm);
	wl_data_source_add_listener(src2, &srclistener, "again");
	wl_data_source_offer(src2, "UTF8_STRING");
	wl_data_device_set_selection(dd, src2, keyserial);
	check("a newer selection: the older source cancelled", waitfor(&cancelled), NULL);
	wl_data_source_destroy(src);
	/* unset by us: emptied while it holds our text, not when a Plan 9 program snarfed since */
	for(i = 0; i < 50; i++){
		if(readfile(snarfpath(), buf, sizeof buf) >= 0 && strcmp(buf, "again") == 0)
			break;
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("the newer selection in the snarf buffer", strcmp(buf, "again") == 0, "%s", buf);
	wl_data_device_set_selection(dd, NULL, keyserial);
	wl_display_roundtrip(d);
	ok = readfile(snarfpath(), buf, sizeof buf) == 0;
	check("our selection unset: the snarf buffer empty", ok, "%s", buf);
	src = wl_data_device_manager_create_data_source(ddm);
	wl_data_source_add_listener(src, &srclistener, "third");
	wl_data_source_offer(src, "text/plain;charset=utf-8");
	wl_data_device_set_selection(dd, src, keyserial);
	for(i = 0; i < 50; i++){
		if(readfile(snarfpath(), buf, sizeof buf) >= 0 && strcmp(buf, "third") == 0)
			break;
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	writefile(snarfpath(), "plan9 again", 11);
	wl_data_device_set_selection(dd, NULL, keyserial);
	wl_display_roundtrip(d);
	readfile(snarfpath(), buf, sizeof buf);
	check("unset after a Plan 9 program snarfed: its text stays", strcmp(buf, "plan9 again") == 0, "%s", buf);
	wl_data_source_destroy(src);
	wl_data_source_destroy(src2);

	/* the window swept bigger: a configure of the size, a buffer of it */
	configured = 0;
	snprintf(msg, sizeof msg, "rect %d %d %d %d", wx0, wy0, wx0 + 300, wy0 + 200);
	check("rect to the window's ctl", winctl(msg) == 0, "%s", strerror(errno));
	while(waitfor(&configured) && !(confw == 300 && confh == 200))
		configured = 0;
	check("configure 300 200", confw == 300 && confh == 200, "%d %d", confw, confh);
	check("activated (the window has the focus)", activated, NULL);
	xdg_surface_ack_configure(xsurf, confserial);
	fd2 = memfd_create("wlclient2", MFD_CLOEXEC);
	if(fd2 < 0 || ftruncate(fd2, 300*200*4) < 0 ||
	   (px2 = mmap(NULL, 300*200*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0)) == MAP_FAILED){
		check("a second pool", 0, "%s", strerror(errno));
		return 1;
	}
	fill(px2, 300*200, Green);
	pool2 = wl_shm_create_pool(shm, fd2, 300*200*4);
	b2 = mkbuffer(fd2, pool2, 0, 300, 200, 2);
	present(b2, 300, 200);
	check("the new buffer presented", waitfor(&framedone), NULL);
	findwin();
	check("the window 300x200", wx1 - wx0 == 300 && wy1 - wy0 == 200, "%dx%d", wx1 - wx0, wy1 - wy0);
	want(250, 180, Green, "the new buffer, green");

	/*
	 * kbdfs' tables changed (x's key gives y now, as kbmap(1) would): the
	 * next window with the keyboard (the popup below) brings a new keymap
	 */
	j = nkeymaps;
	writefile("/dev/kbmap", "none 45 121\n", 12);

	/*
	 * a popup that grabs (a menu): 80x60 below and right of the point 10 10,
	 * 5 5 further; magenta, no border, the keyboard's; a click on its
	 * parent beside it: popup_done, gone
	 */
	if((ppx = mkpool(80*60*4, &ppool)) == NULL)
		return 1;
	fill(ppx, 80*60, Magenta);
	pb = wl_shm_pool_create_buffer(ppool, 0, 80, 60, 320, WL_SHM_FORMAT_XRGB8888);
	pos = xdg_wm_base_create_positioner(wm);
	xdg_positioner_set_size(pos, 80, 60);
	xdg_positioner_set_anchor_rect(pos, 10, 10, 1, 1);
	xdg_positioner_set_anchor(pos, XDG_POSITIONER_ANCHOR_TOP_LEFT);
	xdg_positioner_set_gravity(pos, XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT);
	xdg_positioner_set_offset(pos, 5, 5);
	ps = wl_compositor_create_surface(comp);
	deactivated = 0;
	pop = mkpopup(ps, pos, &pxs, 1);
	xdg_positioner_destroy(pos);
	check("the popup's configure", waitfor(&popconf), NULL);
	check("at 15 15, 80x60", popx == 15 && popy == 15 && popw == 80 && poph == 60, "%d %d %dx%d", popx, popy, popw, poph);
	xdg_surface_ack_configure(pxs, popserial);
	wl_surface_attach(ps, pb, 0, 0);
	wl_surface_damage_buffer(ps, 0, 0, 80, 60);
	wl_surface_commit(ps);
	wl_display_roundtrip(d);
	want(55, 45, Magenta, "the popup over its parent");
	want(15, 15, Magenta, "the popup's corner: no border");
	want(14, 14, Green, "the parent beside it");
	for(i = 0; i < 50 && kfocus != ps; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("the popup has the keyboard (it grabs)", kfocus == ps, NULL);
	check("kbdfs' tables changed: a new keymap with the keyboard's enter", nkeymaps > j, "%d keymaps", nkeymaps);
	check("x's key is y in it", k45is("y"), NULL);
	writefile("/dev/kbmap", "none 45 120\n", 12);
	wl_display_roundtrip(d);
	check("the toplevel stays activated", deactivated == 0, "%d configures without", deactivated);
	mouse(wx0 + 250, wy0 + 180, 1);
	usleep(50*1000);
	mouse(wx0 + 250, wy0 + 180, 0);
	check("a click on the parent: popup_done", waitfor(&popdone), NULL);
	want(55, 45, Green, "the popup gone: its parent there");
	xdg_popup_destroy(pop);
	xdg_surface_destroy(pxs);
	wl_surface_destroy(ps);

	/*
	 * a tooltip (no grab): below the point 10 y, 20 above the screen's
	 * bottom (its top and height: a wsys in a window does not start at
	 * 0), would leave it: flipped above (y - 60); the keyboard stays
	 */
	for(i = 0; i < 50 && kfocus != surf; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	y = screentop() + outh - wy0 - 20;
	ps = wl_compositor_create_surface(comp);
	pos = xdg_wm_base_create_positioner(wm);
	xdg_positioner_set_size(pos, 80, 60);
	xdg_positioner_set_anchor_rect(pos, 10, y, 1, 1);
	xdg_positioner_set_anchor(pos, XDG_POSITIONER_ANCHOR_BOTTOM_LEFT);
	xdg_positioner_set_gravity(pos, XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT);
	xdg_positioner_set_constraint_adjustment(pos, XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y);
	pop = mkpopup(ps, pos, &pxs, 0);
	xdg_positioner_destroy(pos);
	check("the tooltip's configure", waitfor(&popconf), NULL);
	check("flipped above the point", popx == 10 && popy == y - 60, "%d %d, want 10 %d", popx, popy, y - 60);
	xdg_surface_ack_configure(pxs, popserial);
	wl_surface_attach(ps, pb, 0, 0);
	wl_surface_damage_buffer(ps, 0, 0, 80, 60);
	wl_surface_commit(ps);
	wl_display_roundtrip(d);
	want(10 + 40, y - 30, Magenta, "the tooltip");
	for(j = 0; j < 10; j++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("the keyboard stays with the toplevel", kfocus == surf, NULL);
	check("not dismissed", popdone == 0, NULL);
	xdg_popup_destroy(pop);
	xdg_surface_destroy(pxs);
	wl_surface_destroy(ps);
	wl_buffer_destroy(pb);
	wl_shm_pool_destroy(ppool);
	wl_display_roundtrip(d);
	for(i = 0; i < 50 && near(peek(wx0 + 50, wy0 + y - 30), Magenta); i++)
		usleep(20*1000);
	check("the tooltip gone", !near(peek(wx0 + 50, wy0 + y - 30), Magenta), NULL);

	/*
	 * a subsurface 60x40 at 20 30: blue; synchronized, shown with the
	 * parent's commit; below the parent (hidden); half-transparent white
	 * over the parent's green; desynchronized, its commit shown at once
	 * (yellow); the pointer's enter on it; destroyed: the parent alone
	 */
	check("a subcompositor", subcomp != NULL, NULL);
	if(subcomp == NULL)
		return 1;
	if((spx = mkpool(3*60*40*4, &spool)) == NULL)
		return 1;
	fill(spx, 60*40, Blue);
	for(i = 0; i < 60*40; i++)
		spx[60*40 + i] = 0x80808080u;	/* white, half: premultiplied */
	fill(spx + 2*60*40, 60*40, 0xffff00);
	sb = wl_shm_pool_create_buffer(spool, 0, 60, 40, 240, WL_SHM_FORMAT_XRGB8888);
	sw = wl_shm_pool_create_buffer(spool, 60*40*4, 60, 40, 240, WL_SHM_FORMAT_ARGB8888);
	sg = wl_shm_pool_create_buffer(spool, 2*60*40*4, 60, 40, 240, WL_SHM_FORMAT_XRGB8888);
	ss = wl_compositor_create_surface(comp);
	sub = wl_subcompositor_get_subsurface(subcomp, ss, surf);
	wl_subsurface_set_position(sub, 20, 30);
	wl_surface_attach(ss, sb, 0, 0);
	wl_surface_damage_buffer(ss, 0, 0, 60, 40);
	wl_surface_commit(ss);
	wl_display_roundtrip(d);
	usleep(200*1000);
	check("synchronized: not shown before the parent's commit", near(peek(wx0 + 50, wy0 + 50), Green), "%06lx", peek(wx0 + 50, wy0 + 50));
	wl_surface_commit(surf);
	wl_display_roundtrip(d);
	want(50, 50, Blue, "the subsurface, with the parent's commit");
	want(20, 30, Blue, "its corner at 20 30");
	want(19, 29, Green, "the parent beside it");
	want(250, 180, Green, "the parent elsewhere");
	wl_subsurface_place_below(sub, surf);
	wl_surface_commit(surf);
	wl_display_roundtrip(d);
	want(50, 50, Green, "below the parent: covered");
	wl_subsurface_place_above(sub, surf);
	wl_surface_attach(ss, sw, 0, 0);
	wl_surface_damage_buffer(ss, 0, 0, 60, 40);
	wl_surface_commit(ss);
	wl_surface_commit(surf);
	wl_display_roundtrip(d);
	want(50, 50, 0x7fff7f, "half white over green (blended)");
	wl_subsurface_set_desync(sub);
	wl_surface_attach(ss, sg, 0, 0);
	wl_surface_damage_buffer(ss, 0, 0, 60, 40);
	wl_surface_commit(ss);
	wl_display_roundtrip(d);
	want(50, 50, 0xffff00, "desynchronized: its own commit shown");
	mouse(wx0 + 150, wy0 + 150, 0);
	mouse(wx0 + 30, wy0 + 40, 0);
	for(i = 0; i < 50 && penters != ss; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("the pointer entered the subsurface", penters == ss && penterx == 10 && pentery == 10,
		"%s at %d %d", penters == ss ? "it" : penters == surf ? "the parent" : "another", penterx, pentery);
	mouse(wx0 + 150, wy0 + 150, 0);
	for(i = 0; i < 50 && penters != surf; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("and the parent again beside it", penters == surf && penterx == 150 && pentery == 150, "%d %d", penterx, pentery);
	/* an empty input region: the subsurface takes the pointer nowhere (the parent's there) */
	rg = wl_compositor_create_region(comp);
	wl_surface_set_input_region(ss, rg);
	wl_region_destroy(rg);
	wl_surface_commit(ss);
	wl_display_roundtrip(d);
	mouse(wx0 + 30, wy0 + 40, 0);
	for(i = 0; i < 50 && !(pmx == 30 && pmy == 40); i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("an empty input region: the pointer goes through to the parent", penters == surf && pmx == 30 && pmy == 40,
		"%s, at %d %d", penters == ss ? "the subsurface" : "the parent", pmx, pmy);
	/* half of it: 0 0 to 30 40 takes it again */
	rg = wl_compositor_create_region(comp);
	wl_region_add(rg, 0, 0, 60, 40);
	wl_region_subtract(rg, 30, 0, 30, 40);
	wl_surface_set_input_region(ss, rg);
	wl_region_destroy(rg);
	wl_surface_commit(ss);
	wl_display_roundtrip(d);
	mouse(wx0 + 25, wy0 + 35, 0);
	for(i = 0; i < 50 && penters != ss; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("its input region added: the subsurface's again (5 5)", penters == ss && penterx == 5 && pentery == 5, "%d %d", penterx, pentery);
	mouse(wx0 + 60, wy0 + 50, 0);
	for(i = 0; i < 50 && penters != surf; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("and where it was taken away the parent's (60 50)", penters == surf && penterx == 60 && pentery == 50, "%d %d", penterx, pentery);
	wl_subsurface_destroy(sub);
	wl_surface_destroy(ss);
	wl_display_roundtrip(d);
	want(50, 50, Green, "the subsurface gone");
	present(b2, 300, 200);
	check("the parent's own buffer again", waitfor(&framedone), NULL);
	want(50, 50, Green, "the parent alone");

	/*
	 * drag and drop: a second toplevel beside ours; a press in ours, a
	 * drag of text with an icon (16x16 magenta), moved into it with the
	 * button held: its enter, the icon at the pointer; accepted, copy;
	 * released: dropped, the data from our source, finished
	 */
	bs = wl_compositor_create_surface(comp);
	bxs = xdg_wm_base_get_xdg_surface(wm, bs);
	xdg_surface_add_listener(bxs, &popxslistener, NULL);
	btop = xdg_surface_get_toplevel(bxs);
	xdg_toplevel_set_title(btop, "wlclient-b");
	popconf = 0;
	wl_surface_commit(bs);
	check("a second toplevel configured", waitfor(&popconf), NULL);
	xdg_surface_ack_configure(bxs, popserial);
	if((bpx = mkpool(100*100*4 + 16*16*4, &bpool)) == NULL)
		return 1;
	fill(bpx, 100*100, Red);
	fill(bpx + 100*100, 16*16, Magenta);
	bb = wl_shm_pool_create_buffer(bpool, 0, 100, 100, 400, WL_SHM_FORMAT_XRGB8888);
	ib = wl_shm_pool_create_buffer(bpool, 100*100*4, 16, 16, 64, WL_SHM_FORMAT_XRGB8888);
	wl_surface_attach(bs, bb, 0, 0);
	wl_surface_damage_buffer(bs, 0, 0, 100, 100);
	wl_surface_commit(bs);
	wl_display_roundtrip(d);
	usleep(200*1000);
	ok = labelled("wlclient-b", &bid, &bx0, &by0, &bx1, &by1) == 0;
	check("its window", ok, NULL);
	snprintf(msg, sizeof msg, "/mnt/wsys/%d/ctl", bid);
	snprintf(buf, sizeof buf, "rect %d %d %d %d", wx1 + 60, wy0, wx1 + 160, wy0 + 100);
	writefile(msg, buf, strlen(buf));
	labelled("wlclient-b", &bid, &bx0, &by0, &bx1, &by1);
	check("beside ours", bx0 == wx1 + 60, "%d %d", bx0, by0);
	is = wl_compositor_create_surface(comp);
	wl_surface_attach(is, ib, 0, 0);
	wl_surface_damage_buffer(is, 0, 0, 16, 16);
	wl_surface_commit(is);
	src = wl_data_device_manager_create_data_source(ddm);
	wl_data_source_add_listener(src, &srclistener, "dragged");
	wl_data_source_offer(src, "text/plain;charset=utf-8");
	wl_data_source_set_actions(src, WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY | WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE);
	j = pressed;
	mouse(wx0 + 250, wy0 + 150, 0);
	usleep(50*1000);
	mouse(wx0 + 250, wy0 + 150, 1);
	check("a press in ours", waitchange(&pressed, j), NULL);
	wl_data_device_start_drag(dd, src, surf, is, keyserial);
	wl_display_roundtrip(d);
	mouse(bx0 + 30, by0 + 30, 1);
	usleep(50*1000);
	mouse(bx0 + 50, by0 + 50, 1);
	for(i = 0; i < 50 && dndsurf != bs; i++){
		wl_display_roundtrip(d);
		usleep(20*1000);
	}
	check("the drag entered the second toplevel", dndsurf == bs && dndoffer != NULL, NULL);
	check("the offer: our text", offertext, NULL);
	for(i = 0; i < 50 && !near(peek(bx0 + 58, by0 + 58), Magenta); i++)
		usleep(20*1000);
	check("the icon at the pointer", near(peek(bx0 + 58, by0 + 58), Magenta), "%06lx", peek(bx0 + 58, by0 + 58));
	if(dndoffer != NULL){
		wl_data_offer_accept(dndoffer, dndserial, "text/plain;charset=utf-8");
		wl_data_offer_set_actions(dndoffer, WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY, WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY);
	}
	wl_display_roundtrip(d);
	wl_display_roundtrip(d);
	check("the action: copy", srcact == WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY, "%u", srcact);
	mouse(bx0 + 50, by0 + 50, 0);
	check("released: dropped", waitfor(&dnddrops), NULL);
	check("the source told (dnd_drop_performed)", waitfor(&nsrcdropped), NULL);
	buf[0] = 0;
	if(dndoffer != NULL && pipe(p) == 0){
		wl_data_offer_receive(dndoffer, "text/plain;charset=utf-8", p[1]);
		close(p[1]);
		wl_display_roundtrip(d);	/* our source's send comes, writes and closes */
		wl_display_roundtrip(d);
		readall(p[0], buf, sizeof buf);
		close(p[0]);
	}
	check("the data, from our source", strcmp(buf, "dragged") == 0, "%s", buf);
	if(dndoffer != NULL)
		wl_data_offer_finish(dndoffer);
	check("finished (dnd_finished)", waitfor(&nsrcfinished), NULL);
	for(i = 0; i < 50 && near(peek(bx0 + 58, by0 + 58), Magenta); i++)
		usleep(20*1000);
	check("the icon gone", !near(peek(bx0 + 58, by0 + 58), Magenta), NULL);
	wl_data_source_destroy(src);

	/* released over nothing of ours (wsys' background): cancelled */
	src = wl_data_device_manager_create_data_source(ddm);
	wl_data_source_add_listener(src, &srclistener, "dragged");
	wl_data_source_offer(src, "text/plain;charset=utf-8");
	wl_data_source_set_actions(src, WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY);
	j = pressed;
	cancelled = 0;
	mouse(wx0 + 250, wy0 + 150, 0);
	usleep(50*1000);
	mouse(wx0 + 250, wy0 + 150, 1);
	waitchange(&pressed, j);
	wl_data_device_start_drag(dd, src, surf, NULL, keyserial);
	wl_display_roundtrip(d);
	mouse(bx0 + 50, by1 + 60, 1);
	usleep(100*1000);
	mouse(bx0 + 50, by1 + 60, 0);
	check("a drag released over nothing: cancelled", waitfor(&cancelled), NULL);
	wl_data_source_destroy(src);
	xdg_toplevel_destroy(btop);
	xdg_surface_destroy(bxs);
	wl_surface_destroy(bs);
	wl_surface_destroy(is);
	wl_buffer_destroy(bb);
	wl_buffer_destroy(ib);
	wl_shm_pool_destroy(bpool);
	wl_display_roundtrip(d);

	wl_buffer_destroy(sb);
	wl_buffer_destroy(sw);
	wl_buffer_destroy(sg);
	wl_shm_pool_destroy(spool);

	/* deleted in wsys: close */
	check("delete to the window's ctl", winctl("delete") == 0, "%s", strerror(errno));
	check("the toplevel's close", waitfor(&closed), NULL);

	if(deco != NULL)
		zxdg_toplevel_decoration_v1_destroy(deco);
	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xsurf);
	wl_surface_destroy(surf);
	wl_buffer_destroy(b0);
	wl_buffer_destroy(b1);
	wl_buffer_destroy(b2);
	wl_shm_pool_destroy(pool);
	wl_shm_pool_destroy(pool2);
	wl_display_roundtrip(d);
	wl_display_disconnect(d);
	if(nfail){
		printf("wlclient: %d failed\n", nfail);
		return 1;
	}
	printf("wlclient: all passed\n");
	return 0;
}
