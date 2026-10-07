/*
 * ELF program loading for sysexec() (see sysproc.c).
 *
 * Independent implementation written against the public ELF32/ELF64
 * ABI (System V Application Binary Interface, Intel386/AMD64 processor
 * supplements) -- not adapted from any other kernel's loader. Lives in
 * port/ and dispatches on EI_CLASS at runtime so the same code serves
 * both pc (ELFCLASS32/EM_386) and pc64 (ELFCLASS64/EM_X86_64); the two
 * classes' Ehdr/Phdr layouts differ in field order, not just width
 * (p_flags moves from offset 4 in Elf64_Phdr to offset 24 in
 * Elf32_Phdr), so each field is read at its class-specific offset
 * rather than just changing an integer width.
 *
 * Only static, non-dynamically-linked images are supported: ET_EXEC,
 * and ET_DYN when it carries no PT_INTERP (a "static-pie" binary,
 * which self-relocates its own R_*_RELATIVE entries from its startup
 * code before calling main -- the kernel's only job is to load every
 * PT_LOAD segment at a consistent base and start at base+e_entry).
 * Real dynamic linking (PT_INTERP, a userspace ld.so, shared objects)
 * is explicitly out of scope.
 */

#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

enum {
	EI_CLASS	= 4,
	EI_DATA		= 5,
	ELFCLASS32	= 1,
	ELFCLASS64	= 2,
	ELFDATA2LSB	= 1,

	ET_EXEC		= 2,
	ET_DYN		= 3,

	EM_386		= 3,
	EM_X86_64	= 62,

	PT_LOAD		= 1,
	PT_INTERP	= 3,

	PF_X		= 1,
	PF_W		= 2,
	PF_R		= 4,

	Ehdrsz32	= 52,
	Ehdrsz64	= 64,
	Phdrsz32	= 32,
	Phdrsz64	= 56,
	Maxphnum	= 16,	/* generous; typical static(-pie) binaries have 2-4 */

	/* ET_DYN load base: arbitrary but fixed (no ASLR), same convention
	 * a.out binaries already load text at */
	Pieload		= UTZERO,
};

static u16int
get16(uchar *p)
{
	return p[0] | p[1]<<8;
}

static u32int
get32(uchar *p)
{
	return (u32int)p[0] | (u32int)p[1]<<8 | (u32int)p[2]<<16 | (u32int)p[3]<<24;
}

static u64int
get64(uchar *p)
{
	return (u64int)get32(p) | (u64int)get32(p+4)<<32;
}

int
elf64magic(uchar *hdr)
{
	return hdr[0] == 0x7f && hdr[1] == 'E' && hdr[2] == 'L' && hdr[3] == 'F';
}

/*
 * hdr[hdrlen] is the start of the file, already read by the caller
 * (sysexec()'s #! / magic-sniffing read). Validate the ELF64 header,
 * read the program header table from tc, classify each PT_LOAD entry
 * into an ElfSeg the caller can hand straight to newseg()/attachseg(),
 * and return the count. Raises error(Ebadexec) on anything malformed
 * or unsupported rather than returning a failure code, matching the
 * rest of sysexec()'s error-handling style.
 */
