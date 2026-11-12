/*
 * SPDX-License-Identifier: MIT
 *
 * vga type gpu: the screen is the framebuffer of #G (devgpu.c),
 * drawn into directly (softscreen off); flushes are passed to
 * the user-space driver as dirty rectangles (#G/0/dirty).
 *
 *	echo fb <bytes> >#G/0/ctl	(the driver)
 *	echo type gpu >/dev/vgactl
 *	echo size 1280x800x32 x8r8g8b8 >/dev/vgactl
 *	echo drawinit >/dev/vgactl
 *	echo hwgc gpu >/dev/vgactl	(the cursor: the driver's, #G/0/cursor)
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/error.h"

#define	Image	IMAGE
#include <draw.h>
#include <memdraw.h>
#include <cursor.h>
#include "screen.h"

extern int gpufb(uvlong*, void**, ulong*);
extern void gpudirty(int, int, int, int);
extern void gpucurload(uchar*, uchar*, int, int);
extern void gpucurmove(int, int);
extern void gpucursize(int);
extern void gpucuron(int);

/*
 * for devgpu.c (bar N cached): the firmware's MTRRs make the PCI hole
 * below 4 GB uncacheable, which wins over the page tables; memory the
 * driver says is RAM (virtio-gpu's host visible memory) gets write-back
 */
char*
gpubarcache(uvlong pa, uvlong size)
{
	return mtrr(pa, size, "wb");
}

static void
gpuenable(VGAscr *scr)
{
	uvlong pa;
	void *va;
	ulong size;

	if(gpufb(&pa, &va, &size) < 0)
		error("gpu: no framebuffer, see #G/0/ctl");
	scr->paddr = pa;
	scr->vaddr = va;
	scr->apsize = size;
	scr->softscreen = 0;
}

static void
gpudisable(VGAscr *scr)
{
	scr->paddr = 0;
	scr->vaddr = nil;
	scr->apsize = 0;
}

static void
gpudrawinit(VGAscr *scr)
{
	/* the screen must fit the framebuffer: never draw past it */
	if((uvlong)scr->pitch*scr->height > scr->apsize)
		print("vgagpu: %dx%d does not fit the framebuffer (%d bytes)\n",
			scr->width, scr->height, scr->apsize);
}

static void
gpuflush(VGAscr*, Rectangle r)
{
	gpudirty(r.min.x, r.min.y, r.max.x, r.max.y);
}

VGAdev vgagpudev = {
	"gpu",
	gpuenable,
	gpudisable,
	nil,
	nil,
	gpudrawinit,
	nil,
	nil,
	nil,
	gpuflush,
};

/*
 * hwgc gpu: the cursor goes to the driver (#G/0/cursor), which puts it
 * on the display's cursor planes; devdraw draws none
 */
static void
gpucurenable(VGAscr*)
{
	gpucurload(cursor.clr, cursor.set, cursor.offset.x, cursor.offset.y);
	gpucursize(swcursorsize(0));	/* as big as the soft one (vgactl cursorsize) */
	gpucuron(1);
}

static void
gpucurdisable(VGAscr*)
{
	gpucuron(0);
}

static void
gpucurload1(VGAscr*, Cursor *c)
{
	gpucurload(c->clr, c->set, c->offset.x, c->offset.y);
}

static int
gpucurmove1(VGAscr*, Point p)
{
	gpucurmove(p.x, p.y);
	return 0;
}

static void
gpucursize1(VGAscr*, int n)
{
	gpucursize(n);
}

VGAcur vgagpucur = {
	"gpu",
	gpucurenable,
	gpucurdisable,
	gpucurload1,
	gpucurmove1,
	gpucursize1,
};
