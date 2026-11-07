/*
 * clock_gettime, clock_getres (POSIX), timespec_get (C11), usleep,
 * sched_yield, which APE lacks.
 *
 * The clocks are 9front's /dev/bintime (cons(3), port/tod.c): big-endian
 * nanoseconds since the epoch, fastticks, their frequency, and the
 * nanoseconds since boot, which never go back (CLOCK_MONOTONIC; the
 * epoch time moves when the time is set; never slewed, so it is
 * CLOCK_MONOTONIC_RAW as well). The file stays open, shared by
 * all procs of the program; pread, so they need no lock.
 */
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

static int bintimefd = -1;

static int
openbintime(void)
{
	int fd, old;

	fd = open("/dev/bintime", O_RDONLY);
	if(fd < 0)
		return -1;
	fcntl(fd, F_SETFD, FD_CLOEXEC);
	old = -1;
	if(!__atomic_compare_exchange_n(&bintimefd, &old, fd, 0,
	    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)){
		close(fd);	/* another proc was first */
		return old;
	}
	return fd;
}

/* v[0] epoch ns, v[1] ticks, v[2] ticks per second, v[3] ns since boot */
static int
readbintime(uint64_t v[4])
{
	unsigned char b[32];
	int fd, i, j, try;

	for(try = 0; try < 2; try++){
		fd = __atomic_load_n(&bintimefd, __ATOMIC_SEQ_CST);
		if(fd < 0 && (fd = openbintime()) < 0)
			return -1;
		if(pread(fd, b, sizeof b, 0) == sizeof b){
			for(i = 0; i < 4; i++){
				v[i] = 0;
				for(j = 0; j < 8; j++)
					v[i] = v[i] << 8 | b[8*i + j];
			}
			return 0;
		}
		/* the program closed it (daemons close all): once more */
		__atomic_compare_exchange_n(&bintimefd, &fd, -1, 0,
			__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	}
	return -1;
}

static void
nstots(uint64_t ns, struct timespec *ts)
{
	ts->tv_sec = ns / 1000000000;
	ts->tv_nsec = ns % 1000000000;
}

int
clock_gettime(clockid_t id, struct timespec *ts)
{
	uint64_t v[4];
	struct tms t;
	long hz;

	switch(id){
	case CLOCK_REALTIME:
	case CLOCK_MONOTONIC:
	case CLOCK_MONOTONIC_RAW:
		if(readbintime(v) < 0)
			return -1;
		nstots(id == CLOCK_REALTIME ? v[0] : v[3], ts);
		return 0;
	case CLOCK_PROCESS_CPUTIME_ID:
	case CLOCK_THREAD_CPUTIME_ID:
		/* this proc's (a thread is a proc) user and system time */
		times(&t);
		hz = sysconf(_SC_CLK_TCK);
		nstots((uint64_t)(t.tms_utime + t.tms_stime) * (1000000000 / hz), ts);
		return 0;
	}
	errno = EINVAL;
	return -1;
}

int
clock_getres(clockid_t id, struct timespec *ts)
{
	uint64_t v[4];

	switch(id){
	case CLOCK_REALTIME:
	case CLOCK_MONOTONIC:
	case CLOCK_MONOTONIC_RAW:
		if(ts == NULL)
			return 0;
		/* a tick of the counter they come from, at least 1ns */
		if(readbintime(v) < 0)
			return -1;
		nstots(v[2] >= 1000000000 || v[2] == 0 ? 1 : (1000000000 + v[2] - 1) / v[2], ts);
		return 0;
	case CLOCK_PROCESS_CPUTIME_ID:
	case CLOCK_THREAD_CPUTIME_ID:
		if(ts != NULL)
			nstots(1000000000 / sysconf(_SC_CLK_TCK), ts);
		return 0;
	}
	errno = EINVAL;
	return -1;
}

int
timespec_get(struct timespec *ts, int base)
{
	if(base != TIME_UTC || clock_gettime(CLOCK_REALTIME, ts) < 0)
		return 0;
	return base;
}

int
usleep(useconds_t us)
{
	struct timespec ts;

	ts.tv_sec = us / 1000000;
	ts.tv_nsec = us % 1000000 * 1000;
	return nanosleep(&ts, NULL);
}

/* sleep(0): the kernel reschedules */
int
sched_yield(void)
{
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 0;
	nanosleep(&ts, NULL);
	return 0;
}
