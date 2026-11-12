/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, machine/intr.h: the IPLs (meaningless here, see sys/mutex.h) */
#ifndef _OBSD_MACHINE_INTR_H
#define _OBSD_MACHINE_INTR_H
#define IPL_NONE	0
#define IPL_SOFTCLOCK	1
#define IPL_SOFTNET	2
#define IPL_BIO		3
#define IPL_NET		4
#define IPL_SOFTTTY	5
#define IPL_TTY		6
#define IPL_VM		7
#define IPL_AUDIO	8
#define IPL_CLOCK	9
#define IPL_SCHED	10
#define IPL_HIGH	11
#define IPL_IPI		12
#define IPL_MPSAFE	0x100
#define IPL_MPFLOOR	IPL_TTY
#endif
