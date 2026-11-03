/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu, aux/i915: the screen of draw(3) on the card, the Plan 9
 * side, shared (each driver's screen.c includes this file), as
 * aux/virtiogpu makes it: #G's framebuffer (gpu(3)) is what devdraw
 * draws into (vga(3) type gpu), and what changes there (#G/n/dirty) is
 * copied into the screen's object (kmsout.c; the driver's linux/kms.c
 * makes it: amdgpu's in VRAM, i915's in the GGTT), which the outputs
 * show; through the driver's BAR for it (amdgpu's 0, i915's aperture
 * 2), write-combining.  The screen spans the outputs
 * that are on (kmsout.c: at the start every connected one, side by
 * side); writing an output's mode changes it, and devdraw's screen
 * changes size with it (vgactl size, as aux/vga -m does: rio follows).
 * #G's framebuffer is made once (the kernel's memory, never freed): for
 * as many monitors side by side as the display has pipes, each as big
 * as the biggest mode of those connected (1920x1080 at least), else
 * smaller (below).  A monitor connected later goes on right of the
 * others (hotplugproc), if the framebuffer is big enough.  The mouse
 * cursor is the display's (vga hwgc gpu: the kernel passes it on in
 * #G/n/cursor, cursorproc puts it on the outputs' cursor planes); soft,
 * drawn by devdraw, if the kernel or the display cannot.
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include <kpi/p9.h>
#include "kmsout.h"

static char *gdir, *drv;
static QLock scrlock;		/* the geometry, the copies */
static uchar *fb;		/* #G's framebuffer: w x h x 4, stride w*4 */
static ulong fbsize;
static uchar *bar;		/* the BAR the processor writes the screen through */
static uchar *vram;		/* the screen's object, through it */
static int w, h;
static uint pitch;

static void	hotplugproc(void*);
static void	cursorproc(void*);

static int
vgactl(char *fmt, ...)
{
	char buf[128];
	va_list arg;
	int fd, r;

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if((fd = open("#v/vgactl", OWRITE)) < 0)
		return -1;
	r = write(fd, buf, strlen(buf));
	close(fd);
	return r < 0 ? -1 : 0;
}

/* the output's biggest mode, and its preferred one (the first line) */
static void
outmodes(char *name, int *bw, int *bh, int *pw, int *ph)
{
	char *m, *s, *l, *e;
	int x, y;

	*bw = *bh = *pw = *ph = 0;
	if((m = kmsout_modes(name)) == nil)
		return;
	for(l = m; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		x = strtol(l, &s, 10);
		if(*s++ != 'x')
			continue;
		y = strtol(s, nil, 10);
		if(*pw == 0){
			*pw = x;
			*ph = y;
		}
		if((vlong)x*y > (vlong)*bw * *bh){
			*bw = x;
			*bh = y;
		}
	}
	kmsout_free(m);
}

/*
 * the framebuffer's sizes to try, biggest first: every pipe a monitor
 * of the biggest mode, the biggest layout of the connected outputs,
 * theirs side by side in the preferred modes (kmsout_screen's), the
 * first one's preferred mode; 0 if none is connected
 */
static int
fbsizes(uvlong *sz)
{
	char *st, *l, *e, *f[4];
	int bw, bh, pw, ph, sw, mh, mw, sh, qw, qh;

	sw = mh = mw = sh = qw = qh = 0;
	sz[3] = 0;
	if((st = kmsout_status()) == nil)
		return 0;
	for(l = st; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		if(tokenize(l, f, nelem(f)) < 3 || strcmp(f[0], "output") != 0 || strcmp(f[2], "connected") != 0)
			continue;
		outmodes(f[1], &bw, &bh, &pw, &ph);
		if(pw == 0)
			continue;
		if(sz[3] == 0)
			sz[3] = (uvlong)pw*ph*4;
		sw += bw;
		mh = bh > mh ? bh : mh;
		sh += bh;
		mw = bw > mw ? bw : mw;
		qw += pw;
		qh = ph > qh ? ph : qh;
	}
	kmsout_free(st);
	if(sz[3] == 0)
		return 0;
	sz[1] = (uvlong)sw*mh*4;
	if((uvlong)mw*sh*4 > sz[1])
		sz[1] = (uvlong)mw*sh*4;
	sz[2] = (uvlong)qw*qh*4;
	bw = mw > 1920 ? mw : 1920;
	bh = mh > 1080 ? mh : 1080;
	sz[0] = (uvlong)kmsout_npipes()*bw*bh*4;
	if(sz[0] < sz[1])
		sz[0] = sz[1];
	return 4;
}

