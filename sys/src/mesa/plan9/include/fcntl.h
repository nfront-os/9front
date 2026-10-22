/* SPDX-License-Identifier: MIT */
/* O_CLOEXEC: 9front's open has no such flag; ignored */
#include_next <fcntl.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC	0
#endif
