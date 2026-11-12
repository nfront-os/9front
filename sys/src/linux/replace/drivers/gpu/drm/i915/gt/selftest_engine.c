// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the engines' live selftests, our own list (Linux's
 * selftest_engine.c is GPL; included by intel_engine_cs.c): those of the
 * engines' power management.
 */
#include "i915_selftest.h"
#include "selftest_engine.h"

int intel_engine_live_selftests(struct drm_i915_private *i915)
{
	return live_engine_pm_selftests(to_gt(i915));
}
