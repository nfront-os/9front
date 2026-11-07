/*
 * SPDX-License-Identifier: MIT
 *
 * zwp_linux_dmabuf_v1 (version 3): GPU clients' buffers.  On 9front a
 * dma-buf fd is the read end of a pipe holding "dmabuf token\n", gpu(4)'s
 * export token of the object (Mesa's plan9/lib/dmabuf.c); it came here
 * through APE's SCM_RIGHTS as the same pipe.  wlsys reads the token and
 * hands it to wsys (gpubuffer), which imports the object into a context of
 * its own and reads the pixels where the processor sees them.  One plane,
 * linear, argb8888 or xrgb8888.  No fences: the client's rendering must be
 * done when it commits (Mesa's plan9 WSI waits for it).
 */
#include <sys/stat.h>
#include <linux-dmabuf-unstable-v1-server-protocol.h>
#include "dat.h"

enum {
	Fargb = 0x34325241,	/* DRM_FORMAT_ARGB8888, 'AR24' */
	Fxrgb = 0x34325258,	/* DRM_FORMAT_XRGB8888, 'XR24' */
	Ntok = 64,
	Nknown = 64,
};

#define	MODLINEAR	0ULL
#define	MODINVALID	0x00ffffffffffffffULL	/* the driver's own: linear here */

typedef struct Params Params;
struct Params {
	struct wl_resource	*res;
	int	added;
	int	used;
	char	token[Ntok];
	uint32_t	off, stride;
};

/*
 * tokens read, by the pipe's qid: a client may send a dma-buf again (a
 * buffer made anew) and the pipe holds the token only once
 */
static struct {
	ino_t	ino;
	char	token[Ntok];
} known[Nknown];
static int nknown;

static int
token(int fd, char *buf)
{
	struct stat st;
	char msg[Ntok+16], *e;
	int i, m;

	if(fstat(fd, &st) < 0)
		return -1;
	for(i = 0; i < nknown; i++)
		if(known[i].ino == st.st_ino){
			strcpy(buf, known[i].token);
			return 0;
		}
	/* the whole message, or not a dma-buf: never wait for a pipe */
	if(!S_ISFIFO(st.st_mode) || st.st_size < 9 || st.st_size >= (off_t)sizeof msg)
		return -1;
	if((m = read(fd, msg, st.st_size)) != st.st_size)
		return -1;
	msg[m] = 0;
	if(strncmp(msg, "dmabuf ", 7) != 0 || (e = strchr(msg, '\n')) == NULL || e[1] != 0 ||
	   e - (msg+7) >= Ntok || e == msg+7)
		return -1;
	*e = 0;
	strcpy(buf, msg+7);
	i = nknown < Nknown ? nknown++ : st.st_ino % Nknown;
	known[i].ino = st.st_ino;
	strcpy(known[i].token, buf);
	return 0;
}

static void
paramsdestroyres(struct wl_resource *r)
{
	free(wl_resource_get_user_data(r));
}

static void
paramsreqdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static void
paramsadd(struct wl_client *c, struct wl_resource *r, int32_t fd, uint32_t plane,
	uint32_t off, uint32_t stride, uint32_t modhi, uint32_t modlo)
{
	Params *p;
	uint64_t mod;
	int ok;

	(void)c;
	p = wl_resource_get_user_data(r);
	mod = (uint64_t)modhi<<32 | modlo;
	if(p->used){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED, "params used");
		close(fd);
		return;
	}
	if(plane != 0){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX, "plane %u: one plane only", plane);
		close(fd);
		return;
	}
	if(p->added){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET, "plane 0 set already");
		close(fd);
		return;
	}
	if(mod != MODLINEAR && mod != MODINVALID){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT, "modifier %#llx: linear only",
			(unsigned long long)mod);
		close(fd);
		return;
	}
	ok = token(fd, p->token);
	close(fd);	/* the token stays while the client's memory does */
	if(ok < 0){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS,
			"plane 0: not a dma-buf of gpu(4) (no token in the fd)");
		return;
	}
	p->added = 1;
	p->off = off;
	p->stride = stride;
	dprint("dmabuf %s, stride %u\n", p->token, stride);
}

