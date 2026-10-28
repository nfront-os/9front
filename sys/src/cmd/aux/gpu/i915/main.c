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
#include <kpi/linux.h>
#include "i915.h"

static void
usage(void)
{
	fprint(2, "usage: %s [-g #G/n]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gdir;
	int r;

	gdir = "#G/0";
	ARGBEGIN{
	case 'g':
		gdir = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();
	if((r = kpi_start(gdir)) < 0)
		sysfatal("kpi: %s: error %d", gdir, -r);
	if((r = kpi_drm_start()) < 0)
		sysfatal("drm: error %d", -r);
	if((r = i915_start()) < 0)
		sysfatal("i915: error %d", -r);
	fprint(2, "i915: started\n");
	/* the driver's procs (workqueues, irq, timers) go on */
	threadexits(nil);
}
