/*
 * what libdraw makes of its window system's scale (graphics(2)):
 * display->dpi, the border scalesize gives, screen->r (the content), the
 * font's height ($font, as wsys sets it). For tests/ui/hidpi.rc.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

void
main(int, char**)
{
	if(initdraw(nil, nil, "dpitest") < 0)
		sysfatal("initdraw: %r");
	print("dpi %d border %d screen %d %d %d %d font %d %s\n",
		display->dpi, scalesize(display, Borderwidth),
		screen->r.min.x, screen->r.min.y, screen->r.max.x, screen->r.max.y,
		font->height, font->name);
	exits(nil);
}
