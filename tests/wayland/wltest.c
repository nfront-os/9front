/*
 * SPDX-License-Identifier: MIT
 *
 * wltest: libwayland on 9front (../9front-ui PLAN §12, stage 6.4), a server
 * and a client in two processes over APE's PF_UNIX socket
 * ($XDG_RUNTIME_DIR/wayland-wltest, its /srv entry): the client binds wl_shm,
 * makes a pool of a memfd (a segment: memfd_create, ftruncate, mmap), a 64x64
 * buffer in it, writes a pattern; the memfd goes over the socket (sendmsg,
 * SCM_RIGHTS); the server finds the buffer among the client's resources and
 * reads the pattern through wl_shm_buffer_get_data: the same memory, no copy.
 * A timer of the server's event loop (epoll, timerfd) fires.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <wayland-server.h>
#include <wayland-client.h>

enum { W = 64, H = 64, Stride = W*4, Size = Stride*H };

static int nfail;

static void
check(const char *what, int ok, const char *why)
{
	if(ok)
		printf("ok %s\n", what);
	else{
		printf("FAIL %s%s%s\n", what, why ? ": " : "", why ? why : "");
		nfail++;
	}
	fflush(stdout);
}

static uint32_t
pattern(int x, int y)
{
	return 0xff000000u | (uint32_t)x << 16 | (uint32_t)y << 8 | (uint32_t)(x ^ y);
}

/* ---- the client ---- */

static struct wl_shm *shm;

static void
global(void *data, struct wl_registry *reg, uint32_t name, const char *iface, uint32_t version)
{
	(void)data;
	if(strcmp(iface, wl_shm_interface.name) == 0)
		shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
}

static void
global_remove(void *data, struct wl_registry *reg, uint32_t name)
{
	(void)data;
	(void)reg;
	(void)name;
}

static const struct wl_registry_listener reglistener = { global, global_remove };

static int
client(void)
{
	struct wl_display *d;
	struct wl_registry *reg;
	struct wl_shm_pool *pool;
	struct wl_buffer *buf;
	uint32_t *px;
	int fd, x, y;

	if((d = wl_display_connect("wayland-wltest")) == NULL){
		fprintf(stderr, "client: connect: %s\n", strerror(errno));
		return 1;
	}
	reg = wl_display_get_registry(d);
	wl_registry_add_listener(reg, &reglistener, NULL);
	wl_display_roundtrip(d);
	if(shm == NULL){
		fprintf(stderr, "client: no wl_shm\n");
		return 2;
	}
	fd = memfd_create("wltest", MFD_CLOEXEC);
	if(fd < 0 || ftruncate(fd, Size) < 0)
		return 3;
	px = mmap(NULL, Size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(px == MAP_FAILED)
		return 4;
	for(y = 0; y < H; y++)
		for(x = 0; x < W; x++)
			px[y*W + x] = pattern(x, y);
	pool = wl_shm_create_pool(shm, fd, Size);
	buf = wl_shm_pool_create_buffer(pool, 0, W, H, Stride, WL_SHM_FORMAT_ARGB8888);
	close(fd);
	if(wl_display_roundtrip(d) < 0)
		return 5;
	/* the server reads it now; this one waits until it hangs up */
	while(wl_display_dispatch(d) >= 0)
		;
	(void)buf;
	return 0;
}

/* ---- the server ---- */

static int timerfired;
static int bufferseen;
static struct wl_display *display;

static int
ontimer(void *data)
{
	(void)data;
	timerfired++;
	return 0;
}

static enum wl_iterator_result
onresource(struct wl_resource *r, void *data)
{
	struct wl_shm_buffer *b;
	uint32_t *px;
	int x, y, bad;

	(void)data;
	if((b = wl_shm_buffer_get(r)) == NULL)
		return WL_ITERATOR_CONTINUE;
	check("the client's wl_buffer: 64x64 ARGB8888",
		wl_shm_buffer_get_width(b) == W && wl_shm_buffer_get_height(b) == H &&
		wl_shm_buffer_get_stride(b) == Stride && wl_shm_buffer_get_format(b) == WL_SHM_FORMAT_ARGB8888, NULL);
	wl_shm_buffer_begin_access(b);
	px = wl_shm_buffer_get_data(b);
	bad = 0;
	for(y = 0; y < H; y++)
		for(x = 0; x < W; x++)
			if(px[y*W + x] != pattern(x, y))
				bad++;
	wl_shm_buffer_end_access(b);
	check("its pixels, as the client wrote them (shared memory, no copy)", bad == 0, NULL);
	bufferseen = 1;
	return WL_ITERATOR_STOP;
}

static void
onclient(struct wl_listener *l, void *data)
{
	(void)l;
	(void)data;
	check("a client connected", 1, NULL);
}

static struct wl_listener clientlistener = { .notify = onclient };

int
main(void)
{
	struct wl_event_loop *loop;
	struct wl_event_source *timer;
	struct wl_list *clients;
	struct wl_client *c;
	int pid, st, i;

	setenv("XDG_RUNTIME_DIR", "/tmp", 1);
	display = wl_display_create();
	check("wl_display_create", display != NULL, NULL);
	if(display == NULL)
		return 1;
	check("wl_display_add_socket wayland-wltest", wl_display_add_socket(display, "wayland-wltest") == 0, strerror(errno));
	check("wl_display_init_shm", wl_display_init_shm(display) == 0, NULL);
	wl_display_add_client_created_listener(display, &clientlistener);
	loop = wl_display_get_event_loop(display);
	timer = wl_event_loop_add_timer(loop, ontimer, NULL);
	wl_event_source_timer_update(timer, 50);

	switch(pid = fork()){
	case -1:
		check("fork", 0, strerror(errno));
		return 1;
	case 0:
		_exit(client());
	}
	/* serve until the client's buffer is there and the timer fired, at most 10 s */
	for(i = 0; i < 200 && !(bufferseen && timerfired); i++){
		wl_event_loop_dispatch(loop, 50);
		wl_display_flush_clients(display);
		clients = wl_display_get_client_list(display);
		wl_client_for_each(c, clients)
			if(!bufferseen)
				wl_client_for_each_resource(c, onresource, NULL);
	}
	check("the client's buffer came", bufferseen, NULL);
	check("the event loop's timer fired (epoll, timerfd)", timerfired == 1, NULL);
	wl_display_destroy_clients(display);
	waitpid(pid, &st, 0);
	check("the client ended well", WIFEXITED(st) && WEXITSTATUS(st) == 0, NULL);
	wl_display_destroy(display);
	if(nfail){
		printf("wltest: %d failed\n", nfail);
		return 1;
	}
	printf("wltest: all passed\n");
	return 0;
}
