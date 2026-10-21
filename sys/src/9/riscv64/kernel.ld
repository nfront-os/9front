/*
 * GCC port: linker script for the gcc-built riscv64 kernel (qemu virt),
 * the layout of riscv64/mkfile's link ($LD -T$loadaddr -R8): text at
 * KTZERO (0xffffffc080200000, physical 0x80200000, where OpenSBI loads
 * the raw image), the data right after it (8-byte aligned), then bss.
 * AT(ADDR - KZERO) gives each segment its physical load address;
 * constants and string literals go into the data segment, as jl puts
 * them. 9virt.bin is text and data (objcopy -O binary).
 */

OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_mainphys)

KZERO = 0xffffffc000000000;

/* two segments, text and data, also where they share a page (jl -R8) */
PHDRS
{
	text PT_LOAD FLAGS(5);	/* R E */
	data PT_LOAD FLAGS(6);	/* RW */
}

SECTIONS
{
	/* KTZERO, see mkfile and mem.h: the board's (the CONF's #loadaddr, --defsym) */
	. = DEFINED(KTZERO) ? KTZERO : 0xffffffc080200000;

	.text : AT(ADDR(.text) - KZERO) {
		*(.text .text.* .gnu.linkonce.t.*)
	} :text

	PROVIDE(etext = .);
	_mainphys = _main - KZERO;

	. = ALIGN(8);

	.data : AT(ADDR(.data) - KZERO) {
		*(.data .data.* .gnu.linkonce.d.*)
		*(.sdata .sdata.*)
		*(.rodata .rodata.* .srodata .srodata.* .gnu.linkonce.r.*)
	} :data

	. = ALIGN(8);
	PROVIDE(edata = .);

	.bss : AT(ADDR(.bss) - KZERO) {
		*(.sbss .sbss.* .bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	} :data

	. = ALIGN(8);
	PROVIDE(end = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .note.gnu.property .comment .riscv.attributes)
	}
}
