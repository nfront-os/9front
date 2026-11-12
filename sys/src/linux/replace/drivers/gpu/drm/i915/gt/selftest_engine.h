/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: our own (Linux's selftest_engine.h is GPL) */
#ifndef SELFTEST_ENGINE_H
#define SELFTEST_ENGINE_H

struct intel_gt;

int	live_engine_pm_selftests(struct intel_gt *gt);

#endif
