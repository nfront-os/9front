/*
 * SPDX-License-Identifier: MIT
 *
 * amdctxtest: gpu(4)'s contexts on aux/amdgpu (a Radeon of GFX 11): a
 * context, an object for the commands and one the GPU writes into, then
 * on each engine (gfx, compute, sdma) one IB that writes a value there
 * (as amdgpu's own IB tests do: PM4 WRITE_DATA, SDMA's linear write),
 * its fence, the value as the processor reads it through the object's
 * segment; the fence file; the objects and the context freed.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <u.h>
#include <libc.h>

/* PM4 (nvd.h): type 3 packets */
#define PACKET3(op, n)	((3U << 30) | (((op) & 0xFF) << 8) | (((n) & 0x3FFF) << 16))
#define PACKET3_WRITE_DATA	0x37
#define WRITE_DATA_DST_SEL(x)	((x) << 8)	/* 5: memory */
#define WR_CONFIRM	(1U << 20)
/* SDMA 6 (sdma_v6_0_0_pkt_open.h) */
#define SDMA_OP_WRITE	2
#define SDMA_OP_NOP	0

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

typedef struct Bo Bo;
struct Bo {
	int	id;
	char	seg[64];
	uvlong	va, size, gpu;
	u32int	*p;	/* attached */
};

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

/* bo size: "bo id seg va size gpuaddr" */
static int
newbo(int fd, Bo *b, uvlong size)
{
	char buf[256], *f[8];

	if(ctl(fd, buf, sizeof buf, "bo %llud", size) < 0){
		fail("bo %llud: %r", size);
		return -1;
	}
	if(tokenize(buf, f, nelem(f)) != 6 || strcmp(f[0], "bo") != 0){
		fail("bo reply: %s", buf);
		return -1;
	}
	b->id = atoi(f[1]);
	snprint(b->seg, sizeof b->seg, "%s", f[2]);
	b->va = strtoull(f[3], nil, 0);
	b->size = strtoull(f[4], nil, 0);
	b->gpu = strtoull(f[5], nil, 0);
	b->p = segattach(0, b->seg, nil, b->size);
	if(b->p == (void*)-1){
		fail("segattach %s: %r", b->seg);
		return -1;
	}
	ok("bo %d: %s %#llux bytes at gpu %#llux", b->id, b->seg, b->size, b->gpu);
	return 0;
}

/* the IB of engine ip at cmd[off/4...] writing v to dst: its dwords */
static int
ib(int ip, u32int *cmd, uvlong dst, u32int v)
{
	if(ip == 2){	/* sdma: linear write of one dword, NOPs after */
		cmd[0] = SDMA_OP_WRITE;
		cmd[1] = dst;
		cmd[2] = dst >> 32;
		cmd[3] = 0;	/* count - 1 */
		cmd[4] = v;
		cmd[5] = cmd[6] = cmd[7] = SDMA_OP_NOP;
		return 8;
	}
	cmd[0] = PACKET3(PACKET3_WRITE_DATA, 3);
	cmd[1] = WRITE_DATA_DST_SEL(5) | WR_CONFIRM;
	cmd[2] = dst;
	cmd[3] = dst >> 32;
	cmd[4] = v;
	return 5;
}

static char *ipname[] = { "gfx", "compute", "sdma" };

void
main(int argc, char **argv)
{
	char path[128], buf[256], *f[4];
	int cfd, sfd, ffd, n, ip, ndw, ctx;
	u32int v;
	Bo cmd, dst;

	ARGBEGIN{
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	snprint(path, sizeof path, "%s/status", dev);
	if((n = open(path, OREAD)) >= 0){
		int m = read(n, buf, sizeof buf - 1);

		close(n);
		buf[m > 0 ? m : 0] = 0;
		print("%s", buf);
	}
	snprint(path, sizeof path, "%s/clone", dev);
	if((cfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if((n = pread(cfd, buf, sizeof buf - 1, 0)) <= 0)
		sysfatal("read clone: %r");
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 2 || strcmp(f[0], "ctx") != 0)
		sysfatal("clone: %s", buf);
	ctx = atoi(f[1]);
	ok("ctx %d", ctx);
	snprint(path, sizeof path, "%s/ctx/%d/submit", dev, ctx);
	if((sfd = open(path, OWRITE)) < 0)
		sysfatal("open %s: %r", path);
	snprint(path, sizeof path, "%s/ctx/%d/fence", dev, ctx);
	if((ffd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);

	if(newbo(cfd, &cmd, 65536) < 0 || newbo(cfd, &dst, 4096) < 0)
		sysfatal("no objects");
	memset(dst.p, 0, dst.size);

	for(ip = 0; ip < 3; ip++){
		v = 0xC0DE0000 | ip;
		ndw = ib(ip, cmd.p + ip*64, dst.gpu + ip*4, v);
		if(fprint(sfd, "%d %d %d %s", cmd.id, ip*64*4, ndw, ipname[ip]) < 0){
			fail("%s: submit: %r", ipname[ip]);
			continue;
		}
		if(fprint(ffd, "wait %d", ip+1) < 0 || (n = pread(ffd, buf, sizeof buf - 1, 0)) <= 0){
			fail("%s: fence %d: %r", ipname[ip], ip+1);
			continue;
		}
		buf[n] = 0;
		if(strstr(buf, "done") == nil || strstr(buf, "failed") != nil){
			fail("%s: fence %d: %s", ipname[ip], ip+1, buf);
			continue;
		}
		if(dst.p[ip] != v)
			fail("%s: wrote %#ux, memory has %#ux", ipname[ip], v, dst.p[ip]);
		else
			ok("%s: IB wrote %#ux, fence %d done", ipname[ip], v, ip+1);
	}

	fprint(ffd, "0");	/* plain reads again: "done submitted" */
	if((n = pread(ffd, buf, sizeof buf - 1, 0)) > 0){
		buf[n] = 0;
		if(strncmp(buf, "3 3", 3) == 0 && strstr(buf, "failed") == nil)
			ok("fence: %s", buf);
		else
			fail("fence: %s", buf);
	}

	segdetach(cmd.p);
	segdetach(dst.p);
	if(ctl(cfd, nil, 0, "free %d", cmd.id) < 0 || ctl(cfd, nil, 0, "free %d", dst.id) < 0)
		fail("free: %r");
	else
		ok("objects freed");
	close(ffd);
	close(sfd);
	close(cfd);
	snprint(path, sizeof path, "%s/ctx/%d", dev, ctx);
	if(access(path, AEXIST) == 0)
		fail("ctx %d still there", ctx);
	else
		ok("ctx %d gone", ctx);
	if(nfail)
		print("amdctxtest: %d failed\n", nfail);
	else
		print("amdctxtest: all passed\n");
	exits(nfail ? "failed" : nil);
}
