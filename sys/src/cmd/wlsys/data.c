/*
 * SPDX-License-Identifier: MIT
 *
 * wl_data_device_manager: the clipboard is wsys' snarf buffer (rio's, when
 * wsys runs in rio), shared with every Plan 9 program.  A selection a
 * client sets is read from it at once (its text: text/plain;charset=utf-8
 * or another name of text) and written to the snarf buffer; the client
 * with the keyboard gets the snarf buffer offered as the selection, read
 * when it asks for it, so what a Plan 9 program snarfed since is there.  A
 * child process writes it to the client's pipe: wlsys never waits for a
 * client to read.  The snarf buffer outlives the client that set it (as
 * rio's); a selection unset by the client that set it empties the buffer
 * if it still holds that text (a password manager clearing what it
 * copied), not what another program snarfed since.  No drag and drop: a
 * drag's source is cancelled at once.
 */
#include "dat.h"

enum {
	Nmime = 16,
	Maxsnarf = 16*1024*1024,
	RFFDG = 1<<2,
	RFPROC = 1<<4,
	RFNOWAIT = 1<<6,
};

extern void _EXITS(char*);
extern long _WRITE(int, const void*, long);

typedef struct Source Source;
struct Source {
	struct wl_resource	*res;
	char	*mime[Nmime];
	int	nmime;
};

/* the names of text, the preferred first */
static char *textmime[] = {
	"text/plain;charset=utf-8",
	"UTF8_STRING",
	"text/plain",
	"TEXT",
	"STRING",
};

static struct wl_list devices;	/* wl_data_device resources */
static Source *selection;	/* the source of the selection set last */

/* a selection being read from its client */
static struct {
	int	fd;
	struct wl_event_source	*src;
	char	*buf;
	int	n, cap;
} rd = { -1 };

/* the snarf buffer when no window of wsys is there to write it to */
static char *mem;
static int nmem;

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/* a window of wlsys' in wsys: its snarf file is the snarf buffer */
static Win*
anywin(void)
{
	Surface *s;

	wl_list_for_each(s, &surfaces, link)
		if(s->win != NULL && !s->win->gone)
			return s->win;
	return NULL;
}

static int
snarfopen(int mode)
{
	char path[64];
	Win *w;

	if((w = anywin()) == NULL)
		return -1;
	snprintf(path, sizeof path, "/mnt/wsys/%d/snarf", w->id);
	return open(path, mode);
}

/* the snarf buffer's text; its length, -1 if none (*p malloced) */
static int
snarfread(char **p)
{
	char *b, *nb;
	int fd, n, cap, m;

	if((fd = snarfopen(O_RDONLY)) < 0){
		if(mem == NULL)
			return -1;
		if((*p = malloc(nmem + 1)) == NULL)
			return -1;
		memcpy(*p, mem, nmem);
		return nmem;
	}
	b = NULL;
	n = cap = 0;
	for(;;){
		if(n == cap){
			cap = cap ? 2*cap : 8192;
			if(cap > Maxsnarf || (nb = realloc(b, cap)) == NULL)
				break;
			b = nb;
		}
		if((m = read(fd, b + n, cap - n)) <= 0)
			break;
		n += m;
	}
	close(fd);
	*p = b;
	return b != NULL ? n : -1;
}

static void
snarfwrite(char *b, int n)
{
	int fd, m, k;

	free(mem);
	if((mem = malloc(n + 1)) != NULL){
		memcpy(mem, b, n);
		nmem = n;
	}
	if((fd = snarfopen(O_WRONLY)) < 0)
		return;
	if(n == 0 && write(fd, b, 0) < 0)	/* empty: a write of nothing at offset 0 */
		fprintf(stderr, "wlsys: snarf: %s\n", strerror(errno));
	for(m = 0; m < n; m += k)
		if((k = write(fd, b + m, n - m)) <= 0){
			fprintf(stderr, "wlsys: snarf: %s\n", strerror(errno));
			break;
		}
	close(fd);
	dprint("snarf: %d bytes\n", n);
}

/*
 * n bytes of b to the client's fd by a process of its own (no copy of
 * APE's state needed: it writes and exits), then fd closed here
 */
