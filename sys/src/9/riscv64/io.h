/*
 * qemu's riscv64 virt machine
 */
enum {
	PHYSTEST	= 0x00100000,	/* sifive_test: poweroff, reset */
	PHYSRTC		= 0x00101000,	/* goldfish rtc */
	PHYSPLIC	= 0x0C000000,
	PHYSUART	= 0x10000000,	/* ns16550a */
	PHYSECAM	= 0x30000000,
	ECAMSIZE	= 0x10000000,
	PHYSPCIMMIO	= 0x40000000,
	PCIMMIOSIZE	= 0x40000000,

	IRQuart		= 10,
	IRQrtc		= 11,
	IRQpci1		= 32,
	IRQpci2		= 33,
	IRQpci3		= 34,
	IRQpci4		= 35,
};

#define BUSUNKNOWN (-1)
#define PCIWINDOW	0
#define	PCIWADDR(x)	(PADDR(x)+PCIWINDOW)
