/*
 * GCC port: linker script for arm64 user programs, the layout 7l
 * produces (as amd64/plan9.ld for 6l): a text segment with the code
 * only, starting at UTZERO (0x10000) with the headers, and a data
 * segment with the initialised data, the constants and string literals
 * (kencc keeps them writable in the data segment), and bss, on the next
 * 64K boundary (7l's default -R0x10000).
 */
ENTRY(_main)

PHDRS
{
	text PT_LOAD FILEHDR PHDRS FLAGS(5);	/* R E */
	data PT_LOAD FLAGS(6);			/* RW */
}

SECTIONS
{
	. = 0x10000 + SIZEOF_HEADERS;
	.text : {
		*(.text .text.*)
	} :text
	/* unwind tables: read-only; for debuggers, and for C++ exceptions
	 * of the native gcc (../9front-gcc): its crtbegin.o registers
	 * .eh_frame, crtend.o ends it; the language-specific data with it
	 * (as amd64/plan9.ld) */
	.eh_frame : {
		KEEP(*(.eh_frame))
	} :text
	.gcc_except_table : {
		*(.gcc_except_table .gcc_except_table.*)
	} :text
	PROVIDE(etext = .);

	. = ALIGN(0x10000);
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
