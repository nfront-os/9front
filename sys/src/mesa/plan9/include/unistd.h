/* SPDX-License-Identifier: MIT */
/* sysconf(_SC_PHYS_PAGES), which APE's sysconf does not know */
#include_next <unistd.h>

#ifndef _MESA_PLAN9_UNISTD_H
#define _MESA_PLAN9_UNISTD_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SC_PHYS_PAGES
#define _SC_PHYS_PAGES	0x7001
#endif
#ifndef _SC_PAGESIZE
#define _SC_PAGESIZE	0x7002
#endif
extern long mesa_plan9_sysconf(int);
#define sysconf(n) mesa_plan9_sysconf(n)	/* function-like: meson still finds APE's sysconf */

#ifdef __cplusplus
}
#endif
#endif
