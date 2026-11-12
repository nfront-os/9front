/* Public domain. */

#ifndef _LINUX_CIRC_BUF_H
#define _LINUX_CIRC_BUF_H

#define CIRC_SPACE(h,t,s)	(((t) - ((h)+1)) & ((s)-1))

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
struct circ_buf { char *buf; int head; int tail; };
#define CIRC_CNT(h, t, s)	(((h) - (t)) & ((s) - 1))
#define CIRC_CNT_TO_END(h, t, s)	({ int end = (s) - (t); int n = ((h) + end) & ((s) - 1); n < end ? n : end; })
#define CIRC_SPACE_TO_END(h, t, s)	({ int end = (s) - 1 - (h); int n = (end + (t)) & ((s) - 1); n <= end ? n : end + 1; })

#endif