/* #G's framebuffer, size bytes at least: its segment, attached */
static uchar*
fbattach(ulong size)
{
	char path[64], buf[1024], *l, *e, *f[4];
	uchar *p;
	int fd, n;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((fd = open(path, ORDWR)) < 0)
		return nil;
	if(fprint(fd, "fb %lud", size) < 0){
		close(fd);
		return nil;
	}
	n = pread(fd, buf, sizeof buf - 1, 0);
	close(fd);
	if(n <= 0)
		return nil;
	buf[n] = 0;
	for(l = buf; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		/* "fb pa size seg" */
		if(tokenize(l, f, nelem(f)) == 4 && strcmp(f[0], "fb") == 0){
			p = segattach(0, f[3], nil, strtoul(f[2], nil, 0));
			if(p == (void*)-1)
				return nil;
			fbsize = strtoul(f[2], nil, 0);
			return p;
		}
	}
	werrstr("no framebuffer in %s", path);
	return nil;
}

/* what devdraw changed, into the screen's object; scrlock held */
static void
copyrect(int x0, int y0, int x1, int y1)
{
	int y, n;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 > w) x1 = w;
	if(y1 > h) y1 = h;
	if(x0 >= x1 || y0 >= y1)
		return;
	n = (x1 - x0) * 4;
	for(y = y0; y < y1; y++)
		memmove(vram + (uvlong)y*pitch + x0*4, fb + (uvlong)y*w*4 + x0*4, n);
}

static void
dirtyproc(void*)
{
	char path[64], buf[64], *f[4];
	int fd, n;

	snprint(path, sizeof path, "%s/dirty", gdir);
	if((fd = open(path, OREAD)) < 0){
		fprint(2, "%s: screen: %s: %r\n", drv, path);
		return;
	}
	for(;;){
		n = read(fd, buf, sizeof buf - 1);
		if(n <= 0){
			fprint(2, "%s: screen: %s: %r\n", drv, path);
			return;
		}
		buf[n] = 0;
		if(tokenize(buf, f, nelem(f)) == 4){
			qlock(&scrlock);
			copyrect(atoi(f[0]), atoi(f[1]), atoi(f[2]), atoi(f[3]));
			qunlock(&scrlock);
		}
	}
}

/* devdraw's screen w x h */
static int
setsize(void)
{
	if(vgactl("size %dx%dx32 x8r8g8b8", w, h) < 0 || vgactl("drawinit") < 0){
		werrstr("vgactl: %r");
		return -1;
	}
	return 0;
}

/*
 * the screen on the card, written through BAR barno; 0, or -1 (and the
 * screen stays where it was)
 */
int
screenstart(char *dir, int barno, char *name)
{
	unsigned long long off;
	uvlong sz[4];
	uint uw, uh;
	int r, i, n;

	gdir = dir;
	drv = name;
	if((n = fbsizes(sz)) == 0){
		werrstr("no connected output");
		return -1;
	}
	for(i = 0; i < n && fb == nil; i++)
		if(sz[i] > 0 && sz[i] < 1ULL<<31)
			fb = fbattach(sz[i]);
	if(fb == nil)
		return -1;
	if((bar = p9_barwc(barno)) == nil){
		werrstr("BAR %d write-combining: %r", barno);
		return -1;
	}
	if((r = kmsout_screen(fbsize, &uw, &uh, &pitch, &off)) < 0){
		bar = nil;
		werrstr("screen object: error %d", -r);
		return -1;
	}
	w = uw;
	h = uh;
	vram = bar + off;
	if(vgactl("type gpu") < 0 || setsize() < 0)
		return -1;
	copyrect(0, 0, w, h);
	proccreate(dirtyproc, nil, 16384);
	if(vgactl("hwgc gpu") < 0)
		vgactl("hwgc soft");
	else
		proccreate(cursorproc, nil, 1024*1024);	/* amdgpu: DC's cursor update */
	proccreate(hotplugproc, nil, 1024*1024);	/* amdgpu: DC's modeset */
	fprint(2, "%s: screen %dx%d (framebuffer %lud bytes)\n", drv, w, h, fbsize);
	return 0;
}

/*
 * an output's mode written (outputs/conn/mode): the layout, then
 * devdraw's screen of its size; nil or the error.  On a proc with a big
 * stack (amdgpu: DC's modeset).
 */
/* the screen's object as kmsout made it, devdraw's screen of its size; scrlock held */
static void
newscreen(uint uw, uint uh, uint up, unsigned long long off)
{
	vram = bar + off;
	pitch = up;
	if(uw != w || uh != h){
		w = uw;
		h = uh;
		if(setsize() < 0)
			fprint(2, "%s: screen %dx%d: %r\n", drv, w, h);
		else
			fprint(2, "%s: screen %dx%d\n", drv, w, h);
	}
	copyrect(0, 0, w, h);
}

