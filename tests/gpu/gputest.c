/*
 * SPDX-License-Identifier: MIT
 *
 * gputest: draw three known rectangles on the whole screen
 * (no rio); tests/gpu/run.sh checks them in QEMU's screen dump.
 * -l: keep running and redraw them every 20 ms, because the
 * kernel's console keeps writing (and scrolling) on the screen
 * and the window of initdraw is painted white when we exit.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>

static void
box(ulong col, int x, int y)
{
	Image *i;

	i = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, col);
	if(i == nil)
		sysfatal("allocimage: %r");
	draw(screen, rectaddpt(Rect(x, y, x+200, y+200), screen->r.min), i, nil, ZP);
	freeimage(i);
}

static void
boxes(void)
{
	box(DRed, 100, 350);
	box(DGreen, 400, 350);
	box(DBlue, 700, 350);
	draw(screen, rectaddpt(Rect(650, 150, 750, 250), screen->r.min), display->black, nil, ZP);
	flushimage(display, 1);
}

void
main(int argc, char **argv)
{
	char buf[16];
	int loop;

	loop = 0;
	ARGBEGIN{
	case 'l':
		loop = 1;
		break;
	default:
		fprint(2, "usage: gputest [-l]\n");
		exits("usage");
	}ARGEND

	if(initdraw(nil, nil, "gputest") < 0)
		sysfatal("initdraw: %r");
	draw(screen, screen->r, display->black, nil, ZP);
	boxes();
	print("gputest: %R chan %s\n", screen->r, chantostr(buf, screen->chan));
	while(loop){
		sleep(20);
		boxes();
	}
	exits(nil);
}
