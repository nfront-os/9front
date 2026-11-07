/*
 * SPDX-License-Identifier: MIT
 *
 * wlgl: OpenGL ES on EGL's Wayland platform (../9front-ui PLAN §12, stage
 * 6.6), run by wlsys in wsys -x -i (wlvk.rc) on venus: an EGLDisplay of the
 * wl_display (EGL_PLATFORM_WAYLAND_KHR), a wl_egl_window on an xdg
 * toplevel's surface, a GLES 2 context (zink, its kopper presenting through
 * Vulkan's Wayland WSI); frames cleared to two colours, read back from wsys'
 * screen and with glReadPixels; the window made bigger (wl_egl_window_resize),
 * the new size in the window and the surface's EGL_WIDTH.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <xdg-shell-client-protocol.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

enum {
	W = 200,
	H = 150,
	W2 = 260,
	H2 = 190,
};

static int nfail;

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

#define FATAL(...) do { check("setup", 0, __VA_ARGS__); printf("wlgl: failed\n"); exit(1); } while(0)

/* ---- Wayland ---- */

static struct wl_display *d;
static struct wl_compositor *comp;
static struct xdg_wm_base *wm;
static int configured;
static uint32_t confserial;

static void
xdgping(void *data, struct xdg_wm_base *b, uint32_t serial)
{
	xdg_wm_base_pong(b, serial);
}
static const struct xdg_wm_base_listener wmlistener = { xdgping };

/* each configure acknowledged at once (sizes are the client's here): a
 * buffer of a new size is taken once the last one is */
static void
xsconfigure(void *data, struct xdg_surface *x, uint32_t serial)
{
	confserial = serial;
	configured = 1;
	xdg_surface_ack_configure(x, serial);
}
static const struct xdg_surface_listener xslistener = { xsconfigure };

static void topconfigure(void *data, struct xdg_toplevel *t, int32_t w, int32_t h, struct wl_array *states) {}
static void topclose(void *data, struct xdg_toplevel *t) {}
static void topbounds(void *data, struct xdg_toplevel *t, int32_t w, int32_t h) {}
static void topcaps(void *data, struct xdg_toplevel *t, struct wl_array *caps) {}
static const struct xdg_toplevel_listener toplistener = { topconfigure, topclose, topbounds, topcaps };

