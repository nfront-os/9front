/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/uidgid.h (ours, not Linux's) */
#ifndef _KPI_LINUX_UIDGID_H
#define _KPI_LINUX_UIDGID_H

#include <linux/types.h>

typedef struct { uid_t val; } kuid_t;
typedef struct { gid_t val; } kgid_t;
#define overflowuid	65534
#define overflowgid	65534
#define from_kuid_munged(ns, u)	((u).val)
#define current_uid()	((kuid_t){ 0 })
#define current_euid()	((kuid_t){ 0 })
#define uid_eq(a, b)	((a).val == (b).val)
#define GLOBAL_ROOT_UID	((kuid_t){ 0 })

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#define overflowgid	65534


#endif
