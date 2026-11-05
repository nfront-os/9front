/*
 * SPDX-License-Identifier: MIT
 *
 * wsystest: a client of wsys(4) in plain Plan 9 C, run by wsys -x -i: two
 * windows through /mnt/wsys/clone, their pixels in segments of the client's
 * own (in the range ctl gives each window), announced with buffer, shown with
 * present and read back from the screen through draw(3): a swap chain of two
 * buffers released with b events, focus and stacking by a click
 * (/dev/mousein), a key (/dev/kbdin), a resize that keeps the picture until a
 * new buffer comes, part of a buffer presented, a buffer outside the range
 * refused, a window gone when its clone fid is closed, no segment left in #g.
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

enum {
	Nbuf = 2,
	Bufstep = 0x10000000,	/* buffer i at base + i*Bufstep in the window's range */
};

typedef struct Buf Buf;
struct Buf {
	int	w, h, stride;
	u32int	*px;
};

typedef struct Win Win;
struct Win {
	int	id;
	int	ctl;		/* the clone fid: closed, the window goes */
	int	ev;
	Rectangle r;
	uvlong	base, size;	/* where its buffers go */
	int	focus;
	Buf	buf[Nbuf];
	char	evbuf[4096];	/* event lines read, not yet taken */
	char	*evp, *eve;
};

static Display *d;
static Point o;		/* where wsys draws: screen coordinates below are from there */
static Win *wins[8];
static int nwins;

static void ctl(Win*, char*, ...);
static char *event(Win*, int);

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
	char buf[256], *f[10];
	int n;

	if((n = pread(w->ctl, buf, sizeof buf - 1, 0)) <= 0)
		fail("read ctl: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) != 10)
		fail("ctl: %s", buf);
	w->id = atoi(f[0]);
	w->r = Rect(atoi(f[1]), atoi(f[2]), atoi(f[3]), atoi(f[4]));
	w->base = strtoull(f[7], nil, 0);
	w->size = strtoull(f[8], nil, 0);
	w->focus = atoi(f[9]);
}

static char*
segname(Win *w, int i)
{
	static char name[64];
	static int gen;

	snprint(name, sizeof name, "wsystest.%d.%d.%d.%d", getpid(), w->id, i, ++gen);
	return name;
}

/*
 * buffer i of w x h at va: a segment of ours, announced to wsys; its #g
 * entry goes at once (the segment lives while wsys or we have it attached)
 */
