/* Public domain. */
/*
 * 9front: polling until a condition holds or a time is up, as Linux 6.18
 * has it: poll_timeout_us runs op (any statement) and tests cond, sleeping
 * sleep_us between (usleep_range; the _atomic forms udelay), until cond
 * holds (0) or timeout_us is over (-ETIMEDOUT, op run once more first);
 * read_poll_timeout and readx_poll_timeout on it.  (OpenBSD's header,
 * rebuilt on our clock: ktime_get_ns.)
 */

#ifndef _LINUX_IOPOLL_H
#define _LINUX_IOPOLL_H

#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>

#define __kpi_poll_timeout(op, cond, sleep_us, timeout_us, sleep_before, wait)	\
({									\
	u64 __end = ktime_get_ns() + (u64)(timeout_us) * 1000;	\
	unsigned long __sleep = (sleep_us);				\
	int __ret = 0;							\
									\
	if ((sleep_before) && __sleep)					\
		wait(__sleep);						\
	for (;;) {							\
		op;							\
		if (cond)						\
			break;						\
		if ((timeout_us) && ktime_get_ns() > __end) {		\
			op;						\
			if (!(cond))					\
				__ret = -ETIMEDOUT;			\
			break;						\
		}							\
		if (__sleep)						\
			wait(__sleep);					\
		else							\
			cpu_relax();					\
	}								\
	__ret;								\
})
#define __kpi_poll_usleep(us)	usleep_range((us), (us))

#define poll_timeout_us(op, cond, sleep_us, timeout_us, sleep_before)	\
	__kpi_poll_timeout(op, cond, sleep_us, timeout_us, sleep_before, __kpi_poll_usleep)
#define poll_timeout_us_atomic(op, cond, delay_us, timeout_us, delay_before)	\
	__kpi_poll_timeout(op, cond, delay_us, timeout_us, delay_before, udelay)

#define read_poll_timeout(op, val, cond, sleep_us, timeout_us, sleep_before, args...) \
	poll_timeout_us((val) = op(args), cond, sleep_us, timeout_us, sleep_before)
#define read_poll_timeout_atomic(op, val, cond, delay_us, timeout_us, delay_before, args...) \
	poll_timeout_us_atomic((val) = op(args), cond, delay_us, timeout_us, delay_before)

/* 9front: rd(a) into v until c; the waits as above, never before the first read */
#define readx_poll_timeout(rd, a, v, c, us, limit)	\
	read_poll_timeout(rd, v, c, us, limit, false, a)
#define readx_poll_timeout_atomic(rd, a, v, c, us, limit)	\
	read_poll_timeout_atomic(rd, v, c, us, limit, false, a)
#define readl_poll_timeout(a, v, c, us, limit)	\
	readx_poll_timeout(readl, a, v, c, us, limit)
#define readl_poll_timeout_atomic(a, v, c, us, limit)	\
	readx_poll_timeout_atomic(readl, a, v, c, us, limit)

#endif
