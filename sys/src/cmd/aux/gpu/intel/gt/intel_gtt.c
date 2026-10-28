/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the private PAT, the scratch page and tables.
 *
 * Follows Linux 6.6 drivers/gpu/drm/i915/gt/intel_gtt.c (the code first from v4.4);
 * the notices are in NOTICE.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

/*
 * where nothing is mapped the entries point to a scratch page (PTEs)
 * or a page table of only those (PDEs), as i915 has it: stray GPU
 * accesses land there, not at physical address 0
 */
uvlong scratchpte, scratchgtt;
/*
 * the scratch tables below each level (0: the PT, 1: a PD of PT
 * pointers, 2: a PDP of PD pointers), and the entries a new table of
 * a level is filled with (0: PTEs to the scratch page, 1-3: pointers
 * to the scratch table below)
 */
uvlong scratchtab[3], scratchfill[4];

/*
 * the private PAT: index 0 stays what the firmware left (the
 * display's mappings use it), 3 uncached, 4-7 write-back in the LLC
 * (i915's bdw_setup_private_ppat)
 */
void
ppatinit(void)
{
	u32int lo, hi;

	lo = rd(GEN8_PRIVATE_PAT_LO);
	lo = (lo & 0xFF) | 0x09<<8 | 0x0A<<16 | 0x00<<24;	/* 1 WC llc+ellc, 2 WT llc+ellc, 3 UC */
	hi = 0x07<<0 | 0x17<<8 | 0x27<<16 | 0x37<<24;		/* 4-7 WB llc, age 0-3 */
	wr(GEN8_PRIVATE_PAT_LO, lo);
	wr(GEN8_PRIVATE_PAT_HI, hi);
	DBG(2, "ppat %#.8ux %#.8ux\n", rd(GEN8_PRIVATE_PAT_HI), rd(GEN8_PRIVATE_PAT_LO));
}

/*
 * the scratch page and the scratch tables, after the render state's
 * page: a PT of PTEs to the scratch page, a PD of pointers to that PT,
 * a PDP of pointers to that PD
 */
void
scratchinit(void)
{
	uvlong *t, pa, fill;
	u32int *p;
	int i, l;

	p = drv + (nelem(eng)+1)*1024;
	pa = drvpa + (nelem(eng)+1)*4096;
	memset(p, 0, 4096);
	flushcache(p, 4096);
	scratchpte = pa | GEN8_PAGE_PRESENT | GEN8_PAGE_RW | (uncached ? PPAT_UNCACHED : PPAT_CACHED);
	scratchgtt = pa | GEN8_PAGE_PRESENT | (uncached ? PPAT_UNCACHED : PPAT_CACHED);
	fill = scratchpte;
	scratchfill[0] = fill;
	for(l = 0; l < 3; l++){
		t = (uvlong*)(drv + (nelem(eng)+2+l)*1024);
		for(i = 0; i < 512; i++)
			t[i] = fill;
		flushcache(t, 4096);
		scratchtab[l] = drvpa + (nelem(eng)+2+l)*4096;
		fill = scratchtab[l] | GEN8_PAGE_PRESENT | GEN8_PAGE_RW | (uncached ? PPAT_UNCACHED : Pdecached);
		scratchfill[l+1] = fill;
	}
}