static int
mkbuf(Win *w, int i, int bw, int bh, uvlong va, int check)
{
	char *name, path[128], msg[128];
	Buf *b;
	void *p;
	int fd, len;

	b = &w->buf[i];
	if(b->px != nil){
		ctl(w, "unbuffer %d", i);	/* wsys first: the address is taken in it too */
		segdetach(b->px);
		b->px = nil;
	}
	name = segname(w, i);
	len = bw*bh*4;
	snprint(path, sizeof path, "#g/%s", name);
	if((fd = create(path, OREAD, DMDIR|0600)) < 0)
		fail("create %s: %r", path);
	close(fd);
	snprint(path, sizeof path, "#g/%s/ctl", name);
	if((fd = open(path, OWRITE)) < 0 || fprint(fd, "va %#llux %#ux", va, len) < 0)
		fail("%s: %r", path);
	close(fd);
	if((p = segattach(0, name, nil, len)) == (void*)-1)
		fail("segattach %s: %r", name);
	snprint(msg, sizeof msg, "buffer %d %s %d %d %d", i, name, bw, bh, bw*4);
	if(write(w->ctl, msg, strlen(msg)) != strlen(msg)){
		char err[ERRMAX];

		rerrstr(err, sizeof err);	/* wsys' answer, kept through the cleanup */
		snprint(path, sizeof path, "#g/%s", name);
		remove(path);
		segdetach(p);
		werrstr("%s", err);
		if(check)
			fail("%s: %r", msg);
		return -1;
	}
	snprint(path, sizeof path, "#g/%s", name);
	if(remove(path) < 0)
		fail("remove %s: %r", path);
	b->w = bw;
	b->h = bh;
	b->stride = bw*4;
	b->px = p;
	return 0;
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
fill(Buf *b, Rectangle r, u32int rgb)
{
	int x, y;

	for(y = r.min.y; y < r.max.y; y++)
		for(x = r.min.x; x < r.max.x; x++)
			b->px[y*(b->stride/4) + x] = rgb;
}

/* present buffer i (all of it, or r) and wait for its release */
static void
present(Win *w, int i, Rectangle *r)
{
	char want[16], *l;

	if(r == nil)
		ctl(w, "present %d", i);
	else
		ctl(w, "present %d %d %d %d %d", i, r->min.x, r->min.y, r->max.x, r->max.y);
	snprint(want, sizeof want, "b %d", i);
	l = event(w, 'b');
	if(strcmp(l, want) != 0)
		fail("window %d: %s after present %d", w->id, l, i);
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

/* button b held from p to q, then up */
static void
drag(Point p, Point q, int b)
{
	int fd;

	if((fd = open("/dev/mousein", OWRITE)) < 0)
		fail("open mousein: %r");
	fprint(fd, "A %d %d %d", p.x, p.y, b);
	sleep(50);
	fprint(fd, "A %d %d %d", (p.x+q.x)/2, (p.y+q.y)/2, b);
	sleep(50);
	fprint(fd, "A %d %d %d", q.x, q.y, b);
	sleep(50);
	fprint(fd, "A %d %d 0", q.x, q.y);
	close(fd);
}

static int
onnote(void*, char *msg)
{
	return strcmp(msg, "alarm") == 0;
}

static Point
P(int x, int y)
{
	return addpt(o, Pt(x, y));
}

static Rectangle
R(int x0, int y0, int x1, int y1)
{
	return rectaddpt(Rect(x0, y0, x1, y1), o);
}

static void
ctlrect(Win *w, Rectangle r)
{
	ctl(w, "rect %d %d %d %d", r.min.x, r.min.y, r.max.x, r.max.y);
}

/* the origin of wsys' screen (its rio window, when it runs in one): the
 * rectangle in the header of the image(6) its screen file gives */
static void
screenorigin(void)
{
	char buf[5*12+1], *f[5];
	int fd, n;

	if((fd = open("/mnt/wsys/screen", OREAD)) < 0 || (n = readn(fd, buf, 5*12)) != 5*12)
		fail("/mnt/wsys/screen: %r");
	close(fd);
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) != 5)
		fail("/mnt/wsys/screen: %s", buf);
	o = Pt(atoi(f[1]), atoi(f[2]));
	print("ok wsys draws in [%s %s] [%s %s]\n", f[1], f[2], f[3], f[4]);
	if(atoi(f[3]) - o.x < 520 || atoi(f[4]) - o.y < 420)
		fail("wsys' screen is too small for the test");
}

static int
leftover(void)
{
	Dir *d;
	int fd, n, i, left;

	if((fd = open("#g", OREAD)) < 0)
		fail("open #g: %r");
	n = dirreadall(fd, &d);
	close(fd);
	left = 0;
	for(i = 0; i < n; i++)
		if(strncmp(d[i].name, "wsystest.", 9) == 0){
			print("left in #g: %s\n", d[i].name);
			left++;
		}
	free(d);
	return left;
}

