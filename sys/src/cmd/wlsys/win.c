/*
 * SPDX-License-Identifier: MIT
 *
 * The windows of wsys (/mnt/wsys, wsys(4)): one for each toplevel with a
 * buffer, made through clone; its ctl takes the buffers (the pool's segment
 * by name, the buffer's offset in it) and presents; its event and kbd files
 * are sources of the event loop (APE's poll).  A buffer is one of wsys' 8 of
 * the window while it is attached there; when all are taken, the one
 * presented longest ago that wsys has let go of is announced anew.
 */
#include "dat.h"

int screenw, screenh;
static uint32_t clock;

static int
ctlmsg(Win *w, char *fmt, ...)
{
	va_list a;
	char buf[512];
	int n;

	va_start(a, fmt);
	n = vsnprintf(buf, sizeof buf, fmt, a);
	va_end(a);
	if(n >= (int)sizeof buf)
		n = sizeof buf - 1;
	dprint("ctl %d: %s\n", w->id, buf);
	if(write(w->ctl, buf, n) != n){
		fprintf(stderr, "wlsys: window %d: %s: %s\n", w->id, buf, strerror(errno));
		return -1;
	}
	return 0;
}

/* the window's place on the screen, from its ctl */
static int
winrect(Win *w, Rect *r)
{
	char buf[256];
	int n, id;

	if((n = pread(w->ctl, buf, sizeof buf - 1, 0)) <= 0)
		return -1;
	buf[n] = 0;
	if(sscanf(buf, "%d %d %d %d %d", &id, &r->x0, &r->y0, &r->x1, &r->y1) != 5)
		return -1;
	return 0;
}

void
winlabel(Win *w, char *s)
{
	char buf[256], *p;

	snprintf(buf, sizeof buf, "%s", s != NULL ? s : "");
	for(p = buf; *p; p++)
		if(*p == '\n')
			*p = ' ';
	ctlmsg(w, "label %s", buf);
}

/* no more input from the window */
static void
winquiet(Win *w)
{
	seatforget(w);
	if(w->evsrc != NULL){
		wl_event_source_remove(w->evsrc);
		w->evsrc = NULL;
	}
	if(w->kbdsrc != NULL){
		wl_event_source_remove(w->kbdsrc);
		w->kbdsrc = NULL;
	}
}

/* the window is gone in wsys (deleted): the client is told to close */
static void
wingone(Win *w)
{
	if(w->gone)
		return;
	w->gone = 1;
	dprint("window %d gone\n", w->id);
	winquiet(w);
	if(w->s != NULL)
		toplevelclose(w->s);
}

/* buffer i let go of by wsys: the client may write it again */
static void
released(Win *w, int i)
{
	Buffer *b;

	if(i < 0 || i >= Nslot || !w->busy[i])
		return;
	w->busy[i] = 0;
	if((b = w->slot[i]) != NULL)
		wl_buffer_send_release(b->res);
	frames(&w->frames[i], msec());
}

static void
event(Win *w, char *l)
{
	int x, y, b, i;
	uint32_t ms;

	if(l[0] != 'b')
		dprint("event %d: %s\n", w->id, l);
	switch(l[0]){
	case 'm':
		if(sscanf(l+1, "%d %d %d %u", &x, &y, &b, &ms) == 4)
			pointer(w, x, y, b, ms);
		break;
	case 'r':
		if(sscanf(l+1, "%d %d", &x, &y) != 2 || (x == w->w && y == w->h))
			break;
		w->w = x;
		w->h = y;
		if(w->s != NULL)
			configure(w->s);
		break;
	case 'f':
		focus(w, atoi(l+1));
		break;
	case 'b':
		i = atoi(l+1);
		released(w, i);
		break;
	}
}

static int
onevent(int fd, uint32_t mask, void *data)
{
	Win *w;
	char *p, *e, *nl;
	int n;

	(void)mask;
	w = data;
	n = read(fd, w->evbuf + w->nev, sizeof w->evbuf - 1 - w->nev);
	if(n <= 0){
		wingone(w);
		return 0;
	}
	w->nev += n;
	p = w->evbuf;
	e = w->evbuf + w->nev;
	while((nl = memchr(p, '\n', e - p)) != NULL){
		*nl = 0;
		event(w, p);
		if(w->gone)
			return 0;
		p = nl + 1;
	}
	w->nev = e - p;
	if(w->nev == (int)sizeof w->evbuf - 1)
		w->nev = 0;	/* a line too long: dropped */
	memmove(w->evbuf, p, w->nev);
	return 0;
}

