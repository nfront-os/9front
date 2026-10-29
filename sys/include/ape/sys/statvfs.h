/* <sys/statvfs.h> APE (ap/gnu/posix.c): Plan 9's file servers say nothing of space */
#ifndef __APE_SYS_STATVFS_H
#define __APE_SYS_STATVFS_H
#include <sys/types.h>

typedef unsigned long long fsblkcnt_t;
typedef unsigned long long fsfilcnt_t;

struct statvfs {
	unsigned long	f_bsize;
	unsigned long	f_frsize;
	fsblkcnt_t	f_blocks;
	fsblkcnt_t	f_bfree;
	fsblkcnt_t	f_bavail;
	fsfilcnt_t	f_files;
	fsfilcnt_t	f_ffree;
	fsfilcnt_t	f_favail;
	unsigned long	f_fsid;
	unsigned long	f_flag;
	unsigned long	f_namemax;
};

#define ST_RDONLY	1
#define ST_NOSUID	2

#ifdef __cplusplus
extern "C" {
#endif
extern int statvfs(const char *, struct statvfs *);
extern int fstatvfs(int, struct statvfs *);
#ifdef __cplusplus
}
#endif
#endif
