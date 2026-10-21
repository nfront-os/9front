/*
 * StarFive VisionFive 2 (JH7110; the device tree jh7110.dtsi, GPL-2.0 OR MIT): harts 1-4
 * (U74, the CIP-1200 erratum as the FU740), hart 0 the S7 monitor core
 * with a machine context only; ram from 0x40000000 (the CONF's
 * #physdram). The console is uart0, DesignWare's 16550 with 32-bit
 * registers 4 apart on a 24MHz clock. OpenSBI and u-boot (the board's
 * flash) start the kernel (9vf.u, sys/src/boot/vf); sd card and emmc
 * through sddwmmc.c, the two ethernets etherdwmac.c.
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
	{ 0x16030000, 0x16040000 },	/* gmac0, gmac1 (DesignWare, etherdwmac.c) */
	{ 7, 78 },
	0,
	0, 0,			/* no sdhci */
	1,			/* CIP-1200 */
	1,			/* DesignWare uart */
	{ 0x16020000, 0x16010000 },	/* mmc1 the sd card slot, mmc0 emmc (dw_mmc) */
	{ 75, 74 },
	50000000,		/* their card clock (the device tree's assigned rate) */
	{ { 1500, 1500, 6 },	/* phy0, phy1: visionfive-2-v1.3b.dts */
	  { 300, 0, 2 } },
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
