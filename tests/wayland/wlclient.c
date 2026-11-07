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
 * that size, green); deleted in wsys: the toplevel's close.
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

enum {
	W = 200, H = 150,
	Red = 0xff0000, Green = 0x00ff00, Blue = 0x0000ff,
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
static int keymapfd = -1, keymapsize;
static int keydown, keyup, keymods, shifted;

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
	if(s == surf){
		entered++;
		enterx = wl_fixed_to_int(x);
		entery = wl_fixed_to_int(y);
	}
}
static void pleave(void *data, struct wl_pointer *p, uint32_t serial, struct wl_surface *s) { if(s == surf) left++; }
static void pmotion(void *data, struct wl_pointer *p, uint32_t ms, wl_fixed_t x, wl_fixed_t y) {}
static void
pbutton(void *data, struct wl_pointer *p, uint32_t serial, uint32_t ms, uint32_t b, uint32_t st)
{
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
}
static void kenter(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s, struct wl_array *keys) {}
static void kleave(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s) {}
static void
kkey(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t ms, uint32_t key, uint32_t st)
{
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
	}else if(strcmp(iface, wl_seat_interface.name) == 0)
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

int
main(void)
{
	struct wl_registry *reg;
	struct wl_shm_pool *pool, *pool2;
	struct wl_buffer *b0, *b1, *b2;
	struct wl_pointer *ptr;
	struct wl_keyboard *kbd;
	uint32_t *px, *px2;
	char *km, msg[64];
	int fd, fd2, size, ok;

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
	wl_surface_commit(surf);
	check("the first configure", waitfor(&configured), NULL);
	check("its size: ours to choose (0 0)", confw == 0 && confh == 0, "%d %d", confw, confh);
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

	/* deleted in wsys: close */
	check("delete to the window's ctl", winctl("delete") == 0, "%s", strerror(errno));
	check("the toplevel's close", waitfor(&closed), NULL);

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
