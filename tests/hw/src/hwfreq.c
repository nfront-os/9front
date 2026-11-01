/*
 * hwfreq: the clock each processor really runs at, from its APERF
 * and MPERF counters (MSRs 0xE8, 0xE7; #P/msr) over a second, against
 * the TSC's rate. Intel and AMD since about 2008; a processor without
 * them is "no APERF". Run by tests/hw/cat/stress.rc idle and under load.
 *	hwfreq [-l label]
 * prints per processor: cpuN label actual-MHz nominal-MHz
 */
#include <u.h>
#include <libc.h>

enum {
	MPERF = 0xE7,
	APERF = 0xE8,
};

static int
rdmsr(int fd, int msr, uvlong *v)
{
	return pread(fd, v, 8, msr) == 8? 0: -1;
}

static void
wire(int cpu)
{
	char buf[64];
	int fd;

	snprint(buf, sizeof buf, "/proc/%d/ctl", getpid());
	if((fd = open(buf, OWRITE)) < 0)
		sysfatal("open %s: %r", buf);
	if(fprint(fd, "wired %d", cpu) < 0)
		sysfatal("wired %d: %r", cpu);
	close(fd);
	sleep(0);	/* now on cpu */
}

static int
ncpus(void)
{
	char buf[8192];
	int fd, n, i, c;

	if((fd = open("/dev/sysstat", OREAD)) < 0)
		return 1;
	n = readn(fd, buf, sizeof buf);
	close(fd);
	c = 0;
	for(i = 0; i < n; i++)
		if(buf[i] == '\n')
			c++;
	return c > 0? c: 1;
}

void
main(int argc, char *argv[])
{
	char *label;
	uvlong a0, a1, m0, m1, t0, t1;
	vlong ns0, ns1;
	double secs, nominal;
	int fd, i, n;

	label = "";
	ARGBEGIN{
	case 'l':
		label = EARGF(sysfatal("usage: hwfreq [-l label]"));
		break;
	}ARGEND
	if((fd = open("#P/msr", OREAD)) < 0)
		sysfatal("open #P/msr: %r");
	n = ncpus();
	for(i = 0; i < n; i++){
		wire(i);
		if(rdmsr(fd, MPERF, &m0) < 0 || rdmsr(fd, APERF, &a0) < 0){
			print("cpu%d %s no APERF\n", i, label);
			continue;
		}
		cycles(&t0);
		ns0 = nsec();
		while(nsec() < ns0 + 1000000000LL)
			sleep(50);
		if(rdmsr(fd, MPERF, &m1) < 0 || rdmsr(fd, APERF, &a1) < 0){
			print("cpu%d %s no APERF\n", i, label);
			continue;
		}
		cycles(&t1);
		ns1 = nsec();
		secs = (double)(ns1 - ns0) / 1e9;
		nominal = (double)(t1 - t0) / secs / 1e6;	/* the TSC: the nominal clock */
		if(m1 == m0)
			print("cpu%d %s halted all the time %.0f\n", i, label, nominal);
		else
			print("cpu%d %s %.0f %.0f\n", i, label, nominal * (double)(a1 - a0) / (double)(m1 - m0), nominal);
	}
	exits(nil);
}