static void
global(void *data, struct wl_registry *r, uint32_t name, const char *iface, uint32_t version)
{
	if(strcmp(iface, wl_compositor_interface.name) == 0)
		comp = wl_registry_bind(r, name, &wl_compositor_interface, 4);
	else if(strcmp(iface, xdg_wm_base_interface.name) == 0){
		wm = wl_registry_bind(r, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(wm, &wmlistener, NULL);
	}
}
static void globalremove(void *data, struct wl_registry *r, uint32_t name) {}
static const struct wl_registry_listener reglistener = { global, globalremove };

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

/* ---- wsys, from the outside ---- */

static int wx0, wy0, wx1, wy1;

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

/* the window of wsys labelled "wlgl", and where its contents are */
static int
findwin(void)
{
	DIR *dir;
	struct dirent *e;
	char path[128], buf[256];
	int id, winid;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	winid = -1;
	while((e = readdir(dir)) != NULL){
		if((id = atoi(e->d_name)) <= 0)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/label", id);
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, "wlgl") == 0)
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

/* the screen's pixel at x, y as 0xRRGGBB; -1 if not read */
static long
peek(int x, int y)
{
	static unsigned char *img;
	char hdr[61], chan[16];
	int fd, sx0, sy0, sx1, sy1, n, total, m;
	unsigned char *p;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return -1;
	if(read(fd, hdr, 60) != 60){
		close(fd);
		return -1;
	}
	hdr[60] = 0;
	if(sscanf(hdr, "%15s %d %d %d %d", chan, &sx0, &sy0, &sx1, &sy1) != 5 ||
	   (strcmp(chan, "x8r8g8b8") != 0 && strcmp(chan, "a8r8g8b8") != 0)){
		close(fd);
		return -1;
	}
	total = (sx1 - sx0) * (sy1 - sy0) * 4;
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
	p = img + ((long)(y - sy0) * (sx1 - sx0) + (x - sx0)) * 4;
	return (long)p[2]<<16 | p[1]<<8 | p[0];
}

/* the window at x, y is rgb, within a second */
static void
want(int x, int y, long rgb, char *what)
{
	char buf[128];
	long v;
	int i;

	v = -1;
	for(i = 0; i < 50; i++){
		if((v = peek(wx0 + x, wy0 + y)) == rgb)
			break;
		usleep(20*1000);
	}
	snprintf(buf, sizeof buf, "the window at %d,%d: %s", x, y, what);
	check(buf, v == rgb, "%06lx, want %06lx", v, rgb);
}

/* a frame cleared to r, g, b; glReadPixels' view of it */
static void
frame(EGLDisplay ed, EGLSurface es, float r, float g, float b, long rgb, char *what)
{
	unsigned char px[4];
	char buf[96];

	glClearColor(r, g, b, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glReadPixels(5, 5, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);
	snprintf(buf, sizeof buf, "glReadPixels: %s", what);
	check(buf, ((long)px[0]<<16 | px[1]<<8 | px[2]) == rgb, "%02x%02x%02x", px[0], px[1], px[2]);
	snprintf(buf, sizeof buf, "eglSwapBuffers (%s)", what);
	check(buf, eglSwapBuffers(ed, es), "%#x", eglGetError());
}

int
main(void)
{
	struct wl_surface *surf;
	struct xdg_surface *xsurf;
	struct xdg_toplevel *top;
	struct wl_egl_window *ew;
	EGLDisplay ed;
	EGLConfig cfg;
	EGLContext ctx;
	EGLSurface es;
	EGLint major, minor, n, w;
	PFNEGLGETPLATFORMDISPLAYEXTPROC getdisplay;
	PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC mksurface;
	static const EGLint cattr[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE,
	};
	static const EGLint xattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

	if((d = wl_display_connect(NULL)) == NULL)
		FATAL("wl_display_connect: no wlsys");
	wl_registry_add_listener(wl_display_get_registry(d), &reglistener, NULL);
	wl_display_roundtrip(d);
	if(comp == NULL || wm == NULL)
		FATAL("globals: compositor %p, xdg_wm_base %p", comp, wm);

	getdisplay = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
	mksurface = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
	if(getdisplay == NULL || mksurface == NULL)
		FATAL("no EGL_EXT_platform_base");
	check("EGL_EXT_platform_wayland", strstr(eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS), "EGL_EXT_platform_wayland") != NULL,
		"%s", eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS));
	ed = getdisplay(EGL_PLATFORM_WAYLAND_EXT, d, NULL);
	if(ed == EGL_NO_DISPLAY || !eglInitialize(ed, &major, &minor))
		FATAL("eglInitialize on the wl_display: %#x", eglGetError());
	check("eglInitialize on the wl_display", 1, NULL);
	printf("wlgl: EGL %d.%d %s\n", major, minor, eglQueryString(ed, EGL_VENDOR));
	if(!eglChooseConfig(ed, cattr, &cfg, 1, &n) || n < 1)
		FATAL("eglChooseConfig: %d configs, %#x", n, eglGetError());
	eglBindAPI(EGL_OPENGL_ES_API);
	if((ctx = eglCreateContext(ed, cfg, EGL_NO_CONTEXT, xattr)) == EGL_NO_CONTEXT)
		FATAL("eglCreateContext: %#x", eglGetError());

	surf = wl_compositor_create_surface(comp);
	xsurf = xdg_wm_base_get_xdg_surface(wm, surf);
	xdg_surface_add_listener(xsurf, &xslistener, NULL);
	top = xdg_surface_get_toplevel(xsurf);
	xdg_toplevel_add_listener(top, &toplistener, NULL);
	xdg_toplevel_set_title(top, "wlgl");
	wl_surface_commit(surf);
	check("configure", waitfor(&configured), NULL);

	if((ew = wl_egl_window_create(surf, W, H)) == NULL)
		FATAL("wl_egl_window_create");
	if((es = mksurface(ed, cfg, ew, NULL)) == EGL_NO_SURFACE)
		FATAL("eglCreatePlatformWindowSurface: %#x", eglGetError());
	if(!eglMakeCurrent(ed, es, es, ctx))
		FATAL("eglMakeCurrent: %#x", eglGetError());
	printf("wlgl: GL %s, %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

	frame(ed, es, 1, 0, 0, 0xff0000, "red");
	usleep(200*1000);
	check("a window of wsys labelled wlgl", findwin() == 0, NULL);
	check("the window W×H", wx1 - wx0 == W && wy1 - wy0 == H, "%dx%d", wx1 - wx0, wy1 - wy0);
	want(W/2, H/2, 0xff0000, "the first frame (red)");
	frame(ed, es, 0, 0, 1, 0x0000ff, "blue");
	want(W-1, H-1, 0x0000ff, "the second (blue), its last pixel");

	/* bigger: the next frame has the new size (the configures since acknowledged) */
	wl_display_roundtrip(d);
	wl_egl_window_resize(ew, W2, H2, 0, 0);
	frame(ed, es, 0, 1, 0, 0x00ff00, "green, resized");
	frame(ed, es, 0, 1, 0, 0x00ff00, "green again");
	eglQuerySurface(ed, es, EGL_WIDTH, &w);
	check("EGL_WIDTH after the resize", w == W2, "%d", w);
	usleep(300*1000);
	findwin();
	check("the window resized", wx1 - wx0 == W2 && wy1 - wy0 == H2, "%dx%d", wx1 - wx0, wy1 - wy0);
	want(W2-1, H2-1, 0x00ff00, "the resized frame (green), its last pixel");

	eglMakeCurrent(ed, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(ed, es);
	wl_egl_window_destroy(ew);
	eglDestroyContext(ed, ctx);
	eglTerminate(ed);
	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xsurf);
	wl_surface_destroy(surf);
	wl_display_roundtrip(d);
	wl_display_disconnect(d);
	printf(nfail ? "wlgl: %d failed\n" : "wlgl: all passed\n", nfail);
	return nfail != 0;
}
