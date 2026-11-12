/*
 * SPDX-License-Identifier: MIT
 *
 * wlscale: a Wayland client that knows fractional scales (../9front-ui PLAN
 * stage 8, HiDPI), run by wlscale.rc under wlsys in wsys -S 2: a toplevel
 * titled wlscale with a wp_fractional_scale_v1 and a wp_viewport; it draws
 * a buffer of its logical size times the preferred scale (/120), the
 * viewport's destination its logical size, as such clients do; again when
 * the preferred scale or a configure changes.  It writes its state to
 * /tmp/wlscale.state ("preferred P output S logical w h buffer bw bh") and
 * the pointer's last motion to /tmp/wlscale.pointer ("x y", surface units)
 * and ends when /tmp/wlscale.stop is there (60 s at most).
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <math.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>
#include <viewporter-client-protocol.h>
#include <fractional-scale-v1-client-protocol.h>

static struct wl_compositor *comp;
static struct wl_shm *shm;
static struct xdg_wm_base *wm;
static struct wl_seat *seat;
static struct wp_viewporter *vp;
static struct wp_fractional_scale_manager_v1 *fsm;
static int outscale = 1;

static int pref = 120;		/* the preferred scale ×120 */
static int confw, confh;	/* the last configure's size (0: ours) */
static uint32_t serial;
static int configured, dirty, closed;

static void
writefile(char *path, char *fmt, double a, double b, double c, double d, double e, double f, double g)
{
	FILE *o;
	char tmp[64];

	snprintf(tmp, sizeof tmp, "%s.new", path);
	if((o = fopen(tmp, "w")) == NULL)
		return;
	fprintf(o, fmt, a, b, c, d, e, f, g);
	fclose(o);
	rename(tmp, path);
}

static void
ping(void *data, struct xdg_wm_base *b, uint32_t s)
{
	xdg_wm_base_pong(b, s);
}
static const struct xdg_wm_base_listener wmlistener = { ping };

static void
xconf(void *data, struct xdg_surface *x, uint32_t s)
{
	serial = s;
	configured = 1;
	dirty = 1;
}
static const struct xdg_surface_listener xlistener = { xconf };

static void
tconf(void *data, struct xdg_toplevel *t, int32_t w, int32_t h, struct wl_array *st)
{
	confw = w;
	confh = h;
}
static void
tclose(void *data, struct xdg_toplevel *t)
{
	closed = 1;
}
static void tbounds(void *data, struct xdg_toplevel *t, int32_t w, int32_t h) {}
static void tcaps(void *data, struct xdg_toplevel *t, struct wl_array *c) {}
static const struct xdg_toplevel_listener tlistener = { tconf, tclose, tbounds, tcaps };

static void
preferred(void *data, struct wp_fractional_scale_v1 *f, uint32_t s)
{
	fprintf(stderr, "wlscale: preferred scale %u/120\n", s);
	pref = s;
	dirty = 1;
}
static const struct wp_fractional_scale_v1_listener flistener = { preferred };

static void ogeom(void *d, struct wl_output *o, int32_t x, int32_t y, int32_t pw, int32_t ph, int32_t sp, const char *mk, const char *md, int32_t t) {}
static void omode(void *d, struct wl_output *o, uint32_t f, int32_t w, int32_t h, int32_t r) {}
static void odone(void *d, struct wl_output *o) {}
static void
oscale(void *d, struct wl_output *o, int32_t s)
{
	outscale = s;
	dirty = 1;
}
static void oname(void *d, struct wl_output *o, const char *n) {}
static void odesc(void *d, struct wl_output *o, const char *n) {}
static const struct wl_output_listener olistener = { ogeom, omode, odone, oscale, oname, odesc };

static void penter(void *d, struct wl_pointer *p, uint32_t s, struct wl_surface *sf, wl_fixed_t x, wl_fixed_t y)
{
	writefile("/tmp/wlscale.pointer", "%g %g\n", wl_fixed_to_double(x), wl_fixed_to_double(y), 0, 0, 0, 0, 0);
}
static void pleave(void *d, struct wl_pointer *p, uint32_t s, struct wl_surface *sf) {}
static void pmotion(void *d, struct wl_pointer *p, uint32_t t, wl_fixed_t x, wl_fixed_t y)
{
	writefile("/tmp/wlscale.pointer", "%g %g\n", wl_fixed_to_double(x), wl_fixed_to_double(y), 0, 0, 0, 0, 0);
}
static void pbutton(void *d, struct wl_pointer *p, uint32_t s, uint32_t t, uint32_t b, uint32_t st) {}
static void paxis(void *d, struct wl_pointer *p, uint32_t t, uint32_t a, wl_fixed_t v) {}
static void pframe(void *d, struct wl_pointer *p) {}
static void psrc(void *d, struct wl_pointer *p, uint32_t s) {}
static void pstop(void *d, struct wl_pointer *p, uint32_t t, uint32_t a) {}
static void pdisc(void *d, struct wl_pointer *p, uint32_t a, int32_t v) {}
static const struct wl_pointer_listener plistener = { penter, pleave, pmotion, pbutton, paxis, pframe, psrc, pstop, pdisc };

static void
caps(void *d, struct wl_seat *s, uint32_t c)
{
	struct wl_pointer *p;

	if(c & WL_SEAT_CAPABILITY_POINTER){
		p = wl_seat_get_pointer(s);
		wl_pointer_add_listener(p, &plistener, NULL);
	}
}
static void sname(void *d, struct wl_seat *s, const char *n) {}
static const struct wl_seat_listener slistener = { caps, sname };

