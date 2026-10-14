#pragma pack on

struct coffsect
{
	char	name[8];
	ulong	phys;
	ulong	virt;
	ulong	size;
	ulong	fptr;
	ulong	fptrreloc;
	ulong	fptrlineno;
	ulong	nrelocnlineno;
	ulong	flags;
};

/*
 * proprietary exec headers, needed to bootstrap various machines
 */
struct mipsexec	/* GCC port: long -> s32int, file format, kencc long is 32 bit */
{
	short	mmagic;		/* (0x160) mips magic number */
	short	nscns;		/* (unused) number of sections */
	s32int	timdat;		/* (unused) time & date stamp */
	s32int	symptr;		/* offset to symbol table */
	s32int	nsyms;		/* size of symbol table */
	short	opthdr;		/* (0x38) sizeof(optional hdr) */
	short	pcszs;		/* flags */
	short	amagic;		/* see above */
	short	vstamp;		/* version stamp */
	s32int	tsize;		/* text size in bytes */
	s32int	dsize;		/* initialized data */
	s32int	bsize;		/* uninitialized data */
	s32int	mentry;		/* entry pt.				*/
	s32int	text_start;	/* base of text used for this file	*/
	s32int	data_start;	/* base of data used for this file	*/
	s32int	bss_start;	/* base of bss used for this file	*/
	s32int	gprmask;	/* general purpose register mask	*/
union{
	s32int	cprmask[4];	/* co-processor register masks		*/
	s32int	pcsize;
};
	s32int	gp_value;	/* the gp value used for this object    */
};

struct mips4kexec
{
	struct mipsexec	h;
	struct coffsect	itexts;
	struct coffsect idatas;
	struct coffsect ibsss;
};

struct sparcexec
{
	short	sjunk;		/* dynamic bit and version number */
	short	smagic;		/* 0407 */
	ulong	stext;
	ulong	sdata;
	ulong	sbss;
	ulong	ssyms;
	ulong	sentry;
	ulong	strsize;
	ulong	sdrsize;
};

struct nextexec
{
	struct	nexthdr{
		ulong	nmagic;
		ulong	ncputype;
		ulong	ncpusubtype;
		ulong	nfiletype;
		ulong	ncmds;
		ulong	nsizeofcmds;
		ulong	nflags;
	};

	struct nextcmd{
		ulong	cmd;
		ulong	cmdsize;
		uchar	segname[16];
		ulong	vmaddr;
		ulong	vmsize;
		ulong	fileoff;
		ulong	filesize;
		ulong	maxprot;
		ulong	initprot;
		ulong	nsects;
		ulong	flags;
	}textc;
	struct nextsect{
		char	sectname[16];
		char	segname[16];
		ulong	addr;
		ulong	size;
		ulong	offset;
		ulong	align;
		ulong	reloff;
		ulong	nreloc;
		ulong	flags;
		ulong	reserved1;
		ulong	reserved2;
	}texts;
	struct nextcmd	datac;
	struct nextsect	datas;
	struct nextsect	bsss;
	struct nextsym{
		ulong	cmd;
		ulong	cmdsize;
		ulong	symoff;
		ulong	nsyms;
		ulong	spoff;
		ulong	pcoff;
	}symc;
};

struct i386exec
{
	struct	i386coff{
		ulong	isectmagic;
		ulong	itime;
		ulong	isyms;
		ulong	insyms;
		ulong	iflags;
	};
	struct	i386hdr{
		ulong	imagic;
		ulong	itextsize;
		ulong	idatasize;
		ulong	ibsssize;
		ulong	ientry;
		ulong	itextstart;
		ulong	idatastart;
	};
	struct coffsect	itexts;
	struct coffsect idatas;
	struct coffsect ibsss;
	struct coffsect icomments;
};

#pragma pack off
