/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/fcntl.h: open flags, Linux's values (the Linux code passes them through) */
#ifndef _OBSD_SYS_FCNTL_H
#define _OBSD_SYS_FCNTL_H
#define O_RDONLY	00
#define O_WRONLY	01
#define O_RDWR		02
#define O_ACCMODE	03
#define O_CREAT		0100
#define O_EXCL		0200
#define O_TRUNC		01000
#define O_APPEND	02000
#define O_NONBLOCK	04000
#define O_CLOEXEC	02000000
#define FREAD		0x0001
#define FWRITE		0x0002
#endif
