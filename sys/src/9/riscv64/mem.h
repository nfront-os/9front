/*
 * Memory and machine-specific definitions.  Used in C and assembler.
 */
#define KiB		1024u			/* Kibi 0x0000000000000400 */
#define MiB		1048576u		/* Mebi 0x0000000000100000 */
#define GiB		1073741824u		/* Gibi 000000000040000000 */

/*
 * Sv39 paging: three levels of 512 entries,
 * leaves of 4K (level 0), 2M (level 1) or 1G (level 2).
 */
#define	PGSHIFT		12		/* log(BY2PG) */
#define	BY2PG		(1ULL<<PGSHIFT)	/* bytes per page */
#define	ROUND(s, sz)	(((s)+(sz-1))&~(sz-1))
#define	PGROUND(s)	ROUND(s, BY2PG)

#define PTSHIFT		9
#define PTLEVELS	3
#define PTLX(v, l)	((((uintptr)(v)) >> (PGSHIFT + (l)*PTSHIFT)) & ((1 << PTSHIFT)-1))
#define PGLSZ(l)	(1ULL << (PGSHIFT + (l)*PTSHIFT))

#define	MAXMACH		16			/* max # cpus system can run */
#define	MACHSIZE	(16*KiB)		/* Mach and its stack */

#define KSTACK		(16*KiB)
#define STACKALIGN(sp)	((sp) & ~15)		/* bug: assure with alloc */

/*
 * a trap frame on the stack (l.s): two slots for trap()'s argument
 * and link, the Ureg (pc, r1-r31, status, ie, cause, tval, curmode),
 * padding to 16 bytes
 */
#define UREG		16
#define UREGSIZE	(37*8)
#define TRAPFRAME	320

/*
 * the kernel half of the address space: KZERO maps physical memory
 * one to one (KADDR), the kernel is linked at KTZERO, where the
 * firmware (OpenSBI) loads it; device registers are in VMAP.
 */
#define	KZERO		(0xFFFFFFC000000000ULL)	/* 0x0000000000 - */
#define KLIMIT		(KZERO+128ULL*GiB)	/* - 0x2000000000 */
#define VMAP		(KLIMIT)		/* vmap(): 1GB */
#define VMAPSIZE	GiB

#define PHYSKTZERO	0x80200000		/* where the firmware puts the kernel */
#define	KTZERO		(KZERO+PHYSKTZERO)	/* kernel text start */

/*
 * after the kernel's end: the Machs, each with its stack, then the
 * page tables built by l.s (the boot root, the level 1 table of VMAP)
 */
#define MACHBASE	(PGROUND((uintptr)end))
#define MACHADDR(n)	(MACHBASE+(n)*MACHSIZE)
#define L1		MACHADDR(MAXMACH)		/* kernel root page table */
#define VMAPL1		(L1+BY2PG)			/* level 1 table of VMAP */
#define KMEMBASE	(L1+2*BY2PG)			/* first free page */

#define BOOTARGSLEN	(4*KiB)

#define	UZERO		0ULL			/* user segment */
#define	UTZERO		(UZERO+BY2PG)		/* user text start; jl's default */
#define	USTKTOP		(0x4000000000ULL-0x10000)	/* user segment end +1 */
#define	USTKSIZE	(16*1024*1024)		/* user stack size */

#define BLOCKALIGN	64			/* only used in allocb.c */

/*
 * Sizes
 */
#define BI2BY		8			/* bits per byte */
#define BY2SE		8
#define BY2WD		8
#define BY2V		8			/* only used in xalloc.c */

#define	PTEMAPMEM	(1024*1024)
#define	PTEPERTAB	(PTEMAPMEM/BY2PG)
#define	SEGMAPSIZE	8192
#define	SSEGMAPSIZE	16
#define	PPN(x)		((x)&~(BY2PG-1))

/*
 * page table entries
 */
#define	PTEVALID	(1<<0)
#define PTEREAD		(1<<1)
#define PTEWRITE	(1<<2)
#define PTEEXEC		(1<<3)
#define PTEUSER		(1<<4)
#define PTEGLOBAL	(1<<5)
#define PTEACCESS	(1<<6)
#define PTEDIRTY	(1<<7)
#define PTERONLY	0			/* putmmu: PTEWRITE clear */
#define PTECACHED	0
#define PTEUNCACHED	0			/* by PMA on qemu virt */
#define PTENOEXEC	(1<<8)			/* putmmu: PTEEXEC clear; a software bit */
#define PTEKERNEL	(PTEGLOBAL|PTEACCESS|PTEDIRTY)
#define PTEPPNSHIFT	10			/* pa>>PGSHIFT is at this bit */
#define PTE2PA(pte)	((((pte)>>PTEPPNSHIFT)&((1ULL<<44)-1))<<PGSHIFT)
#define PA2PTE(pa)	(((uvlong)(pa)>>PGSHIFT)<<PTEPPNSHIFT)
#define PTELEAF		(PTEREAD|PTEWRITE|PTEEXEC)

/*
 * Physical machine information from here on.
 */
#define	PHYSDRAM	0x80000000

#define ELFMACHINE	243			/* EM_RISCV, for ../port/sysexecelf.c */

#define MIN(a, b)	((a) < (b)? (a): (b))
#define MAX(a, b)	((a) > (b)? (a): (b))
