/*
 * SPDX-License-Identifier: MIT
 *
 * wl_shm: a pool is the client's memfd, a segment of segment(3) where the
 * kernel put it (APE); wlsys keeps its descriptor (the segment stays named
 * in #g while it is open) and hands its name to wsys with each buffer.  It
 * never maps the pool itself.  Formats: argb8888 and xrgb8888, wsys' own
 * layout (it ignores the alpha).
 */
#include "dat.h"

void
poolput(Pool *p)
{
	if(--p->ref > 0)
		return;
	close(p->fd);
	free(p);
}

static void
bufferdestroy(struct wl_resource *r)
{
	Buffer *b;

	b = wl_resource_get_user_data(r);
	buffergone(b);
	unbuffer(b);
	if(b->pool != NULL)
		poolput(b->pool);
	free(b);
}

static void
bufferreqdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static const struct wl_buffer_interface bufferimpl = {
	bufferreqdestroy,
};

/* a wl_buffer for the client, its fields to be set; nil: no memory (posted) */
Buffer*
newbuffer(struct wl_client *c, uint32_t id)
{
	Buffer *b;

	if((b = calloc(1, sizeof *b)) == NULL){
		wl_client_post_no_memory(c);
		return NULL;
	}
	b->res = wl_resource_create(c, &wl_buffer_interface, 1, id);
	if(b->res == NULL){
		free(b);
		wl_client_post_no_memory(c);
		return NULL;
	}
	b->slot = -1;
	wl_resource_set_implementation(b->res, &bufferimpl, b, bufferdestroy);
	return b;
}

/*
 * the pixels of a wl_shm buffer, read from its segment (segment(3)'s data
 * file: wlsys maps no pool); nil if they cannot be read
 */
unsigned char*
pixels(Buffer *b)
{
	char path[160];
	unsigned char *px;
	int fd, n, m, k;

	if(b->pool == NULL)
		return NULL;	/* a dma-buf: in the GPU's memory */
	snprintf(path, sizeof path, "#g/%s/data", b->pool->seg);
	if((fd = open(path, O_RDONLY)) < 0)
		return NULL;
	n = b->stride*(b->h - 1) + 4*b->w;
	if((px = malloc(n)) == NULL){
		close(fd);
		return NULL;
	}
	for(m = 0; m < n; m += k)
		if((k = pread(fd, px + m, n - m, b->off + m)) <= 0)
			break;
	close(fd);
	if(m < n){
		free(px);
		return NULL;
	}
	return px;
}

/*
 * buffer b free again for its client: wl_buffer.release, once for each
 * time it was committed (a buffer wlsys made itself has none)
 */
void
relbuf(Buffer *b)
{
	if(b == NULL || b->res == NULL || b->freed)
		return;
	b->freed = 1;
	wl_buffer_send_release(b->res);
}

Buffer*
getbuffer(struct wl_resource *r)
{
	if(r == NULL || !wl_resource_instance_of(r, &wl_buffer_interface, &bufferimpl))
		return NULL;
	return wl_resource_get_user_data(r);
}

static void
poolcreatebuffer(struct wl_client *c, struct wl_resource *r, uint32_t id,
	int32_t off, int32_t w, int32_t h, int32_t stride, uint32_t format)
{
	Pool *p;
	Buffer *b;

	p = wl_resource_get_user_data(r);
	if(format != WL_SHM_FORMAT_ARGB8888 && format != WL_SHM_FORMAT_XRGB8888){
		wl_resource_post_error(r, WL_SHM_ERROR_INVALID_FORMAT, "format %#x: argb8888 and xrgb8888 only", format);
		return;
	}
	if(w <= 0 || h <= 0 || w > 16384 || h > 16384 || stride < 4*w || off < 0 ||
	   (int64_t)stride*h > p->size - (int64_t)off){
		wl_resource_post_error(r, WL_SHM_ERROR_INVALID_STRIDE, "buffer %dx%d stride %d at %d: not in the pool of %d",
			w, h, stride, off, p->size);
		return;
	}
	if((b = newbuffer(c, id)) == NULL)
		return;
	b->pool = p;
	p->ref++;
	b->off = off;
	b->w = w;
	b->h = h;
	b->stride = stride;
	b->format = format;
}

static void
pooldestroyres(struct wl_resource *r)
{
	Pool *p;

	p = wl_resource_get_user_data(r);
	p->res = NULL;
	poolput(p);
}

static void
poolreqdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static void
poolresize(struct wl_client *c, struct wl_resource *r, int32_t size)
{
	Pool *p;

	(void)c;
	p = wl_resource_get_user_data(r);
	if(size < p->size){
		wl_resource_post_error(r, WL_SHM_ERROR_INVALID_STRIDE, "a pool cannot shrink");
		return;
	}
	/* APE's ftruncate grew the segment where it is (its addresses reserved) */
	p->size = size;
}

static const struct wl_shm_pool_interface poolimpl = {
	poolcreatebuffer,
	poolreqdestroy,
	poolresize,
};

/*
 * the segment fd is the directory of in #g, if it is one: the last field
 * of /fd/nctl (dup(3), as proc(3)'s fd file) is the file's name
 */
int
segname(int fd, char *name, int n)
{
	char ctl[32], buf[512], *path, *e;
	int cfd, m;

	snprintf(ctl, sizeof ctl, "/fd/%dctl", fd);
	if((cfd = open(ctl, O_RDONLY)) < 0)
		return -1;
	m = read(cfd, buf, sizeof buf - 1);
	close(cfd);
	if(m <= 0)
		return -1;
	buf[m] = 0;
	if((e = strchr(buf, '\n')) != NULL)
		*e = 0;
	if((path = strrchr(buf, ' ')) == NULL)
		return -1;
	path++;
	if(strncmp(path, "#g/", 3) != 0 || path[3] == 0 || strchr(path+3, '/') != NULL)
		return -1;
	if(strlen(path+3) >= (size_t)n)
		return -1;
	strcpy(name, path+3);
	return 0;
}

static void
shmcreatepool(struct wl_client *c, struct wl_resource *r, uint32_t id, int32_t fd, int32_t size)
{
	Pool *p;

	if(size <= 0){
		wl_resource_post_error(r, WL_SHM_ERROR_INVALID_STRIDE, "pool of %d bytes", size);
		close(fd);
		return;
	}
	if((p = calloc(1, sizeof *p)) == NULL){
		close(fd);
		wl_client_post_no_memory(c);
		return;
	}
	if(segname(fd, p->seg, sizeof p->seg) < 0){
		/* a file, not shared memory: wsys could not see its bytes */
		wl_resource_post_error(r, WL_SHM_ERROR_INVALID_FD, "pool: not a segment (memfd_create, shm_open)");
		close(fd);
		free(p);
		return;
	}
	p->fd = fd;
	p->size = size;
	p->ref = 1;
	p->res = wl_resource_create(c, &wl_shm_pool_interface, wl_resource_get_version(r), id);
	if(p->res == NULL){
		close(fd);
		free(p);
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(p->res, &poolimpl, p, pooldestroyres);
	dprint("pool %s, %d bytes\n", p->seg, size);
}

static void
shmrelease(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

static const struct wl_shm_interface shmimpl = {
	shmcreatepool,
	shmrelease,
};

static void
shmbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_shm_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &shmimpl, NULL, NULL);
	wl_shm_send_format(r, WL_SHM_FORMAT_ARGB8888);
	wl_shm_send_format(r, WL_SHM_FORMAT_XRGB8888);
}

void
shminit(void)
{
	wl_global_create(display, &wl_shm_interface, 2, NULL, shmbind);
}
