/*
 * 9front-ng (../9front-ui PLAN §12): Linux's epoll, timerfd and eventfd
 * for APE (libwayland's event loop), over poll (buf.c); ppoll; signalfd
 * declared, not there.
 *
 * An epoll descriptor is the read end of a pipe (so that close and dup
 * work as for any descriptor) with a set kept here, by its number;
 * epoll_wait polls the descriptors of the set.  A timerfd is the read end
 * of a pipe too, with a deadline kept here: epoll_wait waits no longer than
 * the earliest deadline in its set and, when one has come, writes the count
 * of expirations into the pipe, which is then readable as on Linux (8
 * bytes).  A timer is only kept by epoll_wait (libwayland's way); a timerfd
 * polled by itself never fires.  An eventfd is a pipe too, written through
 * eventfd_write.
 */
#define _POSIX_SOURCE
#define _BSD_EXTENSION
#define _LOCK_EXTENSION
#define _RESEARCH_SOURCE
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <lock.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>

typedef struct Ep Ep;
typedef struct Item Item;
typedef struct Timer Timer;

struct Item {
	int	fd;
	uint32_t	events;
	epoll_data_t	data;
};

struct Ep {
	int	fd;	/* this set's descriptor */
	int	w;	/* its pipe's other end */
	Item	*item;
	int	n;
	int	max;
	Ep	*next;
};

struct Timer {
	int	fd;	/* the timerfd */
	int	w;	/* its pipe's other end: the expirations go there */
	long long	deadline;	/* ns on CLOCK_MONOTONIC; 0: disarmed */
	long long	interval;
	Timer	*next;
};

static Lock lk;
static Ep *eps;
static Timer *timers;

static long long
now(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec*1000000000LL + t.tv_nsec;
}

static long long
tsns(const struct timespec *t)
{
	return t->tv_sec*1000000000LL + t->tv_nsec;
}

static void
nsts(long long ns, struct timespec *t)
{
	t->tv_sec = ns / 1000000000LL;
	t->tv_nsec = ns % 1000000000LL;
}

/* a pipe: the read end for the caller, the write end kept */
static int
mkpipe(int *w, int flags)
{
	int p[2];

	if(pipe(p) < 0)
		return -1;
	if(flags & 02000000)	/* EPOLL_CLOEXEC, TFD_CLOEXEC, EFD_CLOEXEC */
		fcntl(p[0], F_SETFD, FD_CLOEXEC);
	fcntl(p[1], F_SETFD, FD_CLOEXEC);
	*w = p[1];
	return p[0];
}

/* the set of descriptor fd (a stale one of an fd closed and reused goes); with lk */
static Ep *
findep(int fd, int unlink)
{
	Ep *e, **l;

	for(l = &eps; (e = *l) != NULL; l = &e->next)
		if(e->fd == fd){
			if(unlink)
				*l = e->next;
			return e;
		}
	return NULL;
}

static Timer *
findtimer(int fd, int unlink)
{
	Timer *t, **l;

	for(l = &timers; (t = *l) != NULL; l = &t->next)
		if(t->fd == fd){
			if(unlink)
				*l = t->next;
			return t;
		}
	return NULL;
}

static void
freeep(Ep *e)
{
	if(e == NULL)
		return;
	close(e->w);
	free(e->item);
	free(e);
}

int
epoll_create1(int flags)
{
	Ep *e;
	int fd, w;

	if((e = calloc(1, sizeof *e)) == NULL){
		errno = ENOMEM;
		return -1;
	}
	if((fd = mkpipe(&w, flags)) < 0){
		free(e);
		return -1;
	}
	e->fd = fd;
	e->w = w;
	lock(&lk);
	freeep(findep(fd, 1));	/* an earlier set whose descriptor was closed */
	e->next = eps;
	eps = e;
	unlock(&lk);
	return fd;
}

int
epoll_create(int size)
{
	if(size <= 0){
		errno = EINVAL;
		return -1;
	}
	return epoll_create1(0);
}

