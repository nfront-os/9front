/*
 * qemu's riscv64 virt machine (io.h): devices on pci (pcivirt.c), the
 * console a 16550 with byte registers
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

Soc soc = {
	"qemu virt",
	PHYSPLIC, 96, 1, 2,	/* hart h's supervisor context 2h+1 */
	PHYSUART, 0, 1, IRQuart, 3686400,
};