static int
onkbd(int fd, uint32_t mask, void *data)
{
	Win *w;
	char *p, *e, *z;
	int n;

	(void)mask;
	w = data;
	n = read(fd, w->kbdbuf + w->nkbd, sizeof w->kbdbuf - w->nkbd);
	if(n <= 0){
		if(w->kbdsrc != NULL){
			wl_event_source_remove(w->kbdsrc);
			w->kbdsrc = NULL;
		}
		return 0;
	}
	w->nkbd += n;
	/* kbd(3) messages, each ends with a null byte */
	p = w->kbdbuf;
	e = w->kbdbuf + w->nkbd;
	while((z = memchr(p, 0, e - p)) != NULL){
		keys(w, p, z - p);
		p = z + 1;
	}
	w->nkbd = e - p;
	if(w->nkbd == (int)sizeof w->kbdbuf)
		w->nkbd = 0;
	memmove(w->kbdbuf, p, w->nkbd);
	return 0;
}

static Win*
mkwin(Surface *s, int bw, int bh)
{
	Win *w;
	char buf[256], path[64];
	Rect r;
	int n, i;

	if((w = calloc(1, sizeof *w)) == NULL)
		return NULL;
	w->ev = w->kbd = -1;
	for(i = 0; i < Nslot; i++)
		wl_list_init(&w->frames[i]);
	if((w->ctl = open("/mnt/wsys/clone", O_RDWR)) < 0){
		fprintf(stderr, "wlsys: /mnt/wsys/clone: %s\n", strerror(errno));
		free(w);
		return NULL;
	}
	n = pread(w->ctl, buf, sizeof buf - 1, 0);
	if(n <= 0)
		goto Err;
	buf[n] = 0;
	w->id = atoi(buf);
	snprintf(path, sizeof path, "/mnt/wsys/%d/event", w->id);
	if((w->ev = open(path, O_RDONLY)) < 0)
		goto Err;
	snprintf(path, sizeof path, "/mnt/wsys/%d/kbd", w->id);
	if((w->kbd = open(path, O_RDONLY)) < 0)
		goto Err;
	w->s = s;
	if(winrect(w, &r) < 0)
		goto Err;
	w->w = r.x1 - r.x0;
	w->h = r.y1 - r.y0;
	if(bw != w->w || bh != w->h){
		ctlmsg(w, "rect %d %d %d %d", r.x0, r.y0, r.x0 + bw, r.y0 + bh);
		w->w = bw;
		w->h = bh;
	}
	if(s->title != NULL)
		winlabel(w, s->title);
	w->evsrc = wl_event_loop_add_fd(loop, w->ev, WL_EVENT_READABLE, onevent, w);
	w->kbdsrc = wl_event_loop_add_fd(loop, w->kbd, WL_EVENT_READABLE, onkbd, w);
	if(w->evsrc == NULL || w->kbdsrc == NULL)
		goto Err;
	w->fresh = 1;
	dprint("window %d for %dx%d\n", w->id, bw, bh);
	return w;
Err:
	fprintf(stderr, "wlsys: a window: %s\n", strerror(errno));
	if(w->evsrc != NULL)
		wl_event_source_remove(w->evsrc);
	if(w->kbdsrc != NULL)
		wl_event_source_remove(w->kbdsrc);
	if(w->kbd >= 0)
		close(w->kbd);
	if(w->ev >= 0)
		close(w->ev);
	close(w->ctl);
	free(w);
	return NULL;
}

/* buffer b as one of window w's, announced if it is not yet */
static int
slotof(Win *w, Buffer *b)
{
	int i, old;

	if(b->win == w && b->slot >= 0)
		return b->slot;
	if(b->win != NULL)
		unbuffer(b);
	old = -1;
	for(i = 0; i < Nslot; i++){
		if(w->slot[i] == NULL){
			old = i;
			break;
		}
		if(!w->busy[i] && (old < 0 || w->age[i] < w->age[old]))
			old = i;
	}
	if(old < 0)
		return -1;	/* all of them presented, none released yet */
	if(w->slot[old] != NULL)
		unbuffer(w->slot[old]);
	if(b->pool != NULL){
		if(ctlmsg(w, "buffer %d %s %d %d %d %d", old, b->pool->seg, b->w, b->h, b->stride, b->off) < 0)
			return -1;
	}else if(ctlmsg(w, "gpubuffer %d %s %d %d %d", old, b->token, b->w, b->h, b->stride) < 0)
		return -1;
	w->slot[old] = b;
	b->win = w;
	b->slot = old;
	return old;
}

