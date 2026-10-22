/* SPDX-License-Identifier: MIT */
/* flock: no locks on 9front files; Mesa uses it for its disk caches */
#ifndef _MESA_PLAN9_SYS_FILE_H
#define _MESA_PLAN9_SYS_FILE_H
#ifdef __cplusplus
extern "C" {
#endif

#define LOCK_SH	1
#define LOCK_EX	2
#define LOCK_NB	4
#define LOCK_UN	8
extern int flock(int, int);

#ifdef __cplusplus
}
#endif
#endif
