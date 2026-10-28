/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: outputs/<conn>/{edid,modes,mode}: the text of the files, a mode set by name, the test picture.
 * Plan 9 only: no counterpart in Linux.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * the EDID's modes, the preferred first: detailed timings (base block,
 * CEA extension), standard and some established timings; "wxh hz"
 */
int
addmode(char *b, int n, int len, int w, int h, int hz)
{
	char m[32];

	if(w <= 0 || h <= 0)
		return n;
	snprint(m, sizeof m, "%dx%d %d\n", w, h, hz);
	if(strstr(b, m) != nil)
		return n;
	return n + snprint(b+n, len-n, "%s", m);
}

int
dtd(char *b, int n, int len, uchar *d)
{
	int clk, ha, hb, va, vb;

	clk = (d[0] | d[1]<<8) * 10;	/* kHz */
	if(clk == 0)
		return n;
	ha = d[2] | (d[4]>>4)<<8;
	hb = d[3] | (d[4]&0xF)<<8;
	va = d[5] | (d[7]>>4)<<8;
	vb = d[6] | (d[7]&0xF)<<8;
	if(ha+hb == 0 || va+vb == 0)
		return n;
	return addmode(b, n, len, ha, va, (int)(((vlong)clk*1000 + (ha+hb)*(va+vb)/2) / ((ha+hb)*(va+vb))));
}

char*
outmodes(Output *o)
{
	static struct { int byte, bit, w, h; } est[] = {
		35, 5, 640, 480,  35, 0, 800, 600,  36, 3, 1024, 768,  36, 0, 1280, 1024,
	};
	char *b;
	uchar *e, *x;
	int n, i, w, a, len;

	len = 4096;
	b = mallocz(len, 1);
	if(o->nedid == 0)
		return b;
	e = o->edid;
	n = 0;
	for(i = 54; i < 126; i += 18)
		n = dtd(b, n, len, e+i);
	if(o->nedid >= 256 && e[128] == 2)	/* CEA-861: its detailed timings */
		for(x = e+128, i = x[2]; i >= 4 && i + 18 <= 127; i += 18)
			n = dtd(b, n, len, x+i);
	for(i = 38; i < 54; i += 2){
		if(e[i] == 1 && e[i+1] == 1)
			continue;
		w = (e[i] + 31) * 8;
		a = e[i+1] >> 6;
		n = addmode(b, n, len, w, a == 0 ? w*10/16 : a == 1 ? w*3/4 : a == 2 ? w*4/5 : w*9/16,
			(e[i+1] & 0x3F) + 60);
	}
	for(i = 0; i < nelem(est); i++)
		if(e[est[i].byte] & 1<<est[i].bit)
			n = addmode(b, n, len, est[i].w, est[i].h, 60);
	return b;
}

char*
outmode(Output *o)
{
	if(o->trans < 0)
		return smprint("off\n");
	return smprint("%dx%d %ld\n", o->w, o->h, (o->refresh + 500) / 1000);
}

char*
dispstatus(void)
{
	char *s, *t;
	Output *o;
	int i;

	s = smprint("display cdclk %d kHz\n", cdclk);
	for(i = 0; i < noutputs; i++){
		o = &outputs[i];
		if(o->trans >= 0)
			t = smprint("%soutput %s %s, %dx%d %ld.%03ld Hz, pipe %c transcoder %s, edid %d bytes\n", s,
				o->name, o->connected ? "connected" : "disconnected", o->w, o->h,
				o->refresh / 1000, o->refresh % 1000, 'A' + o->pipe, transname[o->trans], o->nedid);
		else
			t = smprint("%soutput %s %s, off, edid %d bytes\n", s, o->name,
				o->connected ? "connected" : "disconnected", o->nedid);
		free(s);
		s = t;
	}
	return s;
}

int
parsewxh(char *s, int *w, int *h)
{
	char *x;

	*w = strtol(s, &x, 10);
	if(*x != 'x')
		return -1;
	*h = strtol(x+1, &x, 10);
	return *x == 0 && *w > 0 && *h > 0 ? 0 : -1;
}

/* colour bars and a frame: what the driver shows until a client draws */
void
testpicture(Output *o, int w, int h)
{
	static u32int bars[] = { 0xFFFFFF, 0xFFFF00, 0x00FFFF, 0x00FF00, 0xFF00FF, 0xFF0000, 0x0000FF, 0x000000 };
	u32int *p;
	int x, y;

	for(y = 0; y < h; y++){
		p = o->fb + y*w;
		for(x = 0; x < w; x++)
			p[x] = (y < 8 || y >= h-8 || x < 8 || x >= w-8) ? 0x808080 : bars[x*8/w];
	}
	clflushrange(o->fb, (ulong)w*h*4);
}

/* write to outputs/<conn>/mode: wxh [hz] (one of the EDID's detailed timings), or off */
char*
outsetmode(Output *o, char *req)
{
	Timing t;
	char *f[3];
	int n, w, h;

	n = tokenize(req, f, nelem(f));
	if(n == 1 && strcmp(f[0], "off") == 0){
		if(!o->ours)
			return "not set by this driver";
		hsw_crtc_disable(o);
		return nil;
	}
	if(n < 1 || n > 2 || parsewxh(f[0], &w, &h) < 0)
		return "usage: wxh [hz] | off";
	if(!o->connected)
		return "nothing connected";
	if(findtiming(o, w, h, n == 2 ? atoi(f[1]) : 0, &t) < 0)
		return "no such mode among the EDID's detailed timings (up to 300 MHz)";
	return hsw_crtc_enable(o, &t);
}
