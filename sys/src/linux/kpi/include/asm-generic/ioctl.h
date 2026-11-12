/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: asm-generic/ioctl.h, ours.  The numbering of
 * Linux's ioctl requests, which the DRM core decodes and the drivers'
 * uapi headers build: from bit 0 the number (8 bits), the type (8), the
 * size of the argument (14), the direction (2: write 1, read 2).
 */
#ifndef _KPI_ASM_GENERIC_IOCTL_H
#define _KPI_ASM_GENERIC_IOCTL_H

#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	8
#define _IOC_SIZESHIFT	16
#define _IOC_DIRSHIFT	30

/* a field of n bits as a mask */
#define __KPI_IOC_MASK(n)	((1U << (n)) - 1)
#define _IOC_NRMASK	__KPI_IOC_MASK(_IOC_NRBITS)
#define _IOC_TYPEMASK	__KPI_IOC_MASK(_IOC_TYPEBITS)
#define _IOC_SIZEMASK	__KPI_IOC_MASK(_IOC_SIZEBITS)
#define _IOC_DIRMASK	__KPI_IOC_MASK(_IOC_DIRBITS)

#define _IOC_NONE	0U
#define _IOC_WRITE	1U
#define _IOC_READ	2U

/* a request from its fields, and the fields of a request */
#define _IOC(dir, type, nr, size)	\
	((unsigned int)(dir) << _IOC_DIRSHIFT | (unsigned int)(size) << _IOC_SIZESHIFT | \
	 (unsigned int)(type) << _IOC_TYPESHIFT | (unsigned int)(nr) << _IOC_NRSHIFT)
#define __KPI_IOC_FIELD(req, shift, mask)	(((unsigned int)(req) >> (shift)) & (mask))
#define _IOC_DIR(req)	__KPI_IOC_FIELD(req, _IOC_DIRSHIFT, _IOC_DIRMASK)
#define _IOC_TYPE(req)	__KPI_IOC_FIELD(req, _IOC_TYPESHIFT, _IOC_TYPEMASK)
#define _IOC_NR(req)	__KPI_IOC_FIELD(req, _IOC_NRSHIFT, _IOC_NRMASK)
#define _IOC_SIZE(req)	__KPI_IOC_FIELD(req, _IOC_SIZESHIFT, _IOC_SIZEMASK)

#define _IO(type, nr)		_IOC(_IOC_NONE, type, nr, 0)
#define _IOR(type, nr, t)	_IOC(_IOC_READ, type, nr, sizeof(t))
#define _IOW(type, nr, t)	_IOC(_IOC_WRITE, type, nr, sizeof(t))
#define _IOWR(type, nr, t)	_IOC(_IOC_READ | _IOC_WRITE, type, nr, sizeof(t))

/* the direction bits in place (the DRM core tests a request's with them) */
#define IOC_IN		_IOC(_IOC_WRITE, 0, 0, 0)
#define IOC_OUT		_IOC(_IOC_READ, 0, 0, 0)

#endif
