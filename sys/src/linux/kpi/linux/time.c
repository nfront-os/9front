// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: time.  Plan 9's nsec() for both the monotonic
 * and the real clock; jiffies are milliseconds (HZ 1000).
 */
#include <linux/ktime.h>
#include <linux/jiffies.h>
#include <linux/utsname.h>
#include "kpi.h"

u64 ktime_get_mono_fast_ns(void) { return p9_nsec(); }
time64_t ktime_get_seconds(void) { return p9_nsec() / NSEC_PER_SEC; }
void ktime_get_ts64(struct timespec64 *ts) { *ts = ktime_to_timespec64(ktime_get()); }
void ktime_get_real_ts64(struct timespec64 *ts) { *ts = ktime_to_timespec64(ktime_get_real()); }

static struct new_utsname uts;

struct new_utsname *
init_utsname(void)
{
	if (!uts.sysname[0]) {
		strscpy(uts.sysname, "Plan 9", sizeof(uts.sysname));
		strscpy(uts.nodename, p9_sysname(), sizeof(uts.nodename));
		strscpy(uts.release, "9front", sizeof(uts.release));
		strscpy(uts.version, "linux-6.6 drm on the LinuxKPI for 9front", sizeof(uts.version));
		strscpy(uts.machine, "amd64", sizeof(uts.machine));
	}
	return &uts;
}