void
main(void)
{
	Win *a, *b, *c;
	char *l, *f[6];
	Rectangle r;
	int fd;

	atnotify(onnote, 1);
	if((d = initdisplay(nil, nil, nil)) == nil)
		fail("initdisplay: %r");
	screenorigin();

	/* window a: 320x240; buffer 0 left half red, right half green */
	a = newwin();
	print("ok window %d: %R, range %#llux %#llux\n", a->id, a->r, a->base, a->size);
	if(!a->focus)
		fail("a new window has the focus");
	ctlrect(a, R(100, 100, 420, 340));
	l = event(a, 'r');
	if(tokenize(l, f, nelem(f)) != 3 || atoi(f[1]) != 320 || atoi(f[2]) != 240)
		fail("resize event: %s", l);
	readctl(a);
	mkbuf(a, 0, 320, 240, a->base, 1);
	fill(&a->buf[0], Rect(0, 0, 160, 240), Red);
	fill(&a->buf[0], Rect(160, 0, 320, 240), Green);
	present(a, 0, nil);
	print("ok a: buffer 0 presented, released\n");
	want(P(180, 220), Red, "a, left");
	want(P(340, 220), Green, "a, right");
	want(P(98, 200), Focused, "a's border, focused");
	want(P(60, 60), Bg, "background");

	/* a swap chain: buffer 1 blue on top, then buffer 0 again */
	mkbuf(a, 1, 320, 240, a->base + Bufstep, 1);
	fill(&a->buf[1], Rect(0, 0, 320, 240), Blue);
	present(a, 1, nil);
	want(P(180, 220), Blue, "a, buffer 1");
	present(a, 0, nil);
	want(P(180, 220), Red, "a, buffer 0 again");

	/* a buffer outside the window's range is refused */
	if(mkbuf(a, 1, 16, 16, a->base + a->size, 0) == 0)
		fail("a buffer outside the range was taken");
	print("ok a buffer outside the window's range: %r\n");

	/* window b over a's right part: blue; it takes the focus */
	b = newwin();
	if(b->base == a->base)
		fail("a and b share a range");
	ctlrect(b, R(300, 200, 500, 400));
	event(b, 'r');
	readctl(b);
	mkbuf(b, 0, 200, 200, b->base, 1);
	fill(&b->buf[0], Rect(0, 0, 200, 200), Blue);
	present(b, 0, nil);
	want(P(400, 300), Blue, "b over a");
	want(P(340, 150), Green, "a beside b");
	l = event(a, 'f');
	if(strcmp(l, "f 0") != 0)
		fail("a lost the focus: %s", l);
	print("ok a: %s\n", l);

	/* a click on a (where b is not): a comes up and takes the focus, the click is its event */
	click(P(150, 250));
	want(P(350, 250), Green, "a raised by a click");
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

	/* a smaller: it shows what it showed until a buffer of the new size comes */
	ctlrect(a, R(80, 80, 280, 200));
	l = event(a, 'r');
	print("ok a: %s\n", l);
	readctl(a);
	if(Dx(a->r) != 200 || Dy(a->r) != 120)
		fail("a after the resize: %R", a->r);
	want(P(180, 140), Red, "a resized, the old picture");
	want(P(400, 300), Blue, "b where a was");
	mkbuf(a, 0, 200, 120, a->base, 1);
	fill(&a->buf[0], Rect(0, 0, 200, 120), Yellow);
	present(a, 0, nil);
	want(P(180, 140), Yellow, "a's new buffer");

	/* part of a buffer: its right half red */
	fill(&a->buf[0], Rect(100, 0, 200, 120), Red);
	r = Rect(100, 0, 200, 120);
	present(a, 0, &r);
	want(P(230, 140), Red, "a's right half presented");
	want(P(120, 140), Yellow, "a's left half as it was");

	/* b gone with its clone fid; its range is given again */
	close(b->ctl);
	want(P(400, 300), Bg, "background where b was");
	want(P(120, 140), Yellow, "a stays");
	c = newwin();
	if(c->base != b->base)
		fail("a new window got range %#llux, b's %#llux was free", c->base, b->base);
	print("ok window %d: b's range %#llux again\n", c->id, c->base);
	close(c->ctl);

	/* as in rio: button 3 on a's border moves it, button 1 on its corner sweeps it */
	drag(P(180, 78), P(280, 128), 4);
	sleep(200);
	readctl(a);
	if(!eqrect(a->r, R(180, 130, 380, 250)))
		fail("a moved to %R, want %R", a->r, R(180, 130, 380, 250));
	print("ok a moved by the mouse: %R\n", a->r);
	want(P(220, 190), Yellow, "a where it was moved");
	want(P(120, 140), Bg, "background where a was");
	drag(P(381, 251), P(481, 301), 1);
	l = event(a, 'r');
	if(strcmp(l, "r 300 170") != 0)
		fail("a swept: %s", l);
	readctl(a);
	if(!eqrect(a->r, R(180, 130, 480, 300)))
		fail("a swept to %R, want %R", a->r, R(180, 130, 480, 300));
	print("ok a swept by the mouse: %R\n", a->r);
	want(P(220, 190), Yellow, "a swept keeps its picture");
	want(P(450, 280), 0xffffff, "a swept: white beyond it");

	if(leftover())
		fail("segments left in #g");
	print("ok no segment left in #g\n");
	print("wsystest: all passed\n");
	exits(nil);
}