static void
sendto(int fd, char *b, int n)
{
	int m, k;

	switch(_RFORK(RFPROC|RFFDG|RFNOWAIT)){
	case -1:
		fprintf(stderr, "wlsys: rfork: %s\n", strerror(errno));
		break;
	case 0:
		for(m = 0; m < n; m += k)
			if((k = _WRITE(fd, b + m, n - m)) <= 0)
				break;
		_EXITS(NULL);
	}
	close(fd);
}

/* ---- wl_data_offer: the snarf buffer, to the client with the keyboard ---- */

static void
offeraccept(struct wl_client *c, struct wl_resource *r, uint32_t serial, const char *mime)
{
	(void)c; (void)r; (void)serial; (void)mime;
}

static void
offerreceive(struct wl_client *c, struct wl_resource *r, const char *mime, int32_t fd)
{
	char *b;
	int n;

	(void)c; (void)r; (void)mime;
	b = NULL;
	if((n = snarfread(&b)) < 0)
		n = 0;
	dprint("selection to a client: %d bytes as %s\n", n, mime);
	sendto(fd, b, n);
	free(b);
}

static void
offerfinish(struct wl_client *c, struct wl_resource *r)
{
	(void)c; (void)r;
}

static void
offeractions(struct wl_client *c, struct wl_resource *r, uint32_t actions, uint32_t pref)
{
	(void)c; (void)r; (void)actions; (void)pref;
}

static const struct wl_data_offer_interface offerimpl = {
	offeraccept,
	offerreceive,
	resdestroy,
	offerfinish,
	offeractions,
};

/* the snarf buffer as data device d's selection */
static void
offer(struct wl_resource *d)
{
	struct wl_resource *o;
	int i;

	o = wl_resource_create(wl_resource_get_client(d), &wl_data_offer_interface, wl_resource_get_version(d), 0);
	if(o == NULL){
		wl_client_post_no_memory(wl_resource_get_client(d));
		return;
	}
	wl_resource_set_implementation(o, &offerimpl, NULL, NULL);
	wl_data_device_send_data_offer(d, o);
	for(i = 0; i < (int)(sizeof textmime/sizeof textmime[0]); i++)
		wl_data_offer_send_offer(o, textmime[i]);
	wl_data_device_send_selection(d, o);
}

/* window w has the keyboard: its client's devices get the snarf buffer offered */
void
selectionfocus(Win *w)
{
	struct wl_resource *d;
	struct wl_client *c;

	if(w == NULL || w->s == NULL)
		return;
	c = wl_resource_get_client(w->s->res);
	wl_resource_for_each(d, &devices)
		if(wl_resource_get_client(d) == c)
			offer(d);
}

/* ---- reading a selection from its client ---- */

static void
readdone(void)
{
	if(rd.src != NULL)
		wl_event_source_remove(rd.src);
	rd.src = NULL;
	if(rd.fd >= 0)
		close(rd.fd);
	rd.fd = -1;
	free(rd.buf);
	rd.buf = NULL;
	rd.n = rd.cap = 0;
}

static int
onread(int fd, uint32_t mask, void *data)
{
	char *nb;
	int m;

	(void)mask; (void)data;
	if(rd.n == rd.cap){
		rd.cap = rd.cap ? 2*rd.cap : 8192;
		if(rd.cap > Maxsnarf || (nb = realloc(rd.buf, rd.cap)) == NULL){
			fprintf(stderr, "wlsys: a selection too big; not snarfed\n");
			readdone();
			return 0;
		}
		rd.buf = nb;
	}
	if((m = read(fd, rd.buf + rd.n, rd.cap - rd.n)) > 0){
		rd.n += m;
		return 0;
	}
	/* its end: the snarf buffer, offered again to whom has the keyboard */
	snarfwrite(rd.buf, rd.n);
	readdone();
	selectionfocus(keywin());
	return 0;
}

/* the selection of source s read from its client, as its text */
static void
readsel(Source *s)
{
	int p[2], i, j;

	for(i = 0; i < (int)(sizeof textmime/sizeof textmime[0]); i++)
		for(j = 0; j < s->nmime; j++)
			if(strcmp(s->mime[j], textmime[i]) == 0)
				goto Found;
	dprint("selection: no text (%d types)\n", s->nmime);
	return;
Found:
	readdone();	/* one being read: the newer one wins */
	if(pipe(p) < 0){
		fprintf(stderr, "wlsys: pipe: %s\n", strerror(errno));
		return;
	}
	wl_data_source_send_send(s->res, textmime[i], p[1]);
	close(p[1]);
	rd.fd = p[0];
	if((rd.src = wl_event_loop_add_fd(loop, rd.fd, WL_EVENT_READABLE, onread, NULL)) == NULL)
		readdone();
	dprint("selection: reading %s\n", textmime[i]);
}

