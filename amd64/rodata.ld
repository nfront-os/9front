/*
 * GCC port: kencc/8l keep string literals and other constants in the
 * data segment, so programs may write them (ape make's built-in rules,
 * eqsign() terminating "MAKE=make" in place). gcc puts them in .rodata,
 * which GNU ld places in the read-only text segment. This augments the
 * default linker script (INSERT): .rodata goes behind .data, writable.
 */
SECTIONS
{
	.rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
}
INSERT AFTER .data;
