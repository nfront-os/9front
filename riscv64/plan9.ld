/*
 * GCC port: linker script for riscv64 user programs, the layout jl
 * produces (as amd64/plan9.ld for 6l): a text segment with the code
 * only, starting at UTZERO (0x1000) with the headers, and a data
 * segment with the initialised data, the constants and string literals
 * (kencc keeps them writable in the data segment), and bss, on the next
 * page boundary (jl's default -R0x1000).
 */
ENTRY(_main)

PHDRS
{
	text PT_LOAD FILEHDR PHDRS FLAGS(5);	/* R E */
	data PT_LOAD FLAGS(6);			/* RW */
}

SECTIONS
{
	. = 0x1000 + SIZEOF_HEADERS;
	.text : {
		*(.text .text.*)
	} :text
	/* unwind tables: read-only, only for debuggers */
	.eh_frame : {
		KEEP(*(.eh_frame))
	} :text
	PROVIDE(etext = .);

	. = ALIGN(0x1000);
	.data : {
		*(.data .data.*)
		*(.sdata .sdata.*)
		*(.rodata .rodata.* .srodata .srodata.* .gnu.linkonce.r.*)
	} :data
	PROVIDE(edata = .);
	.bss : {
		*(.sbss .sbss.* .bss .bss.* COMMON)
	} :data
	PROVIDE(end = .);
	PROVIDE(_end = .);

	/DISCARD/ : {
		*(.note.gnu.property .note.GNU-stack .interp .dynamic .riscv.attributes)
	}
}
