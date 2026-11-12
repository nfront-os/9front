/* Public domain. */

#ifndef _LINUX_PERF_EVENT_H
#define _LINUX_PERF_EVENT_H

#include <linux/ftrace.h>
#include <linux/file.h> /* via security.h -> kernel_read_file.h */
#include <linux/seq_file.h> /* via linux/cgroup.h */

struct pmu {
};

/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/sysfs.h>
struct perf_event { int unused; };
struct perf_event_attr { u32 type; u32 size; u64 config; };

#endif
