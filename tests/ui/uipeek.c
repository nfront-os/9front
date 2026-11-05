/*
 * SPDX-License-Identifier: MIT
 *
 * uipeek x,y...: the screen's pixels as x,y rrggbb, read through draw(3)
 * uipeek -n x0,y0,x1,y1 rrggbb: how many pixels of the rectangle are that colour
 * uipeek -k text: the runes of text typed, as kbd(3) c messages to /dev/kbdin
 * (each with its null byte, which echo cannot write)
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

void
main(int argc, char **argv)
{
	Display *d;
	Image *s;
	uchar px[4];
	ulong v;
	int i, x, y;
	char *p;

	if(argc == 3 && strcmp(argv[1], "-k") == 0){
		Rune r;
		char m[UTFmax+2];
		int fd, n;

		if((fd = open("/dev/kbdin", OWRITE)) < 0)
			sysfatal("/dev/kbdin: %r");
		for(p = argv[2]; *p; p += n){
			n = chartorune(&r, p);
			m[0] = 'c';
			memmove(m+1, p, n);
			m[1+n] = 0;
			if(write(fd, m, n+2) != n+2)
				sysfatal("write kbdin: %r");
		}
		exits(nil);
	}
	if((d = initdisplay(nil, nil, nil)) == nil)
		sysfatal("initdisplay: %r");
	s = d->image;
	if(argc == 4 && strcmp(argv[1], "-n") == 0){
		Rectangle r;
		uchar *buf;
		ulong want, n, bpp;
		int len;

		r.min.x = strtol(argv[2], &p, 10);
		r.min.y = strtol(p+1, &p, 10);
		r.max.x = strtol(p+1, &p, 10);
		r.max.y = strtol(p+1, &p, 10);
		want = strtoul(argv[3], nil, 16);
		bpp = s->depth/8;
		len = Dx(r)*Dy(r)*bpp;
		if((buf = malloc(len)) == nil || unloadimage(s, r, buf, len) != len)
			sysfatal("unloadimage: %r");
		n = 0;
		for(i = 0; i < Dx(r)*Dy(r); i++){
			if(bpp == 2){
				v = buf[2*i] | buf[2*i+1]<<8;
				v = ((v>>11&31)<<3)<<16 | ((v>>5&63)<<2)<<8 | (v&31)<<3;
			}else
				v = buf[bpp*i+2]<<16 | buf[bpp*i+1]<<8 | buf[bpp*i];
			if(v == want)
				n++;
		}
		print("%lud\n", n);
		exits(nil);
	}
	for(i = 1; i < argc; i++){
		x = strtol(argv[i], &p, 10);
		y = strtol(p+1, nil, 10);
		if(unloadimage(s, Rect(x, y, x+1, y+1), px, sizeof px) != s->depth/8)
			sysfatal("unloadimage: %r");
		if(s->depth == 16){
			v = px[0] | px[1]<<8;
			v = ((v>>11&31)<<3)<<16 | ((v>>5&63)<<2)<<8 | (v&31)<<3;
		}else
			v = px[2]<<16 | px[1]<<8 | px[0];
		print("%d,%d %06lux\n", x, y, v);
	}
	exits(nil);
}
