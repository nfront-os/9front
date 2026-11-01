/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: Linux 6.6's amdgpu driver on the LinuxKPI (sys/src/linux),
 * for gpu(4).  This first part starts the KPI, DRM's core, the DRM
 * scheduler and amdgpu, which probes the device of #G (the video BIOS,
 * the firmware from /lib/firmware/amdgpu, the IP blocks), prints what
 * came up and serves gpu(4)'s files (fs.c).
 */
#include <u.h>
#include <libc.h>
#include <thread.h>
#include <ureg.h>
#include "../../../../libthread/threadimpl.h"	/* threaddump: libthread's procs and threads */
#include <kpi/linux.h>
#include "amdgpu.h"

void	serve(char *srvname);

/* the call chain by frame pointers, for addr2line on the build machine */
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

/* every thread of every proc with its call chain (libthread's lists): who waits where */
static void
threaddump(void)
{
	Proc *p;
	Thread *t;
	uvlong *jb;

	for(p = _threadpq.head; p != nil; p = p->next)
		for(t = p->threads.head; t != nil; t = t->nextt){
			jb = (uvlong*)t->sched;
			fprint(2, "amdgpu thread: pid %d id %d %s state %d pc %#llux; chain",
				p->pid, t->id, t->cmdname ? t->cmdname : "", t->state, jb[1]);
			chain((uvlong*)jb[2]);
		}
}

/* -W s: a thread dump every s seconds, for hangs (a note waits for a running thread) */
static void
watchdog(void *a)
{
	long s = (long)(uintptr)a;

	for(;;){
		sleep(s * 1000);
		fprint(2, "amdgpu: threaddump after %lds\n", s);
		threaddump();
	}
}

/* a trap in any proc (they inherit the handler): the note, the pc, the call chain */
static int
trapnote(void *v, char *note)
{
	struct Ureg *u;

	if(strcmp(note, "threaddump") == 0){
		threaddump();
		return 1;
	}
	if(strncmp(note, "sys:", 4) != 0)
		return 0;
	if(p9_fault(note))
		return 1;
	u = v;
	fprint(2, "amdgpu %d: %s pc %#llux sp %#llux; chain", getpid(), note, u->pc, u->sp);
	chain((uvlong*)u->bp);
	return 0;
}

/* the first device of #G that is AMD's (vendor 1002): its "id" line in ctl */
static char*
findgpu(void)
{
	static char dir[32];
	char buf[512], *p;
	int i, fd, n;

	for(i = 0; i < 4; i++){
		snprint(dir, sizeof dir, "#G/%d", i);
		snprint(buf, sizeof buf, "%s/ctl", dir);
		if((fd = open(buf, OREAD)) < 0)
			break;
		n = read(fd, buf, sizeof buf - 1);
		close(fd);
		if(n <= 0)
			continue;
		buf[n] = 0;
		if((p = strstr(buf, "\nid ")) != nil && strncmp(p + 4, "1002/", 5) == 0)
			return dir;
	}
	return nil;
}

static void
usage(void)
{
	fprint(2, "usage: %s [-DnX] [-W secs] [-d drmdebug] [-l loglevel] [-p name=value]... [-g #G/n (default: the first AMD one)] [-s srvname]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char **argv)
{
	char *gdir, *param[64], *s, *srvname;
	int r, i, nparam, loglevel, noserve, poweroff, nodisplay;
	ulong drmdebug;

	gdir = nil;
	srvname = "gpu.0";
	noserve = 0;
	poweroff = 0;
	nodisplay = 0;
	nparam = 0;
	drmdebug = 0;
	loglevel = 6;	/* KERN_INFO */
	ARGBEGIN{
	case 'd':
		drmdebug = strtoul(EARGF(usage()), nil, 0);
		break;
	case 'l':
		loglevel = atoi(EARGF(usage()));
		break;
	case 'W':
		proccreate(watchdog, (void*)(uintptr)strtol(EARGF(usage()), nil, 0), 16384);
		break;
	case 'p':
		if(nparam >= nelem(param))
			sysfatal("too many -p");
		param[nparam++] = EARGF(usage());
		break;
	case 'g':
		gdir = EARGF(usage());
		break;
	case 'D':
		nodisplay = 1;	/* no outputs, no scanout */
		break;
	case 'n':
		noserve = 1;
		break;
	case 'X':
		poweroff = 1;	/* after the start: the machine goes down */
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();

	if(gdir == nil && (gdir = findgpu()) == nil)
		sysfatal("no AMD GPU in #G");
	if(!noserve){
		char path[64];

		snprint(path, sizeof path, "/srv/%s", srvname);
		if(access(path, AEXIST) == 0)
			sysfatal("%s exists: another driver serves a GPU there (-s)", path);
	}

	atnotify(trapnote, 1);
	threadnotify(trapnote, 1);	/* libthread passes the main proc's other notes (threaddump) only so */
	amdgpu_options(drmdebug, loglevel);
	if((r = kpi_start(gdir)) < 0)
		sysfatal("kpi: %s: error %d", gdir, -r);
	for(i = 0; i < nparam; i++)
		if((r = kpi_param_set(param[i])) < 0)
			sysfatal("-p %s: error %d", param[i], -r);
	if((r = kpi_drm_start()) < 0)
		sysfatal("drm: error %d", -r);
	if((r = amdgpu_start()) < 0)
		sysfatal("amdgpu: error %d", -r);
	s = amdgpu_status();
	fprint(2, "amdgpu: started\n%s", s != nil ? s : "");
	amdgpu_freestr(s);
	if(!nodisplay && (r = amdgpu_kms_start()) < 0)
		fprint(2, "amdgpu: no display: error %d\n", -r);
	if(poweroff)
		amdgpu_test_poweroff();
	if(!noserve)
		serve(srvname);
	/* the driver's procs (workqueues, irq, timers) go on */
	threadexits(nil);
}
