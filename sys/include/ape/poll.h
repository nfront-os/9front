/* poll(2) APE (9front-rust's ap/gnu/poll.c, over select) */
#ifndef _POLL_H
#define _POLL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long nfds_t;

struct pollfd {
	int	fd;
	short	events;
	short	revents;
};

#define POLLIN		0x001
#define POLLPRI		0x002
#define POLLOUT		0x004
#define POLLERR		0x008
#define POLLHUP		0x010
#define POLLNVAL	0x020
#define POLLRDNORM	0x040
#define POLLRDBAND	0x080
#define POLLWRNORM	0x100
#define POLLWRBAND	0x200

extern int poll(struct pollfd *, nfds_t, int);
/* 9front-ng (gnu/epoll.c): poll with a timespec; the signal mask is not applied */
struct timespec;
extern int ppoll(struct pollfd *, nfds_t, const struct timespec *, const void *);

#ifdef __cplusplus
}
#endif

#endif
