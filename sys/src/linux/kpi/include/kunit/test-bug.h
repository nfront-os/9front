/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: kunit/test-bug.h (ours): no KUnit test runs */
#ifndef _KPI_KUNIT_TEST_BUG_H
#define _KPI_KUNIT_TEST_BUG_H
#include <linux/types.h>
struct kunit;
#define kunit_get_current_test()	((struct kunit *)NULL)
#define kunit_fail_current_test(fmt, ...)	do { } while (0)
#endif