/* ---- wl_data_source ---- */

static void
sourceoffer(struct wl_client *c, struct wl_resource *r, const char *mime)
{
	Source *s;

	(void)c;
	s = wl_resource_get_user_data(r);
	if(s->nmime < Nmime && (s->mime[s->nmime] = strdup(mime)) != NULL)
		s->nmime++;
}

static void
sourceactions(struct wl_client *c, struct wl_resource *r, uint32_t actions)
{
	(void)c; (void)r; (void)actions;
}

static const struct wl_data_source_interface sourceimpl = {
	sourceoffer,
	resdestroy,
	sourceactions,
};

static void
sourcedestroyres(struct wl_resource *r)
{
	Source *s;
	int i;

	s = wl_resource_get_user_data(r);
	if(selection == s)
		selection = NULL;	/* the snarf buffer stays */
	for(i = 0; i < s->nmime; i++)
		free(s->mime[i]);
	free(s);
}

/* the selection's text gone from the snarf buffer, if it is still there */
static void
unsetsel(void)
{
	char *b;
	int n;

	readdone();
	b = NULL;
	n = snarfread(&b);
	if(mem != NULL && n == nmem && memcmp(b, mem, n) == 0){
		snarfwrite("", 0);
		dprint("selection unset: the snarf buffer emptied\n");
	}
	free(b);
	selection = NULL;
}

/* ---- wl_data_device ---- */

static void
devicedrag(struct wl_client *c, struct wl_resource *r, struct wl_resource *src, struct wl_resource *origin, struct wl_resource *icon, uint32_t serial)
{
	(void)c; (void)r; (void)origin; (void)icon; (void)serial;
	if(src != NULL)
		wl_data_source_send_cancelled(src);
}

static void
deviceselection(struct wl_client *c, struct wl_resource *r, struct wl_resource *src, uint32_t serial)
{
	Source *s;

	(void)r; (void)serial;
	if(src == NULL){
		/*
		 * unset by the client whose selection it is: emptied, if the
		 * snarf buffer still holds its text (else another program's)
		 */
		if(selection != NULL && wl_resource_get_client(selection->res) == c)
			unsetsel();
		return;
	}
	s = wl_resource_get_user_data(src);
	if(selection != NULL && selection != s)
		wl_data_source_send_cancelled(selection->res);
	selection = s;
	readsel(s);
}

static const struct wl_data_device_interface deviceimpl = {
	devicedrag,
	deviceselection,
	resdestroy,	/* release */
};

static void
devicelink(struct wl_resource *r)
{
	wl_list_remove(wl_resource_get_link(r));
}

/* ---- wl_data_device_manager ---- */

static void
mgrsource(struct wl_client *c, struct wl_resource *r, uint32_t id)
{
	Source *s;

	if((s = calloc(1, sizeof *s)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	if((s->res = wl_resource_create(c, &wl_data_source_interface, wl_resource_get_version(r), id)) == NULL){
		free(s);
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(s->res, &sourceimpl, s, sourcedestroyres);
}

static void
mgrdevice(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *seat)
{
	struct wl_resource *d;
	Win *w;

	(void)seat;
	if((d = wl_resource_create(c, &wl_data_device_interface, wl_resource_get_version(r), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(d, &deviceimpl, NULL, devicelink);
	wl_list_insert(&devices, wl_resource_get_link(d));
	/* made while the client has the keyboard: the selection now */
	if((w = keywin()) != NULL && w->s != NULL && wl_resource_get_client(w->s->res) == c)
		offer(d);
}

static const struct wl_data_device_manager_interface mgrimpl = {
	mgrsource,
	mgrdevice,
};

static void
mgrbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &wl_data_device_manager_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &mgrimpl, NULL, NULL);
}

void
datainit(void)
{
	wl_list_init(&devices);
	wl_global_create(display, &wl_data_device_manager_interface, 3, NULL, mgrbind);
}
