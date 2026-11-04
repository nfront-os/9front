/*
 * SMBIOS (DMI): the firmware's description of the machine (vendor,
 * model, BIOS version, board, chassis) as #P/smbios: the entry point
 * (_SM3_ or _SM_), then the structure table, copied at boot.  The
 * entry point is where the EFI loader saw it (*smbios=, from the EFI
 * configuration table) or found in the BIOS area.  Only eve reads it
 * (it has serial numbers), as Linux's raw DMI tables are root's.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

enum {
	Maxtable	= 1*MB,	/* a sanity limit: tables are a few KB */
};

static uchar *smbios;	/* entry point, then the table */
static long nsmbios;

static uint
get16(uchar *p)
{
	return p[1]<<8 | p[0];
}

static ulong
get32(uchar *p)
{
	return (ulong)p[3]<<24 | p[2]<<16 | p[1]<<8 | p[0];
}

static uvlong
get64(uchar *p)
{
	return (uvlong)get32(p+4)<<32 | get32(p);
}

static int
sum(uchar *p, int n)
{
	int s;

	s = 0;
	while(n-- > 0)
		s += *p++;
	return s & 0xFF;
}

/* the entry point at pa or va (one of them): its length, the table's place and size; 0 if none */
static int
entry(uchar *e, uvlong *tpa, ulong *tlen)
{
	int n;

	if(memcmp(e, "_SM3_", 5) == 0){
		n = e[6];
		if(n < 0x18 || sum(e, n) != 0)
			return 0;
		*tlen = get32(e+0x0C);	/* the most it can be */
		*tpa = get64(e+0x10);
		return n;
	}
	if(memcmp(e, "_SM_", 4) == 0){
		n = e[5];
		if(n < 0x1F || sum(e, n) != 0)
			return 0;
		if(memcmp(e+0x10, "_DMI_", 5) != 0 || sum(e+0x10, 15) != 0)
			return 0;
		*tlen = get16(e+0x16);
		*tpa = get32(e+0x18);
		return n;
	}
	return 0;
}

static long
readsmbios(Chan*, void *v, long n, vlong o)
{
	if(o < 0 || o >= nsmbios)
		return 0;
	if(n > nsmbios - o)
		n = nsmbios - o;
	memmove(v, smbios + o, n);
	return n;
}

void
smbioslink(void)
{
	uchar *e, *t;
	char *cp;
	uvlong pa, tpa;
	ulong tlen;
	int n;

	e = nil;
	pa = 0;
	if((cp = getconf("*smbios")) != nil && (pa = strtoull(cp, nil, 0)) != 0)
		e = vmap(pa, 32);
	if(e == nil && (e = sigsearch("_SM3_", 0x18)) == nil)
		e = sigsearch("_SM_", 0x1F);
	if(e == nil)
		return;
	n = entry(e, &tpa, &tlen);
	if(n > 0 && tlen > 0 && tlen <= Maxtable && (smbios = malloc(n + tlen)) != nil){
		memmove(smbios, e, n);
		if((t = vmap(tpa, tlen)) != nil){
			memmove(smbios + n, t, tlen);
			vunmap(t, tlen);
			nsmbios = n + tlen;
		}else{
			free(smbios);
			smbios = nil;
		}
	}
	if(pa != 0)
		vunmap(e, 32);
	if(smbios == nil){
		print("smbios: no usable table\n");
		return;
	}
	addarchfile("smbios", 0400, readsmbios, nil);
}
