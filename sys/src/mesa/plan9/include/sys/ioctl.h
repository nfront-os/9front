/* SPDX-License-Identifier: MIT */
/*
 * <sys/ioctl.h> for Mesa on 9front: APE's (ioctl, FIONREAD) and BSD's
 * ioctl number macros (sys/ioccom.h), which util/libsync.h uses as it
 * finds them on Linux's
 */
#ifndef _MESA_PLAN9_SYS_IOCTL_H
#define _MESA_PLAN9_SYS_IOCTL_H
#include_next <sys/ioctl.h>
#include <sys/ioccom.h>
#endif
