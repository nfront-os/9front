/*
 * SPDX-License-Identifier: MIT
 *
 * The draw(3) side of Mesa's plan9 WSI (include/plan9win.h): its own
 * connection to /dev/draw (initdisplay), the window by name (namedimage,
 * as a rio client finds its window), the pixels loaded into the window
 * (loadimage, flushimage), or through an image of their size and draw
 * where the window's pixels are not 32-bit x8r8g8b8.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

#include "plan9win.h"

static Lock lk;
static Display *disp;
static char curname[128];	/* the window image we hold */
static Image *win;
static int winborder;		/* a rio window's border is not ours */
static Image *buf;

static void
drawerr(Display *d, char *msg)
{
	(void)d;
	fprint(2, "plan9win: draw: %s\n", msg);
}

/* the window by name, again if the name changed (rio renames on resize) */
static Image*
window(const char *name)
{
	char wname[128];
	int fd, n;

	if(disp == nil){
		disp = initdisplay(nil, nil, drawerr);
		if(disp == nil)
			return nil;
	}
	winborder = 0;
	if(name == nil){
		if((fd = open("/dev/winname", OREAD)) < 0){
			win = nil;
			curname[0] = 0;
			return disp->image;	/* no rio: the screen */
		}
		n = read(fd, wname, sizeof wname - 1);
		close(fd);
		if(n <= 0)
			return disp->image;
		wname[n] = 0;
		name = wname;
		/* rio's windows have a border; devdraw's screen (no rio) is noborder.screen.N */
		if(strncmp(name, "noborder.", 9) != 0)
			winborder = Borderwidth;
	}
	if(win != nil && strcmp(curname, name) == 0)
		return win;
	if(win != nil)
		freeimage(win);
	win = namedimage(disp, (char*)name);
	if(win == nil){
		curname[0] = 0;
		return nil;
	}
	snprint(curname, sizeof curname, "%s", name);
	return win;
}

static Rectangle
area(Image *i)
{
	return insetrect(i->r, winborder);
}

int
plan9win_size(const char *name, int *w, int *h)
{
	Image *i;
	Rectangle r;

	lock(&lk);
	if((i = window(name)) == nil){
		unlock(&lk);
		return -1;
	}
	r = area(i);
	*w = Dx(r);
	*h = Dy(r);
	unlock(&lk);
	return 0;
}

int
plan9win_isscreen(const char *name)
{
	Image *i;
	int r;

	lock(&lk);
	r = 0;
	if((i = window(name)) != nil)
		r = i == disp->image || strncmp(curname, "noborder.screen.", 16) == 0;
	unlock(&lk);
	return r;
}

/* the pixels at the top left of r: whole if they fit as they are, else by rows */
static int
load(Image *i, Rectangle r, const void *pixels, int w, int stride)
{
	const uchar *p;
	int y;

	p = pixels;
	if(stride == w*4 && Dx(r) == w)
		return loadimage(i, r, (uchar*)p, stride*Dy(r));
	for(y = r.min.y; y < r.max.y; y++, p += stride)
		if(loadimage(i, Rect(r.min.x, y, r.max.x, y+1), (uchar*)p, Dx(r)*4) < 0)
			return -1;
	return 0;
}

int
plan9win_put(const char *name, const void *pixels, int w, int h, int stride)
{
	Image *i;
	Rectangle r;

	lock(&lk);
	if((i = window(name)) == nil)
		goto Err;
	r = area(i);
	r.max.x = r.min.x + w < r.max.x ? r.min.x + w : r.max.x;
	r.max.y = r.min.y + h < r.max.y ? r.min.y + h : r.max.y;
	if(i->chan == XRGB32 || i->chan == ARGB32){
		/* straight into the window: no copy of our own */
		if(load(i, r, pixels, w, stride) < 0)
			goto Err;
	}else{
		if(buf == nil || Dx(buf->r) != w || Dy(buf->r) != h){
			if(buf != nil)
				freeimage(buf);
			buf = allocimage(disp, Rect(0, 0, w, h), XRGB32, 0, DNofill);
			if(buf == nil)
				goto Err;
		}
		if(load(buf, buf->r, pixels, w, stride) < 0)
			goto Err;
		draw(i, r, buf, nil, ZP);
	}
	if(flushimage(disp, 1) < 0)
		goto Err;
	unlock(&lk);
	return 0;
Err:
	unlock(&lk);
	return -1;
}

/*
 * wsys(4) gives each window a ctl file (/dev/ctl in the window's name
 * space); a read of it is the window's number, rectangle, size, the
 * range for its buffers and its focus, ten numbers.  Only the process's
 * own window: a surface named after another is not ours to fill.
 */
int
plan9win_wsys(const char *name)
{
	char b[256], *f[12];
	int fd, n;

	if(name != nil){
		if((fd = open("/dev/winname", OREAD)) < 0)
			return -1;
		n = read(fd, b, sizeof b - 1);
		close(fd);
		if(n <= 0)
			return -1;
		b[n] = 0;
		if(strcmp(b, name) != 0)
			return -1;
	}
	if((fd = open("/dev/ctl", ORDWR|OCEXEC)) < 0)
		return -1;
	n = pread(fd, b, sizeof b - 1, 0);
	if(n <= 0){
		close(fd);
		return -1;
	}
	b[n] = 0;
	if(tokenize(b, f, nelem(f)) != 10){
		close(fd);
		return -1;
	}
	return fd;
}

/* the window's events in wsys (wsys(4)'s event): a descriptor, -1 */
int
plan9win_wsysevents(void)
{
	return open("/dev/event", OREAD|OCEXEC);
}

int
plan9win_wsysctl(int fd, const char *msg)
{
	int n;

	n = strlen(msg);
	if(write(fd, msg, n) != n)
		return -1;
	return 0;
}
