#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

#define	Image	IMAGE
#include	<draw.h>
#include	<memdraw.h>
#include	<cursor.h>
#include	"screen.h"

extern Memimage* gscreen;
extern Cursor cursor;	/* devmouse.c's: the cursor now */

/*
 * Software cursor. 
 */
static Memimage*	swback;		/* screen under cursor */
static Memimage*	swimg;		/* cursor image */
static Memimage*	swmask;		/* cursor mask */
static Memimage*	swimg1;
static Memimage*	swmask1;
static Memimage*	swimgn;		/* the cursor image and mask at swsize */
static Memimage*	swmaskn;
static int		swsize = 16;	/* as big as this on the screen (cursorsize) */

static Point		swoffset;
static Rectangle	swrect;		/* screen rectangle in swback */
static Point		swvispt;	/* actual cursor location */
static int		swvisible;	/* is the cursor visible? */

/*
 * called with drawlock locked for us, most of the time.
 * kernel prints at inopportune times might mean we don't
 * hold the lock, but memimagedraw is now reentrant so
 * that should be okay: worst case we get cursor droppings.
 */
void
swcursorhide(int doflush)
{
	if(swvisible == 0)
		return;
	swvisible = 0;
	if(swback == nil || gscreen == nil)
		return;
	memimagedraw(gscreen, swrect, swback, ZP, memopaque, ZP, S);
	if(doflush){
		flushmemscreen(swrect);
		swrect = ZR;
	}
}

void
swcursoravoid(Rectangle r)
{
	if(swvisible && rectXrect(r, swrect)){
		swcursorhide(0);
		mouseredraw();	/* schedule cursor redraw after we release drawlock */
	}
}

void
swcursordraw(Point p)
{
	Rectangle flushr;

	if(swvisible)
		return;
	if(swback == nil || swimg1 == nil || swmask1 == nil || gscreen == nil)
		return;
	swvispt = addpt(swoffset, p);
	flushr = swrect; 
	swrect = rectaddpt(swimg1->r, swvispt);
	combinerect(&flushr, swrect);
	memimagedraw(swback, swback->r, gscreen, swvispt, memopaque, ZP, S);
	memimagedraw(gscreen, swrect, swimg1, ZP, swmask1, ZP, SoverD);
	flushmemscreen(flushr);
	swvisible = 1;
}

void
swcursorload(Cursor *curs)
{
	uchar *ip, *mp;
	int i, j, set, clr;

	if(swimg == nil || swmask == nil || swimg1 == nil || swmask1 == nil)
		return;
	/*
	 * Build cursor image and mask.
	 * Image is just the usual cursor image
	 * but mask is a transparent alpha mask.
	 * 
	 * The 16x16x8 memimages do not have
	 * padding at the end of their scan lines.
	 */
	ip = byteaddr(swimg, ZP);
	mp = byteaddr(swmask, ZP);
	for(i=0; i<32; i++){
		set = curs->set[i];
		clr = curs->clr[i];
		for(j=0x80; j; j>>=1){
			*ip++ = set&j ? 0x00 : 0xFF;
			*mp++ = (clr|set)&j ? 0xFF : 0x00;
		}
	}
	swoffset = curs->offset;
	if(swsize != 16 && swimgn != nil && swmaskn != nil){
		/*
		 * as big as swsize, each pixel the nearest of the 16×16
		 * (a window system drawing at a scale, wsys' HiDPI)
		 */
		ip = byteaddr(swimgn, ZP);
		mp = byteaddr(swmaskn, ZP);
		for(i=0; i<swsize; i++)
			for(j=0; j<swsize; j++){
				*ip++ = byteaddr(swimg, Pt(j*16/swsize, i*16/swsize))[0];
				*mp++ = byteaddr(swmask, Pt(j*16/swsize, i*16/swsize))[0];
			}
		swoffset = Pt(curs->offset.x*swsize/16, curs->offset.y*swsize/16);
		memimagedraw(swimg1, swimg1->r, swimgn, ZP, memopaque, ZP, S);
		memimagedraw(swmask1, swmask1->r, swmaskn, ZP, memopaque, ZP, S);
		return;
	}
	memimagedraw(swimg1, swimg1->r, swimg, ZP, memopaque, ZP, S);
	memimagedraw(swmask1, swmask1->r, swmask, ZP, memopaque, ZP, S);
}

/*
 * the cursor n pixels big (16 to 64; vgactl's cursorsize): its 16×16
 * scaled, for a window system that draws at a scale; called with
 * drawlock held and the cursor off.  0: only the size now
 */
int
swcursorsize(int n)
{
	if(n == 0)
		return swsize;
	if(n < 16 || n > 64)
		error(Ebadarg);
	swsize = n;
	swcursorinit();
	swcursorload(&cursor);
	return swsize;
}

void
swcursorinit(void)
{
	swvisible = 0;
	if(gscreen == nil)
		return;

	if(swback){
		freememimage(swback);
		freememimage(swmask);
		freememimage(swmask1);
		freememimage(swimg);
		freememimage(swimg1); 
		freememimage(swimgn);
		freememimage(swmaskn);
	}
	swback = allocmemimage(Rect(0,0,swsize,swsize), gscreen->chan);
	swmask = allocmemimage(Rect(0,0,16,16), GREY8);
	swmask1 = allocmemimage(Rect(0,0,swsize,swsize), GREY1);
	swimg = allocmemimage(Rect(0,0,16,16), GREY8);
	swimg1 = allocmemimage(Rect(0,0,swsize,swsize), GREY1);
	swimgn = allocmemimage(Rect(0,0,swsize,swsize), GREY8);
	swmaskn = allocmemimage(Rect(0,0,swsize,swsize), GREY8);
	if(swback == nil || swmask == nil || swmask1 == nil || swimg == nil || swimg1 == nil || swimgn == nil || swmaskn == nil){
		print("software cursor: allocmemimage fails\n");
		return;
	}
	memfillcolor(swback, DTransparent);
	memfillcolor(swmask, DOpaque);
	memfillcolor(swmask1, DOpaque);
	memfillcolor(swimg, DBlack);
	memfillcolor(swimg1, DBlack);
}
