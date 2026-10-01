/*
 * vesabootscreenconf() and its helper vmode(), extracted from
 * ../pc/vgavesa.c. Only these two functions are needed for pc64: they
 * just parse the VBE mode-info block the bootloader already passed in
 * (no live VESA BIOS call involved). The rest of vgavesa.c (vbecall()
 * and friends) drives the VESA BIOS via real-mode/V86 Ureg calls through
 * a Ureg386 (with a 32-bit real-mode "trap" field) that doesn't exist --
 * and wouldn't make sense -- for amd64 long mode, and isn't reachable
 * here anyway: this kernel's Dev config lists vgax/vgasoft, not vgavesa,
 * as the real VGA driver.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"io.h"

extern char* rgbmask2chan(char *buf, int depth, u32int rm, u32int gm, u32int bm);

#define WORD(p) ((p)[0] | ((p)[1]<<8))
#define LONG(p) ((p)[0] | ((p)[1]<<8) | ((p)[2]<<16) | ((p)[3]<<24))

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
