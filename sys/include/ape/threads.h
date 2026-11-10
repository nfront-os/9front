/*
 * APE (GCC port): C11's threads (libap, ap/gnu/c11threads.c) on APE's
 * POSIX threads: a thread is a proc (pthread.h), mutexes and conditions
 * are pthread's.
 */
#ifndef __APE_THREADS_H
#define __APE_THREADS_H
#include <pthread.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_t	thrd_t;
typedef pthread_mutex_t	mtx_t;
typedef pthread_cond_t	cnd_t;
typedef pthread_key_t	tss_t;
typedef pthread_once_t	once_flag;
typedef int	(*thrd_start_t)(void *);
typedef void	(*tss_dtor_t)(void *);

#define ONCE_FLAG_INIT	PTHREAD_ONCE_INIT
#define TSS_DTOR_ITERATIONS	PTHREAD_DESTRUCTOR_ITERATIONS
#ifndef __cplusplus
#define thread_local	_Thread_local
#endif

enum {
	thrd_success = 0,
	thrd_busy = 1,
	thrd_error = 2,
	thrd_nomem = 3,
	thrd_timedout = 4,
};

enum {
	mtx_plain = 0,
	mtx_recursive = 1,
	mtx_timed = 2,
};

extern int	thrd_create(thrd_t *, thrd_start_t, void *);
extern thrd_t	thrd_current(void);
extern int	thrd_detach(thrd_t);
extern int	thrd_equal(thrd_t, thrd_t);
extern _Noreturn void	thrd_exit(int);
extern int	thrd_join(thrd_t, int *);
extern int	thrd_sleep(const struct timespec *, struct timespec *);
extern void	thrd_yield(void);

extern int	mtx_init(mtx_t *, int);
extern void	mtx_destroy(mtx_t *);
extern int	mtx_lock(mtx_t *);
extern int	mtx_timedlock(mtx_t *, const struct timespec *);
extern int	mtx_trylock(mtx_t *);
extern int	mtx_unlock(mtx_t *);

extern int	cnd_init(cnd_t *);
extern void	cnd_destroy(cnd_t *);
extern int	cnd_signal(cnd_t *);
extern int	cnd_broadcast(cnd_t *);
extern int	cnd_wait(cnd_t *, mtx_t *);
extern int	cnd_timedwait(cnd_t *, mtx_t *, const struct timespec *);

extern int	tss_create(tss_t *, tss_dtor_t);
extern void	tss_delete(tss_t);
extern void	*tss_get(tss_t);
extern int	tss_set(tss_t, void *);

extern void	call_once(once_flag *, void (*)(void));

#ifdef __cplusplus
}
#endif
#endif
