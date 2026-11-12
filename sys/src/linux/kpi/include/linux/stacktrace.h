/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: linux/stacktrace.h (ours, not Linux's) */
#ifndef _KPI_LINUX_STACKTRACE_H
#define _KPI_LINUX_STACKTRACE_H

#include <linux/kernel.h>

static inline unsigned int stack_trace_save(unsigned long *store, unsigned int size, unsigned int skip) { return 0; }
static inline void stack_trace_print(const unsigned long *e, unsigned int n, int spaces) { }
static inline int stack_trace_snprint(char *buf, size_t size, const unsigned long *e, unsigned int n, int spaces) { if (size) buf[0] = 0; return 0; }

#endif
