/*
 * SPDX-License-Identifier: MIT
 *
 * screenshot: the screen, every f-th pixel of every f-th row, as text on
 * standard output ("shot w h", then "s " and a row's rrggbb in hex per
 * line), read with unloadimage from a draw(3) connection of its own,
 * as screenpx.  For a display that cannot be dumped from outside (a
 * passed-through GPU); the host makes a picture of it.
 *
 * usage: screenshot [-f factor]
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

void
main(int argc, char **argv)
{
	Display *d;
	Image *s;
	uchar *row;
	char *line, *p;
	int f, x, y, w, h, n;

	f = 4;
	ARGBEGIN{
	case 'f':
		f = atoi(EARGF(sysfatal("usage: screenshot [-f factor]")));
		break;
	}ARGEND
	if(f < 1)
		f = 1;
	if((d = initdisplay(nil, nil, nil)) == nil)
		sysfatal("initdisplay: %r");
	s = d->image;
	if(s->depth != 32)
		sysfatal("screen is %d bits", s->depth);
	n = Dx(s->r);
	w = n / f;
	h = Dy(s->r) / f;
	row = malloc(n * 4);
	line = malloc(w * 6 + 4);
	if(row == nil || line == nil)
		sysfatal("malloc: %r");
	print("shot %d %d\n", w, h);
	for(y = 0; y < h; y++){
		if(unloadimage(s, Rect(s->r.min.x, s->r.min.y + y*f, s->r.max.x, s->r.min.y + y*f + 1), row, n * 4) != n * 4)
			sysfatal("unloadimage: %r");
		p = line;
		for(x = 0; x < w; x++)
			p += sprint(p, "%02x%02x%02x", row[x*f*4+2], row[x*f*4+1], row[x*f*4]);
		print("s %s\n", line);
	}
	exits(nil);
}
