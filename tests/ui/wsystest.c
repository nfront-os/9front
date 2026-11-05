/*
 * SPDX-License-Identifier: MIT
 *
 * wsystest: a client of wsys(4) in plain Plan 9 C, run by wsys -x -i: two
 * windows through /mnt/wsys/clone, their pixels written into the segments
 * ctl names, presented, and read back from the screen through draw(3);
 * focus and stacking by a click (/dev/mousein), a key (/dev/kbdin), a
 * resize (new segment), a window gone when its clone fid is closed.
 * Prints ok/FAIL lines and "wsystest: all passed".
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

enum {
	Bg = 0x777777,
	Focused = 0x335588,
	Red = 0xff0000,
	Green = 0x00ff00,
	Blue = 0x0000ff,
	Yellow = 0xffff00,
};

typedef struct Win Win;
struct Win {
	int	id;
	int	ctl;		/* the clone fid: closed, the window goes */
	int	ev;
	Rectangle r;
	int	w, h, stride;
	char	seg[64];
	uvlong	va;
	int	gen;
	int	focus;
	u32int	*px;
	char	evbuf[4096];	/* event lines read, not yet taken */
	char	*evp, *eve;
};

static Display *d;
static Win *wins[8];
static int nwins;

static void
fail(char *fmt, ...)
{
	va_list a;
	char buf[256], ctl[256];
	int i, n;

	va_start(a, fmt);
	vsnprint(buf, sizeof buf, fmt, a);
	va_end(a);
	print("FAIL %s\n", buf);
	/* what wsys says about the windows */
	for(i = 0; i < nwins; i++){
		n = pread(wins[i]->ctl, ctl, sizeof ctl - 1, 0);
		if(n > 0){
			ctl[n] = 0;
			print("ctl: %s", ctl);
		}
	}
	exits("fail");
}

