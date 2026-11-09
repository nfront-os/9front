/*
 * SPDX-License-Identifier: MIT
 *
 * wlsys [-d] [-s name] [cmd [arg ...]]: a Wayland server in a window system
 * of wsys(4) (/mnt/wsys).  It listens on $XDG_RUNTIME_DIR/name (APE's
 * PF_UNIX socket: an entry in /srv), default wayland-$pid, and sets
 * $WAYLAND_DISPLAY (in /env: the processes started after it see it); with
 * cmd, it runs it and exits when it exits, with its status.  -d prints what
 * it does.  Globals: wl_compositor, wl_shm, wl_seat, wl_output, xdg_wm_base,
 * wl_subcompositor, wl_data_device_manager (the snarf buffer), zxdg_decoration_manager_v1 (wsys'
 * borders), zwp_linux_dmabuf_v1 (with gpu(4)).
 */
#include "dat.h"
#include <time.h>
#include <sys/wait.h>

struct wl_display *display;
struct wl_event_loop *loop;
struct wl_list surfaces;
int debug;

static struct wl_list outputs;	/* wl_output resources */
/*
 * frame callbacks due: done at the next tick, 16 ms after the first came,
 * so that a client draws at a display's pace (wsys lets go of a buffer as
 * soon as it has copied it), and commits without a present are answered
 */
static struct wl_list tick;
static struct wl_event_source *ticker;
static int ticking;

void
dprint(char *fmt, ...)
{
	va_list a;

	if(!debug)
		return;
	fprintf(stderr, "wlsys: ");
	va_start(a, fmt);
	vfprintf(stderr, fmt, a);
	va_end(a);
}

uint32_t
msec(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec*1000 + t.tv_nsec/1000000;
}

/* ---- frame callbacks ---- */

static void
framedestroy(struct wl_resource *r)
{
	Frame *f;

	f = wl_resource_get_user_data(r);
	wl_list_remove(&f->link);
	free(f);
}

/* the callbacks of list are done (their picture shown, or skipped) */
void
frames(struct wl_list *list, uint32_t ms)
{
	Frame *f, *t;

	wl_list_for_each_safe(f, t, list, link){
		wl_callback_send_done(f->res, ms);
		wl_resource_destroy(f->res);
	}
}

static int
ontick(void *data)
{
	(void)data;
	ticking = 0;
	frames(&tick, msec());
	return 0;
}

/* the callbacks of list done at the next tick */
void
later(struct wl_list *list)
{
	if(wl_list_empty(list))
		return;
	wl_list_insert_list(tick.prev, list);
	wl_list_init(list);
	if(!ticking){
		ticking = 1;
		wl_event_source_timer_update(ticker, 16);
	}
}

/* ---- wl_region: wsys windows are opaque rectangles; regions are not kept ---- */

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static void
regionrect(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h)
{
	(void)c; (void)r; (void)x; (void)y; (void)w; (void)h;
}

static const struct wl_region_interface regionimpl = {
	resdestroy,
	regionrect,	/* add */
	regionrect,	/* subtract */
};

/* ---- wl_surface ---- */

static Surface*
ssurface(struct wl_resource *r)
{
	return wl_resource_get_user_data(r);
}

static void
rectadd(Rect *d, int x0, int y0, int x1, int y1)
{
	if(x0 >= x1 || y0 >= y1)
		return;
	if(d->x0 >= d->x1 || d->y0 >= d->y1){
		*d = (Rect){x0, y0, x1, y1};
		return;
	}
	if(x0 < d->x0)
		d->x0 = x0;
	if(y0 < d->y0)
		d->y0 = y0;
	if(x1 > d->x1)
		d->x1 = x1;
	if(y1 > d->y1)
		d->y1 = y1;
}

static int
clampadd(int32_t a, int32_t b)
{
	int64_t s;

	s = (int64_t)a + b;
	if(s > 1<<20)
		return 1<<20;
	if(s < -(1<<20))
		return -(1<<20);
	return s;
}

