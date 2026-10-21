#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"

#define	MAXCONF 64
static char *confname[MAXCONF];
static char *confval[MAXCONF];
static int nconf;
static char maxmem[256];
static int cpus;
static char ncpu[256];
static char timebase[32];

/*
 * the harts the kernel can run on: /cpus/cpu@N nodes not "disabled"
 * (PolarFire's E51 monitor core is); reg and status come in any order,
 * a node is done when the next one starts
 */
static int harts[MAXMACH];
static int nharts;
static struct {
	char	path[64];
	int	hart;
	int	off;
} cpunode = { "", -1, 0 };

static void
cpunodeend(void)
{
	if(cpunode.hart >= 0 && !cpunode.off && nharts < nelem(harts))
		harts[nharts++] = cpunode.hart;
	cpunode.path[0] = 0;
	cpunode.hart = -1;
	cpunode.off = 0;
}

/* the i'th hart of the device tree, -1 after the last */
int
dthart(int i)
{
	if(i < 0 || i >= nharts)
		return -1;
	return harts[i];
}

/* the device tree from the firmware (l.s); its /chosen/bootargs */
uintptr dtbpa;
static char cmdline[4*KiB];

static int
findconf(char *k)
{
	int i;

	for(i = 0; i < nconf; i++)
		if(cistrcmp(confname[i], k) == 0)
			return i;
	return -1;
}

static void
addconf(char *k, char *v)
{
	int i;

	i = findconf(k);
	if(i < 0){
		if(nconf >= MAXCONF)
			return;
		i = nconf++;
		confname[i] = k;
	}
	confval[i] = v;
}

static void
plan9iniinit(char *s, int cmdline)
{
	char *toks[MAXCONF];
	int i, c, n;
	char *v;

	if((c = *s) < ' ' || c >= 0x80)
		return;
	if(cmdline)
		n = tokenize(s, toks, MAXCONF);
	else
		n = getfields(s, toks, MAXCONF, 1, "\n");
	for(i = 0; i < n; i++){
		if(toks[i][0] == '#')
			continue;
		v = strchr(toks[i], '=');
		if(v == nil)
			continue;
		*v++ = '\0';
		addconf(toks[i], v);
	}
}

typedef struct Devtree Devtree;
struct Devtree
{
	uchar	*base;
	uchar	*end;
	char	*stab;
	char	path[1024];
};

enum {
	DtHeader	= 0xd00dfeed,
	DtBeginNode	= 1,
	DtEndNode	= 2,
	DtProp		= 3,
	DtEnd		= 9,
};

static u32int
beget4(uchar *p)
{
	return (u32int)p[0]<<24 | (u32int)p[1]<<16 | (u32int)p[2]<<8 | (u32int)p[3];
}

static void
devtreeprop(char *path, char *key, void *val, int len)
{
	uvlong addr;
	uchar *p = val;
	char *q;
	int i;

	if((strncmp(path, "/memory", 7) == 0 || strncmp(path, "/memory@0", 9) == 0)
	&& strcmp(key, "reg") == 0){
		if(findconf("*maxmem") < 0 && len == 16){
			p += 4;	/* ignore */
			addr = (uvlong)beget4(p+4)<<32 | beget4(p);
			addr += beget4(p+8);
			snprint(maxmem, sizeof(maxmem), "%#llux", addr);
			addconf("*maxmem", maxmem);
		}
		return;
	}
	/*
	 * the interrupts of the machine's devices (soc), from their nodes
	 * (name@address): the same registers on a board and in qemu's
	 * model, but other interrupts (FU740's uart0 39, FU540's 4)
	 */
	if(strcmp(key, "interrupts") == 0 && len >= 4 && (q = strrchr(path, '@')) != nil){
		addr = strtoull(q+1, nil, 16);
		if(addr == soc.uart)
			soc.uartirq = beget4(p);
		for(i = 0; i < nelem(soc.ether); i++)
			if(addr != 0 && addr == soc.ether[i])
				soc.etherirq[i] = beget4(p);
		if(addr != 0 && addr == (soc.sdhc & ~0xFFF))
			soc.sdhcirq = beget4(p);
		return;
	}
	/* the plic's sources (plic.c): FU540 53, FU740 69, the icicle 186 */
	if(strcmp(key, "riscv,ndev") == 0 && len == 4){
		soc.nirq = beget4(p) + 1;
		return;
	}
	/* the time csr's frequency (clock.c): 10MHz on qemu's virt, 1MHz on the icicle */
	if(strcmp(path, "/cpus") == 0 && strcmp(key, "timebase-frequency") == 0 && len == 4){
		if(findconf("*timebase") < 0){
			snprint(timebase, sizeof(timebase), "%ud", beget4(p));
			addconf("*timebase", timebase);
		}
		return;
	}
	if(strncmp(path, "/cpus/cpu@", 10) == 0 && strchr(path+10, '/') == nil){
		if(strcmp(path, cpunode.path) != 0){
			cpunodeend();
			strncpy(cpunode.path, path, sizeof(cpunode.path)-1);
		}
		if(strcmp(key, "reg") == 0 && len >= 4)
			cpunode.hart = beget4(p);
		else if(strcmp(key, "status") == 0)
			cpunode.off = strcmp(val, "disabled") == 0;
		return;
	}
	if(strncmp(path, "/chosen", 7) == 0 && strcmp(key, "bootargs") == 0){
		if(len >= sizeof(cmdline))
			len = sizeof(cmdline)-1;
		memmove(cmdline, val, len);
		cmdline[len] = 0;
		plan9iniinit(cmdline, 1);
		return;
	}
}

