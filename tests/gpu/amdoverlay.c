/*
 * SPDX-License-Identifier: MIT
 *
 * amdoverlay: a client's object on an overlay plane (gpu(4) overlay,
 * ../9front-ui PLAN §9b): an object in GTT (gem, map), filled by the
 * processor, shown n times at x y in turns of two colours, each show
 * waiting for the display (the plane's update at the vblank); the time a
 * frame takes, the driver's status line; refusals (a stride the copy
 * cannot take, a window no output has wholly); off.  On the monitor: a
 * 640x480 rectangle at 100,100 blinking red and blue, then gone.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: amdoverlay [-n frames] [-x x] [-y y] [dev]
 */
#include <u.h>
#include <libc.h>

enum { W = 640, H = 480, Stride = W*4 };

static char *dev = "/dev/gpu/0";
static int nfail;
static int cfd;

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

static int
req(char *reply, int nreply, char *fmt, ...)
{
	va_list arg;
	char buf[256];
	int n;

	va_start(arg, fmt);
	n = vsnprint(buf, sizeof buf, fmt, arg);
	va_end(arg);
	if(write(cfd, buf, n) != n)
		return -1;
	if(reply == nil)
		return 0;
	n = pread(cfd, reply, nreply-1, 0);
	if(n < 0)
		return -1;
	reply[n] = 0;
	return 0;
}

static void
fill(u32int *p, u32int c)
{
	int i;

	for(i = 0; i < Stride/4*H; i++)
		p[i] = c;
}

/* the status's overlay line */
static char*
ovline(void)
{
	static char buf[8192];
	char path[128], *p, *e;
	int fd, n;

	snprint(path, sizeof path, "%s/status", dev);
	if((fd = open(path, OREAD)) < 0)
		return "no status";
	n = readn(fd, buf, sizeof buf - 1);
	close(fd);
	buf[n > 0 ? n : 0] = 0;
	if((p = strstr(buf, "overlay ")) == nil)
		return "no overlay line";
	if((e = strchr(p, '\n')) != nil)
		*e = 0;
	return p;
}

void
main(int argc, char **argv)
{
	char path[128], buf[256], *f[8];
	u32int h, *px;
	int i, n, x, y;
	vlong t0, t1;

	n = 120;
	x = 100;
	y = 100;
	ARGBEGIN{
	case 'n':
		n = atoi(EARGF(sysfatal("usage")));
		break;
	case 'x':
		x = atoi(EARGF(sysfatal("usage")));
		break;
	case 'y':
		y = atoi(EARGF(sysfatal("usage")));
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(pread(cfd, buf, sizeof buf - 1, 0) <= 0)
		sysfatal("clone: %r");
	ok("%s", ovline());
	if(req(buf, sizeof buf, "gem %d 4096 gtt cpuaccess", Stride*H) < 0 || tokenize(buf, f, nelem(f)) != 2)
		sysfatal("gem: %r");
	h = strtoul(f[1], nil, 0);
	if(req(buf, sizeof buf, "map %ud", h) < 0 || tokenize(buf, f, nelem(f)) != 5)
		sysfatal("map: %r");
	if((px = segattach(0, f[2], nil, strtoull(f[4], nil, 0))) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);

	/* refusals: a stride not a multiple of 256, a window no output has wholly */
	if(req(nil, 0, "overlay %ud %d %d %d %d %d", h, x, y, W-1, H, Stride-4) < 0)
		ok("stride %d: %r", Stride-4);
	else
		fail("stride %d taken", Stride-4);
	if(req(nil, 0, "overlay %ud %d %d %d %d %d", h, -10, y, W, H, Stride) < 0)
		ok("at -10 %d: %r", y);
	else
		fail("at -10 %d taken", y);

	t0 = nsec();
	for(i = 0; i < n; i++){
		fill(px, i/30 % 2 ? 0x0000ff : 0xff0000);	/* half a second each */
		if(req(nil, 0, "overlay %ud %d %d %d %d %d", h, x, y, W, H, Stride) < 0){
			fail("overlay, frame %d: %r", i);
			break;
		}
		if(i == 0)
			ok("shown: %s", ovline());
	}
	t1 = nsec();
	if(i == n)
		ok("%d frames in %.2f s: %.2f ms each (the display's vblank)", n, (t1-t0)/1e9, (t1-t0)/1e6/n);
	if(req(nil, 0, "overlay off") < 0)
		fail("overlay off: %r");
	else if(strstr(ovline(), "none shown") == nil)
		fail("off: %s", ovline());
	else
		ok("off: %s", ovline());
	segdetach(px);
	req(nil, 0, "close %ud", h);
	close(cfd);
	print(nfail ? "amdoverlay: %d failed\n" : "amdoverlay: all passed\n", nfail);
	exits(nfail ? "failed" : nil);
}
