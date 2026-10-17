package main

// elf2aout: the Plan 9 a.out kernel from the gcc-built ELF kernel, as
// scripts/build/elf2aout.py (whose description this follows): 6l's fat
// header (S_MAGIC, 7l's R_MAGIC for AArch64) or 8l's plain one (I_MAGIC)
// with the sizes and the entry, then the text and data bytes. The build
// tool runs it for the install step on 9front, where there is no python
// (build.exe -elf2aout kernel.elf a.out).

import (
	"debug/elf"
	"encoding/binary"
	"fmt"
	"os"
	"sort"
)

func elf2aout(src, dst string) error {
	const (
		pgsz   = 4096
		sMagic = (4*26*26 + 7) | 0x8000 // 6l, fat header
		rMagic = (4*28*28 + 7) | 0x8000 // 7l
		bMagic = (4*30*30 + 7) | 0x8000 // jl -X
		iMagic = 4*11*11 + 7            // 8l
	)
	f, err := elf.Open(src)
	if err != nil {
		return fmt.Errorf("%s: %v", src, err)
	}
	defer f.Close()
	if f.Data != elf.ELFDATA2LSB {
		return fmt.Errorf("%s: not a little-endian ELF", src)
	}
	elf64 := f.Class == elf.ELFCLASS64
	arm64 := f.Machine == elf.EM_AARCH64
	riscv64 := f.Machine == elf.EM_RISCV
	var load []*elf.Prog
	for _, p := range f.Progs {
		if p.Type == elf.PT_LOAD {
			load = append(load, p)
		}
	}
	if len(load) != 2 {
		return fmt.Errorf("want 2 loadable segments (text, data), have %d", len(load))
	}
	sort.Slice(load, func(i, j int) bool { return load[i].Vaddr < load[j].Vaddr })
	t, d := load[0], load[1]
	entry := f.Entry
	rnd := uint64(pgsz)
	if arm64 {
		rnd = 0x10000
		entry = t.Vaddr
	}
	if riscv64 { // jl -R8: the data right after the text
		rnd = 8
		entry = t.Vaddr
	}
	if t.Vaddr != entry {
		return fmt.Errorf("text at %#x, entry at %#x", t.Vaddr, entry)
	}
	if t.Filesz != t.Memsz || t.Flags&elf.PF_X == 0 {
		return fmt.Errorf("text segment not plain text")
	}
	if d.Vaddr != (t.Vaddr+t.Filesz+rnd-1)&^(rnd-1) {
		return fmt.Errorf("data at %#x, not at the page after the text (%#x)", d.Vaddr, t.Vaddr+t.Filesz)
	}
	text := make([]byte, t.Filesz)
	if _, err := t.ReadAt(text, 0); err != nil {
		return err
	}
	data := make([]byte, d.Filesz)
	if _, err := d.ReadAt(data, 0); err != nil {
		return err
	}
	var hdr []uint32
	if elf64 {
		magic := uint32(sMagic)
		if arm64 {
			magic = rMagic
		}
		if riscv64 {
			magic = bMagic
		}
		paddr := entry &^ 0xfffffffff0000000 // 6l's PADDR
		hdr = []uint32{magic, uint32(t.Filesz), uint32(d.Filesz), uint32(d.Memsz - d.Filesz), 0, uint32(paddr), 0, 0}
	} else {
		hdr = []uint32{iMagic, uint32(t.Filesz), uint32(d.Filesz), uint32(d.Memsz - d.Filesz), 0, uint32(entry), 0, 0}
	}
	o, err := os.Create(dst)
	if err != nil {
		return err
	}
	binary.Write(o, binary.BigEndian, hdr)
	if elf64 {
		binary.Write(o, binary.BigEndian, entry)
	}
	o.Write(text)
	o.Write(data)
	if err := o.Close(); err != nil {
		return err
	}
	if riscv64 { // the raw image OpenSBI loads: text and data
		b, err := os.Create(dst + ".bin")
		if err != nil {
			return err
		}
		b.Write(text)
		b.Write(data)
		if err := b.Close(); err != nil {
			return err
		}
	}
	fmt.Printf("%s: text %#x data %#x bss %#x entry %#x\n", dst, t.Filesz, d.Filesz, d.Memsz-d.Filesz, entry)
	return nil
}
