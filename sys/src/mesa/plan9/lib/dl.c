/*
 * SPDX-License-Identifier: MIT
 *
 * dladdr and dl_iterate_phdr on 9front (headers in ../include): a
 * statically linked program is the only object. Its ELF and program
 * headers are in memory at __ehdr_start (the first PT_LOAD starts at
 * file offset 0). The linker script keeps no notes, so a PT_NOTE with
 * a GNU build-id is added: a hash of the program's text, which changes
 * with every build as a real build-id would (Mesa's build_id.c, for
 * hasvk's device and pipeline cache UUIDs).
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <link.h>
#include <dlfcn.h>

extern const char __ehdr_start[] __attribute__((weak));

enum { Maxph = 16, Idlen = 20 };

static struct {
	ElfW(Nhdr)	nhdr;
	char	name[4];
	unsigned char	id[Idlen];
} note;
static ElfW(Phdr) ph[Maxph+1];
static int nph = -1;

static uint64_t
fnv(const unsigned char *p, size_t n, uint64_t h)
{
	while(n-- > 0){
		h ^= *p++;
		h *= 0x100000001b3ULL;
	}
	return h;
}

static void
init(void)
{
	const ElfW(Ehdr) *eh;
	const ElfW(Phdr) *p;
	const unsigned char *text;
	uint64_t h[3];
	int i, n;

	if(nph >= 0)
		return;
	nph = 0;
	eh = (const ElfW(Ehdr)*)__ehdr_start;
	if(eh == NULL || memcmp(eh->e_ident, "\177ELF", 4) != 0)
		return;
	p = (const ElfW(Phdr)*)(__ehdr_start + eh->e_phoff);
	n = eh->e_phnum < Maxph ? eh->e_phnum : Maxph;
	text = NULL;
	for(i = 0; i < n; i++){
		ph[i] = p[i];
		if(text == NULL && p[i].p_type == PT_LOAD){
			text = (const unsigned char*)(uintptr_t)p[i].p_vaddr;
			h[0] = fnv(text, p[i].p_filesz, 0xcbf29ce484222325ULL);
			h[1] = fnv(text, p[i].p_filesz, h[0] ^ 0x9e3779b97f4a7c15ULL);
			h[2] = fnv(text, p[i].p_filesz, h[1] ^ 0xc2b2ae3d27d4eb4fULL);
		}
	}
	if(text != NULL){
		note.nhdr.n_namesz = 4;
		note.nhdr.n_descsz = Idlen;
		note.nhdr.n_type = NT_GNU_BUILD_ID;
		memcpy(note.name, "GNU", 4);
		memcpy(note.id, h, Idlen);
		memset(&ph[n], 0, sizeof ph[n]);
		ph[n].p_type = PT_NOTE;
		ph[n].p_vaddr = (uintptr_t)&note;
		ph[n].p_filesz = sizeof note;
		ph[n].p_memsz = sizeof note;
		n++;
	}
	nph = n;
}

int
dl_iterate_phdr(int (*f)(struct dl_phdr_info*, size_t, void*), void *a)
{
	struct dl_phdr_info info;

	init();
	if(nph == 0)
		return 0;
	memset(&info, 0, sizeof info);
	info.dlpi_addr = 0;
	info.dlpi_name = "";
	info.dlpi_phdr = ph;
	info.dlpi_phnum = nph;
	return f(&info, sizeof info, a);
}

int
dladdr(const void *addr, Dl_info *info)
{
	int i;

	init();
	for(i = 0; i < nph; i++)
		if(ph[i].p_type == PT_LOAD){
			memset(info, 0, sizeof *info);
			info->dli_fname = "";
			info->dli_fbase = (void*)(uintptr_t)ph[i].p_vaddr;
			(void)addr;
			return 1;
		}
	return 0;
}