static uchar*
devtreenode(Devtree *t, uchar *p, char *cp)
{
	uchar *e = (uchar*)t->stab;
	char *s;
	int n;

	if(p+4 > e || beget4(p) != DtBeginNode)
		return nil;
	p += 4;
	if((s = memchr((char*)p, 0, e - p)) == nil)
		return nil;
	n = s - (char*)p;
	cp += n;
	if(cp >= &t->path[sizeof(t->path)])
		return nil;
	memmove(cp - n, (char*)p, n);
	*cp = 0;
	p += (n + 4) & ~3;
	while(p+12 <= e && beget4(p) == DtProp){
		n = beget4(p+4);
		if(p + 12 + n > e)
			return nil;
		s = t->stab + beget4(p+8);
		if(s < t->stab || s >= (char*)t->end
		|| memchr(s, 0, (char*)t->end - s) == nil)
			return nil;
		devtreeprop(t->path, s, p+12, n);
		p += 12 + ((n + 3) & ~3);
	}
	while(p+4 <= e && beget4(p) == DtBeginNode){
		*cp = '/';
		p = devtreenode(t, p, cp+1);
		if(p == nil)
			return nil;
	}
	if(p+4 > e || beget4(p) != DtEndNode)
		return nil;
	return p+4;
}

static int
parsedevtree(uchar *base, uintptr len)
{
	Devtree t[1];
	u32int total;

	if(len < 28 || beget4(base) != DtHeader)
		return -1;
	total = beget4(base+4);
	if(total < 28 || total > len)
		return -1;
	t->base = base;
	t->end = t->base + total;
	t->stab = (char*)base + beget4(base+12);
	if(t->stab >= (char*)t->end)
		return -1;
	devtreenode(t, base + beget4(base+8), t->path);
	cpunodeend();
	cpus = nharts;
	return  0;
}

void
bootargsinit(void)
{
	void *va;
	uintptr len;

	/* plan9.ini first, the command line's settings over it */
	BOOTARGS[BOOTARGSLEN-1] = 0;
	plan9iniinit(BOOTARGS, 0);
	if(dtbpa == 0 || (len = cankaddr(dtbpa)) == 0)
		return;
	va = KADDR(dtbpa);
	if(parsedevtree(va, len) == 0){
		/* user can provide fewer ncpu */
		if(findconf("*ncpu") < 0){
			snprint(ncpu, sizeof(ncpu), "%d", cpus);
			addconf("*ncpu", ncpu);
		}
	}
}

char*
getconf(char *name)
{
	int i;

	if((i = findconf(name)) < 0)
		return nil;
	return confval[i];
}

void
setconfenv(void)
{
	int i;

	if(nconf < 0){
		/* use defaults when there was no configuration */
		ksetenv("console", "0", 1);
		return;
	}

	for(i = 0; i < nconf; i++){
		if(confname[i][0] != '*')
			ksetenv(confname[i], confval[i], 0);
		ksetenv(confname[i], confval[i], 1);
	}
}

void
writeconf(void)
{
	char *p, *q;
	int n;

	p = getconfenv();
	if(waserror()) {
		free(p);
		nexterror();
	}

	/* convert to name=value\n format */
	for(q=p; *q; q++) {
		q += strlen(q);
		*q = '=';
		q += strlen(q);
		*q = '\n';
	}
	n = q - p + 1;
	if(n >= BOOTARGSLEN)
		error("kernel configuration too large");
	memmove(BOOTARGS, p, n);
	memset(BOOTARGS+n, 0, BOOTARGSLEN-n);
	poperror();
	free(p);
}