static void
readctl(Win *w)
{
	char buf[256], *f[12];
	int n;

	if((n = pread(w->ctl, buf, sizeof buf - 1, 0)) <= 0)
		fail("read ctl: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) != 12)
		fail("ctl: %s", buf);
	w->id = atoi(f[0]);
	w->r = Rect(atoi(f[1]), atoi(f[2]), atoi(f[3]), atoi(f[4]));
	w->w = atoi(f[5]);
	w->h = atoi(f[6]);
	w->stride = atoi(f[7]);
	snprint(w->seg, sizeof w->seg, "%s", f[8]);
	w->va = strtoull(f[9], nil, 0);
	w->gen = atoi(f[10]);
	w->focus = atoi(f[11]);
}

/* the window's current segment: wsys gives the name and the address */
static void
attach(Win *w)
{
	void *p;

	if(w->px != nil)
		segdetach(w->px);
	p = segattach(0, w->seg, nil, w->stride*w->h);
	if(p == (void*)-1)
		fail("segattach %s: %r", w->seg);
	if((uvlong)p != w->va)
		fail("segment %s at %#p, ctl says %#llux", w->seg, p, w->va);
	w->px = p;
}

static void
ctl(Win *w, char *fmt, ...)
{
	va_list a;
	char buf[128];

	va_start(a, fmt);
	vsnprint(buf, sizeof buf, fmt, a);
	va_end(a);
	if(write(w->ctl, buf, strlen(buf)) != strlen(buf))
		fail("ctl %s: %r", buf);
}

static Win*
newwin(void)
{
	Win *w;
	char buf[64];

	w = mallocz(sizeof *w, 1);
	w->evp = w->eve = w->evbuf;
	if((w->ctl = open("/mnt/wsys/clone", ORDWR)) < 0)
		fail("open clone: %r");
	readctl(w);
	if(nwins < nelem(wins))
		wins[nwins++] = w;
	snprint(buf, sizeof buf, "/mnt/wsys/%d/event", w->id);
	if((w->ev = open(buf, OREAD)) < 0)
		fail("open %s: %r", buf);
	return w;
}

static void
fill(Win *w, Rectangle r, u32int rgb)
{
	int x, y;

	for(y = r.min.y; y < r.max.y; y++)
		for(x = r.min.x; x < r.max.x; x++)
			w->px[y*(w->stride/4) + x] = rgb;
}

/* the next event line of w that starts with c (others are skipped) */
static char*
event(Win *w, int c)
{
	char *l;
	int n;

	for(;;){
		while(w->evp < w->eve){
			l = w->evp;
			while(w->evp < w->eve && *w->evp != '\n')
				w->evp++;
			if(w->evp == w->eve)
				break;
			*w->evp++ = 0;
			if(l[0] == c)
				return l;
		}
		w->evp = w->eve = w->evbuf;
		alarm(5000);
		n = read(w->ev, w->evbuf, sizeof w->evbuf - 1);
		alarm(0);
		if(n <= 0)
			fail("window %d: no '%c' event: %r", w->id, c);
		w->eve = w->evbuf+n;
	}
}

static u32int
peek(Point p)
{
	uchar px[4];
	Image *s;

	s = d->image;
	if(!ptinrect(p, s->r))
		fail("peek %P not on the screen %R", p, s->r);
	if(unloadimage(s, Rect(p.x, p.y, p.x+1, p.y+1), px, sizeof px) != s->depth/8)
		fail("unloadimage: %r");
	if(s->depth == 16){
		ulong v, r, g, b;

		v = px[0] | px[1]<<8;
		r = v>>11 & 31;
		g = v>>5 & 63;
		b = v & 31;
		return (r<<3 | r>>2)<<16 | (g<<2 | g>>4)<<8 | (b<<3 | b>>2);
	}
	return px[2]<<16 | px[1]<<8 | px[0];
}

static int
near(u32int a, u32int b)
{
	int i, x;

	for(i = 0; i < 24; i += 8){
		x = (int)(a>>i & 0xff) - (int)(b>>i & 0xff);
		if(x < -8 || x > 8)
			return 0;
	}
	return 1;
}

static void
want(Point p, u32int rgb, char *what)
{
	u32int v;
	int i;

	/* wsys draws after it answers: a moment for the screen */
	for(i = 0; i < 50; i++){
		if(near(v = peek(p), rgb))
			break;
		sleep(20);
	}
	if(!near(v, rgb))
		fail("screen at %P is %06ux, want %06ux (%s)", p, v, rgb, what);
	print("ok screen at %P: %06ux (%s)\n", p, v, what);
}

static void
click(Point p)
{
	int fd;

	if((fd = open("/dev/mousein", OWRITE)) < 0)
		fail("open mousein: %r");
	fprint(fd, "A %d %d 1", p.x, p.y);
	sleep(50);
	fprint(fd, "A %d %d 0", p.x, p.y);
	close(fd);
}

static int
onnote(void*, char *msg)
{
	return strcmp(msg, "alarm") == 0;
}

void
main(void)
{
	Win *a, *b;
	char *l, *f[6];
	Point c;
	int fd;

	atnotify(onnote, 1);
	if((d = initdisplay(nil, nil, nil)) == nil)
		fail("initdisplay: %r");

	/* window a: 320x240, left half red, right half green */
	a = newwin();
	print("ok window %d: %R, %dx%d, segment %s at %#llux\n", a->id, a->r, a->w, a->h, a->seg, a->va);
	if(!a->focus)
		fail("a new window has the focus");
	ctl(a, "rect 100 100 420 340");
	l = event(a, 'r');
	if(tokenize(l, f, nelem(f)) != 4 || atoi(f[1]) != 320 || atoi(f[2]) != 240)
		fail("resize event: %s", l);
	readctl(a);
	attach(a);
	fill(a, Rect(0, 0, 160, 240), Red);
	fill(a, Rect(160, 0, 320, 240), Green);
	ctl(a, "present");
	want(Pt(180, 220), Red, "a, left");
	want(Pt(340, 220), Green, "a, right");
	want(Pt(98, 200), Focused, "a's border, focused");
	want(Pt(60, 60), Bg, "background");

	/* window b over a's right part: blue; it takes the focus */
	b = newwin();
	ctl(b, "rect 300 200 500 400");
	event(b, 'r');
	readctl(b);
	attach(b);
	fill(b, Rect(0, 0, b->w, b->h), Blue);
	ctl(b, "present");
	want(Pt(400, 300), Blue, "b over a");
	want(Pt(340, 150), Green, "a beside b");
	l = event(a, 'f');
	if(strcmp(l, "f 0") != 0)
		fail("a lost the focus: %s", l);
	print("ok a: %s\n", l);

	/* a click on a (where b is not): a comes up and takes the focus, the click is its event */
	click(Pt(150, 250));
	want(Pt(350, 250), Green, "a raised by a click");
	l = event(a, 'm');
	print("ok a: %s\n", l);
	if(tokenize(l, f, nelem(f)) < 4 || atoi(f[1]) != 50 || atoi(f[2]) != 150 || atoi(f[3]) != 1)
		fail("a's click: %s %s %s", f[1], f[2], f[3]);
	readctl(a);
	if(!a->focus)
		fail("a has no focus after the click");

	/* a key for a (kbdfs gives it to a reader of cons: wsys) */
	if((fd = open("/dev/kbdin", OWRITE)) < 0)
		fail("open kbdin: %r");
	/* a kbd(3) message: c, the rune, a null byte */
	if(write(fd, "cx", 3) != 3)
		fail("write kbdin: %r");
	close(fd);
	l = event(a, 'k');
	if(strcmp(l, "k 78") != 0)
		fail("a's key: %s", l);
	print("ok a: %s\n", l);

	/* a resized: a new segment (the old one detached), yellow */
	ctl(a, "rect 80 80 280 200");
	l = event(a, 'r');
	print("ok a: %s\n", l);
	readctl(a);
	if(a->w != 200 || a->h != 120 || a->gen != 2)
		fail("a after the resize: %dx%d generation %d", a->w, a->h, a->gen);
	attach(a);
	fill(a, Rect(0, 0, a->w, a->h), Yellow);
	ctl(a, "present");
	want(Pt(180, 140), Yellow, "a resized");
	want(Pt(400, 300), Blue, "b where a was");

	/* part of a window: a's right half red again */
	fill(a, Rect(100, 0, 200, 120), Red);
	ctl(a, "present 100 0 200 120");
	want(Pt(230, 140), Red, "a's right half presented");
	want(Pt(120, 140), Yellow, "a's left half as it was");

	/* b gone with its clone fid */
	close(b->ctl);
	want(Pt(400, 300), Bg, "background where b was");
	c = Pt(120, 140);
	want(c, Yellow, "a stays");

	print("wsystest: all passed\n");
	exits(nil);
}