long
elf64load(Chan *tc, uchar *hdr, int hdrlen, uintptr *entryp, ElfSeg *seg, long maxseg)
{
	uchar phbuf[Maxphnum*Phdrsz64], *ph;
	uintptr base, entry, lo, hi;
	ulong phoff, phentsize, phnum, filesz, memsz, pgoff;
	uvlong vaddr;
	int i, class, flags, type, n, nseg, haveinterp, phdrsz, ehdrsz;

	if(hdrlen < Ehdrsz32)
		error(Ebadexec);
	class = hdr[EI_CLASS];
	if(hdr[EI_DATA] != ELFDATA2LSB)
		error(Ebadexec);
	switch(class){
	default:
		error(Ebadexec);
	case ELFCLASS32:
		ehdrsz = Ehdrsz32;
		phdrsz = Phdrsz32;
		if(get16(hdr+18) != EM_386)
			error(Ebadexec);
		break;
	case ELFCLASS64:
		ehdrsz = Ehdrsz64;
		phdrsz = Phdrsz64;
		if(get16(hdr+18) != EM_X86_64)
			error(Ebadexec);
		break;
	}
	if(hdrlen < ehdrsz)
		error(Ebadexec);

	type = get16(hdr+16);
	if(type != ET_EXEC && type != ET_DYN)
		error(Ebadexec);
	base = type == ET_DYN? Pieload: 0;

	if(class == ELFCLASS64){
		entry = get64(hdr+24) + base;
		phoff = get64(hdr+32);
		phentsize = get16(hdr+54);
		phnum = get16(hdr+56);
	} else {
		entry = get32(hdr+24) + base;
		phoff = get32(hdr+28);
		phentsize = get16(hdr+42);
		phnum = get16(hdr+44);
	}

	if(phentsize < phdrsz || phnum == 0 || phnum > Maxphnum)
		error(Ebadexec);

	n = devtab[tc->type]->read(tc, phbuf, phnum*phdrsz, phoff);
	if(n != phnum*phdrsz)
		error(Ebadexec);

	haveinterp = 0;
	nseg = 0;
	for(i = 0; i < phnum; i++){
		ph = phbuf + i*phdrsz;		/* only the first phdrsz bytes of each entry are read */

		switch(get32(ph+0)){
		default:
			continue;
		case PT_INTERP:
			haveinterp = 1;
			continue;
		case PT_LOAD:
			break;
		}

		if(class == ELFCLASS64){
			flags = get32(ph+4);
			pgoff = get64(ph+8);
			vaddr = get64(ph+16) + base;
			filesz = get64(ph+32);
			memsz = get64(ph+40);
		} else {
			flags = get32(ph+24);
			pgoff = get32(ph+4);
			vaddr = get32(ph+8) + base;
			filesz = get32(ph+16);
			memsz = get32(ph+20);
		}

		if(memsz == 0 || filesz > memsz)
			error(Ebadexec);
		if((flags & (PF_W|PF_X)) == (PF_W|PF_X))
			error(Ebadexec);	/* refuse writable+executable segments */
		if((flags & PF_R) == 0)
			error(Ebadexec);	/* a loadable segment must be readable */

		lo = vaddr & ~(uintptr)(BY2PG-1);
		hi = vaddr + memsz;

		if(nseg >= maxseg)
			error(Ebadexec);

		if(flags & PF_X)
			seg[nseg].type = SG_TEXT | SG_RONLY;
		else if(flags & PF_W)
			seg[nseg].type = SG_DATA;
		else
			seg[nseg].type = SG_TEXT | SG_RONLY | SG_NOEXEC;

		seg[nseg].va = lo;
		seg[nseg].fileoff = pgoff - (vaddr - lo);
		seg[nseg].flen = filesz + (vaddr - lo);
		seg[nseg].pages = (PGROUND(vaddr+filesz) - lo) >> PGSHIFT;
		nseg++;

		/* zero-fill tail (p_memsz > p_filesz): bss for a data segment,
		 * or just unbacked zero pages tacked onto a text/rodata one.
		 * A writable segment always gets one, even if empty (bss fits
		 * in the last data page, or there is none): like a.out's
		 * newseg(SG_BSS, abss, ...) in sysexec(), brk()/sbrk() grow
		 * BSEG, and without it every malloc() fails with Ebadarg */
		if(memsz > filesz || (flags & PF_W) != 0){
			if(nseg >= maxseg)
				error(Ebadexec);
			lo = PGROUND(vaddr+filesz);
			if(lo < hi || (flags & PF_W) != 0){
				if(hi < lo)
					hi = lo;
				seg[nseg].type = SG_BSS;
				seg[nseg].va = lo;
				seg[nseg].fileoff = 0;
				seg[nseg].flen = 0;
				seg[nseg].pages = (PGROUND(hi) - lo) >> PGSHIFT;
				nseg++;
			}
		}
	}

	if(haveinterp)
		error(Ebadexec);	/* dynamic linking (ld.so) is not supported */
	if(nseg == 0)
		error(Ebadexec);

	for(i = 0; i < nseg; i++){
		lo = seg[i].va;
		hi = lo + (seg[i].pages << PGSHIFT);
		if(lo >= (USTKTOP-USTKSIZE) || hi >= (USTKTOP-USTKSIZE) || hi < lo)
			error(Ebadexec);
	}
	if(entry < UTZERO || entry >= (USTKTOP-USTKSIZE))
		error(Ebadexec);

	*entryp = entry;
	return nseg;
}
