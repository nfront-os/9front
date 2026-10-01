#!/usr/bin/env python3
"""
Patch the physical-address field (p_paddr) of every PT_LOAD program
header in a higher-half ELF64 kernel so it matches the real load
address (VirtAddr - KZERO) instead of GNU ld's default (p_paddr ==
p_vaddr).

Why: this kernel is linked at a KZERO-offset virtual address but must
be loaded by the bootloader at a low physical address. Plan 9's
original 6l/8l either emitted an a.out binary (no ELF phdrs at all)
or otherwise handled this implicitly. GNU ld, with no linker script,
leaves p_paddr == p_vaddr, which some multiboot loaders (seen here:
grub-mkrescue's core.img on a >~1MB kernel) load directly rather than
falling back to the multiboot a.out-kludge fields -- scribbling the
kernel across itself when the two addresses disagree. Setting p_paddr
correctly makes plain ELF program-header loading (the normal, robust
path) agree with the a.out-kludge fields already in the multiboot
header (see sys/src/9/pc64/l.S's _multibootheader).

Usage: fix-elf-paddr.py <kernel-elf> [KZERO]
KZERO defaults to 0xffffffff80000000 (amd64 pc64, see sys/src/9/pc64/mem.h).
"""
import struct
import sys

def main():
	if len(sys.argv) < 2:
		sys.exit(__doc__)
	path = sys.argv[1]
	kzero = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0xffffffff80000000

	with open(path, "r+b") as f:
		data = bytearray(f.read())

		if data[:4] != b"\x7fELF" or data[4] != 2:  # ELFCLASS64
			sys.exit("not an ELF64 file")

		e_phoff = struct.unpack_from("<Q", data, 0x20)[0]
		e_phentsize = struct.unpack_from("<H", data, 0x36)[0]
		e_phnum = struct.unpack_from("<H", data, 0x38)[0]

		e_entry = struct.unpack_from("<Q", data, 0x18)[0]
		changed = 0
		if e_entry >= kzero:
			want = e_entry - kzero
			struct.pack_into("<Q", data, 0x18, want)
			print("e_entry: %#x -> %#x" % (e_entry, want))
			changed += 1

		PT_LOAD = 1
		for i in range(e_phnum):
			off = e_phoff + i * e_phentsize
			p_type = struct.unpack_from("<I", data, off)[0]
			if p_type != PT_LOAD:
				continue
			p_vaddr = struct.unpack_from("<Q", data, off + 16)[0]
			p_paddr = struct.unpack_from("<Q", data, off + 24)[0]
			if p_vaddr < kzero:
				continue
			want = p_vaddr - kzero
			if p_paddr != want:
				struct.pack_into("<Q", data, off + 24, want)
				print("phdr %d: p_paddr %#x -> %#x" % (i, p_paddr, want))
				changed += 1

		if changed:
			f.seek(0)
			f.write(data)
		else:
			print("nothing to change")

if __name__ == "__main__":
	main()
