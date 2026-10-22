/* SPDX-License-Identifier: MIT */
/* pthread_sigmask and SIGSYS for Mesa's u_thread.c */
#include_next <signal.h>

#ifndef _MESA_PLAN9_SIGNAL_H
#define _MESA_PLAN9_SIGNAL_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIGSYS
#define SIGSYS	SIGILL	/* there is no bad system call note to block */
#endif
extern int pthread_sigmask(int, const sigset_t *, sigset_t *);

#ifdef __cplusplus
}
#endif
#endif
