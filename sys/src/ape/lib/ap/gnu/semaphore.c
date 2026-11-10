/*
 * POSIX's unnamed semaphores on 9front's semacquire(2), which is what a
 * POSIX semaphore is: sem_wait takes one, waiting while there is none
 * (a note interrupts it: EINTR), sem_post gives one back. sem_timedwait's
 * time is CLOCK_REALTIME's (POSIX), tsemacquire waits by milliseconds.
 */
#include <semaphore.h>
#include <errno.h>
#include <time.h>

extern int _SEMACQUIRE(int *, int);
extern int _TSEMACQUIRE(int *, unsigned long);
extern int _SEMRELEASE(int *, int);

int
sem_init(sem_t *s, int pshared, unsigned v)
{
	(void)pshared;
	if(v > SEM_VALUE_MAX){
		errno = EINVAL;
		return -1;
	}
	s->__count = v;
	return 0;
}

int
sem_destroy(sem_t *s)
{
	(void)s;
	return 0;
}

int
sem_wait(sem_t *s)
{
	if(_SEMACQUIRE(&s->__count, 1) < 0){
		errno = EINTR;
		return -1;
	}
	return 0;
}

int
sem_trywait(sem_t *s)
{
	if(_SEMACQUIRE(&s->__count, 0) <= 0){
		errno = EAGAIN;
		return -1;
	}
	return 0;
}

int
sem_timedwait(sem_t *s, const struct timespec *t)
{
	struct timespec now;
	long long ms;
	int r;

	if(t->tv_nsec < 0 || t->tv_nsec >= 1000000000){
		errno = EINVAL;
		return -1;
	}
	if(_SEMACQUIRE(&s->__count, 0) > 0)
		return 0;
	clock_gettime(CLOCK_REALTIME, &now);
	ms = (t->tv_sec - now.tv_sec) * 1000LL + (t->tv_nsec - now.tv_nsec) / 1000000;
	if(ms <= 0){
		errno = ETIMEDOUT;
		return -1;
	}
	r = _TSEMACQUIRE(&s->__count, ms);
	if(r > 0)
		return 0;
	errno = r == 0 ? ETIMEDOUT : EINTR;
	return -1;
}

int
sem_post(sem_t *s)
{
	if(s->__count == SEM_VALUE_MAX){
		errno = EOVERFLOW;
		return -1;
	}
	_SEMRELEASE(&s->__count, 1);
	return 0;
}

int
sem_getvalue(sem_t *s, int *v)
{
	*v = s->__count < 0 ? 0 : s->__count;
	return 0;
}

sem_t*
sem_open(const char *name, int flags, ...)
{
	(void)name;
	(void)flags;
	errno = ENOSYS;
	return SEM_FAILED;
}

int
sem_close(sem_t *s)
{
	(void)s;
	errno = ENOSYS;
	return -1;
}

int
sem_unlink(const char *name)
{
	(void)name;
	errno = ENOSYS;
	return -1;
}
