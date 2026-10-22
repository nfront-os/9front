/*
 * SPDX-License-Identifier: MIT
 *
 * The draw(3) side of Mesa's plan9 WSI (include/plan9win.h): its own
 * connection to /dev/draw (initdisplay), the window by name (namedimage,
 * as a rio client finds its window), the pixels through one image of
 * the window's size (loadimage, draw, flushimage).
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
plan9win_put(const char *name, const void *pixels, int w, int h, int stride)
{
	Image *i;
	Rectangle r;
	const uchar *p;
	int y;

	lock(&lk);
	if((i = window(name)) == nil)
		goto Err;
	if(buf == nil || Dx(buf->r) != w || Dy(buf->r) != h){
		if(buf != nil)
			freeimage(buf);
		buf = allocimage(disp, Rect(0, 0, w, h), XRGB32, 0, DNofill);
		if(buf == nil)
			goto Err;
	}
	p = pixels;
	if(stride == w*4){
		if(loadimage(buf, buf->r, (uchar*)p, stride*h) < 0)
			goto Err;
	} else
		for(y = 0; y < h; y++, p += stride)
			if(loadimage(buf, Rect(0, y, w, y+1), (uchar*)p, w*4) < 0)
				goto Err;
	r = area(i);
	r.max.x = r.min.x + w < r.max.x ? r.min.x + w : r.max.x;
	r.max.y = r.min.y + h < r.max.y ? r.min.y + h : r.max.y;
	draw(i, r, buf, nil, ZP);
	if(flushimage(disp, 1) < 0)
		goto Err;
	unlock(&lk);
	return 0;
Err:
	unlock(&lk);
	return -1;
}
