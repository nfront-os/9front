/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: i915's performance selftests, our own list
 * (Linux's is GPL): selftest(name, function), run by
 * i915_perf_selftests(), by name.
 */
#ifndef selftest
#define selftest(x, y)
#endif
selftest(engine_cs, intel_engine_cs_perf_selftests)
selftest(migrate, intel_migrate_perf_selftests)
selftest(region, intel_memory_region_perf_selftests)
selftest(request, i915_request_perf_selftests)
