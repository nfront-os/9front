/*
 * SPDX-License-Identifier: MIT
 *
 * drawspeed: where a full-screen picture's time goes on its way through
 * draw(3), as the plan9 WSI sends it (loadimage of x8r8g8b8 pixels):
 * into an image in the kernel's memory (devdraw alone), onto the screen
 * (devdraw and the frame buffer), and a draw of the image in memory
 * onto the screen (memdraw's copy to the frame buffer). Through a
 * draw(3) connection of its own: the whole screen, not a rio window.
 * Each 20 times; milliseconds a picture and MB/s. The screen is put
 * back as it was.
 *
 * usage: drawspeed [n]
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

static void
report(char *what, vlong t0, int n, long bytes)
{
	double ms;

	ms = (nsec() - t0) / 1e6 / n;
	print("drawspeed: %s: %.2f ms, %.0f MB/s\n", what, ms, bytes / 1e3 / ms);
}

void
main(int argc, char **argv)
{
	Display *d;
	Image *s, *mem, *save;
	uchar *px;
	Rectangle r;
	long bytes;
	vlong t0;
	int i, n;

	n = argc > 1 ? atoi(argv[1]) : 20;
	if((d = initdisplay(nil, nil, nil)) == nil)
		sysfatal("initdisplay: %r");
	s = d->image;
	r = s->r;
	bytes = Dx(r)*Dy(r)*4;
	print("drawspeed: screen %d,%d %d,%d chan %s depth %d\n", r.min.x, r.min.y, r.max.x, r.max.y,
		chantostr(malloc(32), s->chan), s->depth);
	if((px = malloc(bytes)) == nil)
		sysfatal("malloc: %r");
	for(i = 0; i < bytes; i++)
		px[i] = i*7;
	if((mem = allocimage(d, r, XRGB32, 0, DNofill)) == nil)
		sysfatal("allocimage: %r");
	/* what the screen shows now, put back at the end */
	if((save = allocimage(d, r, s->chan, 0, DNofill)) == nil)
		sysfatal("allocimage: %r");
	draw(save, r, s, nil, r.min);

	t0 = nsec();
	for(i = 0; i < n; i++){
		if(loadimage(mem, r, px, bytes) != bytes)
			sysfatal("loadimage: %r");
		flushimage(d, 0);
	}
	report("loadimage into memory", t0, n, bytes);

	t0 = nsec();
	for(i = 0; i < n; i++){
		if(s->chan == XRGB32 && loadimage(s, r, px, bytes) != bytes)
			sysfatal("loadimage: %r");
		flushimage(d, 1);
	}
	report(s->chan == XRGB32 ? "loadimage onto the screen" : "(screen not x8r8g8b8)", t0, n, bytes);

	t0 = nsec();
	for(i = 0; i < n; i++){
		draw(s, r, mem, nil, r.min);
		flushimage(d, 1);
	}
	report("draw memory -> screen", t0, n, bytes);
	draw(s, r, save, nil, r.min);
	flushimage(d, 1);
	exits(nil);
}
