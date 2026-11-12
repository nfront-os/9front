// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: the ring's mock selftest, our own (Linux's
 * selftest_ring.c is GPL; included by intel_ring.c).  intel_ring_direction
 * tells whether an offset lies after another in the ring, modulo its
 * size: it must for anything less than half the ring ahead, and the
 * other way round behind, from any place in the ring, across its end.
 */

static struct intel_ring *
test_ring(u32 size)
{
	struct intel_ring *ring = kzalloc(sizeof(*ring), GFP_KERNEL);

	if (!ring)
		return NULL;
	ring->size = size;
	ring->wrap = BITS_PER_TYPE(ring->size) - ilog2(size);
	ring->effective_size = size;
	return ring;
}

static int
want_direction(struct intel_ring *ring, u32 next, u32 prev, int want)
{
	int d = intel_ring_direction(ring, next, prev);
	int got = d > 0 ? 1 : d < 0 ? -1 : 0;

	if (got == want)
		return 0;
	pr_err("ring of %u bytes: direction of %#x from %#x is %d, want %d\n",
	       ring->size, next, prev, got, want);
	return -EINVAL;
}

static int
igt_ring_direction(void *arg)
{
	static const u32 steps[] = { 8, 64, 4096 };
	struct intel_ring *ring;
	u32 size, half, at, d, mask;
	unsigned int i;
	int err = 0;

	for (size = SZ_4K; size <= SZ_1M; size <<= 2) {
		ring = test_ring(size);
		if (!ring)
			return -ENOMEM;
		half = size / 2;
		mask = size - 1;
		for (at = 0; at < size && !err; at += size / 8 + 8) {
			at &= mask & ~7;
			err |= want_direction(ring, at, at, 0);
			for (i = 0; i < ARRAY_SIZE(steps) && !err; i++) {
				d = steps[i];
				if (d >= half)
					continue;
				/* ahead, behind, and the same across the ring's end */
				err |= want_direction(ring, (at + d) & mask, at, 1);
				err |= want_direction(ring, at, (at + d) & mask, -1);
				err |= want_direction(ring, (at + half - 8) & mask, at, 1);
				err |= want_direction(ring, at, (at + half - 8) & mask, -1);
			}
		}
		kfree(ring);
		if (err)
			return err;
	}
	return 0;
}

int intel_ring_mock_selftests(void)
{
	static const struct i915_subtest tests[] = {
		SUBTEST(igt_ring_direction),
	};

	return i915_subtests(tests, NULL);
}
