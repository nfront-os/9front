/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/eventfd.h (ours, not Linux's) */
#ifndef _KPI_LINUX_EVENTFD_H
#define _KPI_LINUX_EVENTFD_H

#include <linux/kernel.h>
#include <linux/err.h>

struct eventfd_ctx;
#define eventfd_ctx_fdget(fd)		((struct eventfd_ctx *)ERR_PTR(-EBADF))
#define eventfd_ctx_put(c)		do { } while (0)
#define eventfd_signal(c)		((void)(c))	/* 9front: Linux 6.8 dropped n */

#endif
