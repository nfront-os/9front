// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: in place of Linux's selftest_engine_pm.c (GPL;
 * included by intel_engine_pm.c): the live selftests of the engines'
 * power management (busy statistics, timestamps, parking) are not
 * ported (they run on hardware only), so they fail.
 */
#include "selftest_engine.h"

int live_engine_pm_selftests(struct intel_gt *gt)
{
	/* not ported: a failure, not a pass */
	pr_err("i915: engine_pm live selftests not ported (Linux's are GPL)\n");
	return -EOPNOTSUPP;
}
