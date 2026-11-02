/*
 * SPDX-License-Identifier: MIT
 *
 * amdhang: a GPU hang and what comes after it (aux/amdgpu, gpu(4)).  One
 * context submits on gfx an IB that waits for a memory word nobody
 * writes (WAIT_REG_MEM); amdgpu's job timeout (amdgpu.lockup_timeout,
 * 10 s for gfx) is to find it, reset the GPU and fail that context's
 * fence.  Then a new context's IBs on gfx, compute and sdma must run
 * again.  Prints "ok ..." / "FAIL ..." lines and a summary; how long the
 * hang took to end.
 *
 * usage: amdhang [-t secs] [dev]	(-t: how long to wait for the end of the hang, 60)
 */
#include <u.h>
#include <libc.h>

#define PACKET3(op, n)	((3U << 30) | (((op) & 0xFF) << 8) | (((n) & 0x3FFF) << 16))
#define PACKET3_WRITE_DATA	0x37
#define PACKET3_WAIT_REG_MEM	0x3C
#define WRITE_DATA_DST_SEL(x)	((x) << 8)	/* 5: memory */
#define WR_CONFIRM	(1U << 20)
#define WAIT_EQUAL	3
#define WAIT_MEM	(1U << 4)
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

typedef struct Ctx Ctx;
struct Ctx {
	int	id;
	int	cfd, sfd, ffd;
	int	cmd;		/* the command object: id, its memory, GPU address */
	u32int	*cp;
	uvlong	cgpu;
	int	dst;
	u32int	*dp;
	uvlong	dgpu;
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

/* an object in GTT: its id, memory and GPU address */
static int
newbo(int fd, uvlong size, u32int **p, uvlong *gpu)
{
	char buf[256], *f[10];

	if(ctl(fd, buf, sizeof buf, "bo %llud", size) < 0 || tokenize(buf, f, nelem(f)) != 6){
		fail("bo %llud: %r", size);
		return -1;
	}
	*gpu = strtoull(f[5], nil, 0);
	if((*p = segattach(0, f[2], nil, size)) == (void*)-1){
		fail("segattach %s: %r", f[2]);
		return -1;
	}
	memset(*p, 0, size);
	return atoi(f[1]);
}

static int
newctx(Ctx *c)
{
	char path[128], buf[64], *f[4];
	int n;

	memset(c, 0, sizeof *c);
	snprint(path, sizeof path, "%s/clone", dev);
	if((c->cfd = open(path, ORDWR)) < 0 || (n = pread(c->cfd, buf, sizeof buf - 1, 0)) <= 0){
		fail("clone: %r");
		return -1;
	}
	buf[n] = 0;
	if(tokenize(buf, f, nelem(f)) < 2){
		fail("clone: %s", buf);
		return -1;
	}
	c->id = atoi(f[1]);
	snprint(path, sizeof path, "%s/ctx/%d/submit", dev, c->id);
	if((c->sfd = open(path, OWRITE)) < 0){
		fail("open %s: %r", path);
		return -1;
	}
	snprint(path, sizeof path, "%s/ctx/%d/fence", dev, c->id);
	if((c->ffd = open(path, ORDWR)) < 0){
		fail("open %s: %r", path);
		return -1;
	}
	if((c->cmd = newbo(c->cfd, 65536, &c->cp, &c->cgpu)) < 0 ||
	   (c->dst = newbo(c->cfd, 4096, &c->dp, &c->dgpu)) < 0)
		return -1;
	return 0;
}

/* the fence's state: "k done|busy [failed]" */
static int
fence(Ctx *c, int k, int query, char *buf, int nbuf)
{
	int n;

	if(fprint(c->ffd, "%s %d", query ? "query" : "wait", k) < 0 || (n = pread(c->ffd, buf, nbuf - 1, 0)) <= 0)
		return -1;
	buf[n] = 0;
	return 0;
}

static char *ipname[] = { "gfx", "compute", "sdma" };

/* one IB per engine writing a value; fences k0+1... */
static void
writes(Ctx *c, int k0)
{
	char buf[128];
	u32int v, *p;
	int ip, ndw;

	for(ip = 0; ip < 3; ip++){
		v = 0xA11E0000 | ip;
		p = c->cp + ip*64;
		if(ip == 2){
			p[0] = SDMA_OP_WRITE;
			p[1] = c->dgpu + ip*4;
			p[2] = (c->dgpu + ip*4) >> 32;
			p[3] = 0;
			p[4] = v;
			p[5] = p[6] = p[7] = SDMA_OP_NOP;
			ndw = 8;
		}else{
			p[0] = PACKET3(PACKET3_WRITE_DATA, 3);
			p[1] = WRITE_DATA_DST_SEL(5) | WR_CONFIRM;
			p[2] = c->dgpu + ip*4;
			p[3] = (c->dgpu + ip*4) >> 32;
			p[4] = v;
			ndw = 5;
		}
		if(fprint(c->sfd, "%d %d %d %s", c->cmd, ip*64*4, ndw, ipname[ip]) < 0){
			fail("after the hang: %s: submit: %r", ipname[ip]);
			continue;
		}
		if(fence(c, k0+ip+1, 0, buf, sizeof buf) < 0 || strstr(buf, "done") == nil){
			fail("after the hang: %s: fence %d: %r %s", ipname[ip], k0+ip+1, buf);
			continue;
		}
		if(c->dp[ip] != v)
			fail("after the hang: %s: memory has %#ux, not %#ux", ipname[ip], c->dp[ip], v);
		else
			ok("after the hang: %s runs (wrote %#ux)", ipname[ip], v);
	}
}

static void
status(char *grep)
{
	char path[128], *buf, *s, *e;
	int fd, n;

	snprint(path, sizeof path, "%s/status", dev);
	if((fd = open(path, OREAD)) < 0)
		return;
	buf = malloc(16384);
	n = readn(fd, buf, 16383);
	close(fd);
	if(n < 0)
		n = 0;
	buf[n] = 0;
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) == nil)
			e = s + strlen(s);
		else
			e++;
		if(strncmp(s, grep, strlen(grep)) == 0)
			print("\t%.*s", (int)(e - s), s);
	}
	free(buf);
}

