/* SPDX-License-Identifier: MIT */
/* POSIX functions APE lacks (lib/plan9compat.c) */
#include_next <stdlib.h>

#ifndef _MESA_PLAN9_STDLIB_H
#define _MESA_PLAN9_STDLIB_H
#ifdef __cplusplus
extern "C" {
#endif

extern int setenv(const char *, const char *, int);
extern int unsetenv(const char *);
extern int mkstemp(char *);
extern int mkstemps(char *, int);

#ifdef __cplusplus
}
#endif
#endif