char*
screenset(char *name, char *spec)
{
	unsigned long long off;
	const char *e;
	uint uw, uh, up;

	if(bar == nil)
		return "no screen of the driver's (-S)";
	qlock(&scrlock);
	e = kmsout_set(name, spec, fbsize, &uw, &uh, &up, &off);
	newscreen(uw, uh, up, off);
	qunlock(&scrlock);
	return (char*)e;
}

/*
 * a monitor connected or gone (DRM's hotplug event): the layout after
 * it; while a client's picture is on the display, again each second
 */
static void
hotplugproc(void*)
{
	unsigned long long off;
	const char *e;
	uint seen, uw, uh, up;
	int changed;

	seen = 0;
	for(;;){
		if(kmsout_hotplug_wait(&seen) < 0)
			continue;
		for(;;){
			qlock(&scrlock);
			e = kmsout_relayout(fbsize, &changed, &uw, &uh, &up, &off);
			if(changed)
				newscreen(uw, uh, up, off);
			qunlock(&scrlock);
			if(e == nil || strstr((char*)e, "picture") == nil)
				break;
			sleep(1000);
		}
		if(e != nil)
			fprint(2, "%s: hotplug: %s\n", drv, e);
	}
}

/*
 * the requests of the outputs' files: a mode set, the backlight set or
 * read (spec nil), on a proc of their own with a big stack (amdgpu: DC's
 * modeset needs more than lib9p's procs have)
 */
typedef struct Oreq Oreq;
struct Oreq {
	char	*out, *spec;
	int	bl;
	char	*e, *s;
	Channel	*done;
};
static Channel *outc;
static QLock outlock;

static void
outproc(void*)
{
	Oreq *q;

	for(;;){
		q = recvp(outc);
		if(!q->bl)
			q->e = screenset(q->out, q->spec);
		else if(q->spec != nil)
			q->e = (char*)kmsout_setbacklight(q->out, q->spec);
		else
			q->s = kmsout_backlight(q->out);
		sendul(q->done, 1);
	}
}

/* nil or the error; a backlight read: *s, kmsout_free it */
char*
outrequest(char *out, char *spec, int bl, char **s)
{
	Oreq q;

	qlock(&outlock);
	if(outc == nil){
		outc = chancreate(sizeof(void*), 0);
		proccreate(outproc, nil, 1024*1024);
	}
	qunlock(&outlock);
	memset(&q, 0, sizeof q);
	q.out = out;
	q.spec = spec;
	q.bl = bl;
	q.done = chancreate(sizeof(ulong), 0);
	sendp(outc, &q);
	recvul(q.done);
	chanfree(q.done);
	if(s != nil)
		*s = q.s;
	return q.e;
}

static int
hexbytes(char *s, uchar *b, int n)
{
	int i, x;
	char t[3];

	for(i = 0; i < n; i++){
		if(s[0] == 0 || s[1] == 0)
			return -1;
		t[0] = s[0];
		t[1] = s[1];
		t[2] = 0;
		x = strtol(t, nil, 16);
		b[i] = x;
		s += 2;
	}
	return 0;
}

/*
 * the cursor from the kernel (#G/n/cursor: x y on gen ox oy clrset) onto
 * the display; if the display cannot show it, devdraw draws it again
 */
static void
cursorproc(void*)
{
	char path[64], buf[256], *f[8];
	uchar clr[32], set[32];
	int fd, n, gen, lastgen, r;

	snprint(path, sizeof path, "%s/cursor", gdir);
	if((fd = open(path, OREAD)) < 0){
		fprint(2, "%s: cursor: %s: %r\n", drv, path);
		vgactl("hwgc soft");
		return;
	}
	lastgen = -1;
	for(;;){
		n = read(fd, buf, sizeof buf - 1);
		if(n <= 0){
			fprint(2, "%s: cursor: %s: %r\n", drv, path);
			break;
		}
		buf[n] = 0;
		if(tokenize(buf, f, nelem(f)) != 7)
			continue;
		gen = atoi(f[3]);
		if(gen != lastgen){
			if(hexbytes(f[6], clr, 32) < 0 || hexbytes(f[6]+64, set, 32) < 0)
				continue;
			if((r = kmsout_cursor_image(clr, set, atoi(f[4]), atoi(f[5]))) < 0){
				fprint(2, "%s: cursor: the display's: error %d: soft\n", drv, -r);
				break;
			}
			lastgen = gen;
		}
		kmsout_cursor_move(atoi(f[0]), atoi(f[1]), atoi(f[2]));
	}
	close(fd);
	kmsout_cursor_move(0, 0, 0);
	vgactl("hwgc soft");
}