int
epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev)
{
	Ep *e;
	Item *it;
	int i;

	lock(&lk);
	if((e = findep(epfd, 0)) == NULL){
		unlock(&lk);
		errno = EBADF;
		return -1;
	}
	for(i = 0; i < e->n; i++)
		if(e->item[i].fd == fd)
			break;
	switch(op){
	case EPOLL_CTL_ADD:
		if(i < e->n){
			unlock(&lk);
			errno = EEXIST;
			return -1;
		}
		if(e->n == e->max){
			it = realloc(e->item, (e->max*2 + 8) * sizeof *it);
			if(it == NULL){
				unlock(&lk);
				errno = ENOMEM;
				return -1;
			}
			e->item = it;
			e->max = e->max*2 + 8;
		}
		e->item[e->n].fd = fd;
		e->item[e->n].events = ev->events;
		e->item[e->n].data = ev->data;
		e->n++;
		break;
	case EPOLL_CTL_MOD:
		if(i == e->n){
			unlock(&lk);
			errno = ENOENT;
			return -1;
		}
		e->item[i].events = ev->events;
		e->item[i].data = ev->data;
		break;
	case EPOLL_CTL_DEL:
		if(i == e->n){
			unlock(&lk);
			errno = ENOENT;
			return -1;
		}
		e->item[i] = e->item[--e->n];
		break;
	default:
		unlock(&lk);
		errno = EINVAL;
		return -1;
	}
	unlock(&lk);
	return 0;
}

/* a timer whose time came: its expirations into its pipe; with lk */
static void
fire(Timer *t, long long ns)
{
	uint64_t n;

	n = 1;
	if(t->interval > 0){
		n += (ns - t->deadline) / t->interval;
		t->deadline += n * t->interval;
	}else
		t->deadline = 0;
	write(t->w, &n, sizeof n);
}

int
epoll_wait(int epfd, struct epoll_event *evs, int max, int ms)
{
	struct pollfd *pf;
	Ep *e;
	Timer *t;
	Item *items;
	long long ns, wait;
	int i, n, r, got;

	if(max <= 0){
		errno = EINVAL;
		return -1;
	}
	lock(&lk);
	if((e = findep(epfd, 0)) == NULL){
		unlock(&lk);
		errno = EBADF;
		return -1;
	}
	n = e->n;
	items = malloc((n ? n : 1) * sizeof *items);
	pf = malloc((n ? n : 1) * sizeof *pf);
	if(items == NULL || pf == NULL){
		unlock(&lk);
		free(items);
		free(pf);
		errno = ENOMEM;
		return -1;
	}
	memcpy(items, e->item, n * sizeof *items);
	/* the earliest deadline of the set's timers: no longer than that */
	ns = now();
	for(i = 0; i < n; i++)
		if((t = findtimer(items[i].fd, 0)) != NULL && t->deadline != 0){
			if(t->deadline <= ns)
				fire(t, ns);
			else{
				wait = (t->deadline - ns + 999999) / 1000000;
				if(ms < 0 || wait < ms)
					ms = wait;
			}
		}
	unlock(&lk);
	for(i = 0; i < n; i++){
		pf[i].fd = items[i].fd;
		pf[i].events = items[i].events & (POLLIN|POLLPRI|POLLOUT|POLLRDNORM|POLLRDBAND|POLLWRNORM|POLLWRBAND);
		pf[i].revents = 0;
	}
	r = poll(pf, n, ms);
	if(r < 0){
		free(items);
		free(pf);
		return -1;
	}
	/* timers whose time came while waiting: readable now */
	lock(&lk);
	ns = now();
	for(i = 0; i < n; i++)
		if((t = findtimer(items[i].fd, 0)) != NULL && t->deadline != 0 && t->deadline <= ns){
			fire(t, ns);
			pf[i].revents |= POLLIN;
		}
	unlock(&lk);
	for(i = got = 0; i < n && got < max; i++){
		if(pf[i].revents == 0)
			continue;
		evs[got].events = pf[i].revents & (items[i].events | EPOLLERR | EPOLLHUP);
		if(evs[got].events == 0)
			continue;
		evs[got].data = items[i].data;
		got++;
		if(items[i].events & EPOLLONESHOT){
			struct epoll_event none;

			memset(&none, 0, sizeof none);
			none.data = items[i].data;
			epoll_ctl(epfd, EPOLL_CTL_MOD, items[i].fd, &none);
		}
	}
	free(items);
	free(pf);
	return got;
}

