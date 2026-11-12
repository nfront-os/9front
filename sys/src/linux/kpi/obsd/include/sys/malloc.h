/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, kpi_malloc(9) on the file
 * server's heap (kpi/obsd/malloc.c).  The type is ignored; M_WAITOK
 * never fails here (the heap grows or the file server dies), so
 * M_CANFAIL and M_NOWAIT only allow failing.
 */
#ifndef _OBSD_SYS_MALLOC_H
#define _OBSD_SYS_MALLOC_H

#include <sys/types.h>

#define M_WAITOK	0x0001
#define M_NOWAIT	0x0002
#define M_CANFAIL	0x0004
#define M_ZERO		0x0008

#define M_DEVBUF	2
#define M_TEMP		127
#define M_DRM		145
#define M_RADIX		146

/* named so in the program: malloc and free are Plan 9 libc's there */
void	*kpi_malloc(size_t, int, int);
void	*mallocarray(size_t, size_t, int, int);
void	kpi_free(void *, int, size_t);
#define kpi_malloc(size, type, flags)	kpi_malloc(size, type, flags)
#define kpi_free(p, type, size)		kpi_free(p, type, size)

#endif
