/*	$OpenBSD: time.h,v 1.6 2026/08/06 10:25:10 jsg Exp $	*/
/*
 * Copyright (c) 2013, 2014, 2015 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LINUX_TIME_H
#define _LINUX_TIME_H

#include <sys/time.h>

/* 9front: before the includes, which come back here (seqlock.h, ktime.h) */
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define NSEC_PER_SEC	1000000000L

#define USEC_PER_MSEC	1000L
#define USEC_PER_SEC	1000000L

#define MSEC_PER_SEC	1000L

struct timespec64 {
	time_t	tv_sec;
	long	tv_nsec;
};

#include <linux/math64.h>
#include <linux/seqlock.h>

/*
 * 9front: a time broken down (Linux's struct tm: year since 1900, month
 * 0-11) from seconds since 1970 plus offset, UTC, Gregorian calendar
 */
struct tm {
	int	tm_sec, tm_min, tm_hour, tm_mday, tm_mon;
	long	tm_year;
	int	tm_wday, tm_yday;
};

static inline void
time64_to_tm(time64_t t, int offset, struct tm *r)
{
	long long days, rem, y, era, doe, yoe, doy, mp;

	t += offset;
	days = t / 86400;
	rem = t % 86400;
	if (rem < 0) {
		rem += 86400;
		days--;
	}
	r->tm_hour = rem / 3600;
	r->tm_min = rem % 3600 / 60;
	r->tm_sec = rem % 60;
	r->tm_wday = (int)(((days % 7) + 11) % 7);	/* 1970-01-01 was a Thursday */
	/* the date: eras of 400 years from 0000-03-01 */
	days += 719468;
	era = (days >= 0 ? days : days - 146096) / 146097;
	doe = days - era * 146097;
	yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	y = yoe + era * 400;
	doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	mp = (5 * doy + 2) / 153;
	r->tm_mday = (int)(doy - (153 * mp + 2) / 5 + 1);
	r->tm_mon = (int)(mp < 10 ? mp + 2 : mp - 10);
	if (r->tm_mon <= 1)
		y++;
	r->tm_year = y - 1900;
	{
		static const short before[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
		int leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;

		r->tm_yday = before[r->tm_mon] + r->tm_mday - 1 + (leap && r->tm_mon > 1);
	}
}

#endif
