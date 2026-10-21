/*
 * StarFive VisionFive 2 (JH7110; Linux's jh7110.dtsi): harts 1-4
 * (U74, the CIP-1200 erratum as the FU740), hart 0 the S7 monitor core
 * with a machine context only; ram from 0x40000000 (the CONF's
 * #physdram). The console is uart0, DesignWare's 16550 with 32-bit
 * registers 4 apart on a 24MHz clock. OpenSBI and u-boot (the board's
 * flash) start the kernel (9vf.u, sys/src/boot/vf).
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

Soc soc = {
	"starfive visionfive 2",
	0x0C000000, 137, 0, 2,	/* hart h (1-4)'s supervisor context 2h */
	0x10000000, 2, 4, 32, 0,	/* uart0: u-boot set its divisor */
	{ 0 },			/* gmac0, gmac1 (DesignWare): no driver yet */
	{ 0 },
	0,
	0, 0,			/* dw_mmc: no driver yet */
	1,			/* CIP-1200 */
	1,			/* DesignWare uart */
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
