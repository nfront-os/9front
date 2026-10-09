/*
 * GCC port: linker script for the gcc-built pc (386) kernel.
 *
 * See sys/src/9/pc64/kernel.ld for the full rationale (identical here,
 * just 32-bit): AT(ADDR(x) - KZERO) gives every segment its correct low
 * physical load address (p_paddr) while keeping the normal KZERO-relative
 * link address (p_vaddr); and .data is page-aligned directly by this
 * script instead of relying on l.S's boot-time byte-shuffle relocation
 * (which front's original l.s did and this port's l.S no longer does --
 * see the comment on _multibootentry there).
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
ENTRY(_startKADDR)

KZERO = 0xF0000000;

SECTIONS
{
	. = KZERO + 0x00100020;	/* KTZERO, see mem.h / front's mkfile */

	.text : AT(ADDR(.text) - KZERO) {
		*(.text .text.* .gnu.linkonce.t.*)
	}

	PROVIDE(etext = .);

	/*
	 * the kernel expects the data segment to be page-aligned (see
	 * l.S's _multibootentry); make it actually so instead of relying
	 * on a boot-time copy to fix it up.
	 */
	. = ALIGN(0x1000);

	/*
	 * constants and string literals in the data segment, as 8l puts
	 * them: kernel code writes literals (devuart's uartctl(p, "b9600")
	 * tokenizes its argument in place), the text segment is mapped
	 * read-only (the same for programs: 386/plan9.ld)
	 */
	.data : AT(ADDR(.data) - KZERO) {
		*(.data .data.* .gnu.linkonce.d.*)
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	PROVIDE(edata = .);

	.bss : AT(ADDR(.bss) - KZERO) {
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	}

	PROVIDE(end = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .note.gnu.property .comment)
	}
}
