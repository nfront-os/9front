/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/poll.h (ours, not Linux's) */
#ifndef _KPI_LINUX_POLL_H
#define _KPI_LINUX_POLL_H

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/wait.h>

typedef struct poll_table_struct poll_table;
#define EPOLLIN		0x00000001
#define EPOLLPRI	0x00000002
#define EPOLLOUT	0x00000004
#define EPOLLERR	0x00000008
#define EPOLLHUP	0x00000010
#define EPOLLRDNORM	0x00000040
#define EPOLLWRNORM	0x00000100
#define POLLIN		EPOLLIN
#define POLLRDNORM	EPOLLRDNORM
static inline void poll_wait(struct file *f, wait_queue_head_t *q, poll_table *p) { }

#endif
