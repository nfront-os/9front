/* SPDX-License-Identifier: MIT */
/*
 * mmap(2) declarations; 9front maps memory with segattach(2), so
 * Mesa's mmap fails (lib/plan9compat.c) and Mesa takes its other paths.
 */
#ifndef _MESA_PLAN9_SYS_MMAN_H
#define _MESA_PLAN9_SYS_MMAN_H
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define PROT_NONE	0
#define PROT_READ	1
#define PROT_WRITE	2
#define PROT_EXEC	4
#define MAP_SHARED	0x01
#define MAP_PRIVATE	0x02
#define MAP_FIXED	0x10
#define MAP_ANONYMOUS	0x20
#define MAP_ANON	MAP_ANONYMOUS
#define MAP_FAILED	((void *)-1)

/*
 * 9front-ng: Mesa's own calls only (mesa9_mmap fails, as before); the
 * program's mmap stays APE's (shared memory objects: a Wayland client
 * with Vulkan), not replaced by these for all of it
 */
#define mmap(a, n, p, f, fd, o)	mesa9_mmap(a, n, p, f, fd, o)
#define munmap(a, n)	mesa9_munmap(a, n)
extern void *mmap(void *, size_t, int, int, int, off_t);
extern int munmap(void *, size_t);

#ifdef __cplusplus
}
#endif
#endif
