/*
 * GCC port: linker script for the gcc-built pc64 kernel.
 *
 * Replaces the earlier `-Ttext=... -R4096 -N` Oflags hack. Those relied on
 * GNU ld's default p_paddr == p_vaddr and on -N's "no page alignment"
 * behavior to approximate the tightly-packed layout Plan 9's 8l/6l produce.
 * Two things fell out of that approximation:
 *
 *  1. p_paddr stayed equal to the (high, KZERO-relative) p_vaddr instead of
 *     the real low physical load address, which not all multiboot loaders
 *     tolerate.
 *  2. without -N there'd be a huge (2MB, ld's default max-page-size on
 *     x86-64) gap between .text and .data; -N avoided the gap but also
 *     left .data packed tight against .text with no alignment at all --
 *     NOT the same thing l.S's _multibootentry expects (see below).
 *
 * l.S's _multibootentry carries, unmodified, Plan 9's original boot-time
 * data-segment relocation: 8l/6l pack .data immediately after .text with
 * zero gap, so _multibootentry shifts that tightly-packed block up to a
 * page boundary at boot (see the comment above _multibootentry in l.S).
 * GNU ld does not reproduce that exact tight packing (it still applies a
 * small default alignment to .rodata/.data), so the *source* address that
 * relocation copies from no longer lines up with where the content really
 * is -- the copy quietly scrambles .rodata/.data at boot.
 *
 * Fixed at the source instead of patched around: this script page-aligns
 * .data itself (so .data already starts where _multibootentry's relocation
 * used to move it to), and l.S's _multibootentry no longer relocates
 * anything -- see the "GCC port" comment there.
 *
 * AT(ADDR(x) - KZERO) gives every segment its correct low physical load
 * address (p_paddr) while keeping the normal KZERO-relative link address
 * (p_vaddr) symbols are computed against -- the piece plain `-Ttext=`
 * cannot express at all.
 */

OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
ENTRY(_protected)

KZERO = 0xffffffff80000000;

SECTIONS
{
	. = KZERO + 0x00110000;	/* KTZERO, see mem.h */

	.text : AT(ADDR(.text) - KZERO) {
		*(.text .text.* .gnu.linkonce.t.*)
	}

	.rodata : AT(ADDR(.rodata) - KZERO) {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	PROVIDE(etext = .);

	/*
	 * the kernel expects the data segment to be page-aligned (see
	 * l.S's _multibootentry); make it actually so instead of relying
	 * on a boot-time copy to fix it up.
	 */
	. = ALIGN(0x1000);

	.data : AT(ADDR(.data) - KZERO) {
		*(.data .data.* .gnu.linkonce.d.*)
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
