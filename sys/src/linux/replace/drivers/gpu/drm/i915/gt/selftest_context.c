// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: in place of Linux's selftest_context.c (GPL;
 * included by intel_context.c): the live selftests of the contexts are
 * not ported (they run on hardware only), so they fail.
 */
int intel_context_live_selftests(struct drm_i915_private *i915)
{
	/* not ported: a failure, not a pass */
	pr_err("i915: context live selftests not ported (Linux's are GPL)\n");
	return -EOPNOTSUPP;
}
