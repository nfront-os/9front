/*
 * SPDX-License-Identifier: Zlib
 *
 * SDL's plan9 video driver, its libdraw side (APE's: _PLAN9_SOURCE, the
 * rest of the driver has SDL's headers): a connection of its own to
 * draw(3) (initdisplay), the process's window by name (/dev/winname, as
 * a rio client finds its window; the screen without rio), the pixels
 * loaded into it (loadimage) or, where the window's pixels are not
 * x8r8g8b8, into an image of theirs and drawn.  rio and wsys alike.
 */
#define _PLAN9_SOURCE
#include <draw.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "p9draw.h"

static Display *disp;
static Image *win;
static char winname[128];
static int winborder;		/* a rio window's border is not ours */
static Image *buf;

static void
drawerr(Display *d, char *msg)
{
	(void)d;
	fprintf(stderr, "sdl plan9: draw: %s\n", msg);
}

int
p9draw_init(void)
{
	if(disp == nil && (disp = initdisplay(nil, nil, drawerr)) == nil)
		return -1;
	return p9draw_attach();
}

void
p9draw_close(void)
{
	if(buf != nil)
		freeimage(buf);
	if(win != nil)
		freeimage(win);
	buf = win = nil;
	if(disp != nil)
		closedisplay(disp);
	disp = nil;
}

int
p9draw_attach(void)
{
	char name[128];
	int fd, n;

	if(disp == nil)
		return -1;
	winborder = 0;
	if((fd = open("/dev/winname", O_RDONLY)) < 0){
		/* no window system: the screen */
		if(win != nil)
			freeimage(win);
		win = nil;
		winname[0] = 0;
		return 0;
	}
	n = read(fd, name, sizeof name - 1);
	close(fd);
	if(n <= 0)
		return -1;
	name[n] = 0;
	/* rio's windows have a border; devdraw's screen (noborder.screen.N) has none */
	if(strncmp(name, "noborder.", 9) != 0)
		winborder = Borderwidth;
	if(win != nil && strcmp(winname, name) == 0)
		return 0;
	if(win != nil)
		freeimage(win);
	if((win = namedimage(disp, name)) == nil){
		winname[0] = 0;
		return -1;
	}
	snprintf(winname, sizeof winname, "%s", name);
	return 0;
}

static Image*
target(void)
{
	return win != nil ? win : disp->image;
}

void
p9draw_rect(int *x, int *y, int *w, int *h)
{
	Rectangle r;

	r = insetrect(target()->r, winborder);
	*x = r.min.x;
	*y = r.min.y;
	*w = Dx(r);
	*h = Dy(r);
}

void
p9draw_screen(int *w, int *h)
{
	*w = Dx(disp->image->r);
	*h = Dy(disp->image->r);
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
p9draw_put(const void *pixels, int w, int h, int stride)
{
	Image *i;
	Rectangle r;

	if(disp == nil)
		return -1;
	i = target();
	r = insetrect(i->r, winborder);
	if(r.min.x + w < r.max.x)
		r.max.x = r.min.x + w;
	if(r.min.y + h < r.max.y)
		r.max.y = r.min.y + h;
	if(i->chan == XRGB32 || i->chan == ARGB32){
		if(load(i, r, pixels, w, stride) < 0)
			return -1;
	}else{
		if(buf == nil || Dx(buf->r) != w || Dy(buf->r) != h){
			if(buf != nil)
				freeimage(buf);
			if((buf = allocimage(disp, Rect(0, 0, w, h), XRGB32, 0, DNofill)) == nil)
				return -1;
		}
		if(load(buf, buf->r, pixels, w, stride) < 0)
			return -1;
		draw(i, r, buf, nil, ZP);
	}
	return flushimage(disp, 1);
}
