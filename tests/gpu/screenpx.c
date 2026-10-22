/*
 * SPDX-License-Identifier: MIT
 *
 * screenpx: the colours of the screen at the points given, from
 * /dev/screen (draw(3): the screen image, uncompressed; 32-bit
 * channels only): "x,y rrggbb" a line. For tests in a guest whose
 * display cannot be dumped from outside (QEMU with a GL display).
 *
 * usage: screenpx x,y ...
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

void
main(int argc, char **argv)
{
	char hdr[5*12+1], *f[5], *p;
	uchar *data, *px;
	int fd, i, n, x, y, minx, miny, maxx, maxy, bpl;
	long size, got;

	if(argc < 2){
		fprint(2, "usage: screenpx x,y ...\n");
		exits("usage");
	}
	if((fd = open("/dev/screen", OREAD)) < 0)
		sysfatal("open /dev/screen: %r");
	if(readn(fd, hdr, 60) != 60)
		sysfatal("read header: %r");
	hdr[60] = 0;
	if(tokenize(hdr, f, 5) != 5)
		sysfatal("bad header");
	if(chantodepth(strtochan(f[0])) != 32)
		sysfatal("channel %s: not 32 bits", f[0]);
	minx = atoi(f[1]);
	miny = atoi(f[2]);
	maxx = atoi(f[3]);
	maxy = atoi(f[4]);
	bpl = (maxx-minx)*4;
	size = (long)bpl*(maxy-miny);
	data = malloc(size);
	if(data == nil)
		sysfatal("malloc: %r");
	for(got = 0; got < size; got += n)
		if((n = read(fd, data+got, size-got)) <= 0)
			sysfatal("read data: %r");
	print("screen %dx%d %s\n", maxx-minx, maxy-miny, f[0]);
	for(i = 1; i < argc; i++){
		x = strtol(argv[i], &p, 10);
		if(*p != ',')
			sysfatal("bad point %s", argv[i]);
		y = strtol(p+1, nil, 10);
		if(x < minx || x >= maxx || y < miny || y >= maxy){
			print("%d,%d outside\n", x, y);
			continue;
		}
		px = data + (long)(y-miny)*bpl + (x-minx)*4;
		print("%d,%d %02x%02x%02x\n", x, y, px[2], px[1], px[0]);
	}
	exits(nil);
}
