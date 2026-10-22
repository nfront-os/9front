/* SPDX-License-Identifier: MIT */
/* barriers and pthread_getcpuclockid, which the toolchain's pthreads lack */
#include_next <pthread.h>

#ifndef _MESA_PLAN9_PTHREAD_H
#define _MESA_PLAN9_PTHREAD_H
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	pthread_mutex_t	m;
	pthread_cond_t	c;
	unsigned	count;
	unsigned	waiting;
	unsigned	generation;
} pthread_barrier_t;
typedef int pthread_barrierattr_t;
#define PTHREAD_BARRIER_SERIAL_THREAD	(-1)

extern int pthread_barrier_init(pthread_barrier_t *, const pthread_barrierattr_t *, unsigned);
extern int pthread_barrier_destroy(pthread_barrier_t *);
extern int pthread_barrier_wait(pthread_barrier_t *);
extern int pthread_getcpuclockid(pthread_t, clockid_t *);

#ifdef __cplusplus
}
#endif
#endif
