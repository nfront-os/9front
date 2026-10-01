/*
 * rgbmask2chan() (from screen.c) and vesabootscreenconf()/vmode() (from
 * vgavesa.c), extracted standalone. ../port/bootargs.c's multibootargs()
 * calls both unconditionally (whenever the bootloader passed a linear
 * framebuffer / VBE mode-info block), so they're needed regardless of
 * which VGA/draw/mouse drivers are enabled in a given kernel config --
 * same reasoning as pc64/vesabootscreen.c, which extracts the same pair
 * for the same reason. Kept separate from screen.c/vgavesa.c so a
 * minimal (console-only, no draw/vga/mouse) build doesn't have to pull
 * in the whole VGA subsystem just to satisfy these two calls.
 *
 * vmode()/vesabootscreenconf() only parse the VBE mode-info block the
 * bootloader already passed in -- no live VESA BIOS call involved, so
 * this doesn't need vgavesa.c's V86/Ureg386 machinery either.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"io.h"

#define WORD(p) ((p)[0] | ((p)[1]<<8))
#define LONG(p) ((p)[0] | ((p)[1]<<8) | ((p)[2]<<16) | ((p)[3]<<24))

char*
rgbmask2chan(char *buf, int depth, u32int rm, u32int gm, u32int bm)
{
	u32int m[4], dm;	/* r,g,b,x */
	char tmp[32];
	int c, n;

	dm = 1<<depth-1;
	dm |= dm-1;

	m[0] = rm & dm;
	m[1] = gm & dm;
	m[2] = bm & dm;
	m[3] = (~(m[0] | m[1] | m[2])) & dm;

	buf[0] = 0;
Next:
	for(c=0; c<4; c++){
		for(n = 0; m[c] & (1<<n); n++)
			;
		if(n){
			m[0] >>= n, m[1] >>= n, m[2] >>= n, m[3] >>= n;
			snprint(tmp, sizeof tmp, "%c%d%s", "rgbx"[c], n, buf);
			strcpy(buf, tmp);
			goto Next;
		}
	}
	return buf;
}

typedef struct Vmode Vmode;
struct Vmode
{
	char	chan[32];
	int	attr;	/* flags */
	int	bpl;
	int	dx, dy;
	int	depth;
	ulong	paddr;
};

static char*
vmode(Vmode *m, uchar *p)
{
	m->attr = WORD(p);
	if(!(m->attr & (1<<4)))
		return "not in VESA graphics mode";
	if(!(m->attr & (1<<7)))
		return "not in linear graphics mode";
	m->bpl = WORD(p+16);
	m->dx = WORD(p+18);
	m->dy = WORD(p+20);
	m->depth = p[25];
	m->paddr = LONG(p+40);
	if(m->depth <= 8)
		snprint(m->chan, sizeof m->chan, "%c%d",
			(m->attr & (1<<3)) ? 'm' : 'k', m->depth);
	else
		rgbmask2chan(m->chan, m->depth,
			(1UL<<p[31])-1 << p[32],
			(1UL<<p[33])-1 << p[34],
			(1UL<<p[35])-1 << p[36]);
	return nil;
}

/*
 * called from multibootargs() to convert
 * vbe mode info (passed from bootloader)
 * to *bootscreen= parameter
 */
char*
vesabootscreenconf(char *s, char *e, uchar *p)
{
	Vmode m;

	if(vmode(&m, p) != nil)
		return s;
	return seprint(s, e, "*bootscreen=%dx%dx%d %s %#lux\n",
		m.bpl * 8 / m.depth, m.dy, m.depth, m.chan, m.paddr);
}
