// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the engines' selftests, our own (Linux's
 * selftest_engine_cs.c is GPL; included by intel_engine_cs.c).
 *
 * Mock: the table of the engines' register bases, which
 * intel_engine_setup searches for the first entry not newer than the
 * device: each engine has one, the bases of an engine are sorted from
 * the newest graphics version down, none is 0.
 *
 * Perf (the command streamers' timings): not ported, so it fails.
 */

static int
intel_mmio_bases_check(void *arg)
{
	const struct engine_info *info;
	unsigned int i, j;
	u8 prev;

	for (i = 0; i != ARRAY_SIZE(intel_engines); i++) {
		info = &intel_engines[i];
		if (!info->mmio_bases[0].graphics_ver)
			continue;	/* no engine of that id */
		prev = U8_MAX;
		for (j = 0; j < MAX_MMIO_BASES && info->mmio_bases[j].graphics_ver; j++) {
			if (info->mmio_bases[j].graphics_ver >= prev) {
				pr_err("engine %u (class %u instance %u): mmio base %u (graphics %u) not below %u\n",
				       i, info->class, info->instance, j, info->mmio_bases[j].graphics_ver, prev);
				return -EINVAL;
			}
			if (!info->mmio_bases[j].base) {
				pr_err("engine %u: mmio base %u (graphics %u) is 0\n",
				       i, j, info->mmio_bases[j].graphics_ver);
				return -EINVAL;
			}
			prev = info->mmio_bases[j].graphics_ver;
		}
	}
	return 0;
}

int intel_engine_cs_mock_selftests(void)
{
	static const struct i915_subtest tests[] = {
		SUBTEST(intel_mmio_bases_check),
	};

	return i915_subtests(tests, NULL);
}

int intel_engine_cs_perf_selftests(struct drm_i915_private *i915)
{
	/* not ported: a failure, not a pass */
	pr_err("i915: engine_cs perf selftests not ported (Linux's are GPL)\n");
	return -EOPNOTSUPP;
}
