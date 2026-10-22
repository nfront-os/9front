/* SPDX-License-Identifier: MIT */
/*
 * poll(2) declarations for header-only users (Mesa's util/libsync.h);
 * 9front has no file descriptors to poll this way: poll fails.
 */
#ifndef _MESA_PLAN9_POLL_H
#define _MESA_PLAN9_POLL_H
#ifdef __cplusplus
extern "C" {
#endif

#define POLLIN		0x001
#define POLLPRI		0x002
#define POLLOUT		0x004
#define POLLERR		0x008
#define POLLHUP		0x010
#define POLLNVAL	0x020

typedef unsigned long nfds_t;
struct pollfd {
	int	fd;
	short	events;
	short	revents;
};
extern int poll(struct pollfd *, nfds_t, int);

#ifdef __cplusplus
}
#endif
#endif
