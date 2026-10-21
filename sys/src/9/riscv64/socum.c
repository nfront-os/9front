/*
 * SiFive HiFive Unmatched (FU740) and qemu's sifive_u (FU540): harts
 * 1-4 (U54), hart 0 the S7/E51 monitor core with a machine context
 * only; the same registers, other interrupts (the device tree's,
 * bootargs.c). The console is uart0, SiFive's (uartsifive.c), set up
 * by the firmware (no clock here: the divisor stays).
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

Soc soc = {
	"sifive fu540/fu740",
	0x0C000000, 70, 0, 2,	/* hart h (1-4)'s supervisor context 2h */
	0x10010000, 0, 4, 4, 0,	/* uart0 */
	{ 0x10090000 },		/* the GEM (gigabit ethernet) */
	{ 53 },
	0,			/* the phy on (r)gmii */
};

/* no pci (yet): plic.c's intrenable sees no BusPCI devices */
void
pciintrenable(int, void (*)(Ureg*, void*), void*)
{
}

void
pciintrdisable(int, void (*)(Ureg*, void*), void*)
{
}
