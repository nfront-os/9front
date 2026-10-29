#ifndef __SYS_UIO_H__
#define __SYS_UIO_H__

/* GCC port: a program that includes it wants it: _BSD_EXTENSION for
 * this header (bsd.h's prototypes stay off) */
#ifndef _BSD_EXTENSION
#define _BSD_EXTENSION
#define __GCC_BSD_SYS_UIO_H
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma lib "/$M/lib/ape/libbsd.a"

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)uio.h	7.1 (Berkeley) 6/4/86
 */

struct iovec {
	char	*iov_base;
	int	iov_len;
};

extern int writev(int, struct iovec*, int);
extern int readv(int, struct iovec*, int);

#ifdef __cplusplus
}
#endif

#ifdef __GCC_BSD_SYS_UIO_H
#undef _BSD_EXTENSION
#undef __GCC_BSD_SYS_UIO_H
#endif
#endif /* !__SYS_UIO_H__ */
