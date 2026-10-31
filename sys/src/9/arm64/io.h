enum {
	IRQfiq		= -1,

	PPI		= 16,
	SPI		= 32,

	IRQcntvns	= PPI+11,

	IRQipi		= 1,		/* NIX: SGI, inter-core call (port/nix.c) */
	IRQpmc		= 2,		/* pmc(3): SGI, performance counter request (port/devpmc.c) */

	IRQuart		= SPI+1,

	IRQpci1		= SPI+3,
	IRQpci2		= SPI+4,
	IRQpci3		= SPI+5,
	IRQpci4		= SPI+6,
};

#define BUSUNKNOWN (-1)
#define PCIWINDOW	0
#define	PCIWADDR(x)	(PADDR(x)+PCIWINDOW)
