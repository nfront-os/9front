/*
 * SPDX-License-Identifier: MIT
 *
 * screenpeek: a pixel of the screen (or of the process's rio window,
 * in its coordinates inside rio's border, where the plan9 WSI draws)
 * as 0xRRGGBB (from x8r8g8b8 or r5g6b5 screens), read with unloadimage through a
 * draw(3) connection of its own. Plan 9 C over APE's libdraw, apart
 * from vkdraw.c, whose Vulkan headers do not mix with Plan 9's.
 * With $screenpeek set to a file (wsys -g: /dev/screen, the screen as
 * the GPU composes it; the window's pixels are not in devdraw's) the
 * pixel comes from that image(6), uncompressed x8r8g8b8.
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

/* point p of the image(6) in file f (x8r8g8b8, not compressed) */
static int
filepeek(char *f, Point p, unsigned *rgb)
{
	char hdr[61];
	uchar px[4];
	Rectangle r;
	vlong off;
	int fd;

	if((fd = open(f, OREAD)) < 0){
		fprint(2, "screenpeek: %s: %r\n", f);
		return -1;
	}
	if(readn(fd, hdr, 60) != 60){
		fprint(2, "screenpeek: %s: short header\n", f);
		close(fd);
		return -1;
	}
	hdr[60] = 0;
	r = Rect(atoi(hdr+12), atoi(hdr+24), atoi(hdr+36), atoi(hdr+48));
	if(strncmp(hdr, "   x8r8g8b8", 11) != 0 || !ptinrect(p, r)){
		fprint(2, "screenpeek: %s: %.11s %d,%d %d,%d, not %d,%d\n", f, hdr,
			r.min.x, r.min.y, r.max.x, r.max.y, p.x, p.y);
		close(fd);
		return -1;
	}
	off = 60 + ((vlong)(p.y - r.min.y) * Dx(r) + (p.x - r.min.x)) * 4;
	if(pread(fd, px, 4, off) != 4){
		fprint(2, "screenpeek: %s: read: %r\n", f);
		close(fd);
		return -1;
	}
	close(fd);
	*rgb = px[2]<<16 | px[1]<<8 | px[0];
	return 0;
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
	if((s->depth != 32 && s->chan != RGB16) || !ptinrect(p, s->r)){
		fprint(2, "screenpeek: %d,%d not in %d,%d %d,%d or depth %d\n", p.x, p.y,
			s->r.min.x, s->r.min.y, s->r.max.x, s->r.max.y, s->depth);
		return -1;
	}
	if(getenv("screenpeek") != nil)
		return filepeek(getenv("screenpeek"), p, rgb);
	if(unloadimage(s, Rect(p.x, p.y, p.x+1, p.y+1), px, sizeof px) != s->depth/8){
		fprint(2, "screenpeek: unloadimage: %r\n");
		return -1;
	}
	if(s->depth == 16){	/* r5g6b5, widened to 8 bits a channel */
		ulong v, r, g, b;

		v = px[0] | px[1]<<8;
		r = v>>11 & 31;
		g = v>>5 & 63;
		b = v & 31;
		*rgb = (r<<3 | r>>2)<<16 | (g<<2 | g>>4)<<8 | (b<<3 | b>>2);
	}else
		*rgb = px[2]<<16 | px[1]<<8 | px[0];
	return 0;
}
