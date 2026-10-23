/*
 * SPDX-License-Identifier: MIT
 *
 * <link.h> for Mesa on 9front: dl_iterate_phdr (lib/dl.c) over the one
 * object there is, the statically linked program. Mesa's build_id.c
 * finds its build-id note with it (hasvk's device UUIDs).
 */
#ifndef _PLAN9_LINK_H
#define _PLAN9_LINK_H
#include <stddef.h>
#include <stdint.h>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
typedef uint32_t Elf64_Word;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;
typedef uint16_t Elf32_Half;

typedef struct {
	unsigned char	e_ident[16];
	Elf64_Half	e_type, e_machine;
	Elf64_Word	e_version;
	Elf64_Addr	e_entry;
	Elf64_Off	e_phoff, e_shoff;
	Elf64_Word	e_flags;
	Elf64_Half	e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
} Elf64_Ehdr;
typedef struct {
	unsigned char	e_ident[16];
	Elf32_Half	e_type, e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff, e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
} Elf32_Ehdr;
typedef struct {
	Elf64_Word	p_type, p_flags;
	Elf64_Off	p_offset;
	Elf64_Addr	p_vaddr, p_paddr;
	Elf64_Xword	p_filesz, p_memsz, p_align;
} Elf64_Phdr;
typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr, p_paddr;
	Elf32_Word	p_filesz, p_memsz, p_flags, p_align;
} Elf32_Phdr;
typedef struct {
	Elf64_Word	n_namesz, n_descsz, n_type;
} Elf64_Nhdr;
typedef struct {
	Elf32_Word	n_namesz, n_descsz, n_type;
} Elf32_Nhdr;

#define PT_NULL		0
#define PT_LOAD		1
#define PT_NOTE		4
#define NT_GNU_BUILD_ID	3

#if UINTPTR_MAX > 0xFFFFFFFFu
#define ElfW(type)	Elf64_##type
#else
#define ElfW(type)	Elf32_##type
#endif

struct dl_phdr_info {
	ElfW(Addr)	dlpi_addr;
	const char	*dlpi_name;
	const ElfW(Phdr)	*dlpi_phdr;
	ElfW(Half)	dlpi_phnum;
};

#ifdef __cplusplus
extern "C" {
#endif
extern int	dl_iterate_phdr(int (*)(struct dl_phdr_info*, size_t, void*), void*);
#ifdef __cplusplus
}
#endif
#endif
