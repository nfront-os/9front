/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: Linux 7.3's i915 driver on the LinuxKPI (sys/src/linux),
 * as a file server of gpu(4).  This first part starts the KPI, DRM's
 * core and i915, which probes the device of #G; the file server over
 * the driver's ioctls follows.
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include <ureg.h>
#include "../../../../libthread/threadimpl.h"	/* threaddump: libthread's procs and threads */
#include <kpi/linux.h>
#include "i915.h"
#include "../kmsout.h"

/*
 * the Linux code runs on the procs' stacks: more than libthread's 8 KB
 * (Linux gives its kernel code 16 KB; amdgpu's DC overflowed it into the
 * heap, where the stack is)
 */
int	mainstacksize = 1024*1024;

void	serve(char *srvname);

/*
 * a trap in any proc (they inherit the handler): the note, the pc and
 * the call chain by frame pointers to standard error, that is into the
 * log; the addresses are looked up on the build machine (addr2line)
 */
/*
 * the note "threaddump" (echo threaddump >/proc/n/note, any proc of
 * ours): every thread of every proc with its call chain, from libthread's
 * lists; a thread that does not run has its context in sched (sp, pc, bp
 * first, libc/amd64/setjmp.S).  For hangs: who waits where.
 */
static void
chain(uvlong *bp)
{
	int i;

	for(i = 0; i < 40 && bp != nil && ((uintptr)bp & 7) == 0; i++){
		fprint(2, " %#llux", bp[1]);
		if((uvlong*)bp[0] <= bp)
			break;
		bp = (uvlong*)bp[0];
	}
	fprint(2, "\n");
}

static void
threaddump(void)
{
	Proc *p;
	Thread *t;
	uvlong *jb;

	for(p = _threadpq.head; p != nil; p = p->next)
		for(t = p->threads.head; t != nil; t = t->nextt){
			jb = (uvlong*)t->sched;
			fprint(2, "i915 thread: pid %d id %d %s state %d pc %#llux; chain",
				p->pid, t->id, t->cmdname ? t->cmdname : "", t->state, jb[1]);
			chain((uvlong*)jb[2]);
		}
}

/* -W s: a thread dump every s seconds, for hangs (a note would wait for a running thread) */
static void
watchdog(void *a)
{
	long s = (long)(uintptr)a;

	for(;;){
		sleep(s * 1000);
		fprint(2, "i915: threaddump after %lds\n", s);
		threaddump();
	}
}

static int
trapnote(void *v, char *note)
{
	struct Ureg *u;
	uvlong *bp;
	int i;

	if(strcmp(note, "threaddump") == 0){
		threaddump();
		return 1;
	}
	if(strncmp(note, "sys:", 4) != 0)
		return 0;
	if(p9_fault(note))
		return 1;
	u = v;
	fprint(2, "i915 %d: %s pc %#llux sp %#llux; chain", getpid(), note, u->pc, u->sp);
	bp = (uvlong*)u->bp;
	for(i = 0; i < 40 && bp != nil && ((uintptr)bp & 7) == 0; i++){
		fprint(2, " %#llux", bp[1]);
		if((uvlong*)bp[0] <= bp)
			break;
		bp = (uvlong*)bp[0];
	}
	fprint(2, "\n");
	return 0;
}

static void
usage(void)
{
	fprint(2, "usage: %s [-DLMSTnv] [-W secs] [-d drmdebug] [-f selftest-filter] [-p name=value]... [-g #G/n] [-s srvname]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gdir;
	int r, nodisplay, noscreen, loglevel, selftest, noserve, mock, live, nparam, i;
	char *param[64];
	char *filter;
	char *srvname;
	ulong drmdebug;

	gdir = "#G/0";
	nodisplay = 0;
	noscreen = 0;
	selftest = 0;
	mock = 0;
	live = 0;
	nparam = 0;
	filter = nil;
	noserve = 0;
	srvname = "gpu.0";
	drmdebug = 0;
	loglevel = 6;	/* KERN_INFO */
	ARGBEGIN{
	case 'D':
		nodisplay = 1;
		break;
	case 'S':
		noscreen = 1;	/* draw(3)'s screen stays where it is (the firmware's plane, vesa) */
		break;
	case 'd':
		drmdebug = strtoul(EARGF(usage()), nil, 0);
		break;
	case 'T':
		selftest = 1;
		break;
	case 'W':
		proccreate(watchdog, (void*)(uintptr)strtol(EARGF(usage()), nil, 0), 16384);
		break;
	case 'M':
		mock = 1;
		break;
	case 'L':
		live = 1;
		break;
	case 'p':
		if(nparam >= nelem(param))
			sysfatal("too many -p");
		param[nparam++] = EARGF(usage());
		break;
	case 'f':
		filter = EARGF(usage());
		break;
	case 'n':
		noserve = 1;
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	case 'v':
		loglevel = 7;	/* KERN_DEBUG */
		break;
	case 'g':
		gdir = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();
	/* one driver for the device: another one's server means it runs */
	if(!noserve && !selftest){
		char path[64];

		snprint(path, sizeof path, "/srv/%s", srvname);
		if(access(path, AEXIST) == 0)
			sysfatal("%s exists: another driver serves the GPU (aux/intelgpu?)", path);
	}
	atnotify(trapnote, 1);
	threadnotify(trapnote, 1);	/* libthread passes the main proc's other notes (threaddump) only so */
	i915_options(nodisplay, drmdebug, loglevel);
	if((r = kpi_start(gdir)) < 0)
		sysfatal("kpi: %s: error %d", gdir, -r);
	if(selftest){
		r = i915_kpitest();
		threadexitsall(r < 0 ? "selftest failed" : nil);
	}
	for(i = 0; i < nparam; i++)
		if((r = kpi_param_set(param[i])) < 0)
			sysfatal("-p %s: error %d", param[i], -r);
	if(live && (r = kpi_param_set("live_selftests=-1")) < 0)
		sysfatal("live selftests: error %d", -r);
	if((r = kpi_drm_start()) < 0)
		sysfatal("drm: error %d", -r);
	if(mock){
		r = i915_mock(filter);
		fprint(2, "i915: mock selftests: %s (%d)\n", r == 0 ? "ok" : "FAIL", r);
		threadexitsall(r == 0 ? nil : "mock selftests failed");
	}
	r = i915_start();
	if(live){
		/* the probe ran them and let go of the device */
		r = i915_live_result(r);
		fprint(2, "i915: live selftests: %s (%d)\n", r == 0 ? "ok" : "FAIL", r);
		threadexitsall(r == 0 ? nil : "live selftests failed");
	}
	if(r < 0)
		sysfatal("i915: error %d", -r);
	if(!nodisplay && (r = i915_kms_start()) < 0){
		fprint(2, "i915: no display: error %d\n", -r);
		nodisplay = 1;
	}
	/* the screen over the outputs, through the aperture (BAR 2) */
	if(!nodisplay && !noscreen && screenstart(gdir, 2, "i915") < 0)
		fprint(2, "i915: no screen: %r\n");
	fprint(2, "i915: started\n");
	if(!noserve)
		serve(srvname);
	/* the driver's procs (workqueues, irq, timers) go on */
	threadexits(nil);
}
