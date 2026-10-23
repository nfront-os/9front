/*
 * SPDX-License-Identifier: MIT
 *
 * screenpeek: a pixel of the screen (or of the process's rio window,
 * in its coordinates inside rio's border, where the plan9 WSI draws)
 * as 0xRRGGBB, read with unloadimage through a
 * draw(3) connection of its own. Plan 9 C over APE's libdraw, apart
 * from vkdraw.c, whose Vulkan headers do not mix with Plan 9's.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

/*
 * the image the plan9 WSI draws into, as plan9win.c finds it: the rio
 * window by /dev/winname, inside its border; without rio the screen
 */
static Image*
target(Display *d, Point *o)
{
	static Image *w;
	char name[128];
	int fd, n;

	*o = d->image->r.min;
	if((fd = open("/dev/winname", OREAD)) < 0)
		return d->image;
	n = read(fd, name, sizeof name - 1);
	close(fd);
	if(n <= 0)
		return d->image;
	name[n] = 0;
	if(w != nil)
		freeimage(w);
	if((w = namedimage(d, name)) == nil)
		return nil;
	*o = w->r.min;
	if(strncmp(name, "noborder.", 9) != 0)
		*o = addpt(*o, Pt(Borderwidth, Borderwidth));
	return w;
}

int
screenpeek(int x, int y, unsigned *rgb)
{
	static Display *d;
	Image *s;
	Point o, p;
	uchar px[4];

	if(d == nil && (d = initdisplay(nil, nil, nil)) == nil){
		fprint(2, "screenpeek: initdisplay: %r\n");
		return -1;
	}
	if((s = target(d, &o)) == nil){
		fprint(2, "screenpeek: window: %r\n");
		return -1;
	}
	p = addpt(o, Pt(x, y));
	if(s->depth != 32 || !ptinrect(p, s->r)){
		fprint(2, "screenpeek: %d,%d not in %d,%d %d,%d or depth %d\n", p.x, p.y,
			s->r.min.x, s->r.min.y, s->r.max.x, s->r.max.y, s->depth);
		return -1;
	}
	if(unloadimage(s, Rect(p.x, p.y, p.x+1, p.y+1), px, sizeof px) != 4){
		fprint(2, "screenpeek: unloadimage: %r\n");
		return -1;
	}
	*rgb = px[2]<<16 | px[1]<<8 | px[0];
	return 0;
}
