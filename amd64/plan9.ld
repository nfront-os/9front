/*
 * GCC port: linker script for amd64 user programs, the layout 8l (6l)
 * produces: a text segment with the code only, and a data segment with
 * the initialised data, the constants and string literals, and bss.
 * kencc keeps string literals in the data segment, and programs write
 * them (ape make's eqsign() terminates "MAKE=make" in place); gcc puts
 * them in .rodata, which GNU ld's default script maps read-only.
 *
 * The two segments are fixed here instead of patched into ld's default
 * script, so every program gets exactly them whatever sections it has:
 * the kernel's ELF loader refuses a segment that is writable and
 * executable at once.
 */
ENTRY(_main)

PHDRS
{
	text PT_LOAD FILEHDR PHDRS FLAGS(5);	/* R E */
	data PT_LOAD FLAGS(6);			/* RW */
}

SECTIONS
{
	. = 0x400000 + SIZEOF_HEADERS;
	.text : {
		*(.text .text.*)
	} :text
	/* unwind tables: read-only, only for debuggers */
	.eh_frame : {
		KEEP(*(.eh_frame))
	} :text
	PROVIDE(etext = .);

	/* the data segment starts on the next 2MB boundary, as with 6l -R;
	 * in the file the segments are only page aligned (max-page-size
	 * 0x1000 in amd64/cflags.json), the ELF loader needs no more */
	. = ALIGN(0x200000);
	.data : {
		*(.data .data.*)
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	} :data
	PROVIDE(edata = .);
	.bss : {
		*(.bss .bss.* COMMON)
	} :data
	PROVIDE(end = .);
	PROVIDE(_end = .);

	/DISCARD/ : {
		*(.note.gnu.property .note.GNU-stack .interp .dynamic)
	}
}
