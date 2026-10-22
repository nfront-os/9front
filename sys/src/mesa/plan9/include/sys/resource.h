/* SPDX-License-Identifier: MIT */
/* getpriority/setpriority (lib/plan9compat.c): no nice values on 9front */
#include_next <sys/resource.h>

#ifndef _MESA_PLAN9_SYS_RESOURCE_H
#define _MESA_PLAN9_SYS_RESOURCE_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef PRIO_PROCESS
#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2
#endif
extern int getpriority(int, int);
extern int setpriority(int, int, int);

#ifdef __cplusplus
}
#endif
#endif