static void
global(void *data, struct wl_registry *r, uint32_t name, const char *iface, uint32_t v)
{
	struct wl_output *o;

	if(strcmp(iface, wl_compositor_interface.name) == 0)
		comp = wl_registry_bind(r, name, &wl_compositor_interface, 4);
	else if(strcmp(iface, wl_shm_interface.name) == 0)
		shm = wl_registry_bind(r, name, &wl_shm_interface, 1);
	else if(strcmp(iface, xdg_wm_base_interface.name) == 0){
		wm = wl_registry_bind(r, name, &xdg_wm_base_interface, 2);
		xdg_wm_base_add_listener(wm, &wmlistener, NULL);
	}else if(strcmp(iface, wl_seat_interface.name) == 0){
		seat = wl_registry_bind(r, name, &wl_seat_interface, 5);
		wl_seat_add_listener(seat, &slistener, NULL);
	}else if(strcmp(iface, wl_output_interface.name) == 0){
		o = wl_registry_bind(r, name, &wl_output_interface, v < 4 ? v : 4);
		wl_output_add_listener(o, &olistener, NULL);
	}else if(strcmp(iface, wp_viewporter_interface.name) == 0)
		vp = wl_registry_bind(r, name, &wp_viewporter_interface, 1);
	else if(strcmp(iface, wp_fractional_scale_manager_v1_interface.name) == 0)
		fsm = wl_registry_bind(r, name, &wp_fractional_scale_manager_v1_interface, 1);
}
static void gremove(void *d, struct wl_registry *r, uint32_t n) {}
static const struct wl_registry_listener rlistener = { global, gremove };

/* a buffer of w by h, red with a white frame of 4 pixels */
static struct wl_buffer*
buffer(int w, int h)
{
	struct wl_shm_pool *pool;
	struct wl_buffer *b;
	uint32_t *px;
	int fd, x, y;

	if((fd = memfd_create("wlscale", MFD_CLOEXEC)) < 0 || ftruncate(fd, w*h*4) < 0){
		fprintf(stderr, "wlscale: memfd: %s\n", strerror(errno));
		exit(1);
	}
	if((px = mmap(NULL, w*h*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		fprintf(stderr, "wlscale: mmap: %s\n", strerror(errno));
		exit(1);
	}
	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			px[y*w + x] = x < 4 || y < 4 || x >= w-4 || y >= h-4 ? 0xffffffff : 0xffff0000;
	munmap(px, w*h*4);
	pool = wl_shm_create_pool(shm, fd, w*h*4);
	b = wl_shm_pool_create_buffer(pool, 0, w, h, w*4, WL_SHM_FORMAT_XRGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);
	return b;
}

int
main(void)
{
	struct wl_display *d;
	struct wl_surface *s;
	struct xdg_surface *xs;
	struct xdg_toplevel *t;
	struct wp_viewport *v;
	struct wp_fractional_scale_v1 *f;
	struct wl_buffer *b, *old;
	struct pollfd p;
	time_t t0;
	int w, h, bw, bh;

	unlink("/tmp/wlscale.stop");
	if((d = wl_display_connect(NULL)) == NULL){
		fprintf(stderr, "wlscale: no display\n");
		return 1;
	}
	wl_registry_add_listener(wl_display_get_registry(d), &rlistener, NULL);
	wl_display_roundtrip(d);
	wl_display_roundtrip(d);
	if(comp == NULL || shm == NULL || wm == NULL || vp == NULL || fsm == NULL){
		fprintf(stderr, "wlscale: missing globals: viewporter %p fractional %p\n", (void*)vp, (void*)fsm);
		return 1;
	}
	s = wl_compositor_create_surface(comp);
	f = wp_fractional_scale_manager_v1_get_fractional_scale(fsm, s);
	wp_fractional_scale_v1_add_listener(f, &flistener, NULL);
	v = wp_viewporter_get_viewport(vp, s);
	xs = xdg_wm_base_get_xdg_surface(wm, s);
	xdg_surface_add_listener(xs, &xlistener, NULL);
	t = xdg_surface_get_toplevel(xs);
	xdg_toplevel_add_listener(t, &tlistener, NULL);
	xdg_toplevel_set_title(t, "wlscale");
	wl_surface_commit(s);
	old = NULL;
	t0 = time(NULL);
	p.fd = wl_display_get_fd(d);
	p.events = POLLIN;
	while(!closed && time(NULL) - t0 < 60 && access("/tmp/wlscale.stop", 0) < 0){
		if(configured && dirty){
			dirty = 0;
			w = confw > 0 ? confw : 300;
			h = confh > 0 ? confh : 200;
			bw = (int)floor(w * pref / 120.0 + 0.5);
			bh = (int)floor(h * pref / 120.0 + 0.5);
			xdg_surface_ack_configure(xs, serial);
			b = buffer(bw, bh);
			wl_surface_attach(s, b, 0, 0);
			wp_viewport_set_destination(v, w, h);
			wl_surface_damage_buffer(s, 0, 0, bw, bh);
			wl_surface_commit(s);
			if(old != NULL)
				wl_buffer_destroy(old);
			old = b;
			fprintf(stderr, "wlscale: preferred %d output %d logical %d %d buffer %d %d\n", pref, outscale, w, h, bw, bh);
			writefile("/tmp/wlscale.state", "preferred %g output %g logical %g %g buffer %g %g\n", pref, outscale, w, h, bw, bh, 0);
		}
		wl_display_flush(d);
		if(poll(&p, 1, 100) > 0){
			if(wl_display_dispatch(d) < 0)
				break;
		}else
			wl_display_dispatch_pending(d);
	}
	fprintf(stderr, "wlscale: done\n");
	wl_display_disconnect(d);
	return 0;
}
