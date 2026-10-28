/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: EDID: checksum, read, detailed timings.
 *
 * Follows Linux 6.6's drivers/gpu/drm/drm_edid.c (much of it first
 * written after v4.4); the notices are in NOTICE, UPSTREAM says which
 * functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

int
drm_edid_block_valid(uchar *e)
{
	static uchar hdr[8] = { 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0 };
	uchar s;
	int i;

	if(memcmp(e, hdr, 8) != 0)
		return 0;
	for(s = 0, i = 0; i < 128; i++)
		s += e[i];
	return s == 0;
}

/* the EDID, base block and one extension at most */
void
readedid(Output *o)
{
	static int gmpin[] = { 0, 5, 4, 6 };	/* GMBUS_PIN_DPB, DPC, DPD */
	int (*get)(int, int, uchar*, int);
	int a;

	o->nedid = 0;
	if(o->type == Ohdmi){
		get = gmedid;
		a = gmpin[o->port];
	}else{
		get = auxedid;
		a = o->port;
	}
	if(get(a, 0, o->edid, 128) < 0 || !drm_edid_block_valid(o->edid))
		return;
	o->nedid = 128;
	if(o->edid[126] >= 1 && get(a, 128, o->edid+128, 128) == 0)
		o->nedid = 256;
}

int
dtdtiming(uchar *d, Timing *t)
{
	int hb, vb, hso, hsw, vso, vsw;

	t->clock = (d[0] | d[1]<<8) * 10;
	if(t->clock == 0 || (d[17] & 0x80))	/* not a timing, interlaced */
		return -1;
	t->ha = d[2] | (d[4]>>4)<<8;
	hb = d[3] | (d[4]&0xF)<<8;
	t->va = d[5] | (d[7]>>4)<<8;
	vb = d[6] | (d[7]&0xF)<<8;
	hso = d[8] | (d[11]>>6 & 3)<<8;
	hsw = d[9] | (d[11]>>4 & 3)<<8;
	vso = d[10]>>4 | (d[11]>>2 & 3)<<4;
	vsw = (d[10] & 0xF) | (d[11] & 3)<<4;
	t->ht = t->ha + hb;
	t->vt = t->va + vb;
	t->hss = t->ha + hso;
	t->hse = t->hss + hsw;
	t->vss = t->va + vso;
	t->vse = t->vss + vsw;
	if(t->ht == 0 || t->vt == 0)
		return -1;
	t->phsync = (d[17] & 0x18) == 0x18 && (d[17] & 2);
	t->pvsync = (d[17] & 0x18) == 0x18 && (d[17] & 4);
	t->hz = ((vlong)t->clock*1000 + t->ht*t->vt/2) / (t->ht*t->vt);
	return 0;
}

/* the timing for w x h [hz] among the EDID's detailed timings that the port can drive */
int
findtiming(Output *o, int w, int h, int hz, Timing *t)
{
	uchar *e, *x;
	int i;

	e = o->edid;
	if(o->nedid < 128)
		return -1;
	for(i = 54; i < 126; i += 18)
		if(dtdtiming(e+i, t) == 0 && t->ha == w && t->va == h && (hz == 0 || t->hz == hz)
		&& t->clock <= Hdmimaxclock)
			return 0;
	if(o->nedid >= 256 && e[128] == 2)
		for(x = e+128, i = x[2]; i >= 4 && i + 18 <= 127; i += 18)
			if(dtdtiming(x+i, t) == 0 && t->ha == w && t->va == h && (hz == 0 || t->hz == hz)
			&& t->clock <= Hdmimaxclock)
				return 0;
	return -1;
}
