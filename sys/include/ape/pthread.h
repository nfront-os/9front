/*
 * APE (GCC port): POSIX threads (libap,
 * ap/gnu/pthread.c): a thread is a proc, rfork(RFPROC|RFMEM), on a
 * stack from malloc; mutexes, conditions and the rest wait on 9front's
 * semaphores (semacquire, tsemacquire). Not there: cancellation
 * (pthread_cancel says ENOTSUP), process-shared objects, scheduling
 * policies.
 *
 * The objects are plain structures, so they need no destroy and can be
 * initialized statically. A semaphore is 32 bits (the kernel's s32int).
 * Barriers are a mutex, a condition and a count.
 */
#ifndef __APE_PTHREAD
#define __APE_PTHREAD

#include <sched.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __pthread *pthread_t;

typedef struct {
	int	__detach;
	size_t	__stacksize;
} pthread_attr_t;

typedef struct {
	int	__val;		/* 0 free, 1 locked, 2 locked, maybe waiters */
	int	__sem;
	int	__type;
	int	__count;	/* recursive: times locked */
	pthread_t	__owner;
} pthread_mutex_t;

typedef struct {
	int	__type;
} pthread_mutexattr_t;

typedef struct {
	int	__lock;
	struct __pthread	*__head, *__tail;	/* the waiters, in order */
	int	__clock;
} pthread_cond_t;

typedef struct {
	int	__clock;
} pthread_condattr_t;

typedef struct {
	pthread_mutex_t	__m;
	pthread_cond_t	__readers, __writers;
	int	__nreaders;	/* holding it */
	int	__writer;	/* holds it */
	int	__wwait;	/* writers waiting */
} pthread_rwlock_t;

typedef struct {
	int	__dummy;
} pthread_rwlockattr_t;

typedef struct {
	pthread_mutex_t	__m;
	pthread_cond_t	__c;
	unsigned	__count;	/* threads to wait for */
	unsigned	__waiting;
	unsigned	__generation;	/* rounds completed */
} pthread_barrier_t;

typedef struct {
	int	__dummy;
} pthread_barrierattr_t;

typedef int pthread_once_t;
typedef unsigned int pthread_key_t;

enum {
	PTHREAD_CREATE_JOINABLE = 0,
	PTHREAD_CREATE_DETACHED = 1,
};
enum {
	PTHREAD_MUTEX_NORMAL = 0,
	PTHREAD_MUTEX_RECURSIVE = 1,
	PTHREAD_MUTEX_ERRORCHECK = 2,
	PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL,
};

#define PTHREAD_MUTEX_INITIALIZER	{ 0, 0, PTHREAD_MUTEX_NORMAL, 0, 0 }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP	{ 0, 0, PTHREAD_MUTEX_RECURSIVE, 0, 0 }
#define PTHREAD_COND_INITIALIZER	{ 0, 0, 0, CLOCK_REALTIME }
#define PTHREAD_RWLOCK_INITIALIZER	{ PTHREAD_MUTEX_INITIALIZER, \
	PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0 }
#define PTHREAD_ONCE_INIT	0
#define PTHREAD_BARRIER_SERIAL_THREAD	(-1)

#define PTHREAD_KEYS_MAX	128
#define PTHREAD_DESTRUCTOR_ITERATIONS	4
#define PTHREAD_STACK_MIN	16384
#define PTHREAD_THREADS_MAX	1024

extern int	pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
extern int	pthread_join(pthread_t, void **);
extern int	pthread_detach(pthread_t);
extern _Noreturn void	pthread_exit(void *);
extern pthread_t	pthread_self(void);
extern int	pthread_equal(pthread_t, pthread_t);
extern int	pthread_cancel(pthread_t);
extern int	pthread_once(pthread_once_t *, void (*)(void));

extern int	pthread_attr_init(pthread_attr_t *);
extern int	pthread_attr_destroy(pthread_attr_t *);
extern int	pthread_attr_setdetachstate(pthread_attr_t *, int);
extern int	pthread_attr_getdetachstate(const pthread_attr_t *, int *);
extern int	pthread_attr_setstacksize(pthread_attr_t *, size_t);
extern int	pthread_attr_getstacksize(const pthread_attr_t *, size_t *);

extern int	pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
extern int	pthread_mutex_destroy(pthread_mutex_t *);
extern int	pthread_mutex_lock(pthread_mutex_t *);
extern int	pthread_mutex_trylock(pthread_mutex_t *);
extern int	pthread_mutex_timedlock(pthread_mutex_t *, const struct timespec *);
extern int	pthread_mutex_unlock(pthread_mutex_t *);
extern int	pthread_mutexattr_init(pthread_mutexattr_t *);
extern int	pthread_mutexattr_destroy(pthread_mutexattr_t *);
extern int	pthread_mutexattr_settype(pthread_mutexattr_t *, int);
extern int	pthread_mutexattr_gettype(const pthread_mutexattr_t *, int *);

extern int	pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
extern int	pthread_cond_destroy(pthread_cond_t *);
extern int	pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
extern int	pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
extern int	pthread_cond_signal(pthread_cond_t *);
extern int	pthread_cond_broadcast(pthread_cond_t *);
extern int	pthread_condattr_init(pthread_condattr_t *);
extern int	pthread_condattr_destroy(pthread_condattr_t *);
extern int	pthread_condattr_setclock(pthread_condattr_t *, clockid_t);
extern int	pthread_condattr_getclock(const pthread_condattr_t *, clockid_t *);

extern int	pthread_rwlock_init(pthread_rwlock_t *, const pthread_rwlockattr_t *);
extern int	pthread_rwlock_destroy(pthread_rwlock_t *);
extern int	pthread_rwlock_rdlock(pthread_rwlock_t *);
extern int	pthread_rwlock_tryrdlock(pthread_rwlock_t *);
extern int	pthread_rwlock_timedrdlock(pthread_rwlock_t *, const struct timespec *);
extern int	pthread_rwlock_wrlock(pthread_rwlock_t *);
extern int	pthread_rwlock_trywrlock(pthread_rwlock_t *);
extern int	pthread_rwlock_timedwrlock(pthread_rwlock_t *, const struct timespec *);
extern int	pthread_rwlock_unlock(pthread_rwlock_t *);

extern int	pthread_barrier_init(pthread_barrier_t *, const pthread_barrierattr_t *, unsigned);
extern int	pthread_barrier_destroy(pthread_barrier_t *);
extern int	pthread_barrier_wait(pthread_barrier_t *);
extern int	pthread_barrierattr_init(pthread_barrierattr_t *);
extern int	pthread_barrierattr_destroy(pthread_barrierattr_t *);

/* signal masks are the proc's (APE's sigprocmask); no CPU-time clocks */
extern int	pthread_sigmask(int, const sigset_t *, sigset_t *);
extern int	pthread_getcpuclockid(pthread_t, clockid_t *);

extern int	pthread_key_create(pthread_key_t *, void (*)(void *));
extern int	pthread_key_delete(pthread_key_t);
extern void	*pthread_getspecific(pthread_key_t);
extern int	pthread_setspecific(pthread_key_t, const void *);

#ifdef __cplusplus
}
#endif
#endif
