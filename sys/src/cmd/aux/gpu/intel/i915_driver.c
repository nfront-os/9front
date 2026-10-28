/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the device: PCI config, MMIO, GTT, the driver's pages, bring-up.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/i915_driver.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "i915_drv.h"

int gctlfd = -1, irqfd = -1;
u32int *mmio;
uvlong mmiosize;
uvlong drvpa;
u32int *drv;
ulong drvgtt;

ulong
cfg16(int off)
{
	char buf[1024], path[64], *s, *f[2];
	uchar b[2];
	int fd;

	if((s = ctlline("pci", buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 1)
		sysfatal("%s: no pci line", gdir);
	snprint(path, sizeof path, "#$/pci/%sraw", f[0]);
	if((fd = open(path, OREAD)) < 0 || pread(fd, b, 2, off) != 2)
		sysfatal("%s: %r", path);
	close(fd);
	return b[0] | b[1]<<8;
}

void
devinit(void)
{
	char buf[1024], path[64], *s, *f[5];
	uvlong gttsize;
	ulong g;
	int s0, i, n;

	if((s = ctlline("id", buf, sizeof buf)) == nil || strncmp(s, "8086/", 5) != 0)
		sysfatal("%s is %s, not Intel's", gdir, s ? s : "?");
	if((s = ctlline("bar0", buf, sizeof buf)) == nil || tokenize(s, f, nelem(f)) < 3)
		sysfatal("%s: no bar0", gdir);
	mmiosize = strtoull(f[1], nil, 0);
	if((mmio = segattach(0, f[2], nil, mmiosize)) == (void*)-1)
		sysfatal("segattach %s: %r", f[2]);
	gtt = (uvlong*)((uchar*)mmio + mmiosize/2);
	g = cfg16(Pcigmch);
	s0 = g>>6 & 3;
	gttsize = s0 ? (1ULL<<s0)<<20 : 0;
	nent = gttsize/8;
	if(nent*4096 <= Gttlow + 64*1024*1024)
		sysfatal("GTT of %lld entries: too small", nent);
	holes = emalloc9p(sizeof *holes);
	holes->next = nil;
	holes->start = Gttlow;
	holes->len = nent*4096 - Gttlow;
	if(nent*4096 > 0xFFFFFFFFULL)
		holes->len = 0xFFFFF000UL - Gttlow;	/* addresses in one dword (GTT up to 4 GB on gen 8) */

	snprint(path, sizeof path, "%s/ctl", gdir);
	if((gctlfd = open(path, ORDWR)) < 0)
		sysfatal("open %s: %r", path);
	if(fprint(gctlfd, "bme on") < 0)
		sysfatal("bme on: %r");
	lock(&fwlk);
	fwheld = 0;
	fwon();
	unlock(&fwlk);
	if((rd(Forcewakeack) & 1) == 0)
		sysfatal("forcewake: no ack (%#ux)", rd(Forcewakeack));
	if(!uncached)
		ppatinit();

	/*
	 * the driver's own pages: the engines' status pages, the null
	 * render state, a scratch page and the scratch tables (PT, PD, PDP)
	 */
	n = nelem(eng)+5;
	if((drvpa = fixedseg("gpu0drv", Drvva, n*4096, 0700)) == 0)
		sysfatal("#g/gpu0drv: %r");
	if((drv = segattach(0, "gpu0drv", nil, n*4096)) == (void*)-1)
		sysfatal("segattach gpu0drv: %r");
	scratchinit();
	/* our part of the GTT to the scratch page, as i915's gen8_ggtt_clear_range */
	gttunmap(Gttlow, nent*4096 - Gttlow);
	if((drvgtt = gttalloc(n*4096)) == 0)
		sysfatal("no GTT for the status pages");
	gttmap(drvgtt, drvpa, nil, n*4096);
	rsinit();
	/* the contexts' pages, here: the procs forked later see only what is attached now */
	if((ctxpoolpa = fixedseg("gpu0ctx", Ctxva, Nctx*ctxslot(), 0700)) == 0)
		sysfatal("#g/gpu0ctx: %r");
	if((ctxpool = segattach(0, "gpu0ctx", nil, Nctx*ctxslot())) == (void*)-1)
		sysfatal("segattach gpu0ctx: %r");
	if((ctxpoolgtt = gttalloc(Nctx*ctxslot())) == 0)
		sysfatal("no GTT for the contexts");
	gttmap(ctxpoolgtt, ctxpoolpa, nil, Nctx*ctxslot());
	if(!global){
		if((ptpa = fixedseg("gpu0pt", Ptva, Ptpages*4096, 0700)) == 0)
			sysfatal("#g/gpu0pt: %r");
		if((ptpool = segattach(0, "gpu0pt", nil, Ptpages*4096)) == (void*)-1)
			sysfatal("segattach gpu0pt: %r");
	}
	/* a previous driver may have left an engine hung: reset both first */
	for(i = 0; i < nelem(eng); i++){
		wr(eng[i].base+Rresetctl, masked(Resetrequest, 1));
		wr(Gdrst, eng[i].resetbit);
		sleep(10);
		wr(eng[i].base+Rresetctl, masked(Resetrequest, 0));
	}
	sseuinit();
	if(!noclockgating)
		gtinit();
	for(i = 0; i < nelem(eng); i++)
		engineinit(&eng[i]);

	if(!poll){
		snprint(path, sizeof path, "%s/irq", gdir);
		if((irqfd = open(path, OREAD)) < 0)
			fprint(2, "%s: no interrupts, polling: %r\n", argv0);
	}
	if(!nopm)
		rpsinit();
	irqinit();
	dispinit();
}
