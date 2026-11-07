/*
 * mmap APE (ap/gnu/mman.c), which has none: memory from malloc; a file's pages read
 * in, a shared writable one written back at munmap and msync. No executable memory, no
 * protection.
 */
#ifndef __APE_SYS_MMAN_H
#define __APE_SYS_MMAN_H
#include <sys/types.h>
#include <stddef.h>

#define PROT_NONE	0x0
#define PROT_READ	0x1
#define PROT_WRITE	0x2
#define PROT_EXEC	0x4

#define MAP_SHARED	0x01
#define MAP_PRIVATE	0x02
#define MAP_FIXED	0x10
#define MAP_ANONYMOUS	0x20
#define MAP_ANON	MAP_ANONYMOUS
#define MAP_NORESERVE	0x4000
#define MAP_FAILED	((void *)-1)

#define MS_ASYNC	1
#define MS_INVALIDATE	2
#define MS_SYNC		4

#define MADV_NORMAL	0
#define MADV_RANDOM	1
#define MADV_SEQUENTIAL	2
#define MADV_WILLNEED	3
#define MADV_DONTNEED	4
#define POSIX_MADV_NORMAL	MADV_NORMAL
#define POSIX_MADV_RANDOM	MADV_RANDOM
#define POSIX_MADV_SEQUENTIAL	MADV_SEQUENTIAL
#define POSIX_MADV_WILLNEED	MADV_WILLNEED
#define POSIX_MADV_DONTNEED	MADV_DONTNEED

#ifdef __cplusplus
extern "C" {
#endif
extern void *mmap(void *, size_t, int, int, int, off_t);
extern int munmap(void *, size_t);
extern int msync(void *, size_t, int);
extern int mprotect(void *, size_t, int);
extern int madvise(void *, size_t, int);
extern int posix_madvise(void *, size_t, int);
/* shared memory objects: segments, the same memory in every process (segment(3)) */
extern int shm_open(const char *, int, mode_t);
extern int shm_unlink(const char *);
#define MFD_CLOEXEC	1
#define MFD_ALLOW_SEALING	2
#define MFD_NOEXEC_SEAL	8	/* 9front-ng: segments are never executable: as MFD_ALLOW_SEALING */
extern int memfd_create(const char *, unsigned int);
#ifdef __cplusplus
}
#endif
#endif
