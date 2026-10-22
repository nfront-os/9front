/*
 * SPDX-License-Identifier: MIT
 *
 * screenpeek: a pixel of the screen (or of the process's rio window,
 * in its coordinates) as 0xRRGGBB, read with unloadimage through a
 * draw(3) connection of its own. Plan 9 C over APE's libdraw, apart
 * from vkdraw.c, whose Vulkan headers do not mix with Plan 9's.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

int
screenpeek(int x, int y, unsigned *rgb)
{
	static Display *d;
	Image *s;
	uchar px[4];

	if(d == nil && (d = initdisplay(nil, nil, nil)) == nil)
		return -1;
	s = d->image;
	if(s->depth != 32 || !ptinrect(Pt(x, y), s->r))
		return -1;
	if(unloadimage(s, Rect(x, y, x+1, y+1), px, sizeof px) != 4)
		return -1;
	*rgb = px[2]<<16 | px[1]<<8 | px[0];
	return 0;
}
