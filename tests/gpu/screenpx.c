/*
 * SPDX-License-Identifier: MIT
 *
 * screenpx: the colours of the screen at the points given, read with
 * unloadimage from the screen image of a draw(3) connection of its
 * own (no rio needed; rio's /dev/screen is not there without it):
 * "x,y rrggbb" a line. For tests in a guest whose display cannot be
 * dumped from outside (QEMU with a GL display).
 *
 * usage: screenpx x,y ...
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

void
main(int argc, char **argv)
{
	Display *d;
	Image *s;
	uchar px[16];
	char *p, buf[16];
	int i, x, y;

	if(argc < 2){
		fprint(2, "usage: screenpx x,y ...\n");
		exits("usage");
	}
	if((d = initdisplay(nil, nil, nil)) == nil)
		sysfatal("initdisplay: %r");
	s = d->image;
	if(s->depth != 32)
		sysfatal("screen is %d bits", s->depth);
	print("screen %dx%d %s\n", Dx(s->r), Dy(s->r), chantostr(buf, s->chan));
	for(i = 1; i < argc; i++){
		x = strtol(argv[i], &p, 10);
		if(*p != ',')
			sysfatal("bad point %s", argv[i]);
		y = strtol(p+1, nil, 10);
		if(!ptinrect(Pt(x, y), s->r)){
			print("%d,%d outside\n", x, y);
			continue;
		}
		if(unloadimage(s, Rect(x, y, x+1, y+1), px, sizeof px) != 4)
			sysfatal("unloadimage: %r");
		print("%d,%d %02x%02x%02x\n", x, y, px[2], px[1], px[0]);
	}
	exits(nil);
}
