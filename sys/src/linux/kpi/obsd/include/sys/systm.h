/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/systm.h */
#ifndef _OBSD_SYS_SYSTM_H
#define _OBSD_SYS_SYSTM_H

#include <sys/types.h>
#include <sys/stdarg.h>
#include <sys/param.h>
#include <lib/libkern/libkern.h>
#include <sys/proc.h>

extern int	cold;		/* 0: the KPI is up */
extern const int	hz;	/* ticks per second (of jiffies): 1000 */
extern const int	tick;	/* microseconds per tick */
extern int	ncpus, ncpusonline;
extern int	nowake;		/* a channel nobody wakes: sleep for the time only */
/* the uptime in ticks, from the clock (no tick interrupt here) */
unsigned long	kpi_jiffies(void);
#define ticks	((int)kpi_jiffies())

int	printf(const char *, ...) __attribute__((__format__(__printf__, 1, 2)));
int	snprintf(char *, size_t, const char *, ...) __attribute__((__format__(__printf__, 3, 4)));
int	vsnprintf(char *, size_t, const char *, va_list);
int	vprintf(const char *, va_list);
__attribute__((__noreturn__)) void	panic(const char *, ...) __attribute__((__format__(__printf__, 1, 2)));
void	kpi_assert(const char *, const char *, int, const char *) __attribute__((__noreturn__));

#ifdef DIAGNOSTIC
#define KASSERT(e)	((e) ? (void)0 : kpi_assert("diagnostic ", __FILE__, __LINE__, #e))
#else
#define KASSERT(e)	((void)0)
#endif
#define KASSERTMSG(e, msg, ...)	KASSERT(e)
#define CTASSERT(x)	_Static_assert(x, "CTASSERT")

/* the file server's address space is one: copyin/copyout copy */
int	copyin(const void *, void *, size_t);
int	copyout(const void *, void *, size_t);
int	kcopy(const void *, void *, size_t);

void	delay(int);		/* microseconds, busy */
#define DELAY(n)	delay(n)

/* randomness (#c/random) */
uint32_t	arc4random(void);
uint32_t	arc4random_uniform(uint32_t);
void	arc4random_buf(void *, size_t);

/* the interrupt level: there are no interrupts in a file server; the IRQ proc is a proc */
#define splhigh()	0
#define splx(s)		((void)(s))
#define spltty()	0
#define splvm()		0
#define splsched()	0
#define splassert(x)	((void)0)

void	db_enter(void);
void	assertwaitok(void);	/* might_sleep: not in an atomic section */

#endif
