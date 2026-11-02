/*
 * SPDX-License-Identifier: MIT
 *
 * amdctxtest: gpu(4)'s contexts on aux/amdgpu (a Radeon of GFX 11): a
 * context, an object for the commands and one the GPU writes into, then
 * on each engine (gfx, compute, sdma) one IB that writes a value there
 * (as amdgpu's own IB tests do: PM4 WRITE_DATA, SDMA's linear write),
 * its fence, the value as the processor reads it through the object's
 * segment; the same into an object in VRAM, read through BAR 0's
 * segment; a value the processor writes into VRAM through BAR 0, copied
 * by sdma into GTT; the fence file; the objects and the context freed.
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
#define SDMA_OP_COPY	1	/* sub op 0: linear */
#define SDMA_OP_WRITE	2
#define SDMA_OP_NOP	0

static char *dev = "/dev/gpu/0";
static int nfail;
static jmp_buf faultjb;

static void
onfault(void *u, char *msg)
{
	if(strstr(msg, "sys: trap: fault") != nil)
		notejmp(u, faultjb, 1);
	noted(NDFLT);
}

/* does reading p fault (a revoked segment)? */
static int
revoked(void *p)
{
	volatile u32int v;

	notify(onfault);
	if(setjmp(faultjb)){
		notify(nil);
		return 1;
	}
	v = *(volatile u32int*)p;
	USED(v);
	notify(nil);
	return 0;
}

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
	u32int	*p;	/* attached (vram: at its offset in BAR 0) */
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

/*
 * bo size [vram]: "bo id seg va size gpuaddr" (vram: seg is the object's part of BAR 0)
 * with seg BAR 0's segment
 */
static int
newbo(int fd, Bo *b, uvlong size, int vram)
{
	char buf[256], *f[10];
	int n;

	if(ctl(fd, buf, sizeof buf, vram ? "bo %llud vram" : "bo %llud", size) < 0){
		fail("bo %llud%s: %r", size, vram ? " vram" : "");
		return -1;
	}
	n = tokenize(buf, f, nelem(f));
	if(n != 6 || strcmp(f[0], "bo") != 0){
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
	ok("bo %d: %s%s %#llux bytes at gpu %#llux", b->id, vram ? "vram " : "", b->seg, b->size, b->gpu);
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

/* on each engine one IB that writes its value into d; fences from k0+1 */
static void
writes(int sfd, int ffd, Bo *cmd, Bo *d, char *what, int k0)
{
	char buf[128];
	u32int v;
	int ip, ndw, n;
	vlong t0;

	for(ip = 0; ip < 3; ip++){
		v = 0xC0DE0000 | k0 << 8 | ip;
		ndw = ib(ip, cmd->p + ip*64, d->gpu + ip*4, v);
		if(fprint(sfd, "%d %d %d %s", cmd->id, ip*64*4, ndw, ipname[ip]) < 0){
			fail("%s %s: submit: %r", what, ipname[ip]);
			continue;
		}
		t0 = nsec();
		if(fprint(ffd, "wait %d", k0+ip+1) < 0 || (n = pread(ffd, buf, sizeof buf - 1, 0)) <= 0){
			fail("%s %s: fence %d: %r", what, ipname[ip], k0+ip+1);
			continue;
		}
		buf[n] = 0;
		if(strstr(buf, "done") == nil || strstr(buf, "failed") != nil){
			fail("%s %s: fence %d: %s", what, ipname[ip], k0+ip+1, buf);
			continue;
		}
		if(d->p[ip] != v)
			fail("%s %s: wrote %#ux, memory has %#ux", what, ipname[ip], v, d->p[ip]);
		else
			ok("%s %s: IB wrote %#ux, fence %d done in %lld µs", what, ipname[ip], v, k0+ip+1, (nsec() - t0) / 1000);
	}
}

void
main(int argc, char **argv)
{
	char path[128], buf[256], *f[4];
	int cfd, sfd, ffd, n, ctx;
	Bo cmd, dst, vdst;

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

	memset(&vdst, 0, sizeof vdst);
	if(newbo(cfd, &cmd, 65536, 0) < 0 || newbo(cfd, &dst, 4096, 0) < 0)
		sysfatal("no objects");
	memset(dst.p, 0, dst.size);

	writes(sfd, ffd, &cmd, &dst, "gtt", 0);
	if(newbo(cfd, &vdst, 4096, 1) == 0){
		memset(vdst.p, 0, 64);
		writes(sfd, ffd, &cmd, &vdst, "vram", 3);
		/* the processor into VRAM through BAR 0, sdma from there into GTT */
		vdst.p[8] = 0x7ee12345;
		cmd.p[3*64] = SDMA_OP_COPY;
		cmd.p[3*64+1] = 4 - 1;
		cmd.p[3*64+2] = 0;
		cmd.p[3*64+3] = vdst.gpu + 8*4;
		cmd.p[3*64+4] = (vdst.gpu + 8*4) >> 32;
		cmd.p[3*64+5] = dst.gpu + 8*4;
		cmd.p[3*64+6] = (dst.gpu + 8*4) >> 32;
		cmd.p[3*64+7] = SDMA_OP_NOP;
		dst.p[8] = 0;
		if(fprint(sfd, "%d %d %d sdma", cmd.id, 3*64*4, 8) < 0)
			fail("copy: submit: %r");
		else if(fprint(ffd, "wait 7") < 0 || (n = pread(ffd, buf, sizeof buf - 1, 0)) <= 0)
			fail("copy: fence 7: %r");
		else{
			buf[n] = 0;
			if(strstr(buf, "done") == nil || strstr(buf, "failed") != nil)
				fail("copy: fence 7: %s", buf);
			else if(dst.p[8] != 0x7ee12345)
				fail("copy: processor wrote 0x7ee12345 into vram, sdma copied %#ux", dst.p[8]);
			else
				ok("processor -> vram (BAR 0) -> sdma -> gtt: %#ux", dst.p[8]);
		}
	}

	fprint(ffd, "0");	/* plain reads again: "done submitted" */
	if((n = pread(ffd, buf, sizeof buf - 1, 0)) > 0){
		buf[n] = 0;
		if(strncmp(buf, "7 7", 3) == 0 && strstr(buf, "failed") == nil)
			ok("fence: %s", buf);
		else
			fail("fence: %s", buf);
	}

	segdetach(cmd.p);
	segdetach(dst.p);
	if(ctl(cfd, nil, 0, "free %d", cmd.id) < 0 || ctl(cfd, nil, 0, "free %d", dst.id) < 0 ||
	   (vdst.id && ctl(cfd, nil, 0, "free %d", vdst.id) < 0))
		fail("free: %r");
	else
		ok("objects freed");
	/* the VRAM object's segment, still attached: revoked by the free */
	if(vdst.id){
		if(revoked(vdst.p))
			ok("freed VRAM object: its segment faults");
		else
			fail("freed VRAM object still readable");
		segdetach(vdst.p);
	}
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