int
timerfd_create(int clock, int flags)
{
	Timer *t, *old;
	int fd, w;

	if(clock != CLOCK_MONOTONIC && clock != CLOCK_REALTIME){
		errno = EINVAL;
		return -1;
	}
	if((t = calloc(1, sizeof *t)) == NULL){
		errno = ENOMEM;
		return -1;
	}
	if((fd = mkpipe(&w, flags)) < 0){
		free(t);
		return -1;
	}
	t->fd = fd;
	t->w = w;
	lock(&lk);
	if((old = findtimer(fd, 1)) != NULL){	/* a closed one's */
		close(old->w);
		free(old);
	}
	t->next = timers;
	timers = t;
	unlock(&lk);
	return fd;
}

int
timerfd_settime(int fd, int flags, const struct itimerspec *new, struct itimerspec *old)
{
	Timer *t;
	long long ns, v;

	lock(&lk);
	if((t = findtimer(fd, 0)) == NULL){
		unlock(&lk);
		errno = EBADF;
		return -1;
	}
	ns = now();
	if(old != NULL){
		nsts(t->deadline ? t->deadline - ns : 0, &old->it_value);
		nsts(t->interval, &old->it_interval);
	}
	v = tsns(&new->it_value);
	t->interval = tsns(&new->it_interval);
	if(v == 0)
		t->deadline = 0;
	else if(flags & TFD_TIMER_ABSTIME)
		t->deadline = v > 0 ? v : 1;	/* CLOCK_MONOTONIC's time, as now() */
	else
		t->deadline = ns + v;
	unlock(&lk);
	return 0;
}

int
timerfd_gettime(int fd, struct itimerspec *cur)
{
	Timer *t;
	long long ns;

	lock(&lk);
	if((t = findtimer(fd, 0)) == NULL){
		unlock(&lk);
		errno = EBADF;
		return -1;
	}
	ns = now();
	nsts(t->deadline ? (t->deadline > ns ? t->deadline - ns : 1) : 0, &cur->it_value);
	nsts(t->interval, &cur->it_interval);
	unlock(&lk);
	return 0;
}

/*
 * an eventfd: the read end of a pipe, its other end kept as a timer's
 * (no deadline): eventfd_write writes there.  A plain write on the
 * descriptor does not come back to it (a Plan 9 pipe gives what one end
 * writes to the other).
 */
int
eventfd(unsigned int initval, int flags)
{
	Timer *t, *old;
	uint64_t v;
	int fd, w;

	if((t = calloc(1, sizeof *t)) == NULL){
		errno = ENOMEM;
		return -1;
	}
	if((fd = mkpipe(&w, flags)) < 0){
		free(t);
		return -1;
	}
	t->fd = fd;
	t->w = w;
	lock(&lk);
	if((old = findtimer(fd, 1)) != NULL){
		close(old->w);
		free(old);
	}
	t->next = timers;
	timers = t;
	unlock(&lk);
	if(initval != 0){
		v = initval;
		write(w, &v, sizeof v);
	}
	return fd;
}

int
eventfd_read(int fd, eventfd_t *v)
{
	return read(fd, v, sizeof *v) == sizeof *v ? 0 : -1;
}

int
eventfd_write(int fd, eventfd_t v)
{
	Timer *t;
	int w;

	lock(&lk);
	t = findtimer(fd, 0);
	w = t != NULL ? t->w : -1;
	unlock(&lk);
	if(w < 0){
		errno = EBADF;
		return -1;
	}
	return write(w, &v, sizeof v) == sizeof v ? 0 : -1;
}

int
signalfd(int fd, const sigset_t *mask, int flags)
{
	(void)fd;
	(void)mask;
	(void)flags;
	errno = ENOSYS;
	return -1;
}

int
ppoll(struct pollfd *fds, nfds_t n, const struct timespec *ts, const void *sigmask)
{
	(void)sigmask;
	return poll(fds, n, ts == NULL ? -1 : (int)((tsns(ts) + 999999) / 1000000));
}
