/*
 * Microchip PolarFire SoC Icicle Kit (mpfs, Linux's mpfs-icicle-kit.dts;
 * qemu's microchip-icicle-kit): harts 1-4 (U54) run the kernel, hart 0
 * (E51) is the monitor core with only a machine context in the plic;
 * the console is mmuart1, a 16550 with 32-bit registers 4 apart.
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

Soc soc = {
	"polarfire icicle",
	0x0C000000, 187, 0, 2,	/* hart h (1-4)'s supervisor context 2h */
	0x20100000, 2, 4, 91, 150000000,	/* mmuart1 (stdout-path serial1) */
	{ 0x20110000, 0x20112000 },	/* mac0, mac1 (GEM); plan9.ini ether0=port= picks */
	{ 64, 70 },			/* their first (queue 0) interrupts */
	1,				/* the vsc8662 phys on sgmii */
	0x20008000+0x200, 88,		/* the Cadence SD4HC's standard registers (SRS) */
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
