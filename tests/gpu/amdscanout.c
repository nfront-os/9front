/*
 * SPDX-License-Identifier: MIT
 *
 * amdscanout: the display of aux/amdgpu (gpu(4)): a connected output's
 * preferred mode, an object in VRAM of that size with a picture (red,
 * green and blue bars over a grey ramp), on the display for secs
 * seconds, then a second picture (the bars the other way round: a page
 * flip) as long, then the display off again (scanout 0).  What the
 * monitor shows is for the eye; the program checks the replies.
 *
 * usage: amdscanout [-s secs] [dev]
 */
#include <u.h>
#include <libc.h>

static char *dev = "/dev/gpu/0";
static int nfail;

static void
ok(char *fmt, ...)
{
	va_list arg;
	char buf[256];

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("ok %s\n", buf);
}

static void
fail(char *fmt, ...)
{
	va_list arg;
	char buf[256];

	va_start(arg, fmt);
	vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	print("FAIL %s\n", buf);
	nfail++;
}

static char*
readfile(char *path)
{
	static char buf[8192];
	int fd, n;

	if((fd = open(path, OREAD)) < 0)
		return nil;
	n = readn(fd, buf, sizeof buf - 1);
	close(fd);
	if(n < 0)
		return nil;
	buf[n] = 0;
	return buf;
}

/* the first connected output (status: "output name connected") and its preferred mode */
static int
findoutput(char *name, int nname, int *w, int *h)
{
	char path[128], *s, *l, *e, *f[4];

	snprint(path, sizeof path, "%s/status", dev);
	if((s = readfile(path)) == nil)
		return -1;
	for(l = s; l != nil && *l; l = e){
		e = strchr(l, '\n');
		if(e != nil)
			*e++ = 0;
		if(tokenize(l, f, nelem(f)) == 3 && strcmp(f[0], "output") == 0 && strcmp(f[2], "connected") == 0){
			snprint(name, nname, "%s", f[1]);
			snprint(path, sizeof path, "%s/outputs/%s/modes", dev, name);
			if((s = readfile(path)) == nil)
				return -1;
			*w = strtol(s, &s, 10);	/* "WxH Hz", the preferred first */
			if(*s++ != 'x')
				return -1;
			*h = strtol(s, nil, 10);
			return *w > 0 && *h > 0 ? 0 : -1;
		}
	}
	return -1;
}

static int
ctl(int fd, char *reply, int nreply, char *fmt, ...)
{
	va_list arg;
	char buf[256];
	int n;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if(write(fd, buf, n) != n)
		return -1;
	if(reply == nil)
		return 0;
	n = pread(fd, reply, nreply-1, 0);
	if(n < 0)
		return -1;
	reply[n] = 0;
	return 0;
}

/* the picture: three bars, a grey ramp below; flip: the bars the other way */
static void
paint(u32int *p, int w, int h, int stride, int flip)
{
	static u32int bar[3] = { 0xff0000, 0x00ff00, 0x0000ff };
	int x, y, b;
	u32int *row;

	for(y = 0; y < h; y++){
		row = p + y * (stride / 4);
		for(x = 0; x < w; x++){
			if(y < 2*h/3){
				b = x * 3 / w;
				row[x] = bar[flip ? 2 - b : b];
			}else{
				b = x * 255 / (w - 1);
				row[x] = b << 16 | b << 8 | b;
			}
		}
	}
}

void
main(int argc, char **argv)
{
	char out[64], path[128], buf[256], *f[10], *mode;
	int cfd, w, h, stride, secs, id[2], i, n;
	uvlong size, off, barsize;
	uchar *bar0;

	secs = 10;
	ARGBEGIN{
	case 's':
		secs = atoi(EARGF(sysfatal("usage: amdscanout [-s secs] [dev]")));
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	if(findoutput(out, sizeof out, &w, &h) < 0)
		sysfatal("no connected output with modes in %s/status", dev);
	ok("output %s, preferred mode %dx%d", out, w, h);
	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	stride = (w * 4 + 255) & ~255;	/* DCN: pitches of 256 bytes */
	size = (uvlong)stride * h;
	bar0 = nil;
	for(i = 0; i < 2; i++){
		if(ctl(cfd, buf, sizeof buf, "bo %llud vram", size) < 0 || (n = tokenize(buf, f, nelem(f))) != 8){
			fail("bo %llud vram: %r %s", size, buf);
			exits("bo");
		}
		id[i] = atoi(f[1]);
		off = strtoull(f[6], nil, 0);
		barsize = strtoull(f[7], nil, 0);
		if(bar0 == nil && (bar0 = segattach(0, f[2], nil, barsize)) == (void*)-1)
			sysfatal("segattach %s: %r", f[2]);
		paint((u32int*)(bar0 + off), w, h, stride, i);
		ok("picture %d: object %d, %llud bytes at %s+%#llux", i+1, id[i], size, f[2], off);
	}
	for(i = 0; i < 2; i++){
		if(ctl(cfd, nil, 0, "scanout %d %d %d %d", id[i], w, h, stride) < 0){
			fail("scanout %d: %r", id[i]);
			continue;
		}
		snprint(path, sizeof path, "%s/outputs/%s/mode", dev, out);
		mode = readfile(path);
		ok("picture %d on %s: %s", i+1, out, mode != nil ? mode : "?\n");
		if(i == 0)
			print("look: red, green, blue bars over a grey ramp, %d s\n", secs);
		else
			print("look: blue, green, red now (a page flip), %d s\n", secs);
		sleep(secs * 1000);
	}
	if(ctl(cfd, nil, 0, "scanout 0") < 0)
		fail("scanout 0: %r");
	else{
		mode = readfile(path);
		ok("scanout 0: %s %s", out, mode != nil ? mode : "?\n");
	}
	ctl(cfd, nil, 0, "free %d", id[0]);
	ctl(cfd, nil, 0, "free %d", id[1]);
	close(cfd);
	print(nfail ? "amdscanout: %d failed\n" : "amdscanout: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
