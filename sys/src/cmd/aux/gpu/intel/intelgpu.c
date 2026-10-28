/*
 * SPDX-License-Identifier: MIT
 *
 * Parts follow Linux's i915 driver (MIT); their copyright and
 * permission notices are in NOTICE.
 *
 * intelgpu: user-space driver for Intel GPUs of generation 8
 * (Broadwell) over #G (gpu(3)), serving gpu(4)'s files: contexts
 * with an address space of their own (PPGTT) and buffer objects in
 * it, batch buffers on the render (RCS) and blitter (BCS) engines
 * through execlists (a logical ring context per context and engine,
 * as Linux's i915 has it on gen 8), fences from the contexts'
 * sequence numbers and the user and context switch interrupts (MSI).
 * The display stays as the firmware or aux/vga set it up.
 *
 *	/dev/gpu/0/ctl			the visible plane: plane p gpuaddr stride bpp w h
 *	/dev/gpu/0/status		device, engines, memory, contexts
 *	/dev/gpu/0/clone		a new context, ctx/n
 *	/dev/gpu/0/ctx/n/ctl		bo size, free id, map plane
 *	/dev/gpu/0/ctx/n/submit		bo offset [rcs|bcs]
 *	/dev/gpu/0/ctx/n/fence		as gpu(4)
 *
 * A buffer object is a #g segment of pinned pages (sticky; their
 * addresses from #g/seg/pa), mapped into the context's address space; the client
 * attaches it and writes GPU addresses into its batches itself (no
 * relocations).  A context's address space is 4 GB in three levels
 * (gen 8's 32-bit PPGTT: four page directories, which the context
 * image hands the engine at every switch); with -G all contexts share
 * the global GTT instead, without isolation.
 *
 * An engine runs one context at a time: a context with work waits
 * in the engine's queue, goes into the execlist submit port (ELSP)
 * when the engine is idle, and leaves it when the context status
 * buffer says it is done (no lite restore).
 *
 * usage: aux/intelgpu [-dpGU] [-g #G/n] [-s srvname]
 *
 * Register offsets and command formats as in Intel's PRMs (Broadwell,
 * vol. 2 and 5) and Linux's i915 for gen 8.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

char *gdir = "#G/0";
int debug, poll, uncached, global;
int noclockgating, norcsinit;	/* -C, -R: to find what hangs a machine */
int nopm;		/* -P: no RC6, no RPS, forcewake held */
int legacy32;		/* -3: 32-bit address spaces (three levels) */
QLock lk;		/* engines, fences, the GTT */
Rendez done;		/* fences completed */
QLock ctxlock;		/* contexts, objects */

void
usage(void)
{
	fprint(2, "usage: %s [-3dpCGPRU] [-g #G/n] [-s srvname]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *srvname;
	int i;

	srvname = "gpu.0";
	ARGBEGIN{
	case 'd':
		debug++;
		break;
	case 'g':
		gdir = EARGF(usage());
		break;
	case '3':
		legacy32 = 1;
		break;
	case 'C':
		noclockgating = 1;
		break;
	case 'G':
		global = 1;
		break;
	case 'P':
		nopm = 1;
		break;
	case 'R':
		norcsinit = 1;
		break;
	case 'p':
		poll = 1;
		break;
	case 's':
		srvname = EARGF(usage());
		break;
	case 'U':
		uncached = 1;
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();

	done.l = &lk;
	i915_driver_probe();
	for(i = 0; i < nelem(eng); i++)
		fprint(2, "%s: %s %s\n", argv0, eng[i].name, eng[i].ok ? "ok" : "off");

	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		irqproc();
		exits(nil);
	}
	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		tickproc();
		exits(nil);
	}
	switch(rfork(RFPROC|RFMEM|RFNOWAIT)){
	case -1:
		sysfatal("rfork: %r");
	case 0:
		fwproc();
		exits(nil);
	}

	fs.open = fsopen;
	fs.read = fsread;
	fs.write = fswrite;
	fs.destroyfid = fsdestroyfid;
	fs.end = fsend;
	mktree();
	postmountsrv(&fs, srvname, "/dev", MAFTER);
	exits(nil);
}
