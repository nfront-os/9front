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
	/* unwind tables: read-only; for debuggers, and for C++ exceptions
	 * of the native gcc (../9front-gcc): its crtbegin.o registers
	 * .eh_frame, crtend.o ends it; the language-specific data with it */
	.eh_frame : {
		KEEP(*(.eh_frame))
	} :text
	.gcc_except_table : {
		*(.gcc_except_table .gcc_except_table.*)
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
	/* C++ constructors: 9front's C has none; the native gcc
	 * (../9front-gcc) runs them from __wrap_main before main */
	.init_array : {
		PROVIDE(__init_array_start = .);
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE(__init_array_end = .);
	} :data
	PROVIDE(edata = .);
	/* Go's .noptrbss too (cgo programs of ../9front-go, linked by gcc):
	 * before end, where APE's malloc begins; as an orphan after .bss it
	 * came after end, and malloc's memory was Go's variables */
	.bss : {
		*(.bss .bss.* COMMON)
		*(.noptrbss .noptrbss.*)
	} :data
	PROVIDE(end = .);
	PROVIDE(_end = .);

	/DISCARD/ : {
		*(.note.gnu.property .note.GNU-stack .interp .dynamic)
	}
}