void
main(int argc, char **argv)
{
	char buf[128];
	Ctx h, c;
	vlong t0;
	int secs, done, i;

	secs = 60;
	ARGBEGIN{
	case 't':
		secs = atoi(EARGF(sysfatal("usage: amdhang [-t secs] [dev]")));
		break;
	}ARGEND
	if(argc > 0)
		dev = argv[0];

	/* the hang: gfx waits for dst[0] == 1, which nobody writes */
	if(newctx(&h) < 0)
		sysfatal("no context");
	ok("ctx %d: an IB on gfx that waits for a word nobody writes", h.id);
	h.cp[0] = PACKET3(PACKET3_WAIT_REG_MEM, 5);
	h.cp[1] = WAIT_EQUAL | WAIT_MEM;
	h.cp[2] = h.dgpu;
	h.cp[3] = h.dgpu >> 32;
	h.cp[4] = 1;		/* reference */
	h.cp[5] = ~0;		/* mask */
	h.cp[6] = 4;		/* poll interval */
	if(fprint(h.sfd, "%d 0 7 gfx", h.cmd) < 0)
		sysfatal("submit the hang: %r");
	t0 = nsec();
	sleep(1000);
	if(fence(&h, 1, 1, buf, sizeof buf) == 0 && strstr(buf, "busy") != nil)
		ok("it hangs: fence 1 %s", buf);
	else
		fail("fence 1 after 1 s: %s (want busy)", buf);
	status("ring gfx");

	/* amdgpu's timeout, the reset: the fence ends, failed */
	done = 0;
	for(i = 0; i < secs; i++){
		if(fence(&h, 1, 1, buf, sizeof buf) == 0 && strstr(buf, "done") != nil){
			done = 1;
			break;
		}
		sleep(1000);
	}
	if(!done)
		fail("the hang did not end in %d s: fence 1 %s", secs, buf);
	else if(strstr(buf, "failed") == nil)
		fail("the hang ended after %lld s, its fence not failed: %s", (nsec() - t0) / 1000000000LL, buf);
	else
		ok("the hang ended after %lld s, its context's fence failed", (nsec() - t0) / 1000000000LL);
	status("ring ");

	/* a new context: the GPU works again */
	if(newctx(&c) < 0)
		fail("no context after the hang");
	else
		writes(&c, 0);
	if(nfail)
		print("amdhang: %d failed\n", nfail);
	else
		print("amdhang: all passed\n");
	exits(nfail ? "failed" : nil);
}
