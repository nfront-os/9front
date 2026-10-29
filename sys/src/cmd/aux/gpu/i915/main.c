/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915: Linux 6.6's i915 driver on the LinuxKPI (sys/src/linux),
 * as a file server of gpu(4).  This first part starts the KPI, DRM's
 * core and i915, which probes the device of #G; the file server over
 * the driver's ioctls follows.
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include <ureg.h>
#include <kpi/linux.h>
#include "i915.h"

void	serve(char *srvname);

/*
 * a trap in any proc (they inherit the handler): the note, the pc and
 * the call chain by frame pointers to standard error, that is into the
 * log; the addresses are looked up on the build machine (addr2line)
 */
static int
trapnote(void *v, char *note)
{
	struct Ureg *u;
	uvlong *bp;
	int i;

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
	fprint(2, "usage: %s [-DLMTnv] [-d drmdebug] [-f selftest-filter] [-p name=value]... [-g #G/n] [-s srvname]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gdir;
	int r, nodisplay, loglevel, selftest, noserve, mock, live, nparam, i;
	char *param[64];
	char *filter;
	char *srvname;
	ulong drmdebug;

	gdir = "#G/0";
	nodisplay = 0;
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
	case 'd':
		drmdebug = strtoul(EARGF(usage()), nil, 0);
		break;
	case 'T':
		selftest = 1;
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
	if(!nodisplay && (r = i915_kms_start()) < 0)
		fprint(2, "i915: no display: error %d\n", -r);
	fprint(2, "i915: started\n");
	if(!noserve)
		serve(srvname);
	/* the driver's procs (workqueues, irq, timers) go on */
	threadexits(nil);
}
