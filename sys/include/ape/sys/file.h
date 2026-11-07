/*
 * 9front-ng: BSD's flock for APE (ap/gnu/flock.c).  Plan 9 has no advisory
 * locks: flock grants every one (libwayland's lock file beside its socket
 * says a compositor has the name; on 9front the /srv entry of the socket is
 * the real claim).
 */
#ifndef _SYS_FILE_H
#define _SYS_FILE_H

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
