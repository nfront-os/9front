/*
 * SPDX-License-Identifier: MIT
 *
 * drawtest: an ordinary draw(3) program (event(2)) for wsys' rio(4) files:
 * red in its window, blue after a resize; logs to the file given: its window
 * id, its rectangle, each click and key, each resize. q quits.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

static int logfd;

static void
paint(ulong col)
{
	Image *c;

	c = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, col);
	draw(screen, screen->r, c, nil, ZP);
	freeimage(c);
	flushimage(display, 1);
}

void
eresized(int new)
{
	if(new && getwindow(display, Refnone) < 0)
		sysfatal("getwindow: %r");
	fprint(logfd, "resized %R\n", screen->r);
	paint(DBlue);
}

void
main(int argc, char **argv)
{
	Event e;
	char id[32];
	int fd, n;

	if(argc != 2)
		sysfatal("usage: drawtest logfd");
	if((logfd = create(argv[1], OWRITE, 0666)) < 0)
		sysfatal("%s: %r", argv[1]);
	if(initdraw(nil, nil, "drawtest") < 0)
		sysfatal("initdraw: %r");
	id[0] = 0;
	if((fd = open("/dev/winid", OREAD)) >= 0){
		n = read(fd, id, sizeof id - 1);
		id[n > 0 ? n : 0] = 0;
		close(fd);
	}
	einit(Emouse|Ekeyboard);
	paint(DRed);
	fprint(logfd, "start %d %R\n", atoi(id), screen->r);
	for(;;)
		switch(event(&e)){
		case Emouse:
			if(e.mouse.buttons)
				fprint(logfd, "mouse %d %d %d\n", e.mouse.xy.x, e.mouse.xy.y, e.mouse.buttons);
			break;
		case Ekeyboard:
			fprint(logfd, "key %C\n", e.kbdc);
			if(e.kbdc == 'q')
				exits(nil);
			break;
		}
}