static void
surfattach(struct wl_client *c, struct wl_resource *r, struct wl_resource *buf, int32_t x, int32_t y)
{
	Surface *s;

	(void)c;
	s = ssurface(r);
	s->pbuf = getbuffer(buf);
	s->pattached = 1;
	if(wl_resource_get_version(r) < WL_SURFACE_OFFSET_SINCE_VERSION){
		s->pdx += x;
		s->pdy += y;
	}
}

/* damage: the surface's coordinates are its buffer's (no scale, no transform) */
static void
surfdamage(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y, int32_t w, int32_t h)
{
	Surface *s;

	(void)c;
	s = ssurface(r);
	rectadd(&s->pdamage, x, y, clampadd(x, w), clampadd(y, h));
}

static void
surfframe(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	Surface *s;
	Frame *f;

	s = ssurface(r);
	if((f = calloc(1, sizeof *f)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	if((f->res = wl_resource_create(c, &wl_callback_interface, 1, id)) == NULL){
		free(f);
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(f->res, NULL, f, framedestroy);
	wl_list_insert(s->pframes.prev, &f->link);
}

static void
surfregion(struct wl_client *c, struct wl_resource *r, struct wl_resource *reg)
{
	(void)c; (void)r; (void)reg;
}

static void
surfcommit(struct wl_client *c, struct wl_resource *r)
{
	Surface *s;
	int changed, attached;

	(void)c;
	s = ssurface(r);
	attached = s->pattached;
	if(s->role == Rsub && subcommit(s)){
		/* synchronized: kept for the parent's commit */
		later(&s->pframes);
		s->pdamage = (Rect){0, 0, 0, 0};
		s->pdx = s->pdy = 0;
		return;
	}
	changed = s->pattached || (s->pdamage.x0 < s->pdamage.x1 && s->pdamage.y0 < s->pdamage.y1);
	if(s->pattached){
		s->buf = s->pbuf;
		if(s->buf != NULL)
			s->buf->freed = 0;
		s->pbuf = NULL;
		s->pattached = 0;
		if(s->buf == NULL)
			unshow(s);	/* unmapped */
	}
	if(s->role == Rcursor){
		/* the pointer's picture: read now, the buffer free again */
		cursorcommit(s, attached, s->pdx, s->pdy);
	}
	s->pdx = s->pdy = 0;
	subcommitted(s, attached);
	if(hassubs(s))
		changed = 1;	/* a subsurface's commit may have come with it */
	if((s->toplevel != NULL || s->popup != NULL) && !s->committed){
		/* the first commit: the toplevel's state (popup's place); a configure answers */
		s->committed = 1;
		configure(s);
	}
	if(changed && (s->toplevel != NULL || (s->popup != NULL && !s->dismissed)) && s->buf != NULL)
		show(s);
	later(&s->pframes);
	s->pdamage = (Rect){0, 0, 0, 0};
}

static void
surfint(struct wl_client *c, struct wl_resource *r, int32_t v)
{
	(void)c; (void)r; (void)v;
}

static void
surfoffset(struct wl_client *c, struct wl_resource *r, int32_t x, int32_t y)
{
	Surface *s;

	(void)c;
	s = ssurface(r);
	s->pdx += x;
	s->pdy += y;
}

static const struct wl_surface_interface surfaceimpl = {
	resdestroy,
	surfattach,
	surfdamage,
	surfframe,
	surfregion,	/* set_opaque_region */
	surfregion,	/* set_input_region */
	surfcommit,
	surfint,	/* set_buffer_transform */
	surfint,	/* set_buffer_scale */
	surfdamage,	/* damage_buffer */
	surfoffset,
	NULL,		/* get_release: version 7, not offered */
};

/* the surface's buffer is gone: no surface shows it */
void
buffergone(Buffer *b)
{
	Surface *s;

	wl_list_for_each(s, &surfaces, link){
		if(s->pbuf == b)
			s->pbuf = NULL;
		if(s->cbuf == b)
			s->cbuf = NULL;
		if(s->buf == b)
			s->buf = NULL;
	}
}

static void
surfdestroyres(struct wl_resource *r)
{
	Surface *s;
	Frame *f, *t;

	s = ssurface(r);
	popupforget(s);
	subgone(s);
	unshow(s);
	cursorgone(s);
	pointergone(s);
	wl_list_for_each_safe(f, t, &s->pframes, link)
		wl_resource_destroy(f->res);
	if(s->xdgsurf != NULL)
		wl_resource_set_user_data(s->xdgsurf, NULL);
	if(s->toplevel != NULL)
		wl_resource_set_user_data(s->toplevel, NULL);
	if(s->deco != NULL)
		wl_resource_set_user_data(s->deco, NULL);
	if(s->popup != NULL)
		wl_resource_set_user_data(s->popup, NULL);
	wl_list_remove(&s->link);
	free(s->title);
	free(s);
}

/* the window of the surface is on the output */
void
sendenter(Surface *s)
{
	struct wl_resource *o;

	wl_resource_for_each(o, &outputs)
		if(wl_resource_get_client(o) == wl_resource_get_client(s->res))
			wl_surface_send_enter(s->res, o);
}

/* ---- wl_compositor ---- */

static void
compcreatesurface(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	Surface *s;

	if((s = calloc(1, sizeof *s)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	if((s->res = wl_resource_create(c, &wl_surface_interface, wl_resource_get_version(r), id)) == NULL){
		free(s);
		wl_client_post_no_memory(c);
		return;
	}
	wl_list_init(&s->pframes);
	subnew(s);
	wl_list_insert(&surfaces, &s->link);
	wl_resource_set_implementation(s->res, &surfaceimpl, s, surfdestroyres);
}

static void
compcreateregion(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	struct wl_resource *g;

	if((g = wl_resource_create(c, &wl_region_interface, 1, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	(void)r;
	wl_resource_set_implementation(g, &regionimpl, NULL, NULL);
}

static const struct wl_compositor_interface compimpl = {
	compcreatesurface,
	compcreateregion,
	resdestroy,	/* release: version 7 */
};

static void
compbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_compositor_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &compimpl, NULL, NULL);
}

/* ---- wl_output: wsys' screen ---- */

static void
outlink(struct wl_resource *r)
{
	wl_list_remove(wl_resource_get_link(r));
}

static const struct wl_output_interface outputimpl = {
	resdestroy,	/* release */
};

static void
outbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_output_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &outputimpl, NULL, outlink);
	wl_list_insert(&outputs, wl_resource_get_link(r));
	wl_output_send_geometry(r, 0, 0, 0, 0, WL_OUTPUT_SUBPIXEL_UNKNOWN, "9front", "wsys", WL_OUTPUT_TRANSFORM_NORMAL);
	wl_output_send_mode(r, WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, screenw, screenh, 60000);
	if(version >= WL_OUTPUT_SCALE_SINCE_VERSION)
		wl_output_send_scale(r, 1);
	if(version >= WL_OUTPUT_NAME_SINCE_VERSION)
		wl_output_send_name(r, "wsys");
	if(version >= WL_OUTPUT_DESCRIPTION_SINCE_VERSION)
		wl_output_send_description(r, "the screen of wsys");
	if(version >= WL_OUTPUT_DONE_SINCE_VERSION)
		wl_output_send_done(r);
}

/* ---- main ---- */

static void
usage(void)
{
	fprintf(stderr, "usage: wlsys [-d] [-s name] [cmd [arg ...]]\n");
	exit(1);
}

/* $name=val for this process and, in /env, for those started after it */
static void
putenv9(char *name, char *val)
{
	char path[128];
	int fd;

	setenv(name, val, 1);
	snprintf(path, sizeof path, "/env/%s", name);
	if((fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666)) >= 0){
		write(fd, val, strlen(val));
		close(fd);
	}
}

static int
run(char **argv)
{
	char path[256];
	int pid;

	switch(pid = fork()){
	case -1:
		fprintf(stderr, "wlsys: fork: %s\n", strerror(errno));
		exit(1);
	case 0:
		execv(argv[0], argv);
		if(strchr(argv[0], '/') == NULL){
			snprintf(path, sizeof path, "/bin/%s", argv[0]);
			execv(path, argv);
		}
		fprintf(stderr, "wlsys: %s: %s\n", argv[0], strerror(errno));
		_exit(127);
	}
	return pid;
}

int
main(int argc, char **argv)
{
	char name[64], *s;
	int c, pid, st;

	snprintf(name, sizeof name, "wayland-%d", (int)getpid());
	while((c = getopt(argc, argv, "+ds:")) != -1)
		switch(c){
		case 'd':
			debug = 1;
			break;
		case 's':
			snprintf(name, sizeof name, "%s", optarg);
			break;
		default:
			usage();
		}
	argv += optind;
	/*
	 * in a window of wsys /mnt/wsys is that window ($wsys names the
	 * connection): a name space of wlsys' own with wsys' tree there
	 */
	if(access("/mnt/wsys/clone", 0) < 0 && (s = getenv("wsys")) != NULL && *s != 0){
		int fd;

		if(_RFORK(RFNAMEG) < 0 || (fd = open(s, O_RDWR)) < 0 ||
		   _MOUNT(fd, -1, "/mnt/wsys", MREPL, "") < 0)
			fprintf(stderr, "wlsys: mount %s on /mnt/wsys: %s\n", s, strerror(errno));
	}
	if(wsysinit() < 0){
		fprintf(stderr, "wlsys: /mnt/wsys/screen: %s: not in wsys\n", strerror(errno));
		return 1;
	}
	/*
	 * a note group of its own (as wsys, and servers): the window it was
	 * started in may go, or its wsys exit, hanging up the window's
	 * processes; wlsys and its clients stay and find the next wsys
	 * (win.c: wsyslost)
	 */
	if(_RFORK(RFNOTEG) < 0)
		fprintf(stderr, "wlsys: rfork: %s\n", strerror(errno));
	if((s = getenv("XDG_RUNTIME_DIR")) == NULL || *s == 0)
		putenv9("XDG_RUNTIME_DIR", "/tmp");
	wl_list_init(&surfaces);
	wl_list_init(&outputs);
	wl_list_init(&tick);
	if((display = wl_display_create()) == NULL){
		fprintf(stderr, "wlsys: wl_display_create failed\n");
		return 1;
	}
	if(wl_display_add_socket(display, name) < 0){
		fprintf(stderr, "wlsys: socket %s: %s\n", name, strerror(errno));
		return 1;
	}
	putenv9("WAYLAND_DISPLAY", name);
	loop = wl_display_get_event_loop(display);
	ticker = wl_event_loop_add_timer(loop, ontick, NULL);
	wl_global_create(display, &wl_compositor_interface, 5, NULL, compbind);
	subinit();
	wl_global_create(display, &wl_output_interface, 4, NULL, outbind);
	shminit();
	dmabufinit();
	seatinit();
	datainit();
	xdginit();
	decoinit();
	dprint("%s, screen %dx%d\n", name, screenw, screenh);

	pid = -1;
	if(argv[0] != NULL)
		pid = run(argv);
	for(;;){
		wl_display_flush_clients(display);
		if(wl_event_loop_dispatch(loop, pid > 0 ? 100 : -1) < 0 && errno != EINTR){
			fprintf(stderr, "wlsys: event loop: %s\n", strerror(errno));
			break;
		}
		if(pid > 0 && waitpid(pid, &st, WNOHANG) == pid){
			wl_display_destroy_clients(display);
			wl_display_destroy(display);
			if(WIFEXITED(st))
				return WEXITSTATUS(st);
			return 1;
		}
	}
	return 1;
}
