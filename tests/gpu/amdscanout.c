/*
 * SPDX-License-Identifier: MIT
 *
 * amdscanout: the display of aux/amdgpu (gpu(4)): a connected output's
 * preferred mode, an object in VRAM of that size with a picture (red,
 * green and blue bars over a grey ramp), on the display for secs
 * seconds, then a second picture (the bars the other way round: a page
 * flip) as long, then the display off again (scanout 0).  What the
 * monitor shows is for the eye; the program checks the replies, and
 * that the GPU sees the picture as the processor wrote it: sdma copies
 * pixels of it (first, middle, last row) into GTT.
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

/* sdma: copy 4 bytes from src to dst (sdma_v6_0: COPY_LINEAR) */
static int
sdmacopy(u32int *p, uvlong src, uvlong dst)
{
	p[0] = 1;	/* SDMA_OP_COPY, linear */
	p[1] = 4 - 1;
	p[2] = 0;
	p[3] = src;
	p[4] = src >> 32;
	p[5] = dst;
	p[6] = dst >> 32;
	p[7] = 0;	/* NOP */
	return 8;
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
		if(tokenize(l, f, nelem(f)) >= 3 && strcmp(f[0], "output") == 0 && strcmp(f[2], "connected") == 0){
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
	int cfd, sfd, ffd, w, h, stride, secs, id[2], i, j, n, ctx, cid, did, nk;
	uvlong size, gpu[2], cgpu, dgpu, at[3];
	u32int *cmd, *dst, want;
	uchar *pic[2];

	secs = 10;
	ARGBEGIN{
	case 's':
		secs = atoi(EARGF(sysfatal("usage: amdscanout [-s secs] [dev]")));
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	/* a monitor without a signal may sleep and drop its hotplug line: wait for it */
	for(i = 0; findoutput(out, sizeof out, &w, &h) < 0; i++){
		if(i == 20)
			sysfatal("no connected output with modes in %s/status", dev);
		sleep(1000);
	}
	ok("output %s, preferred mode %dx%d", out, w, h);
	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	stride = (w * 4 + 255) & ~255;	/* DCN: pitches of 256 bytes */
	size = (uvlong)stride * h;
	for(i = 0; i < 2; i++){
		if(ctl(cfd, buf, sizeof buf, "bo %llud vram", size) < 0 || (n = tokenize(buf, f, nelem(f))) != 6){
			fail("bo %llud vram: %r %s", size, buf);
			exits("bo");
		}
		id[i] = atoi(f[1]);
		gpu[i] = strtoull(f[5], nil, 0);
		/* the object's own segment: its part of BAR 0 */
		if((pic[i] = segattach(0, f[2], nil, size)) == (void*)-1)
			sysfatal("segattach %s: %r", f[2]);
		paint((u32int*)pic[i], w, h, stride, i);
		ok("picture %d: object %d, %llud bytes, segment %s", i+1, id[i], size, f[2]);
	}

	/* what the GPU sees of the pictures: sdma copies pixels into GTT */
	if(ctl(cfd, buf, sizeof buf, "bo 4096") < 0 || tokenize(buf, f, nelem(f)) != 6)
		sysfatal("bo 4096: %r");
	cid = atoi(f[1]);
	cgpu = strtoull(f[5], nil, 0);
	if((cmd = segattach(0, f[2], nil, 4096)) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	if(ctl(cfd, buf, sizeof buf, "bo 4096") < 0 || tokenize(buf, f, nelem(f)) != 6)
		sysfatal("bo 4096: %r");
	did = atoi(f[1]);
	dgpu = strtoull(f[5], nil, 0);
	if((dst = segattach(0, f[2], nil, 4096)) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	snprint(path, sizeof path, "%s/ctx", dev);
	{
		Dir *d;
		int dfd;

		ctx = 0;
		if((dfd = open(path, OREAD)) >= 0){
			n = dirreadall(dfd, &d);
			for(j = 0; j < n; j++)
				if(atoi(d[j].name) > ctx)
					ctx = atoi(d[j].name);
			free(d);
			close(dfd);
		}
	}
	snprint(path, sizeof path, "%s/ctx/%d/submit", dev, ctx);
	sfd = open(path, OWRITE);
	snprint(path, sizeof path, "%s/ctx/%d/fence", dev, ctx);
	ffd = open(path, ORDWR);
	if(sfd < 0 || ffd < 0)
		sysfatal("ctx %d: %r", ctx);
	at[0] = 0;
	at[1] = (uvlong)(h/2) * stride + (w/2) * 4;
	at[2] = (uvlong)(h-1) * stride + (w-1) * 4;
	nk = 0;
	for(i = 0; i < 2; i++)
		for(j = 0; j < 3; j++){
			memset(dst, 0, 64);
			n = sdmacopy(cmd, gpu[i] + at[j], dgpu);
			if(fprint(sfd, "%d 0 %d sdma", cid, n) < 0 || fprint(ffd, "wait %d", ++nk) < 0 ||
			   pread(ffd, buf, sizeof buf - 1, 0) <= 0){
				fail("sdma read of picture %d: %r", i+1);
				continue;
			}
			want = *(u32int*)(pic[i] + at[j]);
			if(dst[0] != want)
				fail("picture %d at %#llux: the processor wrote %#ux, the GPU reads %#ux", i+1, at[j], want, dst[0]);
			else
				ok("picture %d at %#llux: the GPU reads %#ux as written", i+1, at[j], dst[0]);
		}
	close(sfd);
	close(ffd);
	ctl(cfd, nil, 0, "free %d", cid);
	ctl(cfd, nil, 0, "free %d", did);
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
