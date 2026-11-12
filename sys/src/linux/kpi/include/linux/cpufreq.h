/* Public domain. */

#ifndef _LINUX_CPUFREQ_H
#define _LINUX_CPUFREQ_H

#include <linux/kobject.h>

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/pm.h>
struct cpufreq_cpuinfo { unsigned int max_freq; unsigned int min_freq; unsigned int transition_latency; };
struct cpufreq_policy { unsigned int min, max, cur; struct cpufreq_cpuinfo cpuinfo; };
#define cpufreq_cpu_get(c)		((struct cpufreq_policy *)NULL)
#define cpufreq_cpu_put(p)		do { } while (0)

#endif
