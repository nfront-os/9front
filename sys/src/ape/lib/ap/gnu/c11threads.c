/*
 * C11's threads (threads.h) on APE's POSIX threads (pthread.c). A
 * thread's start function returns an int: it travels as the pthread's
 * result, a pointer.
 */
#include <threads.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

typedef struct Start Start;
struct Start {
	thrd_start_t	f;
	void	*arg;
};

static int
result(int e)
{
	switch(e){
	case 0:
		return thrd_success;
	case ENOMEM:
		return thrd_nomem;
	case EBUSY:
		return thrd_busy;
	case ETIMEDOUT:
		return thrd_timedout;
	}
	return thrd_error;
}

static void*
start(void *a)
{
	Start s;

	s = *(Start*)a;
	free(a);
	return (void*)(intptr_t)s.f(s.arg);
}

int
thrd_create(thrd_t *t, thrd_start_t f, void *arg)
{
	Start *s;
	int e;

	if((s = malloc(sizeof *s)) == NULL)
		return thrd_nomem;
	s->f = f;
	s->arg = arg;
	if((e = pthread_create(t, NULL, start, s)) != 0){
		free(s);
		return result(e);
	}
	return thrd_success;
}

thrd_t
thrd_current(void)
{
	return pthread_self();
}

int
thrd_detach(thrd_t t)
{
	return result(pthread_detach(t));
}

int
thrd_equal(thrd_t a, thrd_t b)
{
	return pthread_equal(a, b);
}

_Noreturn void
thrd_exit(int r)
{
	pthread_exit((void*)(intptr_t)r);
}

int
thrd_join(thrd_t t, int *r)
{
	void *v;
	int e;

	if((e = pthread_join(t, &v)) != 0)
		return result(e);
	if(r != NULL)
		*r = (int)(intptr_t)v;
	return thrd_success;
}

/* 0 slept, -1 interrupted (rem: what was left), else another error */
int
thrd_sleep(const struct timespec *d, struct timespec *rem)
{
	if(nanosleep(d, rem) == 0)
		return 0;
	return errno == EINTR ? -1 : -2;
}

void
thrd_yield(void)
{
	sched_yield();
}

int
mtx_init(mtx_t *m, int type)
{
	pthread_mutexattr_t a;
	int e;

	pthread_mutexattr_init(&a);
	pthread_mutexattr_settype(&a, (type & mtx_recursive) ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL);
	e = pthread_mutex_init(m, &a);
	pthread_mutexattr_destroy(&a);
	return result(e);
}

void
mtx_destroy(mtx_t *m)
{
	pthread_mutex_destroy(m);
}

int
mtx_lock(mtx_t *m)
{
	return result(pthread_mutex_lock(m));
}

int
mtx_timedlock(mtx_t *m, const struct timespec *t)
{
	return result(pthread_mutex_timedlock(m, t));
}

int
mtx_trylock(mtx_t *m)
{
	return result(pthread_mutex_trylock(m));
}

int
mtx_unlock(mtx_t *m)
{
	return result(pthread_mutex_unlock(m));
}

int
cnd_init(cnd_t *c)
{
	return result(pthread_cond_init(c, NULL));
}

void
cnd_destroy(cnd_t *c)
{
	pthread_cond_destroy(c);
}

int
cnd_signal(cnd_t *c)
{
	return result(pthread_cond_signal(c));
}

int
cnd_broadcast(cnd_t *c)
{
	return result(pthread_cond_broadcast(c));
}

int
cnd_wait(cnd_t *c, mtx_t *m)
{
	return result(pthread_cond_wait(c, m));
}

/* t: TIME_UTC, as pthread's conditions count by default (CLOCK_REALTIME) */
int
cnd_timedwait(cnd_t *c, mtx_t *m, const struct timespec *t)
{
	return result(pthread_cond_timedwait(c, m, t));
}

int
tss_create(tss_t *k, tss_dtor_t d)
{
	return result(pthread_key_create(k, d));
}

void
tss_delete(tss_t k)
{
	pthread_key_delete(k);
}

void*
tss_get(tss_t k)
{
	return pthread_getspecific(k);
}

int
tss_set(tss_t k, void *v)
{
	return result(pthread_setspecific(k, v));
}

void
call_once(once_flag *f, void (*fn)(void))
{
	pthread_once(f, fn);
}
