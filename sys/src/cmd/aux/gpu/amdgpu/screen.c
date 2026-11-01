/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: the screen of draw(3) on the card, as aux/virtiogpu
 * makes it: #G's framebuffer (gpu(3)) is what devdraw draws into (vga(3)
 * type gpu), and what changes there (#G/n/dirty) is copied into the
 * screen's object in VRAM (linux/kms.c: amdgpu_screen_start), which the
 * display shows; through BAR 0, write-combining.  The mode is the
 * preferred one of the first connected output.
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include <kpi/p9.h>
#include "amdgpu.h"

static char *gdir;
static uchar *fb;		/* #G's framebuffer: w x h x 4, stride w*4 */
static uchar *vram;		/* the screen's object, through BAR 0 */
static int w, h;
static uint pitch;

static int
vgactl(char *fmt, ...)
{
	char buf[128];
	va_list arg;
	int fd, r;

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if((fd = open("#v/vgactl", OWRITE)) < 0)
		return -1;
	r = write(fd, buf, strlen(buf));
	close(fd);
	return r < 0 ? -1 : 0;
}

/* the first connected output's preferred mode */
static int
findmode(int *pw, int *ph)
{
	char *st, *s, *l, *e, *f[4], *m;
	int r;

	r = -1;
	if((st = amdgpu_outputs_status()) == nil)
		return -1;
	for(l = st; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		if(tokenize(l, f, nelem(f)) != 3 || strcmp(f[0], "output") != 0 || strcmp(f[2], "connected") != 0)
			continue;
		if((m = amdgpu_output_modes(f[1])) == nil)
			continue;
		s = m;
		*pw = strtol(s, &s, 10);
		if(*s++ == 'x')
			*ph = strtol(s, nil, 10);
		else
			*ph = 0;
		amdgpu_freestr(m);
		if(*pw > 0 && *ph > 0){
			r = 0;
			break;
		}
	}
	amdgpu_freestr(st);
	return r;
}

/* #G's framebuffer, size bytes at least: its segment, attached */
static uchar*
fbattach(ulong size)
{
	char path[64], buf[1024], *l, *e, *f[4];
	uchar *p;
	int fd, n;

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((fd = open(path, ORDWR)) < 0)
		return nil;
	if(fprint(fd, "fb %lud", size) < 0){
		close(fd);
		return nil;
	}
	n = pread(fd, buf, sizeof buf - 1, 0);
	close(fd);
	if(n <= 0)
		return nil;
	buf[n] = 0;
	for(l = buf; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		/* "fb pa size seg" */
		if(tokenize(l, f, nelem(f)) == 4 && strcmp(f[0], "fb") == 0){
			p = segattach(0, f[3], nil, strtoul(f[2], nil, 0));
			return p == (void*)-1 ? nil : p;
		}
	}
	werrstr("no framebuffer in %s", path);
	return nil;
}

/* what devdraw changed, into VRAM */
static void
copyrect(int x0, int y0, int x1, int y1)
{
	int y, n;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 > w) x1 = w;
	if(y1 > h) y1 = h;
	if(x0 >= x1 || y0 >= y1)
		return;
	n = (x1 - x0) * 4;
	for(y = y0; y < y1; y++)
		memmove(vram + (uvlong)y*pitch + x0*4, fb + (uvlong)y*w*4 + x0*4, n);
}

static void
dirtyproc(void*)
{
	char path[64], buf[64], *f[4];
	int fd, n;

	snprint(path, sizeof path, "%s/dirty", gdir);
	if((fd = open(path, OREAD)) < 0){
		fprint(2, "amdgpu: screen: %s: %r\n", path);
		return;
	}
	for(;;){
		n = read(fd, buf, sizeof buf - 1);
		if(n <= 0){
			fprint(2, "amdgpu: screen: %s: %r\n", path);
			return;
		}
		buf[n] = 0;
		if(tokenize(buf, f, nelem(f)) == 4)
			copyrect(atoi(f[0]), atoi(f[1]), atoi(f[2]), atoi(f[3]));
	}
}

/* the screen on the card; 0, or -1 (and the screen stays where it was) */
int
screenstart(char *dir)
{
	unsigned long long off;
	uchar *bar;
	int r;

	gdir = dir;
	if(findmode(&w, &h) < 0){
		werrstr("no connected output");
		return -1;
	}
	if((fb = fbattach((ulong)w*h*4)) == nil)
		return -1;
	if((r = amdgpu_screen_start(w, h, &off, &pitch)) < 0){
		werrstr("screen object: error %d", -r);
		return -1;
	}
	if((bar = p9_barwc(0)) == nil){
		werrstr("BAR 0 write-combining: %r");
		return -1;
	}
	vram = bar + off;
	if(vgactl("type gpu") < 0 || vgactl("size %dx%dx32 x8r8g8b8", w, h) < 0 || vgactl("drawinit") < 0){
		werrstr("vgactl: %r");
		return -1;
	}
	vgactl("hwgc soft");
	copyrect(0, 0, w, h);
	proccreate(dirtyproc, nil, 16384);
	fprint(2, "amdgpu: screen %dx%d\n", w, h);
	return 0;
}