/* the buffer, or nil: failed (sent as an event, or posted when immed) */
static Buffer*
mkbuffer(struct wl_client *c, struct wl_resource *r, uint32_t id,
	int32_t w, int32_t h, uint32_t format, uint32_t flags, int immed)
{
	Params *p;
	Buffer *b;
	char *why;

	p = wl_resource_get_user_data(r);
	if(p->used){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED, "params used");
		return NULL;
	}
	p->used = 1;
	if(!p->added){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE, "no plane");
		return NULL;
	}
	if(w <= 0 || h <= 0 || w > 16384 || h > 16384){
		wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS, "size %dx%d", w, h);
		return NULL;
	}
	why = NULL;
	if(format != Fargb && format != Fxrgb)
		why = "format: argb8888 and xrgb8888 only";
	else if(p->stride < 4*(uint32_t)w)
		why = "stride shorter than a row";
	else if(p->off != 0)
		why = "offset: 0 only";	/* wsys' gpubuffer: the object from its start */
	else if(flags != 0)
		why = "flags: none";
	if(why != NULL){
		dprint("dmabuf %s: %s\n", p->token, why);
		if(immed)
			wl_resource_post_error(r, ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER, "%s", why);
		else
			zwp_linux_buffer_params_v1_send_failed(r);
		return NULL;
	}
	if((b = newbuffer(c, id)) == NULL)
		return NULL;
	strcpy(b->token, p->token);
	b->w = w;
	b->h = h;
	b->stride = p->stride;
	b->format = format;
	return b;
}

static void
paramscreate(struct wl_client *c, struct wl_resource *r, int32_t w, int32_t h, uint32_t format, uint32_t flags)
{
	Buffer *b;

	if((b = mkbuffer(c, r, 0, w, h, format, flags, 0)) != NULL)
		zwp_linux_buffer_params_v1_send_created(r, b->res);
}

static void
paramscreateimmed(struct wl_client *c, struct wl_resource *r, uint32_t id,
	int32_t w, int32_t h, uint32_t format, uint32_t flags)
{
	mkbuffer(c, r, id, w, h, format, flags, 1);
}

static const struct zwp_linux_buffer_params_v1_interface paramsimpl = {
	paramsreqdestroy,
	paramsadd,
	paramscreate,
	paramscreateimmed,
};

static void
dmabufreqdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static void
dmabufcreateparams(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	Params *p;

	if((p = calloc(1, sizeof *p)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	p->res = wl_resource_create(c, &zwp_linux_buffer_params_v1_interface, wl_resource_get_version(r), id);
	if(p->res == NULL){
		free(p);
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p->res, &paramsimpl, p, paramsdestroyres);
}

static const struct zwp_linux_dmabuf_v1_interface dmabufimpl = {
	dmabufreqdestroy,
	dmabufcreateparams,
	NULL,	/* get_default_feedback: version 4 */
	NULL,	/* get_surface_feedback */
};

static void
dmabufbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	static const uint32_t fmts[] = { Fargb, Fxrgb };
	struct wl_resource *r;
	int i;

	(void)data;
	if((r = wl_resource_create(c, &zwp_linux_dmabuf_v1_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &dmabufimpl, NULL, NULL);
	for(i = 0; i < 2; i++){
		zwp_linux_dmabuf_v1_send_format(r, fmts[i]);
		if(version >= 3){
			zwp_linux_dmabuf_v1_send_modifier(r, fmts[i], MODLINEAR>>32, MODLINEAR & 0xffffffff);
			zwp_linux_dmabuf_v1_send_modifier(r, fmts[i], MODINVALID>>32, MODINVALID & 0xffffffff);
		}
	}
}

/* only when there is a GPU: wsys imports the buffers from /dev/gpu/0 */
void
dmabufinit(void)
{
	if(access("/dev/gpu/0", 0) < 0){
		dprint("no /dev/gpu/0: no linux-dmabuf\n");
		return;
	}
	wl_global_create(display, &zwp_linux_dmabuf_v1_interface, 3, NULL, dmabufbind);
}
