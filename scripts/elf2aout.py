#!/usr/bin/env python3
# Make a Plan 9 amd64 (or 386) a.out kernel from the gcc-built ELF kernel, as
# 6l writes it for 9front's kernel link (pc64/mkfile: $LD -T$KTZERO
# -R4096 -l, header type 2): the fat header -- magic S_MAGIC, the sizes
# of text, data and bss, no symbols, PADDR(entry), then the 64-bit
# entry -- followed by the text and data bytes. 9boot (sys/src/boot/pc,
# efi) loads only this format: text at the entry address, data at the
# next page boundary, bss after it. The ELF must already be laid out so
# (amd64/plan9.ld, kernel.ld): text segment at the entry, data segment
# at the page boundary after the text.
# A 32-bit ELF (pc) gets 8l's plain header (pc/mkfile: $LD -T$KTZERO
# -l): magic I_MAGIC, the sizes, no symbols, the 32-bit entry.
#
# usage: scripts/elf2aout.py kernel.elf a.out
import struct, sys

PT_LOAD = 1
PGSZ = 4096
S_MAGIC = (4*26*26 + 7) | 0x8000	# 6l: 4*26*26+7, fat header
I_MAGIC = 4*11*11 + 7			# 8l

def die(msg):
	sys.exit('elf2aout: ' + msg)

src, dst = sys.argv[1:3]
f = open(src, 'rb').read()
if f[:4] != b'\x7fELF' or f[4] not in (1, 2) or f[5] != 1:
	die('%s: not a little-endian ELF' % src)
elf64 = f[4] == 2
load = []
if elf64:
	entry, phoff = struct.unpack_from('<QQ', f, 24)
	phentsize, phnum = struct.unpack_from('<HH', f, 54)
	for i in range(phnum):
		typ, flags, off, vaddr, paddr, filesz, memsz, align = struct.unpack_from('<IIQQQQQQ', f, phoff + i*phentsize)
		if typ == PT_LOAD:
			load.append((vaddr, off, filesz, memsz, flags))
else:
	entry, phoff = struct.unpack_from('<II', f, 24)
	phentsize, phnum = struct.unpack_from('<HH', f, 42)
	for i in range(phnum):
		typ, off, vaddr, paddr, filesz, memsz, flags, align = struct.unpack_from('<IIIIIIII', f, phoff + i*phentsize)
		if typ == PT_LOAD:
			load.append((vaddr, off, filesz, memsz, flags))
if len(load) != 2:
	die('want 2 loadable segments (text, data), have %d' % len(load))
(tva, toff, tfsz, tmsz, tfl), (dva, doff, dfsz, dmsz, dfl) = sorted(load)
if tva != entry:
	die('text at %#x, entry at %#x' % (tva, entry))
if tfsz != tmsz or not tfl & 1:
	die('text segment not plain text')
if dva != (tva + tfsz + PGSZ - 1) & ~(PGSZ - 1):
	die('data at %#x, not at the page after the text (%#x)' % (dva, tva + tfsz))
if elf64:
	paddr = entry & ~0xfffffffff0000000	# 6l's PADDR
	hdr = struct.pack('>8I', S_MAGIC, tfsz, dfsz, dmsz - dfsz, 0, paddr, 0, 0) + struct.pack('>Q', entry)
else:
	hdr = struct.pack('>8I', I_MAGIC, tfsz, dfsz, dmsz - dfsz, 0, entry, 0, 0)
with open(dst, 'wb') as o:
	o.write(hdr)
	o.write(f[toff:toff + tfsz])
	o.write(f[doff:doff + dfsz])
print('%s: text %#x data %#x bss %#x entry %#x' % (dst, tfsz, dfsz, dmsz - dfsz, entry))
