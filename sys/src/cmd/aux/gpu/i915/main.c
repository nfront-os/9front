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
	fprint(2, "usage: %s [-DTnv] [-d drmdebug] [-g #G/n] [-s srvname]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gdir;
	int r, nodisplay, loglevel, selftest, noserve;
	char *srvname;
	ulong drmdebug;

	gdir = "#G/0";
	nodisplay = 0;
	selftest = 0;
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
		r = i915_selftest();
		threadexitsall(r < 0 ? "selftest failed" : nil);
	}
	if((r = kpi_drm_start()) < 0)
		sysfatal("drm: error %d", -r);
	if((r = i915_start()) < 0)
		sysfatal("i915: error %d", -r);
	fprint(2, "i915: started\n");
	if(!noserve)
		serve(srvname);
	/* the driver's procs (workqueues, irq, timers) go on */
	threadexits(nil);
}