/* the buffer let go of by its window */
void
unbuffer(Buffer *b)
{
	Win *w;

	if((w = b->win) == NULL)
		return;
	if(!w->gone)
		ctlmsg(w, "unbuffer %d", b->slot);
	w->slot[b->slot] = NULL;
	if(w->busy[b->slot]){
		/* no release for a buffer gone; the frame done all the same */
		w->busy[b->slot] = 0;
		frames(&w->frames[b->slot], msec());
	}
	b->win = NULL;
	b->slot = -1;
}

/* the surface's buffer to its window: made, sized, presented */
void
show(Surface *s)
{
	Buffer *b;
	Win *w;
	Rect r, d;
	int i, ok;

	if((b = s->buf) == NULL)
		return;
	if((w = s->win) == NULL){
		if((w = mkwin(s, b->w, b->h)) == NULL)
			return;
		s->win = w;
		sendenter(s);
	}
	if(w->gone){
		wl_buffer_send_release(b->res);
		return;
	}
	/*
	 * the client's size, unless it has not seen the one swept in wsys
	 * yet (a configure not acknowledged)
	 */
	if((b->w != w->w || b->h != w->h) && s->acked == s->serial && winrect(w, &r) == 0){
		ctlmsg(w, "rect %d %d %d %d", r.x0, r.y0, r.x0 + b->w, r.y0 + b->h);
		w->w = b->w;
		w->h = b->h;
		w->fresh = 1;
	}
	if((i = slotof(w, b)) < 0){
		/* wsys has all of them: this picture is skipped */
		wl_buffer_send_release(b->res);
		frames(&s->pframes, msec());
		return;
	}
	d = s->pdamage;
	if(w->fresh){
		/* a new window, or a new size: all of it */
		d = (Rect){0, 0, b->w, b->h};
		w->fresh = 0;
	}
	if(d.x0 < 0)
		d.x0 = 0;
	if(d.y0 < 0)
		d.y0 = 0;
	if(d.x1 > b->w)
		d.x1 = b->w;
	if(d.y1 > b->h)
		d.y1 = b->h;
	if(w->busy[i])
		frames(&w->frames[i], msec());	/* presented again before released */
	wl_list_insert_list(&w->frames[i], &s->pframes);
	wl_list_init(&s->pframes);
	w->age[i] = ++clock;
	w->busy[i] = 1;
	if(d.x0 == 0 && d.y0 == 0 && d.x1 == b->w && d.y1 == b->h)
		ok = ctlmsg(w, "present %d", i);
	else if(d.x0 < d.x1 && d.y0 < d.y1)
		ok = ctlmsg(w, "present %d %d %d %d %d", i, d.x0, d.y0, d.x1, d.y1);
	else
		ok = -1;	/* nothing damaged: nothing to show, the buffer is free again */
	if(ok < 0)
		released(w, i);
}

/* the surface's window goes (no buffer, the toplevel destroyed) */
void
unshow(Surface *s)
{
	Win *w;
	int i;

	if((w = s->win) == NULL)
		return;
	s->win = NULL;
	for(i = 0; i < Nslot; i++)
		if(w->slot[i] != NULL)
			unbuffer(w->slot[i]);
	winquiet(w);
	close(w->kbd);
	close(w->ev);
	close(w->ctl);
	free(w);
}

/* the screen's size, from wsys' screen file (an image(6) header); 0 if none */
int
wsysinit(void)
{
	char buf[5*12+1];
	int fd, n, x0, y0, x1, y1;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return -1;
	n = read(fd, buf, 5*12);
	close(fd);
	if(n != 5*12)
		return -1;
	buf[n] = 0;
	if(sscanf(buf, "%*s %d %d %d %d", &x0, &y0, &x1, &y1) != 4)
		return -1;
	screenw = x1 - x0;
	screenh = y1 - y0;
	return 0;
}
