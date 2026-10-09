/*
 * GCC port: linker script for the gcc-built arm64 kernel (qemu), the
 * layout of arm64/mkfile's link ($LD -T$loadaddr -l, 7l's -R0x10000):
 * text at loadaddr (0xffffffffc0100000, physical 0x40100000), the data
 * segment on the next 64K boundary, then bss. As pc64/kernel.ld:
 * AT(ADDR - KZERO) gives each segment its physical load address, and
 * constants and string literals go into the data segment, as 7l puts
 * them (devuart's uartctl(p, "b9600") tokenizes its argument in place).
 * The ELF entry is _start's physical address: QEMU -kernel and the boot
 * loaders enter with the MMU off.
 */

OUTPUT_FORMAT("elf64-littleaarch64")
ENTRY(_startphys)

KZERO = 0xffffffff80000000;

SECTIONS
{
	. = 0xffffffffc0100000;	/* loadaddr, see mkfile and mem.h (KTZERO) */

	.text : AT(ADDR(.text) - KZERO) {
		*(.text .text.* .gnu.linkonce.t.*)
	}

	PROVIDE(etext = .);
	_startphys = _start - KZERO;

	. = ALIGN(0x10000);

	.data : AT(ADDR(.data) - KZERO) {
		*(.data .data.* .gnu.linkonce.d.*)
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	. = ALIGN(8);	/* _start clears [edata, end) by 8 bytes */
	PROVIDE(edata = .);

	.bss : AT(ADDR(.bss) - KZERO) {
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	}

	. = ALIGN(8);
	PROVIDE(end = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack .note.gnu.property .comment)
	}
}
