/*
 * RISC-V supervisor definitions, for C and assembler;
 * from Geoff Collyer's 9k riscv64 port (rv/riscv64l.h).
 */

/* CSRs */
#define FFLAGS		0x001
#define FRM		0x002
#define FCSR		0x003

#define SSTATUS		0x100
#define SIE		0x104
#define STVEC		0x105
#define SCOUNTEREN	0x106
#define SSCRATCH	0x140
#define SEPC		0x141
#define SCAUSE		0x142
#define STVAL		0x143
#define SIP		0x144
#define SATP		0x180

#define CYCLO		0xc00
#define TIMELO		0xc01
#define INSTRETLO	0xc02

/* sstatus */
#define Sie		(1<<1)		/* supervisor-mode intr enable */
#define Spie		(1<<5)		/* previous Sie */
#define Spp		(1<<8)		/* previous priv. mode (super) */
#define Fsshft		13
#define Fsoff		(0<<Fsshft)	/* fp: disabled, to detect uses */
#define Fsinitial	(1<<Fsshft)	/* fp: on but initialised */
#define Fsclean		(2<<Fsshft)	/* fp: loaded but not changed */
#define Fsdirty		(3<<Fsshft)	/* fp: changed */
#define Fsst		(3<<Fsshft)	/* fpu state mask */
#define Sum		(1<<18)		/* super access to user memory */
#define Mxr		(1<<19)		/* make executable readable */

/* scause: interrupts (with Rv64intr), then exceptions */
#define Rv64intr	(1ULL<<63)

#define Supswintr	1		/* interprocessor interrupts */
#define Suptmrintr	5		/* clock interrupts */
#define Supextintr	9		/* plic interrupts */

#define Instaddralign	0
#define Instaccess	1
#define Illinst		2
#define Breakpt		3
#define Loadaddralign	4
#define Loadaccess	5
#define Storeaddralign	6
#define Storeaccess	7
#define Envcalluser	8		/* system call from user mode */
#define Envcallsup	9
#define Instpage	12		/* page faults */
#define Loadpage	13
#define Storepage	15

/* sie and sip bits */
#define Ssie		(1<<Supswintr)
#define Stie		(1<<Suptmrintr)
#define Seie		(1<<Supextintr)

/* satp */
#define Satpasidshft	44
#define Sv39		(8ULL<<60)

/* SBI extensions and functions */
#define SBILEGACYTIMER	0x00
#define SBILEGACYPUTC	0x01
#define SBIBASE		0x10
#define SBITIME		0x54494D45	/* "TIME" */
#define SBIIPI		0x735049	/* "sPI" */
#define SBIRFENCE	0x52464E43	/* "RFNC" */
#define SBIHSM		0x48534D	/* "HSM" */
#define SBISRST		0x53525354	/* "SRST" */

#define SRSTSHUT	0
#define SRSTCOLD	1
