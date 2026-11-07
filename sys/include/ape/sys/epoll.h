/*
 * 9front-ng (../9front-ui PLAN §12): Linux's epoll for APE (libwayland's
 * event loop), over poll (ap/gnu/epoll.c).  An epoll descriptor is a pipe's
 * end that keeps the set; epoll_wait polls the set.  timerfd (sys/timerfd.h)
 * descriptors in a set are timed by epoll_wait itself.  An epoll descriptor
 * is not itself pollable.
 */
#ifndef _SYS_EPOLL_H
#define _SYS_EPOLL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EPOLL_CLOEXEC	02000000

#define EPOLL_CTL_ADD	1
#define EPOLL_CTL_DEL	2
#define EPOLL_CTL_MOD	3

#define EPOLLIN		0x001
#define EPOLLPRI	0x002
#define EPOLLOUT	0x004
#define EPOLLERR	0x008
#define EPOLLHUP	0x010
#define EPOLLRDNORM	0x040
#define EPOLLRDBAND	0x080
#define EPOLLWRNORM	0x100
#define EPOLLWRBAND	0x200
#define EPOLLRDHUP	0x2000
#define EPOLLONESHOT	(1U<<30)
#define EPOLLET		(1U<<31)

typedef union epoll_data {
	void	*ptr;
	int	fd;
	uint32_t	u32;
	uint64_t	u64;
} epoll_data_t;

struct epoll_event {
	uint32_t	events;
	epoll_data_t	data;
} __attribute__((packed));

extern int epoll_create(int);
extern int epoll_create1(int);
extern int epoll_ctl(int, int, int, struct epoll_event *);
extern int epoll_wait(int, struct epoll_event *, int, int);

#ifdef __cplusplus
}
#endif

#endif
