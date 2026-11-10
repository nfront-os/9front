/*
 * APE (GCC port): POSIX's unnamed semaphores (libap, ap/gnu/semaphore.c)
 * on 9front's semacquire(2): a count the kernel decrements, waiting while
 * it is 0, and increments. Shared between the procs of a program (threads,
 * rfork RFMEM) and, in memory they share, between programs; no named ones
 * (sem_open: ENOSYS).
 */
#ifndef __APE_SEMAPHORE_H
#define __APE_SEMAPHORE_H
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int	__count;	/* the kernel's s32int */
} sem_t;

#define SEM_FAILED	((sem_t *)0)
#define SEM_VALUE_MAX	0x7fffffff

extern int	sem_init(sem_t *, int, unsigned);
extern int	sem_destroy(sem_t *);
extern int	sem_wait(sem_t *);
extern int	sem_trywait(sem_t *);
extern int	sem_timedwait(sem_t *, const struct timespec *);
extern int	sem_post(sem_t *);
extern int	sem_getvalue(sem_t *, int *);
extern sem_t	*sem_open(const char *, int, ...);
extern int	sem_close(sem_t *);
extern int	sem_unlink(const char *);

#ifdef __cplusplus
}
#endif
#endif
